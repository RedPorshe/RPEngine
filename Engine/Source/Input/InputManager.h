#pragma once
#include "Core/Utility.h"
#include "KeyCodes.h"
#include <unordered_map>
#include <string>
#include <functional>
#include <memory>

namespace RPE
{
struct InputEvent;
class IController;

class InputManager : public NonCopyable
{
public:
    using ExitCallback = std::function<void()>;
    InputManager();
    virtual ~InputManager();
    void processInput(const InputEvent& event);

    void update();
    void setExitCallback(ExitCallback callback) { m_exitCallback = callback; }
    bool isKeyPressed(Key key) const;
    bool isKeyJustPressed(Key key) const;
    bool isMouseButtonPressed(int button) const;
    bool isMouseButtonJustPressed(int button) const;

    double getMouseX() const;
    double getMouseY() const;
    double getMouseDeltaX() const;
    double getMouseDeltaY() const;
    void resetMouseState();
    void setActiveController(IController* controller);
    IController* getPlayerController() const;
    bool isKeyJustReleased(Key key) const;

protected:
    std::string getKeyName(Key key) const;
    KeyMod getKeyMods(int glfwMods) const;
    void handleKeyEvent(int key, int scancode, int action, int mods);
    void handleMouseMoveEvent(double x, double y);
    void handleMouseButtonEvent(int button, int action, int mods, double x, double y);
    void handleMouseScrollEvent(double xoffset, double yoffset);

    std::unordered_map<int, bool> m_currentMouseButtons;
    std::unordered_map<int, bool> m_previousMouseButtons;

    double m_mouseX = 0.0;
    double m_mouseY = 0.0;
    double m_mouseDeltaX = 0.0;
    double m_mouseDeltaY = 0.0;
    double m_lastMouseX = 0.0;
    double m_lastMouseY = 0.0;
    bool m_firstMouse = true;

    double m_scrollOffset = 0.0;
    ExitCallback m_exitCallback;
    std::unordered_map<Key, bool> m_currentKeys;
    std::unordered_map<Key, bool> m_previousKeys;
    IController* m_activeController = nullptr;
};
}  // namespace RPE