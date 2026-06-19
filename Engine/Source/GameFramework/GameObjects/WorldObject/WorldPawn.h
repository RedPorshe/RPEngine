#pragma once
#include "Core/SystemObject.h"
#include "WorldActor.h"
namespace RPE
{
class IController;

class WPawn : public WActor  // aka APawn
{
    CHUDDO_DECLARE_CLASS(WPawn, WActor);

public:
    WPawn(const std::string& inDisplayName = "WorldPawnObject", CObject* inOwner = nullptr);
    virtual ~WPawn();

    class WInputComponent* getInputComponent();

    void Tick(float DeltaTime) override;
    void EndPlay() override;

    void BeginPlay() override;
    virtual void onPossess(IController* controller);
    class PlayerController* getPlayerController() const;
    IController* getController() const;
    void unPossess();

protected:
    virtual void setupInputBindings(class WInputComponent* inputComponent);
    void addMovementInput(const FVector& WorldDirection, float ScaleValue, bool bForce = false);
    IController* m_controller = nullptr;
    class WInputComponent* m_inputComponent = nullptr;
};

}  // namespace RPE
namespace RPE
{
REGISTER_CLASS_FACTORY(WPawn);
}