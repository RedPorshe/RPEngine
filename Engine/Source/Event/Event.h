#pragma once
#include <string>
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(LogEvent);

namespace RPE
{

template <typename... Args>
class Event
{
public:
    void invoke(Args... args) { RP_LOG(LogEvent, Display, "Dispatch event"); }
};
}  // namespace RPE