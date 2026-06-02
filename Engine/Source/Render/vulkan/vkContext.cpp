#include "vkContext.h"
#include "Log/Log.h"
#include "Managers/IVkManager.h"
#include "Managers/InstanceManager.h"

DEFINE_LOG_CATEGORY_STATIC(vkContextLog)
using namespace RPE;

VulkanContext::VulkanContext()
{
    registerManager(std::make_unique<InstanceManager>());
}

VulkanContext::~VulkanContext()
{
    shutdown();
}

bool VulkanContext::preInit(const WindowSettings& settings, const std::string& EngineName)
{
    if (!preInitManagers(settings, EngineName)) return false;
    return true;
}

bool VulkanContext::init(IWindow* windowHandle)
{
    if (!windowHandle) return false;
    m_windowHandle = windowHandle;
    if (!initManagers()) return false;
    return true;
}

bool VulkanContext::render()
{
    return true;
}

void VulkanContext::setRenderData() {}

void VulkanContext::shutdown()
{
    RP_LOG(vkContextLog, Display, "Shutdown start");
    for (auto it = m_managers.rbegin(); it != m_managers.rend(); ++it)
    {
        (*it)->shutdown();
    }

    m_managers.clear();

    RP_LOG(vkContextLog, Display, "Shutdown complete");
}

void VulkanContext::OnResize(int width, int height)
{
    for (auto& manager : m_managers)
    {
        manager->onResize(width, height);
    }
    RP_LOG(vkContextLog, Display, " new width :{} new height:{}", width, height);
}

bool VulkanContext::isInitialized() const
{
    return m_initialized;
}

IWindow* VulkanContext::getWindowHandle()
{
    return m_windowHandle;
}

void VulkanContext::registerManager(std::unique_ptr<IVkManager> manager)
{
    if (manager)
    {
        RP_LOG(vkContextLog, Display, "Registered manager: {}", manager->getName());
        manager->setVkContextPtr(this);
        m_managers.push_back(std::move(manager));
    }
}

bool VulkanContext::preInitManagers(const WindowSettings& settings, const std::string& engineName)
{
    RP_LOG(vkContextLog, Display, " preinitializing managers (creating)");
    for (auto& manager : m_managers)
    {
        if (!manager->preInit(settings, engineName))
        {
            RP_LOG(vkContextLog, Error, "Failed to preInit manager: {}", manager->getName());
            return false;
        }
    }
    return true;
}

bool VulkanContext::initManagers()
{
    RP_LOG(vkContextLog, Display, "Context Init managers");
    for (auto& manager : m_managers)
    {
        if (!manager->init())
        {
            RP_LOG(vkContextLog, Error, "Failed to init manager: {}", manager->getName());
            return false;
        }
    }

    m_initialized = true;
    return true;
}
