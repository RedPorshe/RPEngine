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
        window->setTitle("Tests of ");
    }

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
