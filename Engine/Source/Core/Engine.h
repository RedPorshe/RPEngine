#pragma once
#include "EngineConfig.h"
#include <memory>
#include <string_view>

namespace RPE
{
class GLFWWindowManager;

class Engine final
{
public:
    Engine();
    ~Engine();
    void run();
    static constexpr std::string_view version() noexcept { return ENGINE_VERSION_STRING; }

private:
    std::unique_ptr<GLFWWindowManager> m_WindowManager;
    bool m_initialized{false};
};
}  // namespace RPE