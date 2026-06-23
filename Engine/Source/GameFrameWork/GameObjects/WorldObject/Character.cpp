#include "Character.h"
#include "GameFrameWork/GameObjects/Components/InputComponent.h"
#include "GameFrameWork/Collision/Capsule.h"
#include "GameFrameWork/GameObjects/Components/MeshComponent.h"
#include "Render/RHI/StaticMesh.h"
#include "Core/Engine.h"
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(CharacterLog);

using namespace RPE;

WCharacter::WCharacter(const std::string& inDisplayName, CObject* inOwner)  //
    : Super(inDisplayName, inOwner)                                         //
{
    m_Capsule = addComponent<CapsuleComponent>("Capsule" + GetName());
    m_Capsule->setHalfHeight(9.8f);
    m_Capsule->setRadius(0.38f);
    setRootComponent(m_Capsule);
    auto mesh = addComponent<MeshComponent>("meshcomp");
    auto model = new StaticMesh();
    model->SetIsLoaded(true);
    mesh->setMesh(model);
    mesh->setVisible(true);
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
    auto location = getActorLocation();
    float x = location.x;
    float y = location.y;
    float z = location.z;
    RP_LOG(CharacterLog, Display, "Character {} End Play position is {},{},{}", GetName(), x, y, z);
}

void WCharacter::BeginPlay()
{
    Super::BeginPlay();
    auto location = getActorLocation();
    float x = location.x;
    float y = location.y;
    float z = location.z;

    RP_LOG(CharacterLog, Display, "Character {} start position is {},{},{}", GetName(), x, y, z);
}

void WCharacter::setupInputBindings(WInputComponent* inputComponent)
{
    Super::setupInputBindings(inputComponent);
    if (getInputComponent())
    {
        auto comp = getInputComponent();
        comp->bindAxis(Key::W, Key::S, [this](float value) { MoveForward(value); });
        comp->bindAction(Key::Space, ActionType::Press, [this]() { Jump(); });
        comp->bindAction(Key::Escape, ActionType::Press, [this]() { Engine::Get().requestExit(); });
    }
}

void WCharacter::MoveForward(float val)
{
    if (CEMath::IsZero(val)) return;
    auto Forward = getActorForwardVector();
    float movementspeed = 300;
    float offset = movementspeed * m_deltaTime * val;
    MoveActor(Forward, offset);
}

void WCharacter::Jump()
{
    if (!bisJumping)
    {
        StartJump();
        EndJump();
    }
}

void WCharacter::StartJump()
{
    bisJumping = true;

    RP_LOG(CharacterLog, Display, "{} start Jump", GetName());
}

void WCharacter::EndJump()
{
    bisJumping = false;
    RP_LOG(CharacterLog, Display, "{} stop Jump", GetName());
}
