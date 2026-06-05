#pragma once
#include "Render/RHI/Renderer.h"
#include "EngineConfig.h"
#include <string>
#include <memory>

namespace RPE
{
class VkRenderer : public RHI
{
public:
    VkRenderer();
    ~VkRenderer() override;

    bool preInit(WindowSettings, const std::string& EngineName) override;
    bool init(IWindow* windowHandle) override;
    void update() override;
    void setEnginePtr(Engine* ptr) override;
    bool render() override;
    void shutdown() override;
    std::string getName() const override;
    std::string getVersion() const override;
    class Engine* getEngine();

protected:
    void onResize(int width, int height) override;

private:
    class Engine* m_EnginePtr = nullptr;
    std::unique_ptr<class VulkanContext> m_ctx;
    std::string m_version{ENGINE_VERSION_STRING};
    void* m_WindowHandle = nullptr;
    bool m_initialized{false};
};
}  // namespace RPE