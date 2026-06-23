#include "vkRender.h"
#include "vkContext.h"
#include "Core/Engine.h"
#include "Managers/SyncManager.h"
#include "Managers/DeviceManager.h"
#include "Managers/SwapchainManager.h"
#include "Managers/CommandManager.h"
#include "Managers/PipelineManager.h"
#include "Managers/DescriptorManager.h"
#include "Managers/RenderpassManager.h"
#include "Managers/FrameBufferManager.h"
#include "Managers/BufferManager.h"
#include "GameFrameWork/World/World.h"
#include "Render/FrameInfo.h"
#include "Log/Log.h"
#include <chrono>
#include <cstring>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

DEFINE_LOG_CATEGORY_STATIC(VkRenderLog);

using namespace RPE;

struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

VkRenderer::VkRenderer() : m_ctx(std::make_unique<VulkanContext>())
{
    RP_LOG(VkRenderLog, Display, "{} created with engine version {}", getName(), getVersion());
#ifdef _DEBUG
    m_RenderFallbackTriangle = true;
#else
    m_RenderFallbackTriangle = false;
#endif  //  _DEBUG
}

VkRenderer::~VkRenderer()
{
    shutdown();
}

bool VkRenderer::preInit(WindowSettings settings, const std::string& EngineName)
{
    if (!m_ctx->preInit(settings, EngineName)) return false;
    return true;
}

bool VkRenderer::init(IWindow* windowHandle)
{
    m_ctx->SetRenderPtr(this);
    m_WindowHandle = windowHandle->getWindowHandle();
    if (m_WindowHandle == nullptr) return false;

    if (!m_ctx->init(windowHandle))
    {
        RP_LOG(VkRenderLog, Error, "{} init failed", getName());
        return false;
    }

    // Создаём ресурсы после инициализации контекста
    if (!createUniformBuffers())
    {
        RP_LOG(VkRenderLog, Error, "Failed to create uniform buffers");
        return false;
    }

    if (!createDescriptorSets())
    {
        RP_LOG(VkRenderLog, Error, "Failed to create descriptor sets");
        return false;
    }

    m_initialized = true;
    RP_LOG(VkRenderLog, Display, "{} init success", getName());
    return true;
}

void VkRenderer::update()
{
    m_FrameInfo.clear();
    const auto GameInstance = m_EnginePtr->getGameInstance();
    if (GameInstance)
    {
        auto world = GameInstance->GetWorld();
        if (world)
        {
            world->CollectRenderInfo(m_FrameInfo);
        }
    }
}

void VkRenderer::setEnginePtr(Engine* ptr)
{
    m_EnginePtr = ptr;
    if (m_EnginePtr)
    {
        RP_LOG(VkRenderLog, Display, "Engine ptr successfully gained");
    }
    else
    {
        RP_LOG(VkRenderLog, Display, "Engine ptr cleared");
    }
}

bool VkRenderer::render()
{
    if (!m_initialized)
    {
        RP_LOG(VkRenderLog, Error, "Renderer not initialized");
        return false;
    }

    if (!startFrame())
    {
        RP_LOG(VkRenderLog, Error, "Failed to start frame");
        return false;
    }
    else
    {
        if (!endFrame())
        {
            RP_LOG(VkRenderLog, Error, "Failed to end frame");
            return false;
        }
    }

    return true;
}

void VkRenderer::shutdown()
{
    if (!m_initialized) return;

    RP_LOG(VkRenderLog, Display, "{} starting shutdown", getName());

    auto* deviceMgr = m_ctx->getDeviceManager();
    if (deviceMgr)
    {
        VkDevice device = deviceMgr->getDevice();
        if (device != VK_NULL_HANDLE)
        {
            vkDeviceWaitIdle(device);
        }
    }

    cleanupUniformBuffers();
    cleanupDescriptorSets();

    if (m_ctx)
    {
        m_ctx->shutdown();
    }

    m_initialized = false;
    RP_LOG(VkRenderLog, Display, "{} shutdown complete", getName());
}

std::string VkRenderer::getName() const
{
    return "[Vulkan Renderer]";
}

std::string VkRenderer::getVersion() const
{
    return m_version;
}

Engine* VkRenderer::getEngine()
{
    return m_EnginePtr;
}

void VkRenderer::onResize(int width, int height)
{
    m_width = width;
    m_height = height;

    if (m_ctx) m_ctx->OnResize(width, height);
}

bool VkRenderer::startFrame()
{
    if (m_frameStarted)
    {
        RP_LOG(VkRenderLog, Warning, "Frame already started");
        return true;
    }

    auto* deviceMgr = m_ctx->getDeviceManager();
    auto* swapchainMgr = m_ctx->getSwapchainManager();
    auto* syncMgr = m_ctx->getSyncManager();

    if (!deviceMgr || !swapchainMgr || !syncMgr)
    {
        RP_LOG(VkRenderLog, Error, "Required managers are null");
        return false;
    }

    VkDevice device = deviceMgr->getDevice();
    VkSwapchainKHR swapchain = swapchainMgr->getSwapchain();

    if (device == VK_NULL_HANDLE || swapchain == VK_NULL_HANDLE)
    {
        RP_LOG(VkRenderLog, Error, "Device or Swapchain is null");
        return false;
    }

    uint32_t currentFrame = syncMgr->getCurrentFrame();

    VkFence currentFence = syncMgr->getInFlightFence(currentFrame);
    VkResult result = vkWaitForFences(device, 1, &currentFence, VK_TRUE, UINT64_MAX);

    if (result != VK_SUCCESS)
    {
        RP_LOG(VkRenderLog, Error, "Failed to wait for fence: {}", static_cast<int>(result));
        return false;
    }

    vkResetFences(device, 1, &currentFence);

    result = vkAcquireNextImageKHR(
        device, swapchain, UINT64_MAX, syncMgr->getImageAvailableSemaphore(currentFrame), VK_NULL_HANDLE, &m_currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RP_LOG(VkRenderLog, Warning, "Swapchain out of date, recreating");
        return recreateSwapchain();
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        RP_LOG(VkRenderLog, Error, "Failed to acquire swapchain image: {}", static_cast<int>(result));
        return false;
    }

    updateUniformBuffer(currentFrame);

    // Записываем command buffer
    if (!recordCommandBuffer(m_currentImageIndex))
    {
        RP_LOG(VkRenderLog, Error, "Failed to record command buffer");
        return false;
    }

    VkSemaphore waitSemaphores[] = {syncMgr->getImageAvailableSemaphore(currentFrame)};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore signalSemaphores[] = {syncMgr->getRenderFinishedSemaphore(currentFrame)};

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;

    auto* cmdMgr = m_ctx->getCommandManager();
    VkCommandBuffer commandBuffer = cmdMgr->getCommandBuffer(m_currentImageIndex);
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VkQueue graphicsQueue;
    vkGetDeviceQueue(device, deviceMgr->getGraphicsQueueIndex(), 0, &graphicsQueue);

    result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, currentFence);
    if (result != VK_SUCCESS)
    {
        RP_LOG(VkRenderLog, Error, "Failed to submit draw command buffer: {}", static_cast<int>(result));
        return false;
    }

    m_frameStarted = true;
    return true;
}

bool VkRenderer::endFrame()
{
    if (!m_frameStarted)
    {
        RP_LOG(VkRenderLog, Warning, "No frame to end");
        return true;
    }

    auto* deviceMgr = m_ctx->getDeviceManager();
    auto* swapchainMgr = m_ctx->getSwapchainManager();
    auto* syncMgr = m_ctx->getSyncManager();

    if (!deviceMgr || !swapchainMgr || !syncMgr)
    {
        RP_LOG(VkRenderLog, Error, "Required managers are null");
        return false;
    }

    VkDevice device = deviceMgr->getDevice();
    VkQueue presentQueue;
    vkGetDeviceQueue(device, deviceMgr->getPresentQueueIndex(), 0, &presentQueue);

    VkSwapchainKHR swapchains[] = {swapchainMgr->getSwapchain()};
    auto pWaitSemaphores = syncMgr->getRenderFinishedSemaphore(syncMgr->getCurrentFrame());
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &pWaitSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &m_currentImageIndex;
    presentInfo.pResults = nullptr;

    VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        RP_LOG(VkRenderLog, Warning, "Swapchain out of date or suboptimal, recreating");
        m_frameStarted = false;
        return recreateSwapchain();
    }
    else if (result != VK_SUCCESS)
    {
        RP_LOG(VkRenderLog, Error, "Failed to present swapchain image: {}", static_cast<int>(result));
        return false;
    }

    // Переходим к следующему фрейму
    syncMgr->nextFrame();
    m_frameStarted = false;

    return true;
}

bool VkRenderer::createUniformBuffers()
{
    auto* deviceMgr = m_ctx->getDeviceManager();
    auto* syncMgr = m_ctx->getSyncManager();

    if (!deviceMgr || !syncMgr) return false;

    VkDevice device = deviceMgr->getDevice();
    VkPhysicalDevice physicalDevice = deviceMgr->getPhysicalDevice();

    if (device == VK_NULL_HANDLE || physicalDevice == VK_NULL_HANDLE) return false;

    uint32_t frameCount = m_ctx->getMaxFrames();
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    m_uniformBuffers.resize(frameCount);
    m_uniformBuffersMemory.resize(frameCount);
    m_uniformBuffersMapped.resize(frameCount);

    for (uint32_t i = 0; i < frameCount; i++)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_uniformBuffers[i]) != VK_SUCCESS)
        {
            RP_LOG(VkRenderLog, Error, "Failed to create uniform buffer {}", i);
            return false;
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, m_uniformBuffers[i], &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;

        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        uint32_t memoryTypeIndex = 0;
        for (uint32_t j = 0; j < memProperties.memoryTypeCount; j++)
        {
            if ((memRequirements.memoryTypeBits & (1 << j)) &&
                (memProperties.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) &&
                (memProperties.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
            {
                memoryTypeIndex = j;
                break;
            }
        }

        allocInfo.memoryTypeIndex = memoryTypeIndex;

        if (vkAllocateMemory(device, &allocInfo, nullptr, &m_uniformBuffersMemory[i]) != VK_SUCCESS)
        {
            RP_LOG(VkRenderLog, Error, "Failed to allocate uniform buffer memory {}", i);
            vkDestroyBuffer(device, m_uniformBuffers[i], nullptr);
            return false;
        }

        vkBindBufferMemory(device, m_uniformBuffers[i], m_uniformBuffersMemory[i], 0);
        vkMapMemory(device, m_uniformBuffersMemory[i], 0, bufferSize, 0, &m_uniformBuffersMapped[i]);
    }

    RP_LOG(VkRenderLog, Display, "Created {} uniform buffers", frameCount);
    return true;
}

bool VkRenderer::createDescriptorSets()
{
    auto* descMgr = m_ctx->getDescriptorManager();
    auto* syncMgr = m_ctx->getSyncManager();

    if (!descMgr || !syncMgr) return false;

    uint32_t frameCount = m_ctx->getMaxFrames();
    m_descriptorSets.resize(frameCount);

    // Используем layout для pipeline "Triangle"
    const std::string layoutName = "Triangle";

    for (uint32_t i = 0; i < frameCount; i++)
    {
        if (!descMgr->allocateDescriptorSet("MainPool", layoutName, m_descriptorSets[i]))
        {
            RP_LOG(VkRenderLog, Error, "Failed to allocate descriptor set {}", i);
            return false;
        }

        descMgr->updateUniformBufferDescriptor(m_descriptorSets[i], 0, m_uniformBuffers[i], sizeof(UniformBufferObject), 0);
    }

    RP_LOG(VkRenderLog, Display, "Created {} descriptor sets", frameCount);
    return true;
}

void VkRenderer::updateUniformBuffer(uint32_t currentFrame)
{
    if (currentFrame >= m_uniformBuffersMapped.size() || m_uniformBuffersMapped[currentFrame] == nullptr)
    {
        return;
    }

    auto* swapchainMgr = m_ctx->getSwapchainManager();
    VkExtent2D extent = swapchainMgr->getExtent();

    // Проверяем валидность размера
    if (extent.width == 0 || extent.height == 0)
    {
        return;
    }

    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    float aspect = static_cast<float>(extent.width) / static_cast<float>(extent.height);
    ubo.proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    memcpy(m_uniformBuffersMapped[currentFrame], &ubo, sizeof(ubo));
}

bool VkRenderer::recreateSwapchain()
{
    auto* deviceMgr = m_ctx->getDeviceManager();
    auto* swapchainMgr = m_ctx->getSwapchainManager();

    if (!deviceMgr || !swapchainMgr) return false;

    VkDevice device = deviceMgr->getDevice();
    if (device == VK_NULL_HANDLE) return false;

    vkDeviceWaitIdle(device);

    // Пересоздаём swapchain
    if (swapchainMgr->isInitialized())
    {
        if (!swapchainMgr->RecreateSwapchain())
        {
            RP_LOG(VkRenderLog, Error, "Failed to recreate swapchain");
            return false;
        }
    }

    // Проверяем, что размеры валидны после пересоздания
    VkExtent2D extent = swapchainMgr->getExtent();
    if (extent.width == 0 || extent.height == 0)
    {
        RP_LOG(VkRenderLog, Error, "Swapchain recreated with invalid extent: {}x{}", extent.width, extent.height);
        return false;
    }

    // После recreate swapchain нужно обновить размеры
    auto* framebufferMgr = m_ctx->getFrameBufferManager();
    if (framebufferMgr)
    {
        framebufferMgr->onResize(extent.width, extent.height);
    }

    RP_LOG(VkRenderLog, Display, "Swapchain recreated successfully");
    return true;
}

bool VkRenderer::renderTriangle(uint32_t imageIndex)
{
    auto* deviceMgr = m_ctx->getDeviceManager();
    auto* cmdMgr = m_ctx->getCommandManager();
    auto* renderPassMgr = m_ctx->getRenderpassManager();
    auto* framebufferMgr = m_ctx->getFrameBufferManager();
    auto* pipelineMgr = m_ctx->getPipelineManager();
    auto* swapchainMgr = m_ctx->getSwapchainManager();
    auto* syncMgr = m_ctx->getSyncManager();

    if (!deviceMgr || !cmdMgr || !renderPassMgr || !framebufferMgr || !pipelineMgr)
    {
        RP_LOG(VkRenderLog, Error, "Managers are null in recordCommandBuffer");
        return false;
    }

    VkExtent2D extent = swapchainMgr->getExtent();
    if (extent.width == 0 || extent.height == 0)
    {
        RP_LOG(VkRenderLog, Warning, "Cannot record command buffer with invalid extent: {}x{}", extent.width, extent.height);
        return false;
    }

    VkDevice device = deviceMgr->getDevice();
    VkCommandBuffer commandBuffer = cmdMgr->getCommandBuffer(imageIndex);

    if (commandBuffer == VK_NULL_HANDLE)
    {
        RP_LOG(VkRenderLog, Error, "Command buffer is null");
        return false;
    }

    VkResult result = vkResetCommandBuffer(commandBuffer, 0);
    if (result != VK_SUCCESS)
    {
        RP_LOG(VkRenderLog, Error, "Failed to reset command buffer: {}", static_cast<int>(result));
        return false;
    }

    // Начинаем запись
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        RP_LOG(VkRenderLog, Error, "Failed to begin recording command buffer");
        return false;
    }

    // Начинаем render pass
    const auto& framebuffers = framebufferMgr->getFramebuffers();
    if (imageIndex >= framebuffers.size())
    {
        RP_LOG(VkRenderLog, Error, "Invalid framebuffer index: {}", imageIndex);
        return false;
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPassMgr->getRenderPass();
    renderPassInfo.framebuffer = framebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};

    renderPassInfo.renderArea.extent = extent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Устанавливаем viewport и scissor
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    const std::string pipelineName = "Triangle";
    VkPipeline pipeline = pipelineMgr->getPipeline(pipelineName);
    VkPipelineLayout layout = pipelineMgr->getLayout(pipelineName);

    if (pipeline != VK_NULL_HANDLE && layout != VK_NULL_HANDLE)
    {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        // Bind descriptor sets
        auto* descMgr = m_ctx->getDescriptorManager();
        uint32_t currentFrame = syncMgr->getCurrentFrame();

        if (currentFrame < m_descriptorSets.size() && m_descriptorSets[currentFrame] != VK_NULL_HANDLE)
        {
            vkCmdBindDescriptorSets(
                commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &m_descriptorSets[currentFrame], 0, nullptr);
        }

        // Рисуем треугольник (3 вершины)
        vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    }
    else
    {
        RP_LOG(VkRenderLog, Warning, "Pipeline '{}' not found", pipelineName);
    }

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        RP_LOG(VkRenderLog, Error, "Failed to record command buffer");
        return false;
    }

    return true;
}

bool RPE::VkRenderer::renderWorld(uint32_t imageIndex)
{
    return true;
}

bool RPE::VkRenderer::renderClear(uint32_t imageIndex)
{
    auto* deviceMgr = m_ctx->getDeviceManager();
    auto* cmdMgr = m_ctx->getCommandManager();
    auto* renderPassMgr = m_ctx->getRenderpassManager();
    auto* framebufferMgr = m_ctx->getFrameBufferManager();
    auto* swapchainMgr = m_ctx->getSwapchainManager();

    if (!deviceMgr || !cmdMgr || !renderPassMgr || !framebufferMgr)
    {
        RP_LOG(VkRenderLog, Error, "Managers are null in renderClear");
        return false;
    }

    VkExtent2D extent = swapchainMgr->getExtent();
    if (extent.width == 0 || extent.height == 0)
    {
        RP_LOG(VkRenderLog, Warning, "Cannot record command buffer with invalid extent: {}x{}", extent.width, extent.height);
        return false;
    }

    VkCommandBuffer commandBuffer = cmdMgr->getCommandBuffer(imageIndex);
    if (commandBuffer == VK_NULL_HANDLE)
    {
        RP_LOG(VkRenderLog, Error, "Command buffer is null");
        return false;
    }

    VkResult result = vkResetCommandBuffer(commandBuffer, 0);
    if (result != VK_SUCCESS)
    {
        RP_LOG(VkRenderLog, Error, "Failed to reset command buffer: {}", static_cast<int>(result));
        return false;
    }

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        RP_LOG(VkRenderLog, Error, "Failed to begin recording command buffer");
        return false;
    }

    const auto& framebuffers = framebufferMgr->getFramebuffers();
    if (imageIndex >= framebuffers.size())
    {
        RP_LOG(VkRenderLog, Error, "Invalid framebuffer index: {}", imageIndex);
        return false;
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPassMgr->getRenderPass();
    renderPassInfo.framebuffer = framebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = extent;

    // Чистый цвет - например, синий для отладки
    VkClearValue clearColor = {{{0.1f, 0.2f, 0.4f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Устанавливаем viewport и scissor (нужно для валидности command buffer)
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // Не рисуем ничего - просто очищаем экран

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        RP_LOG(VkRenderLog, Error, "Failed to record command buffer");
        return false;
    }

    return true;
}

void VkRenderer::cleanupUniformBuffers()
{
    auto* deviceMgr = m_ctx->getDeviceManager();
    if (!deviceMgr) return;

    VkDevice device = deviceMgr->getDevice();
    if (device == VK_NULL_HANDLE) return;

    for (size_t i = 0; i < m_uniformBuffers.size(); i++)
    {
        if (m_uniformBuffersMapped[i])
        {
            vkUnmapMemory(device, m_uniformBuffersMemory[i]);
        }
        if (m_uniformBuffersMemory[i] != VK_NULL_HANDLE)
        {
            vkFreeMemory(device, m_uniformBuffersMemory[i], nullptr);
        }
        if (m_uniformBuffers[i] != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(device, m_uniformBuffers[i], nullptr);
        }
    }

    m_uniformBuffers.clear();
    m_uniformBuffersMemory.clear();
    m_uniformBuffersMapped.clear();
}

void VkRenderer::cleanupDescriptorSets()
{
    m_descriptorSets.clear();
}

bool VkRenderer::recordCommandBuffer(uint32_t imageIndex)
{
    bool Succes = false;
    if (m_FrameInfo.isValid())
    {
        Succes = renderWorld(imageIndex);
    }
    else
    {
        if (m_RenderFallbackTriangle)
        {
            Succes = renderTriangle(imageIndex);
        }
        else
        {
            Succes = renderClear(imageIndex);
        }
    }
    return Succes;
}