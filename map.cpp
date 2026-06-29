#include "map.h"


Map::Map() {
	texture.loadFromFile("assets/characterCreation.png");
	sprite.setTexture(texture);
}

void Map::draw(sf::RenderWindow& window) {
	window.draw(sprite);
}

void Map::setTexture(std::string t) {
	texture.loadFromFile("assets/" + t + ".png");
	sprite.setTexture(texture);
}