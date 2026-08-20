#include <iostream>
#include "map.h"


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

MapTransition* Map::getTransitionAtPosition(const sf::FloatRect& bounds) {
	for (auto& transition : transitions) {
		if (bounds.intersects(transition.trigger)) {
			return &transition;
		}
	}
	return nullptr;
}