#pragma once
#include <vector>
#include <random>
#include <string>
#include <SFML/Graphics.hpp>
#include "player.h"
#include "enemy.h"
#include "enemyData.h"
#include "messageLog.h"
#include "levelSystem.h"

enum class CombatState {
	PlayerTurn,
	PlayerAnimation,
	EnemyTurn,
	EnemyAnimation,
	Victory,
	Defeat
};

class CombatMenu {
public:
	enum class MenuOption { Attack, Skill, Item, Defend, Count };

private:
	sf::Font font;
	std::vector<sf::Text> optionTexts;
	int selectedIndex = 0;
	std::vector<MenuOption> availableOptions;
	const float menuX = 400.f;
	const float menuY = 500.f;
	const float optionSpacing = 20.f;
	bool lastUpPressed = false;
	bool lastDownPressed = false;
	bool lastEnterPressed = false;

public:
	CombatMenu();
	void handleInput(sf::Keyboard::Key key);
	void draw(sf::RenderWindow& window);
	MenuOption getSelectedOption();
	void reset();
	bool getLastUpPressed() const { return lastUpPressed; }
	bool getLastDownPressed() const { return lastDownPressed; }
	bool getLastEnterPressed() const { return lastEnterPressed; }
	void setLastUpPressed(bool pressed) { lastUpPressed = pressed; }
	void setLastDownPressed(bool pressed) { lastDownPressed = pressed; }
	void setLastEnterPressed(bool pressed) { lastEnterPressed = pressed; }

};

class Combat {
private:
	sf::Texture backgroundTexture;
	sf::Sprite background;
	CombatState currentState;
	Player& player;
	std::vector<Enemy> enemies;
	CombatMenu menu;
	MessageLog& messageLog;
	LevelSystem& levelSystem;

	float animationTimer = 0.f;

public:

	Combat(Player& p, MessageLog& m, LevelSystem& l);
	void update(float dt);
	void draw(sf::RenderWindow& window);
	void start(EnemyData& data);
	CombatState getState();

private:
	void handlePlayerTurn();
	void playerAttack();
	void handleEnemyTurn();
	void updatePlayerAnimation(float dt);
	void updateEnemyAnimation(float dt);
	void checkEnemyDeath(Enemy& enemy);
	void checkCombatEnd();
};
