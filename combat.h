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
	SelectingEnemy,
	ExecutingActions,
	PlayerAnimation,
	EnemyTurn,
	EnemyAnimation,
	Victory,
	Defeat
};

enum class ActionType {
	None,
	Attack,
	Skill,
	Item,
	Defend
};

struct QueuedAction {
	ActionType type = ActionType::None;
	
	Character* actor = nullptr;
	Character* target = nullptr;
	Skill* skill = nullptr;
	//Item* item;
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

class SkillMenu {
private:
	sf::Font font;
	std::vector<sf::Text> optionTexts;
	int selectedIndex = 0;
	int skillCount = 0;
	const float menuX = 440.f;
	const float menuY = 500.f;
	const float optionSpacing = 20.f;
	bool lastUpPressed = false;
	bool lastDownPressed = false;
	bool lastEnterPressed = false;
	bool lastEscapePressed = false;

public:
	SkillMenu();
	void populate(const std::vector<std::unique_ptr<Skill>>& skills);
	void handleInput(sf::Keyboard::Key key);
	void draw(sf::RenderWindow& window);
	int getSelectedIndex() const;
	void reset();
	bool getLastUpPressed() const { return lastUpPressed; }
	bool getLastDownPressed() const { return lastDownPressed; }
	bool getLastEnterPressed() const { return lastEnterPressed; }
	bool getLastEscapePressed() const { return lastEscapePressed; }
	void setLastUpPressed(bool pressed) { lastUpPressed = pressed; }
	void setLastDownPressed(bool pressed) { lastDownPressed = pressed; }
	void setLastEnterPressed(bool pressed) { lastEnterPressed = pressed; }
	void setLastEscapePressed(bool pressed) { lastEscapePressed = pressed; }
};

class Combat {
private:
	sf::Texture backgroundTexture;
	sf::Sprite background;
	CombatState currentState;
	std::vector<Player>& party;
	std::vector<Enemy> enemies;
	CombatMenu menu;
	SkillMenu skillMenu;
	bool inSkillMenu = false;
	MessageLog& messageLog;
	LevelSystem& levelSystem;
	QueuedAction currentAction;
	std::vector<QueuedAction> actionQueue;
	//Item* pendingItem = nullptr;

	float animationTimer = 0.f;
	int activePlayerIndex = 0;
	int activeEnemyIndex = 0;
	std::vector<bool> playerActed;
	std::vector<bool> enemyActed;
	bool lastLeftPressed = false;
	bool lastRightPressed = false;
	bool lastUpPressed = false;
	bool lastDownPressed = false;
	bool lastEnterPressed = false;
	sf::Font font;

public:

	Combat(std::vector<Player>& p, MessageLog& m, LevelSystem& l);
	void update(float dt);
	void draw(sf::RenderWindow& window);
	void start(EnemyData& data);
	CombatState getState();

private:
	void handlePlayerTurn();
	void targetEnemy();
	void performAttack(QueuedAction& action);
	void performSkill(QueuedAction& action);
	void executeAction(QueuedAction& action);
	void executeNextAction();
	void playerAttack(Player& player, Enemy& enemy);
	void playerUseSkill(Skill* skill);
	void handleEnemyTurn();
	void updatePlayerAnimation(float dt);
	void updateEnemyAnimation(float dt);
	void checkEnemyDeath(Enemy& enemy);
	void checkCombatEnd();
	void advanceActivePlayer();
};
