#pragma once
#include "IVkManager.h"
#include "Window/IWindow.h"
#include <string>
#include <vulkan/vulkan.h>

namespace RPE
{
class DeviceManager : public IVkManager
{
public:
    DeviceManager();
    ~DeviceManager() override;

    bool preInit(const WindowSettings& settings, const std::string& EngineName) override;
    bool init() override;
    void shutdown() override;
    std::string getName() override;
    VkDevice getDevice() const;
    VkPhysicalDevice getPhysicalDevice() const;
    int getGraphicsQueueIndex() const;
    int getPresentQueueIndex() const;

private:
    bool pickPhysicalDevice();
    bool createDevice();
    VkDevice m_device{VK_NULL_HANDLE};
    VkPhysicalDevice m_physicalDevice{VK_NULL_HANDLE};
    int m_graphicsQueueIndex{-1};
    int m_presentQueueIndex{-1};
};
}  // namespace RPE