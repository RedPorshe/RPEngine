#include "WorldActorComponent.h"
#include "../WorldObject/WorldActor.h"
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(WorldActorComponentLog);

namespace RPE
{

WActorComponent::WActorComponent(const std::string& inDisplayName, CObject* inOwner) : Super(inDisplayName, inOwner)
{
    m_owner = dynamic_cast<WActor*>(inOwner);
    if (m_owner)
    {
        RP_LOG(WorldActorComponentLog, Display, "[{}] attached to actor [{}]", GetName(), m_owner->GetName());
    }
    else
    {
        RP_LOG(WorldActorComponentLog, Warning, "[{}] owner is not a WActor", GetName());
    }
    SetPrimaryTickEnable();
    Created();
}

WActorComponent::~WActorComponent()
{
    RP_LOG(WorldActorComponentLog, Display, "[{}] destroyed", GetName());
}

void WActorComponent::tick(float deltaTime)
{
    if (bisUseTick)
    {
        for (auto* component : m_childComponents)
        {
            if (component) component->tick(deltaTime);
        }
    }
}

void WActorComponent::onDestroy()
{
    RP_LOG(WorldActorComponentLog, Display, " Called on destroy (Base class) for [{}]", GetName());
    for (auto& child : m_childComponents)
    {
        if (child)
        {
            child->onDestroy();
        }
    }
}

void WActorComponent::onBeginPlay() {}

void WActorComponent::addChildComponent(WActorComponent* component)
{
    if (!component)
    {
        RP_LOG(WorldActorComponentLog, Warning, "Attempt to add null component");
        return;
    }

    if (getChildComponentIndex(component) != -1)
    {
        RP_LOG(WorldActorComponentLog, Warning, "Component '{}' already added", component->GetName());
        return;
    }

    m_childComponents.push_back(component);
    RP_LOG(WorldActorComponentLog, Display, "[{}] added child component [{}]", GetName(), component->GetName());
}

void WActorComponent::removeChildComponent(WActorComponent* component)
{
    auto it = std::find(m_childComponents.begin(), m_childComponents.end(), component);
    if (it != m_childComponents.end())
    {
        m_childComponents.erase(it);
        RP_LOG(WorldActorComponentLog, Display, "[{}] removed child component [{}]", GetName(), component->GetName());
    }
}

WActorComponent* WActorComponent::getChildComponent(const std::string& componentName) const
{
    int index = getChildComponentIndex(componentName);
    if (index == -1)
    {
        RP_LOG(WorldActorComponentLog, Warning, "[{}] doesn't have child component named '{}'", GetName(), componentName);
        return nullptr;
    }
    return getChildComponent(index);
}

WActorComponent* WActorComponent::getChildComponent(int index) const
{
    if (index >= 0 && index < static_cast<int>(m_childComponents.size()))
    {
        return m_childComponents[index];
    }
    return nullptr;
}

int WActorComponent::getChildComponentIndex(const std::string& componentName) const
{
    for (int i = 0; i < static_cast<int>(m_childComponents.size()); ++i)
    {
        if (m_childComponents[i] && m_childComponents[i]->GetName() == componentName) return i;
    }
    return -1;
}

int WActorComponent::getChildComponentIndex(const WActorComponent* component) const
{
    for (int i = 0; i < static_cast<int>(m_childComponents.size()); ++i)
    {
        if (m_childComponents[i] == component) return i;
    }
    return -1;
}

void WActorComponent::SetPrimaryTickEnable(bool enable)
{
    bisUseTick = enable;
}

void WActorComponent::OnCreate()
{
    bIsInitialized = true;
    RP_LOG(WorldActorComponentLog, Display, "Actor component base func OnCreate called for {}", GetName());
}

void WActorComponent::serializeProperties(nlohmann::json& jsonObject) const
{
    Super::serializeProperties(jsonObject);
}

void WActorComponent::deserializeProperties(const nlohmann::json& jsonObject)
{
    Super::deserializeProperties(jsonObject);
}

void WActorComponent::SetIsEnabled(bool enable)
{
    bIsEnabled = enable;
}

void WActorComponent::Created()
{
    this->OnCreate();
}

}  // namespace RPE