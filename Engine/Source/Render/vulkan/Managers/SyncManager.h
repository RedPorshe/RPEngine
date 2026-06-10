#pragma once
#include "IVkManager.h"
#include "Window/IWindow.h"
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace RPE
{
class SyncManager : public IVkManager
{
public:
    SyncManager();
    ~SyncManager() override;

    bool preInit(const WindowSettings& settings, const std::string& EngineName) override;
    bool init() override;
    void shutdown() override;
    std::string getName() override;

    VkSemaphore getImageAvailableSemaphore(uint32_t index) const;
    VkSemaphore getRenderFinishedSemaphore(uint32_t index) const;
    VkFence getInFlightFence(uint32_t index) const;
    uint32_t getCurrentFrame() const { return m_currentFrame; }
    void nextFrame() { m_currentFrame = (m_currentFrame + 1) % m_MAX_FRAMES_IN_FLIGHT; }

private:
    bool createSyncs();
    void destroySyncs();

    uint32_t m_MAX_FRAMES_IN_FLIGHT;

    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    uint32_t m_currentFrame{0};
};
}  // namespace RPE