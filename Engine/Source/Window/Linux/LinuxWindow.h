#pragma once
#include <string>
#include <functional>
#include "Window/IWindow.h"

namespace RPE
{

class LinuxWindow final : public IWindow
{
public:
    LinuxWindow(const WindowSettings& settings);
    ~LinuxWindow() override;

    void setTitle(const std::string& title) override;
    bool isValid() const override;
    bool shouldClose() const override;
    void* getWindowHandle() const override;
};

}  // namespace RPE