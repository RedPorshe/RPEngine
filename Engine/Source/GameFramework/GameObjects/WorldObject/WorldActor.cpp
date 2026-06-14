#include "WorldActor.h"
#include "../Components/WorldActorComponent.h"
#include "../Components/TransformComponent.h"
#include "Log/Log.h"

namespace RPE
{
REGISTER_CLASS_FACTORY(WActor);

DEFINE_LOG_CATEGORY_STATIC(WActorLog);

WActor::WActor(const std::string& inDisplayName, CObject* inOwner) : CObject(inDisplayName, inOwner)
{
    RP_LOG(WActorLog, Display, "[{}] created", GetName());
    m_RootComponent = addComponent<WTransformComponent>("Default Root");
}

WActor::~WActor()
{
    RP_LOG(WActorLog, Display, "[{}] destroyed", GetName());
    cleanUp();
}

void WActor::BeginPlay()
{
    RP_LOG(WActorLog, Display, "[{}] BeginPlay", GetName());

    for (auto* component : m_actorComponents)
    {
        if (component) component->onBeginPlay();
    }
}

void WActor::Tick(float DeltaTime)
{
    static int cccc2 = 0;
    if (cccc2 < 1)
    {
        RP_LOG(WActorLog, Display, "Tick for {}", GetName());
        cccc2++;
    }
    for (auto* component : m_actorComponents)
    {
        if (component) component->tick(DeltaTime);
    }
}

void WActor::EndPlay()
{
    RP_LOG(WActorLog, Display, "[{}] EndPlay", GetName());

    for (auto* component : m_actorComponents)
    {
        if (component) component->onDestroy();
    }
}

void WActor::destroy()
{
    if (m_isPendingToDestroy) return;

    m_isPendingToDestroy = true;
    RP_LOG(WActorLog, Display, "[{}] marked for destruction", GetName());

    if (ObjectOwner)
    {
        ObjectOwner->RemoveOwnedObject(GetName());
    }
}

void WActor::cleanUp()
{
    m_actorComponents.clear();
    m_RootComponent = nullptr;
}

void WActor::removeComponent(WActorComponent* component)
{
    if (!component) return;

    auto it = std::find(m_actorComponents.begin(), m_actorComponents.end(), component);
    if (it != m_actorComponents.end())
    {
        if (component == m_RootComponent)
        {
            m_RootComponent = nullptr;
        }

        m_actorComponents.erase(it);
        RP_LOG(WActorLog, Display, "[{}] removed component [{}]", GetName(), component->GetName());

        if (component->GetOwner() == this)
        {
            RemoveOwnedObject(component->GetName());
        }
    }
}

void WActor::removeComponent(const std::string& name)
{
    auto it = std::find_if(
        m_actorComponents.begin(), m_actorComponents.end(), [&name](WActorComponent* comp) { return comp && comp->GetName() == name; });

    if (it != m_actorComponents.end())
    {
        WActorComponent* component = *it;

        if (component == m_RootComponent)
        {
            m_RootComponent = nullptr;
        }

        m_actorComponents.erase(it);
        RP_LOG(WActorLog, Display, "[{}] removed component [{}]", GetName(), name);

        if (component->GetOwner() == this)
        {
            RemoveOwnedObject(component->GetName());
        }
    }
}

void WActor::clearComponents()
{
    for (auto* component : m_actorComponents)
    {
        if (component && component->GetOwner() == this)
        {
            RemoveOwnedObject(component->GetName());
        }
    }
    m_actorComponents.clear();
    m_RootComponent = nullptr;
    RP_LOG(WActorLog, Display, "[{}] cleared all components", GetName());
}

void WActor::setRootComponent(WTransformComponent* component)
{
    if (!component)
    {
        RP_LOG(WActorLog, Warning, "[{}] trying to set null root component", GetName());
        return;
    }

    if (m_RootComponent == component) return;

    m_RootComponent = component;
    RP_LOG(WActorLog, Display, "[{}] root component set to [{}]", GetName(), component->GetName());
}

}  // namespace RPE