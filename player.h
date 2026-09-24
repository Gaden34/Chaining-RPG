#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include "animation.h"
#include "character.h"
#include "discipline.h"
#include "disciplines.h"
#include "skill.h"
#include "messageLog.h"
#include "item.h"
#include "inputHandler.h"
#include "map.h"

struct AxisInput {
	InputAction negativeAction;
	TransitionDirection negativeDirection;
	InputAction positiveAction;
	TransitionDirection positiveDirection;
};

struct SpritePaths {
	std::string overworldTexturePath;
	std::string portraitTexturePath;
	std::string battleTexturePath;
};

class Player : public Character {
private:
	int level = 1;
	int experience = 0;
	bool isMoving = false;
	MessageLog& messageLog;
	DisciplineID discipline;
	std::vector<DisciplineID> unlockedDisciplines = { DisciplineID::Mage, DisciplineID::Thief, DisciplineID::Combatant };
	std::vector<std::unique_ptr<Skill>> skills;
	sf::Sprite portraitSprite;
	sf::Texture portraitTexture;
	sf::Sprite battleSprite;
	sf::Texture battleTexture;
	Animation walkAnimation;
	sf::Texture walkTexture;
	Inventory& partyInventory;
	std::optional<TransitionDirection> horizontalDirection;
	std::optional<TransitionDirection> verticalDirection;
	sf::FloatRect previousCollisionBox;

public:
	Player(MessageLog& m, std::string n, const SpritePaths& spritePaths, Inventory& inv);
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;
	Player(Player&&) = default;
	void update(float dt) override {}
	void update(float dt, InputHandler& inputHandler, const Map& map);
	void draw(sf::RenderTarget& target) override { drawExploring(target); }
	void drawExploring(sf::RenderTarget& target);
	void drawCombat(sf::RenderTarget& target);
	void move(float dt, const Map& map) override {}
	void move(float dt, InputHandler& inputHandler, const Map& map);
	void resolveAxis(std::optional<TransitionDirection>& axis, InputHandler& input, const AxisInput& axisInput);
	std::optional<TransitionDirection> getHorizontalDirection() const { return horizontalDirection; }
	std::optional<TransitionDirection> getVerticalDirection() const { return verticalDirection; }
	int getExp();
	void addExp(int amount);
	int expNeededForNextLevel(int level);
	int getLevel();
	void levelUp();
	void setLevel(int newLevel) { level = newLevel; }
	void setLevelFromTest(int targetLevel);
	bool getIsMoving();
	Discipline& getDiscipline();
	void setDiscipline(DisciplineID id);
	void addCharacter(char c);
	void removeLastCharacter();
	void learnSkill(std::unique_ptr<Skill> skill);
	const std::vector<std::unique_ptr<Skill>>& getSkills() const;
	Skill* getSkillByIndex(int index);
	void unlockLevelSkills();
	bool hasSkill(const std::string& skillName) const;
	Inventory& getInventory() override { return partyInventory; }
	sf::Sprite& getPortraitSprite() { return portraitSprite; }
	sf::Sprite& getBattleSprite() { return battleSprite; }
	Animation& getWalkAnimation() { return walkAnimation; }
	sf::FloatRect getCollisionBox(sf::Vector2f position) const;
	sf::FloatRect getPreviousCollisionBox() const { return previousCollisionBox; }
	std::vector<DisciplineID>& getUnlockedDisciplines() { return unlockedDisciplines; }
	void unlockDiscipline(DisciplineID id);
};