#pragma once
#include <SFML/Graphics.hpp>


class Map {
private:
	sf::Texture texture;
	sf::Sprite sprite;
	sf::Image collisionMap;
	

public:
	Map();
	void draw(sf::RenderTarget& target);
	void setTexture(std::string t);
	sf::Vector2u getSize() const { return texture.getSize(); }
	void setCollisionMap(std::string t);
	bool isBlocked(int x, int y) const;
	sf::Image& getCollisionMap() { return collisionMap; }

};