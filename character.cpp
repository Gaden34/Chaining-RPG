#include "character.h"


std::string Character::getName() const {
	return name;
}

void Character::setPosition(float x, float y) {
	sprite.setPosition(x, y);
}

int Character::getHp() const {
	return hp;
}

void Character::setHp(int x) {
	hp = x;
}

int Character::getMp() const {
	return mp;
}

int Character::getMaxHp() const {
	return maxHp;
}

int Character::getMaxMp() const {
	return maxMp;
}

int Character::getAttack() const {
	return attack;
}

int Character::getMagAttack() const {
	return magAttack;
}

float Character::getMoveSpeed() const {
	return moveSpeed;
}

void Character::takeDamage(int amount) {
	hp -= amount;

	if (hp <= 0) alive = false;
}

bool Character::isAlive() const {
	return alive;
}
