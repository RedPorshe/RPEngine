// KeyCodes.h
#pragma once
#include <cstdint>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

namespace RPE
{

enum class Key : uint16_t
{
    // Буквы
    Space = 32,
    Apostrophe = 39,
    Comma = 44,
    Minus = 45,
    Period = 46,
    Slash = 47,

    Digit0 = 48,
    Digit1 = 49,
    Digit2 = 50,
    Digit3 = 51,
    Digit4 = 52,
    Digit5 = 53,
    Digit6 = 54,
    Digit7 = 55,
    Digit8 = 56,
    Digit9 = 57,

    Semicolon = 59,
    Equal = 61,

    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,

    LeftBracket = 91,
    Backslash = 92,
    RightBracket = 93,
    GraveAccent = 96,

    // Функциональные клавиши
    Escape = 256,
    Enter = 257,
    Tab = 258,
    Backspace = 259,
    Insert = 260,
    Delete = 261,
    Right = 262,
    Left = 263,
    Down = 264,
    Up = 265,
    PageUp = 266,
    PageDown = 267,
    Home = 268,
    End = 269,
    CapsLock = 280,
    ScrollLock = 281,
    NumLock = 282,
    PrintScreen = 283,
    Pause = 284,

    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    F13 = 302,
    F14 = 303,
    F15 = 304,
    F16 = 305,
    F17 = 306,
    F18 = 307,
    F19 = 308,
    F20 = 309,
    F21 = 310,
    F22 = 311,
    F23 = 312,
    F24 = 313,
    F25 = 314,

    // Клавиши модификаторы
    LeftShift = 340,
    LeftControl = 341,
    LeftAlt = 342,
    LeftSuper = 343,
    RightShift = 344,
    RightControl = 345,
    RightAlt = 346,
    RightSuper = 347,

    // Клавиши на numpad
    KP0 = 320,
    KP1 = 321,
    KP2 = 322,
    KP3 = 323,
    KP4 = 324,
    KP5 = 325,
    KP6 = 326,
    KP7 = 327,
    KP8 = 328,
    KP9 = 329,
    KPDecimal = 330,
    KPDivide = 331,
    KPMultiply = 332,
    KPSubtract = 333,
    KPAdd = 334,
    KPEnter = 335,
    KPEqual = 336,

    Menu = 348,
    Last = Menu
};

// Модификаторы (битовые флаги)
enum class KeyMod : uint8_t
{
    None = 0,
    Shift = 1 << 0,     // 1
    Control = 1 << 1,   // 2
    Alt = 1 << 2,       // 4
    Super = 1 << 3,     // 8
    CapsLock = 1 << 4,  // 16
    NumLock = 1 << 5    // 32
};

inline KeyMod operator|(KeyMod a, KeyMod b)
{
    return static_cast<KeyMod>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline bool operator&(KeyMod a, KeyMod b)
{
    return (static_cast<uint8_t>(a) & static_cast<uint8_t>(b)) != 0;
}

// Конвертация Win32 VK кодов в Key
#ifdef _WIN32
inline Key Win32ToRPKey(int vkCode)
{
    // Буквы A-Z
    if (vkCode >= 'A' && vkCode <= 'Z') return static_cast<Key>(vkCode);

    // Цифры 0-9
    if (vkCode >= '0' && vkCode <= '9') return static_cast<Key>(vkCode);

    // Функциональные клавиши F1-F24
    if (vkCode >= VK_F1 && vkCode <= VK_F24) return static_cast<Key>(290 + (vkCode - VK_F1));

    switch (vkCode)
    {
        case VK_SPACE: return Key::Space;
        case VK_OEM_7: return Key::Apostrophe;
        case VK_OEM_COMMA: return Key::Comma;
        case VK_OEM_MINUS: return Key::Minus;
        case VK_OEM_PERIOD: return Key::Period;
        case VK_OEM_2: return Key::Slash;
        case VK_OEM_1: return Key::Semicolon;
        case VK_OEM_PLUS: return Key::Equal;
        case VK_OEM_4: return Key::LeftBracket;
        case VK_OEM_5: return Key::Backslash;
        case VK_OEM_6: return Key::RightBracket;
        case VK_OEM_3: return Key::GraveAccent;

        case VK_ESCAPE: return Key::Escape;
        case VK_RETURN: return Key::Enter;
        case VK_TAB: return Key::Tab;
        case VK_BACK: return Key::Backspace;
        case VK_INSERT: return Key::Insert;
        case VK_DELETE: return Key::Delete;
        case VK_RIGHT: return Key::Right;
        case VK_LEFT: return Key::Left;
        case VK_DOWN: return Key::Down;
        case VK_UP: return Key::Up;
        case VK_PRIOR: return Key::PageUp;
        case VK_NEXT: return Key::PageDown;
        case VK_HOME: return Key::Home;
        case VK_END: return Key::End;
        case VK_CAPITAL: return Key::CapsLock;
        case VK_SCROLL: return Key::ScrollLock;
        case VK_NUMLOCK: return Key::NumLock;
        case VK_SNAPSHOT: return Key::PrintScreen;
        case VK_PAUSE: return Key::Pause;

        case VK_LSHIFT: return Key::LeftShift;
        case VK_LCONTROL: return Key::LeftControl;
        case VK_LMENU: return Key::LeftAlt;
        case VK_LWIN: return Key::LeftSuper;
        case VK_RSHIFT: return Key::RightShift;
        case VK_RCONTROL: return Key::RightControl;
        case VK_RMENU: return Key::RightAlt;
        case VK_RWIN: return Key::RightSuper;

        case VK_NUMPAD0: return Key::KP0;
        case VK_NUMPAD1: return Key::KP1;
        case VK_NUMPAD2: return Key::KP2;
        case VK_NUMPAD3: return Key::KP3;
        case VK_NUMPAD4: return Key::KP4;
        case VK_NUMPAD5: return Key::KP5;
        case VK_NUMPAD6: return Key::KP6;
        case VK_NUMPAD7: return Key::KP7;
        case VK_NUMPAD8: return Key::KP8;
        case VK_NUMPAD9: return Key::KP9;
        case VK_DECIMAL: return Key::KPDecimal;
        case VK_DIVIDE: return Key::KPDivide;
        case VK_MULTIPLY: return Key::KPMultiply;
        case VK_SUBTRACT: return Key::KPSubtract;
        case VK_ADD: return Key::KPAdd;

        default: return Key::Last;
    }
}

inline int RPToWin32Key(Key rpKey)
{
    int key = static_cast<int>(rpKey);

    if (key >= 32 && key <= 96 && key != 91 && key != 92 && key != 93) return key;
    if (key >= 65 && key <= 90) return key;

    switch (rpKey)
    {
        case Key::Space: return VK_SPACE;
        case Key::Apostrophe: return VK_OEM_7;
        case Key::Comma: return VK_OEM_COMMA;
        case Key::Minus: return VK_OEM_MINUS;
        case Key::Period: return VK_OEM_PERIOD;
        case Key::Slash: return VK_OEM_2;
        case Key::Semicolon: return VK_OEM_1;
        case Key::Equal: return VK_OEM_PLUS;
        case Key::LeftBracket: return VK_OEM_4;
        case Key::Backslash: return VK_OEM_5;
        case Key::RightBracket: return VK_OEM_6;
        case Key::GraveAccent: return VK_OEM_3;

        case Key::Escape: return VK_ESCAPE;
        case Key::Enter: return VK_RETURN;
        case Key::Tab: return VK_TAB;
        case Key::Backspace: return VK_BACK;
        case Key::Insert: return VK_INSERT;
        case Key::Delete: return VK_DELETE;
        case Key::Right: return VK_RIGHT;
        case Key::Left: return VK_LEFT;
        case Key::Down: return VK_DOWN;
        case Key::Up: return VK_UP;
        case Key::PageUp: return VK_PRIOR;
        case Key::PageDown: return VK_NEXT;
        case Key::Home: return VK_HOME;
        case Key::End: return VK_END;
        case Key::CapsLock: return VK_CAPITAL;
        case Key::ScrollLock: return VK_SCROLL;
        case Key::NumLock: return VK_NUMLOCK;
        case Key::PrintScreen: return VK_SNAPSHOT;
        case Key::Pause: return VK_PAUSE;

        case Key::LeftShift: return VK_LSHIFT;
        case Key::LeftControl: return VK_LCONTROL;
        case Key::LeftAlt: return VK_LMENU;
        case Key::LeftSuper: return VK_LWIN;
        case Key::RightShift: return VK_RSHIFT;
        case Key::RightControl: return VK_RCONTROL;
        case Key::RightAlt: return VK_RMENU;
        case Key::RightSuper: return VK_RWIN;

        case Key::KP0: return VK_NUMPAD0;
        case Key::KP1: return VK_NUMPAD1;
        case Key::KP2: return VK_NUMPAD2;
        case Key::KP3: return VK_NUMPAD3;
        case Key::KP4: return VK_NUMPAD4;
        case Key::KP5: return VK_NUMPAD5;
        case Key::KP6: return VK_NUMPAD6;
        case Key::KP7: return VK_NUMPAD7;
        case Key::KP8: return VK_NUMPAD8;
        case Key::KP9: return VK_NUMPAD9;
        case Key::KPDecimal: return VK_DECIMAL;
        case Key::KPDivide: return VK_DIVIDE;
        case Key::KPMultiply: return VK_MULTIPLY;
        case Key::KPSubtract: return VK_SUBTRACT;
        case Key::KPAdd: return VK_ADD;

        default: return 0;
    }
}
#endif  // _WIN32

// Функции для конвертации (GLFW)
inline Key toRPKey(int glfwKey)
{
    return static_cast<Key>(glfwKey);
}

inline int toGLFWKey(Key rpKey)
{
    return static_cast<int>(rpKey);
}

// Удобные alias для действий
namespace KeyAction
{
constexpr int Release = 0;
constexpr int Press = 1;
constexpr int Repeat = 2;
}  // namespace KeyAction

// Mouse buttons
constexpr int MOUSE_BUTTON_LEFT = 0;
constexpr int MOUSE_BUTTON_RIGHT = 1;
constexpr int MOUSE_BUTTON_MIDDLE = 2;
constexpr int MOUSE_BUTTON_4 = 3;
constexpr int MOUSE_BUTTON_5 = 4;
constexpr int MOUSE_BUTTON_6 = 5;
constexpr int MOUSE_BUTTON_7 = 6;
constexpr int MOUSE_BUTTON_8 = 7;

}  // namespace RPE