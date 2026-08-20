#pragma once
#include <SFML/Graphics.hpp>
#include <string>


struct MapTransition {
	sf::FloatRect trigger;
	std::string destination;
	sf::Vector2f spawnPosition;
};


class Map {
private:
	sf::Texture texture;
	sf::Sprite sprite;
	sf::Image collisionMap;

	std::vector<MapTransition> transitions;
	

public:
	Map();
	void draw(sf::RenderTarget& target);
	void setTexture(std::string t);
	sf::Vector2u getSize() const { return texture.getSize(); }
	void setCollisionMap(std::string t);
	bool isBlocked(int x, int y) const;
	sf::Image& getCollisionMap() { return collisionMap; }
	std::vector<MapTransition>& getTransitions() { return transitions; }
	MapTransition* getTransitionAtPosition(const sf::FloatRect& bounds);

};