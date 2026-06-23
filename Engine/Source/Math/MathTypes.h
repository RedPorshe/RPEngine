#pragma once

#include <cmath>
#include "CE_MathConstants.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "CE_MathHelpers.h"

namespace CEMath
{
using Vec2 = Vector2D;
using Vec3 = Vector3D;
using Vec4 = Vector4D;
using Mat4 = Matrix4x4;
using Quat = Quaternion;
}  // namespace CEMath

using FVector2D = CEMath::Vec2;
using FVector = CEMath::Vec3;
using FVector4D = CEMath::Vec4;
using FMat4 = CEMath::Mat4;
using FQuat = CEMath::Quat;
using int32 = int;

struct FSphere
{
    FVector Center;
    float Radius;

    FSphere() : Center(FVector::Zero()), Radius(1.0f) {}
    FSphere(const FVector& center, float radius) : Center(center), Radius(radius) {}
};

struct FAABB
{
    FVector Min;
    FVector Max;

    FAABB() : Min(FVector::Zero()), Max(FVector::One()) {}
    FAABB(const FVector& min, const FVector& max) : Min(min), Max(max) {}

    FVector GetCenter() const { return (Min + Max) * 0.5f; }
    FVector GetSize() const { return Max - Min; }
    float GetVolume() const
    {
        FVector size = GetSize();
        return size.x * size.y * size.z;
    }

    bool Contains(const FVector& point) const
    {
        return point.x >= Min.x && point.x <= Max.x && point.y >= Min.y && point.y <= Max.y && point.z >= Min.z && point.z <= Max.z;
    }

    bool Intersects(const FAABB& other) const
    {
        return Min.x <= other.Max.x && Max.x >= other.Min.x && Min.y <= other.Max.y && Max.y >= other.Min.y && Min.z <= other.Max.z &&
               Max.z >= other.Min.z;
    }
};