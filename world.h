#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "map.h"


class World {
private:
	std::vector<Map> loadedMaps;

public:
	void loadMap(const std::string& mapID, sf::Vector2f position);

