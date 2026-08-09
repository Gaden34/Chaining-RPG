#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>
#include "animation.h"
#include "character.h"
#include "discipline.h"
#include "disciplines.h"
#include "skill.h"
#include "messageLog.h"
#include "item.h"
#include "inputHandler.h"
#include "map.h"

class Player : public Character {
private:
	int level = 1;
	int experience = 0;
	bool isMoving = false;
	MessageLog& messageLog;
	DisciplineID discipline;
	std::vector<std::unique_ptr<Skill>> skills;
	Animation walkAnimation;
	sf::Texture walkTexture;
	Inventory& partyInventory;

public:
	Player(MessageLog& m, std::string n, std::string textureName, Inventory& inv);
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;
	Player(Player&&) = default;
	void update(float dt) override {}
	void update(float dt, const Map& map);
	void draw(sf::RenderTarget& target) override { drawExploring(target); }
	void drawExploring(sf::RenderTarget& target);
	void drawCombat(sf::RenderTarget& target);
	void move(float dt, const Map& map) override;
	void move(float dt, InputHandler& inputHandler);
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
	Animation& getWalkAnimation() { return walkAnimation; }
	sf::FloatRect getCollisionBox(sf::Vector2f position) const;
};