#pragma once
#include <SFML/Graphics.hpp>


class Map {
private:
	sf::Texture texture;
	sf::Sprite sprite;

public:
	Map();
	void draw(sf::RenderWindow& window);
	void setTexture(std::string t);
	

};