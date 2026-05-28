#include "GLFWWindow.h"
#include "Log/Log.h"
#include "EngineConfig.h"
#include <GLFW/glfw3.h>

DEFINE_LOG_CATEGORY_STATIC(LogGLFWWindow);
using namespace RPE;

GLFWWindow::GLFWWindow(const WindowSettings& sSettings)
{
    const std::string TitlePostfix = ENGINE_NAME;
    const std::string finalTitle = sSettings.title + "  ||  " + TitlePostfix;
    m_window = glfwCreateWindow(sSettings.width, sSettings.height, finalTitle.c_str(), nullptr, nullptr);
    if (!m_window)
    {
        RP_LOG(LogGLFWWindow, Error, "GLFW window create error");
        return;
    }
    glfwSetWindowPos(m_window, sSettings.x, sSettings.y);
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
