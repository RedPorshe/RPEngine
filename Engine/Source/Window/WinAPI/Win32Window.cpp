#include "Win32Window.h"
#include "Log/Log.h"

using namespace RPE;

DEFINE_LOG_CATEGORY_STATIC(LogWin32Window);

Win32Window::Win32Window(const WindowSettings& settings)
{
    RP_LOG(LogWin32Window, Error, "Win32 API window is not implemented!");
}

Win32Window::~Win32Window()
{
    RP_LOG(LogWin32Window, Display, "Deleting win32 window");
}

void Win32Window::setTitle(const std::string& title) {}

bool Win32Window::isValid() const
{
    return false;
}

bool Win32Window::shouldClose() const
{
    return true;
}

void* RPE::Win32Window::getWindowHandle() const
{
    return nullptr;
}
