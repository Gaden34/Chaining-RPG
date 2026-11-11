#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "world.h"


using json = nlohmann::json;

MapData World::loadMap(const std::string& mapID) {
	MapData mapData;

	std::ifstream file("maps.json");
	if (!file.is_open()) {
		std::cerr << "Failed to open maps JSON: maps.json" << std::endl;
		return mapData;
	}

	json data;
	file >> data;

	for (const auto& mapJson : data["maps"]) {
		if (mapJson.value("id", "") != mapID)
			continue;

		// Load map data from JSON
		mapData.texturePath = mapJson.value("texture", "");
		mapData.collisionMapPath = mapJson.value("collisionMap", "");

		if (mapJson.contains("worldPosition")) {
			mapData.worldPosition = sf::Vector2f(
				mapJson["worldPosition"].value("x", 0.f),
				mapJson["worldPosition"].value("y", 0.f)
			);
		
			mapData.transitions = Map::loadTransitionsFromJson(mapID, "maps.json");

			return mapData;
	}

		std::cerr << "Map not found: " << mapID << std::endl;
	return mapData;
}