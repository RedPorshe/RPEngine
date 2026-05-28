#pragma once
#include "EngineConfig.h"
#include <memory>
#include <string_view>

namespace RPE
{

class Engine final
{
public:
    Engine(std::unique_ptr<class IWindowManager> WindowManager);
    ~Engine();
    void run();
    static constexpr std::string_view version() noexcept { return ENGINE_VERSION_STRING; }

private:
    const std::unique_ptr<class IWindowManager> m_WindowManager;
    bool m_initialized{false};
};
}  // namespace RPE