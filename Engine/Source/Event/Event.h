#pragma once
#include <string>
#include "Log/Log.h"
#include "InputEvent.h"
#include <functional>
#include <vector>

DEFINE_LOG_CATEGORY_STATIC(LogEvent);

namespace RPE
{

template <typename... Args>
class Event
{
public:
    using Callback = std::function<void(Args...)>;

    void subscribe(Callback callback) { m_callbacks.push_back(callback); }

    void invoke(Args... args)
    {
        for (const auto& callback : m_callbacks)
        {
            if (callback) callback(args...);
        }
    }

private:
    std::vector<Callback> m_callbacks;
};
}  // namespace RPE