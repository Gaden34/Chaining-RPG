#pragma once
#include <random>
#include <vector>
#include <SFML/Graphics.hpp>
#include "character.h"
#include "player.h"
#include "enemy.h"
#include "enemyData.h"
#include "map.h"
#include "combat.h"
#include "messageLog.h"
#include "levelSystem.h"
#include "enums.h"
#include "discipline.h"
#include "disciplines.h"

enum class GameState {
	CharacterCreation,
	Exploring,
	Combat,
	Menu,
	GameOver
};

class Game {
private:
	Player player;
	Combat combat;
	Map map;
	MessageLog messageLog;
	LevelSystem levelSystem;
	sf::RenderWindow window;
	sf::Clock clock;
	GameState currentState = GameState::CharacterCreation;
	CreationStep creationStep = CreationStep::Name;
	float encounterTimer = 0.f;
	std::mt19937 rng;

public: 
	Game();
	void draw();
	void drawCharacterCreation();
	void drawExploring();
	void drawCombat();
	void handleEvents();
	void handleTextInput(sf::Event event);
	void handleCreationInput(sf::Event event);
	void update(float dt);
	void updateCombat(float dt);
	void updateCharacterCreation(float dt);
	void startCharacterCreation();
	void startClassSelection();
	void handleNameSelection();
	void handleClassSelection(sf::Event event);
	void startExploring();
	void checkForEncounter(float dt);
	void startCombat();
	void run();

};