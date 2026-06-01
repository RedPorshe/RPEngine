#pragma once
#include <string>
#include <functional>
#include "Window/IWindow.h"

namespace RPE
{

class MacOSWindow final : public IWindow
{
public:
    MacOSWindow(const WindowSettings& settings);
    ~MacOSWindow() override;

    void setTitle(const std::string& title) override;
    bool isValid() const override;
    bool shouldClose() const override;
};

}  // namespace RPE