#pragma once
#include "Input/KeyCodes.h"
#include <functional>
#include <string>

namespace RPE
{

enum class ActionType
{
    Press,    // Однократное действие
    Release,  // Отпускание
    Repeat,   // Повтор (зажатая клавиша)
    Any       // Любое изменение
};

class InputComponent
{
public:
    using ActionCallback = std::function<void()>;
    using AxisCallback = std::function<void(float)>;
    using MouseMoveCallback = std::function<void(float, float)>;
    using MouseScrollCallback = std::function<void(float)>;

    // Привязка действий
    void bindAction(Key key, ActionType type, ActionCallback callback);

    // Привязка осей (WASD, стрелки)
    void bindAxis(Key positive, Key negative, AxisCallback callback, float deadZone = 0.1f);

    // Привязка мыши
    void bindMouseMove(MouseMoveCallback callback);
    void bindMouseScroll(MouseScrollCallback callback);

    // Обработка ввода
    void processKey(Key key, int action);
    void processMouseMove(float deltaX, float deltaY);
    void processMouseScroll(float delta);

    // Контексты ввода
    void setContext(const std::string& context);
    void pushContext(const std::string& context);
    void popContext();

private:
    struct ActionBinding
    {
        Key key;
        ActionType type;
        ActionCallback callback;
    };

    struct AxisBinding
    {
        Key positive;
        Key negative;
        AxisCallback callback;
        float deadZone = 0.1f;
    };

    std::vector<ActionBinding> m_activeActionBindings;
    std::vector<AxisBinding> m_activeAxisBindings;
    MouseMoveCallback m_mouseMoveCallback;
    MouseScrollCallback m_mouseScrollCallback;
    std::vector<std::string> m_contextStack;
    std::unordered_map<Key, bool> m_previousKeys;
    // Удалено problematic: std::unordered_map<AxisCallback, float> m_axisValues;
};
}  // namespace RPE