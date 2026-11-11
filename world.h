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
	void loadMap(const std::string& mapID);
	Map& getMap();
	bool isMapLoaded(const std::string& mapID) const;
};