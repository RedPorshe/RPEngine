#include "InputComponent.h"
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(LogInputComponent)

using namespace RPE;

WInputComponent::WInputComponent(const std::string& inDisplayName, CObject* inOwner)  //
    : Super(inDisplayName, inOwner)
{
}

WInputComponent::~WInputComponent() {}

void WInputComponent::tick(float deltaTime)
{
    (void)deltaTime;
}

void WInputComponent::onDestroy()
{
    popContext();
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

void WInputComponent::processKey(Key key, int action)
{
    bool isPressed = (action == 1 || action == 2);  // Press или Repeat

    // Обработка осей
    for (const auto& binding : m_activeAxisBindings)
    {
        float value = 0.0f;
        if (key == binding.positive && isPressed) value = 1.0f;
        if (key == binding.negative && isPressed) value = -1.0f;

        // Apply dead zone
        if (std::abs(value) <= binding.deadZone)
        {
            value = 0.0f;
        }

        if (value != 0.0f && binding.callback)
        {
            binding.callback(value);
        }
    }

    // Обработка действий
    for (const auto& binding : m_activeActionBindings)
    {
        if (key == binding.key)
        {
            bool shouldTrigger = false;
            switch (binding.type)
            {
                case ActionType::Press: shouldTrigger = (action == 1); break;
                case ActionType::Release: shouldTrigger = (action == 0); break;
                case ActionType::Repeat: shouldTrigger = (action == 2); break;
                case ActionType::Any: shouldTrigger = true; break;
            }
            if (shouldTrigger && binding.callback)
            {
                binding.callback();
            }
        }
    }
}

void WInputComponent::processMouseMove(float deltaX, float deltaY)
{
    if (m_mouseMoveCallback)
    {
        m_mouseMoveCallback(deltaX, deltaY);
    }
}

void WInputComponent::processMouseScroll(float delta)
{
    if (m_mouseScrollCallback)
    {
        m_mouseScrollCallback(delta);
    }
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
