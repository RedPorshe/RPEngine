#include "Engine.h"
#include "Log/Log.h"
#include "GameInstance.h"
#include "Window/IWindowManager.h"
#include "Window/IWindow.h"
#include <fstream>

#include "Render/RHI/Renderer.h"

#include "Event/InputEvent.h"
#include "Input/InputManager.h"

#ifdef RPE_USE_NATIVE_WINDOW
#ifdef _WIN32
#include "Window/WinAPI/Win32WindowManager.h"
#include "Window/WinAPI/Win32Window.h"
#elif defined(__linux__)
#include "Window/Linux/LinuxWindowManager.h"
#include "Window/Linux/LinuxWindow.h"
#elif defined(__APPLE__)
#include "Window/MacOS/MacOSWindowManager.h"
#include "Window/MacOS/MacOSWindow.h"
#endif  // WIN32
#else
#include "Window/GLFW/GLFWWindowManager.h"
#include "Window/GLFW/GLFWWindow.h"
#endif  // RPE_USE_NATIVE_WINDOW

#include <format>
#include <thread>

#include "Core/SystemObject.h"
#include "Core/ObjectFactory.h"

#include "../GameFramework/GameObjects/WorldObject/WorldActor.h"
#include "../GameFramework/GameObjects/WorldObject/WorldPawn.h"

using namespace RPE;
Engine* Engine::s_instance = nullptr;

DEFINE_LOG_CATEGORY_STATIC(EngineLog);

Engine::Engine(std::unique_ptr<class IWindowManager> WindowManager, std::unique_ptr<class RHI> renderer)  //
    : m_WindowManager(std::move(WindowManager)), m_inputManager(std::make_shared<InputManager>()), m_renderer(std::move(renderer)),
      mainWindowId(WindowId{0})
{
    s_instance = this;
}

Engine::~Engine()
{
    if (!SaveGameInstance())
    {
        RP_LOG(EngineLog, Error, "Failed to save GameInstance to json");
    }
    s_instance = nullptr;
    m_initialized = false;
    Gameinstance.reset();
}

void Engine::run()
{
    if (!m_initialized)
    {
        RP_LOG(EngineLog, Error, "Cannot run: {} is not initialized ...", ENGINE_NAME);
        return;
    }
    if (Gameinstance)
    {
        if (Gameinstance->isInitialized())
        {
            Gameinstance->StartGame();
        }
    }
    auto lastTime = std::chrono::high_resolution_clock::now();
    RP_LOG(EngineLog, Display, "{} runned", ENGINE_NAME);
    while (!m_WindowManager->areAllWindowsClosed() && !m_requestExit)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        deltaTime = (std::min)(deltaTime, 0.033f);
        m_WindowManager->update();
        updateGameLogic(deltaTime);
        static int failcount = 0;
        constexpr int MAXFAILCOUNT = 3;
        if (m_currentWidth == 0 && m_currentHeigth == 0)
        {
            m_WindowManager->update();
            continue;
        }

        if (m_renderer)
        {
            m_renderer->update();
            if (m_WindowManager->areAllWindowsClosed()) return;
            if (!m_renderer->render())
            {

                if (failcount >= MAXFAILCOUNT)
                {
                    RP_LOG(EngineLog, Error, "Exit after {} failes", failcount);
                    requestExit();
                    break;
                }
                if (failcount < MAXFAILCOUNT) failcount++;
            }
            else
            {
                failcount = 0;
            }
        }

        m_inputManager->update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    RP_LOG(EngineLog, Display, "{} stoped", ENGINE_NAME);
}

void Engine::requestExit()
{
    m_requestExit = true;
}

IWindow* Engine::getMainWindow() const
{
    return m_WindowManager->getWindowById(mainWindowId).get();
}

std::vector<std::string>& Engine::getNeededPipelineNames()
{
    return m_neededPipelineNames;
}

int Engine::preInit(int argc, char* argv[])
{
    if (argc > 0)
    {
        std::string fullPath = argv[0];

        // Находим последний разделитель пути
        size_t lastSlash = fullPath.find_last_of("\\/");
        if (lastSlash != std::string::npos)
        {
            m_executablePath = fullPath.substr(0, lastSlash + 1);
        }
        else
        {
            m_executablePath = "./";
        }

        RP_LOG(EngineLog, Display, "Executable path: {}", m_executablePath);
    }
    return 0;
}

int Engine::init()
{
    RP_LOG(EngineLog, Display, "Initializing {}, version {}", ENGINE_NAME, version());

  
    m_neededPipelineNames.push_back("Triangle");
   
    WindowSettings wset;
    wset.width = 1024;
    wset.height = 768;
    wset.title = "Lost";
    wset.x = 200;
    wset.y = 200;
    const auto windowResult = m_WindowManager->createWindow(wset);
    if (m_renderer)
    {
        if (!m_renderer->preInit(wset, ENGINE_NAME))
        {
            RP_LOG(EngineLog, Error, "Failed to preInit {}", m_renderer->getName());
            m_renderer->shutdown();
            return 1;
        }
    }
    else
    {
        RP_LOG(EngineLog, Fatal, "Renderer is not created!!!");
        return 2;
    }

    if (!windowResult)
    {
        RP_LOG(EngineLog, Error, "Failed to create Window for {}", ENGINE_NAME);
        m_initialized = false;
        return 3;
    }
    setupWindowEvents(windowResult.value());
    mainWindowId = WindowId{windowResult.value()};
    if (m_inputManager)
    {
        m_inputManager->setExitCallback([this]() { requestExit(); });
    }
    if (const auto window = m_WindowManager->getWindowById(windowResult.value()))
    {
        if (!window->isValid())
        {
            RP_LOG(EngineLog, Error, "Created window is not valid for {}", ENGINE_NAME);
            m_initialized = false;
            return 4;
        }
        m_renderer->setEnginePtr(this);
        if (!m_renderer->init(window.get()))
        {
            RP_LOG(EngineLog, Error, "Failed to Init renderer");
            m_renderer->shutdown();
            return 5;
        }
    }
    if (!LoadGameInstance())
    {
        RP_LOG(EngineLog, Error, "Failed to load game instance");
        return 99;
    }
    if (Gameinstance)
    {
        Gameinstance->Init();
    }
    m_initialized = true;

    return 0;
}

bool Engine::LoadGameInstance()
{
    std::string SettingsPath = m_executablePath + "GameInstance.json";
    std::ifstream file(SettingsPath);

    if (file.is_open())
    {
        RP_LOG(EngineLog, Display, "Loading GameInstance from: {}", SettingsPath);

        nlohmann::json jsonData;
        try
        {
            file >> jsonData;
            file.close();

            // Проверяем наличие ClassName (один раз, не два)
            if (!jsonData.contains("ClassName") || !jsonData["ClassName"].is_string())
            {
                RP_LOG(EngineLog, Error, "Invalid GameInstance.json: missing ClassName");
                return CreateDefaultGameInstance();
            }

            std::string className = jsonData["ClassName"].get<std::string>();

            // ПРОВЕРЯЕМ, зарегистрирован ли класс
            if (!OBJECT_FACTORY.IsClassRegistered(className))
            {
                RP_LOG(EngineLog, Error, "Class '{}' not registered, creating default GameInstance", className);
                return CreateDefaultGameInstance();
            }

            // Опционально: проверяем, что класс наследуется от RGameInstance
            if (!OBJECT_FACTORY.IsDerivedFrom(className, "RGameInstance"))
            {
                RP_LOG(EngineLog, Error, "Class '{}' is not derived from RGameInstance, creating default", className);
                return CreateDefaultGameInstance();
            }

            std::string displayName = "GameInstance";
            if (jsonData.contains("DisplayName") && jsonData["DisplayName"].is_string())
            {
                displayName = jsonData["DisplayName"].get<std::string>();
            }

            // Создаем объект через фабрику
            CObject* obj = OBJECT_FACTORY.Create(className, nullptr, displayName);
            if (!obj)
            {
                RP_LOG(EngineLog, Error, "Failed to create GameInstance of class '{}'", className);
                return CreateDefaultGameInstance();
            }

            // Десериализуем
            obj->deserialize(jsonData);
            Gameinstance.reset();
            // Приводим к нужному типу
            Gameinstance = std::unique_ptr<RGameInstance>(static_cast<RGameInstance*>(obj));

            RP_LOG(EngineLog, Display, "GameInstance loaded successfully: {}", displayName);
            return true;
        }
        catch (const nlohmann::json::parse_error& e)
        {
            RP_LOG(EngineLog, Error, "JSON parse error in GameInstance.json: {}", e.what());
            return CreateDefaultGameInstance();
        }
        catch (const std::exception& e)
        {
            RP_LOG(EngineLog, Error, "Unexpected error loading GameInstance: {}", e.what());
            return CreateDefaultGameInstance();
        }
    }

    return CreateDefaultGameInstance();
}
bool Engine::CreateDefaultGameInstance()
{
    RP_LOG(EngineLog, Warning, "Creating default GameInstance");
    Gameinstance = std::make_unique<RGameInstance>("GameInstance", nullptr);
    return true;
}

bool Engine::SaveGameInstance()
{
    if (!Gameinstance)
    {
        RP_LOG(EngineLog, Error, "Cannot save GameInstance: not initialized");
        return false;
    }

    std::string SettingsPath = m_executablePath + "GameInstance.json";
    std::ofstream file(SettingsPath);

    if (!file.is_open())
    {
        RP_LOG(EngineLog, Error, "Cannot save GameInstance to: {}", SettingsPath);
        return false;
    }

    nlohmann::json jsonData;
    Gameinstance->serialize(jsonData);

    file << jsonData.dump(4);  // pretty print
    file.close();

    RP_LOG(EngineLog, Display, "GameInstance saved to: {}", SettingsPath);
    return true;
}

void Engine::onInputEvent(const InputEvent& event)
{
    if (m_renderer)
    {
        if (std::holds_alternative<ResizeData>(event.data))
        {
            const auto data = std::get<ResizeData>(event.data);
            m_currentHeigth = data.height;
            m_currentWidth = data.width;
            if (data.width > 0 && data.height > 0)
            {
                m_renderer->onResize(data.width, data.height);
            }
        }
    }
    if (!m_inputManager) return;
    m_inputManager->processInput(event);
}

void Engine::updateGameLogic(float deltaTime)
{
    if (!Gameinstance) return;

    Gameinstance->tick(deltaTime);
}

void Engine::setupWindowEvents(WindowId windowId)
{
#ifdef RPE_USE_NATIVE_WINDOW
#ifdef _WIN32
    auto* specificManager = dynamic_cast<Win32WindowManager*>(m_WindowManager.get());
    if (specificManager)
    {
        auto window = m_WindowManager->getWindowById(windowId);
        if (auto* specificWindow = dynamic_cast<Win32Window*>(window.get()))
        {
            specificWindow->onEvent().subscribe([this](const InputEvent& event) { onInputEvent(event); });
        }
    }
#elif defined(__linux__)
    auto* specificManager = dynamic_cast<LinuxWindowManager*>(m_WindowManager.get());
    if (specificManager)
    {
        auto window = m_WindowManager->getWindowById(windowId);
        if (auto* specificWindow = dynamic_cast<LinuxWindow*>(window.get()))
        {
            specificWindow->onEvent().subscribe([this](const InputEvent& event) { onInputEvent(event); });
        }
    }
#elif defined(__APPLE__)
    auto* specificManager = dynamic_cast<MacOSWindowManager*>(m_WindowManager.get());
    if (specificManager)
    {
        auto window = m_WindowManager->getWindowById(windowId);
        if (auto* specificWindow = dynamic_cast<MacOSWindow*>(window.get()))
        {
            specificWindow->onEvent().subscribe([this](const InputEvent& event) { onInputEvent(event); });
        }
    }
#endif
#else
    auto* specificManager = dynamic_cast<GLFWWindowManager*>(m_WindowManager.get());
    if (specificManager)
    {
        auto window = m_WindowManager->getWindowById(windowId);
        if (auto* specificWindow = dynamic_cast<GLFWWindow*>(window.get()))
        {
            specificWindow->onEvent().subscribe([this](const InputEvent& event) { onInputEvent(event); });
        }
    }
#endif
}
