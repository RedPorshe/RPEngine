#pragma once
#include "Input/KeyCodes.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "WorldActorComponent.h"

namespace RPE
{

enum class ActionType
{
    Press,
    Release,
    Repeat,
    Any
};

class WInputComponent : public WActorComponent
{
    CHUDDO_DECLARE_CLASS(WInputComponent, WActorComponent);

public:
    WInputComponent(const std::string& inDisplayName = "Input Component", CObject* inOwner = nullptr);
    virtual ~WInputComponent();
    void tick(float deltaTime) override;
    void onDestroy() override;

    using ActionCallback = std::function<void()>;
    using AxisCallback = std::function<void(float)>;
    using MouseMoveCallback = std::function<void(float, float)>;
    using MouseScrollCallback = std::function<void(float)>;

    // Привязка действий
    void bindAction(Key key, ActionType type, ActionCallback callback);

    // Привязка осей (WASD, стрелки) - теперь обрабатываются в tick
    void bindAxis(Key positive, Key negative, AxisCallback callback, float deadZone = 0.1f);

    // Привязка мыши
    void bindMouseMove(MouseMoveCallback callback);
    void bindMouseScroll(MouseScrollCallback callback);


    void processMouseMove(float deltaX, float deltaY);
    void processMouseScroll(float delta);

    // Контексты ввода
    void setContext(const std::string& context);
    void pushContext(const std::string& context);
    void popContext();
    void clearContext();


    float getAxisValue(Key positive, Key negative) const;

private:
    void processInput(float deltaTime);

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

  

    // Привязки
    std::vector<ActionBinding> m_activeActionBindings;
    std::vector<AxisBinding> m_activeAxisBindings;

    // Колбэки мыши
    MouseMoveCallback m_mouseMoveCallback;
    MouseScrollCallback m_mouseScrollCallback;

    // Контексты
    std::vector<std::string> m_contextStack;

    // Состояние мыши
    float m_mouseDeltaX = 0.0f;
    float m_mouseDeltaY = 0.0f;
    float m_mouseScrollDelta = 0.0f;
    bool m_hasMouseMove = false;
    bool m_hasMouseScroll = false;
};
}  // namespace RPE