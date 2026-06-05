#pragma once

#include <string>
#include <functional>
#include "Window/IWindow.h"
#include "Event/Event.h"
#include "Event/InputEvent.h"
#include <Windows.h>

namespace RPE
{

class Win32Window final : public IWindow
{
public:
    Win32Window(WindowId inID, const WindowSettings& sSettings);
    ~Win32Window() override;

    void setTitle(const std::string& title) override;
    bool isValid() const override;
    bool shouldClose() const override;
    void* getWindowHandle() const override;
    Event<const InputEvent&>& onEvent();

    LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    bool RegisterWindowClass();
    bool CreateWindowHandle();

    HWND m_window{nullptr};
    WindowId m_id;
    WindowSettings m_settings;
    bool m_shouldClose{false};
    Event<const InputEvent&> m_windowEvent;
};

}  // namespace RPE