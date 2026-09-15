#pragma once
#include <random>
#include <SFML/Graphics.hpp>
#include "map.h"
#include "player.h"
#include "messageLog.h"
#include "menu.h"
#include "submenuScreen.h"


constexpr unsigned VirtualWidth = 640;
constexpr unsigned VirtualHeight = 360;
class InputHandler;

enum class ExplorationState {
	Exploring,
	FieldMenu
};

enum class FieldMenuState {
	Main,
	DisciplineScreen,
	StatusScreen,
	EquipmentScreen,
	ItemMenu,
	None
};

class FieldMenu : public Menu {
public:
	enum class Option {
		Status,
		Discipline,
		Equipment,
		Inventory,
		Exit,
		Count
	};

private:

	FieldMenuState currentState = FieldMenuState::Main;
	StatusScreen statusScreen;
	ItemMenu itemMenu;
	size_t selectedMemberIndex = 0;


public:
	FieldMenu();
	void draw(sf::RenderTarget& target, std::vector<Player>& party);
	void drawMain(sf::RenderTarget& target, bool disabled);
	void handleInput(InputHandler& inputHandler, std::vector<Player>& party);
	void handleMain(InputHandler& inputHandler, std::vector<Player>& party);
	void handleItemMenu(const InputHandler& inputHandler, std::vector<Player>& party);
	void handleStatusScreen(const InputHandler& inputHandler, std::vector<Player>& party);
	void selectState(std::vector<Player>& party);
	FieldMenu::Option getSelectedOption() const { return static_cast<FieldMenu::Option>(getSelectedIndex()); }
	FieldMenuState getCurrentState() const { return currentState; }
	void setCurrentState(FieldMenuState state) { currentState = state; }

};

class Exploration {
private:
	Map map;
	sf::View camera;
	float encounterTimer = 0.f;
	sf::Vector2f playerPosition = { 40.f, 20.f };
	std::mt19937& rng;
	std::vector<Player>& party;
	MessageLog& messageLog;
	FieldMenu fieldMenu;
	ItemMenu itemMenu;
	StatusScreen statusScreen;
	ExplorationState currentState = ExplorationState::Exploring;

	void setCamera(Player& player);

public:
	Exploration(std::mt19937& rng, std::vector<Player>& party, MessageLog& messageLog);

	void start(Player& player);
	void update(float dt, InputHandler& inputHandler);
	void draw(sf::RenderTarget& target);

	// Returns true if a random encounter was triggered this frame.
	void openFieldMenu(InputHandler& inputHandler);
	void handleFieldMenu(InputHandler& inputHandler);
	bool checkForEncounter(float dt, Player& player);

	MapTransition* getTransitionAtPosition(const sf::FloatRect& currentBounds, const sf::FloatRect& previousBounds, const std::optional<TransitionDirection>& horizontal, const std::optional<TransitionDirection>& vertical);

	void savePlayerPosition(const sf::Vector2f& pos) { playerPosition = pos; }
	Map& getMap() { return map; }
};
