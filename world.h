#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <memory>
#include "map.h"


class World {
private:
	std::unordered_map <std::string, MapData> mapDefinitions;
	std::vector<std::unique_ptr<Map>> loadedMaps;

public:
	void draw(sf::RenderTarget& target);
	void loadMap(const std::string& mapID);
	std::vector<std::unique_ptr<Map>>& getLoadedMaps();
	bool isMapLoaded(const std::string& mapID) const;
	Map* getMapAtPosition(sf::Vector2f position);
};