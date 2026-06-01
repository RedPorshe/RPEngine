#pragma once
#include "Window/IWindow.h"
#include "Window/IWindowManager.h"
#include "Core/Utility.h"
#include <memory>
#include <unordered_map>
#include <expected>
#include <functional>

namespace RPE
{

class LinuxWindowManager final : public IWindowManager, public NonCopyable
{
public:
    LinuxWindowManager();
    ~LinuxWindowManager() override;
    bool Isinitialized() const;
    void update() override;
    std::expected<WindowId, WindowCreationerror> createWindow(const WindowSettings& settings) override;
    std::shared_ptr<IWindow> getWindowById(WindowId id) const override;

    bool areAllWindowsClosed() const override;
};
}  // namespace RPE