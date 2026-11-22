#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "world.h"


using json = nlohmann::json;

void World::draw(sf::RenderTarget& target) {
	for (auto& map : loadedMaps) {
		if (drawExteriorDarkened) {
			sf::RectangleShape darkenRect(sf::Vector2f(target.getSize()));
			darkenRect.setFillColor(sf::Color(26, 26, 26, 255));
			target.draw(darkenRect);
			map->draw(target);
		}
		else {
			map->draw(target);
		}
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

		if (mapJson.contains("darkenOutside")) {
			mapData.darkenOutside = mapJson.value("darkenOutside", false);
			if (mapData.darkenOutside)
				drawExteriorDarkened = true;
			else drawExteriorDarkened = false;
		}

		if (mapJson.contains("worldPosition")) {
			mapData.worldPosition = sf::Vector2f(
				mapJson["worldPosition"].value("x", 0.f),
				mapJson["worldPosition"].value("y", 0.f)
			);
		}
		if (mapJson.contains("transitions")) {
			mapData.transitions = Map::loadTransitionsFromJson(mapID, "maps.json");
		}

			loadedMaps.push_back(std::make_unique<Map>(mapData));
			std::cout << "Loaded map: " << mapID << std::endl;
			std::cout << "Loaded maps: " << loadedMaps.size() << std::endl;
			std::cout << "Position: "
				<< mapData.worldPosition.x << ", "
				<< mapData.worldPosition.y << std::endl;

		return;
	}
	std::cerr << "Map not found: " << mapID << std::endl;
}

std::vector<std::unique_ptr<Map>>& World::getLoadedMaps() {
	return loadedMaps;
}

void World::unloadCurrentMap(sf::Vector2f position) {
	for (auto it = loadedMaps.begin(); it != loadedMaps.end(); ++it) {
		if ((*it)->containsWorldPosition(position)) {
			std::cout << "Unloading map: " << (*it)->getID() << std::endl;
			loadedMaps.erase(it);
			return;
		}
	}
}

bool World::isMapLoaded(const std::string& mapID) const {
	for (const auto& map : loadedMaps) {
		if (map->getID() == mapID) {
			return true;
		}
	}
	return false;
}

Map* World::getMapAtPosition(sf::Vector2f position) const {
	for (auto& map : loadedMaps) {
		if (map->containsWorldPosition(position)) {
			return map.get();
		}
	}
	return nullptr;
}

bool World::isBlocked(sf::Vector2f position) const {
	for (const auto& map : loadedMaps) {
		if (map->containsWorldPosition(position)) {
			sf::Vector2f localPos = map->worldToLocalPosition(position);
			return map->isBlocked(static_cast<int>(localPos.x), static_cast<int>(localPos.y));
		}

		
	}
	return true;
}