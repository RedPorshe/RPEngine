#pragma once
#include <string>
#include <functional>
#include "Window/IWindow.h"
#include "Event/Event.h"
#include "Event/InputEvent.h"

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
    Event<const InputEvent&>& onEvent() { return m_windowEvent; };

private:
    const WindowId m_id;
    Event<const InputEvent&> m_windowEvent;
};

}  // namespace RPE