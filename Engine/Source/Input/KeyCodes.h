// KeyCodes.h
#pragma once
#include <cstdint>

namespace RPE
{

// Собственные коды клавиш (независимые от GLFW)
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

// Функции для конвертации
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

constexpr int MOUSE_BUTTON_LEFT = 0;
constexpr int MOUSE_BUTTON_RIGHT = 1;
constexpr int MOUSE_BUTTON_MIDDLE = 2;
constexpr int MOUSE_BUTTON_4 = 3;
constexpr int MOUSE_BUTTON_5 = 4;
constexpr int MOUSE_BUTTON_6 = 5;
constexpr int MOUSE_BUTTON_7 = 6;
constexpr int MOUSE_BUTTON_8 = 7;

}  // namespace RPE