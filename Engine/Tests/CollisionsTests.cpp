#include <gtest/gtest.h>
#include "Physics/PhysicsManager.h"
#include "Physics/PhysicsUtils.h"
#include "GameFrameWork/Collision/CollisionComponent.h"
#include "GameFrameWork/Collision/Capsule.h"
#include "GameFrameWork/Collision/Cube.h"
#include "GameFrameWork/Collision/Plane.h"
#include "GameFrameWork/Collision/Sphere.h"
#include "GameFrameWork/GameObjects/WorldObject/WorldActor.h"
#include "Math/MathTypes.h"
#include <memory>

using namespace RPE;

// ======================== Вспомогательные классы ========================

class TestCollisionActor : public WActor
{
    CHUDDO_DECLARE_CLASS(TestCollisionActor, WActor);

public:
    TestCollisionActor(const std::string& name, CObject* owner = nullptr) : WActor(name, owner) {}
    ~TestCollisionActor() = default;
};

REGISTER_CLASS_FACTORY(TestCollisionActor);

// ======================== Тесты для Sphere ========================

TEST(CollisionSphereTests, SphereSphereOverlap)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");
    auto actor2 = std::make_unique<TestCollisionActor>("Actor2");

    auto sphere1 = std::make_unique<SphereComponent>("Sphere1", actor1.get(), true);
    sphere1->setRadius(1.0f);
    sphere1->setLocation(FVector(0, 0, 0));

    auto sphere2 = std::make_unique<SphereComponent>("Sphere2", actor2.get(), true);
    sphere2->setRadius(1.0f);
    sphere2->setLocation(FVector(1.5f, 0, 0));

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(sphere1.get());
    pm.registerCollisionComponent(sphere2.get());
    pm.update(0.016f);

    EXPECT_EQ(sphere1->getOverlappedComponents().size(), 1);
    EXPECT_EQ(sphere2->getOverlappedComponents().size(), 1);
    EXPECT_EQ(sphere1->getOverlappedComponents()[0], sphere2.get());
    pm.shutdown();
}

TEST(CollisionSphereTests, SphereSphereNoOverlap)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");
    auto actor2 = std::make_unique<TestCollisionActor>("Actor2");

    auto sphere1 = std::make_unique<SphereComponent>("Sphere1", actor1.get(), true);
    sphere1->setRadius(1.0f);
    sphere1->setLocation(FVector(0, 0, 0));

    auto sphere2 = std::make_unique<SphereComponent>("Sphere2", actor2.get(), true);
    sphere2->setRadius(1.0f);
    sphere2->setLocation(FVector(3.0f, 0, 0));

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(sphere1.get());
    pm.registerCollisionComponent(sphere2.get());
    pm.update(0.016f);

    EXPECT_EQ(sphere1->getOverlappedComponents().size(), 0);
    EXPECT_EQ(sphere2->getOverlappedComponents().size(), 0);
    pm.shutdown();
}

TEST(CollisionSphereTests, SphereSphereTouching)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");
    auto actor2 = std::make_unique<TestCollisionActor>("Actor2");

    auto sphere1 = std::make_unique<SphereComponent>("Sphere1", actor1.get(), true);
    sphere1->setRadius(1.0f);
    sphere1->setLocation(FVector(0, 0, 0));

    auto sphere2 = std::make_unique<SphereComponent>("Sphere2", actor2.get(), true);
    sphere2->setRadius(1.0f);
    sphere2->setLocation(FVector(2.0f, 0, 0));

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(sphere1.get());
    pm.registerCollisionComponent(sphere2.get());
    pm.update(0.016f);

    // Касание считается перекрытием -> ожидаем 1
    EXPECT_EQ(sphere1->getOverlappedComponents().size(), 1);
    EXPECT_EQ(sphere2->getOverlappedComponents().size(), 1);
    pm.shutdown();
}

// ======================== Тесты для Cube ========================

TEST(CollisionCubeTests, CubeCubeOverlap)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");
    auto actor2 = std::make_unique<TestCollisionActor>("Actor2");

    auto cube1 = std::make_unique<CubeComponent>("Cube1", actor1.get(), true);
    cube1->SetCubeSize(1.0f);
    cube1->setLocation(FVector(0, 0, 0));

    auto cube2 = std::make_unique<CubeComponent>("Cube2", actor2.get(), true);
    cube2->SetCubeSize(1.0f);
    cube2->setLocation(FVector(0.75f, 0, 0));

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(cube1.get());
    pm.registerCollisionComponent(cube2.get());
    pm.update(0.016f);

    EXPECT_EQ(cube1->getOverlappedComponents().size(), 1);
    EXPECT_EQ(cube2->getOverlappedComponents().size(), 1);
    pm.shutdown();
}

TEST(CollisionCubeTests, CubeCubeNoOverlap)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");
    auto actor2 = std::make_unique<TestCollisionActor>("Actor2");

    auto cube1 = std::make_unique<CubeComponent>("Cube1", actor1.get(), true);
    cube1->SetCubeSize(1.0f);
    cube1->setLocation(FVector(0, 0, 0));

    auto cube2 = std::make_unique<CubeComponent>("Cube2", actor2.get(), true);
    cube2->SetCubeSize(1.0f);
    cube2->setLocation(FVector(2.0f, 0, 0));

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(cube1.get());
    pm.registerCollisionComponent(cube2.get());
    pm.update(0.016f);

    EXPECT_EQ(cube1->getOverlappedComponents().size(), 0);
    EXPECT_EQ(cube2->getOverlappedComponents().size(), 0);
    pm.shutdown();
}

// ======================== Тесты для Capsule ========================

TEST(CollisionCapsuleTests, CapsuleCapsuleOverlap)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");
    auto actor2 = std::make_unique<TestCollisionActor>("Actor2");

    auto cap1 = std::make_unique<CapsuleComponent>("Capsule1", actor1.get(), true);
    cap1->setRadius(0.5f);
    cap1->setHalfHeight(1.0f);
    cap1->setLocation(FVector(0, 0, 0));

    auto cap2 = std::make_unique<CapsuleComponent>("Capsule2", actor2.get(), true);
    cap2->setRadius(0.5f);
    cap2->setHalfHeight(1.0f);
    cap2->setLocation(FVector(0.75f, 0, 0));

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(cap1.get());
    pm.registerCollisionComponent(cap2.get());
    pm.update(0.016f);

    EXPECT_EQ(cap1->getOverlappedComponents().size(), 1);
    EXPECT_EQ(cap2->getOverlappedComponents().size(), 1);
    pm.shutdown();
}

TEST(CollisionCapsuleTests, CapsuleCapsuleNoOverlap)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");
    auto actor2 = std::make_unique<TestCollisionActor>("Actor2");

    auto cap1 = std::make_unique<CapsuleComponent>("Capsule1", actor1.get(), true);
    cap1->setRadius(0.5f);
    cap1->setHalfHeight(1.0f);
    cap1->setLocation(FVector(0, 0, 0));

    auto cap2 = std::make_unique<CapsuleComponent>("Capsule2", actor2.get(), true);
    cap2->setRadius(0.5f);
    cap2->setHalfHeight(1.0f);
    cap2->setLocation(FVector(4.0f, 0, 0));

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(cap1.get());
    pm.registerCollisionComponent(cap2.get());
    pm.update(0.016f);

    EXPECT_EQ(cap1->getOverlappedComponents().size(), 0);
    EXPECT_EQ(cap2->getOverlappedComponents().size(), 0);
    pm.shutdown();
}

// ======================== Тесты для Plane ========================

TEST(CollisionPlaneTests, SpherePlaneOverlap)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");
    auto actor2 = std::make_unique<TestCollisionActor>("Actor2");

    auto sphere = std::make_unique<SphereComponent>("Sphere", actor1.get(), true);
    sphere->setRadius(1.0f);
    sphere->setLocation(FVector(0, 0.5f, 0));

    auto plane = std::make_unique<PlaneComponent>("Plane", actor2.get(), true);
    plane->setPlane(FVector(0, 1, 0), FVector(0, 0, 0));
    plane->setInfinitePlane(true);

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(sphere.get());
    pm.registerCollisionComponent(plane.get());
    pm.update(0.016f);

    EXPECT_EQ(sphere->getOverlappedComponents().size(), 1);
    EXPECT_EQ(plane->getOverlappedComponents().size(), 1);
    pm.shutdown();
}

TEST(CollisionPlaneTests, SpherePlaneNoOverlap)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");
    auto actor2 = std::make_unique<TestCollisionActor>("Actor2");

    auto sphere = std::make_unique<SphereComponent>("Sphere", actor1.get(), true);
    sphere->setRadius(1.0f);
    sphere->setLocation(FVector(0, 2.0f, 0));

    auto plane = std::make_unique<PlaneComponent>("Plane", actor2.get(), true);
    plane->setPlane(FVector(0, 1, 0), FVector(0, 0, 0));
    plane->setInfinitePlane(true);

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(sphere.get());
    pm.registerCollisionComponent(plane.get());
    pm.update(0.016f);

    EXPECT_EQ(sphere->getOverlappedComponents().size(), 0);
    EXPECT_EQ(plane->getOverlappedComponents().size(), 0);
    pm.shutdown();
}

TEST(CollisionPlaneTests, CubePlaneOverlap)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");
    auto actor2 = std::make_unique<TestCollisionActor>("Actor2");

    auto cube = std::make_unique<CubeComponent>("Cube", actor1.get(), true);
    cube->SetCubeSize(1.0f);
    cube->setLocation(FVector(0, 0.25f, 0));

    auto plane = std::make_unique<PlaneComponent>("Plane", actor2.get(), true);
    plane->setPlane(FVector(0, 1, 0), FVector(0, 0, 0));
    plane->setInfinitePlane(true);

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(cube.get());
    pm.registerCollisionComponent(plane.get());
    pm.update(0.016f);

    EXPECT_EQ(cube->getOverlappedComponents().size(), 1);
    pm.shutdown();
}

TEST(CollisionPlaneTests, CapsulePlaneOverlap)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");
    auto actor2 = std::make_unique<TestCollisionActor>("Actor2");

    auto capsule = std::make_unique<CapsuleComponent>("Capsule", actor1.get(), true);
    capsule->setRadius(0.5f);
    capsule->setHalfHeight(1.0f);
    capsule->setLocation(FVector(0, 0.25f, 0));

    auto plane = std::make_unique<PlaneComponent>("Plane", actor2.get(), true);
    plane->setPlane(FVector(0, 1, 0), FVector(0, 0, 0));
    plane->setInfinitePlane(true);

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(capsule.get());
    pm.registerCollisionComponent(plane.get());
    pm.update(0.016f);

    EXPECT_EQ(capsule->getOverlappedComponents().size(), 1);
    pm.shutdown();
}

// ======================== Cross-Type Tests ========================

TEST(CollisionCrossTests, SphereCubeOverlap)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");
    auto actor2 = std::make_unique<TestCollisionActor>("Actor2");

    auto sphere = std::make_unique<SphereComponent>("Sphere", actor1.get(), true);
    sphere->setRadius(0.5f);
    sphere->setLocation(FVector(0, 0, 0));

    auto cube = std::make_unique<CubeComponent>("Cube", actor2.get(), true);
    cube->SetCubeSize(1.0f);
    cube->setLocation(FVector(0.5f, 0, 0));

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(sphere.get());
    pm.registerCollisionComponent(cube.get());
    pm.update(0.016f);

    EXPECT_EQ(sphere->getOverlappedComponents().size(), 1);
    EXPECT_EQ(cube->getOverlappedComponents().size(), 1);
    pm.shutdown();
}

TEST(CollisionCrossTests, SphereCubeNoOverlap)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");
    auto actor2 = std::make_unique<TestCollisionActor>("Actor2");

    auto sphere = std::make_unique<SphereComponent>("Sphere", actor1.get(), true);
    sphere->setRadius(0.5f);
    sphere->setLocation(FVector(0, 0, 0));

    auto cube = std::make_unique<CubeComponent>("Cube", actor2.get(), true);
    cube->SetCubeSize(1.0f);
    cube->setLocation(FVector(2.0f, 0, 0));

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(sphere.get());
    pm.registerCollisionComponent(cube.get());
    pm.update(0.016f);

    EXPECT_EQ(sphere->getOverlappedComponents().size(), 0);
    EXPECT_EQ(cube->getOverlappedComponents().size(), 0);
    pm.shutdown();
}

TEST(CollisionCrossTests, SphereCapsuleOverlap)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");
    auto actor2 = std::make_unique<TestCollisionActor>("Actor2");

    auto sphere = std::make_unique<SphereComponent>("Sphere", actor1.get(), true);
    sphere->setRadius(0.5f);
    sphere->setLocation(FVector(0, 0, 0));

    auto capsule = std::make_unique<CapsuleComponent>("Capsule", actor2.get(), true);
    capsule->setRadius(0.5f);
    capsule->setHalfHeight(1.0f);
    capsule->setLocation(FVector(0.75f, 0, 0));

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(sphere.get());
    pm.registerCollisionComponent(capsule.get());
    pm.update(0.016f);

    EXPECT_EQ(sphere->getOverlappedComponents().size(), 1);
    EXPECT_EQ(capsule->getOverlappedComponents().size(), 1);
    pm.shutdown();
}

TEST(CollisionCrossTests, CubeCapsuleOverlap)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");
    auto actor2 = std::make_unique<TestCollisionActor>("Actor2");

    auto cube = std::make_unique<CubeComponent>("Cube", actor1.get(), true);
    cube->SetCubeSize(1.0f);
    cube->setLocation(FVector(0, 0, 0));

    auto capsule = std::make_unique<CapsuleComponent>("Capsule", actor2.get(), true);
    capsule->setRadius(0.5f);
    capsule->setHalfHeight(1.0f);
    capsule->setLocation(FVector(0.75f, 0, 0));

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(cube.get());
    pm.registerCollisionComponent(capsule.get());
    pm.update(0.016f);

    EXPECT_EQ(cube->getOverlappedComponents().size(), 1);
    EXPECT_EQ(capsule->getOverlappedComponents().size(), 1);
    pm.shutdown();
}

// ======================== Dynamic Tests ========================

TEST(CollisionDynamicTests, OverlapStartAndEnd)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");
    auto actor2 = std::make_unique<TestCollisionActor>("Actor2");

    auto sphere1 = std::make_unique<SphereComponent>("Sphere1", actor1.get(), true);
    sphere1->setRadius(1.0f);
    sphere1->setLocation(FVector(0, 0, 0));

    auto sphere2 = std::make_unique<SphereComponent>("Sphere2", actor2.get(), true);
    sphere2->setRadius(1.0f);
    sphere2->setLocation(FVector(3.0f, 0, 0));

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(sphere1.get());
    pm.registerCollisionComponent(sphere2.get());
    pm.update(0.016f);

    EXPECT_EQ(sphere1->getOverlappedComponents().size(), 0);
    EXPECT_EQ(sphere2->getOverlappedComponents().size(), 0);

    // Двигаем второй шар к первому
    sphere2->setLocation(FVector(1.5f, 0, 0));
    pm.update(0.016f);

    EXPECT_EQ(sphere1->getOverlappedComponents().size(), 1);
    EXPECT_EQ(sphere2->getOverlappedComponents().size(), 1);

    // Убираем перекрытие
    sphere2->setLocation(FVector(3.0f, 0, 0));
    pm.update(0.016f);

    EXPECT_EQ(sphere1->getOverlappedComponents().size(), 0);
    EXPECT_EQ(sphere2->getOverlappedComponents().size(), 0);
    pm.shutdown();
}

TEST(CollisionDynamicTests, MultipleObjectsOverlap)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");
    auto actor2 = std::make_unique<TestCollisionActor>("Actor2");
    auto actor3 = std::make_unique<TestCollisionActor>("Actor3");

    auto sphere1 = std::make_unique<SphereComponent>("Sphere1", actor1.get(), true);
    sphere1->setRadius(1.0f);
    sphere1->setLocation(FVector(0, 0, 0));

    auto sphere2 = std::make_unique<SphereComponent>("Sphere2", actor2.get(), true);
    sphere2->setRadius(1.0f);
    sphere2->setLocation(FVector(1.5f, 0, 0));

    auto sphere3 = std::make_unique<SphereComponent>("Sphere3", actor3.get(), true);
    sphere3->setRadius(1.0f);
    sphere3->setLocation(FVector(0, 1.5f, 0));

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(sphere1.get());
    pm.registerCollisionComponent(sphere2.get());
    pm.registerCollisionComponent(sphere3.get());
    pm.update(0.016f);

    EXPECT_EQ(sphere1->getOverlappedComponents().size(), 2);
    EXPECT_EQ(sphere2->getOverlappedComponents().size(), 1);
    EXPECT_EQ(sphere3->getOverlappedComponents().size(), 1);
    pm.shutdown();
}

// ======================== Edge Cases ========================

TEST(CollisionEdgeTests, SameActorComponentsDontCollide)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");

    auto sphere1 = std::make_unique<SphereComponent>("Sphere1", actor1.get(), true);
    sphere1->setRadius(1.0f);
    sphere1->setLocation(FVector(0, 0, 0));

    auto sphere2 = std::make_unique<SphereComponent>("Sphere2", actor1.get(), true);
    sphere2->setRadius(1.0f);
    sphere2->setLocation(FVector(1.5f, 0, 0));

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(sphere1.get());
    pm.registerCollisionComponent(sphere2.get());
    pm.update(0.016f);

    // Компоненты одного актора не должны коллидировать
    EXPECT_EQ(sphere1->getOverlappedComponents().size(), 0);
    EXPECT_EQ(sphere2->getOverlappedComponents().size(), 0);
    pm.shutdown();
}

TEST(CollisionEdgeTests, NoCollisionShape)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");
    auto actor2 = std::make_unique<TestCollisionActor>("Actor2");

    auto comp1 = std::make_unique<CollisionComponent>("Comp1", actor1.get(), true);
    comp1->setShape(ECollisionShape::NO_COLLISION);
    comp1->setLocation(FVector(0, 0, 0));

    auto comp2 = std::make_unique<CollisionComponent>("Comp2", actor2.get(), true);
    comp2->setShape(ECollisionShape::NO_COLLISION);
    comp2->setLocation(FVector(0, 0, 0));

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(comp1.get());
    pm.registerCollisionComponent(comp2.get());
    pm.update(0.016f);

    EXPECT_EQ(comp1->getOverlappedComponents().size(), 0);
    EXPECT_EQ(comp2->getOverlappedComponents().size(), 0);
    pm.shutdown();
}

TEST(CollisionEdgeTests, VerySmallObjects)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");
    auto actor2 = std::make_unique<TestCollisionActor>("Actor2");

    auto sphere1 = std::make_unique<SphereComponent>("Sphere1", actor1.get(), true);
    sphere1->setRadius(0.001f);
    sphere1->setLocation(FVector(0, 0, 0));

    auto sphere2 = std::make_unique<SphereComponent>("Sphere2", actor2.get(), true);
    sphere2->setRadius(0.001f);
    sphere2->setLocation(FVector(0.0015f, 0, 0));

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(sphere1.get());
    pm.registerCollisionComponent(sphere2.get());
    pm.update(0.016f);

    EXPECT_EQ(sphere1->getOverlappedComponents().size(), 1);
    EXPECT_EQ(sphere2->getOverlappedComponents().size(), 1);
    pm.shutdown();
}

TEST(CollisionEdgeTests, VeryLargeObjects)
{
    auto actor1 = std::make_unique<TestCollisionActor>("Actor1");
    auto actor2 = std::make_unique<TestCollisionActor>("Actor2");

    auto sphere1 = std::make_unique<SphereComponent>("Sphere1", actor1.get(), true);
    sphere1->setRadius(1000.0f);
    sphere1->setLocation(FVector(0, 0, 0));

    auto sphere2 = std::make_unique<SphereComponent>("Sphere2", actor2.get(), true);
    sphere2->setRadius(1000.0f);
    sphere2->setLocation(FVector(1500.0f, 0, 0));

    PhysicsManager pm;
    pm.init();
    pm.registerCollisionComponent(sphere1.get());
    pm.registerCollisionComponent(sphere2.get());
    pm.update(0.016f);

    EXPECT_EQ(sphere1->getOverlappedComponents().size(), 1);
    EXPECT_EQ(sphere2->getOverlappedComponents().size(), 1);
    pm.shutdown();
}

// ======================== Performance Tests ========================

TEST(CollisionPerformanceTests, ManyObjects)
{
    PhysicsManager pm;
    pm.init();

    std::vector<std::unique_ptr<SphereComponent>> spheres;
    std::vector<std::unique_ptr<TestCollisionActor>> actors;

    const int numSpheres = 100;

    for (int i = 0; i < numSpheres; ++i)
    {
        auto actor = std::make_unique<TestCollisionActor>("Actor" + std::to_string(i));
        auto sphere = std::make_unique<SphereComponent>("Sphere" + std::to_string(i), actor.get(), true);
        sphere->setRadius(0.5f);
        sphere->setLocation(FVector(static_cast<float>(i % 10), static_cast<float>(i / 10), 0));

        pm.registerCollisionComponent(sphere.get());

        spheres.push_back(std::move(sphere));
        actors.push_back(std::move(actor));
    }

    EXPECT_EQ(pm.GetCollisionComponentCount(), numSpheres);

    pm.update(0.016f);

    bool hasCollision = false;
    for (auto& sphere : spheres)
    {
        if (!sphere->getOverlappedComponents().empty())
        {
            hasCollision = true;
            break;
        }
    }
    EXPECT_TRUE(hasCollision);

    // Сначала очищаем PhysicsManager, потом удаляем объекты
    pm.shutdown();

    // Теперь можно удалить объекты (они удалятся автоматически при выходе из области видимости)
    // Но порядок важен: сначала spheres, потом actors
    spheres.clear();
    actors.clear();
}