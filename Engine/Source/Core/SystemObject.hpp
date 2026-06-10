#pragma once
#include "Core/SystemObject.h"
#include "Core/ObjectFactory.h"
#include "Log/Log.h"

namespace RPE
{

template <typename ClassName, typename... Args>
inline ClassName* CObject::AddSubObject(const std::string& desiredDisplayName, Args&&... args)
{
    static_assert(std::is_base_of<CObject, ClassName>::value, "Class must be derived from CObject");

    if (desiredDisplayName.empty()) return nullptr;

    CObject* root = this;
    while (root->GetOwner())
        root = root->GetOwner();
    std::string finalDisplayName = desiredDisplayName;
    finalDisplayName = CheckNameAndGenerateUniqName(finalDisplayName);

    if (root->FindRecursive(finalDisplayName))
    {
        finalDisplayName = GenerateUniqueDisplayNameVariant(desiredDisplayName, root);
    }

    std::string classNameStr = ClassName::StaticClassName();
    CObject* obj = OBJECT_FACTORY.Create(classNameStr, this, finalDisplayName);
    return static_cast<ClassName*>(obj);
}

}  // namespace RPE

namespace RPE
{
REGISTER_CLASS_FACTORY(CObject);
}