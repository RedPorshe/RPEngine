#include "PlayerController.h"
#include "Input/KeyCodes.h"
#include "GameFramework/GameObjects/Components/InputComponent.h"
#include "GameFramework/GameObjects/WorldObject/WorldObject.h"
#include "Log/Log.h"

using namespace RPE;

DEFINE_LOG_CATEGORY_STATIC(PlayerControllerLog);

// ========== PlayerController ==========

PlayerController::PlayerController() : controllerName("Player Controller"), m_ControlledObject(nullptr) {}

PlayerController::~PlayerController() {}

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

InputComponent* PlayerController::getPlayerInputComponent() const
{
    if (getControlledObject())
    {
        return getControlledObject()->getInputComponent();
    }
    return nullptr;
}

WorldObject* PlayerController::getControlledObject() const
{
    return m_ControlledObject;
}

void PlayerController::setControlledObject(WorldObject* object)
{
    m_ControlledObject = object;
}

void PlayerController::update(float deltaTime) {}

void PlayerController::handleKeyPress(int key, int scancode, int action, int mods)
{
    // RP_LOG(PlayerControllerLog, Display, "Key Pressed: {} (Scancode: {})", key, scancode);
    if (auto* obj = getControlledObject())
    {
        obj->getInputComponent()->processKey(static_cast<Key>(key), action);
    }
}

void PlayerController::handleMouseMove(double x, double y)
{
    // RP_LOG(PlayerControllerLog, Display, "Mouse Moved: ({:.0f}, {:.0f})", x, y);
    if (auto* obj = getControlledObject())
    {
        obj->getInputComponent()->processMouseMove(static_cast<float>(x), static_cast<float>(y));
    }
}

void PlayerController::handleMouseButton(int button, int action, int mods, double x, double y)
{
    const char* actionStr = (action == 0) ? "Released" : "Pressed";
    if (auto obj = getControlledObject())
    {
        obj->getInputComponent()->processKey(static_cast<Key>(button), action);
    }
    // RP_LOG(PlayerControllerLog, Display, "Mouse Button {}: {} at ({:.0f}, {:.0f})", button, actionStr, x, y);
    // Можно добавить обработку мыши в InputComponent при необходимости
}

void PlayerController::handleMouseScroll(double xoffset, double yoffset)
{
    //  RP_LOG(PlayerControllerLog, Display, "Mouse Scrolled: ({:.1f}, {:.1f})", xoffset, yoffset);
    if (auto* obj = getControlledObject())
    {
        obj->getInputComponent()->processMouseScroll(static_cast<float>(yoffset));
    }
}

// ========== InputComponent ==========
