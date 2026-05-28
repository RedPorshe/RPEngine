#pragma once
#include <string>
#include <functional>
#include "Window/IWindow.h"

struct GLFWwindow;

namespace RPE
{

class GLFWWindow final : public IWindow
{
public:
    GLFWWindow(const WindowSettings& sSettings);
    ~GLFWWindow() override;
    void setTitle(const std::string& title) override;
    bool isValid() const override;
    bool shouldClose() const override;
    void shutdown();

private:
    GLFWwindow* m_window{nullptr};
};
}  // namespace RPE
