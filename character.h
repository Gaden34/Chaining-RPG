#pragma once
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "item.h"
#include "map.h"


class Character {
protected:
	sf::Sprite sprite;
	sf::Texture texture;

	std::string name;
	int hp;
	int mp;
	int maxHp;
	int maxMp;
	int attack;
	int magAttack;
	float moveSpeed;
	bool alive = true;

public:
	Character() = default;
	virtual ~Character() = default;
	Character(const Character&) = delete;
	Character& operator=(const Character&) = delete;
	Character(Character&&);
	Character& operator=(Character&&);
	virtual Inventory& getInventory() = 0;
	virtual void update(float dt) = 0;
	virtual void draw(sf::RenderTarget& target) = 0;
	virtual void move(float dt, const Map& map) = 0;

	std::string getName() const { return name; }
	void setPosition(float x, float y) { sprite.setPosition(x, y); }
	sf::FloatRect getGlobalBounds() const { return sprite.getGlobalBounds(); }
	int getHp() const { return hp; }
	void setHp(int x);
	int getMp() const { return mp; }
	void setMp(int x);
	int getMaxHp() const { return maxHp; }
	int getMaxMp() const { return maxMp; }
	int getAttack() const { return attack; }
	int getMagAttack() const { return magAttack; }
	float getMoveSpeed() const { return moveSpeed; }
	void takeDamage(int amount);
	bool isAlive() const { return alive; }


	
};