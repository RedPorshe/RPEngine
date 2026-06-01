#include "Win32WindowManager.h"
#include "Win32WindowManager.h"
#include "Win32Window.h"
#include "Log/Log.h"

using namespace RPE;

DEFINE_LOG_CATEGORY_STATIC(LogWin32WindowManager);

Win32WindowManager::Win32WindowManager()
{
    RP_LOG(LogWin32WindowManager, Error, "Win32 API window manager is not implemented!");
    RP_LOG(LogWin32WindowManager, Display, "Try create win32 Window...");
    WindowSettings set;
    Win32Window* temp = new Win32Window(set);
    delete temp;
}

Win32WindowManager::~Win32WindowManager() {}

void Win32WindowManager::update() {}

bool Win32WindowManager::areAllWindowsClosed() const
{
    return true;
}

std::expected<WindowId, WindowCreationerror> Win32WindowManager::createWindow(const WindowSettings& settings)
{
    return std::unexpected(WindowCreationerror::ManagerIsNotInitialized);
}

std::shared_ptr<IWindow> Win32WindowManager::getWindowById(WindowId id) const
{
    return nullptr;
}

bool RPE::Win32WindowManager::Isinitialized() const
{
    return true;
}
