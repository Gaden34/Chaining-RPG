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