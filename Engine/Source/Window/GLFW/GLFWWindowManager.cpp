#include "GLFWWindowManager.h"
#include "GLFWWindow.h"
#include "Log/Log.h"
#include <GLFW/glfw3.h>

DEFINE_LOG_CATEGORY_STATIC(LogGLFWWindowManager);

using namespace RPE;

GLFWWindowManager::GLFWWindowManager()
{
    glfwSetErrorCallback([](int errorCode, const char* description)
        { RP_LOG(LogGLFWWindowManager, Error, "GLFW error code: {}, description: {}", errorCode, description); });
    if (!glfwInit())
    {
        RP_LOG(LogGLFWWindowManager, Error, "Failed to initialize GLFW!");
        return;
    }
    m_initialized = true;
    RP_LOG(LogGLFWWindowManager, Display, "Initialize GLFW Success!");
}

GLFWWindowManager::~GLFWWindowManager()
{
    m_Windows.clear();
    if (m_initialized)
    {

        glfwSetErrorCallback(nullptr);
        glfwTerminate();
    }
    m_initialized = false;
    RP_LOG(LogGLFWWindowManager, Display, "GLFW shutdown complete!");
}

bool GLFWWindowManager::Isinitialized() const
{
    return m_initialized;
}

void GLFWWindowManager::update()
{
    if (!Isinitialized()) return;

    glfwPollEvents();
    cleanupClosedWindows();
}

std::expected<WindowId, WindowCreationerror> GLFWWindowManager::createWindow(const WindowSettings& settings)
{
    if (!Isinitialized())
    {
        RP_LOG(LogGLFWWindowManager, Error, "Cannot create window. GLFW is not initialized");
        return std::unexpected(WindowCreationerror::ManagerIsNotInitialized);
    }
    auto window = std::make_shared<GLFWWindow>(settings);
    if (!window->isValid())
    {
        RP_LOG(LogGLFWWindowManager, Error, "Failed  to create GLFW window.");
        return std::unexpected(WindowCreationerror::InvalidWindow);
    }
    const WindowId id = m_windowIdCounter++;
    m_Windows[id] = window;

    RP_LOG(LogGLFWWindowManager, Display, "Added window with id: {}", id.value);

    return id;
}

std::shared_ptr<GLFWWindow> GLFWWindowManager::getWindowById(WindowId id) const
{
    const auto it = m_Windows.find(id);
    return it != m_Windows.end() ? it->second : nullptr;
}

bool GLFWWindowManager::areAllWindowsClosed() const
{
    return m_Windows.empty();
}

void GLFWWindowManager::cleanupClosedWindows()
{
    auto it = m_Windows.begin();
    while (it != m_Windows.end())
    {
        if (it->second->shouldClose())
        {
            RP_LOG(LogGLFWWindowManager, Display, "Remove closed window with id: {}", it->first.value);
            it = m_Windows.erase(it);
            m_windowIdCounter--;
            continue;
        }
        ++it;
    }
}
