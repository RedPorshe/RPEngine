#pragma once
#include "Render/RHI/Renderer.h"
#include "EngineConfig.h"
#include <string>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

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

    bool startFrame();
    bool endFrame();
    bool renderTriangle(uint32_t imageIndex);
    bool renderWorld(uint32_t imageIndex);
    bool renderClear(uint32_t imageIndex);
    bool recordCommandBuffer(uint32_t imageIndex);
    bool createUniformBuffers();
    bool createDescriptorSets();
    void updateUniformBuffer(uint32_t currentFrame);
    bool recreateSwapchain();
    void cleanupUniformBuffers();
    void cleanupDescriptorSets();

    // Rendering resources
    std::vector<VkBuffer> m_uniformBuffers;
    std::vector<VkDeviceMemory> m_uniformBuffersMemory;
    std::vector<void*> m_uniformBuffersMapped;
    std::vector<VkDescriptorSet> m_descriptorSets;
    uint32_t m_currentImageIndex{0};
    bool m_frameStarted{false};
    bool m_RenderFallbackTriangle = false;
};
}  // namespace RPE