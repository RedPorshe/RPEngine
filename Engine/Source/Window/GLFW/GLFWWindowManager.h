#pragma once
#include "Window/IWindow.h"
#include "Window/IWindowManager.h"
#include "Core/Utility.h"
#include <memory>
#include <unordered_map>
#include <expected>

namespace RPE
{

class GLFWWindowManager final : public IWindowManager, public NonCopyable
{
public:
    GLFWWindowManager();
    ~GLFWWindowManager() override;
    bool Isinitialized() const;
    void update() override;
    std::expected<WindowId, WindowCreationerror> createWindow(const WindowSettings& settings) override;
    std::shared_ptr<IWindow> getWindowById(WindowId id) const override;

    bool areAllWindowsClosed() const override;

private:
    bool m_initialized{false};
    std::unordered_map<WindowId, std::shared_ptr<IWindow>> m_Windows;
    WindowId m_windowIdCounter{1};
    void cleanupClosedWindows();
};
}  // namespace RPE