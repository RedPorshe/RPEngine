#include "GLFWWindow.h"
#include "Log/Log.h"
#include "EngineConfig.h"
#include <GLFW/glfw3.h>

DEFINE_LOG_CATEGORY_STATIC(LogGLFWWindow);
using namespace RPE;

GLFWWindow::GLFWWindow(WindowId inID, const WindowSettings& sSettings) : m_id(inID)
{
    // init
    const std::string TitlePostfix = ENGINE_NAME;
    const std::string finalTitle = sSettings.title + "  ||  " + TitlePostfix;
    m_window = glfwCreateWindow(sSettings.width, sSettings.height, finalTitle.c_str(), nullptr, nullptr);
    if (!m_window)
    {
        RP_LOG(LogGLFWWindow, Error, "GLFW window create error");
        return;
    }
    glfwSetWindowPos(m_window, sSettings.x, sSettings.y);

    // user pointer
    glfwSetWindowUserPointer(m_window, this);
    // events
    glfwSetWindowCloseCallback(m_window,
        [](GLFWwindow* window)
        {
            auto* thisWindow = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
            RP_LOG(LogGLFWWindow, Display, "Window with id = {} closed!", thisWindow->m_id.value);
            InputEvent event;
            event.type = EventType::WindowClose;
            event.data = 100;
            thisWindow->m_windowEvent.invoke(event);
        });

    glfwSetFramebufferSizeCallback(m_window,
        [](GLFWwindow* window, int width, int height)
        {
            auto* thisWindow = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
            RP_LOG(LogGLFWWindow, Display, "Resize  window with id {} to  {}x{} ", thisWindow->m_id.value, width, height);
            InputEvent event;
            event.type = EventType::WindowResize;
            thisWindow->m_windowEvent.invoke(event);
        });

    glfwSetKeyCallback(m_window,
        [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            auto* win = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
            RP_LOG(LogGLFWWindow, Display, " key ={}, scancode ={}", key, scancode);
            InputEvent event;
            event.type = EventType::KeyPress;
            win->m_windowEvent.invoke(event);
        });

    glfwSetCursorPosCallback(m_window,
        [](GLFWwindow* window, double xpos, double ypos)
        {
            auto* thisWindow = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
            // RP_LOG(LogGLFWWindow, Display, "Window with id = {} closed!", thisWindow->m_id.value);
            InputEvent event;
            event.type = EventType::MouseMove;
            thisWindow->m_windowEvent.invoke(event);
        });

    glfwSetMouseButtonCallback(m_window,
        [](GLFWwindow* window, int button, int action, int mods)
        {
            auto* thisWindow = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
            RP_LOG(LogGLFWWindow, Display, "mouse button :{} ation: {}", button, action);
            InputEvent event;
            event.type = EventType::MouseButton;
            thisWindow->m_windowEvent.invoke(event);
        });

    glfwSetScrollCallback(m_window,
        [](GLFWwindow* window, double xoffset, double yoffset)
        {
            auto* thisWindow = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
            RP_LOG(LogGLFWWindow, Display, "mouse xoffset:{} yoffset: {}", xoffset, yoffset);
            InputEvent event;
            event.type = EventType::MouseScroll;
            thisWindow->m_windowEvent.invoke(event);
        });
}

GLFWWindow::~GLFWWindow()
{
    if (isValid()) shutdown();
}

void GLFWWindow::setTitle(const std::string& title)
{
    if (!m_window) return;
    const std::string TitlePostfix = ENGINE_NAME;
    const std::string finalTitle = title + "  ||  " + TitlePostfix;
    glfwSetWindowTitle(m_window, finalTitle.c_str());
}

bool GLFWWindow::isValid() const
{
    return m_window != nullptr;
}

bool GLFWWindow::shouldClose() const
{
    if (!m_window) return true;
    return glfwWindowShouldClose(m_window);
}

void GLFWWindow::shutdown()
{
    if (m_window)
    {
        glfwDestroyWindow(m_window);
        RP_LOG(LogGLFWWindow, Display, "GLFW window destroyed");
        m_window = nullptr;
    }
}
