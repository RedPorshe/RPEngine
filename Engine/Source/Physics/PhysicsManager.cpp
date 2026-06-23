#include "PhysicsManager.h"
#include "GameFrameWork/Collision/CollisionComponent.h"
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(PhysManagerLog);

using namespace RPE;

PhysicsManager::PhysicsManager()
{
    RP_LOG(PhysManagerLog, Display, " Created");
}

bool PhysicsManager::init()
{
    bisInitialized = true;
    RP_LOG(PhysManagerLog, Display, "PhysicsManager initialized");
    return true;
}

void PhysicsManager::update(float DeltaTime)
{
    if (!bisInitialized) return;
    if (m_collisionComponents.empty()) return;
    CheckCollisions();
}

void PhysicsManager::shutdown()
{
    if (!m_collisionComponents.empty())
    {
        std::vector<std::string> keys;
        keys.reserve(m_collisionComponents.size());
        for (const auto& [key, comp] : m_collisionComponents)
        {
            keys.push_back(key);
        }

        for (const auto& key : keys)
        {
            auto it = m_collisionComponents.find(key);
            if (it != m_collisionComponents.end())
            {
                CollisionComponent* comp = it->second;
                if (comp)
                {
                    RP_LOG(PhysManagerLog, Display, "Unregistering collision component: {}", key);
                }
                m_collisionComponents.erase(it);
            }
        }
    }
    if (!m_collisionComponentsNames.empty())
    {
        m_collisionComponentsNames.clear();
    }

    bisInitialized = false;
    RP_LOG(PhysManagerLog, Display, "PhysicsManager shutdown complete");
}

void PhysicsManager::registerPhysicsComponent() {}

void PhysicsManager::registerCollisionComponent(CollisionComponent* component)
{
    if (!component) return;
    const std::string& name = component->GetName();
    auto it = m_collisionComponents.find(name);
    if (it == m_collisionComponents.end())
    {
        m_collisionComponents[component->GetName()] = component;
        m_collisionComponentsNames.push_back(component->GetName());
        RP_LOG(PhysManagerLog, Display, "{} collision component registered", component->GetName());
    }
}

void PhysicsManager::unregisterCollisionComponent(CollisionComponent* component)
{
    if (!component) return;
    auto it = m_collisionComponents.find(component->GetName());
    if (it != m_collisionComponents.end())
    {
        RP_LOG(PhysManagerLog, Display, "{} collision component unregistered", component->GetName());
        m_collisionComponents.erase(it);
        auto it2 = std::find(m_collisionComponentsNames.begin(), m_collisionComponentsNames.end(), component->GetName());
        if (it2 != m_collisionComponentsNames.end()) m_collisionComponentsNames.erase(it2);
    }
}

void PhysicsManager::CheckCollisions()
{
    if (m_collisionComponentsNames.size() < 2) return;
    for (size_t i = 0; i < m_collisionComponentsNames.size(); ++i)
    {
        for (size_t j = i + 1; j < m_collisionComponentsNames.size(); ++j)
        {
            auto compA = m_collisionComponentsNames[i];
            auto compB = m_collisionComponentsNames[j];
            if (!allReadyOverlapped(compA, compB))
            {
                if (CheckOverlaping(compA, compB))
                {
                    onCollision(compA, compB);
                }
            }
            else
            {
                if (!CheckOverlaping(compA, compB))
                {
                    endCollision(compA, compB);
                }
            }
        }
    }
}

void PhysicsManager::onCollision(const std::string& compA, const std::string& compB)
{
    auto itA = m_collisionComponents.find(compA);
    auto itB = m_collisionComponents.find(compB);
    if (itA == m_collisionComponents.end() || itB == m_collisionComponents.end()) return;

    CollisionComponent* compAptr = itA->second;
    CollisionComponent* compBptr = itB->second;

    auto& overlapsA = compAptr->overlapedCollisions;
    bool alreadyOverlapping = std::find(overlapsA.begin(), overlapsA.end(), compBptr) != overlapsA.end();

    if (!alreadyOverlapping)
    {
        compAptr->onBeginOverlap(compBptr);
        compBptr->onBeginOverlap(compAptr);
    }
}

void RPE::PhysicsManager::endCollision(const std::string& compA, const std::string& compB)
{
    auto itA = m_collisionComponents.find(compA);
    auto itB = m_collisionComponents.find(compB);
    if (itA == m_collisionComponents.end() || itB == m_collisionComponents.end()) return;
    CollisionComponent* compAptr = itA->second;
    CollisionComponent* compBptr = itB->second;
    compAptr->onEndOverlap(compBptr);
    compBptr->onEndOverlap(compAptr);
}

bool PhysicsManager::CheckOverlaping(const std::string& compA, const std::string& compB)
{
    if (compA.empty() || compB.empty()) return false;
    if (compA == compB) return false;
    auto itA = m_collisionComponents.find(compA);
    auto itB = m_collisionComponents.find(compB);
    if (itA == m_collisionComponents.end() || itB == m_collisionComponents.end()) return false;

    CollisionComponent* compAptr = itA->second;
    CollisionComponent* compBptr = itB->second;
    if (compAptr->getOwner() == compBptr->getOwner()) return false;
    ECollisionShape shapeA = compAptr->getCollisionShape();
    ECollisionShape shapeB = compBptr->getCollisionShape();

    switch (shapeA)
    {
        case ECollisionShape::NO_COLLISION:
        {
            return false;
        }
        case ECollisionShape::Capsule:
        {
            switch (shapeB)
            {
                case ECollisionShape::NO_COLLISION: return false;
                case ECollisionShape::Capsule: return CheckCapsuleVsCapsule(compAptr, compBptr);
                case ECollisionShape::Sphere: return CheckSphereVsCapsule(compAptr, compBptr);
                case ECollisionShape::Cube: return CheckCubeVsCapsule(compAptr, compBptr);
                case ECollisionShape::Plane: return CheckCapsuleVsPlane(compAptr, compBptr);
                default: return false;
            }
        }
        case ECollisionShape::Sphere:
        {
            switch (shapeB)
            {
                case ECollisionShape::NO_COLLISION: return false;
                case ECollisionShape::Capsule: return CheckSphereVsCapsule(compAptr, compBptr);
                case ECollisionShape::Sphere: return CheckSphereSphere(compAptr, compBptr);
                case ECollisionShape::Cube: return CheckSphereVsCube(compAptr, compBptr);
                case ECollisionShape::Plane: return CheckSphereVsPlane(compAptr, compBptr);
                default: return false;
            }
        }
        case ECollisionShape::Cube:
        {
            switch (shapeB)
            {
                case ECollisionShape::NO_COLLISION: return false;
                case ECollisionShape::Capsule: return CheckCubeVsCapsule(compAptr, compBptr);
                case ECollisionShape::Sphere: return CheckSphereVsCube(compAptr, compBptr);
                case ECollisionShape::Cube: return CheckCubeVsCube(compAptr, compBptr);
                case ECollisionShape::Plane: return CheckCubeVsPlane(compAptr, compBptr);
                default: return false;
            }
        }
        case ECollisionShape::Plane:
        {
            switch (shapeB)
            {
                case RPE::ECollisionShape::NO_COLLISION: return false;
                case RPE::ECollisionShape::Capsule: return CheckCapsuleVsPlane(compAptr, compBptr);
                case RPE::ECollisionShape::Sphere: return CheckSphereVsPlane(compAptr, compBptr);
                case RPE::ECollisionShape::Cube: return CheckCubeVsPlane(compAptr, compBptr);
                case RPE::ECollisionShape::Plane: return CheckPlaneVsPlane(compAptr, compBptr);
                default: return false;
            }
        }
        default: return false;
    }
}

bool PhysicsManager::allReadyOverlapped(const std::string& compA, const std::string& compB)
{
    auto itA = m_collisionComponents.find(compA);
    auto itB = m_collisionComponents.find(compB);
    if (itA == m_collisionComponents.end() || itB == m_collisionComponents.end()) return false;

    CollisionComponent* compAptr = itA->second;
    CollisionComponent* compBptr = itB->second;

    auto& overlapsA = compAptr->overlapedCollisions;
    return std::find(overlapsA.begin(), overlapsA.end(), compBptr) != overlapsA.end();
}

float PhysicsManager::ClosestPointBetweenSegments(
    const FVector& p1, const FVector& q1, const FVector& p2, const FVector& q2, FVector& outClosest1, FVector& outClosest2)
{
    FVector d1 = q1 - p1;
    FVector d2 = q2 - p2;
    FVector r = p1 - p2;

    float a = d1.Dot(d1);
    float e = d2.Dot(d2);
    float f = d2.Dot(r);

    float s, t;

    if (a <= 0.0001f && e <= 0.0001f)
    {
        outClosest1 = p1;
        outClosest2 = p2;
        return outClosest1.DistanceSquared(outClosest2);
    }

    if (a <= 0.0001f)
    {
        s = 0.0f;
        t = CEMath::Clamp(f / e, 0.0f, 1.0f);
    }
    else
    {
        float c = d1.Dot(r);
        if (e <= 0.0001f)
        {
            t = 0.0f;
            s = CEMath::Clamp(-c / a, 0.0f, 1.0f);
        }
        else
        {
            float b = d1.Dot(d2);
            float denom = a * e - b * b;

            if (denom != 0.0f)
            {
                s = CEMath::Clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            }
            else
            {
                s = 0.0f;
            }

            t = (b * s + f) / e;
            if (t < 0.0f)
            {
                t = 0.0f;
                s = CEMath::Clamp(-c / a, 0.0f, 1.0f);
            }
            else if (t > 1.0f)
            {
                t = 1.0f;
                s = CEMath::Clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }

    outClosest1 = p1 + d1 * s;
    outClosest2 = p2 + d2 * t;

    return outClosest1.DistanceSquared(outClosest2);
}

bool PhysicsManager::CheckPlaneRectanglesOverlap(CollisionComponent* planeA, CollisionComponent* planeB)
{
    if (!planeA || !planeB) return false;

    FVector normalA = planeA->getPlaneNormal();
    FVector normalB = planeB->getPlaneNormal();
    FVector centerA = planeA->getPlanePoint();
    FVector centerB = planeB->getPlanePoint();
    FVector2D sizeA = planeA->getPlaneSize();
    FVector2D sizeB = planeB->getPlaneSize();

    FVector upA = FVector(0, 1, 0);
    if (std::abs(normalA.Dot(upA)) > 0.99f) upA = FVector(1, 0, 0);

    FVector rightA = normalA.Cross(upA).Normalized();
    FVector localUpA = rightA.Cross(normalA).Normalized();

    FVector upB = FVector(0, 1, 0);
    if (std::abs(normalB.Dot(upB)) > 0.99f) upB = FVector(1, 0, 0);

    FVector rightB = normalB.Cross(upB).Normalized();
    FVector localUpB = rightB.Cross(normalB).Normalized();

    // Проверяем, что плоскости не параллельны
    FVector crossProduct = normalA.Cross(normalB);
    float crossLengthSquared = crossProduct.LengthSquared();

    if (crossLengthSquared < 0.0001f)
    {
        return false;
    }

    FVector lineDirection = crossProduct / std::sqrt(crossLengthSquared);

    float d1 = centerA.Dot(normalA);
    float d2 = centerB.Dot(normalB);
    float denom = crossLengthSquared;

    FVector pointOnLine = (normalB.Cross(lineDirection) * d1 + lineDirection.Cross(normalA) * d2) / denom;

    float tMinA = -INFINITY, tMaxA = INFINITY;
    bool hasIntersectionA = GetLineRectangleIntersection(pointOnLine, lineDirection, centerA, rightA, localUpA, sizeA, tMinA, tMaxA);

    if (!hasIntersectionA) return false;

    float tMinB = -INFINITY, tMaxB = INFINITY;
    bool hasIntersectionB = GetLineRectangleIntersection(pointOnLine, lineDirection, centerB, rightB, localUpB, sizeB, tMinB, tMaxB);

    if (!hasIntersectionB) return false;

    return tMinA <= tMaxB && tMinB <= tMaxA;
}

bool RPE::PhysicsManager::GetLineRectangleIntersection(const FVector& linePoint, const FVector& lineDirection, const FVector& rectCenter,
    const FVector& rectRight, const FVector& rectUp, const FVector2D& rectSize, float& outTMin, float& outTMax)
{
    float halfWidth = rectSize.x * 0.5f;
    float halfHeight = rectSize.y * 0.5f;

    // Проектируем линию на локальные координаты прямоугольника
    FVector toPoint = linePoint - rectCenter;
    float x0 = toPoint.Dot(rectRight);
    float y0 = toPoint.Dot(rectUp);
    float dx = lineDirection.Dot(rectRight);
    float dy = lineDirection.Dot(rectUp);

    // Если линия параллельна плоскости прямоугольника
    if (std::abs(dx) < 0.0001f && std::abs(dy) < 0.0001f)
    {
        // Линия параллельна - проверяем, находится ли она внутри
        if (std::abs(x0) <= halfWidth && std::abs(y0) <= halfHeight)
        {
            outTMin = -INFINITY;
            outTMax = INFINITY;
            return true;
        }
        return false;
    }

    // Находим интервалы пересечения по X и Y
    float tMinX = -INFINITY, tMaxX = INFINITY;
    float tMinY = -INFINITY, tMaxY = INFINITY;

    if (std::abs(dx) >= 0.0001f)
    {
        tMinX = (-halfWidth - x0) / dx;
        tMaxX = (halfWidth - x0) / dx;
        if (tMinX > tMaxX) std::swap(tMinX, tMaxX);
    }
    else if (std::abs(x0) >= halfWidth)
    {
        return false;
    }

    if (std::abs(dy) > 0.0001f)
    {
        tMinY = (-halfHeight - y0) / dy;
        tMaxY = (halfHeight - y0) / dy;
        if (tMinY > tMaxY) std::swap(tMinY, tMaxY);
    }
    else if (std::abs(y0) >= halfHeight)
    {
        return false;
    }

    outTMin = std::max(tMinX, tMinY);
    outTMax = std::min(tMaxX, tMaxY);

    return outTMin <= outTMax;
}

bool PhysicsManager::CheckSphereSphere(CollisionComponent* compA, CollisionComponent* compB)
{
    if (!compA || !compB) return false;

    FVector locA = compA->getLocation();
    FVector locB = compB->getLocation();
    float radiusA = compA->getRadius();
    float radiusB = compB->getRadius();

    float distanceSquared = locA.DistanceSquared(locB);
    float radiusSum = radiusA + radiusB;

    return distanceSquared <= (radiusSum * radiusSum);
}

bool PhysicsManager::CheckSphereVsCube(CollisionComponent* compA, CollisionComponent* compB)
{
    if (!compA || !compB) return false;

    CollisionComponent* sphere = nullptr;
    CollisionComponent* cube = nullptr;

    if (compA->getCollisionShape() == ECollisionShape::Cube)
    {
        sphere = compB;
        cube = compA;
    }
    else if (compB->getCollisionShape() == ECollisionShape::Cube)
    {
        sphere = compA;
        cube = compB;
    }
    else
    {
        return false;
    }

    if (sphere->getCollisionShape() != ECollisionShape::Sphere) return false;

    FVector spherePos = sphere->getLocation();
    FVector cubePos = cube->getLocation();
    FVector halfSize = cube->getCubeHalfSize();
    float radius = sphere->getRadius();

    FVector closestPoint;
    closestPoint.x = CEMath::Clamp(spherePos.x, cubePos.x - halfSize.x, cubePos.x + halfSize.x);
    closestPoint.y = CEMath::Clamp(spherePos.y, cubePos.y - halfSize.y, cubePos.y + halfSize.y);
    closestPoint.z = CEMath::Clamp(spherePos.z, cubePos.z - halfSize.z, cubePos.z + halfSize.z);

    float distanceSquared = closestPoint.DistanceSquared(spherePos);
    return distanceSquared <= (radius * radius);
}

bool PhysicsManager::CheckSphereVsCapsule(CollisionComponent* compA, CollisionComponent* compB)
{
    if (!compA || !compB) return false;

    CollisionComponent* sphere = nullptr;
    CollisionComponent* capsule = nullptr;

    if (compA->getCollisionShape() == ECollisionShape::Capsule)
    {
        sphere = compB;
        capsule = compA;
    }
    else if (compB->getCollisionShape() == ECollisionShape::Capsule)
    {
        sphere = compA;
        capsule = compB;
    }
    else
    {
        return false;
    }

    if (sphere->getCollisionShape() != ECollisionShape::Sphere) return false;

    FVector spherePos = sphere->getLocation();
    FVector capsulePos = capsule->getLocation();
    float halfHeight = capsule->getHalfHeight();
    float capsuleRadius = capsule->getRadius();
    float sphereRadius = sphere->getRadius();

    FVector top = capsulePos + FVector(0, halfHeight, 0);
    FVector bottom = capsulePos - FVector(0, halfHeight, 0);

    FVector capsuleAxis = (top - bottom).Normalized();
    float segmentLength = (top - bottom).Length();
    float t = (spherePos - bottom).Dot(capsuleAxis);
    t = CEMath::Clamp(t, 0.0f, segmentLength);
    FVector closestPoint = bottom + capsuleAxis * t;

    float distanceSquared = closestPoint.DistanceSquared(spherePos);
    float radiusSum = sphereRadius + capsuleRadius;

    return distanceSquared <= (radiusSum * radiusSum);
}

bool PhysicsManager::CheckSphereVsPlane(CollisionComponent* compA, CollisionComponent* compB)
{
    if (!compA || !compB) return false;

    CollisionComponent* sphere = nullptr;
    CollisionComponent* plane = nullptr;

    if (compA->getCollisionShape() == ECollisionShape::Plane)
    {
        sphere = compB;
        plane = compA;
    }
    else if (compB->getCollisionShape() == ECollisionShape::Plane)
    {
        sphere = compA;
        plane = compB;
    }
    else
    {
        return false;
    }

    if (sphere->getCollisionShape() != ECollisionShape::Sphere) return false;

    FVector spherePos = sphere->getLocation();
    float radius = sphere->getRadius();
    FVector planeNormal = plane->getPlaneNormal();
    FVector planePoint = plane->getPlanePoint();
    FVector planeSize = plane->getPlaneSize();

    float distance = (spherePos - planePoint).Dot(planeNormal);

    if (std::abs(distance) >= radius) return false;

    if (plane->isInfinitePlane()) return true;

    FVector closestPoint = spherePos - planeNormal * distance;

    FVector up = FVector(0, 1, 0);
    if (std::abs(planeNormal.Dot(up)) > 0.99f) up = FVector(1, 0, 0);

    FVector right = planeNormal.Cross(up).Normalized();
    FVector localUp = right.Cross(planeNormal).Normalized();

    FVector localPos = closestPoint - planePoint;
    float x = localPos.Dot(right);
    float y = localPos.Dot(localUp);

    float halfWidth = planeSize.x * 0.5f;
    float halfHeight = planeSize.y * 0.5f;

    return (std::abs(x) <= halfWidth + radius) && (std::abs(y) <= halfHeight + radius);
}

bool PhysicsManager::CheckCubeVsCube(CollisionComponent* compA, CollisionComponent* compB)
{
    if (!compA || !compB) return false;

    FVector posA = compA->getLocation();
    FVector posB = compB->getLocation();
    FVector halfSizeA = compA->getCubeHalfSize();
    FVector halfSizeB = compB->getCubeHalfSize();

    return (std::abs(posA.x - posB.x) <= (halfSizeA.x + halfSizeB.x)) && (std::abs(posA.y - posB.y) <= (halfSizeA.y + halfSizeB.y)) &&
           (std::abs(posA.z - posB.z) <= (halfSizeA.z + halfSizeB.z));
}

bool PhysicsManager::CheckCubeVsCapsule(CollisionComponent* compA, CollisionComponent* compB)
{
    if (!compA || !compB) return false;

    CollisionComponent* cube = nullptr;
    CollisionComponent* capsule = nullptr;

    if (compA->getCollisionShape() == ECollisionShape::Capsule)
    {
        cube = compB;
        capsule = compA;
    }
    else if (compB->getCollisionShape() == ECollisionShape::Capsule)
    {
        cube = compA;
        capsule = compB;
    }
    else
    {
        return false;
    }

    if (cube->getCollisionShape() != ECollisionShape::Cube) return false;

    FVector cubePos = cube->getLocation();
    FVector capsulePos = capsule->getLocation();
    FVector halfSize = cube->getCubeHalfSize();
    float halfHeight = capsule->getHalfHeight();
    float capsuleRadius = capsule->getRadius();

    FVector top = capsulePos + FVector(0, halfHeight, 0);
    FVector bottom = capsulePos - FVector(0, halfHeight, 0);
    FVector capsuleAxis = (top - bottom).Normalized();
    float segmentLength = (top - bottom).Length();

    const int numPoints = 10;
    for (int i = 0; i <= numPoints; ++i)
    {
        float t = (float)i / numPoints;
        FVector point = bottom + capsuleAxis * (t * segmentLength);

        bool insideCube = (std::abs(point.x - cubePos.x) <= halfSize.x)      //
                          && (std::abs(point.y - cubePos.y) <= halfSize.y)   //
                          && (std::abs(point.z - cubePos.z) <= halfSize.z);  //

        if (insideCube) return true;

        FVector closestPoint;
        closestPoint.x = CEMath::Clamp(point.x, cubePos.x - halfSize.x, cubePos.x + halfSize.x);
        closestPoint.y = CEMath::Clamp(point.y, cubePos.y - halfSize.y, cubePos.y + halfSize.y);
        closestPoint.z = CEMath::Clamp(point.z, cubePos.z - halfSize.z, cubePos.z + halfSize.z);

        float distanceSquared = closestPoint.DistanceSquared(point);
        if (distanceSquared <= (capsuleRadius * capsuleRadius)) return true;
    }

    FVector corners[8] = {cubePos + FVector(-halfSize.x, -halfSize.y, -halfSize.z), cubePos + FVector(halfSize.x, -halfSize.y, -halfSize.z),
        cubePos + FVector(-halfSize.x, halfSize.y, -halfSize.z), cubePos + FVector(halfSize.x, halfSize.y, -halfSize.z),
        cubePos + FVector(-halfSize.x, -halfSize.y, halfSize.z), cubePos + FVector(halfSize.x, -halfSize.y, halfSize.z),
        cubePos + FVector(-halfSize.x, halfSize.y, halfSize.z), cubePos + FVector(halfSize.x, halfSize.y, halfSize.z)};

    for (int i = 0; i < 8; ++i)
    {
        FVector corner = corners[i];
        FVector toCorner = corner - bottom;
        float t = toCorner.Dot(capsuleAxis);
        t = CEMath::Clamp(t, 0.0f, segmentLength);
        FVector closestPoint = bottom + capsuleAxis * t;

        float distanceSquared = closestPoint.DistanceSquared(corner);
        if (distanceSquared <= (capsuleRadius * capsuleRadius)) return true;
    }

    return false;
}

bool PhysicsManager::CheckCubeVsPlane(CollisionComponent* compA, CollisionComponent* compB)
{
    if (!compA || !compB) return false;

    CollisionComponent* cube = nullptr;
    CollisionComponent* plane = nullptr;

    if (compA->getCollisionShape() == ECollisionShape::Plane)
    {
        cube = compB;
        plane = compA;
    }
    else if (compB->getCollisionShape() == ECollisionShape::Plane)
    {
        cube = compA;
        plane = compB;
    }
    else
    {
        return false;
    }

    if (cube->getCollisionShape() != ECollisionShape::Cube) return false;

    FVector cubePos = cube->getLocation();
    FVector halfSize = cube->getCubeHalfSize();
    FVector planeNormal = plane->getPlaneNormal();
    FVector planePoint = plane->getPlanePoint();
    FVector planeSize = plane->getPlaneSize();

    float halfExtent = halfSize.x * std::abs(planeNormal.x)  //
                       + halfSize.y * std::abs(planeNormal.y) + halfSize.z * std::abs(planeNormal.z);

    float distance = (cubePos - planePoint).Dot(planeNormal);

    if (std::abs(distance) >= halfExtent) return false;

    if (plane->isInfinitePlane()) return true;

    FVector projectedCenter = cubePos - planeNormal * distance;

    FVector up = FVector(0, 1, 0);
    if (std::abs(planeNormal.Dot(up)) > 0.99f) up = FVector(1, 0, 0);

    FVector right = planeNormal.Cross(up).Normalized();
    FVector localUp = right.Cross(planeNormal).Normalized();

    FVector localPos = projectedCenter - planePoint;
    float x = localPos.Dot(right);
    float y = localPos.Dot(localUp);

    float halfSizeOnPlaneX = halfSize.x * std::abs(right.x) +
                             halfSize.y  //
                                 * std::abs(right.y) +
                             halfSize.z * std::abs(right.z);
    float halfSizeOnPlaneY = halfSize.x * std::abs(localUp.x) +
                             halfSize.y  //
                                 * std::abs(localUp.y) +
                             halfSize.z * std::abs(localUp.z);

    float halfWidth = planeSize.x * 0.5f;
    float halfHeight = planeSize.y * 0.5f;

    return (std::abs(x) <= halfWidth + halfSizeOnPlaneX) &&  //
           (std::abs(y) <= halfHeight + halfSizeOnPlaneY);
}

bool PhysicsManager::CheckCapsuleVsCapsule(CollisionComponent* compA, CollisionComponent* compB)
{
    if (!compA || !compB) return false;

    if (compA->getCollisionShape() != ECollisionShape::Capsule || compB->getCollisionShape() != ECollisionShape::Capsule) return false;

    FVector posA = compA->getLocation();
    FVector posB = compB->getLocation();
    float halfHeightA = compA->getHalfHeight();
    float halfHeightB = compB->getHalfHeight();
    float radiusA = compA->getRadius();
    float radiusB = compB->getRadius();

    FVector topA = posA + FVector(0, halfHeightA, 0);
    FVector bottomA = posA - FVector(0, halfHeightA, 0);
    FVector topB = posB + FVector(0, halfHeightB, 0);
    FVector bottomB = posB - FVector(0, halfHeightB, 0);

    FVector closestA, closestB;
    float distSq = ClosestPointBetweenSegments(bottomA, topA, bottomB, topB, closestA, closestB);

    float radiusSum = radiusA + radiusB;
    return distSq <= (radiusSum * radiusSum);
}

bool PhysicsManager::CheckCapsuleVsPlane(CollisionComponent* compA, CollisionComponent* compB)
{
    if (!compA || !compB) return false;

    CollisionComponent* capsule = nullptr;
    CollisionComponent* plane = nullptr;

    if (compA->getCollisionShape() == ECollisionShape::Plane)
    {
        capsule = compB;
        plane = compA;
    }
    else if (compB->getCollisionShape() == ECollisionShape::Plane)
    {
        capsule = compA;
        plane = compB;
    }
    else
    {
        return false;
    }

    if (capsule->getCollisionShape() != ECollisionShape::Capsule) return false;

    FVector capsulePos = capsule->getLocation();
    float halfHeight = capsule->getHalfHeight();
    float radius = capsule->getRadius();
    FVector planeNormal = plane->getPlaneNormal();
    FVector planePoint = plane->getPlanePoint();
    FVector planeSize = plane->getPlaneSize();

    FVector top = capsulePos + FVector(0, halfHeight, 0);
    FVector bottom = capsulePos - FVector(0, halfHeight, 0);

    float distTop = (top - planePoint).Dot(planeNormal);
    float distBottom = (bottom - planePoint).Dot(planeNormal);

    if (distTop * distBottom > 0)
    {
        float minDist = std::min(std::abs(distTop), std::abs(distBottom));
        if (minDist >= radius) return false;
    }

    if (plane->isInfinitePlane()) return true;

    float minDistance = std::min(std::abs(distTop), std::abs(distBottom));
    FVector closestPointOnCapsule = (std::abs(distTop) < std::abs(distBottom)) ? top : bottom;
    FVector closestPointOnPlane = closestPointOnCapsule - planeNormal * minDistance;

    FVector up = FVector(0, 1, 0);
    if (std::abs(planeNormal.Dot(up)) > 0.99f) up = FVector(1, 0, 0);

    FVector right = planeNormal.Cross(up).Normalized();
    FVector localUp = right.Cross(planeNormal).Normalized();

    FVector localPos = closestPointOnPlane - planePoint;
    float x = localPos.Dot(right);
    float y = localPos.Dot(localUp);

    float planeHalfWidth = planeSize.x * 0.5f;
    float planeHalfHeight = planeSize.y * 0.5f;

    return (std::abs(x) <= planeHalfWidth + radius) && (std::abs(y) <= planeHalfHeight + radius);
}

bool PhysicsManager::CheckPlaneVsPlane(CollisionComponent* compA, CollisionComponent* compB)
{
    if (!compA || !compB) return false;

    if (compA->getCollisionShape() != ECollisionShape::Plane || compB->getCollisionShape() != ECollisionShape::Plane) return false;

    FVector normalA = compA->getPlaneNormal();
    FVector normalB = compB->getPlaneNormal();
    FVector pointA = compA->getPlanePoint();
    FVector pointB = compB->getPlanePoint();

    float dot = normalA.Dot(normalB);
    if (CEMath::Abs(std::abs(dot) - 1.0f) <= 0.0001f)
    {

        float dist = (pointA - pointB).Dot(normalA);

        if (std::abs(dist) <= 0.0001f)
        {
            if (compA->isInfinitePlane() && compB->isInfinitePlane()) return true;

            return CheckPlaneRectanglesOverlap(compA, compB);
        }
        return false;
    }

    if (compA->isInfinitePlane() || compB->isInfinitePlane()) return true;

    return CheckPlaneRectanglesOverlap(compA, compB);
}
