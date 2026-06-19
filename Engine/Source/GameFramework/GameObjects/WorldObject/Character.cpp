#include "Character.h"
#include "../Components/InputComponent.h"
#include "Core/Engine.h"
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(CharacterLog);

using namespace RPE;

WCharacter::WCharacter(const std::string& inDisplayName, CObject* inOwner)  //
    : Super(inDisplayName, inOwner)                                         //
{
    m_Capsule = addComponent<WTransformComponent>("Capsule_Stub");
    setRootComponent(m_Capsule);

    bisJumping = false;
}

WCharacter::~WCharacter() {}

void WCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void WCharacter::EndPlay()
{
    Super::EndPlay();
}

void WCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void WCharacter::setupInputBindings(WInputComponent* inputComponent)
{
    Super::setupInputBindings(inputComponent);
    if (getInputComponent())
    {
        auto comp = getInputComponent();
        comp->bindAction(Key::Space, ActionType::Press, [this]() { Jump(); });
        comp->bindAction(Key::Escape, ActionType::Press, [this]() { Engine::Get().requestExit(); });
    }
}

void RPE::WCharacter::Jump()
{
    if (!bisJumping)
    {
        StartJump();
        EndJump();
    }
}

void RPE::WCharacter::StartJump()
{
    bisJumping = true;

    RP_LOG(CharacterLog, Display, "{} start Jump", GetName());
}

void RPE::WCharacter::EndJump()
{
    bisJumping = false;
    RP_LOG(CharacterLog, Display, "{} stop Jump", GetName());
}
