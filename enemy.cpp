#include "enemy.h"



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
}

void Enemy::update(float dt) {

}

void Enemy::draw(sf::RenderTarget& target) {
	target.draw(sprite);
}

void Enemy::move(float dt) {

}

int Enemy::getExpValue() {
	return expValue;
}