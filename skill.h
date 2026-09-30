#pragma once
#include <string>
#include <vector>
#include <random>
#include "item.h"

class Character;

enum class SkillType {
    Attack,
	Magic,
    Heal,
    Buff,
    Debuff,
	Steal
};

struct StealResult {
	enum class Result {
		Success,
		Failed,
		NoItems
	};

	Result result = Result::Failed;
	ItemID stolenItemID = ItemID::Invalid;
};

struct SkillData {
	std::string name;
	std::string description;
	int mpCost;
	SkillType type;
	int baseDamage;
	std::vector<int> hits;
	std::vector<float> hitTimes;
	std::string animationName;
	std::string screenEffectName;
};


class Skill
{
private:
	std::string name;
	std::string description;
	std::string screenEffectName;
	const std::vector<float> hitTimes;
	int mpCost;
	SkillType type;
	int baseDamage;
	std::vector<int> hits;
	std::string animationName;

public:
	Skill(const SkillData& data);
	std::vector<int> getHits() const;
	const std::string& getName() const { return name; }
	const std::string& getDescription() const { return description; }
	int getDamage() const { return baseDamage; }
	int getMpCost() const { return mpCost; }
	SkillType getType() const { return type; }
	const std::string& getAnimationName() const { return animationName; }
	static SkillType getSkillTypeFromString(const std::string& type);
	const std::string& getScreenEffectName() const { return screenEffectName; }
	const std::vector<float>& getHitTimes() const { return hitTimes; }
	StealResult useSteal(Character& user, Character& target, std::mt19937& rng);
};

