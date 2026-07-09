#pragma once
#include <string>
#include <vector>
//#include "skill.h"

enum class DisciplineID {
	Mage,
	Thief,
	Combatant
};

struct StatGrowth {
	int healthGrowth;
	int mpGrowth;
	int attackGrowth;
	int magAttackGrowth;
};

class Discipline
{
private:
	std::string name;
	int baseHealth;
	int baseMp;
	int baseAttack;
	int baseMagAttack;

	StatGrowth statGrowth;


	//std::vector<Skill> startingSkills;

public: 
	Discipline(std::string n, int health, int mp, int attack, int magAttack);
	std::string getName();
	int getBaseHealth() { return baseHealth; }
	int getBaseMp() { return baseMp; }
	int getBaseAttack() { return baseAttack; }
	int getBaseMagAttack() { return baseMagAttack; }
	StatGrowth getStatGrowth() { return statGrowth; }
};

