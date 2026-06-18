#pragma once
#include "Core/SystemObject.h"

#include <vector>
#include "Math/MathTypes.h"

namespace RPE
{
class WActorComponent;
class WTransformComponent;

class WActor : public CObject
{
    CHUDDO_DECLARE_CLASS(WActor, CObject);

public:
    WActor(const std::string& inDisplayName = "WorldActor", CObject* inOwner = nullptr);
    virtual ~WActor();

    // Life circle
    virtual void Tick(float DeltaTime);
    virtual void BeginPlay();
    virtual void EndPlay();
    void destroy();

    bool isPendingToDestroy() const { return m_isPendingToDestroy; }
    void setPendingToDestroy(bool value) { m_isPendingToDestroy = value; }

  
    const std::vector<WActorComponent*>& getComponents() const { return m_actorComponents; }
    std::vector<WActorComponent*>& getComponents() { return m_actorComponents; }

    template <typename Comp, typename... Args>
    Comp* addComponent(const std::string& inName, Args&&... args);

    template <typename Comp>
    Comp* getComponent() const;

    template <typename Comp>
    std::vector<Comp*> getComponentsByType() const;

    void removeComponent(WActorComponent* component);
    void removeComponent(const std::string& name);
    void clearComponents();

  
    WTransformComponent* getRootComponent() const { return m_RootComponent; }
    void setRootComponent(WTransformComponent* component);
    bool hasRootComponent() const { return m_RootComponent != nullptr; }
    WActor* GetAttachParentActor() const;
    void AttachActorToActor(WActor* newParent);
    std::vector<WActor*>& getChildActors();
    const std::vector<WActor*>& getChildActors() const;

    void DetachFromParent();
    bool IsAttachedToActor(const WActor* potentialParent) const;

    bool IsRootActor() const { return m_ParentActor == nullptr; }

    WActor* GetRootParent();
    const WActor* GetRootParent() const;

    // actorTransform
    // 
    // location
    void setActorLocation(const FVector& loc);
    void setActorLocation(float x, float y, float z);
    void setActorRelativeLocation(const FVector& loc);
    void setActorRelativeLocation(float x, float y, float z);

    FVector getActorLocation() const;
    FVector getActorLocation();
    FVector getActorRelativeLocation() const;
    FVector getActorRelativeLocation();

    // rotation
    void setActorRotation(const FQuat& rot);
    void setActorRotation(float x, float y, float z, float w = 1);
    void setActorRelativeRotation(const FQuat& rot);
    void setActorRelativeRotation(float x, float y, float z, float w = 1);

    FQuat getActorRotation() const;
    FQuat getActorRotation();
    FQuat getActorRelativeRotation() const;
    FQuat getActorRelativeRotation();

    // scale
    void setActorScale(const FVector& scale);
    void setActorScale(float x, float y, float z);
    void setActorScale(float scale);
    void setActorRelativeScale(const FVector& scale);
    void setActorRelativeScale(float x, float y, float z);
    void setActorRelativeScale(float scale);
    FVector getActorScale() const;
    FVector getActorScale();
    FVector getActorRelativeScale() const;
    FVector getActorRelativeScale();

    void MoveActor(const FVector& Direction, float offset, bool bTeleport = false);
    void LaunchActor(const FVector& Direction, float force, bool bTeleport = false);

protected:
    void cleanUp();
    void addChildActor(WActor* child);
    float m_deltaTime = 0.f;
private:
    WActor* m_ParentActor = nullptr;
    bool m_isPendingToDestroy = false;
    std::vector<WActorComponent*> m_actorComponents;
    std::vector<WActor*> m_ChildActors;
    WTransformComponent* m_RootComponent = nullptr;
    bool IsCircularAttachment(WActor* potentialParent) const;
   
    friend class WLevel;
};
}  // namespace RPE
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(WActorLogH);

#include "../Components/TransformComponent.h"
namespace RPE
{
// Шаблонные методы
template <typename Comp, typename... Args>
inline Comp* WActor::addComponent(const std::string& inName, Args&&... args)
{
    static_assert(std::is_base_of_v<WActorComponent, Comp>, "Comp must be derived from WActorComponent");

    // Создаем компонент через фабрику
    CObject* obj = OBJECT_FACTORY.Create(Comp::StaticClassName(), this, inName + "_" + GetName());
    if (!obj) return nullptr;

    Comp* component = static_cast<Comp*>(obj);
    m_actorComponents.push_back(component);
    if (m_RootComponent == nullptr)
    {
        if (auto* transformComp = dynamic_cast<WTransformComponent*>(component))
        {
            m_RootComponent = transformComp;
        }
    }

    RP_LOG(WActorLogH, Display, "[{}] added component [{}]", GetName(), component->GetName());
    return component;
}

template <typename Comp>
inline Comp* WActor::getComponent() const
{
    for (auto* component : m_actorComponents)
    {
        if (auto* casted = dynamic_cast<Comp*>(component)) return casted;
    }
    return nullptr;
}

template <typename Comp>
inline std::vector<Comp*> WActor::getComponentsByType() const
{
    std::vector<Comp*> result;
    for (auto* component : m_actorComponents)
    {
        if (auto* casted = dynamic_cast<Comp*>(component)) result.push_back(casted);
    }
    return result;
}

}  // namespace RPE