#pragma once
#include "EngineConfig.h"
#include <string_view>




namespace RPE
{
class Engine final
{
public:
    Engine();
    ~Engine();
    void Run();
    static constexpr std::string_view version() noexcept { return ENGINE_VERSION_STRING; }
};
}  // namespace RPE