#include "RenderpassManager.h"
#include "SwapchainManager.h"
#include "DeviceManager.h"
#include "../vkContext.h"
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(RenderPassLog);

using namespace RPE;

RenderPassManager::RenderPassManager()
{
    RP_LOG(RenderPassLog, Display, "{} created", getName());
}

RenderPassManager::~RenderPassManager()
{
    RP_LOG(RenderPassLog, Display, "{} destroing", getName());
    shutdown();
    RP_LOG(RenderPassLog, Display, "{} destroing complete", getName());
}

bool RenderPassManager::preInit(const WindowSettings& settings, const std::string& EngineName)
{
    return true;
}

bool RenderPassManager::init()
{
    if (!m_contextPtr)
    {
        RP_LOG(RenderPassLog, Error, "{} has no context ptr...", getName());
        return false;
    }
    auto swapchainMgr = m_contextPtr->getSwapchainManager();
    if (!swapchainMgr)
    {
        RP_LOG(RenderPassLog, Error, "SwapchainManager is null");
        return false;
    }
    m_swapchainFormat = swapchainMgr->getImageFormat();
    swapchainMgr->onRecreate().subscribe([this](const InputEvent& event) { RecreateRenderPass(); });
    if (!createRenderPass())
    {
        RP_LOG(RenderPassLog, Error, "Failed to create render pass");
        return false;
    }
    m_isInitialized = true;
    RP_LOG(RenderPassLog, Display, "{} created successfully", getName());
    return true;
}

void RenderPassManager::shutdown()
{
    if (isInitialized())
    {
        if (m_renderPass != VK_NULL_HANDLE)
        {
            auto* deviceMgr = m_contextPtr->getDeviceManager();
            if (deviceMgr)
            {
                VkDevice device = deviceMgr->getDevice();
                vkDestroyRenderPass(device, m_renderPass, nullptr);
            }
            m_renderPass = VK_NULL_HANDLE;
            RP_LOG(RenderPassLog, Display, "{} Renderpass destroyed", getName());
        }
        m_isInitialized = false;
    }
    RP_LOG(RenderPassLog, Display, "{} shutdown complete", getName());
}

std::string RenderPassManager::getName()
{
    return "[Renderpass Manager]";
}

void RenderPassManager::onResize(int width, int height)
{
    RecreateRenderPass();
}

VkRenderPass RPE::RenderPassManager::getRenderPass() const
{
    return m_renderPass;
}

bool RenderPassManager::createRenderPass()
{
    if (!m_contextPtr)
    {
        RP_LOG(RenderPassLog, Error, "{} has no context ptr...", getName());
        return false;
    }
    VkDevice device = VK_NULL_HANDLE;
    auto devicMgr = m_contextPtr->getDeviceManager();
    if (!devicMgr)
    {
        RP_LOG(RenderPassLog, Error, "device manager is null");
        return false;
    }
    device = devicMgr->getDevice();
    if (device == VK_NULL_HANDLE)
    {
        RP_LOG(RenderPassLog, Error, "device is null");
        return false;
    }

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_swapchainFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Subpass
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    // Subpass dependency
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkResult result{};
    result = vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_renderPass);
    if (result != VK_SUCCESS)
    {
        RP_LOG(RenderPassLog, Error, "Failed to create render pass: {}", static_cast<int>(result));
        return false;
    }
    return true;
}

bool RenderPassManager::RecreateRenderPass()
{

    if (!m_contextPtr)
    {
        RP_LOG(RenderPassLog, Error, "{} has no context ptr...", getName());
        return false;
    }
    VkDevice device = VK_NULL_HANDLE;
    auto devicMgr = m_contextPtr->getDeviceManager();
    if (!devicMgr)
    {
        RP_LOG(RenderPassLog, Error, "device manager is null");
        return false;
    }
    device = devicMgr->getDevice();
    if (device == VK_NULL_HANDLE)
    {
        RP_LOG(RenderPassLog, Error, "device is null");
        return false;
    }
    auto swapchainMgr = m_contextPtr->getSwapchainManager();
    if (!swapchainMgr)
    {
        RP_LOG(RenderPassLog, Error, "SwapchainManager is null");
        return false;
    }
    m_swapchainFormat = swapchainMgr->getImageFormat();

    if (m_renderPass != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(device);
        vkDestroyRenderPass(device, m_renderPass, nullptr);
        m_renderPass = VK_NULL_HANDLE;
    }
    bool success = createRenderPass();
    if (success)
    {
        RP_LOG(RenderPassLog, Display, "Success recreate {}", getName());
        return success;
    }
    RP_LOG(RenderPassLog, Error, "{} failed to create renderpass ...", getName());
    return success;
}
