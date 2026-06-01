#include "MacOSWindow.h"
#include "Log/Log.h"

using namespace RPE;

DEFINE_LOG_CATEGORY_STATIC(LogMacOSWindow);

MacOSWindow::MacOSWindow(const WindowSettings& settings)
{
    RP_LOG(LogMacOSWindow, Error, "macOS window is not implemented!");
}

MacOSWindow::~MacOSWindow()
{
    RP_LOG(LogMacOSWindow, Display, "Deleting macOS window");
}

void MacOSWindow::setTitle(const std::string& title) {}

bool MacOSWindow::isValid() const
{
    return false;
}

bool MacOSWindow::shouldClose() const
{
    return true;
}