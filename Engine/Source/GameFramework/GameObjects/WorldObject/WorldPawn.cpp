#include "WorldPawn.h"
#include "Input/KeyCodes.h"
#include "Core/GameInstance.h"
#include "GameFrameWork/GameObjects/Components/InputComponent.h"
#include "../../World/World.h"
#include "../../World/Level.h"
#include "../../Controller/PlayerController.h"
#include "Log/Log.h"
#include "Core/ObjectFactory.h"

#include "Core/Engine.h"

using namespace RPE;

DEFINE_LOG_CATEGORY_STATIC(WPawnLog);

WPawn::WPawn(const std::string& inDisplayName, CObject* inOwner)  //
    : Super(inDisplayName, inOwner)
{
    SetMovableState(EMovableState::Dynamic);
}

WPawn::~WPawn()
{
    m_inputComponent = nullptr;
}

WInputComponent* WPawn::getInputComponent()
{
    return m_inputComponent;
}

void WPawn::Tick(float DeltaTime)
{
    static int testCount = 0;
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

void RPE::WPawn::onPossess(IController* controller)
{
    if (!controller) return;
    m_controller = controller;
    const auto inputs = controller->getInputComponents();
    auto it = inputs.find(GetName());
    if (it != inputs.end())
    {
        auto inputComp = it->second;
        setupInputBindings(inputComp);
    }
}

PlayerController* WPawn::getPlayerController() const
{
    return dynamic_cast<PlayerController*>(m_controller);
}

IController* RPE::WPawn::getController() const
{
    return m_controller ? m_controller : nullptr;  // stub in nullptr for AI controller;
}

void RPE::WPawn::unPosses()
{
    if (m_controller)
    {
        m_controller = nullptr;
        m_inputComponent = nullptr;  // clear binding doin in controller
    }
}

void WPawn::setupInputBindings(WInputComponent* inputComponent)
{
    if (!inputComponent) return;
    m_inputComponent = inputComponent;
    m_inputComponent->clearContext();
}
