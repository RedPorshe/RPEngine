#pragma once
#include <atomic>
#include <random>
#include <sstream>
#include <iomanip>
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <typeinfo>
#include <algorithm>
#include <cctype>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

// Forward declaration
namespace RPE
{
class CObjectFactory;
}

namespace RPE
{

#define CLASS_NAME_TO_STR(ClassName) std::string(#ClassName)

#define CHUDDO_DECLARE_CLASS(ClassName, BaseClassName)                                                                          \
public:                                                                                                                         \
    using Super = BaseClassName;                                                                                                \
    static const char* StaticClassName()                                                                                        \
    {                                                                                                                           \
        return #ClassName;                                                                                                      \
    }                                                                                                                           \
    static const char* StaticBaseClassName()                                                                                    \
    {                                                                                                                           \
        return #BaseClassName;                                                                                                  \
    }                                                                                                                           \
    virtual const char* GetObjectClassName() const override                                                                     \
    {                                                                                                                           \
        return #ClassName;                                                                                                      \
    }                                                                                                                           \
    virtual void SerializeProperties(rapidjson::Value& jsonValue, rapidjson::Document::AllocatorType& allocator) const override \
    {                                                                                                                           \
        Super::SerializeProperties(jsonValue, allocator);                                                                       \
    }                                                                                                                           \
    virtual void DeserializeProperties(const rapidjson::Value& jsonValue) override                                              \
    {                                                                                                                           \
        Super::DeserializeProperties(jsonValue);                                                                                \
    }

#define CHUDDO_DECLARE_ABSTRACT_CLASS(ClassName, BaseClassName)                                                                 \
public:                                                                                                                         \
    using Super = BaseClassName;                                                                                                \
    static const char* StaticClassName()                                                                                        \
    {                                                                                                                           \
        return #ClassName;                                                                                                      \
    }                                                                                                                           \
    static const char* StaticBaseClassName()                                                                                    \
    {                                                                                                                           \
        return #BaseClassName;                                                                                                  \
    }                                                                                                                           \
    virtual const char* GetObjectClassName() const override                                                                     \
    {                                                                                                                           \
        return #ClassName;                                                                                                      \
    }                                                                                                                           \
    virtual void SerializeProperties(rapidjson::Value& jsonValue, rapidjson::Document::AllocatorType& allocator) const override \
    {                                                                                                                           \
        Super::SerializeProperties(jsonValue, allocator);                                                                       \
    }                                                                                                                           \
    virtual void DeserializeProperties(const rapidjson::Value& jsonValue) override                                              \
    {                                                                                                                           \
        Super::DeserializeProperties(jsonValue);                                                                                \
    }

class CObject
{
public:
    using Super = CObject;
    static const char* StaticClassName() { return "CObject"; }
    static const char* StaticBaseClassName() { return ""; }
    virtual const char* GetObjectClassName() const { return "CObject"; }
    virtual void SerializeProperties(rapidjson::Value& jsonValue, rapidjson::Document::AllocatorType& allocator) const;
    virtual void DeserializeProperties(const rapidjson::Value& jsonValue);

private:
    static std::string GenerateUUID()
    {
        static std::random_device rd;
        static std::mt19937_64 gen(rd());
        static std::uniform_int_distribution<uint64_t> dis;

        uint64_t part1 = dis(gen);
        uint64_t part2 = dis(gen);

        std::stringstream ss;
        ss << std::hex << std::setfill('0') << std::setw(16) << part1 << std::setw(16) << part2;
        return ss.str();
    }
    friend class CObjectFactory;

public:
    CObject(const std::string& inDisplayName = "Object", CObject* inOwner = nullptr);
    virtual ~CObject();

    void SetOwner(CObject* newOwner);
    CObject* GetOwner() const { return ObjectOwner; }
    bool HasOwner() const { return GetOwner() != nullptr; }
    std::string GetName() const { return DisplayName; }
    std::string GetUUID() const { return ObjectUUID; }
    std::string GetUniqName() const { return DisplayName + "_" + GetShortUUID(); }
    std::string GetFullIdentifier() const { return GetUniqName(); }
    void UpdateDebugIdentifier();
    std::string GetShortUUID() const { return (ObjectUUID.length() > 8) ? ObjectUUID.substr(0, 8) : ObjectUUID; }

    CObject* FindOwned(const std::string& displayName) const;
    const std::vector<std::unique_ptr<CObject>>& GetOwnedObjects() const { return OwnedObjects; }
    size_t GetNumOwnedObjects() const { return OwnedObjects.size(); }
    bool HasOwnedObjects() const { return !OwnedObjects.empty(); }

    bool FindRecursive(const std::string& displayName);
    CObject* FindObjectByDisplayNameRecursive(const std::string& displayName);

    CObject* FindByUUID(const std::string& uuid) const;
    CObject* FindByUUIDRecursive(const std::string& uuid);

    std::string CheckNameAndGenerateUniqName(const std::string& DesiredName);

    bool RemoveOwnedObject(const std::string& displayName);
    void AddOwnedObject(std::unique_ptr<CObject> object);
    void AddOwnedObject(CObject* object);
    bool TransferOwnership(CObject* obj, CObject* newOwner);

    template <typename ClassName, typename... Args>
    ClassName* AddSubObject(const std::string& desiredDisplayName = "SubObject", Args&&... args);

    CObject* AddSubObjectByClass(const std::string& className, const std::string& desiredDisplayName = "SubObject");

    template <typename ObjectType>
    ObjectType* FindObjectByType() const
    {
        for (const auto& actor : OwnedObjects)
        {
            if (auto casted = dynamic_cast<ObjectType*>(actor.get()))
            {
                return casted;
            }
            ObjectType* founded = actor->FindObjectByType<ObjectType>();
            if (founded) return founded;
        }
        return nullptr;
    }

    bool RenameOwnedObject(const std::string& oldDisplayName, const std::string& newDisplayName);
    bool Rename(const std::string& newDisplayName);

    template <typename T>
    T* FindOwnedAs(const std::string& displayName) const
    {
        CObject* obj = FindOwned(displayName);
        if (obj) return dynamic_cast<T*>(obj);
        return nullptr;
    }

    std::unique_ptr<CObject> Clone() const;

    CObject* GetRoot() const
    {
        const CObject* root = this;
        while (root->GetOwner())
            root = root->GetOwner();
        return const_cast<CObject*>(root);
    }

    virtual void Serialize(rapidjson::Value& jsonValue, rapidjson::Document::AllocatorType& allocator) const;
    virtual void Deserialize(const rapidjson::Value& jsonValue);
    std::string ToJSON(bool pretty = false) const;
    bool FromJSON(const std::string& jsonString);
    bool SaveToFile(const std::string& filename, bool pretty = true) const;
    bool LoadFromFile(const std::string& filename);
    static std::unique_ptr<CObject> CreateFromJSON(const std::string& jsonString);
    static std::unique_ptr<CObject> LoadFromJSONFile(const std::string& filename);
    std::string GetJSONSchema() const;

protected:
    CObject* ObjectOwner = nullptr;
    std::string DisplayName{};
    std::string ObjectUUID{};
    std::vector<std::unique_ptr<CObject>> OwnedObjects;

    static std::string GenerateUniqueDisplayNameVariant(const std::string& baseDisplayName, CObject* root);
    static void CollectSimilarDisplayNames(CObject* node, const std::string& baseDisplayName, std::vector<std::string>& result);
    static std::string GetShortUUID(const std::string& uuid) { return uuid.length() > 8 ? uuid.substr(0, 8) : uuid; }
};

}  // namespace RPE

// Включаем реализацию шаблона
#include "Core/SystemObject.hpp"