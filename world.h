#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include "map.h"


class World {
private:
	std::unordered_map <std::string, MapData> mapDefinitions;
	std::vector<Map> loadedMaps;

public:
	void draw(sf::RenderTarget& target);
	void loadMap(const std::string& mapID);
	std::vector<Map>& getLoadedMaps();
	bool isMapLoaded(const std::string& mapID) const;
};