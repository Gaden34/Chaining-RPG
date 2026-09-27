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
	defense(other.defense),
	magDefense(other.magDefense),
	moveSpeed(other.moveSpeed),
	alive(other.alive),
	statsOutdated(other.statsOutdated),
	instanceId(other.instanceId)
{
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
		defense = other.defense;
		magDefense = other.magDefense;
		moveSpeed = other.moveSpeed;
		alive = other.alive;
		statsOutdated = other.statsOutdated;
		instanceId = other.instanceId;
		//inventory = std::move(other.inventory);
		sprite.setTexture(texture);
		sprite.setPosition(other.sprite.getPosition());
	}
	return *this;
}

void Character::setHp(int x) {
	hp = x;
	if (hp < 0) hp = 0;
	if (hp > maxHp) hp = maxHp;
}

void Character::setMp(int x) {
	mp = x;
	if (mp < 0) mp = 0;
	if (mp > maxMp) mp = maxMp;
}

void Character::takeDamage(int amount) {
	hp -= amount;

	if (hp <= 0) alive = false;
}
