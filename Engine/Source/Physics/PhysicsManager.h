#pragma once
#include "PhysicsUtils.h"
#include "Math/MathTypes.h"
#include <unordered_map>
#include <vector>

namespace RPE
{
class CollisionComponent;

class PhysicsManager
{
public:
    PhysicsManager();
    virtual ~PhysicsManager() = default;
    bool init();
    void update(float DeltaTime);
    void shutdown();
    inline bool isInitialized() const { return bisInitialized; }
    void registerPhysicsComponent();  // TODO: Add Physics components
    void registerCollisionComponent(CollisionComponent* collisionComp);
    void unregisterCollisionComponent(CollisionComponent* collisionComp);
    size_t GetCollisionComponentCount() const { return m_collisionComponents.size(); }
    const std::unordered_map<std::string, CollisionComponent*>& GetCollisionComponents() const { return m_collisionComponents; }

private:
    bool bisInitialized{false};
    void CheckCollisions();
    std::unordered_map<std::string, CollisionComponent*> m_collisionComponents;
    void onCollision(const std::string& compA, const std::string& compB);

    void endCollision(const std::string& compA, const std::string& compB);
    bool CheckOverlaping(const std::string& compA, const std::string& compB);
    bool allReadyOverlapped(const std::string& compA, const std::string& compB);
    std::vector<std::string> m_collisionComponentsNames;

    float ClosestPointBetweenSegments(
        const FVector& p1, const FVector& q1, const FVector& p2, const FVector& q2, FVector& outClosest1, FVector& outClosest2);
    bool CheckPlaneRectanglesOverlap(CollisionComponent* planeA, CollisionComponent* planeB);
    bool GetLineRectangleIntersection(const FVector& linePoint, const FVector& lineDirection, const FVector& rectCenter,
        const FVector& rectRight, const FVector& rectUp, const FVector2D& rectSize, float& outTMin, float& outTMax);
    // checking by shape
    // Spere
    bool CheckSphereSphere(CollisionComponent* compA, CollisionComponent* compB);
    bool CheckSphereVsCube(CollisionComponent* compA, CollisionComponent* compB);
    bool CheckSphereVsCapsule(CollisionComponent* compA, CollisionComponent* compB);
    bool CheckSphereVsPlane(CollisionComponent* compA, CollisionComponent* compB);

    // Cube
    bool CheckCubeVsCube(CollisionComponent* compA, CollisionComponent* compB);
    bool CheckCubeVsCapsule(CollisionComponent* compA, CollisionComponent* compB);
    bool CheckCubeVsPlane(CollisionComponent* compA, CollisionComponent* compB);

    // capsule
    bool CheckCapsuleVsCapsule(CollisionComponent* compA, CollisionComponent* compB);
    bool CheckCapsuleVsPlane(CollisionComponent* compA, CollisionComponent* compB);

    // plane
    bool CheckPlaneVsPlane(CollisionComponent* compA, CollisionComponent* compB);
};
}  // namespace RPE