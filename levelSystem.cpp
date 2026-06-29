#include <iostream>
#include "Player.h"
#include "levelSystem.h"



void LevelSystem::gainExp(Player& player, int amount) {
	player.addExp(amount);

	while (player.getExp() >= expNeededForLevel(player.getLevel())) {
		levelUp(player);
	}
}

int LevelSystem::expNeededForLevel(int level) {
	return (level - 1) * 27 + 50;
}

void LevelSystem::levelUp(Player& player) {
	player.incrementLevel();

	//player.increaseMaxHp(10)
	//player.increaseAttack(2);
}