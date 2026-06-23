// MeshComponent.h
#pragma once
#include "TransformComponent.h"
#include "Render/RHI/Mesh.h"
#include "Render/RHI/Material.h"

namespace RPE
{

class MeshComponent : public WTransformComponent
{
    CHUDDO_DECLARE_CLASS(MeshComponent, WTransformComponent);

public:
    MeshComponent(const std::string& inName = "MeshComponent", CObject* inOwner = nullptr);
    virtual ~MeshComponent() = default;

    void tick(float deltaTime) override;
    void onDestroy() override;
    void onBeginPlay() override;
    void OnCreate() override;

    // Управление материалом
    void setMaterial(Material* material);
    void setMaterial(const std::string& materialName);
    Material* getMaterial() const { return m_material; }

    // Управление видимостью
    void setVisible(bool visible);
    bool isVisible() const;
    void setCastShadow(bool cast);
    bool isCastingShadow() const;

    // Управление мешем
    virtual Mesh* getMesh() const { return m_mesh; }
    void setMesh(Mesh* mesh);

    // Bounding volumes
    virtual FSphere getBoundingSphere() const { return m_boundingSphere; }
    virtual FAABB getBoundingBox() const { return m_boundingBox; }
    virtual void updateBoundingVolumes();

    static void cleanupDefaultMaterial();

protected:
    Mesh* m_mesh = nullptr;
    Material* m_material = nullptr;

    FSphere m_boundingSphere = FSphere(FVector::Zero(), 1.0f);
    FAABB m_boundingBox = FAABB(FVector(-1.0f), FVector(1.0f));

    bool m_bIsVisible = true;
    bool m_bCastShadow = true;
    bool m_bReceiveShadow = true;
    bool m_bIsDirty = false;

    virtual void updateMeshData();
    virtual void onMaterialChanged();

private:
    std::string m_materialName;
    static Material* s_defaultMaterial;
    static Material* getDefaultMaterial();
};

REGISTER_CLASS_FACTORY(MeshComponent);

}  // namespace RPE