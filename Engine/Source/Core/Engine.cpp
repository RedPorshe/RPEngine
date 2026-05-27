#include "Engine.h"
#include "Log/Log.h"
#include "Window/GLFW/GLFWWindowManager.h"

#include <format>

using namespace RPE;
DEFINE_LOG_CATEGORY_STATIC(EngineLog);

Engine::Engine()
{
    RP_LOG(EngineLog, Display, "Initializing {}, version {}", ENGINE_NAME, version());
    m_WindowManager = std::make_unique<GLFWWindowManager>();
    const auto windowResult = m_WindowManager->createWindow(WindowSettings{});

    if (!windowResult)
    {
        RP_LOG(EngineLog, Error, "Failed to create Window for {}", ENGINE_NAME);
        return;
    }
    if (auto window = m_WindowManager->getWindowById(windowResult.value()))
    {
        window->setTitle("Test ");
    }
    WindowSettings set;
    set.height = 400;
    set.width = 400;
    set.posy = 100;
    set.posx = 100;
    set.title = "asdaaa";
    m_WindowManager->createWindow(set);
    set.title = "asd4234321a";
    m_WindowManager->createWindow(set);
    set.title = "asdasda";
    m_WindowManager->createWindow(set);
    set.title = "asdas342da";
    m_WindowManager->createWindow(set);
    set.title = "asda423sda";
    m_WindowManager->createWindow(set);
    set.title = "as432dasda";
    m_WindowManager->createWindow(set);
    set.title = "as432dasdsda";
    m_WindowManager->createWindow(set);

    m_initialized = true;
}
Engine::~Engine() = default;

void Engine::run()
{
    if (!m_initialized)
    {
        RP_LOG(EngineLog, Error, "Cannot run: {} is not initialized ...", ENGINE_NAME);
        return;
    }
    RP_LOG(EngineLog, Display, "{} runned", ENGINE_NAME);
    while (!m_WindowManager->areAllWindowsClosed())
    {
        m_WindowManager->update();
    }
    RP_LOG(EngineLog, Display, "{} stoped", ENGINE_NAME);
}
