#include "Core/ObjectFactory.h"
#include "SystemObject.h"
#include <set>
#include "Log/Log.h"

using namespace RPE;
DEFINE_LOG_CATEGORY_STATIC(FactoryLog);

bool CObjectFactory::IsDerivedFrom(const std::string& ClassName, const std::string& BaseClassName) const
{
    // Базовая валидация
    if (ClassName.empty() || BaseClassName.empty())
    {
        return false;
    }

    auto it = ClassHierarchy.find(ClassName);
    if (it == ClassHierarchy.end())
    {
        return false;
    }

    // Класс является наследником самого себя
    if (ClassName == BaseClassName)
    {
        return true;
    }

    std::string current = it->second;

    // Защита от циклов
    std::set<std::string> visited;
    visited.insert(ClassName);

    while (!current.empty())
    {
        // Проверка на цикл
        if (visited.find(current) != visited.end())
        {
            return false;
        }
        visited.insert(current);

        if (current == BaseClassName)
        {
            return true;
        }

        auto parentIt = ClassHierarchy.find(current);
        if (parentIt != ClassHierarchy.end())
        {
            current = parentIt->second;
        }
        else
        {
            current.clear();
        }
    }

    return false;
}

CObject* CObjectFactory::Create(const std::string& className, CObject* owner, const std::string& displayName)
{

    if (displayName.empty())
    {
        return nullptr;
    }

    std::string finalDisplayName = displayName;
    if (owner)
    {
        CObject* root = owner->GetRoot();
        if (root->FindRecursive(finalDisplayName))
        {
            finalDisplayName = CObject::GenerateUniqueDisplayNameVariant(finalDisplayName, root);
        }
    }

    auto it = ClassCreators.find(className);
    if (it != ClassCreators.end())
    {

        CObject* obj = it->second(finalDisplayName, owner);
        if (obj && owner)
        {
            if (obj->GetOwner() != owner)
            {
                owner->AddOwnedObject(obj);
                obj->SetOwner(owner);
            }
        }
        return obj;
    }

    std::string parentClass = FindParentClass(className);
    if (!parentClass.empty())
    {
        return Create(parentClass, owner, finalDisplayName);
    }

    RP_LOG(FactoryLog, Error, "[FACTORY] ERROR - Unknown class type: '{}'", className);
    if (!ClassCreators.empty())
    {
        RP_LOG(FactoryLog, Error, "[FACTORY] Available classes:");

        for (const auto& [name, _] : ClassCreators)
            RP_LOG(FactoryLog, Error, "     {}", name);
    }
    return nullptr;
}