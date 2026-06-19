#include "Level.h"
#include "World.h"
#include "../GameObjects/WorldObject/WorldActor.h"
#include "../GameObjects/WorldObject/Character.h"
#include "../Controller/PlayerController.h"
#include "../../Input/InputManager.h"
#include "Core/Engine.h"
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(levelLog);

using namespace RPE;
REGISTER_CLASS_FACTORY(WLevel);

WLevel::WLevel(const std::string& inName, CObject* inOwner) : Super(inName, inOwner)
{
    bisLoaded = false;
    bHasBegunPlay = false;
    auto* controller = dynamic_cast<PlayerController*>(SpawnActor("PlayerController", "Player_Controller"));
    pawn = SpawnActor<WCharacter>("Player");
    if (controller)
    {
        controller->possess(pawn);
    }
}

WLevel::~WLevel()
{
    if (bisLoaded)
    {
        onLevelUnload();
    }
    DestroyAllActors();
}

void WLevel::Tick(float deltaTime)
{
    for (auto& [name, actor] : m_actors)
    {
        if (actor)
        {
            actor->Tick(deltaTime);
        }
    }
}

void WLevel::BeginPlay()
{
    if (bHasBegunPlay)
    {
        RP_LOG(levelLog, Warning, "Level '{}' BeginPlay already called!", GetName());
        return;
    }

    bHasBegunPlay = true;
    RP_LOG(levelLog, Display, "Level '{}' BeginPlay", GetName());

    for (auto& [name, actor] : m_actors)
    {
        if (actor)
        {
            actor->BeginPlay();
        }
    }
}

void WLevel::EndPlay()
{
    if (!bHasBegunPlay)
    {
        RP_LOG(levelLog, Warning, "Level '{}' EndPlay without BeginPlay!", GetName());
        return;
    }

    RP_LOG(levelLog, Display, "Level '{}' EndPlay", GetName());

    for (auto& [name, actor] : m_actors)
    {
        if (actor)
        {
            actor->EndPlay();
        }
    }

    bHasBegunPlay = false;
}

bool WLevel::isLoaded() const
{
    return bisLoaded;
}

WWorld* WLevel::getWorld()
{
    return dynamic_cast<WWorld*>(ObjectOwner);
}

const WWorld* WLevel::getWorld() const
{
    return dynamic_cast<const WWorld*>(ObjectOwner);
}

WActor* WLevel::SpawnActor(const std::string& ActorClass, const std::string& ActorName)
{
    CObject* obj = OBJECT_FACTORY.Create(ActorClass, this, ActorName);
    if (!obj)
    {
        RP_LOG(levelLog, Error, "Failed to create actor of class {} in level {}", ActorClass, GetName());
        return nullptr;
    }
    if (WActor* actor = dynamic_cast<WActor*>(obj))
    {
        std::string realname = actor->GetName();
        m_actors[realname] = actor;
        RP_LOG(levelLog, Display, "Actor of class '{}' with name '{}' was spawned in level {}", ActorClass, realname, GetName());
        if (bHasBegunPlay && bisLoaded)
        {
            actor->BeginPlay();
        }
        return actor;
    }
    if (obj)
    {
        RemoveOwnedObject(obj->GetName());
        RP_LOG(levelLog, Error, "Object of class '{}' is not an actor", ActorClass);
    }
    return nullptr;
}

void WLevel::onLevelLoaded()
{
    if (bisLoaded)
    {
        RP_LOG(levelLog, Warning, "Level '{}' already loaded!", GetName());
        return;
    }

    bisLoaded = true;
    RP_LOG(levelLog, Display, "Level '{}' loaded", GetName());
}

void WLevel::onLevelUnload()
{
    if (!bisLoaded)
    {
        RP_LOG(levelLog, Warning, "Level '{}' is not loaded!", GetName());
        return;
    }

    if (bHasBegunPlay)
    {
        EndPlay();
    }

    bisLoaded = false;
    RP_LOG(levelLog, Display, "Level '{}' unloaded", GetName());
}

void WLevel::serializeProperties(nlohmann::json& jsonObject) const
{
    Super::serializeProperties(jsonObject);

    // Сохраняем ТОЛЬКО ссылки на акторов (как в WWorld с Levels)
    nlohmann::json actorsArray = nlohmann::json::array();
    for (const auto& [name, actor] : m_actors)
    {
        if (actor)
        {
            nlohmann::json actorInfo;
            actorInfo["Name"] = name;
            actorInfo["UUID"] = actor->GetUUID();
            actorsArray.push_back(actorInfo);
        }
    }
    jsonObject["Actors"] = actorsArray;
    jsonObject["isLoaded"] = bisLoaded;

    RP_LOG(levelLog, Display, "Level '{}' serialized {} actor references", GetName(), m_actors.size());
}

void WLevel::deserializeProperties(const nlohmann::json& jsonObject)
{
    RP_LOG(levelLog, Display, "deserializeproperties with dynamic_cast");
    Super::deserializeProperties(jsonObject);

    if (jsonObject.contains("isLoaded"))
    {
        bisLoaded = jsonObject["isLoaded"].get<bool>();
    }

    if (jsonObject.contains("Actors") && jsonObject["Actors"].is_array())
    {
        for (const auto& actorInfo : jsonObject["Actors"])
        {
            if (actorInfo.contains("UUID") && actorInfo["UUID"].is_string())
            {
                std::string actorUUID = actorInfo["UUID"].get<std::string>();

                CObject* found = FindByUUIDRecursive(actorUUID);

                WActor* actor = dynamic_cast<WActor*>(found);
                if (actor)
                {
                    std::string actorName = actor->GetName();
                    m_actors[actorName] = actor;
                    RP_LOG(levelLog, Display, "Restored actor reference: '{}' (UUID: {})", actorName, actorUUID.substr(0, 8));
                }
                else
                {
                    RP_LOG(levelLog, Warning, "Actor with UUID '{}' not found in level '{}'", actorUUID, GetName());
                }
            }
        }
    }

    RP_LOG(levelLog, Display, "Level '{}' deserialized {} actor references", GetName(), m_actors.size());
}

void WLevel::DestroyActor(const std::string& ActorName)
{
    auto it = m_actors.find(ActorName);
    if (it != m_actors.end())
    {
        WActor* actor = it->second;
        if (actor)
        {
            if (bHasBegunPlay)
            {
                actor->EndPlay();
            }
            RemoveOwnedObject(actor->GetName());
        }
        m_actors.erase(it);
        RP_LOG(levelLog, Display, "Destroyed actor '{}' from level '{}'", ActorName, GetName());
    }
}

void WLevel::DestroyActor(WActor* Actor)
{
    if (Actor)
    {
        DestroyActor(Actor->GetName());
    }
}

void WLevel::DestroyAllActors()
{
    std::vector<std::string> actorNames;
    for (const auto& [name, actor] : m_actors)
    {
        actorNames.push_back(name);
    }

    for (const auto& name : actorNames)
    {
        DestroyActor(name);
    }

    m_actors.clear();
}

bool WLevel::HasActor(const std::string& ActorName) const
{
    return m_actors.find(ActorName) != m_actors.end();
}