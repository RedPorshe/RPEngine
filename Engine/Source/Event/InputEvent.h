#pragma once

#include <string>
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

using EventData = int;  // temp

struct InputEvent
{
    EventType type;
    EventData data;
};

}  // namespace RPE