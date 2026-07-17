#pragma once
#include <string>
#include <vector>

enum class SkillType {
    Attack,
	Magic,
    Heal,
    Buff,
    Debuff,
	Steal
};

enum class StealResult {
	Success,
	Failure,
	NoItems
};


class Skill
{
private:
	std::string name;
	std::string description;
	int mpCost;
	SkillType type;
	int baseDamage;
	std::vector<int> hits;

	ChainData* chainData; // Pointer to ChainData for this skill

public:
	Skill(const std::string& name, const std::string& description, int mpCost, SkillType type, int baseDamage, const std::vector<int>& hits = {}, ChainData* chainData = nullptr);
	std::vector<int> getHits() const;
	const std::string& getName() const { return name; }
	int getMpCost() const { return mpCost; }
	SkillType getType() const { return type; }
	static SkillType getSkillTypeFromString(const std::string& type);
	StealResult useSteal(Character& user, Character& target);
};

