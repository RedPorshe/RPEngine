// MeshComponent.cpp
#include "MeshComponent.h"
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(MeshComponentLog);

namespace RPE
{

Material* MeshComponent::s_defaultMaterial = nullptr;

MeshComponent::MeshComponent(const std::string& inName, CObject* inOwner) : Super(inName, inOwner)
{
    SetMovableState(EMovableState::Dynamic);
    // По умолчанию устанавливаем дефолтный материал
    m_material = getDefaultMaterial();
}

void MeshComponent::tick(float deltaTime)
{
    Super::tick(deltaTime);

    if (m_bIsDirty)
    {
        updateMeshData();
        m_bIsDirty = false;
    }
}

void MeshComponent::onDestroy()
{
    Super::onDestroy();
    // Не удаляем материал, т.к. он может быть общим
    m_material = nullptr;
    m_mesh = nullptr;
}

void MeshComponent::onBeginPlay()
{
    Super::onBeginPlay();
    updateMeshData();
}

void MeshComponent::OnCreate()
{
    Super::OnCreate();
    RP_LOG(MeshComponentLog, Display, "MeshComponent '{}' created", GetName());
}

void MeshComponent::setMaterial(Material* material)
{
    if (!material)
    {
        RP_LOG(MeshComponentLog, Warning, "Null material provided for '{}', using default", GetName());
        material = getDefaultMaterial();
    }

    if (m_material == material) return;

    // Если материал не загружен - пытаемся загрузить
    if (!material->isLoaded())
    {
        if (!material->loadMaterial(material->getName()))
        {
            RP_LOG(MeshComponentLog, Error, "Failed to load material '{}', using default", material->getName());
            material = getDefaultMaterial();
        }
    }

    m_material = material;
    onMaterialChanged();
    m_bIsDirty = true;

    RP_LOG(MeshComponentLog, Display, "Material '{}' set for '{}'", material->getName(), GetName());
}

void MeshComponent::setMaterial(const std::string& materialName)
{
    if (materialName.empty())
    {
        RP_LOG(MeshComponentLog, Warning, "Empty material name for '{}', using default", GetName());
        m_material = getDefaultMaterial();
        return;
    }

    m_materialName = materialName;

    Material* material = new Material();
    material->setName(materialName);

    if (!material->loadMaterial(materialName))
    {
        RP_LOG(MeshComponentLog, Error, "Failed to load material '{}', using default", materialName);
        delete material;
        m_material = getDefaultMaterial();
        return;
    }

    setMaterial(material);
}

void MeshComponent::setVisible(bool visible)
{
    m_bIsVisible = visible;
}

bool MeshComponent::isVisible() const
{
    return m_bIsVisible;
}

void MeshComponent::setCastShadow(bool cast)
{
    m_bCastShadow = cast;
}

bool MeshComponent::isCastingShadow() const
{
    return m_bCastShadow;
}

Material* MeshComponent::getDefaultMaterial()
{
    if (!s_defaultMaterial)
    {
        s_defaultMaterial = new Material();
        s_defaultMaterial->setName("DefaultMaterial");

        // Базовые параметры материала
        s_defaultMaterial->setDiffuseColor(FVector(0.5f, 0.5f, 0.5f));
        s_defaultMaterial->setSpecularColor(FVector(0.1f, 0.1f, 0.1f));
        s_defaultMaterial->setShininess(16.0f);
        s_defaultMaterial->setOpacity(1.0f);

        RP_LOG(MeshComponentLog, Display, "Default material created");
    }

    return s_defaultMaterial;
}

void MeshComponent::setMesh(Mesh* mesh)
{
    if (!mesh) return;
    if (!mesh->isLoaded()) return;

    if (m_mesh == mesh) return;
    if (m_mesh && !m_mesh->isLoaded())
    {
        m_mesh = mesh;
    }
    else if (m_mesh)
    {
        m_mesh->unloadMesh();
        m_mesh = mesh;
    }
    else
    {
        m_mesh = mesh;
    }
    updateBoundingVolumes();
    m_bIsDirty = true;
}

void MeshComponent::cleanupDefaultMaterial()
{
    delete s_defaultMaterial;
    s_defaultMaterial = nullptr;
    RP_LOG(MeshComponentLog, Display, "Default material destroyed");
}

void MeshComponent::updateMeshData()
{
    // Базовый класс ничего не обновляет
    // Наследники могут переопределить
}

void MeshComponent::onMaterialChanged()
{
    // Базовый класс ничего не делает
    // Наследники могут переопределить
}

void MeshComponent::updateBoundingVolumes()
{
    if (m_mesh)
    {
        //  m_boundingSphere = m_mesh->getBoundingSphere();
        //  m_boundingBox = m_mesh->getBoundingBox();
    }
    else
    {
        //  m_boundingSphere = FSphere(FVector::Zero(), 1.0f);
        // m_boundingBox = FAABB(FVector(-1.0f), FVector(1.0f));
    }
}

}  // namespace RPE