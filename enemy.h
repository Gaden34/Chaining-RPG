#pragma once
#include "character.h"
#include "enemyData.h"
#include "item.h"

class Enemy : public Character {
private:
	int expValue;
	Inventory inventory;

public: 
	Enemy(const EnemyData& data);
	void update(float dt) override;
	void draw(sf::RenderTarget& target) override;
	void move(float dt, const Map& map) override;
	int getExpValue();
	Inventory& getInventory() override { return inventory; }
};