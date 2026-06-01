#include "PlayerController.h"
#include "Log/Log.h"

using namespace RPE;

DEFINE_LOG_CATEGORY_STATIC(PlayerControllerLog);

PlayerController::PlayerController() : controllerName("Player Controller") {}

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

void RPE::PlayerController::setName(const std::string& newName)
{
    controllerName = newName;
}

void PlayerController::handleKeyPress(int key, int scancode, int action, int mods)
{
    RP_LOG(PlayerControllerLog, Display, "Key Pressed: {} (Scancode: {})", key, scancode);
}

void PlayerController::handleMouseMove(double x, double y)
{
    RP_LOG(PlayerControllerLog, Display, "Mouse Moved: ({}, {})", x, y);
}

void PlayerController::handleMouseButton(int button, int action, int mods, double x, double y)
{
    RP_LOG(PlayerControllerLog, Display, "Mouse Button {}: ({}, {})", action, x, y);
}

void PlayerController::handleMouseScroll(double xoffset, double yoffset)
{
    RP_LOG(PlayerControllerLog, Display, "Mouse Scrolled: ({}, {})", xoffset, yoffset);
}
