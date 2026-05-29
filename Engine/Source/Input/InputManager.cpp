#include "InputManager.h"
#include "Log/Log.h"
#include "Event/InputEvent.h"

DEFINE_LOG_CATEGORY_STATIC(InputManagerLog)

using namespace RPE;

InputManager::InputManager() {}

InputManager::~InputManager() {}

void InputManager::proccessInput(const InputEvent& event)
{
    const char* typeStr = "Unknown";
    switch (event.type)
    {
        case EventType::WindowClose: typeStr = "WindowClose"; break;
        case EventType::WindowResize: typeStr = "WindowResize"; break;
        case EventType::MouseMove: typeStr = "MouseMove"; break;
        case EventType::MouseButton: typeStr = "MouseButton"; break;
        case EventType::MouseScroll: typeStr = "MouseScroll"; break;
        case EventType::KeyPress: typeStr = "KeyPress"; break;
    }

    RP_LOG(InputManagerLog, Display, "InputManager received: {}", typeStr);
}
