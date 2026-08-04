#pragma once
#include <string>
#include "item.h"

struct EnemyData {
	std::string name;

	int maxHp;
	int maxMp;

	int attack;
	int mAttack;

	int expValue;

	float moveSpeed;

	std::string texturePath;

	std::vector<InventorySlot> startingItems;
};

extern EnemyData knight;
extern EnemyData bat;