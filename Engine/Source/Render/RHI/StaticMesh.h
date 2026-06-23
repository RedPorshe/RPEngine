// Render/RHI/StaticMesh.h
#pragma once
#include "Mesh.h"
#include <vector>

namespace RPE
{

struct StaticMeshLOD
{
    std::vector<FVector> vertices;
    std::vector<uint32_t> indices;
    std::vector<FVector> normals;
    std::vector<FVector2D> uvs;
    FSphere boundingSphere;
    FAABB boundingBox;
};

class StaticMesh : public Mesh
{
public:
    StaticMesh() = default;
    ~StaticMesh() override = default;

    // Загрузка из файла
    bool loadFromFile(const std::string& filePath);
    bool loadFromData(const std::vector<FVector>& vertices, const std::vector<uint32_t>& indices);

    // LOD поддержка
    void addLODLevel(const StaticMeshLOD& lod);
    const StaticMeshLOD& getLODData(uint32_t level) const;
    uint32_t getLODCount() const { return static_cast<uint32_t>(m_lods.size()); }

    // Реализация Mesh
    const std::vector<FVector>& getVertices() const override { return m_lods[m_currentLOD].vertices; }
    const std::vector<uint32_t>& getIndices() const override { return m_lods[m_currentLOD].indices; }
    const std::vector<FVector>& getNormals() const override { return m_lods[m_currentLOD].normals; }
    const std::vector<FVector2D>& getUVs() const override { return m_lods[m_currentLOD].uvs; }

    FSphere getBoundingSphere() const override { return m_lods[m_currentLOD].boundingSphere; }
    FAABB getBoundingBox() const override { return m_lods[m_currentLOD].boundingBox; }

    std::string getName() const override { return m_name; }
    uint32_t getVertexCount() const override { return static_cast<uint32_t>(m_lods[m_currentLOD].vertices.size()); }
    uint32_t getIndexCount() const override { return static_cast<uint32_t>(m_lods[m_currentLOD].indices.size()); }

    void setCurrentLOD(uint32_t lod) { m_currentLOD = (std::min)(lod, getLODCount() - 1); }
    void setName(const std::string& name) { m_name = name; }

    void unloadMesh() override;

private:
    std::vector<StaticMeshLOD> m_lods;
    uint32_t m_currentLOD = 0;
    std::string m_name;

    // Inherited via Mesh
};

}  // namespace RPE