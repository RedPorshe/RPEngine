#pragma once

#include <string>
#include <variant>

#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(LogInputEvent);

namespace RPE
{
enum class EventType
{
    WindowClose,
    WindowResize,
    MouseMove,
    MouseButton,
    MouseScroll,
    KeyPress
};

// using EventData = int;  // temp

struct WindowResizeData
{
    int width;
    int height;
};
struct MouseMoveData
{
    double x;
    double y;
};
struct MouseButtonData
{
    int button;
    int action;
    int mods;
    double x;
    double y;
};
struct MouseScrollData
{
    double xoffset;
    double yoffset;
};
struct KeyPressData
{
    int key;
    int scancode;
    int action;
    int mods;
};

struct InputEvent
{
    EventType type;
    std::variant<std::monostate,  // для WindowClose (нет данных)
        WindowResizeData, MouseMoveData, MouseButtonData, MouseScrollData, KeyPressData>
        data;
};

}  // namespace RPE