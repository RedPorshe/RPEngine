#include "Engine.h"
#include "Log/Log.h"
#include "Window/IWindowManager.h"
#include "Window/IWindow.h"

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

/*            TESTS      */
#include "SystemObject.h"
#include <cassert>

DEFINE_LOG_CATEGORY_STATIC(TestLog)

using namespace RPE;

void RunObjectSystemTests()
{
    RP_LOG(TestLog, Display, "========== Starting Object System Tests ==========");

    // Тест 1: Создание и базовая иерархия
    {
        RP_LOG(TestLog, Display, "Test 1: Basic hierarchy creation");
        auto root = std::make_shared<SysObject>(nullptr, "Root");

        auto child1 = root->addSubobject<SysObject>("Child1");
        auto child2 = root->addSubobject<SysObject>("Child2");
        auto grandChild = child1->addSubobject<SysObject>("GrandChild");

        RP_LOG(TestLog, Display, "Root children count: {}", root->getChildCount());
        RP_LOG(TestLog, Display, "Child1 children count: {}", child1->getChildCount());

        assert(root->getChildCount() == 2);
        assert(child1->getChildCount() == 1);
        assert(root->getChild("Child1") == child1);
        assert(child1->getOwner() == root.get());
    }

    // Тест 2: Смена владельца
    {
        RP_LOG(TestLog, Display, "Test 2: Changing owner");
        auto root = std::make_shared<SysObject>(nullptr, "Root");
        auto newParent = std::make_shared<SysObject>(nullptr, "NewParent");
        auto object = std::make_shared<SysObject>(root.get(), "MovableObject");

        RP_LOG(TestLog, Display, "Before move - owner: {}", object->getOwner()->getName());
        assert(object->getOwner() == root.get());

        object->setOwner(newParent.get());

        RP_LOG(TestLog, Display, "After move - owner: {}", object->getOwner()->getName());
        assert(object->getOwner() == newParent.get());
        assert(root->getChild("MovableObject") == nullptr);
        assert(newParent->getChild("MovableObject") == object.get());
    }

    // Тест 3: Поиск объектов
    {
        RP_LOG(TestLog, Display, "Test 3: Object search");
        auto root = std::make_shared<SysObject>(nullptr, "Root");
        auto child = root->addSubobject<SysObject>("Child");
        auto grandChild = child->addSubobject<SysObject>("GrandChild");

        auto found = root->findChildRecursive("GrandChild");
        RP_LOG(TestLog, Display, "Found object: {}", found ? found->getName() : "Not found");
        assert(found == grandChild);

        auto notFound = root->findChildRecursive("NonExistent");
        assert(notFound == nullptr);
    }

    // Тест 4: Проверка отношений
    {
        RP_LOG(TestLog, Display, "Test 4: Relationship checks");
        auto root = std::make_shared<SysObject>(nullptr, "Root");
        auto child = root->addSubobject<SysObject>("Child");
        auto grandChild = child->addSubobject<SysObject>("GrandChild");

        assert(root->isRoot());
        assert(!child->isRoot());
        assert(grandChild->isChildOf(child));
        assert(grandChild->isChildOf(root.get()));
        assert(!root->isChildOf(child));
    }

    // Тест 5: Удаление объектов
    {
        RP_LOG(TestLog, Display, "Test 5: Object destruction");
        auto root = std::make_shared<SysObject>(nullptr, "Root");
        auto childShared = std::make_shared<SysObject>(root.get(), "Child");
        root->addChild(childShared);  // Добавляем через addChild, а не addSubobject
        auto grandChild = std::make_shared<SysObject>(childShared.get(), "GrandChild");
        childShared->addChild(grandChild);
        RP_LOG(TestLog, Display, "Root childs = {}", root->getChildCount());
        // Удаляем child - grandChild должен удалиться автоматически
        childShared.reset();
        RP_LOG(TestLog, Display, "Root childs after  childShared.reset() = {}", root->getChildCount());

        assert(root->getChildCount() == 1);
        RP_LOG(TestLog, Display, "Child and GrandChild automatically destroyed");
    }

    RP_LOG(TestLog, Display, "========== Object System Tests Completed ==========");
}
void TestCircularReferences()
{
    auto a = std::make_shared<SysObject>(nullptr, "A");
    auto b = std::make_shared<SysObject>(nullptr, "B");
    auto c = std::make_shared<SysObject>(b.get(), "C");
    auto cD = std::make_shared<SysObject>(a.get(), "CD");

    a->addChild(b);  // B становится дочерним A
    b->addChild(a);  // Должно быть запрещено или вызывать предупреждение
    c->addChild(a);
    cD->setOwner(b.get());
}

/*                 end tests*/

using namespace RPE;

Engine* Engine::s_instance = nullptr;

DEFINE_LOG_CATEGORY_STATIC(EngineLog);

Engine::Engine(std::unique_ptr<class IWindowManager> WindowManager, std::unique_ptr<class RHI> renderer)  //
    : m_WindowManager(std::move(WindowManager)), m_inputManager(std::make_shared<InputManager>()), m_renderer(std::move(renderer)),
      mainWindowId(WindowId{0})
{
    s_instance = this;
    RunObjectSystemTests();
    TestCircularReferences();
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
        // m_currentWidth = m_renderer->getWidth();
        //  m_currentHeigth = m_renderer->getHeight();

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

    // m_neededPipelineNames.push_back("main");
    m_neededPipelineNames.push_back("Triangle");
    // m_neededPipelineNames.push_back("Wireframe");
    // m_neededPipelineNames.push_back("PointCloud");
    // m_neededPipelineNames.push_back("Transparent");
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
    m_initialized = true;
    return 0;
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
