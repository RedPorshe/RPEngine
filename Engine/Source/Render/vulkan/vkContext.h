#pragma once
#include "Core/Utility.h"
#include "Window/IWindow.h"

#include <string>
#include <vector>
#include <memory>

namespace RPE
{
class VulkanContext : public NonCopyable
{
public:
    VulkanContext();
    virtual ~VulkanContext();
    bool preInit(const WindowSettings&, const std::string& EngineName);
    bool init(IWindow* windowHandle);
    bool render();
    void setRenderData(/*data*/);
    void shutdown();
    void OnResize(int width, int height);
    bool isInitialized() const;
    IWindow* getWindowHandle();

private:
    void registerManager(std::unique_ptr<class IVkManager> manager);
    bool preInitManagers(const WindowSettings& settings, const std::string& engineName);
    bool initManagers();
    std::vector<std::unique_ptr<class IVkManager>> m_managers;
    IWindow* m_windowHandle{nullptr};
    bool m_initialized{false};

public:
    class InstanceManager* getInstanceManager();
    const class InstanceManager* getInstanceManager() const;
    class DeviceManager* getDeviceManager();
    const class DeviceManager* getDeviceManager() const;
    class SwapchainManager* getSwapchainManager();
    const class SwapchainManager* getSwapchainManager() const;
    class RenderPassManager* getRenderpassManager();
    const class RenderPassManager* getRenderpassManager() const;
    class FrameBufferManager* getFrameBufferManager();
    const class FrameBufferManager* getFrameBufferManager() const;
    template <typename T>
    T* findManager()
    {
        for (auto& manager : m_managers)
        {
            T* casted = dynamic_cast<T*>(manager.get());
            if (casted) return casted;
        }
        return nullptr;
    }

    template <typename T>
    const T* findManager() const
    {
        for (const auto& manager : m_managers)
        {
            const T* casted = dynamic_cast<const T*>(manager.get());
            if (casted) return casted;
        }
        return nullptr;
    }
};
}  // namespace RPE