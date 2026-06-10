#include <gtest/gtest.h>
#include "Core/ObjectFactory.h"
#include "Core/SystemObject.h"
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(ObjTests);
using namespace RPE;

// Тестовые классы для фабрики
class TestGameObject : public CObject
{
    CHUDDO_DECLARE_CLASS(TestGameObject, CObject)
public:
    TestGameObject(const std::string& name = "TestGameObject", CObject* owner = nullptr) : CObject(name, owner), testValue(0) {}

    int testValue;
};

class TestActor : public TestGameObject
{
    CHUDDO_DECLARE_CLASS(TestActor, TestGameObject)
public:
    TestActor(const std::string& name = "TestActor", CObject* owner = nullptr) : TestGameObject(name, owner), actorHealth(100) {}

    int actorHealth;
};

class TestComponent : public CObject
{
    CHUDDO_DECLARE_CLASS(TestComponent, CObject)
public:
    TestComponent(const std::string& name = "TestComponent", CObject* owner = nullptr) : CObject(name, owner) {}
};

// Регистрируем классы
REGISTER_CLASS_FACTORY(TestGameObject);
REGISTER_CLASS_FACTORY(TestActor);
REGISTER_CLASS_FACTORY(TestComponent);

// ============================================================================
// CObjectFactory Tests
// ============================================================================

TEST(ObjectFactoryTest, SingletonInstance)
{
    CObjectFactory& instance1 = CObjectFactory::GetInstance();
    CObjectFactory& instance2 = CObjectFactory::GetInstance();

    EXPECT_EQ(&instance1, &instance2);
}

TEST(ObjectFactoryTest, RegisterClass)
{
    CObjectFactory& factory = CObjectFactory::GetInstance();

    EXPECT_TRUE(factory.IsClassRegistered("TestGameObject"));
    EXPECT_TRUE(factory.IsClassRegistered("TestActor"));
    EXPECT_TRUE(factory.IsClassRegistered("TestComponent"));
    EXPECT_TRUE(factory.IsClassRegistered("CObject"));
}

TEST(ObjectFactoryTest, CreateObject)
{
    CObjectFactory& factory = CObjectFactory::GetInstance();

    CObject* obj = factory.Create("TestGameObject", nullptr, "MyTestObject");

    ASSERT_NE(obj, nullptr);
    EXPECT_EQ(obj->GetName(), "MyTestObject");
    EXPECT_EQ(obj->GetOwner(), nullptr);

    delete obj;
}

TEST(ObjectFactoryTest, CreateObjectWithOwner)
{
    CObjectFactory& factory = CObjectFactory::GetInstance();
    CObject* owner = new CObject("Owner");

    CObject* child = factory.Create("TestGameObject", owner, "ChildObject");

    ASSERT_NE(child, nullptr);
    EXPECT_EQ(child->GetOwner(), owner);
    EXPECT_EQ(owner->GetNumOwnedObjects(), 1);
    EXPECT_EQ(owner->FindOwned("ChildObject"), child);

    delete owner;
}

TEST(ObjectFactoryTest, CreateUnknownClass)
{
    CObjectFactory& factory = CObjectFactory::GetInstance();

    CObject* obj = factory.Create("UnknownClass", nullptr, "Test");

    EXPECT_EQ(obj, nullptr);
}

TEST(ObjectFactoryTest, CreateWithDuplicateNameAutoRename)
{
    CObjectFactory& factory = CObjectFactory::GetInstance();
    CObject* root = new CObject("Root");

    CObject* obj1 = factory.Create("TestGameObject", root, "SameName");
    CObject* obj2 = factory.Create("TestGameObject", root, "SameName");

    ASSERT_NE(obj1, nullptr);
    ASSERT_NE(obj2, nullptr);

    EXPECT_EQ(obj1->GetName(), "SameName");
    EXPECT_EQ(obj2->GetName(), "SameName_1");

    delete root;
}

TEST(ObjectFactoryTest, GetRegisteredClasses)
{
    CObjectFactory& factory = CObjectFactory::GetInstance();
    std::vector<std::string> classes = factory.GetRegisteredClasses();

    EXPECT_GT(classes.size(), 0);

    bool foundCObject = false;
    bool foundTestGameObject = false;

    for (const auto& className : classes)
    {
        if (className == "CObject") foundCObject = true;
        if (className == "TestGameObject") foundTestGameObject = true;
    }

    EXPECT_TRUE(foundCObject);
    EXPECT_TRUE(foundTestGameObject);
}

TEST(ObjectFactoryTest, IsDerivedFrom)
{
    CObjectFactory& factory = CObjectFactory::GetInstance();

    EXPECT_TRUE(factory.IsDerivedFrom("TestActor", "TestActor"));
    EXPECT_TRUE(factory.IsDerivedFrom("TestActor", "TestGameObject"));
    EXPECT_TRUE(factory.IsDerivedFrom("TestGameObject", "CObject"));
    EXPECT_TRUE(factory.IsDerivedFrom("TestActor", "CObject"));

    EXPECT_FALSE(factory.IsDerivedFrom("TestActor", "TestComponent"));
    EXPECT_FALSE(factory.IsDerivedFrom("CObject", "TestGameObject"));
}

TEST(ObjectFactoryTest, GetClassHierarchy)
{
    CObjectFactory& factory = CObjectFactory::GetInstance();
    std::string hierarchy = factory.GetClassHierarchy();

    EXPECT_FALSE(hierarchy.empty());
}

// ============================================================================
// CObject Tests
// ============================================================================

TEST(ObjectTest, ConstructionAndDestruction)
{
    CObject* obj = new CObject("TestObject");

    EXPECT_EQ(obj->GetName(), "TestObject");
    EXPECT_EQ(obj->GetOwner(), nullptr);
    EXPECT_FALSE(obj->GetUUID().empty());
    EXPECT_EQ(obj->GetNumOwnedObjects(), 0);

    delete obj;
}

TEST(ObjectTest, ObjectWithOwner)
{
    CObject* parent = new CObject("Parent");
    CObject* child = new CObject("Child", parent);

    EXPECT_EQ(child->GetOwner(), parent);
    EXPECT_EQ(parent->GetNumOwnedObjects(), 1);
    EXPECT_EQ(parent->FindOwned("Child"), child);

    delete parent;
}

TEST(ObjectTest, AddOwnedObject)
{
    CObject* parent = new CObject("Parent");
    CObject* child = new CObject("Child");

    parent->AddOwnedObject(child);

    EXPECT_EQ(child->GetOwner(), parent);
    EXPECT_EQ(parent->GetNumOwnedObjects(), 1);
    EXPECT_EQ(parent->FindOwned("Child"), child);

    delete parent;
}

TEST(ObjectTest, RemoveOwnedObject)
{
    CObject* parent = new CObject("Parent");
    CObject* child = new CObject("Child", parent);

    EXPECT_EQ(parent->GetNumOwnedObjects(), 1);

    bool removed = parent->RemoveOwnedObject("Child");

    EXPECT_TRUE(removed);
    EXPECT_EQ(parent->GetNumOwnedObjects(), 0);

    delete parent;
}

TEST(ObjectTest, FindOwned)
{
    CObject* parent = new CObject("Parent");
    CObject* child1 = new CObject("First", parent);
    CObject* child2 = new CObject("Second", parent);

    EXPECT_EQ(parent->FindOwned("First"), child1);
    EXPECT_EQ(parent->FindOwned("Second"), child2);
    EXPECT_EQ(parent->FindOwned("NonExistent"), nullptr);

    delete parent;
}

TEST(ObjectTest, FindRecursive)
{
    CObject* root = new CObject("Root");
    CObject* child = new CObject("Child", root);
    CObject* grandChild = new CObject("GrandChild", child);

    EXPECT_TRUE(root->FindRecursive("Root"));
    EXPECT_TRUE(root->FindRecursive("Child"));
    EXPECT_TRUE(root->FindRecursive("GrandChild"));
    EXPECT_FALSE(root->FindRecursive("NonExistent"));

    delete root;
}

TEST(ObjectTest, GetRoot)
{
    CObject* root = new CObject("Root");
    CObject* child = new CObject("Child", root);
    CObject* grandChild = new CObject("GrandChild", child);

    EXPECT_EQ(root->GetRoot(), root);
    EXPECT_EQ(child->GetRoot(), root);
    EXPECT_EQ(grandChild->GetRoot(), root);

    delete root;
}

TEST(ObjectTest, Rename)
{
    CObject* obj = new CObject("OldName");

    EXPECT_TRUE(obj->Rename("NewName"));
    EXPECT_EQ(obj->GetName(), "NewName");

    delete obj;
}

TEST(ObjectTest, RenameWithDuplicate)
{
    CObject* root = new CObject("Root");
    CObject* obj1 = new CObject("SameName", root);
    CObject* obj2 = new CObject("OtherName", root);

    bool result = obj2->Rename("SameName");

    EXPECT_TRUE(result);
    EXPECT_NE(obj2->GetName(), "SameName");

    delete root;
}

TEST(ObjectTest, AddSubObject)
{
    CObject* parent = new CObject("Parent");
    EXPECT_EQ(parent->GetNumOwnedObjects(), 0);
    TestGameObject* child = parent->AddSubObject<TestGameObject>("MyGameObject");
    EXPECT_EQ(parent->GetNumOwnedObjects(), 1);

    ASSERT_NE(child, nullptr);
    if (parent->GetNumOwnedObjects() > 0)
    {
        RP_LOG(ObjTests, Display, "num of object not zero");
        for (const auto& obj : parent->GetOwnedObjects())
        {
            RP_LOG(ObjTests, Display, "{} owned by {}", obj->GetName(), parent->GetName());
        }
    }
    EXPECT_EQ(child->GetName(), "MyGameObject");
    EXPECT_EQ(child->GetOwner(), parent);
    EXPECT_EQ(parent->GetNumOwnedObjects(), 1);

    delete parent;
}

TEST(ObjectTest, AddSubObjectAutoRename)
{
    CObject* parent = new CObject("Parent");

    TestGameObject* obj1 = parent->AddSubObject<TestGameObject>("Duplicate");
    TestGameObject* obj2 = parent->AddSubObject<TestGameObject>("Duplicate");

    EXPECT_EQ(obj1->GetName(), "Duplicate");
    EXPECT_EQ(obj2->GetName(), "Duplicate_1");

    delete parent;
}

TEST(ObjectTest, TransferOwnership)
{
    CObject* oldOwner = new CObject("OldOwner");
    CObject* newOwner = new CObject("NewOwner");
    CObject* obj = new CObject("Movable", oldOwner);

    EXPECT_EQ(obj->GetOwner(), oldOwner);
    EXPECT_EQ(oldOwner->GetNumOwnedObjects(), 1);
    EXPECT_EQ(newOwner->GetNumOwnedObjects(), 0);

    bool transferred = oldOwner->TransferOwnership(obj, newOwner);

    EXPECT_TRUE(transferred);
    EXPECT_EQ(obj->GetOwner(), newOwner);
    EXPECT_EQ(oldOwner->GetNumOwnedObjects(), 0);
    EXPECT_EQ(newOwner->GetNumOwnedObjects(), 1);
    EXPECT_EQ(newOwner->FindOwned("Movable"), obj);

    delete oldOwner;
    delete newOwner;
}

TEST(ObjectTest, Clone)
{
    CObject* original = new CObject("Original");
    TestGameObject* child = original->AddSubObject<TestGameObject>("Child");
    child->testValue = 42;

    std::unique_ptr<CObject> clone = original->Clone();

    EXPECT_NE(clone.get(), original);
    EXPECT_EQ(clone->GetName(), "Original_Copy");
    EXPECT_EQ(clone->GetNumOwnedObjects(), 1);

    delete original;
}

TEST(ObjectTest, FindByUUID)
{
    CObject* obj = new CObject("Test");
    std::string uuid = obj->GetUUID();

    CObject* found = obj->FindByUUID(uuid);

    EXPECT_EQ(found, obj);

    delete obj;
}

TEST(ObjectTest, FindByType)
{
    CObject* root = new CObject("Root");
    TestGameObject* gameObj = root->AddSubObject<TestGameObject>("Game");
    TestActor* actor = root->AddSubObject<TestActor>("Actor");

    auto* foundGame = root->FindObjectByType<TestGameObject>();
    auto* foundActor = root->FindObjectByType<TestActor>();
    auto* foundComponent = root->FindObjectByType<TestComponent>();

    EXPECT_EQ(foundGame, gameObj);
    EXPECT_EQ(foundActor, actor);
    EXPECT_EQ(foundComponent, nullptr);

    delete root;
}

TEST(ObjectTest1, FindByType1)
{
    CObject* root = new CObject("Root");
    TestGameObject* gameObj = root->AddSubObject<TestGameObject>("Game");
    TestActor* actor = root->AddSubObject<TestActor>("Actor");

    // Ищем на уровне root (root сам не является TestGameObject)
    TestGameObject* foundGame = root->FindObjectByType<TestGameObject>();
    TestActor* foundActor = root->FindObjectByType<TestActor>();

    EXPECT_EQ(foundGame, gameObj);
    EXPECT_EQ(foundActor, actor);

    delete root;
}

// ============================================================================
// JSON Serialization Tests
// ============================================================================

TEST(ObjectSerializationTest, ToJSON)
{
    CObject* obj = new CObject("SerializableObject");

    std::string json = obj->ToJSON();

    EXPECT_FALSE(json.empty());
    EXPECT_TRUE(json.find("SerializableObject") != std::string::npos);
    EXPECT_TRUE(json.find("CObject") != std::string::npos);

    delete obj;
}

TEST(ObjectSerializationTest, FromJSON)
{
    CObject* original = new CObject("Original");

    std::string json = original->ToJSON();

    std::unique_ptr<CObject> restored = CObject::CreateFromJSON(json);

    ASSERT_NE(restored, nullptr);
    EXPECT_EQ(restored->GetName(), original->GetName());
    EXPECT_EQ(restored->GetUUID(), original->GetUUID());

    delete original;
}

TEST(ObjectSerializationTest, SaveAndLoadFromFile)
{
    CObject* original = new CObject("FileObject");

    bool saved = original->SaveToFile("test_object.json", true);

    EXPECT_TRUE(saved);

    std::unique_ptr<CObject> loaded = CObject::LoadFromJSONFile("test_object.json");

    ASSERT_NE(loaded, nullptr);
    EXPECT_EQ(loaded->GetName(), original->GetName());

    delete original;

    std::remove("test_object.json");
}

TEST(ObjectSerializationTest, SerializeWithChildren)
{
    CObject* root = new CObject("Root");
    root->AddSubObject<TestGameObject>("Child1");
    root->AddSubObject<TestActor>("Child2");

    std::string json = root->ToJSON(true);

    EXPECT_TRUE(json.find("Child1") != std::string::npos);
    EXPECT_TRUE(json.find("Child2") != std::string::npos);

    delete root;
}

TEST(ObjectSerializationTest, GetJSONSchema)
{
    CObject* obj = new CObject("SchemaTest");

    std::string schema = obj->GetJSONSchema();

    EXPECT_FALSE(schema.empty());
    EXPECT_TRUE(schema.find("$schema") != std::string::npos);

    delete obj;
}