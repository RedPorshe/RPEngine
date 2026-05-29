#pragma once
#include "Window/IWindow.h"
#include <memory>
#include <expected>
#include <functional>

namespace RPE
{
enum class WindowCreationerror
{
    ManagerIsNotInitialized,
    CreationFailed,
    InvalidWindow
};

struct InputEvent;

class IWindowManager
{
public:
    virtual ~IWindowManager() = default;
    virtual void update() = 0;
    virtual std::expected<WindowId, WindowCreationerror> createWindow(const WindowSettings& settings) = 0;
    virtual std::shared_ptr<IWindow> getWindowById(WindowId id) const = 0;
    virtual bool areAllWindowsClosed() const = 0;
};
}  // namespace RPE