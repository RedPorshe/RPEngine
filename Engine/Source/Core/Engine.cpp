#include "Engine.h"
#include "Log/Log.h"
#include "Window/IWindowManager.h"
#include "Window/IWindow.h"
#include "Window/GLFW/GLFWWindowManager.h"
#include "Window/GLFW/GLFWWindow.h"
#include "Event/InputEvent.h"
#include "Input/InputManager.h"

#include <format>

using namespace RPE;
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
        return;
    }

    if (auto GLFWWndowManager = dynamic_cast<GLFWWindowManager*>(m_WindowManager.get()))
    {
        auto wind = m_WindowManager->getWindowById(windowResult.value());
        if (auto glfwwin = dynamic_cast<GLFWWindow*>(wind.get()))
        {
            glfwwin->onEvent().subscribe([this](const InputEvent& event) { onInputEvent(event); });
        }
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

void Engine::onInputEvent(const InputEvent& event)
{
    if (!m_inputManager) return;
    m_inputManager->processInput(event);
    if (std::holds_alternative<MouseMoveData>(event.data))
    {
        const auto& mouseData = std::get<MouseMoveData>(event.data);
        std::string tempTitle = {};
        tempTitle = std::format("mouse pos x{},y{}", mouseData.x, mouseData.y);

        if (auto window = m_WindowManager->getWindowById(WindowId{1}))
        {
            window->setTitle(tempTitle);
        }
    }
}
