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
}

WActorComponent::~WActorComponent()
{
    RP_LOG(WorldActorComponentLog, Display, "[{}] destroyed", GetName());
}

void WActorComponent::tick(float deltaTime)
{
    for (auto* component : m_childComponents)
    {
        if (component) component->tick(deltaTime);
    }
}

void WActorComponent::onDestroy() {}

void WActorComponent::onBeginPlay() {}

void WActorComponent::attachTo(WActorComponent* inOwnComponent)
{
    if (inOwnComponent == nullptr) return;
    if (this == inOwnComponent) return;
    if (m_parent = inOwnComponent) return;

    if (m_parent)
    {
        m_parent->removeChildComponent(this);
        m_parent = nullptr;
    }
    if (m_owner && m_owner->TransferOwnership(this, inOwnComponent))
    {
        m_parent = inOwnComponent;
    }
    else
    {
        RP_LOG(WorldActorComponentLog, Error, "[{}] failed to attach to component [{}]", GetName(), inOwnComponent->GetName());
    }
}

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

void WActorComponent::serializeProperties(nlohmann::json& jsonObject) const
{
    Super::serializeProperties(jsonObject);
}

void WActorComponent::deserializeProperties(const nlohmann::json& jsonObject)
{
    Super::deserializeProperties(jsonObject);
}

}  // namespace RPE