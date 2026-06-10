#pragma once
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include "Utility.h"

namespace RPE
{
class SysObject : public NonCopyable
{
public:
    SysObject(SysObject* inOwner = nullptr, const std::string& inObjectName = "Object");
    virtual ~SysObject();

    void setOwner(SysObject* inOwner);
    SysObject* getOwner() const;

    std::string& getName();
    const std::string& getName() const;

    // Управление дочерними объектами
    void addChild(std::shared_ptr<SysObject> child);
    void removeChild(SysObject* child);
    void removeChild(const std::string& name);
    void clearChildren();

    SysObject* getChild(size_t index) const;
    SysObject* getChild(const std::string& name) const;
    size_t getChildCount() const;
    bool hasChildren() const { return !m_childs.empty(); }

    // Поиск
    SysObject* findChild(const std::string& name) const;
    SysObject* findChildRecursive(const std::string& name) const;

    // Утилиты
    bool isRoot() const { return m_Owner == nullptr; }
    bool isChildOf(const SysObject* potentialParent) const;

    template <typename ClassName, typename... Args>
    ClassName* addSubobject(const std::string& newObjectName, Args&&... args)
    {
        static_assert(std::is_base_of<SysObject, ClassName>::value, "Subobject class must be derived from SysObject");

        auto newSysObject = std::make_shared<ClassName>(this, newObjectName, std::forward<Args>(args)...);
        if (newSysObject)
        {
            m_childs.push_back(newSysObject);
            return newSysObject.get();
        }
        return nullptr;
    }

protected:
    virtual void onOwnerChanged(SysObject* oldOwner, SysObject* newOwner) {}
    virtual void onChildAdded(SysObject* child) {}
    virtual void onChildRemoved(SysObject* child) {}
    virtual void onBeforeDestroy() {}

private:
    SysObject* m_Owner = nullptr;
    std::string m_name{"Object"};
    std::vector<std::shared_ptr<SysObject>> m_childs;

    auto findChildIterator(SysObject* child) const;
    auto findChildIterator(const std::string& name) const;
};
}  // namespace RPE