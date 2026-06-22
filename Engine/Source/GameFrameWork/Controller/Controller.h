#pragma once
#include <string>
#include "Core/Utility.h"
#include <unordered_map>

namespace RPE
{
class WInputComponent;
class WPawn;

class IController : public NonCopyable
{

public:
    virtual ~IController() = default;
    virtual void onKeyPress(int key, int scancode, int action, int mods) = 0;
    virtual void onMouseMove(double x, double y) = 0;
    virtual void onMouseButton(int button, int action, int mods, double x, double y) = 0;
    virtual void onMouseScroll(double xoffset, double yoffset) = 0;
    virtual std::string name() const = 0;
    virtual void setName(const std::string& newName) = 0;
    std::unordered_map<std::string, WInputComponent*>& getInputComponents() { return m_inputComponents; }
    virtual void possess(WPawn* pawn) = 0;
    virtual void unPossess(WPawn* pawn) = 0;
    virtual void unPossess() = 0;

protected:
    WInputComponent* m_currentControlledInputComponent = nullptr;
    std::unordered_map<std::string, WInputComponent*> m_inputComponents;
    WPawn* m_ControlledObject = nullptr;
};

}  // namespace RPE