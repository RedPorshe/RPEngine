#include "PlayerController.h"
#include "Input/KeyCodes.h"
#include "GameFrameWork/GameObjects/Components/InputComponent.h"
#include "GameFrameWork/GameObjects/WorldObject/WorldPawn.h"
#include "Log/Log.h"

using namespace RPE;

DEFINE_LOG_CATEGORY_STATIC(PlayerControllerLog);

PlayerController::PlayerController(const std::string& inDisplayName, CObject* inOwner)
    : Super(inDisplayName, inOwner), controllerName(inDisplayName)
{
}

PlayerController::~PlayerController()
{
    unPossess();
}

void PlayerController::BeginPlay()
{
    Super::BeginPlay();
    removeComponent("Default_Root");
}

void RPE::PlayerController::Tick(float deltaTime)
{
    if (m_currentControlledInputComponent)
    {
        m_currentControlledInputComponent->tick(deltaTime);
    }
}

void PlayerController::onKeyPress(int key, int scancode, int action, int mods)
{
    handleKeyPress(key, scancode, action, mods);
}

void PlayerController::onMouseMove(double x, double y)
{
    handleMouseMove(x, y);
}

void PlayerController::onMouseButton(int button, int action, int mods, double x, double y)
{
    handleMouseButton(button, action, mods, x, y);
}

void PlayerController::onMouseScroll(double xoffset, double yoffset)
{
    handleMouseScroll(xoffset, yoffset);
}

std::string PlayerController::name() const
{
    return controllerName;
}

void PlayerController::setName(const std::string& newName)
{
    controllerName = newName;
}

WInputComponent* PlayerController::getPlayerInputComponent() const
{
    if (getControlledObject())
    {
        return getControlledObject()->getInputComponent();
    }
    return nullptr;
}

WPawn* PlayerController::getControlledObject() const
{
    return m_ControlledObject;
}

void PlayerController::possess(WPawn* pawn)
{
    if (!pawn) return;
    if (m_ControlledObject == pawn) return;
    if (m_ControlledObject)
    {
        m_ControlledObject->unPosses();
    }
    auto imputcomponent = AddSubObject<WInputComponent>(pawn->GetName() + "_Input");
    if (imputcomponent)
    {
        m_inputComponents[pawn->GetName()] = imputcomponent;
        setControlledObject(pawn);
        pawn->onPossess(this);
        m_currentControlledInputComponent = imputcomponent;
        RP_LOG(PlayerControllerLog, Display, "[{}] controller posses to {}", GetName(), pawn->GetName());
    }
}

void PlayerController::setControlledObject(WPawn* object)
{
    m_ControlledObject = object;
}

void RPE::PlayerController::unPossess(WPawn* pawn)
{
    if (!pawn) return;
    pawn->unPosses();
}

void RPE::PlayerController::unPossess()
{
    if (m_ControlledObject)
    {
        m_ControlledObject->unPosses();
        m_ControlledObject = nullptr;
    }
}

void PlayerController::update(float deltaTime) {}

void PlayerController::handleKeyPress(int key, int scancode, int action, int mods) {}

void PlayerController::handleMouseMove(double x, double y)
{
    if (auto* obj = getControlledObject())
    {
        obj->getInputComponent()->processMouseMove(static_cast<float>(x), static_cast<float>(y));
    }
}

void PlayerController::handleMouseButton(int button, int action, int mods, double x, double y) {}

void PlayerController::handleMouseScroll(double xoffset, double yoffset)
{

    if (auto* obj = getControlledObject())
    {
        obj->getInputComponent()->processMouseScroll(static_cast<float>(yoffset));
    }
}

void RPE::PlayerController::clearInputBindings()
{
    if (m_inputComponents.empty()) return;
    for (const auto& [name, object] : m_inputComponents)
    {
        RP_LOG(PlayerControllerLog, Display, "clearing input bindings for input component: {}", name);
        object->clearContext();
    }
}

void RPE::PlayerController::test()
{
    auto world = GetWorld();
}
