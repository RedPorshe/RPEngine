#include "Core/SystemObject.h"
#include "Core/ObjectFactory.h"
#include <functional>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <fstream>
#include "Log/Log.h"
#include <nlohmann/json.hpp>

// json tests
using json = nlohmann::json;

json toJSon(RPE::CObject* object)
{
    if (!object) return json{};
    json j;
    j["ClassName"] = object->GetObjectClassName();
    j["DisplayName"] = object->GetName();
    j["UUID"] = object->GetUUID();

    // Сериализуем детей
    json children = json::array();
    for (const auto& child : object->GetOwnedObjects())
    {
        children.push_back(toJSon(child.get()));
    }
    j["Children"] = children;

    return j;
}

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
    auto jj = toJSon(this);
    if (!jj.empty())
    {
        std::cout << jj << "\n";
    }
}

CObject::~CObject()
{
    RP_LOG(ObjectLog, Display, "{} object destroyed", GetName());

    // Очищаем детей
    OwnedObjects.clear();

    if (ObjectOwner)
    {
        // Ищем и удаляем себя из вектора владельца
        auto& ownerChildren = ObjectOwner->OwnedObjects;
        auto it = std::find_if(
            ownerChildren.begin(), ownerChildren.end(), [this](const std::unique_ptr<CObject>& ptr) { return ptr.get() == this; });

        if (it != ownerChildren.end())
        {
            // Отвязываем связь, но не удаляем объект (он уже удаляется)
            it->release();  // Освобождаем указатель, чтобы не было двойного удаления
                            // ownerChildren.erase(it);
        }

        ObjectOwner = nullptr;
    }
}

void CObject::SetOwner(CObject* newOwner)
{
    if (ObjectOwner == newOwner) return;

    // Удаляем из старого владельца
    if (ObjectOwner)
    {
        ObjectOwner->RemoveOwnedObject(GetName());
    }

    // Устанавливаем нового владельца
    ObjectOwner = newOwner;

    // Добавляем к новому владельцу
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

    // Search up the hierarchy
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

    // Find hierarchy root for global check
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
