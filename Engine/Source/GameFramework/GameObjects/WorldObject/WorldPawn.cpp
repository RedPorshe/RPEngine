#include "WorldPawn.h"
#include "Input/KeyCodes.h"
#include "GameFrameWork/GameObjects/Components/InputComponent.h"
#include "Log/Log.h"
#include "Core/ObjectFactory.h"

#include "Core/Engine.h"

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
    Super::Tick(DeltaTime);
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
        m_inputComponent->bindAction(Key::Escape, ActionType::Press, [this]() { Quit(); });
    }
}

void WPawn::moveRight(float value)
{
    if (value == 0.0f) return;

    RP_LOG(WPawnLog, Display, "Moving Right: {:.2f}", value);
    RP_LOG(WPawnLog, Display, "m_deltaTime = : {:.2f}", m_deltaTime);
    RP_LOG(WPawnLog, Display, "start location: {:.10f},{:.10f},{:.10f}", getActorLocation().x, getActorLocation().y, getActorLocation().z);
    FVector Forward = FVector::Right();
    float forfawdSpeed = .300f;
    float offset = (value * forfawdSpeed);
    MoveActor(Forward, offset);
}

void WPawn::moveForward(float value)
{
    if (value == 0.0f) return;
    RP_LOG(WPawnLog, Display, "Moving Forward: {:.2f}", value);
    RP_LOG(WPawnLog, Display, "start location: {:.10f},{:.10f},{:.10f}", getActorLocation().x, getActorLocation().y, getActorLocation().z);
    std::cout << getActorLocation() << "\n";
    FVector Forward = FVector::Forward();
    float forfawdSpeed = .3f;
    float offset = (value * forfawdSpeed);
    MoveActor(Forward, offset);
}

void WPawn::look(float value) {}

void WPawn::lookUp(float value) {}

void WPawn::jump()
{
    RP_LOG(WPawnLog, Display, "Jumping!");
}

void RPE::WPawn::Quit()
{
    RP_LOG(WPawnLog, Display, "Request Exit from Pawn!");
    Engine::Get().requestExit();
}
