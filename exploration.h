#pragma once
#include <random>
#include <SFML/Graphics.hpp>
#include "map.h"
#include "player.h"
#include "messageLog.h"

constexpr unsigned VirtualWidth = 640;
constexpr unsigned VirtualHeight = 360;

class Exploration {
private:
	Map map;
	sf::View camera;
	float encounterTimer = 0.f;
	sf::Vector2f playerPosition = { 40.f, 20.f };
	std::mt19937& rng;
	MessageLog& messageLog;

	void setCamera(Player& player);

public:
	Exploration(std::mt19937& rng, MessageLog& messageLog);

	void start(Player& player);
	void update(float dt, InputHandler& inputHandler, Player& player);
	void draw(sf::RenderTarget& target, Player& player);

	// Returns true if a random encounter was triggered this frame.
	bool checkForEncounter(float dt, Player& player);

	MapTransition* getTransitionAtPosition(const sf::FloatRect& currentBounds, const sf::FloatRect& previousBounds, const std::optional<TransitionDirection>& horizontal, const std::optional<TransitionDirection>& vertical);

	void savePlayerPosition(const sf::Vector2f& pos) { playerPosition = pos; }
	Map& getMap() { return map; }
};
