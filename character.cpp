#include "character.h"

Character::Character(Character&& other)
	: texture(std::move(other.texture)),
	  name(std::move(other.name)),
	  hp(other.hp),
	  mp(other.mp),
	  maxHp(other.maxHp),
	  maxMp(other.maxMp),
	  attack(other.attack),
	  magAttack(other.magAttack),
	  moveSpeed(other.moveSpeed),
	  alive(other.alive) {
	sprite.setTexture(texture);
	sprite.setPosition(other.sprite.getPosition());
}

Character& Character::operator=(Character&& other) {
	if (this != &other) {
		texture = std::move(other.texture);
		name = std::move(other.name);
		hp = other.hp;
		mp = other.mp;
		maxHp = other.maxHp;
		maxMp = other.maxMp;
		attack = other.attack;
		magAttack = other.magAttack;
		moveSpeed = other.moveSpeed;
		alive = other.alive;
		sprite.setTexture(texture);
		sprite.setPosition(other.sprite.getPosition());
	}
	return *this;
}

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

void Character::setMp(int x) {
	mp = x;
	if (mp < 0) mp = 0;
	if (mp > maxMp) mp = maxMp;
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
