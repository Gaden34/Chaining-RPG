#pragma once
#include <SFML/Graphics.hpp>


class Map {
private:
	sf::Texture texture;
	sf::Sprite sprite;

public:
	Map();
	void draw(sf::RenderTarget& target);
	void setTexture(std::string t);
	sf::Vector2u getSize() const { return texture.getSize(); }
	

};