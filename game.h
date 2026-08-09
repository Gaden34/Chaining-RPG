#pragma once
#include <random>
#include <vector>
#include <SFML/Graphics.hpp>
#include "character.h"
#include "player.h"
#include "enemy.h"
#include "enemyData.h"
#include "map.h"
#include "menu.h"
#include "combat.h"
#include "messageLog.h"
#include "enums.h"
#include "discipline.h"
#include "disciplines.h"
#include "combatTestSetup.h"
#include "item.h"
#include "inputHandler.h"

enum class GameState {
	StartMenu,
	CharacterCreation,
	Exploring,
	Combat,
	CombatTest,
	Menu,
	GameOver
};

class StartMenu : public Menu {
private:
	sf::Texture backgroundTexture;
	sf::Sprite background;

public:
	StartMenu();
	void draw(sf::RenderTarget& target);

};


class Game {
private:
	std::vector<Player> party;
	Combat combat;
	Map map;
	MessageLog messageLog;
	StartMenu startMenu;
	CombatTestSetup combatTestSetup;
	sf::RenderWindow window;
	sf::RenderTexture gameTexture;
	sf::Clock clock;
	GameState currentState = GameState::CharacterCreation;
	CreationStep creationStep = CreationStep::Name;
	float encounterTimer = 0.f;
	std::mt19937 rng;
	Inventory partyInventory;
	InputHandler inputHandler;
	sf::View camera;
	sf::Vector2f playerExploringPosition = {40.f, 20.f};

public: 
	Game();
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	void draw();
	void drawStartMenu();
	void drawCharacterCreation();
	void drawExploring();
	void drawCombat();
	void handleStartMenu();
	void handleEvents();
	void handleTextInput(sf::Event event);
	void handleCreationInput(sf::Event event);
	void update(float dt);
	void updateStartMenu(float dt);
	void updateCombat(float dt);
	void updateCharacterCreation(float dt);
	void startCharacterCreation();
	void startClassSelection();
	void startCombatTest();
	void handleNameSelection();
	void handleClassSelection(sf::Event event);
	void startExploring();
	void checkForEncounter(float dt);
	void startCombat();
	void run();
	void setCamera();

};

constexpr unsigned VirtualWidth = 640;
constexpr unsigned VirtualHeight = 360;