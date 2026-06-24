#pragma once
#include "Math/MathTypes.h"
#include <vector>
#include <string>
#include <cstdint>
namespace RPE
{

class Mesh
{
public:
    Mesh() = default;
    virtual ~Mesh() = default;

    virtual const std::vector<FVector>& getVertices() const = 0;
    virtual const std::vector<uint32_t>& getIndices() const = 0;
    virtual const std::vector<FVector>& getNormals() const = 0;
    virtual const std::vector<FVector2D>& getUVs() const = 0;
    virtual void unloadMesh() = 0;
    virtual FSphere getBoundingSphere() const = 0;
    virtual FAABB getBoundingBox() const = 0;
    bool isLoaded() const { return bisLoaded; }
    virtual std::string getName() const = 0;
    virtual uint32_t getVertexCount() const = 0;
    virtual uint32_t getIndexCount() const = 0;
    void SetIsLoaded(bool val) { bisLoaded = val; }

private:
    bool bisLoaded{false};
};

}  // namespace RPE