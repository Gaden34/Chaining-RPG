#include "skillDatabase.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp> // Include the library

// Initialize our static database storage
std::map<std::string, std::map<int, std::vector<nlohmann::json>>> SkillDatabase::m_database;

using json = nlohmann::json;

bool SkillDatabase::loadSkills(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open skills JSON file at: " << filePath << std::endl;
        return false;
    }

    json data;
    file >> data; // This single line parses the entire file!

    // Loop through disciplines (e.g., "Combatant")
    for (auto& [disciplineName, levels] : data.items()) {
        // Loop through levels (e.g., "2")
        for (auto& [levelStr, skillArray] : levels.items()) {
            int level = std::stoi(levelStr);
            
            for (auto& skillJson : skillArray) {
                m_database[disciplineName][level].push_back(skillJson);
            }
        }
    }
    return true;
}

std::vector<std::unique_ptr<Skill>> SkillDatabase::getSkillsForLevel(const std::string& discipline, int level) {
    std::vector<std::unique_ptr<Skill>> unlockedSkills;

    // Check if we have any data for this combination
    if (m_database.count(discipline) && m_database[discipline].count(level)) {
        for (const auto& skillJson : m_database[discipline][level]) {
            std::string typeStr = skillJson.value("type", "Attack");
            SkillType skillType = Skill::getSkillTypeFromString(typeStr);
            
            // Extract values safely using .value(key, default_fallback)
            auto skill = std::make_unique<Skill>(
                skillJson.value("name", "Unknown Skill"),
                skillJson.value("description", ""),
                skillJson.value("mp_cost", 0),
                skillType, 
                skillJson.value("base_damage", 0),
                skillJson.value("hits", std::vector<int>{}),
                skillJson.value("animation", "")
              );

            // Handle unique chain parameters if they exist in the JSON object


            unlockedSkills.push_back(std::move(skill));
        }
    }
    return unlockedSkills;
}


