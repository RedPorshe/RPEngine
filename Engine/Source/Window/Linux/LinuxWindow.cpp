#include "LinuxWindow.h"
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(LogLinuxWindow);

using namespace RPE;

LinuxWindow::LinuxWindow(const WindowSettings& settings)
{
    RP_LOG(LogLinuxWindow, Error, "Linux window is not implemented!");
}

LinuxWindow::~LinuxWindow() {}

void LinuxWindow::setTitle(const std::string& title) {}

bool LinuxWindow::isValid() const
{
    return false;
}

bool LinuxWindow::shouldClose() const
{
    return true;
}

void* RPE::LinuxWindow::getWindowHandle() const
{
    return nullptr;
}
