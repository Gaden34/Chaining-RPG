#include "combat.h"
#include <iostream>
#include <algorithm>


Combat::Combat(std::vector<Player>& p, MessageLog& m, LevelSystem& l) : party(p), messageLog(m), levelSystem(l) {
	currentState = CombatState::PlayerTurn;
	backgroundTexture.loadFromFile("assets/battleBG.png");
	background.setTexture(backgroundTexture);
	font.loadFromFile("assets/Roboto_Condensed-Black.ttf");
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

	for (auto& player : party) {
		player.draw(window);
	}
	for (auto& enemy : enemies) {
		enemy.draw(window);
	}

	messageLog.draw(window);

	if (currentState == CombatState::PlayerTurn) {
		if (!party.empty()) {
			sf::Text nameLabel(party[activePlayerIndex].getName(), font, 12);
			nameLabel.setPosition(400.f, 480.f);
			nameLabel.setFillColor(sf::Color::Yellow);
			window.draw(nameLabel);
		}
		if (inSkillMenu)
			skillMenu.draw(window);
		else
			menu.draw(window);
	}

}

void Combat::start(EnemyData& data) {
	currentState = CombatState::PlayerTurn;
	activePlayerIndex = 0;
	playerActed.assign(party.size(), false);
	menu.reset();
	inSkillMenu = false;
	enemies.emplace_back(data);

	party[0].setPosition(200.f, 400.f);
	party[1].setPosition(200.f, 450.f);

	for (int i = 0; i < enemies.size(); i++) {
		enemies[i].setPosition(600.f, 350.f + i * 50);
	}

}

CombatState Combat::getState() {
	return currentState;
}

void Combat::handlePlayerTurn() {
	if (currentState != CombatState::PlayerTurn) return;

	bool upPressed     = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
	bool downPressed   = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
	bool enterPressed  = sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);
	bool escapePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);
	bool leftPressed   = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
	bool rightPressed  = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);

	if (!inSkillMenu) {
		if (leftPressed && !lastLeftPressed) {
			activePlayerIndex = (activePlayerIndex - 1 + (int)party.size()) % (int)party.size();
			menu.reset();
		}
		if (rightPressed && !lastRightPressed) {
			activePlayerIndex = (activePlayerIndex + 1) % (int)party.size();
			menu.reset();
		}
	}
	lastLeftPressed  = leftPressed;
	lastRightPressed = rightPressed;

	if (playerActed[activePlayerIndex]) return;

	if (inSkillMenu) {
		if (upPressed && !skillMenu.getLastUpPressed())
			skillMenu.handleInput(sf::Keyboard::Up);
		if (downPressed && !skillMenu.getLastDownPressed())
			skillMenu.handleInput(sf::Keyboard::Down);
		if (enterPressed && !skillMenu.getLastEnterPressed()) {
			int index = skillMenu.getSelectedIndex();
			if (index == -1) {
				inSkillMenu = false;
				skillMenu.reset();
				menu.setLastEnterPressed(true);
			}
			else {
				Skill* skill = party[activePlayerIndex].getSkillByIndex(index);
				if (skill) playerUseSkill(skill);
			}
		}
		if (escapePressed && !skillMenu.getLastEscapePressed()) {
			inSkillMenu = false;
			skillMenu.reset();
		}
		skillMenu.setLastUpPressed(upPressed);
		skillMenu.setLastDownPressed(downPressed);
		skillMenu.setLastEnterPressed(enterPressed);
		skillMenu.setLastEscapePressed(escapePressed);
	}
	else {
		if (upPressed && !menu.getLastUpPressed())
			menu.handleInput(sf::Keyboard::Up);
		if (downPressed && !menu.getLastDownPressed())
			menu.handleInput(sf::Keyboard::Down);
		if (enterPressed && !menu.getLastEnterPressed()) {
			CombatMenu::MenuOption selectedOption = menu.getSelectedOption();
			switch (selectedOption) {
			case CombatMenu::MenuOption::Attack:
				playerAttack();
				break;
			case CombatMenu::MenuOption::Skill:
				skillMenu.populate(party[activePlayerIndex].getSkills());
				skillMenu.setLastEnterPressed(true);
				inSkillMenu = true;
				break;
			case CombatMenu::MenuOption::Item:
				break;
			case CombatMenu::MenuOption::Defend:
				break;
			default:
				break;
			}
		}
		menu.setLastUpPressed(upPressed);
		menu.setLastDownPressed(downPressed);
		menu.setLastEnterPressed(enterPressed);
	}
}

void Combat::playerAttack(Player& player, Enemy& enemy) {
	
	int damage = player.getAttack();

	enemy.takeDamage(damage);
	messageLog.addMessage(player.getName() + " hits the " + enemy.getName() + " for " + std::to_string(damage) + " damage!", sf::Color::Black);

	checkEnemyDeath(enemy);

	if (currentState == CombatState::PlayerTurn) {
		advanceActivePlayer();
	}
}

void Combat::playerUseSkill(Skill* skill) {
	if (party[activePlayerIndex].getMp() < skill->getMpCost()) {
		messageLog.addMessage("Not enough MP!", sf::Color::Red);
		return;
	}

	party[activePlayerIndex].setMp(party[activePlayerIndex].getMp() - skill->getMpCost());

	switch (skill->getType()) {
	case SkillType::Attack: {
		Enemy& enemy = enemies[0];
		int damage = static_cast<int>(skill->getDamage());
		enemy.takeDamage(damage);
		messageLog.addMessage(party[activePlayerIndex].getName() + " uses " + skill->getName() + " on the " + enemy.getName() + " for " + std::to_string(damage) + " damage!", sf::Color::Black);
		checkEnemyDeath(enemy);
		break;
	}
	case SkillType::Heal: {
		int healAmount = static_cast<int>(skill->getDamage());
		party[activePlayerIndex].setHp(std::min(party[activePlayerIndex].getHp() + healAmount, party[activePlayerIndex].getMaxHp()));
		messageLog.addMessage(party[activePlayerIndex].getName() + " uses " + skill->getName() + " and recovers " + std::to_string(healAmount) + " HP!", sf::Color::Green);
		break;
	}
	default:
		break;
	}

	inSkillMenu = false;
	skillMenu.reset();

	if (currentState == CombatState::PlayerTurn) {
		advanceActivePlayer();
	}
}

void Combat::advanceActivePlayer() {
	playerActed[activePlayerIndex] = true;

	for (int i = 1; i <= (int)party.size(); i++) {
		int next = (activePlayerIndex + i) % (int)party.size();
		if (!playerActed[next]) {
			activePlayerIndex = next;
			menu.reset();
			inSkillMenu = false;
			return;
		}
	}

	playerActed.assign(party.size(), false);
	activePlayerIndex = 0;
	menu.reset();
	inSkillMenu = false;
	currentState = CombatState::PlayerAnimation;
}

void Combat::handleEnemyTurn() {
	party[0].takeDamage(enemies[0].getAttack());
	if (party[0].getHp() <= 0) party[0].setHp(0);
	messageLog.addMessage("The " + enemies[0].getName() + " hits you for " + std::to_string(enemies[0].getAttack()) + " damage!", sf::Color::Black);
	messageLog.addMessage("Player HP: " + std::to_string(party[0].getHp()) + "/" + std::to_string(party[0].getMaxHp()), sf::Color::Black);

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
		int level = party[0].getLevel();
		enemy.setHp(0);

		messageLog.addMessage(party[0].getName() + " defeated the " + enemy.getName() + " and gained " + std::to_string(enemy.getExpValue()) + " experience points!", sf::Color::Blue);
		party[0].addExp(enemy.getExpValue());
		if (party[0].getLevel() > level) {
			messageLog.addMessage(party[0].getName() + " has reached level " + std::to_string(party[0].getLevel()) + "!", sf::Color::Yellow);
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

SkillMenu::SkillMenu() {
	if (!font.loadFromFile("assets/Roboto_Condensed-Black.ttf")) {
		std::cerr << "Failed to load font!" << std::endl;
	}
}

void SkillMenu::populate(const std::vector<std::unique_ptr<Skill>>& skills) {
	optionTexts.clear();
	skillCount = static_cast<int>(skills.size());

	for (int i = 0; i < skillCount; i++) {
		sf::Text text(skills[i]->getName() + " (" + std::to_string(skills[i]->getMpCost()) + " MP)", font, 12);
		text.setPosition(menuX, menuY + i * optionSpacing);
		text.setFillColor(sf::Color::Black);
		optionTexts.push_back(text);
	}

	sf::Text backText("Back", font, 12);
	backText.setPosition(menuX, menuY + skillCount * optionSpacing);
	backText.setFillColor(sf::Color::Black);
	optionTexts.push_back(backText);

	selectedIndex = 0;
}

void SkillMenu::handleInput(sf::Keyboard::Key key) {
	int total = static_cast<int>(optionTexts.size());
	if (total == 0) return;
	if (key == sf::Keyboard::Up) {
		selectedIndex = (selectedIndex - 1 + total) % total;
	}
	else if (key == sf::Keyboard::Down) {
		selectedIndex = (selectedIndex + 1) % total;
	}
}

void SkillMenu::draw(sf::RenderWindow& window) {
	if (skillCount == 0) {
		sf::Text noSkills("No skills learned.", font, 12);
		noSkills.setPosition(menuX, menuY - optionSpacing);
		noSkills.setFillColor(sf::Color(128, 128, 128));
		window.draw(noSkills);
	}
	for (int i = 0; i < static_cast<int>(optionTexts.size()); i++) {
		optionTexts[i].setFillColor(i == selectedIndex ? sf::Color::White : sf::Color::Black);
		window.draw(optionTexts[i]);
	}
}

int SkillMenu::getSelectedIndex() const {
	if (selectedIndex < skillCount) return selectedIndex;
	return -1;
}

void SkillMenu::reset() {
	selectedIndex = 0;
}
