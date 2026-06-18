#include "WorldActor.h"
#include "../Components/WorldActorComponent.h"
#include "../Components/TransformComponent.h"
#include "Math/MathTypes.h"
#include "Log/Log.h"

namespace RPE
{
REGISTER_CLASS_FACTORY(WActor);

DEFINE_LOG_CATEGORY_STATIC(WActorLog);

WActor::WActor(const std::string& inDisplayName, CObject* inOwner) : CObject(inDisplayName, inOwner)
{
    RP_LOG(WActorLog, Display, "[{}] created", GetName());
    m_RootComponent = addComponent<WTransformComponent>("Default_Root");
}

WActor::~WActor()
{
    RP_LOG(WActorLog, Display, "[{}] destroyed", GetName());

    DetachFromParent();

    m_ChildActors.clear();

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
    m_deltaTime = DeltaTime;
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

    for (WActor* child : m_ChildActors)
    {
        if (child && !child->isPendingToDestroy())
        {
            child->destroy();
        }
    }
    m_ChildActors.clear();

    DetachFromParent();

    if (ObjectOwner)
    {
        ObjectOwner->RemoveOwnedObject(GetName());
        return;
    }
}

void WActor::cleanUp()
{
    m_actorComponents.clear();
    m_RootComponent = nullptr;
}

void WActor::addChildActor(WActor* child)
{
    if (!child || child == this) return;

    auto it = std::find(m_ChildActors.begin(), m_ChildActors.end(), child);
    if (it != m_ChildActors.end()) return;

    m_ChildActors.push_back(child);
    RP_LOG(WActorLog, Display, "[{}] added child actor [{}]", GetName(), child->GetName());
}

bool WActor::IsCircularAttachment(WActor* potentialParent) const
{
    WActor* temp = potentialParent;
    while (temp)
    {
        if (temp == this) return true;
        temp = temp->GetAttachParentActor();
    }
    return false;
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

WActor* WActor::GetAttachParentActor() const
{
    return m_ParentActor;
}

void WActor::AttachActorToActor(WActor* newParent)
{
    if (newParent == nullptr)
    {
        RP_LOG(WActorLog, Warning, "[{}] trying to attach to null parent", GetName());
        return;
    }

    if (newParent == this)
    {
        RP_LOG(WActorLog, Warning, "[{}] cannot attach to itself", GetName());
        return;
    }

    if (IsCircularAttachment(newParent))
    {
        RP_LOG(WActorLog, Warning, "[{}] circular attachment detected", GetName());
        return;
    }

    if (m_ParentActor)
    {
        auto& parentChildren = m_ParentActor->m_ChildActors;
        auto it = std::find(parentChildren.begin(), parentChildren.end(), this);
        if (it != parentChildren.end())
        {
            parentChildren.erase(it);
        }
        RP_LOG(WActorLog, Display, "[{}] detached from parent [{}]", GetName(), m_ParentActor->GetName());
    }

    m_ParentActor = newParent;
    newParent->addChildActor(this);

    if (m_RootComponent)
    {

        WTransformComponent* parentTransform = newParent->getRootComponent();
        if (parentTransform)
        {

            m_RootComponent->setRelativeLocation(FVector::Zero());
            m_RootComponent->setRelativeRotation(FQuat::Identity());
            m_RootComponent->setRelativeScale(FVector::One());
        }
    }

    RP_LOG(WActorLog, Display, "[{}] attached to parent [{}]", GetName(), newParent->GetName());
}

void WActor::DetachFromParent()
{
    if (!m_ParentActor) return;

    auto& parentChildren = m_ParentActor->m_ChildActors;
    auto it = std::find(parentChildren.begin(), parentChildren.end(), this);
    if (it != parentChildren.end())
    {
        parentChildren.erase(it);
    }

    m_ParentActor = nullptr;
    RP_LOG(WActorLog, Display, "[{}] detached from parent", GetName());
}

bool WActor::IsAttachedToActor(const WActor* potentialParent) const
{
    const WActor* current = this;
    while (current)
    {
        if (current == potentialParent) return true;
        current = current->GetAttachParentActor();
    }
    return false;
}

WActor* WActor::GetRootParent()
{
    WActor* root = this;
    while (root->GetAttachParentActor())
    {
        root = root->GetAttachParentActor();
    }
    return root;
}

const WActor* WActor::GetRootParent() const
{
    const WActor* root = this;
    while (root->GetAttachParentActor())
    {
        root = root->GetAttachParentActor();
    }
    return root;
}

void WActor::setActorLocation(const FVector& loc)
{
    if (m_RootComponent)
    {
        m_RootComponent->setLocation(loc);
    }
}

void WActor::setActorLocation(float x, float y, float z)
{
    setActorLocation(FVector(x, y, z));
}

void WActor::setActorRelativeLocation(const FVector& loc)
{
    if (m_RootComponent)
    {
        m_RootComponent->setRelativeLocation(loc);
    }
}

void WActor::setActorRelativeLocation(float x, float y, float z)
{
    setActorRelativeLocation(FVector(x, y, z));
}

FVector WActor::getActorLocation() const
{
    if (m_RootComponent)
    {
        return m_RootComponent->getLocation();
    }
    return FVector::Zero();
}

FVector WActor::getActorLocation()
{
    if (m_RootComponent)
    {
        return m_RootComponent->getLocation();
    }
    return FVector::Zero();
}

FVector WActor::getActorRelativeLocation() const
{
    if (m_RootComponent)
    {
        return m_RootComponent->getRelativeLocation();
    }
    return FVector::Zero();
}

FVector WActor::getActorRelativeLocation()
{
    if (m_RootComponent)
    {
        return m_RootComponent->getRelativeLocation();
    }
    return FVector::Zero();
}

void WActor::setActorRotation(const FQuat& rot)
{
    if (m_RootComponent)
    {
        m_RootComponent->setRotation(rot);
    }
}

void WActor::setActorRotation(float x, float y, float z, float w)
{
    setActorRotation(FQuat(x, y, z, w));
}

void WActor::setActorRotation(float pitch, float yaw, float roll)
{
    setActorRotation(FQuat(pitch, yaw, roll));
}

void WActor::setActorRotation(const FVector& eulerDegrees)
{
    float pitch = CEMath::DegreesToRadians(eulerDegrees.x);
    float yaw = CEMath::DegreesToRadians(eulerDegrees.y);
    float roll = CEMath::DegreesToRadians(eulerDegrees.z);
    setActorRotation(pitch, yaw, roll, 1.f);
}

void WActor::setActorRelativeRotation(const FQuat& rot)
{
    if (m_RootComponent)
    {
        m_RootComponent->setRelativeRotation(rot);
    }
}

void WActor::setActorRelativeRotation(float x, float y, float z, float w)
{
    setActorRelativeRotation(FQuat(x, y, z, w));
}

void WActor::setActorRelativeRotation(float pitch, float yaw, float roll)
{
    setActorRotation(FQuat(pitch, yaw, roll));
}

void WActor::setActorRelativeRotation(const FVector& eulerDegrees)
{
    float pitch = CEMath::DegreesToRadians(eulerDegrees.x);
    float yaw = CEMath::DegreesToRadians(eulerDegrees.y);
    float roll = CEMath::DegreesToRadians(eulerDegrees.z);
    setActorRotation(pitch, yaw, roll, 1.f);
}

FQuat WActor::getActorRotation() const
{
    if (m_RootComponent)
    {
        return m_RootComponent->getRotation();
    }
    return FQuat::Identity();
}

FQuat WActor::getActorRotation()
{
    if (m_RootComponent)
    {
        return m_RootComponent->getRotation();
    }
    return FQuat::Identity();
}

FQuat WActor::getActorRelativeRotation() const
{
    if (m_RootComponent)
    {
        return m_RootComponent->getRelativeRotation();
    }
    return FQuat::Identity();
}

FQuat WActor::getActorRelativeRotation()
{
    if (m_RootComponent)
    {
        return m_RootComponent->getRelativeRotation();
    }
    return FQuat::Identity();
}

void WActor::setActorScale(const FVector& scale)
{
    if (m_RootComponent)
    {
        m_RootComponent->setScale(scale);
    }
}

void WActor::setActorScale(float x, float y, float z)
{
    setActorScale(FVector(x, y, z));
}

void WActor::setActorScale(float scale)
{
    setActorScale(scale, scale, scale);
}

void WActor::setActorRelativeScale(const FVector& scale)
{
    if (m_RootComponent)
    {
        m_RootComponent->setRelativeScale(scale);
    }
}

void WActor::setActorRelativeScale(float x, float y, float z)
{
    setActorRelativeScale(FVector(x, y, z));
}

void WActor::setActorRelativeScale(float scale)
{
    setActorRelativeScale(scale, scale, scale);
}

FVector WActor::getActorScale() const
{
    if (m_RootComponent)
    {
        return m_RootComponent->getScale();
    }
    return FVector::One();
}

FVector WActor::getActorScale()
{
    if (m_RootComponent)
    {
        return m_RootComponent->getScale();
    }
    return FVector::One();
}

FVector WActor::getActorRelativeScale() const
{
    if (m_RootComponent)
    {
        return m_RootComponent->getRelativeScale();
    }
    return FVector::One();
}

FVector WActor::getActorRelativeScale()
{
    if (m_RootComponent)
    {
        return m_RootComponent->getRelativeScale();
    }
    return FVector::One();
}

void WActor::MoveActor(const FVector& Direction, float offset, bool bTeleport)
{
    if (CEMath::IsZero(offset)) return;
    if (Direction.IsZero()) return;
    const auto& currentLocation = getActorLocation();
    RP_LOG(WActorLog, Display, "moving actor {}  from {:.10f},{:.10f},{:.10f}", GetName(), currentLocation.x, currentLocation.y,
        currentLocation.z);
    FVector NewLocation;

    if (bTeleport)
    {
        NewLocation = currentLocation + (Direction * offset);
    }
    else
    {
        constexpr float MAX_DELTA_TIME = 0.05f;  // 50ms
        float deltaTime = (std::min)(m_deltaTime, MAX_DELTA_TIME);
        NewLocation = currentLocation + (Direction * offset * deltaTime);
    }
    RP_LOG(WActorLog, Display, "moving actor {}  to {:.10f},{:.10f},{:.10f}", GetName(), NewLocation.x, NewLocation.y, NewLocation.z);
    setActorLocation(NewLocation);
}

void WActor::LaunchActor(const FVector& Direction, float force, bool bTeleport)
{
    if (CEMath::IsZero(force)) return;
    if (Direction.IsZero()) return;

    FVector dir = Direction.IsNormalized() ? Direction : Direction.Normalized();

    float offset;
    if (bTeleport)
    {
        offset = force;
    }
    else
    {
        constexpr float MAX_DELTA_TIME = 0.05f;
        float deltaTime = (std::min)(m_deltaTime, MAX_DELTA_TIME);
        offset = force * deltaTime;
    }

    setActorLocation(getActorLocation() + (dir * offset));
}

FVector WActor::getActorForwardVector() const
{
    if (m_RootComponent)
    {
        return m_RootComponent->getRotation() * FVector::Forward();
    }
    return FVector::Forward();
}

FVector WActor::getActorRightVector() const
{
    if (m_RootComponent)
    {
        return m_RootComponent->getRotation() * FVector::Right();
    }
    return FVector::Right();
}

FVector WActor::getActorUpVector() const
{
    if (m_RootComponent)
    {
        return m_RootComponent->getRotation() * FVector::Up();
    }
    return FVector::Up();
}

void WActor::rotateYaw(float radians)
{
    addActorRotation(FQuat(FVector::Up(), radians));
}

void WActor::rotatePitch(float radians)
{
    addActorRotation(FQuat(FVector::Right(), radians));
}

void WActor::rotateRoll(float radians)
{
    addActorRotation(FQuat(FVector::Forward(), radians));
}

void WActor::rotateActor(const FQuat& rotation)
{
    setActorRotation(rotation);
}

void WActor::rotateActor(float pitch, float yaw, float roll)
{
    setActorRotation(FQuat(pitch, yaw, roll));
}

void WActor::addActorRotation(const FQuat& delta)
{
    FQuat current = getActorRotation();
    setActorRotation(delta * current);
}

void WActor::addActorRotation(float pitch, float yaw, float roll)
{
    addActorRotation(FQuat(pitch, yaw, roll));
}

void WActor::rotateRelativeYaw(float radians)
{
    addActorRelativeRotation(FQuat(getActorUpVector(), radians));
}

void WActor::rotateRelativePitch(float radians)
{
    addActorRelativeRotation(FQuat(getActorRightVector(), radians));
}

void WActor::rotateRelativeRoll(float radians)
{
    addActorRelativeRotation(FQuat(getActorForwardVector(), radians));
}

void WActor::rotateRelativeActor(const FQuat& rotation)
{
    setActorRelativeRotation(rotation);
}

void WActor::rotateRelativeActor(float pitch, float yaw, float roll)
{
    setActorRelativeRotation(FQuat(pitch, yaw, roll));
}

void WActor::addActorRelativeRotation(const FQuat& delta)
{
    setActorRelativeRotation(delta * getActorRelativeRotation());
}

void WActor::addActorRelativeRotation(float pitch, float yaw, float roll)
{
    addActorRelativeRotation(FQuat(pitch, yaw, roll));
}

std::vector<WActor*>& WActor::getChildActors()
{
    return m_ChildActors;
}

const std::vector<WActor*>& WActor::getChildActors() const
{
    return m_ChildActors;
}

}  // namespace RPE