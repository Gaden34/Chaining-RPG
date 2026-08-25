#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "map.h"

using json = nlohmann::json;


Map::Map() {
	texture.loadFromFile("assets/characterCreation.png");
	sprite.setTexture(texture, true);
}

void Map::draw(sf::RenderTarget& target) {
	target.draw(sprite);
}

void Map::setTexture(std::string t) {
	texture.loadFromFile("assets/" + t + ".png");
	sprite.setTexture(texture, true);
}

void Map::setCollisionMap(std::string t) {
	std::string path = "assets/" + t + ".png";

	if (!collisionMap.loadFromFile(path))
		std::cerr << "Failed to load collision map: " << path << std::endl;
}

bool Map::isBlocked(int x, int y) const {
	if (x < 0 || y < 0 || x >= static_cast<int>(collisionMap.getSize().x) || y >= static_cast<int>(collisionMap.getSize().y)) {
		return true;
	}
	
	return collisionMap.getPixel(x, y) == sf::Color::Red;

	std::cout << "Collision map loaded: " << collisionMap.getSize().x << "x" << collisionMap.getSize().y << std::endl;
}

MapTransition* Map::getTransitionAtPosition(const sf::FloatRect& bounds, const std::optional<TransitionDirection>& horizontal, const std::optional<TransitionDirection>& vertical) {
	for (auto& transition : transitions) {
		if (horizontal && canTriggerTransition(bounds, transition, *horizontal)) {
			return &transition;
		}
		if (vertical && canTriggerTransition(bounds, transition, *vertical)) {
			return &transition;
		}
	}
	return nullptr;
}

bool Map::canTriggerTransition(const sf::FloatRect& feet, const MapTransition& transition, TransitionDirection direction) const {
	if (!feet.intersects(transition.trigger)) return false;

	if (transition.direction != direction) return false;

	sf::FloatRect topLeft(feet.left, feet.top, 1.f, 1.f);
	sf::FloatRect topRight(feet.left + feet.width - 1.f, feet.top, 1.f, 1.f);
	sf::FloatRect bottomLeft(feet.left, feet.top + feet.height - 1.f, 1.f, 1.f);
	sf::FloatRect bottomRight(feet.left + feet.width - 1.f, feet.top + feet.height - 1.f, 1.f, 1.f);

	switch (transition.direction) {
	case TransitionDirection::Up:
		if (transition.direction != direction) return false;
		return topLeft.intersects(transition.trigger) && topRight.intersects(transition.trigger);
	case TransitionDirection::Down:
		if (transition.direction != direction) return false;
		return bottomLeft.intersects(transition.trigger) && bottomRight.intersects(transition.trigger);
	case TransitionDirection::Left:
		if (transition.direction != direction) return false;
		return topLeft.intersects(transition.trigger) && bottomLeft.intersects(transition.trigger);
	case TransitionDirection::Right:
		if (transition.direction != direction) return false;
		return topRight.intersects(transition.trigger) && bottomRight.intersects(transition.trigger);
	}
	return false;
}

bool Map::isPartiallyEnteringTransition(const sf::FloatRect& feet, const std::optional<TransitionDirection>& horizontal, const std::optional<TransitionDirection>& vertical) const {
	for (const auto& transition : transitions) {

		if (!feet.intersects(transition.trigger)) continue;
		
		if (horizontal && canTriggerTransition(feet, transition, *horizontal)) {
			return true;
		}
		if (vertical && canTriggerTransition(feet, transition, *vertical)) {
			return true;
		}
	}
	return false;
}

std::vector<MapTransition> Map::loadTransitionsFromJson(const std::string& mapId, const std::string& filePath) {
	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cerr << "Failed to open maps JSON: " << filePath << std::endl;
		return {};
	}

	json data;
	try { 
		file >> data; 
	}
	catch (const std::exception& e) {
		std::cerr << "Failed to parse maps JSON: " << e.what() << std::endl;
		return {};
	}

	for (const auto& mapJson : data["maps"]) {
		if (mapJson.value("id", "") != mapId)
			continue;
		
		std::vector<MapTransition> transitions;
		for (const auto& t : mapJson["transitions"]) {
			MapTransition transition;
			const auto& trig = t["trigger"];
			transition.trigger = sf::FloatRect(
				trig.value("x", 0.f),
				trig.value("y", 0.f),
				trig.value("w", 0.f),
				trig.value("h", 0.f)
			);
			transition.targetMap.texturePath = t.value("targetMap", "");
			transition.targetMap.collisionMapPath = t.value("targetCollisionMap", "");
			transition.direction = stringToDirection(t.value("direction", ""));

			if (t.contains("spawnPosition")) {
				transition.spawnPosition = sf::Vector2f(
					t["spawnPosition"].value("x", 0.f),
					t["spawnPosition"].value("y", 0.f)
				);
			}

			transitions.push_back(transition);
		}
		return transitions;
	}

	std::cerr << "Map id '" << mapId << "' not found in " << filePath << std::endl;
	return {};
}

TransitionDirection stringToDirection(const std::string& str) {
	if (str == "Up") return TransitionDirection::Up;
	if (str == "Down") return TransitionDirection::Down;
	if (str == "Left") return TransitionDirection::Left;
	if (str == "Right") return TransitionDirection::Right;
	return TransitionDirection::Up; // Default value
}