#include "WorldPawn.h"
#include "Input/KeyCodes.h"
#include "GameFrameWork/GameObjects/Components/InputComponent.h"
#include "Log/Log.h"
#include "Core/ObjectFactory.h"

using namespace RPE;

DEFINE_LOG_CATEGORY_STATIC(WPawnLog);

WPawn::WPawn(const std::string& inDisplayName, CObject* inOwner) : Super(inDisplayName, inOwner)
{
    m_inputComponent = this->addComponent<WInputComponent>("Input");
    setupInputBindings();
}

WPawn::~WPawn()
{
    delete m_inputComponent;
}

WInputComponent* WPawn::getInputComponent()
{
    return m_inputComponent;
}

void WPawn::Tick(float DeltaTime)
{
    static int cccc = 0;
    if (cccc < 1)
    {
        RP_LOG(WPawnLog, Display, "Tick for {}", GetName());
        cccc++;
    }
}

void WPawn::EndPlay()
{
    RP_LOG(WPawnLog, Display, "End play for {}", GetName());
}

void WPawn::BeginPlay()
{
    RP_LOG(WPawnLog, Display, "Begin play for {}", GetName());
}

void WPawn::setupInputBindings()
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

void WPawn::moveRight(float value)
{
    RP_LOG(WPawnLog, Display, "Moving Right: {:.2f}", value);
}

void WPawn::moveForward(float value)
{
    RP_LOG(WPawnLog, Display, "Moving Forward: {:.2f}", value);
}

void WPawn::look(float value)
{
    RP_LOG(WPawnLog, Display, "Looking: {:.2f}", value);
}

void WPawn::lookUp(float value)
{
    RP_LOG(WPawnLog, Display, "Looking Up: {:.2f}", value);
}

void WPawn::jump()
{
    RP_LOG(WPawnLog, Display, "Jumping!");
}
