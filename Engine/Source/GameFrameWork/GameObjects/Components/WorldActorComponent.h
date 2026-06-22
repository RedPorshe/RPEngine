#pragma once
#include "Core/SystemObject.h"
#include <vector>

namespace RPE
{

class WActor;

class WActorComponent : public CObject
{
    CHUDDO_DECLARE_CLASS(WActorComponent, CObject);

public:
    WActorComponent(const std::string& inDisplayName = "Actor Component", CObject* inOwner = nullptr);
    virtual ~WActorComponent();

    virtual void tick(float deltaTime);
    void onDestroy() override;
    virtual void onBeginPlay();

    bool hasChildComponents() const { return !m_childComponents.empty(); }
    WActor* getOwner() const { return m_owner; }

    virtual void attachTo(WActorComponent* inOwnComponent) {};

    void addChildComponent(WActorComponent* component);
    void removeChildComponent(WActorComponent* component);

    WActorComponent* getChildComponent(const std::string& componentName) const;
    WActorComponent* getChildComponent(int index) const;

    int getChildComponentIndex(const std::string& componentName) const;
    int getChildComponentIndex(const WActorComponent* component) const;

    size_t getChildComponentCount() const { return m_childComponents.size(); }
    const std::vector<WActorComponent*>& getChildComponents() const { return m_childComponents; }
    WActorComponent* getParent() const { return m_parent; }
    void SetPrimaryTickEnable(bool enable = true);

protected:
    virtual void OnCreate();
    WActor* m_owner = nullptr;
    WActorComponent* m_parent = nullptr;
    bool bIsInitialized{false};
    bool bisUseTick{false};
    std::vector<WActorComponent*> m_childComponents;
    void serializeProperties(nlohmann::json& jsonObject) const override;
    void deserializeProperties(const nlohmann::json& jsonObject) override;
    void SetIsEnabled(bool enable = true);
    bool bIsEnabled{false};
    void Created();
};

}  // namespace RPE
namespace RPE
{
REGISTER_CLASS_FACTORY(WActorComponent);
}