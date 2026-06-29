#include "combat.h"
#include <iostream>
#include <algorithm>


Combat::Combat(Player& p, MessageLog& m, LevelSystem& l) : player(p), messageLog(m), levelSystem(l) {
	currentState = CombatState::PlayerTurn;
	backgroundTexture.loadFromFile("assets/battleBG.png");
	background.setTexture(backgroundTexture);
}

void Combat::update(float dt) {
	switch (currentState) {
	case CombatState::PlayerTurn:
		handlePlayerTurn();
		break;

	case CombatState::PlayerAnimation:
		updatePlayerAnimation(dt);
		break;

	case CombatState::EnemyTurn:
		handleEnemyTurn();
		break;

	case CombatState::EnemyAnimation:
		updateEnemyAnimation(dt);
		break;

	case CombatState::Victory:
		break;


	}

}

void Combat::draw(sf::RenderWindow& window) {

	window.draw(background);

	player.draw(window);

	for (auto& enemy : enemies) {
		enemy.draw(window);
	}

	messageLog.draw(window);

	if (currentState == CombatState::PlayerTurn) {
		menu.draw(window);
	}

}

void Combat::start(EnemyData& data) {
	currentState = CombatState::PlayerTurn;
	enemies.emplace_back(data);

	player.setPosition(200.f, 400.f);

	for (int i = 0; i < enemies.size(); i++) {
		enemies[i].setPosition(600.f, 350.f + i * 50);
	}

}

CombatState Combat::getState() {
	return currentState;
}

void Combat::handlePlayerTurn() {
	if (currentState == CombatState::PlayerTurn) {
		bool upPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
		bool downPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
		bool enterPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);

		// Only call handleInput on key press transition (not while held)
		if (upPressed && !menu.getLastUpPressed()) {
			menu.handleInput(sf::Keyboard::Up);
		}
		if (downPressed && !menu.getLastDownPressed()) {
			menu.handleInput(sf::Keyboard::Down);
		}

			// Handle menu selection on Enter key press transition
			if (enterPressed && !menu.getLastEnterPressed()) {
				CombatMenu::MenuOption selectedOption = menu.getSelectedOption();
				switch (selectedOption) {
				case CombatMenu::MenuOption::Attack:
					playerAttack();
					break;
				case CombatMenu::MenuOption::Skill:
					// Handle skill selection
					break;
				case CombatMenu::MenuOption::Item:
					// Handle item selection
					break;
				case CombatMenu::MenuOption::Defend:
					// Handle defend action
					break;
				default:
					break;
				}
			}

			// Store current state for next frame
			menu.setLastUpPressed(upPressed);
			menu.setLastDownPressed(downPressed);
			menu.setLastEnterPressed(enterPressed);
			}
		}

void Combat::playerAttack() {
	Enemy& enemy = enemies[0];
	int damage = player.getAttack();

	enemy.takeDamage(damage);
	messageLog.addMessage(player.getName() + " hits the " + enemy.getName() + " for " + std::to_string(damage) + " damage!", sf::Color::Black);

	checkEnemyDeath(enemy);

	if (currentState == CombatState::PlayerTurn) {
		currentState = CombatState::PlayerAnimation;
	}
}

void Combat::handleEnemyTurn() {
	player.takeDamage(enemies[0].getAttack());
	if (player.getHp() <= 0) player.setHp(0);
	std::cout << "The " << enemies[0].getName() << " hits you for " << enemies[0].getAttack() << " damage!\n";
	std::cout << "Player HP: " << player.getHp() << "/" << player.getMaxHp() << std::endl;

	currentState = CombatState::EnemyAnimation;
}

void Combat::updatePlayerAnimation(float dt) {
	if (currentState == CombatState::PlayerAnimation) {
		animationTimer += dt;
	}

	if (animationTimer >= 3.0f) {
		currentState = CombatState::EnemyTurn;
		animationTimer = 0.f;
	}

}

void Combat::updateEnemyAnimation(float dt) {
	if (currentState == CombatState::EnemyAnimation) {
		animationTimer += dt;
	}

	if (animationTimer >= 3.0f) {
		currentState = CombatState::PlayerTurn;
		animationTimer = 0.f;
	}


}

void Combat::checkEnemyDeath(Enemy& enemy) {
	if (enemy.getHp() <= 0) {
		int level = player.getLevel();
		enemy.setHp(0);
		
		messageLog.addMessage(player.getName() + " defeated the " + enemy.getName() + " and gained " + std::to_string(enemy.getExpValue()) + " experience points!", sf::Color::Blue);
		player.addExp(enemy.getExpValue());
		if (player.getLevel() > level) {
			messageLog.addMessage(player.getName() + " has reached level " + std::to_string(player.getLevel()) + "!", sf::Color::Yellow);
		}


		std::erase_if(enemies, [](const auto& enemy) {
			return enemy.getHp() <= 0;
			});

		if (enemies.empty()) {
			currentState = CombatState::Victory;
		}
	}
}

void Combat::checkCombatEnd() {
	/*
	

	if (enemies.empty()) {
		currentState = CombatState::Victory;
	} else if (player.getHp() <= 0) {
        currentState = CombatState::Defeat;
    }
	
	*/
}

CombatMenu::CombatMenu() {
	if (!font.loadFromFile("assets/Roboto_Condensed-Black.ttf")) {
		std::cerr << "Failed to load font!" << std::endl;
	}

	std::vector<std::string> options = { "Attack", "Skill", "Item", "Defend" };

	for (int i = 0; i < options.size(); i++) {
		sf::Text text(options[i], font, 12);
		text.setPosition(menuX, menuY + (i * optionSpacing));
		text.setFillColor(sf::Color::Black);
		optionTexts.push_back(text);
	}
}

void CombatMenu::handleInput(sf::Keyboard::Key key) {
	if (key == sf::Keyboard::Up) {
		selectedIndex = (selectedIndex - 1 + (int)MenuOption::Count) % (int)MenuOption::Count;
	}
	else if (key == sf::Keyboard::Down) {
		selectedIndex = (selectedIndex + 1) % (int)MenuOption::Count;
	}
}

void CombatMenu::draw(sf::RenderWindow& window) {
	for (int i = 0; i < optionTexts.size(); i++) {
		if (i == selectedIndex) {
			optionTexts[i].setFillColor(sf::Color::White);
		}
		else {
			optionTexts[i].setFillColor(sf::Color::Black);
		}
		window.draw(optionTexts[i]);
	}
}

CombatMenu::MenuOption CombatMenu::getSelectedOption() {
	return static_cast<MenuOption>(selectedIndex);
}

void CombatMenu::reset() {
	selectedIndex = 0;
}