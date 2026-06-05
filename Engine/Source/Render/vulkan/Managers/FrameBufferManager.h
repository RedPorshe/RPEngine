#pragma once
#include "IVkManager.h"
#include "Window/IWindow.h"
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace RPE
{
class FrameBufferManager : public IVkManager
{
public:
    FrameBufferManager();
    ~FrameBufferManager() override;

    bool preInit(const WindowSettings& settings, const std::string& EngineName) override;
    bool init() override;
    void shutdown() override;
    std::string getName() override;
    void onResize(int width, int height) override;
    const std::vector<VkFramebuffer>& getFramebuffers() const;

private:
    void clearBuffers();
    bool createBuffers();
    bool recreateBuffer();
    std::vector<VkFramebuffer> m_framebuffers;
};
}  // namespace RPE