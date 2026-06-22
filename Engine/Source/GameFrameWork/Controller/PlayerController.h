#pragma once
#include "Controller.h"
#include "Input/KeyCodes.h"
#include <functional>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include "GameFrameWork/GameObjects/WorldObject/WorldActor.h"

namespace RPE
{
class WPawn;
class WInputComponent;

class PlayerController : public WActor, public IController
{
    CHUDDO_DECLARE_CLASS(PlayerController, WActor);

public:
    PlayerController(const std::string& inDisplayName = "PlayerController", CObject* inOwner = nullptr);
    ~PlayerController() override;
    void BeginPlay() override;
    void Tick(float deltaTime) override;
    void onKeyPress(int key, int scancode, int action, int mods) override;
    void onMouseMove(double x, double y) override;
    void onMouseButton(int button, int action, int mods, double x, double y) override;
    void onMouseScroll(double xoffset, double yoffset) override;
    std::string name() const override;
    void setName(const std::string& newName) override;

    WInputComponent* getPlayerInputComponent() const;
    WPawn* getControlledObject() const;
    void possess(WPawn* pawn) override;
    void setControlledObject(class WPawn* object);
    void unPossess(WPawn* pawn) override;
    void unPossess() override;

private:
    void update(float deltaTime);
    void handleKeyPress(int key, int scancode, int action, int mods);
    void handleMouseMove(double x, double y);
    void handleMouseButton(int button, int action, int mods, double x, double y);
    void handleMouseScroll(double xoffset, double yoffset);
    void clearInputBindings();
    std::string controllerName;

    void test();
};

REGISTER_CLASS_FACTORY(PlayerController);

}  // namespace RPE