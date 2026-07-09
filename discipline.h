#pragma once
#include <string>
#include <vector>
//#include "skill.h"

enum class DisciplineID {
	Mage,
	Thief,
	Combatant
};


class Discipline
{
private:
	std::string name;
	int baseHealth;
	int baseAttack;
	int baseMagAttack;

	//std::vector<Skill> startingSkills;

public: 
	Discipline(std::string n, int health, int attack, int magAttack);
	std::string getName();
	int getBaseHealth() { return baseHealth; }
	int getBaseAttack() { return baseAttack; }
	int getBaseMagAttack() { return baseMagAttack; }
};

