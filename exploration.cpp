#include "exploration.h"
#include "inputHandler.h"
#include <algorithm>
#include <iostream>

Exploration::Exploration(std::mt19937& rng, std::vector<Player>& party, MessageLog& messageLog)
	: rng(rng), party(party), messageLog(messageLog) {
	camera = sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(VirtualWidth), static_cast<float>(VirtualHeight)));
}

void Exploration::start(Player& player) {
	map.setTexture("BurumiaMap");
	map.setCollisionMap("BurumiaCollisionMap");
	map.getTransitions() = Map::loadTransitionsFromJson("BurumiaMap", "maps.json");

	player.setPosition(playerPosition.x, playerPosition.y);
	player.getWalkAnimation().setFrame(1);
}

void Exploration::update(float dt, InputHandler& inputHandler) {
	party[0].update(dt, inputHandler, map);
	setCamera(party[0]);

	openFieldMenu(inputHandler);

	if (currentState == ExplorationState::FieldMenu) {
		fieldMenu.handleInput(inputHandler, party);
	}
}

void Exploration::draw(sf::RenderTarget& target) {
	target.setView(camera);
	map.draw(target);
	party[0].drawExploring(target);
	target.setView(target.getDefaultView());

	if (currentState == ExplorationState::FieldMenu) {
			fieldMenu.draw(target, party);
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
	if (inputHandler.wasPressed(InputAction::Cancel) && currentState == ExplorationState::Exploring) {
		fieldMenu.setCurrentState(FieldMenuState::Main);
		currentState = ExplorationState::FieldMenu;
	} 
	else if (inputHandler.wasPressed(InputAction::Cancel) && currentState == ExplorationState::FieldMenu) {
		if (fieldMenu.getCurrentState() == FieldMenuState::Main) {
			currentState = ExplorationState::Exploring;
			fieldMenu.reset();
		}
	}

	if (fieldMenu.getCurrentState() == FieldMenuState::None) {
		currentState = ExplorationState::Exploring;
		fieldMenu.reset();
	}
}

void FieldMenu::selectState(std::vector<Player>& party) {
	FieldMenu::Option selected = getSelectedOption();
	switch (selected) {
		case FieldMenu::Option::Status:
			currentState = FieldMenuState::StatusScreen;
			break;
		case FieldMenu::Option::Discipline:
			disciplineScreen.getDisciplineSelectMenu().populate(party[selectedMemberIndex]);
			// When opening from the field menu, do not include the "Back" entry
			// in the skill list used for viewing skills on the Discipline screen.
			disciplineScreen.getSkillMenu().populate(party[selectedMemberIndex].getSkills(), false);
			// Ensure we start with the skill list focused (not the discipline selection)
			disciplineScreen.setInDisciplineSelection(false);
			disciplineScreen.setSkillListFocused(false);
			disciplineScreen.getSkillMenu().setSelectedIndex(-1);
			currentState = FieldMenuState::DisciplineScreen;
			break;
		case FieldMenu::Option::Equipment:
			currentState = FieldMenuState::EquipmentScreen;
			break;
		case FieldMenu::Option::Inventory:
			itemMenu.populate(party[selectedMemberIndex].getInventory(), 270.f, 100.f, 10);
			currentState = FieldMenuState::ItemMenu;
			break;
		case FieldMenu::Option::Exit:
			currentState = FieldMenuState::None;
			break;
	}
}

FieldMenu::FieldMenu() {
	// Use base Menu defaults for font; set specific position/spacing for this menu
	setPosition(200.f, 100.f);
	setOptionSpacing(12.f);
	std::vector<std::string> options = { "Status", "Discipline", "Equipment", "Inventory", "Exit" };

	for (size_t i = 0; i < options.size(); ++i) {
		const auto& option = options[i];
		sf::Text text(option, font, 12);
		text.setPosition(menuX, menuY + (i * optionSpacing));
		text.setFillColor(sf::Color::Black);
		optionTexts.push_back(text);
	}
}

void FieldMenu::draw(sf::RenderTarget& target, std::vector<Player>& party) {

	switch (currentState) {
	case FieldMenuState::Main:
		drawMain(target, false);
		break;
	
	case FieldMenuState::StatusScreen:
		statusScreen.draw(target, party[selectedMemberIndex]);
		break;
	case FieldMenuState::DisciplineScreen:
		disciplineScreen.draw(target, party[selectedMemberIndex]);
		break;
	case FieldMenuState::EquipmentScreen:
		// Draw the equipment screen
		break;
	case FieldMenuState::ItemMenu:
		drawMain(target, true);
		itemMenu.draw(target);
		break;
	}

}

void FieldMenu::drawMain(sf::RenderTarget& target, bool disabled) {
	for (size_t i = 0; i < optionTexts.size(); ++i) {
		if (disabled) {
			optionTexts[i].setFillColor(sf::Color(128, 128, 128));
		}
		else if (i == selectedIndex) {
			optionTexts[i].setFillColor(sf::Color::White);
		}
		else {
			optionTexts[i].setFillColor(sf::Color::Black);
		}
		target.draw(optionTexts[i]);
	}
}

void FieldMenu::handleInput(InputHandler& inputHandler, std::vector<Player>& party) {

		switch(currentState) {
		case FieldMenuState::Main:
			handleMain(inputHandler, party);
			break;
		case FieldMenuState::ItemMenu:
			handleItemMenu(inputHandler, party);
			break;
		case FieldMenuState::DisciplineScreen:
			handleDisciplineScreen(inputHandler, party);
			break;
		case FieldMenuState::StatusScreen:
			handleStatusScreen(inputHandler, party);
			break;
		default:
			break;
	}
}

void FieldMenu::handleMain(InputHandler& inputHandler, std::vector<Player>& party) {
	if (inputHandler.wasPressed(InputAction::MenuUp)) {
		moveUp();
	}
	else if (inputHandler.wasPressed(InputAction::MenuDown)) {
		moveDown();
	}
	else if (inputHandler.wasPressed(InputAction::Confirm)) {
		selectState(party);
	}
}

void FieldMenu::handleItemMenu(const InputHandler& inputHandler, std::vector<Player>& party) {
	if (inputHandler.wasPressed(InputAction::MenuUp))
		itemMenu.moveUp();
	if (inputHandler.wasPressed(InputAction::MenuDown))
		itemMenu.moveDown();
	if (inputHandler.wasPressed(InputAction::Confirm)) {
		const auto& slots = party[0].getInventory().getItems();
		int index = itemMenu.getSelectedIndex();
		if (!slots.empty() && index >= 0 && index < (int)slots.size()) {
			const ItemData* item = ItemDatabase::getItemByID(slots[index].itemID);
		}
	}
	if (inputHandler.wasPressed(InputAction::Cancel)) {
		currentState = FieldMenuState::Main;
		itemMenu.reset();
	}
}

void FieldMenu::handleDisciplineScreen(const InputHandler& inputHandler, std::vector<Player>& party) {
	if (inputHandler.wasPressed(InputAction::MenuRight)) {
		selectedMemberIndex = (selectedMemberIndex + 1) % party.size();
		disciplineScreen.setNeedsRebuild(true);
		disciplineScreen.getSkillMenu().populate(party[selectedMemberIndex].getSkills(), false);
	}
	else if (inputHandler.wasPressed(InputAction::MenuLeft)) {
		selectedMemberIndex = (selectedMemberIndex - 1 + party.size()) % party.size();
		disciplineScreen.setNeedsRebuild(true);
		disciplineScreen.getSkillMenu().populate(party[selectedMemberIndex].getSkills(), false);
	}

	if (inputHandler.wasPressed(InputAction::MenuUp)) {
		if (disciplineScreen.isInDisciplineSelection()) {
			disciplineScreen.getDisciplineSelectMenu().moveUp();
		}
		else if (!disciplineScreen.isSkillListFocused()) {
			disciplineScreen.getSkillMenu().setSelectedIndex(disciplineScreen.getSkillMenu().getSkillCount() - 1);
			disciplineScreen.setSkillListFocused(true);
		}
		else if (disciplineScreen.isSkillListFocused()) {
			if (disciplineScreen.getSkillMenu().getSelectedIndex() == 0) {
				disciplineScreen.setSkillListFocused(false);
				disciplineScreen.getSkillMenu().setSelectedIndex(-1);
			}
			else {
				disciplineScreen.getSkillMenu().moveUp();
			}
		}
	}
	else if (inputHandler.wasPressed(InputAction::MenuDown)) {
		if (disciplineScreen.isInDisciplineSelection()) {
			disciplineScreen.getDisciplineSelectMenu().moveDown();
		}
		else if (!disciplineScreen.isSkillListFocused()) {
			disciplineScreen.getSkillMenu().setSelectedIndex(0);
			disciplineScreen.setSkillListFocused(true);
		}
		else if (disciplineScreen.isSkillListFocused()) {
			if (disciplineScreen.getSkillMenu().getSelectedIndex() == disciplineScreen.getSkillMenu().getSkillCount() - 1) {
				disciplineScreen.setSkillListFocused(false);
				disciplineScreen.getSkillMenu().setSelectedIndex(-1);
			}
			else {
				disciplineScreen.getSkillMenu().moveDown();
			}
		}
	}

	if (inputHandler.wasPressed(InputAction::Confirm)) {
		if (!disciplineScreen.isInDisciplineSelection()) {
			disciplineScreen.setInDisciplineSelection(true);
		}
		else {
			disciplineScreen.getDisciplineSelectMenu().changeDiscipline(party[selectedMemberIndex]);
			disciplineScreen.setInDisciplineSelection(false);
			statusScreen.setNeedsRebuild(true);
		}
	}

	if (inputHandler.wasPressed(InputAction::Cancel)) {
		if (disciplineScreen.isInDisciplineSelection()) {
			disciplineScreen.setInDisciplineSelection(false);
		}
		else {
			currentState = FieldMenuState::Main;
		}
	}
}

void FieldMenu::handleStatusScreen(const InputHandler& inputHandler, std::vector<Player>& party) {
	if (inputHandler.wasPressed(InputAction::MenuRight)) {
		selectedMemberIndex = (selectedMemberIndex + 1) % party.size();
		statusScreen.setNeedsRebuild(true);
	}
	else if (inputHandler.wasPressed(InputAction::MenuLeft)) {
		selectedMemberIndex = (selectedMemberIndex - 1 + party.size()) % party.size();
		statusScreen.setNeedsRebuild(true);
	}

	if (inputHandler.wasPressed(InputAction::Cancel)) {
		currentState = FieldMenuState::Main;
	}
}

