#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include "skill.h"

class SkillDatabase {
private:
    static std::map<std::string, std::map<int, std::vector<nlohmann::json>>> m_database;

public:

 // Call this once when the game boots up
    static bool loadSkills(const std::string& filePath);

    // Call this when a player levels up
    static std::vector<std::unique_ptr<Skill>> getSkillsForLevel(const std::string& discipline, int level);


};