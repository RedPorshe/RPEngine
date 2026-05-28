#pragma once
#include <string>
#include <functional>
#include "Window/IWindow.h"

namespace RPE
{

class Win32Window final : public IWindow
{
public:
    Win32Window(const WindowSettings& settings);
    ~Win32Window() override;

    void setTitle(const std::string& title) override;
    bool isValid() const override;
    bool shouldClose() const override;
};

}  // namespace LifeExe