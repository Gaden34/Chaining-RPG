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
	for (int i = 1; i <= level; ++i) {
		if (m_database.count(discipline) && m_database[discipline].count(i)) {
			for (const auto& skillJson : m_database[discipline][i]) {
				std::string typeStr = skillJson.value("type", "Attack");
				SkillType skillType = Skill::getSkillTypeFromString(typeStr);

				// Extract values safely using .value(key, default_fallback)
                    SkillData skillData{};
                    skillData.name = skillJson.value("name", "Unknown Skill");
                    skillData.description = skillJson.value("description", "");
                    skillData.mpCost = skillJson.value("mp_cost", 0);
                    skillData.type = skillType;
                    skillData.baseDamage = skillJson.value("base_damage", 0);
                    skillData.hits = skillJson.value("hits", std::vector<int>{});
                    skillData.animationName = skillJson.value("animation", "");
                    skillData.screenEffectName = skillJson.value("screen_effect", "");
                    skillData.hitTimes = skillJson.value("hit_times", std::vector<float>{});

            // Handle unique chain parameters if they exist in the JSON object


            unlockedSkills.push_back(std::make_unique<Skill>(skillData));
        }
        }
    }
    return unlockedSkills;
}


