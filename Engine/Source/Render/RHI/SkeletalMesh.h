//// Render/RHI/SkeletalMesh.h
// #pragma once
// #include "Mesh.h"
// #include <vector>
//
// namespace RPE
//{
//
// struct Bone
//{
//     std::string name;
//     int32_t parentIndex = -1;
//     FMat4 bindPose;  // Локальная трансформация в позе привязки
// };
//
// struct SkinWeight
//{
//     uint32_t boneIndices[4] = {0, 0, 0, 0};
//     float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};
// };
//
// class SkeletalMesh : public Mesh
//{
// public:
//     SkeletalMesh() = default;
//     ~SkeletalMesh() override = default;
//
//     bool loadFromFile(const std::string& filePath);
//
//     // Управление скелетом
//     void setBones(const std::vector<Bone>& bones);
//     const std::vector<Bone>& getBones() const { return m_bones; }
//     uint32_t getBoneCount() const { return static_cast<uint32_t>(m_bones.size()); }
//     const std::vector<FMat4>& getBindPose() const { return m_bindPose; }
//
//     // Скиннинг данные
//     void setSkinningData(const std::vector<SkinWeight>& weights);
//     const std::vector<SkinWeight>& getSkinningData() const { return m_skinningData; }
//
//     // Blend shapes (морфинг)
//     void setBlendShapeCount(uint32_t count);
//     uint32_t getBlendShapeCount() const { return m_blendShapeCount; }
//
//     // Реализация Mesh
//     const std::vector<FVector>& getVertices() const override { return m_vertices; }
//     const std::vector<uint32_t>& getIndices() const override { return m_indices; }
//     const std::vector<FVector>& getNormals() const override { return m_normals; }
//     const std::vector<FVector2D>& getUVs() const override { return m_uvs; }
//
//     FSphere getBoundingSphere() const override { return m_boundingSphere; }
//     FAABB getBoundingBox() const override { return m_boundingBox; }
//
//     std::string getName() const override { return m_name; }
//     uint32_t getVertexCount() const override { return static_cast<uint32_t>(m_vertices.size()); }
//     uint32_t getIndexCount() const override { return static_cast<uint32_t>(m_indices.size()); }
//
//     void setName(const std::string& name) { m_name = name; }
//
// private:
//     std::vector<Bone> m_bones;
//     std::vector<FMat4> m_bindPose;
//     std::vector<SkinWeight> m_skinningData;
//     uint32_t m_blendShapeCount = 0;
//
//     std::vector<FVector> m_vertices;
//     std::vector<uint32_t> m_indices;
//     std::vector<FVector> m_normals;
//     std::vector<FVector2D> m_uvs;
//     FSphere m_boundingSphere;
//     FAABB m_boundingBox;
//     std::string m_name;
// };
//
// }  // namespace RPE