#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "world.h"


using json = nlohmann::json;

void World::loadMap(const std::string& mapID) {

	if (isMapLoaded(mapID)) return;

	MapData mapData;

	std::ifstream file("maps.json");
	if (!file.is_open()) {
		std::cerr << "Failed to open maps JSON: maps.json" << std::endl;
		return;
	}

	json data;
	file >> data;

	for (const auto& mapJson : data["maps"]) {
		if (mapJson.value("id", "") != mapID)
			continue;

		// Load map data from JSON
		mapData.id = mapID;
		mapData.texturePath = mapJson.value("texture", "");
		mapData.collisionMapPath = mapJson.value("collisionMap", "");

		if (mapJson.contains("worldPosition")) {
			mapData.worldPosition = sf::Vector2f(
				mapJson["worldPosition"].value("x", 0.f),
				mapJson["worldPosition"].value("y", 0.f)
			);

			mapData.transitions = Map::loadTransitionsFromJson(mapID, "maps.json");

			loadedMaps.emplace_back(mapData);
		}
		return;
	}
	std::cerr << "Map not found: " << mapID << std::endl;
}

Map& World::getMap() {
	if (loadedMaps.empty()) {
		loadedMaps.emplace_back();
	}
	return loadedMaps[0];
}

bool World::isMapLoaded(const std::string& mapID) const {
	for (const auto& map : loadedMaps) {
		if (map.getID() == mapID) {
			return true;
		}
	}
	return false;
}