#pragma once
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>


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
	virtual ~Character() = default;
	virtual void update(float dt) = 0;
	virtual void draw(sf::RenderWindow& window) = 0;
	virtual void move(float dt) = 0;

	std::string getName() const;
	void setPosition(float x, float y);
	int getHp() const;
	void setHp(int x);
	int getMp() const;
	int getMaxHp() const;
	int getMaxMp() const;
	int getAttack() const;
	int getMagAttack() const;
	float getMoveSpeed() const;
	void takeDamage(int amount);
	bool isAlive() const;

	
};