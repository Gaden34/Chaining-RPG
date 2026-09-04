#include "exploration.h"
#include "inputHandler.h"
#include <algorithm>
#include <iostream>

Exploration::Exploration(std::mt19937& rng, MessageLog& messageLog)
	: rng(rng), messageLog(messageLog) {
	camera = sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(VirtualWidth), static_cast<float>(VirtualHeight)));
}

void Exploration::start(Player& player) {
	map.setTexture("BurumiaMap");
	map.setCollisionMap("BurumiaCollisionMap");
	map.getTransitions() = Map::loadTransitionsFromJson("BurumiaMap", "maps.json");

	player.setPosition(playerPosition.x, playerPosition.y);
	player.getWalkAnimation().setFrame(1);
}

void Exploration::update(float dt, InputHandler& inputHandler, Player& player) {
	player.update(dt, inputHandler, map);
	setCamera(player);

	if (inItemMenu) {
		if (inputHandler.wasPressed(InputAction::MenuUp))
			itemMenu.moveUp();
		if (inputHandler.wasPressed(InputAction::MenuDown))
			itemMenu.moveDown();
		if (inputHandler.wasPressed(InputAction::Cancel)) {
			inItemMenu = false;
			itemMenu.reset();
		}
		return;
	}

	openFieldMenu(inputHandler);

	if (inFieldMenu) {
		fieldMenu.handleInput(inputHandler);
	}
}

void Exploration::draw(sf::RenderTarget& target, Player& player) {
	target.setView(camera);
	map.draw(target);
	player.drawExploring(target);
	target.setView(target.getDefaultView());

	if (inFieldMenu) {
			fieldMenu.draw(target, player);
	}
}

bool Exploration::checkForEncounter(float dt, Player& player) {
	std::uniform_int_distribution<int> rollEncounter(1, 100);
	if (player.getIsMoving()) encounterTimer += dt;

	if (encounterTimer >= 3.0f && player.getIsMoving()) {
		if (rollEncounter(rng) == 1) {
			playerPosition = player.getGlobalBounds().getPosition();
			encounterTimer = 0.f;
			return true;
		}
	}
	return false;
}

MapTransition* Exploration::getTransitionAtPosition(const sf::FloatRect& currentBounds, const sf::FloatRect& previousBounds, const std::optional<TransitionDirection>& horizontal, const std::optional<TransitionDirection>& vertical) {
	return map.getTransitionAtPosition(currentBounds, previousBounds, horizontal, vertical);
}

void Exploration::setCamera(Player& player) {
	sf::Vector2u mapSize = map.getSize();
	float halfWidth = camera.getSize().x / 2.f;
	float halfHeight = camera.getSize().y / 2.f;
	float cameraX = player.getGlobalBounds().getPosition().x + player.getGlobalBounds().width / 2.f;
	float cameraY = player.getGlobalBounds().getPosition().y + player.getGlobalBounds().height / 2.f;
	cameraX = std::clamp(cameraX, halfWidth, static_cast<float>(mapSize.x) - halfWidth);
	cameraY = std::clamp(cameraY, halfHeight, static_cast<float>(mapSize.y) - halfHeight);
	camera.setCenter(cameraX, cameraY);
}

void Exploration::openFieldMenu(InputHandler& inputHandler) {
	if (inputHandler.wasPressed(InputAction::Cancel) && !inFieldMenu) {
		inFieldMenu = true;
	} 
	else if (inputHandler.wasPressed(InputAction::Cancel) && inFieldMenu) {
		if (fieldMenu.getCurrentState() == FieldMenuState::Main) {
			inFieldMenu = false;
			fieldMenu.reset();
		}
		else {
			fieldMenu.setCurrentState(FieldMenuState::Main);
		}
		
	}
}

void FieldMenu::onSelect() {
	FieldMenu::Option selected = getSelectedOption();
	switch (selected) {
		case FieldMenu::Option::Status:
			currentState = FieldMenuState::StatusScreen;
			break;
		case FieldMenu::Option::Discipline:
			currentState = FieldMenuState::DisciplineScreen;
			break;
		case FieldMenu::Option::Equipment:
			currentState = FieldMenuState::EquipmentScreen;
			break;
		case FieldMenu::Option::Inventory:
			currentState = FieldMenuState::ItemScreen;
			break;
		case FieldMenu::Option::Exit:
			currentState = FieldMenuState::Main;
			break;
	}
}

FieldMenu::FieldMenu() {
	std::vector<std::string> options = { "Status", "Discipline", "Equipment", "Inventory", "Exit" };

	for (size_t i = 0; i < options.size(); ++i) {
		const auto& option = options[i];
		sf::Text text(option, font, 12);
		text.setPosition(menuX, menuY + (i * optionsSpacing));
		text.setFillColor(sf::Color::Black);
		optionTexts.push_back(text);
	}
}

void FieldMenu::draw(sf::RenderTarget& target, Player& player) {

	switch (currentState) {
	case FieldMenuState::Main:
		for (size_t i = 0; i < optionTexts.size(); ++i) {
			if (i == selectedIndex) {
				optionTexts[i].setFillColor(sf::Color::White);
			}
			else {
				optionTexts[i].setFillColor(sf::Color::Black);
			}
			target.draw(optionTexts[i]);
		}
		break;
	
	case FieldMenuState::StatusScreen:
		statusScreen.draw(target, player);
		break;
	case FieldMenuState::DisciplineScreen:
		// Draw the discipline screen
		break;
	case FieldMenuState::EquipmentScreen:
		// Draw the equipment screen
		break;
	//case FieldMenuState::ItemScreen:
		//itemMenu.draw(target);
		break;
	}

}

