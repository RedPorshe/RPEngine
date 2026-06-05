#pragma once
#include "EngineConfig.h"
#include "Window/IWindow.h"
#include "Utility.h"
#include <memory>
#include <string_view>
#include <stdexcept>
#include <atomic>

namespace RPE
{

class Engine final : public NonCopyable
{
public:
    Engine(std::unique_ptr<class IWindowManager> WindowManager, std::unique_ptr<class RHI> renderer);
    ~Engine();
    void run();
    static constexpr std::string_view version() noexcept { return ENGINE_VERSION_STRING; }
    bool isInitialized() const noexcept { return m_initialized; }
    void requestExit();
    static Engine& Get()
    {
        if (!s_instance)
        {
            throw std::runtime_error("Engine instance is not initialized.");
        }
        return *s_instance;
    }
    std::shared_ptr<class InputManager> getInputManager() const { return m_inputManager; }
    class IWindow* getMainWindow() const;

private:
    const std::unique_ptr<class IWindowManager> m_WindowManager;
    const std::shared_ptr<class InputManager> m_inputManager;
    const std::unique_ptr<class RHI> m_renderer;
    bool m_initialized{false};
    class IWindow* m_MainWindow = nullptr;
    struct WindowId mainWindowId;
    void onInputEvent(const struct InputEvent& event);
    void updateGameLogic(float deltaTime);
    void setupWindowEvents(struct WindowId windowId);
    std::atomic<bool> m_requestExit{false};
    static Engine* s_instance;
};

}  // namespace RPE