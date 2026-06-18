#pragma once
#include <string>
#include "Log/Log.h"
#include "InputEvent.h"
#include <functional>
#include <unordered_map>

DEFINE_LOG_CATEGORY_STATIC(LogEvent);

namespace RPE
{

template <typename... Args>
class Event
{
public:
    using Callback = std::function<void(Args...)>;
    using CallbackId = size_t;

    CallbackId subscribe(Callback callback)
    {
        CallbackId id = m_nextId++;
        m_callbacks[id] = std::move(callback);
        return id;
    }

    void unsubscribe(CallbackId id) { m_callbacks.erase(id); }

    void invoke(Args... args)
    {
        for (auto& pair : m_callbacks)
        {
            if (pair.second)
            {
                pair.second(args...);
            }
        }
    }

    void clear() { m_callbacks.clear(); }

    size_t getSubscriberCount() const { return m_callbacks.size(); }

private:
    std::unordered_map<CallbackId, Callback> m_callbacks;
    CallbackId m_nextId = 0;
};

}  // namespace RPE