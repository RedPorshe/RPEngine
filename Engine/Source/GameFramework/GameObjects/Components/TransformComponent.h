#pragma once
#include "WorldActorComponent.h"
#include <vector>
#include "Math/MathTypes.h"

#include "Math/Vector3D.h"  // Для FVector
#include "Math/Quaternion.h"

namespace RPE
{

class WActor;

class WTransformComponent : public WActorComponent
{
    CHUDDO_DECLARE_CLASS(WTransformComponent, WActorComponent);

public:
    WTransformComponent(const std::string& inDisplayName = "Actor Component", CObject* inOwner = nullptr);
    virtual ~WTransformComponent();

    void tick(float deltaTime) override;
    void onDestroy() override;
    void onBeginPlay() override;

    // Установка трансформации (мировая)
    void setLocation(const FVector& loc);
    void setRotation(const FQuat& rot);
    void setScale(const FVector& scale);

    // Установка относительной трансформации (относительно родителя)
    void setRelativeLocation(const FVector& loc);
    void setRelativeRotation(const FQuat& rot);
    void setRelativeScale(const FVector& scale);

    // Получение трансформации (мировая)
    FVector getLocation() const;
    FQuat getRotation() const;
    FVector getScale() const;

    // Получение относительной трансформации
    FVector getRelativeLocation() const { return m_RelativeLocation; }
    FQuat getRelativeRotation() const { return m_RelativeRotation; }
    FVector getRelativeScale() const { return m_RelativeScale; }

    // Матрица
    FMat4& getMatrix();
    const FMat4& getMatrix() const;
    void updateTransform();

    // Относительный родитель
    bool hasParentTransform() const;
    WTransformComponent* getParentTransform() const;

protected:
    FVector m_Location = FVector::Zero();          // Локальная позиция (относительно родителя)
    FQuat m_Rotation = FQuat::Identity();          // Локальный поворот (относительно родителя)
    FVector m_Scale = FVector::One();              // Локальный масштаб (относительно родителя)
    FVector m_RelativeLocation = FVector::Zero();  // Дополнительное смещение
    FQuat m_RelativeRotation = FQuat::Identity();  // Дополнительный поворот
    FVector m_RelativeScale = FVector::One();      // Дополнительный масштаб
    FMat4 m_transformMatrix = FMat4(1.0f);

    bool isDirty{false};

    void serializeProperties(nlohmann::json& jsonObject) const override;
    void deserializeProperties(const nlohmann::json& jsonObject) override;

    bool isTransformDirty() const;
    void updateTransformMatrix();
    void markDirty();
    void markCleared();
    bool hasParentAsTransformComponent() const;

    FMat4 buildTransformMatrix(const FVector& location, const FQuat& rotation, const FVector& scale) const;
};

}  // namespace RPE