#include "Win32Window.h"
#include "EngineConfig.h"
#include "Event/Event.h"
#include "Event/InputEvent.h"
#include "Log/Log.h"

using namespace RPE;

DEFINE_LOG_CATEGORY_STATIC(LogWin32Window);

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Win32Window* pWindow = nullptr;

    if (uMsg == WM_NCCREATE)
    {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pWindow = reinterpret_cast<Win32Window*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
    }
    else
    {
        pWindow = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (pWindow && pWindow->isValid())
    {
        return pWindow->handleMessage(uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

Win32Window::Win32Window(WindowId inID, const WindowSettings& sSettings)
    : m_id(inID), m_settings(sSettings), m_window(nullptr), m_shouldClose(false)
{
    RP_LOG(LogWin32Window, Display, "Creating Win32 window...");

    if (!RegisterWindowClass())
    {
        RP_LOG(LogWin32Window, Error, "Failed to register window class");
        return;
    }

    if (!CreateWindowHandle())
    {
        RP_LOG(LogWin32Window, Error, "Failed to create window");
        return;
    }

    RP_LOG(LogWin32Window, Display, "Win32 API window Created");
}

Win32Window::~Win32Window()
{
    if (m_window)
    {
        DestroyWindow(m_window);
        m_window = nullptr;
    }
    RP_LOG(LogWin32Window, Display, "Deleting win32 window");
}

void Win32Window::setTitle(const std::string& title)
{
    SetWindowTextA(m_window, title.c_str());
}

bool Win32Window::isValid() const
{
    return m_window != nullptr;
}

bool Win32Window::shouldClose() const
{
    return m_shouldClose;
}

void* Win32Window::getWindowHandle() const
{
    return m_window;
}

Event<const InputEvent&>& Win32Window::onEvent()
{
    return m_windowEvent;
}

bool Win32Window::RegisterWindowClass()
{
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    const wchar_t* className = L"KuzbassEngineWindow";  // ← широкие символы

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = className;

    if (!RegisterClassExW(&wc))
    {
        DWORD error = GetLastError();
        RP_LOG(LogWin32Window, Error, "RegisterClassEx failed with error: {}", error);
        return false;
    }

    RP_LOG(LogWin32Window, Display, "Window class registered successfully");
    return true;
}

bool Win32Window::CreateWindowHandle()
{
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    const wchar_t* className = L"KuzbassEngineWindow";

    // Конвертируем заголовок в wide string
    int wideSize = MultiByteToWideChar(CP_UTF8, 0, m_settings.title.c_str(), -1, nullptr, 0);
    std::wstring wideTitle(wideSize, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, m_settings.title.c_str(), -1, &wideTitle[0], wideSize);

    m_window = CreateWindowExW(0, className, wideTitle.c_str(), WS_OVERLAPPEDWINDOW, m_settings.x, m_settings.y, m_settings.width,
        m_settings.height, nullptr, nullptr, hInstance, this);

    if (m_window)
    {
        ShowWindow(m_window, SW_SHOW);
        UpdateWindow(m_window);
        RP_LOG(LogWin32Window, Display, "Win32 window created successfully");
        return true;
    }
    else
    {
        DWORD error = GetLastError();
        RP_LOG(LogWin32Window, Error, "CreateWindowEx failed with error: {}", error);
        return false;
    }
}

LRESULT Win32Window::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
        {
            m_shouldClose = true;
            PostQuitMessage(0);
            InputEvent event;
            event.type = EventType::WindowClose;
            m_windowEvent.invoke(event);
        }
            return 0;

        case WM_CLOSE:
        {
            m_shouldClose = true;
            InputEvent event;
            event.type = EventType::WindowClose;
            m_windowEvent.invoke(event);
            DestroyWindow(m_window);
        }
            return 0;

        case WM_SIZE:
        {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            InputEvent event;
            event.type = EventType::WindowResize;
            event.data = ResizeData{width, height};
            m_windowEvent.invoke(event);
        }
            return 0;

            // ========== КЛАВИАТУРА ==========
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            int key = static_cast<int>(wParam);
            int scancode = (lParam >> 16) & 0xFF;
            int extended = (lParam >> 24) & 1;

            // Различаем левый и правый Shift
            if (key == VK_SHIFT)
            {
                if (scancode == 54)
                    key = VK_RSHIFT;
                else if (scancode == 42)
                    key = VK_LSHIFT;
            }

            // Различаем левый и правый Control
            if (key == VK_CONTROL)
            {
                if (extended)
                    key = VK_RCONTROL;
                else
                    key = VK_LCONTROL;
            }

            // Различаем левый и правый Alt
            if (key == VK_MENU)
            {
                if (extended)
                    key = VK_RMENU;
                else
                    key = VK_LMENU;
            }

            int action = 1;
            int mods = 0;

            // Правильное определение модификаторов в GLFW формате
            if (GetAsyncKeyState(VK_CONTROL) & 0x8000) mods |= 2;                                         // GLFW_MOD_CONTROL = 2
            if (GetAsyncKeyState(VK_SHIFT) & 0x8000) mods |= 1;                                           // GLFW_MOD_SHIFT = 1
            if (GetAsyncKeyState(VK_MENU) & 0x8000) mods |= 4;                                            // GLFW_MOD_ALT = 4
            if ((GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000)) mods |= 8;  // GLFW_MOD_SUPER = 8

            InputEvent event;
            event.type = EventType::KeyPress;
            event.data = KeyData(key, scancode, action, mods);
            m_windowEvent.invoke(event);
        }
            return 0;

        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            int key = static_cast<int>(wParam);
            int scancode = (lParam >> 16) & 0xFF;
            int extended = (lParam >> 24) & 1;

            if (key == VK_SHIFT)
            {
                if (scancode == 54)
                    key = VK_RSHIFT;
                else if (scancode == 42)
                    key = VK_LSHIFT;
            }
            if (key == VK_CONTROL)
            {
                if (extended)
                    key = VK_RCONTROL;
                else
                    key = VK_LCONTROL;
            }
            if (key == VK_MENU)
            {
                if (extended)
                    key = VK_RMENU;
                else
                    key = VK_LMENU;
            }

            int action = 0;
            int mods = 0;

            // Правильное определение модификаторов в GLFW формате
            if (GetAsyncKeyState(VK_CONTROL) & 0x8000) mods |= 2;                                         // GLFW_MOD_CONTROL = 2
            if (GetAsyncKeyState(VK_SHIFT) & 0x8000) mods |= 1;                                           // GLFW_MOD_SHIFT = 1
            if (GetAsyncKeyState(VK_MENU) & 0x8000) mods |= 4;                                            // GLFW_MOD_ALT = 4
            if ((GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000)) mods |= 8;  // GLFW_MOD_SUPER = 8

            InputEvent event;
            event.type = EventType::KeyPress;
            event.data = KeyData(key, scancode, action, mods);
            m_windowEvent.invoke(event);
        }
            return 0;

        // ========== МЫШЬ ==========
        case WM_MOUSEMOVE:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            InputEvent event;
            event.type = EventType::MouseMove;
            event.data = MouseMoveData(static_cast<double>(x), static_cast<double>(y));
            m_windowEvent.invoke(event);
        }
            return 0;

        // Нажатия кнопок
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
        {
            int button = 0;
            if (uMsg == WM_LBUTTONDOWN) button = 0;
            if (uMsg == WM_RBUTTONDOWN) button = 1;
            if (uMsg == WM_MBUTTONDOWN) button = 2;
            if (uMsg == WM_XBUTTONDOWN)
            {
                button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4;
            }

            int action = 1;  // Press
            int mods = 0;
            if (GetKeyState(VK_CONTROL) & 0x8000) mods |= 2;  // GLFW_MOD_CONTROL
            if (GetKeyState(VK_SHIFT) & 0x8000) mods |= 1;    // GLFW_MOD_SHIFT
            if (GetKeyState(VK_MENU) & 0x8000) mods |= 4;
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(m_window, &pt);

            InputEvent event;
            event.type = EventType::MouseButton;
            event.data = MouseButtonData(button, action, mods, static_cast<double>(pt.x), static_cast<double>(pt.y));
            m_windowEvent.invoke(event);
        }
            return 0;

        // Отпускания кнопок
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
        {
            int button = 0;
            if (uMsg == WM_LBUTTONUP) button = 0;
            if (uMsg == WM_RBUTTONUP) button = 1;
            if (uMsg == WM_MBUTTONUP) button = 2;
            if (uMsg == WM_XBUTTONUP)
            {
                button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4;
            }

            int action = 0;  // Release
            int mods = 0;
            if (GetKeyState(VK_CONTROL) & 0x8000) mods |= 2;  // GLFW_MOD_CONTROL
            if (GetKeyState(VK_SHIFT) & 0x8000) mods |= 1;    // GLFW_MOD_SHIFT
            if (GetKeyState(VK_MENU) & 0x8000) mods |= 4;

            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(m_window, &pt);

            InputEvent event;
            event.type = EventType::MouseButton;
            event.data = MouseButtonData(button, action, mods, static_cast<double>(pt.x), static_cast<double>(pt.y));
            m_windowEvent.invoke(event);
        }
            return 0;

        case WM_MOUSEWHEEL:
        {
            double delta = GET_WHEEL_DELTA_WPARAM(wParam) / 120.0;
            InputEvent event;
            event.type = EventType::MouseScroll;
            event.data = ScrollData(0.0, delta);
            m_windowEvent.invoke(event);
        }
            return 0;

        default: return DefWindowProc(m_window, uMsg, wParam, lParam);
    }
}