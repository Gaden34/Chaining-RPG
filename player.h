#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "character.h"
#include "levelSystem.h"
#include "discipline.h"
#include "disciplines.h"
#include "skill.h"

class Player : public Character {
private:
	LevelSystem levelSystem;
	int level = 1;
	int experience = 0;
	bool isMoving = false;
	DisciplineID discipline;
	std::vector<std::unique_ptr<Skill>> skills;

public:
	Player(LevelSystem& l, std::string textureName);
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;
	Player(Player&&) = default;
	void update(float dt) override;
	void draw(sf::RenderWindow& window) override;
	void move(float dt) override;
	int getExp();
	void addExp(int amount);
	int getLevel();
	void incrementLevel();
	bool getIsMoving();
	Discipline& getDiscipline();
	void setDiscipline(DisciplineID id);
	void addCharacter(char c);
	void removeLastCharacter();
	void learnSkill(std::unique_ptr<Skill> skill);
	const std::vector<std::unique_ptr<Skill>>& getSkills() const;
	Skill* getSkillByIndex(int index);
	void unlockLevelSkills();
};