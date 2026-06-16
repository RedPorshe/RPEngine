#pragma once
#include "Core/SystemObject.h"
#include <unordered_map>
#include <string>

namespace RPE
{
class WLevel;

class WWorld : public CObject
{
    CHUDDO_DECLARE_CLASS(WWorld, CObject)
public:
    WWorld(const std::string& inName = "World", CObject* inOwner = nullptr);
    virtual ~WWorld();
    // life circle
    void Tick(float deltaTime);
    void BeginPlay();
    void EndPlay();
    // levels managment
    WLevel* getCurrentLevel() const;
    void createLevel(const std::string& levelName);
    const WLevel* getLevel(const std::string& levelName) const;
    WLevel* getLevel(const std::string& levelName);
    void loadLevel(const std::string& levelName);
    void unloadLevel(const std::string& levelName);
    bool hasLevel(const std::string& levelName) const;
    // get levels
    const std::unordered_map<std::string, WLevel*>& getAllLevels() const { return m_levels; }
    size_t getLevelCount() const { return m_levels.size(); }
    // setting current level
    void setCurrentLevel(const std::string& levelName);
    void setCurrentLevel(WLevel* level);

protected:
    virtual void onLevelLoaded(WLevel* level);
    virtual void onLevelUnload(WLevel* level);
    void serializeProperties(nlohmann::json& jsonObject) const override;
    void deserializeProperties(const nlohmann::json& jsonObject) override;
    WLevel* CurrentLevel = nullptr;
    std::unordered_map<std::string, WLevel*> m_levels;
    void unloadLevels();
    void clearLevels();
    bool isGameStarted{false};

    friend class RGameInstance;
};
}  // namespace RPE