#pragma once
#include <vector>
#include <map>
#include <unordered_map>
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
#include "inputHandler.h"
#include "animation.h"

class Inventory;

enum class CombatState {
	PlayerTurn,
	SkillMenu,
	ItemMenu,
	SelectingTarget,
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

struct PendingSkillAction {
	QueuedAction action;
	int effectID = -1;
};

struct ActiveAnimation {
	Character* character = nullptr;
	sf::Vector2f startPosition;
	sf::Vector2f targetPosition;
	float duration = 0.f;
	float elapsedTime = 0.f;
	bool isActive = false;
};

// A one-off visual effect (e.g. a spell dropping onto its target) played over the target's sprite.
struct CombatVisualEffect {
	int effectID = -1;
	Animation animation;
	sf::Sprite sprite;
	sf::Vector2f startPosition;
	sf::Vector2f targetPosition;
	float dropDuration = 0.5f;
	float elapsedTime = 0.f;
	float delay = 0.f; // holds the effect back so multi-hit skills stagger their drops
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

public:
	CombatMenu();
	void draw(sf::RenderTarget& target);
	MenuOption getSelectedOption();

};

class SkillMenu : public Menu {
private:
	int skillCount = 0;

public:
	SkillMenu();
	void populate(const std::vector<std::unique_ptr<Skill>>& skills);
	void draw(sf::RenderTarget& target);
	int getSelectedIndex() const;
};

class QueuedActionMenu : public Menu {
private:


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
	std::vector<PendingSkillAction> pendingSkillActions;
	std::unordered_map<std::string, sf::Texture> effectTextures;
	std::unordered_map<std::string, AnimationAsset> effectAssets;
	std::vector<CombatVisualEffect> skillEffects;
	int nextEffectID = 0;
	std::mt19937& rng;
	CombatMenu menu;
	SkillMenu skillMenu;
	ItemMenu itemMenu;
	QueuedActionMenu queuedActionMenu;
	ChainSystem chain;
	MessageLog& messageLog;
	QueuedAction currentAction;
	std::vector<QueuedAction> actionQueue;
	std::map<ItemID, int> queuedConsumableCounts;
	std::vector<Character*> validTargets;
	//Item* pendingItem = nullptr;

	float animationTimer = 0.f;
	int activePlayerIndex = 0;
	int activeEnemyIndex = 0;
	int validTargetIndex = 0;
	std::vector<bool> playerActed;
	std::vector<bool> enemyActed;
	sf::Font font;

public:

	Combat(std::vector<Player>& p, MessageLog& m, std::mt19937& rng);
	void update(float dt, const InputHandler& input);
	void draw(sf::RenderTarget& target);
	void start();
	//void start(const EnemyData& data) { start({ EnemySpawn{ &data, 1 } }); }
	CombatState getState();

private:
	void handlePlayerTurn(const InputHandler& input);
	void handleSkillMenu(const InputHandler& input);
	void handleItemMenu(const InputHandler& input);
	void buildValidTargets();
	void beginTargeting();
	void targetCharacter(const InputHandler& input);
	void drawTargetPointer(sf::RenderTarget& target);
	void drawActivePlayerName(sf::RenderTarget& target);
	int randomRange(int min, int max);
	void performAttack(QueuedAction& action);
	void performSkill(QueuedAction& action);
	void performItem(QueuedAction& action);
	void calculateSkillDamage(Skill* skill, Character* actor, Character* target);
	void handleSteal (Skill* skill, Character* actor, Character* target);
	void executeAction(QueuedAction& action);
	void handleQueuedActionMenu(const InputHandler& input);
	void handleEnemyTurn();
	void updateAnimations(float dt);
	void updatePlayerAnimation(float dt);
	void updateEnemyAnimation(float dt);
	bool loadEffectAnimation(const std::string& animationName);
	int triggerSkillEffect(const std::string& animationName, Character& target, int instanceCount = 1);
	bool isEffectFinished(int effectID) const;
	void updateSkillEffect(float dt);
	void drawSkillEffect(sf::RenderTarget& target);
	std::vector<EnemySpawn> makeRandomEncounter();
	void handleDeath(Character& character);
	void eraseDeadEnemies();
	void checkCombatEnd();
	void advanceActivePlayer();
	void resetEnemyIndex();
	int getQueuedConsumableCount(ItemID itemID) const;
	bool canQueueConsumableItem(const ItemData& item, const Inventory& inventory) const;
	void reserveConsumableItem(const ItemData& item);
	void releaseConsumableItemReservation(const ItemData& item);
};
