#pragma once
#include <string>
#include <functional>
#include "Window/IWindow.h"
#include "Core/Utility.h"
#include "Event/Event.h"
#include "Event/InputEvent.h"

struct GLFWwindow;

namespace RPE
{

class GLFWWindow final : public IWindow, public NonCopyable
{
public:
    GLFWWindow(WindowId inID,const WindowSettings& sSettings);
    ~GLFWWindow() override;
    void setTitle(const std::string& title) override;
    bool isValid() const override;
    bool shouldClose() const override;
    void shutdown();

private:
    GLFWwindow* m_window{nullptr};
    const WindowId m_id;
    Event<const InputEvent&> m_windowEvent;
};
}  // namespace RPE
