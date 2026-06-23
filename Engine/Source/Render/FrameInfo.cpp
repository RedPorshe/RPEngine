#include "FrameInfo.h"

FStaticMeshObject::FStaticMeshObject(VkBuffer InVertexBuffer, uint32_t InVertexCount, VkBuffer InIndexBuffer, uint32_t InIndexCount,
    const FMat4& InModel, const std::string& InPipelineName)
    : Model(InModel), VertexBuffer(InVertexBuffer), IndexBuffer(InIndexBuffer), VertexCount(InVertexCount), IndexCount(InIndexCount),
      MaterialId(0), PipelineName(InPipelineName)

{
}

void FStaticMeshObject::Clear()
{
    Model = FMat4(1.f);
    VertexBuffer = VK_NULL_HANDLE;
    IndexBuffer = VK_NULL_HANDLE;
    VertexCount = 0;
    IndexCount = 0;
    MaterialId = 0;
    PipelineName.clear();
}

bool FStaticMeshObject::IsValid() const
{
    return VertexBuffer != VK_NULL_HANDLE && VertexCount > 0;
}

void FrameInfo::AddMesh(const FStaticMeshObject& mesh)
{
    staticMeshes.push_back(mesh);
}

void FrameInfo::clear()
{
    camera.reset();
    staticMeshes.clear();
}

bool FrameInfo::isValid() const
{
    return !staticMeshes.empty();
}
