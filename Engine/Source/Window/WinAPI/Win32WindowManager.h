#pragma once

#include "Window/IWindowManager.h"
#include "Window/IWindow.h"
#include <memory>
#include <expected>

namespace RPE
{

class Win32WindowManager final : public IWindowManager
{
public:
    Win32WindowManager();
    ~Win32WindowManager() override;

    void update() override;
    bool areAllWindowsClosed() const override;

    std::expected<WindowId, WindowCreationerror> createWindow(const WindowSettings& settings) override;
    std::shared_ptr<IWindow> getWindowById(WindowId id) const override;
    bool Isinitialized() const;

private:
    bool m_initialized{false};
    std::unordered_map<WindowId, std::shared_ptr<IWindow>> m_Windows;
    WindowId m_windowIdCounter{1};
    void cleanupClosedWindows();
};

}  // namespace RPE