#include "vkRender.h"
#include "vkContext.h"
#include "Core/Engine.h"
#include "Managers/SyncManager.h"
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(VkRenderLog);

using namespace RPE;

VkRenderer::VkRenderer() : m_ctx(std::make_unique<VulkanContext>())
{
    RP_LOG(VkRenderLog, Display, "{} created with engine version {}", getName(), getVersion());
}

VkRenderer::~VkRenderer()
{
    shutdown();
}

bool VkRenderer::preInit(WindowSettings settings, const std::string& EngineName)
{
    if (!m_ctx->preInit(settings, EngineName)) return false;
    return true;  // now stub
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
    RP_LOG(VkRenderLog, Display, "{} init success", getName());
    return true;
}

void VkRenderer::update() {}

void VkRenderer::setEnginePtr(Engine* ptr)
{
    m_EnginePtr = ptr;
    if (m_EnginePtr)
    {
        RP_LOG(VkRenderLog, Display, "Engine ptr succes gained");
    }
    else
    {
        RP_LOG(VkRenderLog, Display, "Engine ptr succes cleared");
    }
}

bool VkRenderer::render()
{
    if (!startFrame())
    {
        RP_LOG(VkRenderLog, Error, "failed to start frame");
    }
    if (!endFrame())
    {
        RP_LOG(VkRenderLog, Error, "failed to end frame");
    }
    return true;
}

void VkRenderer::shutdown()
{
    RP_LOG(VkRenderLog, Display, " {} starting shutdown", getName());
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
    if (m_ctx) m_ctx->OnResize(width, height);
}

bool RPE::VkRenderer::startFrame()
{
    auto currentFrame = m_ctx->getSyncManager()->getCurrentFrame();
    return true;
}

bool RPE::VkRenderer::endFrame()
{
    return true;
}

bool RPE::VkRenderer::renderTriangle(uint32_t imageIndex)
{
    return true;
}
