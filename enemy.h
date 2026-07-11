#pragma once
#include "character.h"
#include "enemyData.h"

class Enemy : public Character {
private:
	int expValue;

public: 
	Enemy(const EnemyData& data);
	void update(float dt) override;
	void draw(sf::RenderTarget& target) override;
	void move(float dt) override;
	int getExpValue();
};