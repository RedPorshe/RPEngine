#pragma once
#include <string>
#include "Core/Utility.h"

namespace RPE
{

enum class LogVerbosity : uint8_t
{
    NoLogging = 0,
    Dysplay,
    Warning,
    Error,
    Fatal
};

class Log final : public NonCopyable
{
public:
    static Log& getInstance()
    {
        static Log instance;
        return instance;
    }

    void log(LogVerbosity verbosity, const std::string& message) const;

private:
    Log();
    ~Log();

};
}  // namespace RPE