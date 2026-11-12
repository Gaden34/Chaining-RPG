#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "world.h"


using json = nlohmann::json;

void World::draw(sf::RenderTarget& target) {
	for (auto& map : loadedMaps) {
		map.draw(target);
	}
}

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
		}

			mapData.transitions = Map::loadTransitionsFromJson(mapID, "maps.json");

			loadedMaps.emplace_back(mapData);

		return;
	}
	std::cerr << "Map not found: " << mapID << std::endl;
}

std::vector<Map>& World::getLoadedMaps() {
	return loadedMaps;
}

bool World::isMapLoaded(const std::string& mapID) const {
	for (const auto& map : loadedMaps) {
		if (map.getID() == mapID) {
			return true;
		}
	}
	return false;
}