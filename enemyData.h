#pragma once
#include <string>

struct EnemyData {
	std::string name;

	int maxHp;
	int maxMp;

	int attack;
	int mAttack;

	int expValue;

	float moveSpeed;

	std::string texturePath;
};

extern EnemyData knight;