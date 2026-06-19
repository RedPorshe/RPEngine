#include "InputComponent.h"
#include "Log/Log.h"
#include "Core/Engine.h"
#include "../../../Input/InputManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogInputComponent)

using namespace RPE;

REGISTER_CLASS_FACTORY(WInputComponent);

WInputComponent::WInputComponent(const std::string& inDisplayName, CObject* inOwner) : Super(inDisplayName, inOwner) {}

WInputComponent::~WInputComponent() {}

void WInputComponent::tick(float deltaTime)
{
    Super::tick(deltaTime);

    processInput(deltaTime);

    m_hasMouseMove = false;
    m_hasMouseScroll = false;
}

void WInputComponent::processInput(float deltaTime)
{
    auto inputMgr = Engine::Get().getInputManager();

    for (const auto& binding : m_activeAxisBindings)
    {
        float value = getAxisValue(binding.positive, binding.negative);

        if (std::abs(value) <= binding.deadZone)
        {
            value = 0.0f;
        }

        if (binding.callback)
        {
            binding.callback(value);
        }
    }

    for (const auto& binding : m_activeActionBindings)
    {
        bool isPressed = inputMgr->isKeyPressed(binding.key);
        bool justPressed = inputMgr->isKeyJustPressed(binding.key);
        bool justReleased = inputMgr->isKeyJustReleased(binding.key);

        bool shouldTrigger = false;
        switch (binding.type)
        {
            case ActionType::Press: shouldTrigger = justPressed; break;
            case ActionType::Release: shouldTrigger = justReleased; break;
            case ActionType::Repeat: shouldTrigger = isPressed; break;
            case ActionType::Any: shouldTrigger = justPressed || justReleased; break;
        }

        if (shouldTrigger && binding.callback)
        {
            binding.callback();
        }
    }

    if (m_hasMouseMove && m_mouseMoveCallback)
    {
        m_mouseMoveCallback(m_mouseDeltaX, m_mouseDeltaY);
    }

    if (m_hasMouseScroll && m_mouseScrollCallback)
    {
        m_mouseScrollCallback(m_mouseScrollDelta);
    }
}

void WInputComponent::onDestroy()
{
    popContext();
    clearContext();
}

void WInputComponent::bindAction(Key key, ActionType type, ActionCallback callback)
{
    m_activeActionBindings.push_back({key, type, callback});
}

void WInputComponent::bindAxis(Key positive, Key negative, AxisCallback callback, float deadZone)
{
    m_activeAxisBindings.push_back({positive, negative, callback, deadZone});
}

void WInputComponent::bindMouseMove(MouseMoveCallback callback)
{
    m_mouseMoveCallback = callback;
}

void WInputComponent::bindMouseScroll(MouseScrollCallback callback)
{
    m_mouseScrollCallback = callback;
}

void WInputComponent::processMouseMove(float deltaX, float deltaY)
{
    m_mouseDeltaX = deltaX;
    m_mouseDeltaY = deltaY;
    m_hasMouseMove = true;
}

void WInputComponent::processMouseScroll(float delta)
{
    m_mouseScrollDelta = delta;
    m_hasMouseScroll = true;
}

float WInputComponent::getAxisValue(Key positive, Key negative) const
{
    auto inputMgr = Engine::Get().getInputManager();
    float value = 0.0f;
    if (inputMgr->isKeyPressed(positive)) value += 1.0f;
    if (inputMgr->isKeyPressed(negative)) value -= 1.0f;
    return value;
}

void WInputComponent::setContext(const std::string& context)
{
    auto it = std::find(m_contextStack.begin(), m_contextStack.end(), context);
    if (it == m_contextStack.end())
    {
        m_contextStack.push_back(context);
    }
    else
    {
        std::rotate(m_contextStack.begin(), it, it + 1);
    }
}

void WInputComponent::pushContext(const std::string& context)
{
    m_contextStack.push_back(context);
}

void WInputComponent::popContext()
{
    if (!m_contextStack.empty())
    {
        m_contextStack.pop_back();
    }
}

void WInputComponent::clearContext()
{
    m_contextStack.clear();

    m_activeActionBindings.clear();
    m_activeAxisBindings.clear();

    m_mouseMoveCallback = nullptr;
    m_mouseScrollCallback = nullptr;
}