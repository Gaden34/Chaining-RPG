#include "game.h"
#include <iostream>
#include <algorithm>
#include <vector>



Game::Game() : player(levelSystem), combat(player, messageLog, levelSystem), window(sf::VideoMode({ 800, 600 }), "Nameless RPG"), rng(std::random_device{}()) {
	window.setFramerateLimit(60);

	startCharacterCreation();
}

void Game::draw() {
	window.clear();

	switch (currentState) {

	case GameState::CharacterCreation:
		drawCharacterCreation();
		break;

	case GameState::Exploring:
		drawExploring();
		break;

	case GameState::Combat:
		combat.draw(window);
		break;
	}

	window.display();
}

void Game::drawCharacterCreation() {
	map.setTexture("characterCreation");
	map.draw(window);
	player.draw(window);
	messageLog.draw(window);

	
}

void Game::drawExploring() {
	map.setTexture("dirtgrassmap");
	map.draw(window);
	player.draw(window);
	messageLog.draw(window);

	
}


void Game::update(float dt) {
	switch (currentState)
	{

	case GameState::CharacterCreation:
		updateCharacterCreation(dt);
		break;

	case GameState::Exploring:
		player.update(dt);
		checkForEncounter(dt);
		break;

	case GameState::Combat:
		combat.update(dt);
		if (combat.getState() == CombatState::Victory) {
			currentState = GameState::Exploring;
		}
		break;

	}
}

void Game::updateCharacterCreation(float dt) {

	switch (creationStep) {

	case CreationStep::Name:
		messageLog.setCurrentMessage("Please enter your name: " + player.getName(), sf::Color::Black);
		break;

	case CreationStep::Class:
		break;
	}
	
}

void Game::startCharacterCreation() {
	currentState = GameState::CharacterCreation;
	creationStep = CreationStep::Name;

	player.setPosition(400.f, 300.f);
}


void Game::startClassSelection() {
	creationStep = CreationStep::Class;

	messageLog.addMessage("Hello, " + player.getName() + ". Please choose a discipline: ", sf::Color::Black);

	messageLog.addMessage("-- 1. Mage -- 2. Thief -- 3. Combatant --", sf::Color::Black);
}

void Game::handleClassSelection(sf::Event event) {
	if (event.type == sf::Event::KeyPressed) {
		switch (event.key.code) {

		case sf::Keyboard::Num1:
			player.setDiscipline(DisciplineID::Mage);
			std::cout << player.getDiscipline().getName() << std::endl;
			startExploring();
			break;

		case sf::Keyboard::Num2:
			player.setDiscipline(DisciplineID::Thief);
			std::cout << player.getDiscipline().getName() << std::endl;
			startExploring();
			break;

		case sf::Keyboard::Num3:
			player.setDiscipline(DisciplineID::Combatant);
			std::cout << player.getDiscipline().getName() << std::endl;
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
		}

}

void Game::handleTextInput(sf::Event event) {
	
	if (creationStep == CreationStep::Name) {
		if (event.text.unicode == 8) {// backspace
			player.removeLastCharacter();
		}
		else if (event.text.unicode < 128) {
			player.addCharacter(static_cast<char>(event.text.unicode));
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

	player.setPosition(100.f, 100.f);
}

void Game::checkForEncounter(float dt) {
	std::uniform_int_distribution<int> rollEncounter(1, 100);
	if (player.getIsMoving()) encounterTimer += dt;

	if (encounterTimer >= 3.0f && player.getIsMoving()) {
		if (rollEncounter(rng) == 1) {
			combat.start(knight);
			messageLog.addMessage("You've encountered a knight!", sf::Color::White);
			currentState = GameState::Combat;
			encounterTimer = 0.f;
		}
	}
}

void Game::run() {
	while (window.isOpen()) {
		float dt = clock.restart().asSeconds();
		handleEvents();
		update(dt);
		draw();
	}
}
