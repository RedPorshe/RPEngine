#pragma once
#include "EngineConfig.h"
#include "Utility.h"
#include <memory>
#include <string_view>

namespace RPE
{

class Engine final : public NonCopyable
{
public:
    Engine(std::unique_ptr<class IWindowManager> WindowManager);
    ~Engine();
    void run();
    static constexpr std::string_view version() noexcept { return ENGINE_VERSION_STRING; }

private:
    const std::unique_ptr<class IWindowManager> m_WindowManager;
    const std::shared_ptr<class InputManager> m_inputManager;
    bool m_initialized{false};
    void onInputEvent(const struct InputEvent& event);
};
}  // namespace RPE