#include "game.h"
#include "skillDatabase.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include "item.h"



Game::Game() : combat(party, messageLog, rng), combatTestSetup(party, messageLog), exploration(rng, messageLog), window(sf::VideoMode({ 1280, 720 }), "Nameless RPG"), rng(std::random_device{}()) {
	gameTexture.create(640, 360);
	SkillDatabase::loadSkills("skills.json");
	ItemDatabase::loadItems("items.json");
	party.emplace_back(messageLog, "Gaden", "spiky", partyInventory);
	party.emplace_back(messageLog, "Kari", "bluey", partyInventory);
	party[1].setDiscipline(DisciplineID::Combatant);
	window.setFramerateLimit(60);

	currentState = GameState::StartMenu;
}

void Game::draw() {
	gameTexture.clear();

	switch (currentState) {

	case GameState::StartMenu:
		startMenu.draw(gameTexture);
		break;

	case GameState::CharacterCreation:
		drawCharacterCreation();
		break;

	case GameState::Exploring:
		exploration.draw(gameTexture, party[0]);
		messageLog.draw(gameTexture);
		break;

	case GameState::Combat:
		combat.draw(gameTexture);
		break;

	case GameState::CombatTest:
		combat.draw(gameTexture);
		break;
	}

	gameTexture.display();

	sf::Sprite gameSprite(gameTexture.getTexture());
	float scaleX = window.getSize().x / static_cast<float>(VirtualWidth);
	float scaleY = window.getSize().y / static_cast<float>(VirtualHeight);
	gameSprite.setScale(scaleX, scaleY);

	window.clear();
	window.draw(gameSprite);
	window.display();
}

StartMenu::StartMenu() {
	if (!font.loadFromFile("assets/Roboto_Condensed-Black.ttf")) {
		std::cerr << "Failed to load font!" << std::endl;
	}
	std::vector<std::string> options = { "Start Game", "Combat Test" };
	for (int i = 0; i < options.size(); i++) {
		sf::Text text(options[i], font, 20);
		text.setPosition(280.f, 240.f + (i * 24));
		text.setFillColor(sf::Color::Black);
		optionTexts.push_back(text);
	}

	backgroundTexture.loadFromFile("assets/characterCreation.png");
	background.setTexture(backgroundTexture);
}

void StartMenu::draw(sf::RenderTarget& target) {
	target.draw(background);


	for (int i = 0; i < optionTexts.size(); i++) {
		if (i == selectedIndex) {
			optionTexts[i].setFillColor(sf::Color::White);
		}
		else {
			optionTexts[i].setFillColor(sf::Color::Black);
		}
		target.draw(optionTexts[i]);
	}


}

void Game::drawCharacterCreation() {
	characterCreationMap.setTexture("characterCreation");
	characterCreationMap.draw(gameTexture);
	party[0].drawCombat(gameTexture);
	messageLog.draw(gameTexture);


}


void Game::update(float dt) {
	inputHandler.update();

	switch (currentState)
	{
	case GameState::StartMenu:
		handleStartMenu();
		break;

	case GameState::CharacterCreation:
		updateCharacterCreation(dt);
		break;

	case GameState::Exploring:
		updateExploration(dt);
		break;
	
	case GameState::Combat:
		combat.update(dt, inputHandler);
		if (combat.getState() == CombatState::Victory) {
			startExploring();
		}
		break;

	case GameState::CombatTest:
		combatTestSetup.update();
		if (combatTestSetup.isFinished()) {
			combat.start();
			currentState = GameState::Combat;
			combatTestSetup.reset();
		}
		break;

	}
}

void Game::updateCharacterCreation(float dt) {

	switch (creationStep) {

	case CreationStep::Name:
		messageLog.setCurrentMessage("Please enter your name: " + party[0].getName(), sf::Color::Black);
		break;

	case CreationStep::Class:
		break;
	}
	
}

void Game::updateExploration(float dt) {
	exploration.update(dt, inputHandler, party[0]);
	/*if (exploration.checkForEncounter(dt, party[0])) {
		startCombat();
	}*/

		MapTransition* transition = exploration.getTransitionAtPosition(party[0].getCollisionBox(party[0].getSprite().getPosition()), party[0].getPreviousCollisionBox(),
			party[0].getHorizontalDirection(), party[0].getVerticalDirection());
		if (transition) {
	
			exploration.getMap().setTexture(transition->targetMap.texturePath);
			exploration.getMap().setCollisionMap(transition->targetMap.collisionMapPath);
			if (transition->spawnPosition.has_value()) party[0].setPosition(transition->spawnPosition->x, transition->spawnPosition->y);

			exploration.getMap().getTransitions() = Map::loadTransitionsFromJson(transition->targetMap.texturePath, "maps.json");
		}
	}

	void Game::startCharacterCreation() {
		currentState = GameState::CharacterCreation;
		creationStep = CreationStep::Name;

		party[0].setPosition(320.f, 180.f);
	}


void Game::startClassSelection() {
	creationStep = CreationStep::Class;

	messageLog.addMessage("Hello, " + party[0].getName() + ". Please choose a discipline: ", sf::Color::Black);

	messageLog.addMessage("-- 1. Mage -- 2. Thief -- 3. Combatant --", sf::Color::Black);
}

void Game::startCombatTest() {
	messageLog.addMessage("1. Mage, 2. Thief, 3. Combatant", sf::Color::Black);
	 
}

void Game::handleStartMenu() {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !startMenu.getLastUpPressed()) {
		startMenu.moveUp();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && !startMenu.getLastDownPressed()) {
		startMenu.moveDown();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && !startMenu.getLastEnterPressed()) {
		int selectedIndex = startMenu.getSelectedIndex();
		if (selectedIndex == 0) {
			startCharacterCreation();
		}
		else if (selectedIndex == 1) {
			currentState = GameState::CombatTest;
		}
	}
	startMenu.setLastUpPressed(sf::Keyboard::isKeyPressed(sf::Keyboard::Up));
	startMenu.setLastDownPressed(sf::Keyboard::isKeyPressed(sf::Keyboard::Down));
	startMenu.setLastEnterPressed(sf::Keyboard::isKeyPressed(sf::Keyboard::Enter));
}

void Game::handleClassSelection(sf::Event event) {
	if (event.type == sf::Event::KeyPressed) {
		switch (event.key.code) {

		case sf::Keyboard::Num1:
			party[0].setDiscipline(DisciplineID::Mage);
			std::cout << party[0].getDiscipline().getName() << std::endl;
			startExploring();
			break;

		case sf::Keyboard::Num2:
			party[0].setDiscipline(DisciplineID::Thief);
			std::cout << party[0].getDiscipline().getName() << std::endl;
			startExploring();
			break;

		case sf::Keyboard::Num3:
			party[0].setDiscipline(DisciplineID::Combatant);
			std::cout << party[0].getDiscipline().getName() << std::endl;
			startExploring();
			break;
		}
	}
}


void Game::handleEvents() {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();

			if (currentState == GameState::CharacterCreation) {
				if (event.type == sf::Event::TextEntered) {
					handleTextInput(event);
				}

				if (event.type == sf::Event::KeyPressed) {
					handleCreationInput(event);
				}
			}

			if (currentState == GameState::CombatTest) {
				combatTestSetup.handleEvent(event);
			}
		}

}

void Game::handleTextInput(sf::Event event) {
	
	if (creationStep == CreationStep::Name) {
		if (event.text.unicode == 8) {// backspace
			party[0].removeLastCharacter();
		}
		else if (event.text.unicode < 128) {
			party[0].addCharacter(static_cast<char>(event.text.unicode));
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
		creationStep = CreationStep::Class;
	}
}

void Game::handleCreationInput(sf::Event event) {
	if (event.key.code == sf::Keyboard::Enter) {
		if (creationStep == CreationStep::Name) {
			startClassSelection();
		}
	}

	if (creationStep == CreationStep::Class) {
		handleClassSelection(event);
	}
}


void Game::startExploring() {
	currentState = GameState::Exploring;
	exploration.start(party[0]);
}

void Game::startCombat() {

		currentState = GameState::Combat;
		combat.start();

}

void Game::run() {
	while (window.isOpen()) {
		float dt = clock.restart().asSeconds();
		handleEvents();
		update(dt);
		draw();
	}
}
