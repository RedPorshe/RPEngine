#include "Core/SystemObject.h"
#include "Core/ObjectFactory.h"
#include "Core/Engine.h"
#include <functional>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <fstream>
#include "Log/Log.h"

using namespace RPE;
DEFINE_LOG_CATEGORY_STATIC(ObjectLog);

CObject::CObject(const std::string& inDisplayName, CObject* inOwner) : ObjectOwner(inOwner), DisplayName(inDisplayName)
{
    ObjectUUID = GenerateUUID();
    if (ObjectOwner)
    {
        ObjectOwner->AddOwnedObject(this);
    }
    RP_LOG(ObjectLog, Display, "[{}] created", GetName());
}

CObject::~CObject()
{
    RP_LOG(ObjectLog, Display, "{} object destroyed", GetName());

    OwnedObjects.clear();

    if (ObjectOwner)
    {
        auto& ownerChildren = ObjectOwner->OwnedObjects;
        auto it = std::find_if(
            ownerChildren.begin(), ownerChildren.end(), [this](const std::unique_ptr<CObject>& ptr) { return ptr.get() == this; });

        if (it != ownerChildren.end())
        {

            it->release();
        }

        ObjectOwner = nullptr;
    }
}

void CObject::serialize(nlohmann::json& jsonObject) const
{
    jsonObject["ClassName"] = GetObjectClassName();
    jsonObject["DisplayName"] = DisplayName;
    jsonObject["UUID"] = ObjectUUID;

    if (!OwnedObjects.empty())
    {
        nlohmann::json children = nlohmann::json::array();
        for (const auto& child : OwnedObjects)
        {
            nlohmann::json childJSon;
            child->serialize(childJSon);
            children.push_back(childJSon);
        }
        jsonObject["Children"] = children;
    }
    serializeProperties(jsonObject);
}

void CObject::deserialize(const nlohmann::json& jsonObject)
{
    if (jsonObject.contains("DisplayName") && jsonObject["DisplayName"].is_string())
        DisplayName = jsonObject["DisplayName"].get<std::string>();

    if (jsonObject.contains("UUID") && jsonObject["UUID"].is_string()) ObjectUUID = jsonObject["UUID"].get<std::string>();

    if (jsonObject.contains("Children") && jsonObject["Children"].is_array())
    {
        for (const auto& childJson : jsonObject["Children"])
        {
            if (childJson.contains("ClassName") && childJson["ClassName"].is_string())
            {
                std::string className = childJson["ClassName"].get<std::string>();
                std::string displayName = "LoadedObject";

                if (childJson.contains("DisplayName") && childJson["DisplayName"].is_string())
                    displayName = childJson["DisplayName"].get<std::string>();

                CObject* existingObj = FindObjectByDisplayNameRecursive(displayName);

                if (existingObj)
                {
                    RP_LOG(ObjectLog, Display, "Updating existing object: {}", displayName);
                    existingObj->deserialize(childJson);
                }
                else
                {
                    RP_LOG(ObjectLog, Display, "Creating new object: {}", displayName);
                    CObject* childObj = AddSubObjectByClass(className, displayName);
                    if (childObj) childObj->deserialize(childJson);
                }
            }
        }
    }
    deserializeProperties(jsonObject);
}

CObject* CObject::FindOwnedRecursive(const std::string& displayName) const
{
    for (const auto& obj : OwnedObjects)
    {
        if (obj->GetName() == displayName) return obj.get();

        if (auto found = obj->FindOwnedRecursive(displayName)) return found;
    }
    return nullptr;
}

void CObject::serializeProperties(nlohmann::json& jsonObject) const {}

void CObject::deserializeProperties(const nlohmann::json& jsonObject) {}

CObject* CObject::CreateFromJSON(nlohmann::json jsonObject)
{
    if (!jsonObject.contains("ClassName") || !jsonObject["ClassName"].is_string())
    {
        RP_LOG(ObjectLog, Error, "CreateFromJSON: Missing or invalid 'ClassName' field");
        return nullptr;
    }
    std::string className = jsonObject["ClassName"].get<std::string>();
    std::string displayName = "LoadedObject";

    if (jsonObject.contains("DisplayName") && jsonObject["DisplayName"].is_string())
    {
        displayName = jsonObject["DisplayName"].get<std::string>();
    }
    CObject* obj = OBJECT_FACTORY.Create(className, nullptr, displayName);
    if (!obj)
    {
        RP_LOG(ObjectLog, Error, "CreateFromJSON: Failed to create object of class '{}'", className);
        return nullptr;
    }
    obj->deserialize(jsonObject);

    return obj;
}

CObject* RPE::CObject::LoadFromJSONFile(const std::string& filename)
{
    std::string fullPath = filename;

    try
    {
        auto& engine = Engine::Get();

        if (fullPath.find("Assets/") == std::string::npos && fullPath.find("Assets\\") == std::string::npos)
        {
            fullPath = engine.getAssetsPath() + filename;
        }
    }
    catch (const std::exception& e)
    {
        RP_LOG(ObjectLog, Warning, "Engine not available, using raw path: {}", e.what());
    }

    std::ifstream file(fullPath);
    if (!file.is_open())
    {
        RP_LOG(ObjectLog, Error, "LoadFromJSONFile: Cannot open file '{}'", fullPath);
        return nullptr;
    }

    nlohmann::json jsonObject;
    try
    {
        file >> jsonObject;
    }
    catch (const nlohmann::json::parse_error& e)
    {
        RP_LOG(ObjectLog, Error, "LoadFromJSONFile: JSON parse error in '{}': {}", filename, e.what());
        return nullptr;
    }

    file.close();

    return CreateFromJSON(jsonObject);
}

bool RPE::CObject::SaveToJSONFile(const std::string& filename, bool pretty) const
{
    std::string fullPath = filename;

    try
    {
        auto& engine = Engine::Get();
        if (fullPath.find("Assets/") == std::string::npos && fullPath.find("Assets\\") == std::string::npos)
        {
            fullPath = engine.getAssetsPath() + filename;
        }
    }
    catch (const std::exception& e)
    {
        RP_LOG(ObjectLog, Warning, "Engine not available, using raw path: {}", e.what());
    }

    std::filesystem::path path(fullPath);
    std::filesystem::create_directories(path.parent_path());

    std::ofstream file(fullPath);
    if (!file.is_open())
    {
        RP_LOG(ObjectLog, Error, "SaveToJSONFile: Cannot open file '{}'", fullPath);
        return false;
    }

    nlohmann::json j;
    serialize(j);

    if (pretty)
        file << j.dump(4);
    else
        file << j.dump();

    file.close();
    RP_LOG(ObjectLog, Display, "Saved object to '{}'", fullPath);
    return true;
}

bool CObject::SaveObjectToJSONFile(const CObject* obj, const std::string& filename, bool pretty)
{
    return obj->SaveToJSONFile(filename, pretty);
}

void CObject::SetOwner(CObject* newOwner)
{
    if (ObjectOwner == newOwner) return;

    if (ObjectOwner)
    {
        ObjectOwner->RemoveOwnedObject(GetName());
    }

    ObjectOwner = newOwner;

    if (ObjectOwner)
    {
        ObjectOwner->AddOwnedObject(this);
    }
}

void CObject::UpdateDebugIdentifier() {}

CObject* CObject::FindOwned(const std::string& displayName) const
{
    for (const auto& obj : OwnedObjects)
    {
        if (obj->GetName() == displayName)
        {
            return obj.get();
        }
        const auto& temp = obj->FindOwned(displayName);
        if (temp)
        {
            return temp;
        }
    }
    return nullptr;
}

bool CObject::FindRecursive(const std::string& displayName)
{
    if (GetName() == displayName)
    {
        return true;
    }

    for (const auto& child : OwnedObjects)
    {
        if (child->FindRecursive(displayName))
        {
            return true;
        }
    }

    return false;
}

CObject* CObject::FindObjectByDisplayNameRecursive(const std::string& displayName)
{
    auto FoundChild = FindOwned(displayName);
    if (FoundChild)
    {
        return FoundChild;
    }

    auto root = this;
    while (root->GetOwner())
    {
        root = root->GetOwner();
        FoundChild = root->FindOwned(displayName);
        if (FoundChild) return FoundChild;
    }

    return nullptr;
}

CObject* CObject::FindByUUID(const std::string& uuid) const
{
    if (ObjectUUID == uuid) return const_cast<CObject*>(this);

    return nullptr;
}

CObject* CObject::FindByUUIDRecursive(const std::string& uuid)
{
    if (ObjectUUID == uuid) return this;

    for (const auto& child : OwnedObjects)
    {
        CObject* found = child->FindByUUIDRecursive(uuid);
        if (found) return found;
    }

    return nullptr;
}

std::string CObject::CheckNameAndGenerateUniqName(const std::string& DesiredName)
{
    if (FindRecursive(DesiredName))
    {
        return GenerateUniqueDisplayNameVariant(DesiredName, GetRoot());
    }
    return DesiredName;
}

bool CObject::RemoveOwnedObject(const std::string& displayName)
{
    auto it = std::find_if(OwnedObjects.begin(), OwnedObjects.end(),
        [&displayName](const std::unique_ptr<CObject>& obj) { return obj && obj->GetName() == displayName; });

    if (it != OwnedObjects.end())
    {
        RP_LOG(ObjectLog, Display, "{} [UUID:{}] removed from {} ", (*it)->GetName(), GetShortUUID((*it)->ObjectUUID), GetName());

        OwnedObjects.erase(it);
        return true;
    }
    return false;
}

void CObject::AddOwnedObject(std::unique_ptr<CObject> object)
{
    if (!object || object.get() == this) return;
    if (FindOwned(object.get()->GetName())) return;
    object->ObjectOwner = this;
    OwnedObjects.push_back(std::move(object));
}

void CObject::AddOwnedObject(CObject* object)
{
    if (!object || object == this) return;
    if (FindOwned(object->GetName())) return;
    if (FindObjectByDisplayNameRecursive(object->GetName())) return;
    auto uniquePtr = std::unique_ptr<CObject>(object);
    AddOwnedObject(std::move(uniquePtr));
}

CObject* CObject::AddSubObjectByClass(const std::string& className, const std::string& desiredDisplayName)
{
    if (desiredDisplayName.empty())
    {
        RP_LOG(ObjectLog, Error, "Error: Object display name cannot be empty!");
        return nullptr;
    }

    CObject* root = this;
    while (root->GetOwner())
    {
        root = root->GetOwner();
    }

    std::string finalDisplayName = desiredDisplayName;

    // Check if display name already exists globally
    if (root->FindRecursive(desiredDisplayName))
    {
        // Display name exists in hierarchy, generate unique variant
        finalDisplayName = GenerateUniqueDisplayNameVariant(desiredDisplayName, root);
    }

    // Use factory to create object
    CObject* newObj = OBJECT_FACTORY.Create(className, this, finalDisplayName);

    return newObj;
}

bool CObject::TransferOwnership(CObject* obj, CObject* newOwner)
{
    if (!obj || !newOwner || obj == newOwner || obj == this)
    {
        return false;
    }

    auto it = std::find_if(OwnedObjects.begin(), OwnedObjects.end(),
        [obj](const std::unique_ptr<CObject>& owned)
        {
            bool found = owned.get() == obj;
            return found;
        });

    if (it != OwnedObjects.end())
    {
        std::unique_ptr<CObject> temp = std::move(*it);
        OwnedObjects.erase(it);

        temp->ObjectOwner = newOwner;
        newOwner->AddOwnedObject(std::move(temp));
        return true;
    }
    return false;
}

// Helper function to collect similar display names
void CObject::CollectSimilarDisplayNames(CObject* node, const std::string& baseDisplayName, std::vector<std::string>& result)
{
    if (!node) return;

    std::string name = node->GetName();

    // Check if name starts with baseDisplayName
    if (name.find(baseDisplayName) == 0)
    {
        // Exact match
        if (name == baseDisplayName)
        {
            result.push_back(name);
        }
        // Name in format "baseDisplayName_number"
        else if (name.size() > baseDisplayName.size() && name[baseDisplayName.size()] == '_')
        {
            std::string suffix = name.substr(baseDisplayName.size() + 1);

            // Check if suffix is a number
            bool isNumber = !suffix.empty();
            for (char c : suffix)
            {
                if (!std::isdigit(static_cast<unsigned char>(c)))
                {
                    isNumber = false;
                    break;
                }
            }

            if (isNumber)
            {
                result.push_back(name);
            }
        }
    }

    // Recursively check children
    for (const auto& child : node->GetOwnedObjects())
    {
        CollectSimilarDisplayNames(child.get(), baseDisplayName, result);
    }
}

// Generate unique display name variant
std::string CObject::GenerateUniqueDisplayNameVariant(const std::string& baseDisplayName, CObject* root)
{
    std::vector<std::string> similarNames;
    CollectSimilarDisplayNames(root, baseDisplayName, similarNames);

    // If no similar names, baseDisplayName is available
    if (similarNames.empty())
    {
        return baseDisplayName;
    }

    // Extract numbers from names
    std::vector<int> usedNumbers;
    bool baseNameExists = false;

    for (const auto& name : similarNames)
    {
        if (name == baseDisplayName)
        {
            baseNameExists = true;
            usedNumbers.push_back(0);  // Base name is number 0
        }
        else
        {
            std::string suffix = name.substr(baseDisplayName.size() + 1);
            try
            {
                int num = std::stoi(suffix);
                usedNumbers.push_back(num);
            }
            catch (...)
            {
                // Not a number, ignore
            }
        }
    }

    // Sort numbers
    std::sort(usedNumbers.begin(), usedNumbers.end());

    // Find first available number
    int nextNumber = baseNameExists ? 1 : 0;
    for (int num : usedNumbers)
    {
        if (num == nextNumber)
        {
            nextNumber++;
        }
        else if (num > nextNumber)
        {
            break;  // Found a gap
        }
    }

    // If nextNumber is 0, return baseDisplayName (shouldn't happen here)
    if (nextNumber == 0)
    {
        return baseDisplayName;
    }

    return baseDisplayName + "_" + std::to_string(nextNumber);
}

bool CObject::RenameOwnedObject(const std::string& oldDisplayName, const std::string& newDisplayName)
{
    if (oldDisplayName.empty())
    {
        return false;
    }

    if (newDisplayName.empty())
    {
        return false;
    }

    // Check if new display name is already used among children
    if (FindOwned(newDisplayName))
    {
        return false;
    }

    CObject* obj = FindOwned(oldDisplayName);
    if (obj)
    {
        return obj->Rename(newDisplayName);
    }

    return false;
}

std::unique_ptr<CObject> CObject::Clone() const
{
    auto clone = std::make_unique<CObject>(GetName() + "_Copy");

    for (const auto& child : OwnedObjects)
    {
        auto childClone = child->Clone();
        clone->AddOwnedObject(std::move(childClone));
    }

    return clone;
}

bool CObject::Rename(const std::string& newDisplayName)
{
    if (newDisplayName.empty())
    {
        return false;
    }

    // If display name hasn't changed
    if (newDisplayName == DisplayName)
    {
        return true;
    }

    // Get hierarchy root
    CObject* root = GetRoot();

    // Global uniqueness check
    if (root->FindRecursive(newDisplayName))
    {
        auto existingObj = FindObjectByDisplayNameRecursive(newDisplayName);
        if (existingObj && typeid(*existingObj) == typeid(*this))
        {
            // Generate unique display name
            std::string uniqueDisplayName = GenerateUniqueDisplayNameVariant(newDisplayName, root);

            std::string oldDisplayName = DisplayName;
            DisplayName = uniqueDisplayName;
            return true;
        }
        return false;
    }

    // Display name is unique, rename
    std::string oldDisplayName = DisplayName;
    DisplayName = newDisplayName;
    return true;
}
