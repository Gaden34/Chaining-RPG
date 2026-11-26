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
	bool drawExteriorDarkened = false;
	Map* currentMap = nullptr;

public:
	void drawBackground(sf::RenderTarget& target);
	void drawForeground(sf::RenderTarget& target);
	void loadMapData();
	void loadMap(const std::string& mapID);
	void unloadCurrentMap(sf::Vector2f position);
	std::vector<std::unique_ptr<Map>>& getLoadedMaps();
	bool isMapLoaded(const std::string& mapID) const;
	Map* getMapAtPosition(sf::Vector2f position) const;
	bool isBlocked(sf::Vector2f position) const;
};