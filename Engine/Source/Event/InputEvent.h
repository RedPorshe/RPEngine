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

struct MouseMoveData
{
    double x, y;
};

struct MouseButtonData
{
    int button;
    int action;
    int mods;
    double x, y;
};

struct ScrollData
{
    double xoffset, yoffset;
};

struct KeyData
{
    int key;
    int scancode;
    int action;
    int mods;
};

struct ResizeData
{
    int width, height;
};

using EventData = std::variant<std::monostate, int, MouseMoveData, MouseButtonData, ScrollData, KeyData, ResizeData>;

struct InputEvent
{
    EventType type;
    EventData data;
};

}  // namespace RPE