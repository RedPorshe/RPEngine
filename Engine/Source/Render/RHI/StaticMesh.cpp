#include "StaticMesh.h"

using namespace RPE;

bool RPE::StaticMesh::loadFromFile(const std::string& filePath)
{
    return false;
}

bool RPE::StaticMesh::loadFromData(const std::vector<FVector>& vertices, const std::vector<uint32_t>& indices)
{
    return false;
}

void RPE::StaticMesh::addLODLevel(const StaticMeshLOD& lod) {}

const StaticMeshLOD& StaticMesh::getLODData(uint32_t level) const
{
    return m_lods[level];
}

void RPE::StaticMesh::unloadMesh() {}
