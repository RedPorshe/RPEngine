#include "TransformComponent.h"
#include "../WorldObject/WorldActor.h"
#include "Math/MathTypes.h"
#include "Math/Vector3D.h"
#include "Log/Log.h"

using namespace RPE;
DEFINE_LOG_CATEGORY_STATIC(TransformLog);

REGISTER_CLASS_FACTORY(WTransformComponent);

WTransformComponent::WTransformComponent(const std::string& inDisplayName, CObject* inOwner)  //
    : Super(inDisplayName, inOwner)
{
    SetMovableState(EMovableState::Dynamic);
}

WTransformComponent::~WTransformComponent() {}

void WTransformComponent::tick(float deltaTime)
{
    Super::tick(deltaTime);
    if (isTransformDirty())
    {
        updateTransformMatrix();
    }
}

void WTransformComponent::onDestroy()
{
    Super::onDestroy();
}

void WTransformComponent::onBeginPlay()
{
    Super::onBeginPlay();
}

void WTransformComponent::setLocation(const FVector& loc)
{
    if (m_MovableState == EMovableState::Static) return;
    if (!hasParentAsTransformComponent())
    {
        m_Location = loc;
        m_RelativeLocation = FVector::Zero();
        markDirty();
        return;
    }

    WTransformComponent* parentTransform = getParentTransform();
    if (!parentTransform)
    {
        m_Location = loc;
        m_RelativeLocation = FVector::Zero();
        markDirty();
        return;
    }

    FVector parentLocation = parentTransform->getLocation();
    FQuat parentRotation = parentTransform->getRotation();
    FVector parentScale = parentTransform->getScale();

    FVector relativeLocation = loc - parentLocation;

    relativeLocation = parentRotation.Inverse() * relativeLocation;
    relativeLocation = FVector(relativeLocation.x / parentScale.x, relativeLocation.y / parentScale.y, relativeLocation.z / parentScale.z);

    m_RelativeLocation = relativeLocation;
    m_Location = FVector::Zero();
    markDirty();
}

void WTransformComponent::setRotation(const FQuat& rot)
{
    if (m_MovableState == EMovableState::Static) return;
    if (!hasParentAsTransformComponent())
    {
        m_Rotation = rot;
        m_RelativeRotation = FQuat::Identity();
        markDirty();
        return;
    }

    WTransformComponent* parentTransform = getParentTransform();
    if (!parentTransform)
    {
        m_Rotation = rot;
        m_RelativeRotation = FQuat::Identity();
        markDirty();
        return;
    }

    m_RelativeRotation = parentTransform->getRotation().Inverse() * rot;
    m_Rotation = FQuat::Identity();
    markDirty();
}

void WTransformComponent::setScale(const FVector& scale)
{
    if (m_MovableState == EMovableState::Static) return;
    if (!hasParentAsTransformComponent())
    {
        m_Scale = scale;
        m_RelativeScale = FVector::One();
        markDirty();
        return;
    }

    WTransformComponent* parentTransform = getParentTransform();
    if (!parentTransform)
    {
        m_Scale = scale;
        m_RelativeScale = FVector::One();
        markDirty();
        return;
    }

    FVector parentScale = parentTransform->getScale();

    m_RelativeScale = FVector(scale.x / parentScale.x, scale.y / parentScale.y, scale.z / parentScale.z);
    m_Scale = FVector::One();
    markDirty();
}

void WTransformComponent::setRelativeLocation(const FVector& loc)
{
    if (m_MovableState == EMovableState::Static) return;
    m_RelativeLocation = loc;
    markDirty();
}

void WTransformComponent::setRelativeRotation(const FQuat& rot)
{
    if (m_MovableState == EMovableState::Static) return;
    m_RelativeRotation = rot;
    markDirty();
}

void WTransformComponent::setRelativeScale(const FVector& scale)
{
    if (m_MovableState == EMovableState::Static) return;
    m_RelativeScale = scale;
    markDirty();
}

FVector WTransformComponent::getLocation() const
{
    if (!hasParentAsTransformComponent())
    {
        return m_Location + m_RelativeLocation;
    }

    WTransformComponent* parentTransform = getParentTransform();
    if (!parentTransform)
    {
        return m_Location + m_RelativeLocation;
    }

    FVector parentLocation = parentTransform->getLocation();
    FQuat parentRotation = parentTransform->getRotation();
    FVector parentScale = parentTransform->getScale();

    FVector relativePos = m_Location + m_RelativeLocation;
    FVector rotatedLocation = parentRotation * relativePos;
    FVector scaledLocation =
        FVector(rotatedLocation.x * parentScale.x, rotatedLocation.y * parentScale.y, rotatedLocation.z * parentScale.z);

    return parentLocation + scaledLocation;
}

FQuat WTransformComponent::getRotation() const
{

    if (!hasParentAsTransformComponent())
    {
        return m_Rotation * m_RelativeRotation;
    }

    WTransformComponent* parentTransform = getParentTransform();
    if (!parentTransform)
    {
        return m_Rotation * m_RelativeRotation;
    }

    return parentTransform->getRotation() * (m_Rotation * m_RelativeRotation);
}

FVector WTransformComponent::getScale() const
{
    if (!hasParentAsTransformComponent())
    {
        return FVector(m_Scale.x * m_RelativeScale.x, m_Scale.y * m_RelativeScale.y, m_Scale.z * m_RelativeScale.z);
    }

    WTransformComponent* parentTransform = getParentTransform();
    if (!parentTransform)
    {
        return FVector(m_Scale.x * m_RelativeScale.x, m_Scale.y * m_RelativeScale.y, m_Scale.z * m_RelativeScale.z);
    }

    FVector parentScale = parentTransform->getScale();

    return FVector(parentScale.x * m_Scale.x * m_RelativeScale.x, parentScale.y * m_Scale.y * m_RelativeScale.y,
        parentScale.z * m_Scale.z * m_RelativeScale.z);
}

FVector WTransformComponent::getRelativeLocation() const
{
    return m_RelativeLocation;
}

FQuat WTransformComponent::getRelativeRotation() const
{
    return m_RelativeRotation;
}

FVector RPE::WTransformComponent::getRelativeScale() const
{
    return m_RelativeScale;
}

FMat4& WTransformComponent::getMatrix()
{
    if (isTransformDirty())
    {
        updateTransformMatrix();
    }
    return m_transformMatrix;
}

const FMat4& WTransformComponent::getMatrix() const
{
    return m_transformMatrix;
}

void WTransformComponent::updateTransform()
{
    RP_LOG(TransformLog, Display, "forced update transform matrix for {}", GetName());
    updateTransformMatrix();
}

bool WTransformComponent::hasParentTransform() const
{
    return hasParentAsTransformComponent();
}

WTransformComponent* WTransformComponent::getParentTransform() const
{
    WActor* owner = getOwner();
    if (!owner) return nullptr;
    if (m_parent) return dynamic_cast<WTransformComponent*>(m_parent);
    WActor* parentActor = owner->GetAttachParentActor();
    if (!parentActor) return nullptr;

    return parentActor->getRootComponent();
}

void RPE::WTransformComponent::SetMovableState(EMovableState state)
{
    m_MovableState = state;
    std::string Statestr{""};
    switch (state)
    {
        case EMovableState::Static: Statestr = "Static"; break;
        case EMovableState::Movable: Statestr = "Movable"; break;
        case EMovableState::Dynamic: Statestr = "Dynamic"; break;
        default: break;
    }
    RP_LOG(TransformLog, Display, "Movable state changed to {}", Statestr);
}

void RPE::WTransformComponent::OnCreate()
{
    Super::OnCreate();
}

void WTransformComponent::serializeProperties(nlohmann::json& jsonObject) const
{
    Super::serializeProperties(jsonObject);

    jsonObject["Location"] = {{"x", m_Location.x}, {"y", m_Location.y}, {"z", m_Location.z}};
    jsonObject["RelativeLocation"] = {{"x", m_RelativeLocation.x}, {"y", m_RelativeLocation.y}, {"z", m_RelativeLocation.z}};

    jsonObject["Rotation"] = {{"x", m_Rotation.x}, {"y", m_Rotation.y}, {"z", m_Rotation.z}, {"w", m_Rotation.w}};
    jsonObject["RelativeRotation"] = {
        {"x", m_RelativeRotation.x}, {"y", m_RelativeRotation.y}, {"z", m_RelativeRotation.z}, {"w", m_RelativeRotation.w}};

    jsonObject["Scale"] = {{"x", m_Scale.x}, {"y", m_Scale.y}, {"z", m_Scale.z}};
    jsonObject["RelativeScale"] = {{"x", m_RelativeScale.x}, {"y", m_RelativeScale.y}, {"z", m_RelativeScale.z}};

    std::string stateStr;
    switch (m_MovableState)
    {
        case EMovableState::Static: stateStr = "Static"; break;
        case EMovableState::Movable: stateStr = "Movable"; break;
        case EMovableState::Dynamic: stateStr = "Dynamic"; break;
        default: stateStr = "Static"; break;
    }
    jsonObject["MovableState"] = stateStr;

    RP_LOG(TransformLog, Display, "TransformComponent '{}' serialized", GetName());
}

void WTransformComponent::deserializeProperties(const nlohmann::json& jsonObject)
{
    Super::deserializeProperties(jsonObject);

    if (jsonObject.contains("Location") && jsonObject["Location"].is_object())
    {
        float x = jsonObject["Location"].value("x", 0.0f);
        float y = jsonObject["Location"].value("y", 0.0f);
        float z = jsonObject["Location"].value("z", 0.0f);
        m_Location = FVector(x, y, z);
    }

    if (jsonObject.contains("RelativeLocation") && jsonObject["RelativeLocation"].is_object())
    {
        float x = jsonObject["RelativeLocation"].value("x", 0.0f);
        float y = jsonObject["RelativeLocation"].value("y", 0.0f);
        float z = jsonObject["RelativeLocation"].value("z", 0.0f);
        m_RelativeLocation = FVector(x, y, z);
    }

    if (jsonObject.contains("Rotation") && jsonObject["Rotation"].is_object())
    {
        float x = jsonObject["Rotation"].value("x", 0.0f);
        float y = jsonObject["Rotation"].value("y", 0.0f);
        float z = jsonObject["Rotation"].value("z", 0.0f);
        float w = jsonObject["Rotation"].value("w", 1.0f);
        m_Rotation = FQuat(x, y, z, w);
    }

    if (jsonObject.contains("RelativeRotation") && jsonObject["RelativeRotation"].is_object())
    {
        float x = jsonObject["RelativeRotation"].value("x", 0.0f);
        float y = jsonObject["RelativeRotation"].value("y", 0.0f);
        float z = jsonObject["RelativeRotation"].value("z", 0.0f);
        float w = jsonObject["RelativeRotation"].value("w", 1.0f);
        m_RelativeRotation = FQuat(x, y, z, w);
    }

    if (jsonObject.contains("Scale") && jsonObject["Scale"].is_object())
    {
        float x = jsonObject["Scale"].value("x", 1.0f);
        float y = jsonObject["Scale"].value("y", 1.0f);
        float z = jsonObject["Scale"].value("z", 1.0f);
        m_Scale = FVector(x, y, z);
    }

    if (jsonObject.contains("RelativeScale") && jsonObject["RelativeScale"].is_object())
    {
        float x = jsonObject["RelativeScale"].value("x", 1.0f);
        float y = jsonObject["RelativeScale"].value("y", 1.0f);
        float z = jsonObject["RelativeScale"].value("z", 1.0f);
        m_RelativeScale = FVector(x, y, z);
    }

    if (jsonObject.contains("MovableState") && jsonObject["MovableState"].is_string())
    {
        std::string stateStr = jsonObject["MovableState"].get<std::string>();
        if (stateStr == "Static")
            m_MovableState = EMovableState::Static;
        else if (stateStr == "Movable")
            m_MovableState = EMovableState::Movable;
        else if (stateStr == "Dynamic")
            m_MovableState = EMovableState::Dynamic;
    }

    updateTransform();

    RP_LOG(TransformLog, Display, "TransformComponent '{}' deserialized", GetName());
}

bool WTransformComponent::isTransformDirty() const
{
    return isDirty;
}

void WTransformComponent::updateTransformMatrix()
{
    FVector relativePos = m_Location + m_RelativeLocation;
    FQuat relativeRot = m_Rotation * m_RelativeRotation;
    FVector relativeScale = FVector(m_Scale.x * m_RelativeScale.x, m_Scale.y * m_RelativeScale.y, m_Scale.z * m_RelativeScale.z);

    FMat4 localMatrix = buildTransformMatrix(relativePos, relativeRot, relativeScale);

    if (hasParentAsTransformComponent())
    {
        WTransformComponent* parentTransform = getParentTransform();
        if (parentTransform)
        {
            m_transformMatrix = parentTransform->getMatrix() * localMatrix;
            markCleared();
            return;
        }
    }

    m_transformMatrix = localMatrix;
    markCleared();
}

FMat4 WTransformComponent::buildTransformMatrix(const FVector& location, const FQuat& rotation, const FVector& scale) const
{
    FMat4 translationMatrix = FMat4::Translation(location);
    FMat4 rotationMatrix = rotation.ToMatrix();
    FMat4 scaleMatrix = FMat4::Scaling(scale);

    return translationMatrix * rotationMatrix * scaleMatrix;
}

void WTransformComponent::markDirty()
{
    isDirty = true;
}

void WTransformComponent::markCleared()
{
    isDirty = false;
}

bool WTransformComponent::hasParentAsTransformComponent() const
{
    if (m_parent)
    {
        if (WActorComponent* papa = dynamic_cast<WTransformComponent*>(m_parent))
        {
            return true;
        }
    }
    WActor* owner = getOwner();
    if (!owner) return false;

    WActor* parentActor = owner->GetAttachParentActor();
    if (!parentActor) return false;

    return dynamic_cast<WTransformComponent*>(parentActor->getRootComponent()) != nullptr;
}

void WTransformComponent::attachTo(WActorComponent* inOwnComponent)
{
    if (inOwnComponent == nullptr) return;
    if (this == inOwnComponent) return;
    if (m_parent == inOwnComponent) return;

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
        RP_LOG(TransformLog, Error, "[{}] failed to attach to component [{}]", GetName(), inOwnComponent->GetName());
        return;
    }
    RP_LOG(TransformLog, Display, "[{}] attached to component [{}]", GetName(), inOwnComponent->GetName());
}
