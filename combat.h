#pragma once
#include <vector>
#include <random>
#include <string>
#include <initializer_list>
#include <SFML/Graphics.hpp>
#include "player.h"
#include "enemy.h"
#include "enemyData.h"
#include "messageLog.h"
#include "menu.h"
#include "chainSystem.h"

class Inventory;

enum class CombatState {
	PlayerTurn,
	SelectingEnemy,
	ChoosingQueuedActions,
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
	ItemData* item = nullptr;
};

struct ActiveAnimation {
	Character* character = nullptr;
	sf::Vector2f startPosition;
	sf::Vector2f targetPosition;
	float duration = 0.f;
	float elapsedTime = 0.f;
	bool isActive = false;
};

struct EnemySpawn {
	const EnemyData* data = nullptr;
	int count = 1;
};

class CombatMenu : public Menu {
public:
	enum class MenuOption { Attack, Skill, Item, Defend, Count };

private:
	std::vector<MenuOption> availableOptions;
	const float menuX = 320.f;
	const float menuY = 300.f;
	const float optionSpacing = 12.f;

public:
	CombatMenu();
	void draw(sf::RenderTarget& target);
	MenuOption getSelectedOption();

};

class SkillMenu : public Menu {
private:
	int skillCount = 0;
	const float menuX = 352.f;
	const float menuY = 300.f;
	const float optionSpacing = 12.f;

public:
	SkillMenu();
	void populate(const std::vector<std::unique_ptr<Skill>>& skills);
	void handleInput(sf::Keyboard::Key key);
	void draw(sf::RenderTarget& target);
	int getSelectedIndex() const;
};

class ItemMenu : public Menu {
private:
	const float menuX = 352.f;
	const float menuY = 300.f;
	const float optionSpacing = 12.f;

public:
	ItemMenu();
	void populate(const Inventory& inventory);
	void draw(sf::RenderTarget& target);
};

class QueuedActionMenu : public Menu {
private:
	const float menuX = 320.f;
	const float menuY = 300.f;
	const float optionSpacing = 12.f;

public:
	QueuedActionMenu();
	void populate(const std::vector<QueuedAction>& actions);
	void draw(sf::RenderTarget& target);

};

class Combat {
private:
	sf::Texture backgroundTexture;
	sf::Sprite background;
	sf::Texture pointerTexture;
	sf::Sprite pointerSprite;
	CombatState currentState;
	std::vector<Player>& party;
	std::vector<Enemy> enemies;
	std::vector<ActiveAnimation> activeAnimations;
	std::mt19937& rng;
	CombatMenu menu;
	SkillMenu skillMenu;
	ItemMenu itemMenu;
	QueuedActionMenu queuedActionMenu;
	bool inSkillMenu = false;
	bool inItemMenu = false;
	ChainSystem chain;
	MessageLog& messageLog;
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
	bool lastEnterPressed = true;
	sf::Font font;

public:

	Combat(std::vector<Player>& p, MessageLog& m, std::mt19937& rng);
	void update(float dt);
	void draw(sf::RenderTarget& target);
	void start();
	//void start(const EnemyData& data) { start({ EnemySpawn{ &data, 1 } }); }
	CombatState getState();

private:
	void handlePlayerTurn();
	void targetEnemy();
	void drawTargetPointer(sf::RenderTarget& target);
	int randomRange(int min, int max);
	void performAttack(QueuedAction& action);
	void performSkill(QueuedAction& action);
	void performItem(QueuedAction& action);
	void calculateSkillDamage(Skill* skill, Character* actor, Character* target);
	void handleSteal (Skill* skill, Character* actor, Character* target);
	void executeAction(QueuedAction& action);
	void executeNextAction();
	void handleQueuedActionMenu();
	void handleEnemyTurn();
	void updateAnimations(float dt);
	void updatePlayerAnimation(float dt);
	void updateEnemyAnimation(float dt);
	std::vector<EnemySpawn> makeRandomEncounter();
	void handleDeath(Character& character);
	void eraseDeadEnemies();
	void checkCombatEnd();
	void advanceActivePlayer();
	void resetEnemyIndex();
};
