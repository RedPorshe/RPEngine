#pragma once
#include "GLFWWindow.h"
#include <memory>
#include <unordered_map>
#include <expected>

namespace RPE
{
class GLFWWindow;

enum class WindowCreationerror
{
    ManagerIsNotInitialized,
    CreationFailed,
    InvalidWindow
};

class GLFWWindowManager final
{
public:
    GLFWWindowManager();
    ~GLFWWindowManager();
    bool Isinitialized() const;
    void update();
    std::expected<WindowId, WindowCreationerror> createWindow(const WindowSettings& settings);
    std::shared_ptr<GLFWWindow> getWindowById(WindowId id) const;

    bool areAllWindowsClosed() const;

private:
    bool m_initialized{false};
    std::unordered_map<WindowId, std::shared_ptr<GLFWWindow>> m_Windows;
    WindowId m_windowIdCounter{1};
    void cleanupClosedWindows();
};
}  // namespace RPE