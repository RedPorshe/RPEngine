#pragma once
#include "Core/SystemObject.h"
#include "WorldPawn.h"

namespace RPE
{
class WTransformComponent;

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
    WTransformComponent* m_Capsule = nullptr;

    void Jump();
    void StartJump();
    void EndJump();
    bool bisJumping{false};
};

}  // namespace RPE
namespace RPE
{
REGISTER_CLASS_FACTORY(WCharacter);
}