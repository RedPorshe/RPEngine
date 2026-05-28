#pragma once
#include <string>
#include "Log/Log.h"
#include "InputEvent.h"

DEFINE_LOG_CATEGORY_STATIC(LogEvent);

namespace RPE
{

template <typename... Args>
class Event
{
public:
    void invoke(Args... args)
    {
        auto printEvent = [](const InputEvent& event)
        {
            std::string eventStr{};
            switch (event.type)
            {
                case EventType::WindowClose: eventStr = "Window Close"; break;
                case EventType::WindowResize: eventStr = "Window Resize"; break;
                case EventType::MouseMove: eventStr = "Mouse Move"; break;
                case EventType::MouseButton: eventStr = "Mouse Button"; break;
                case EventType::MouseScroll: eventStr = "Mouse Scroll"; break;
                case EventType::KeyPress: eventStr = "Key Press"; break;
            }
            RP_LOG(LogEvent, Display, "Dispatch event: {}", eventStr);
        };
        (printEvent(args), ...);
    }
};
}  // namespace RPE