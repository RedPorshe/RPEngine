#include "SystemObject.h"
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(ObjectLog)

namespace RPE
{

SysObject::SysObject(SysObject* inOwner, const std::string& inObjectName) : m_Owner(inOwner), m_name(inObjectName)
{
    RP_LOG(ObjectLog, Display, "{} object created", getName());
}

// Private helper methods
auto SysObject::findChildIterator(SysObject* child) const
{
    return std::find_if(m_childs.begin(), m_childs.end(), [child](const std::shared_ptr<SysObject>& ptr) { return ptr.get() == child; });
}

auto SysObject::findChildIterator(const std::string& name) const
{
    return std::find_if(
        m_childs.begin(), m_childs.end(), [&name](const std::shared_ptr<SysObject>& ptr) { return ptr && ptr->getName() == name; });
}

SysObject::~SysObject()
{
    RP_LOG(ObjectLog, Display, "{} object destroying", getName());

    onBeforeDestroy();

    // Очищаем всех детей
    clearChildren();

    // Отвязываемся от владельца
    if (m_Owner)
    {
        m_Owner->removeChild(this);
        m_Owner = nullptr;
    }

    RP_LOG(ObjectLog, Display, "{} object destroyed", getName());
}

void SysObject::setOwner(SysObject* inOwner)
{
    if (m_Owner == inOwner) return;

    SysObject* oldOwner = m_Owner;

    // Отвязываемся от старого владельца
    if (oldOwner) oldOwner->removeChild(this);

    // Привязываемся к новому владельцу
    m_Owner = inOwner;

    if (m_Owner) m_Owner->addChild(std::shared_ptr<SysObject>(this, [](SysObject*) {}));

    onOwnerChanged(oldOwner, inOwner);

    RP_LOG(ObjectLog, Display, "{} owner changed from {} to {}", getName(), oldOwner ? oldOwner->getName() : "null",
        m_Owner ? m_Owner->getName() : "null");
}

SysObject* SysObject::getOwner() const
{
    return m_Owner;
}

std::string& SysObject::getName()
{
    return m_name;
}

const std::string& SysObject::getName() const
{
    return m_name;
}

void SysObject::addChild(std::shared_ptr<SysObject> child)
{
    if (!child) return;
    if (child.get() == this)
    {
        RP_LOG(ObjectLog, Error, "Cannot add self '{}' as child", getName());
        return;
    }
    if (isChildOf(child.get()))
    {
        RP_LOG(ObjectLog, Error, "Cannot add '{}' as child of '{}' - would create circular reference", child->getName(), getName());
        return;
    }
    if (findChildIterator(child.get()) != m_childs.end())
    {
        RP_LOG(ObjectLog, Warning, "Child '{}' already exists in '{}'", child->getName(), getName());
        return;
    }

    // Удаляем из предыдущего владельца
    if (child->getOwner() && child->getOwner() != this)
    {
        child->getOwner()->removeChild(child.get());
    }

    m_childs.push_back(child);
    child->m_Owner = this;
    onChildAdded(child.get());

    RP_LOG(ObjectLog, Display, "Child '{}' added to '{}'", child->getName(), getName());
}

void SysObject::removeChild(SysObject* child)
{
    auto it = findChildIterator(child);
    if (it != m_childs.end())
    {
        onChildRemoved(child);
        m_childs.erase(it);

        RP_LOG(ObjectLog, Display, "Child {} removed from {}", child->getName(), getName());
    }
}

void SysObject::removeChild(const std::string& name)
{
    auto it = findChildIterator(name);
    if (it != m_childs.end())
    {
        onChildRemoved(it->get());
        m_childs.erase(it);

        RP_LOG(ObjectLog, Display, "Child {} removed from {}", name, getName());
    }
}

void SysObject::clearChildren()
{
    for (auto& child : m_childs)
    {
        if (child) child->m_Owner = nullptr;  // Предотвращаем обратную связь
    }
    m_childs.clear();
    RP_LOG(ObjectLog, Display, "All children cleared from {}", getName());
}

SysObject* SysObject::getChild(size_t index) const
{
    if (index < m_childs.size()) return m_childs[index].get();
    return nullptr;
}

SysObject* SysObject::getChild(const std::string& name) const
{
    auto it = findChildIterator(name);
    return (it != m_childs.end()) ? it->get() : nullptr;
}

size_t SysObject::getChildCount() const
{
    return m_childs.size();
}

SysObject* SysObject::findChild(const std::string& name) const
{
    return getChild(name);
}

SysObject* SysObject::findChildRecursive(const std::string& name) const
{
    // Проверяем прямых детей
    if (auto direct = findChild(name)) return direct;

    // Рекурсивно ищем у детей
    for (const auto& child : m_childs)
    {
        if (child)
        {
            if (auto found = child->findChildRecursive(name)) return found;
        }
    }

    return nullptr;
}

bool SysObject::isChildOf(const SysObject* potentialParent) const
{
    const SysObject* current = m_Owner;
    while (current)
    {
        if (current == potentialParent) return true;
        current = current->getOwner();
    }
    return false;
}

}  // namespace RPE