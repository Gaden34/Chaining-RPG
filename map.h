#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <optional>

enum class TransitionDirection
{
	Up,
	Down,
	Left,
	Right
};

struct MapData {
	std::string texturePath;
	std::string collisionMapPath;
};

struct MapTransition {
	sf::FloatRect trigger;
	MapData targetMap;
	std::optional<sf::Vector2f> spawnPosition;
	TransitionDirection direction;
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
	static std::vector<MapTransition> loadTransitionsFromJson(const std::string& mapId, const std::string& filePath);

};