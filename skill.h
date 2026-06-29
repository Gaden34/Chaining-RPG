#pragma once
#include <string>

enum class SkillType {
    Attack,
    Heal,
    Buff,
    Debuff
};

struct ChainData {
    float chainWindow = 0.f;      // Time window for next hit to chain
    float chainTimer = 0.f;       // Current chain timer
    int chainHitCount = 0;        // Current consecutive hits
    float damageMultiplier = 1.0f; // Current chain damage bonus
    bool canChain = true;         // Whether this skill participates in chains
};


class Skill
{
private:
	std::string name;
	std::string description;
	int mpCost;
	float cooldown;
	SkillType type;
	int baseDamage;

	ChainData* chainData; // Pointer to ChainData for this skill

public:
	Skill(const std::string& name, const std::string& description, int mpCost, float cooldown, SkillType type, int baseDamage, ChainData* chainData = nullptr);
	void initializeChaining(float chainWindow, float damagePerChain);
	void updateChainTimer(float dt);
	void onHit();
	float getDamage() const;
	bool canChainIntoNextSkill() const;
	const std::string& getName() const { return name; }
	int getMpCost() const { return mpCost; }
	SkillType getType() const { return type; }
};

