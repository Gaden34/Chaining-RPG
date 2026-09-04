#pragma once
#include <random>
#include <SFML/Graphics.hpp>
#include "map.h"
#include "player.h"
#include "messageLog.h"
#include "menu.h"
#include "statusScreen.h"


constexpr unsigned VirtualWidth = 640;
constexpr unsigned VirtualHeight = 360;
class InputHandler;


enum class FieldMenuState {
	Main,
	DisciplineScreen,
	StatusScreen,
	EquipmentScreen,
	ItemScreen,
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
	const float menuX = 200.f;
	const float menuY = 100.f;
	const float optionsSpacing = 12.f;
	FieldMenuState currentState = FieldMenuState::Main;
	StatusScreen statusScreen;
	size_t selectedMemberIndex = 0;


public:
	FieldMenu();
	void draw(sf::RenderTarget& target, std::vector<Player>& party);
	void handleInput(InputHandler& inputHandler, size_t partySize);
	void onSelect();
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
	MessageLog& messageLog;
	FieldMenu fieldMenu;
	ItemMenu itemMenu;
	StatusScreen statusScreen;
	bool inFieldMenu = false;
	bool inItemMenu = false;

	void setCamera(Player& player);

public:
	Exploration(std::mt19937& rng, MessageLog& messageLog);

	void start(Player& player);
	void update(float dt, InputHandler& inputHandler, std::vector<Player>& party);
	void draw(sf::RenderTarget& target, std::vector<Player>& party);

	// Returns true if a random encounter was triggered this frame.
	void openFieldMenu(InputHandler& inputHandler);
	bool checkForEncounter(float dt, Player& player);

	MapTransition* getTransitionAtPosition(const sf::FloatRect& currentBounds, const sf::FloatRect& previousBounds, const std::optional<TransitionDirection>& horizontal, const std::optional<TransitionDirection>& vertical);

	void savePlayerPosition(const sf::Vector2f& pos) { playerPosition = pos; }
	Map& getMap() { return map; }
};
