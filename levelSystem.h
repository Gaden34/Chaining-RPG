#pragma once


class Player;

class LevelSystem
{
private:

	
	void levelUp(Player& player);

public:
	void gainExp(Player& player, int amount);
	int expNeededForLevel(int level);
};

