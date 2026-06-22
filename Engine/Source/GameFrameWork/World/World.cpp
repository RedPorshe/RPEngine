#include "World.h"
#include "Level.h"
#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(WorldLog);
using namespace RPE;
REGISTER_CLASS_FACTORY(WWorld);

WWorld::WWorld(const std::string& inName, CObject* inOwner)  //
    : Super(inName, inOwner)
{
    isGameStarted = false;
}

WWorld::~WWorld()
{
    clearLevels();
    CurrentLevel = nullptr;
}

void WWorld::Tick(float deltaTime)
{
    if (CurrentLevel)
    {
        CurrentLevel->Tick(deltaTime);
    }
}

void WWorld::BeginPlay()
{
    isGameStarted = true;
    RP_LOG(WorldLog, Display, "[{}] World BeginPlay", GetName());

    if (CurrentLevel)
    {
        CurrentLevel->BeginPlay();
    }
}

void WWorld::EndPlay()
{
    RP_LOG(WorldLog, Display, "[{}] World EndPlay", GetName());

    if (CurrentLevel)
    {
        CurrentLevel->EndPlay();
    }
}

WLevel* WWorld::getCurrentLevel() const
{
    return CurrentLevel;
}

void WWorld::createLevel(const std::string& levelName)
{
    if (!hasLevel(levelName))
    {
        auto newLevel = AddSubObject<WLevel>(levelName);
        if (newLevel)
        {
            m_levels[levelName] = newLevel;
            newLevel->onLevelLoaded();
            RP_LOG(WorldLog, Display, "[{}] Created level: '{}'", GetName(), levelName);
            if (!CurrentLevel)
            {
                setCurrentLevel(newLevel);
            }
        }
    }
}

WLevel* WWorld::getLevel(const std::string& levelName)
{
    auto it = m_levels.find(levelName);
    return it != m_levels.end() ? it->second : nullptr;
}

const WLevel* WWorld::getLevel(const std::string& levelName) const
{
    auto it = m_levels.find(levelName);
    return it != m_levels.end() ? it->second : nullptr;
}

void WWorld::loadLevel(const std::string& levelName)
{
    if (!hasLevel(levelName)) return;
    if (CurrentLevel && CurrentLevel->GetName() == levelName && CurrentLevel->isLoaded()) return;
    setCurrentLevel(levelName);
}

void WWorld::unloadLevel(const std::string& levelName)
{
    auto* level = getLevel(levelName);
    if (!level) return;

    if (CurrentLevel == level)
    {
        CurrentLevel->EndPlay();
        CurrentLevel->onLevelUnload();
        CurrentLevel = nullptr;
        return;
    }

    level->onLevelUnload();
}

bool WWorld::hasLevel(const std::string& levelName) const
{
    bool found = m_levels.find(levelName) != m_levels.end();
    RP_LOG(WorldLog, Display, "hasLevel('{}') = {} (total levels: {})", levelName, found, m_levels.size());

    // Выводим все существующие ключи для отладки
    if (!found)
    {
        RP_LOG(WorldLog, Display, "Existing level names:");
        for (const auto& [name, level] : m_levels)
        {
            RP_LOG(WorldLog, Display, "  - '{}' (loaded: {})", name, level->isLoaded());
        }
    }

    return found;
}

void WWorld::setCurrentLevel(const std::string& levelName)
{
    if (!hasLevel(levelName)) return;
    if (CurrentLevel && CurrentLevel->GetName() == levelName) return;

    auto level = getLevel(levelName);
    if (level)
    {
        setCurrentLevel(level);
    }
}

void WWorld::setCurrentLevel(WLevel* level)
{
    if (level == nullptr) return;
    if (CurrentLevel == level) return;

    if (CurrentLevel)
    {
        CurrentLevel->EndPlay();
        CurrentLevel->onLevelUnload();
    }

    CurrentLevel = level;

    if (CurrentLevel && CurrentLevel->isLoaded())
    {
        if (isGameStarted)
        {
            CurrentLevel->BeginPlay();
        }
    }
}

void WWorld::onLevelLoaded(WLevel* level)
{
    if (level)
    {
        level->onLevelLoaded();
    }
}

void WWorld::onLevelUnload(WLevel* level)
{
    if (level)
    {
        level->onLevelUnload();
    }
}

void WWorld::serializeProperties(nlohmann::json& jsonObject) const
{
    Super::serializeProperties(jsonObject);
    using namespace nlohmann;
    json levelsjson = json::array();
    for (const auto& [name, level] : m_levels)
    {
        if (level)
        {
            json leveljson;
            leveljson["Name"] = name;
            leveljson["UUID"] = level->GetUUID();
            levelsjson.push_back(leveljson);
        }
    }
    jsonObject["Levels"] = levelsjson;
    if (CurrentLevel)
    {
        jsonObject["CurrentLevelName"] = CurrentLevel->GetName();
    }
    RP_LOG(WorldLog, Display, "[{}] Serialized {} level references", GetName(), m_levels.size());
}

void WWorld::deserializeProperties(const nlohmann::json& jsonObject)
{
    Super::deserializeProperties(jsonObject);
    //  clearLevels();
    if (jsonObject.contains("Levels") && jsonObject["Levels"].is_array())
    {
        for (const auto& levelInfo : jsonObject["Levels"])
        {
            if (levelInfo.contains("Name") && levelInfo["Name"].is_string())
            {
                std::string levelName = levelInfo["Name"].get<std::string>();

                WLevel* existingLevel = nullptr;

                auto found = FindObjectByDisplayNameRecursive(levelName);
                if (found && found->GetObjectClassName() == std::string("WLevel"))
                {
                    existingLevel = static_cast<WLevel*>(found);
                }

                if (existingLevel)
                {
                    m_levels[levelName] = existingLevel;
                    RP_LOG(WorldLog, Display, "[{}] Restored level reference: '{}'", GetName(), levelName);
                }
                else
                {
                    RP_LOG(WorldLog, Warning, "[{}] Level '{}' not found during deserialization", GetName(), levelName);
                }
            }
        }
    }

    if (jsonObject.contains("CurrentLevelName") && jsonObject["CurrentLevelName"].is_string())
    {
        std::string currentLevelName = jsonObject["CurrentLevelName"].get<std::string>();
        auto it = m_levels.find(currentLevelName);
        if (it != m_levels.end())
        {
            CurrentLevel = it->second;
            RP_LOG(WorldLog, Display, "[{}] Restored current level: '{}'", GetName(), currentLevelName);
        }
    }

    RP_LOG(WorldLog, Display, "[{}] Deserialized {} level references", GetName(), m_levels.size());
}

void WWorld::unloadLevels()
{
    std::vector<std::string> levelsToUnload;
    for (auto& [name, level] : m_levels)
    {
        if (level->isLoaded())
        {
            levelsToUnload.push_back(name);
        }
    }

    for (const auto& name : levelsToUnload)
    {
        unloadLevel(name);
        RP_LOG(WorldLog, Display, "Level {} unloaded", name);
    }
}

void WWorld::clearLevels()
{

    unloadLevels();
    for (auto& [Name, Level] : m_levels)
    {
        if (Level)
        {
            RemoveOwnedObject(Level->GetName());
        }
    }
    m_levels.clear();
    isGameStarted = false;
}
