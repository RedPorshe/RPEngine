#pragma once
#include "Math/MathTypes.h"
#include <vector>
#include <vulkan/vulkan.h>

struct CameraData
{
    FMat4 viewMatrix{FMat4::IdentityMatrix()};
    FMat4 projectionMatrix{FMat4::IdentityMatrix()};
    FVector CameraPosition{FVector::Zero()};
    FVector CameraDirection{FVector::Forward()};
    void reset()
    {
        viewMatrix = FMat4::IdentityMatrix();
        projectionMatrix = FMat4::IdentityMatrix();
        CameraPosition = FVector::Zero();
        CameraDirection = FVector::Forward();
    }
};

struct FStaticMeshObject
{
    FMat4 Model = FMat4(1.f);
    VkBuffer VertexBuffer = VK_NULL_HANDLE;
    VkBuffer IndexBuffer = VK_NULL_HANDLE;
    uint32_t VertexCount = 0;
    uint32_t IndexCount = 0;
    uint32_t MaterialId = 0;
    std::string PipelineName = "StaticMesh";

    FStaticMeshObject() = default;
    FStaticMeshObject(VkBuffer InVertexBuffer, uint32_t InVertexCount, VkBuffer InIndexBuffer = VK_NULL_HANDLE, uint32_t InIndexCount = 0,
        const FMat4& InModel = FMat4(1.f), const std::string& InPipelineName = "StaticMesh");

    void Clear();
    bool IsValid() const;
};
struct FShadowCaster
{
};

struct FrameInfo
{
    CameraData camera;
    std::vector<FStaticMeshObject> staticMeshes;
    void AddMesh(const FStaticMeshObject& mesh);
    void clear();
    bool isValid() const;
    FrameInfo() = default;
    FrameInfo(const FrameInfo&) = default;
    FrameInfo& operator=(const FrameInfo&) = default;
    ~FrameInfo() = default;
};