#pragma once
#include "Core/SystemObject.h"
#include <unordered_map>
#include <string>

namespace RPE
{
class WWorld;
class WActor;

class WLevel : public CObject
{
    CHUDDO_DECLARE_CLASS(WLevel, CObject)
public:
    WLevel(const std::string& inName = "Level", CObject* inOwner = nullptr);
    virtual ~WLevel();
    // life circle
    virtual void Tick(float deltaTime);
    virtual void BeginPlay();
    virtual void EndPlay();
    bool isLoaded() const;
    WWorld* getWorld();
    const WWorld* getWorld() const;

    template <typename ActorClass>
    ActorClass* SpawnActor(const std::string& ActorName);
    template <typename ActorClass>
    ActorClass* GetActor(const std::string& ActorName) const;

    void DestroyActor(const std::string& ActorName);
    void DestroyActor(WActor* Actor);
    void DestroyAllActors();

    size_t GetActorCount() const { return m_actors.size(); }
    const std::unordered_map<std::string, WActor*>& GetAllActors() const { return m_actors; }

    bool HasActor(const std::string& ActorName) const;

protected:
    virtual void onLevelLoaded();
    virtual void onLevelUnload();
    void serializeProperties(nlohmann::json& jsonObject) const override;
    void deserializeProperties(const nlohmann::json& jsonObject) override;
    class WPawn* pawn = nullptr;

private:
    std::unordered_map<std::string, WActor*> m_actors;
    bool bisLoaded{false};
    bool bHasBegunPlay{false};
    friend class WWorld;
};
}  // namespace RPE

namespace RPE
{
template <typename ActorClass>
inline ActorClass* WLevel::SpawnActor(const std::string& ActorName)
{
    static_assert(std::is_base_of_v<WActor, ActorClass>, "Spawned class must be derived from WActor");

    if (HasActor(ActorName))
    {
        return nullptr;
    }

    ActorClass* Actor = AddSubObject<ActorClass>(ActorName);
    if (Actor)
    {
        std::string realName = Actor->GetName();
        m_actors[realName] = Actor;

        if (bHasBegunPlay && bisLoaded)
        {
            Actor->BeginPlay();
        }

        return Actor;
    }

    return nullptr;
}

template <typename ActorClass>
ActorClass* WLevel::GetActor(const std::string& ActorName) const
{
    auto it = m_actors.find(ActorName);
    if (it != m_actors.end())
    {
        return dynamic_cast<ActorClass*>(it->second);
    }
    return nullptr;
}
}  // namespace RPE