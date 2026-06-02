#include "InputManager.h"
#include "Log/Log.h"
#include "Event/InputEvent.h"
#include "GameFrameWork/Controller/PlayerController.h"
#include <GLFW/glfw3.h>

DEFINE_LOG_CATEGORY_STATIC(InputManagerLog)

using namespace RPE;

InputManager::InputManager()
{
    m_activeController = nullptr;
    m_firstMouse = true;
    m_mouseDeltaX = 0.0;
    m_mouseDeltaY = 0.0;
    m_scrollOffset = 0.0;
    m_mouseX = 0.0;
    m_mouseY = 0.0;
    m_lastMouseX = 0.0;
    m_lastMouseY = 0.0;
}

InputManager::~InputManager() {}

void InputManager::update()
{
    m_previousKeys = m_currentKeys;
    m_previousMouseButtons = m_currentMouseButtons;

    m_mouseDeltaX = 0.0;
    m_mouseDeltaY = 0.0;
    m_scrollOffset = 0.0;
}

bool InputManager::isKeyPressed(Key key) const
{
    auto it = m_currentKeys.find(key);

    return it != m_currentKeys.end() ? it->second : false;
}

bool InputManager::isKeyJustPressed(Key key) const
{
    bool current = isKeyPressed(key);
    auto prevIt = m_previousKeys.find(key);
    bool previous = (prevIt != m_previousKeys.end()) && prevIt->second;
    return current && !previous;
}

bool InputManager::isMouseButtonPressed(int button) const
{
    auto it = m_currentMouseButtons.find(button);
    return it != m_currentMouseButtons.end() && it->second;
}

bool InputManager::isMouseButtonJustPressed(int button) const
{
    bool current = isMouseButtonPressed(button);
    auto prevIt = m_previousMouseButtons.find(button);
    bool previous = (prevIt != m_previousMouseButtons.end()) && prevIt->second;
    return current && !previous;
}

double InputManager::getMouseX() const
{
    return m_mouseX;
}

double InputManager::getMouseY() const
{
    return m_mouseY;
}

double InputManager::getMouseDeltaX() const
{
    return m_mouseDeltaX;
}

double InputManager::getMouseDeltaY() const
{
    return m_mouseDeltaY;
}

void InputManager::resetMouseState()
{
    m_firstMouse = true;
    m_mouseDeltaX = 0.0;
    m_mouseDeltaY = 0.0;
}

void InputManager::processInput(const InputEvent& event)
{
    switch (event.type)
    {
        case EventType::KeyPress:
        {
            if (std::holds_alternative<KeyData>(event.data))
            {
                const auto& keydata = std::get<KeyData>(event.data);
                if (m_activeController)
                {
                    m_activeController->onKeyPress(keydata.key, keydata.scancode, keydata.action, keydata.mods);
                }
                else
                {
                    handleKeyEvent(keydata.key, keydata.scancode, keydata.action, keydata.mods);
                }
            }
            break;
        }
        case EventType::MouseMove:
        {
            if (std::holds_alternative<MouseMoveData>(event.data))
            {
                const auto& data = std::get<MouseMoveData>(event.data);
                if (m_activeController)
                {
                    m_activeController->onMouseMove(data.x, data.y);
                }
                else
                {
                    handleMouseMoveEvent(data.x, data.y);
                }
            }
            break;
        }
        case EventType::MouseButton:
        {
            if (std::holds_alternative<MouseButtonData>(event.data))
            {
                const auto& data = std::get<MouseButtonData>(event.data);
                if (m_activeController)
                {
                    m_activeController->onMouseButton(data.button, data.action, data.mods, data.x, data.y);
                }
                else
                {
                    handleMouseButtonEvent(data.button, data.action, data.mods, data.x, data.y);
                }
            }
            break;
        }
        case EventType::MouseScroll:
        {
            if (std::holds_alternative<ScrollData>(event.data))
            {
                const auto& data = std::get<ScrollData>(event.data);
                if (m_activeController)
                {
                    m_activeController->onMouseScroll(data.xoffset, data.yoffset);
                }
                else
                {
                    handleMouseScrollEvent(data.xoffset, data.yoffset);
                }
            }
            break;
        }
        default: break;
    }
}

KeyMod InputManager::getKeyMods(int glfwMods) const
{
    KeyMod mods = KeyMod::None;
    if (glfwMods & GLFW_MOD_SHIFT) mods = mods | KeyMod::Shift;
    if (glfwMods & GLFW_MOD_CONTROL) mods = mods | KeyMod::Control;
    if (glfwMods & GLFW_MOD_ALT) mods = mods | KeyMod::Alt;
    if (glfwMods & GLFW_MOD_SUPER) mods = mods | KeyMod::Super;
    if (glfwMods & GLFW_MOD_CAPS_LOCK) mods = mods | KeyMod::CapsLock;
    if (glfwMods & GLFW_MOD_NUM_LOCK) mods = mods | KeyMod::NumLock;
    return mods;
}

std::string InputManager::getKeyName(Key key) const
{
    switch (key)
    {
        // Буквы
        case Key::Space: return "Space";
        case Key::A: return "A";
        case Key::B: return "B";
        case Key::C: return "C";
        case Key::D: return "D";
        case Key::E: return "E";
        case Key::F: return "F";
        case Key::G: return "G";
        case Key::H: return "H";
        case Key::I: return "I";
        case Key::J: return "J";
        case Key::K: return "K";
        case Key::L: return "L";
        case Key::M: return "M";
        case Key::N: return "N";
        case Key::O: return "O";
        case Key::P: return "P";
        case Key::Q: return "Q";
        case Key::R: return "R";
        case Key::S: return "S";
        case Key::T: return "T";
        case Key::U: return "U";
        case Key::V: return "V";
        case Key::W: return "W";
        case Key::X: return "X";
        case Key::Y: return "Y";
        case Key::Z: return "Z";

        // Цифры
        case Key::Digit0: return "0";
        case Key::Digit1: return "1";
        case Key::Digit2: return "2";
        case Key::Digit3: return "3";
        case Key::Digit4: return "4";
        case Key::Digit5: return "5";
        case Key::Digit6: return "6";
        case Key::Digit7: return "7";
        case Key::Digit8: return "8";
        case Key::Digit9: return "9";

        // Функциональные
        case Key::Escape: return "Escape";
        case Key::Enter: return "Enter";
        case Key::Tab: return "Tab";
        case Key::Backspace: return "Backspace";
        case Key::Insert: return "Insert";
        case Key::Delete: return "Delete";
        case Key::Right: return "Right";
        case Key::Left: return "Left";
        case Key::Down: return "Down";
        case Key::Up: return "Up";
        case Key::PageUp: return "PageUp";
        case Key::PageDown: return "PageDown";
        case Key::Home: return "Home";
        case Key::End: return "End";
        case Key::CapsLock: return "Caps Lock";
        case Key::GraveAccent: return "~";
        case Key::Backslash: return "\\";

        // F-клавиши
        case Key::F1: return "F1";
        case Key::F2: return "F2";
        case Key::F3: return "F3";
        case Key::F4: return "F4";
        case Key::F5: return "F5";
        case Key::F6: return "F6";
        case Key::F7: return "F7";
        case Key::F8: return "F8";
        case Key::F9: return "F9";
        case Key::F10: return "F10";
        case Key::F11: return "F11";
        case Key::F12: return "F12";

        // Модификаторы
        case Key::LeftShift: return "Left Shift";
        case Key::LeftControl: return "Left Ctrl";
        case Key::LeftAlt: return "Left Alt";
        case Key::LeftSuper: return "Left Win";
        case Key::RightShift: return "Right Shift";
        case Key::RightControl: return "Right Ctrl";
        case Key::RightAlt: return "Right Alt";
        case Key::RightSuper: return "Right Win";

        // Numpad
        case Key::KP0: return "Num 0";
        case Key::KP1: return "Num 1";
        case Key::KP2: return "Num 2";
        case Key::KP3: return "Num 3";
        case Key::KP4: return "Num 4";
        case Key::KP5: return "Num 5";
        case Key::KP6: return "Num 6";
        case Key::KP7: return "Num 7";
        case Key::KP8: return "Num 8";
        case Key::KP9: return "Num 9";
        case Key::KPDecimal: return "Num .";
        case Key::KPDivide: return "Num /";
        case Key::KPMultiply: return "Num *";
        case Key::KPSubtract: return "Num -";
        case Key::KPAdd: return "Num +";
        case Key::KPEnter: return "Num Enter";

        case Key::Apostrophe: return "'";
        case Key::Comma: return ",";
        case Key::Minus: return "-";
        case Key::Period: return ".";
        case Key::Slash: return "/";
        case Key::Semicolon: return ";";
        case Key::Equal: return "=";
        case Key::LeftBracket: return "[";
        case Key::RightBracket: return "]";
        case Key::ScrollLock: return "Scroll Lock";
        case Key::NumLock: return "Num Lock";
        case Key::PrintScreen: return "Print Screen";
        case Key::Pause: return "Pause";
        case Key::Menu: return "Menu";
        case Key::KPEqual: return "Num =";
        default: return "Unknown";
    }
}

void InputManager::handleKeyEvent(int key, int scancode, int action, int mods)
{
    Key rpKey = toRPKey(key);
    bool isPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    m_currentKeys[rpKey] = isPressed;
    if (isPressed && action == GLFW_PRESS)
    {
        std::string displayName = getKeyName(rpKey);
        KeyMod keyMods = getKeyMods(mods);

        bool isModifierKey =
            (rpKey == Key::LeftShift || rpKey == Key::RightShift || rpKey == Key::LeftControl || rpKey == Key::RightControl ||
                rpKey == Key::LeftAlt || rpKey == Key::RightAlt || rpKey == Key::LeftSuper || rpKey == Key::RightSuper);

        std::string modStr;
        if (keyMods & KeyMod::Shift && rpKey != Key::LeftShift && rpKey != Key::RightShift) modStr += "Shift+";
        if (keyMods & KeyMod::Control && rpKey != Key::LeftControl && rpKey != Key::RightControl) modStr += "Ctrl+";
        if (keyMods & KeyMod::Alt && rpKey != Key::LeftAlt && rpKey != Key::RightAlt) modStr += "Alt+";
        if (keyMods & KeyMod::Super && rpKey != Key::LeftSuper && rpKey != Key::RightSuper) modStr += "Super+";
        ;

        if (!modStr.empty())
        {
            RP_LOG(InputManagerLog, Display, "Key Pressed: {}{} (Scancode: {})", modStr, displayName, scancode);
        }
        else
        {
            RP_LOG(InputManagerLog, Display, "Key Pressed: {} (Scancode: {})", displayName, scancode);
        }

        if (rpKey == Key::Escape && m_exitCallback)
        {
            m_exitCallback();
        }

        if (m_activeController)
        {
            m_activeController->onKeyPress(key, scancode, action, mods);
        }
    }
}

void RPE::InputManager::handleMouseMoveEvent(double x, double y)
{
    if (m_firstMouse)
    {
        m_lastMouseX = x;
        m_lastMouseY = y;
        m_firstMouse = false;
    }

    m_mouseDeltaX = x - m_lastMouseX;
    m_mouseDeltaY = y - m_lastMouseY;

    m_mouseX = x;
    m_mouseY = y;
    m_lastMouseX = x;
    m_lastMouseY = y;
    if (m_activeController)
    {
        m_activeController->onMouseMove(x, y);
    }
}

void InputManager::handleMouseButtonEvent(int button, int action, int mods, double x, double y)
{
    bool isPressed = (action == GLFW_PRESS);
    m_currentMouseButtons[button] = isPressed;

    if (isPressed)
    {
        if (m_activeController)
        {
            m_activeController->onMouseButton(button, action, mods, x, y);
        }
    }
}

void InputManager::handleMouseScrollEvent(double xoffset, double yoffset)
{
    m_scrollOffset = yoffset;
    RP_LOG(InputManagerLog, Display, "Mouse Scroll: {:.1f}", yoffset);
    if (m_activeController)
    {
        m_activeController->onMouseScroll(xoffset, yoffset);
    }
}

void RPE::InputManager::setActiveController(std::shared_ptr<IController> controller)
{
    if (controller)
    {
        m_activeController = controller;
        RP_LOG(InputManagerLog, Display, "Active controller set: {}", controller->name());
    }
    else
    {
        RP_LOG(InputManagerLog, Display, "Active controller cleared");
    }
}

std::shared_ptr<IController> InputManager::getPlayerController() const
{
    return m_activeController;
}
