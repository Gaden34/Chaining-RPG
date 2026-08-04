#include <iostream>
#include "enemy.h"
#include "item.h"


Enemy::Enemy(const EnemyData& data) {
	name = data.name;

	maxHp = data.maxHp;
	hp = maxHp;

	maxMp = data.maxMp;
	mp = maxMp;

	attack = data.attack;
	magAttack = data.mAttack;

	expValue = data.expValue;

	moveSpeed = data.moveSpeed;

	texture.loadFromFile(data.texturePath);
	sprite.setTexture(texture);

	inventory = Inventory(data.startingItems);

	std::cout << name << " inventory:\n";

	for (const auto& slot : inventory.getItems())
	{
		std::cout << "  ID: " << static_cast<int>(slot.itemID)
			<< " Qty: " << slot.quantity << '\n';
	}
}

void Enemy::update(float dt) {

}

void Enemy::draw(sf::RenderTarget& target) {
	if (alive) target.draw(sprite);
}

void Enemy::move(float dt) {

}

int Enemy::getExpValue() {
	return expValue;
}