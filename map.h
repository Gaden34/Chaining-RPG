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

struct MapTransition {
	sf::FloatRect trigger;
	std::string targetMapID;
	TransitionDirection direction;
	std::optional<sf::Vector2f> spawnPosition;
};


struct MapData {
	std::string id;
	std::string texturePath;
	std::string collisionMapPath;
	sf::Vector2f worldPosition;
	std::vector<MapTransition> transitions;

};

struct CollisionBoxCorners {
	sf::FloatRect topLeft;
	sf::FloatRect topRight;
	sf::FloatRect bottomLeft;
	sf::FloatRect bottomRight;
};


class Map {
private:
	sf::Texture texture;
	sf::Sprite sprite;
	sf::Image collisionMap;
	sf::Vector2f worldPosition;
	std::string id;

	std::vector<MapTransition> transitions;
	

public:
	Map() = default;
	Map(MapData& data);
	void draw(sf::RenderTarget& target);
	void setTexture(std::string t);
	sf::Vector2u getSize() const { return texture.getSize(); }
	void setCollisionMap(std::string t);
	void setPosition(const sf::Vector2f& position) { worldPosition = position; sprite.setPosition(position); }
	sf::Vector2f getPosition() const { return worldPosition; }
	sf::Vector2f worldToLocal(const sf::Vector2f& position) const { return position - worldPosition; }
	bool isBlocked(int x, int y) const;
	sf::Image& getCollisionMap() { return collisionMap; }
	std::vector<MapTransition>& getTransitions() { return transitions; }
	std::string getID() const { return id; }
	MapTransition* getTransitionAtPosition(const sf::FloatRect& currentBounds, const sf::FloatRect& previousBounds, const std::optional<TransitionDirection>& horizontal, const std::optional<TransitionDirection>& vertical);
	bool canTriggerTransition(const sf::FloatRect& currentFeet, const sf::FloatRect& previousFeet, const MapTransition& transition, TransitionDirection direction) const;
	static std::vector<MapTransition> loadTransitionsFromJson(const std::string& mapId, const std::string& filePath);

};

TransitionDirection stringToDirection(const std::string& str);
CollisionBoxCorners getCollisionBoxCorners(const sf::FloatRect& rect);