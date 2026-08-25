#include "exploration.h"
#include <algorithm>
#include <iostream>

Exploration::Exploration(std::mt19937& rng, MessageLog& messageLog)
	: rng(rng), messageLog(messageLog) {
	camera = sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(VirtualWidth), static_cast<float>(VirtualHeight)));
}

void Exploration::start(Player& player) {
	map.setTexture("BurumiaMap");
	map.setCollisionMap("BurumiaCollisionMap");
	map.getTransitions() = Map::loadTransitionsFromJson("BurumiaMap", "maps.json");

	player.setPosition(playerPosition.x, playerPosition.y);
	player.getWalkAnimation().setFrame(1);
}

void Exploration::update(float dt, InputHandler& inputHandler, Player& player) {
	player.update(dt, inputHandler, map);
	setCamera(player);
}

void Exploration::draw(sf::RenderTarget& target, Player& player) {
	target.setView(camera);
	map.draw(target);
	player.drawExploring(target);
	target.setView(target.getDefaultView());
}

bool Exploration::checkForEncounter(float dt, Player& player) {
	std::uniform_int_distribution<int> rollEncounter(1, 100);
	if (player.getIsMoving()) encounterTimer += dt;

	if (encounterTimer >= 3.0f && player.getIsMoving()) {
		if (rollEncounter(rng) == 1) {
			playerPosition = player.getGlobalBounds().getPosition();
			encounterTimer = 0.f;
			return true;
		}
	}
	return false;
}

MapTransition* Exploration::getTransitionAtPosition(const sf::FloatRect& box, const std::optional<TransitionDirection>& horizontal, const std::optional<TransitionDirection>& vertical) {
	return map.getTransitionAtPosition(box, horizontal, vertical);
}

void Exploration::setCamera(Player& player) {
	sf::Vector2u mapSize = map.getSize();
	float halfWidth = camera.getSize().x / 2.f;
	float halfHeight = camera.getSize().y / 2.f;
	float cameraX = player.getGlobalBounds().getPosition().x + player.getGlobalBounds().width / 2.f;
	float cameraY = player.getGlobalBounds().getPosition().y + player.getGlobalBounds().height / 2.f;
	cameraX = std::clamp(cameraX, halfWidth, static_cast<float>(mapSize.x) - halfWidth);
	cameraY = std::clamp(cameraY, halfHeight, static_cast<float>(mapSize.y) - halfHeight);
	camera.setCenter(cameraX, cameraY);
}
