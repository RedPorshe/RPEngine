#include "WorldObject.h"
#include "Input/KeyCodes.h"
#include "GameFramework/GameObjects/Components/InputComponent.h"

using namespace RPE;

DEFINE_LOG_CATEGORY_STATIC(WorldObjectLog);

WorldObject::WorldObject()
{
    m_inputComponent = new InputComponent();
    setupInputBindings();
}

WorldObject::~WorldObject()
{
    delete m_inputComponent;
}

InputComponent* WorldObject::getInputComponent()
{
    return m_inputComponent;
}

void WorldObject::update(float deltaTime)
{
    // Здесь будет обновление позиции, физики и т.д.
    (void)deltaTime;
}

void WorldObject::setupInputBindings()
{
    if (m_inputComponent)
    {
        m_inputComponent->bindAxis(Key::W, Key::S, [this](float value) { moveForward(value); }, 0.1f);
        m_inputComponent->bindAxis(Key::A, Key::D, [this](float value) { moveRight(value); }, 0.1f);
        m_inputComponent->bindMouseMove(
            [this](float x, float y)
            {
                look(x);
                lookUp(y);
            });
        m_inputComponent->bindAction(Key::Space, ActionType::Press, [this]() { jump(); });
    }
}

void WorldObject::moveRight(float value)
{
    RP_LOG(WorldObjectLog, Display, "Moving Right: {:.2f}", value);
}

void WorldObject::moveForward(float value)
{
    RP_LOG(WorldObjectLog, Display, "Moving Forward: {:.2f}", value);
}

void WorldObject::look(float value)
{
    RP_LOG(WorldObjectLog, Display, "Looking: {:.2f}", value);
}

void WorldObject::lookUp(float value)
{
    RP_LOG(WorldObjectLog, Display, "Looking Up: {:.2f}", value);
}

void WorldObject::jump()
{
    RP_LOG(WorldObjectLog, Display, "Jumping!");
}