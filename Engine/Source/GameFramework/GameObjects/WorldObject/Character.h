#pragma once
#include "Core/SystemObject.h"
#include "WorldPawn.h"

namespace RPE
{
class WTransformComponent;
class CapsuleComponent;
class CollisionComponent;

class WCharacter : public WPawn
{
    CHUDDO_DECLARE_CLASS(WCharacter, WPawn);

public:
    WCharacter(const std::string& inDisplayName = "WorldPawnObject", CObject* inOwner = nullptr);
    virtual ~WCharacter();

    void Tick(float DeltaTime) override;
    void EndPlay() override;
    void BeginPlay() override;
    bool isJumping() const { return bisJumping; }

protected:
    void setupInputBindings(class WInputComponent* inputComponent) override;

private:
    CapsuleComponent* m_Capsule = nullptr;  // stub
    void MoveForward(float val);
    void Jump();
    void StartJump();
    void EndJump();
    bool bisJumping{false};
    CollisionComponent* otherCollision = nullptr;
};

}  // namespace RPE
namespace RPE
{
REGISTER_CLASS_FACTORY(WCharacter);
}