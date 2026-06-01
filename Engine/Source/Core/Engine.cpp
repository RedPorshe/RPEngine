#include "Engine.h"
#include "Log/Log.h"
#include "Window/IWindowManager.h"
#include "Window/IWindow.h"

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

using namespace RPE;
// temp include
#include "GameFramework/PlayerController.h"

Engine* Engine::s_instance = nullptr;

DEFINE_LOG_CATEGORY_STATIC(EngineLog);

Engine::Engine(std::unique_ptr<class IWindowManager> WindowManager)  //
    : m_WindowManager(std::move(WindowManager)), m_inputManager(std::make_shared<InputManager>())
{
    RP_LOG(EngineLog, Display, "Initializing {}, version {}", ENGINE_NAME, version());
    WindowSettings wset;
    wset.width = 1024;
    wset.height = 768;
    wset.title = "Lost";
    wset.x = 200;
    wset.y = 200;
    const auto windowResult = m_WindowManager->createWindow(wset);

    if (!windowResult)
    {
        RP_LOG(EngineLog, Error, "Failed to create Window for {}", ENGINE_NAME);
        m_initialized = false;
        return;
    }
    setupWindowEvents(windowResult.value());
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
        }
    }
    //*****
    auto playerController = std::make_shared<PlayerController>();
    playerController->setName("Main Player Controller");
    m_inputManager->setActiveController(playerController);
    /******/
    m_initialized = true;
    s_instance = this;
}

Engine::~Engine()
{
    s_instance = nullptr;
    m_initialized = false;
}

void Engine::run()
{
    if (!m_initialized)
    {
        RP_LOG(EngineLog, Error, "Cannot run: {} is not initialized ...", ENGINE_NAME);
        return;
    }
    auto lastTime = std::chrono::high_resolution_clock::now();
    RP_LOG(EngineLog, Display, "{} runned", ENGINE_NAME);
    while (!m_WindowManager->areAllWindowsClosed() && !m_requestExit)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        deltaTime = std::min(deltaTime, 0.033f);
        m_WindowManager->update();
        updateGameLogic(deltaTime);
        // then m_renderer->render();
        m_inputManager->update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    RP_LOG(EngineLog, Display, "{} stoped", ENGINE_NAME);
}

void RPE::Engine::requestExit()
{
    m_requestExit = true;
}

void Engine::onInputEvent(const InputEvent& event)
{
    if (!m_inputManager) return;
    m_inputManager->processInput(event);
}

void Engine::updateGameLogic(float deltaTime)
{
    // here we will update our game logic, physics, animations, etc. based on the delta time
    (void)deltaTime;
    /*   if (!m_currentGameInstance) return;
       m_currentGameInstance->update(deltaTime);*/
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
