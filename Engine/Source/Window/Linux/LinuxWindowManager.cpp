#include "LinuxWindowManager.h"
#include "LinuxWindow.h"
#include "Log/Log.h"

using namespace RPE;

DEFINE_LOG_CATEGORY_STATIC(LogLinuxWindowManager);

LinuxWindowManager::LinuxWindowManager()
{
    RP_LOG(LogLinuxWindowManager, Error, "Linux window manager is not implemented!");
    auto settings = WindowSettings{};
    auto window = std::make_shared<LinuxWindow>(settings);
    window.reset();
}

LinuxWindowManager::~LinuxWindowManager() {}

bool LinuxWindowManager::Isinitialized() const
{
    return true;
}

void LinuxWindowManager::update() {}

std::expected<WindowId, WindowCreationerror> LinuxWindowManager::createWindow(const WindowSettings& settings)
{
    return std::unexpected(WindowCreationerror::ManagerIsNotInitialized);
}

std::shared_ptr<IWindow> LinuxWindowManager::getWindowById(WindowId id) const
{
    return nullptr;
}

bool LinuxWindowManager::areAllWindowsClosed() const
{
    return true;
}
