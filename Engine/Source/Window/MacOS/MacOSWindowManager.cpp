#include "MacOSWindowManager.h"
#include "MacOSWindowManager.h"
#include "MacOSWindow.h"
#include "Log/Log.h"

using namespace RPE;

DEFINE_LOG_CATEGORY_STATIC(LogMacOSWindowManager);

MacOSWindowManager::MacOSWindowManager()
{
    RP_LOG(LogMacOSWindowManager, Error, "macOS window manager is not implemented!");
    RP_LOG(LogMacOSWindowManager, Display, "Try create macOS Window...");
    WindowSettings set;
    MacOSWindow* temp = new MacOSWindow(set);
    delete temp;
}

MacOSWindowManager::~MacOSWindowManager() {}

void MacOSWindowManager::update() {}

bool MacOSWindowManager::areAllWindowsClosed() const
{
    return true;
}

std::expected<WindowId, WindowCreationerror> MacOSWindowManager::createWindow(const WindowSettings& settings)
{
    return std::unexpected(WindowCreationerror::ManagerIsNotInitialized);
}

std::shared_ptr<IWindow> MacOSWindowManager::getWindowById(WindowId id) const
{
    return nullptr;
}

bool MacOSWindowManager::Isinitialized() const
{
    return true;
}
