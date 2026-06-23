#pragma once
#include <cstdint>
namespace RPE
{
enum class EMovableState : uint8_t
{
    Static,
    Movable,
    Dynamic
};

enum class ECollisionShape : uint8_t
{
    NO_COLLISION = 0,
    Capsule,
    Sphere,
    Cube,
    Plane,
};
}  // namespace RPE