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

using EventData = int;

struct InputEvent
{
    EventType type;
    EventData data;
};

}  // namespace RPE