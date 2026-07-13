#include "combat.h"
#include <iostream>
#include <algorithm>


Combat::Combat(std::vector<Player>& p, MessageLog& m, std::mt19937& rng) : party(p), messageLog(m), rng(rng) {
	currentState = CombatState::PlayerTurn;
	backgroundTexture.loadFromFile("assets/battleSimulator.png");
	background.setTexture(backgroundTexture);
	font.loadFromFile("assets/Roboto_Condensed-Black.ttf");
	pointerTexture.loadFromFile("assets/targetPointer.png");
	pointerSprite.setTexture(pointerTexture);

}

void Combat::update(float dt) {
	switch (currentState) {
	case CombatState::PlayerTurn:
		handlePlayerTurn();
		break;

	case CombatState::SelectingEnemy:
		targetEnemy();
		break;

	case CombatState::ExecutingActions:
		executeNextAction();
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

void Combat::draw(sf::RenderTarget& target) {

	target.draw(background);

	for (auto& player : party) {
		player.draw(target);
	}
	for (auto& enemy : enemies) {
		enemy.draw(target);
	}

	messageLog.draw(target);

	if (currentState == CombatState::PlayerTurn) {
		if (!party.empty()) {
			sf::Text nameLabel(party[activePlayerIndex].getName(), font, 12);
			nameLabel.setPosition(320.f, 288.f);
			nameLabel.setFillColor(sf::Color::White);
			target.draw(nameLabel);
		}
		if (inSkillMenu)
			skillMenu.draw(target);
		else
			menu.draw(target);

	}
	if (currentState == CombatState::SelectingEnemy) {
		drawTargetPointer(target);
	}

}

void Combat::start() {
	auto encounter = makeRandomEncounter();
	currentState = CombatState::PlayerTurn;
	activePlayerIndex = 0;
	activeEnemyIndex = 0;
	playerActed.assign(party.size(), false);
	enemyActed.clear();
	actionQueue.clear();
	currentAction = {};
	menu.reset();
	inSkillMenu = false;
	enemies.clear();

	for (const EnemySpawn& spawn : encounter) {
		if (spawn.data == nullptr || spawn.count <= 0) {
			continue; // Skip invalid spawns
		}
		for (int i = 0; i < spawn.count; ++i) {
			enemies.emplace_back(*spawn.data);
		}
	}

	if (enemies.empty()) return;

	party[0].setPosition(175.f, 180.f);
	party[1].setPosition(175.f, 220.f);

	for (int i = 0; i < enemies.size(); i++) {
		enemies[i].setPosition(465.f, 180.f + i * 40);
	}

	enemyActed.assign(enemies.size(), false);

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
				currentAction = {};
				currentAction.type = ActionType::Skill;
				currentAction.actor = &party[activePlayerIndex];
				currentAction.skill = party[activePlayerIndex].getSkillByIndex(index);
				currentState = CombatState::SelectingEnemy;
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
			menu.moveUp();
		if (downPressed && !menu.getLastDownPressed())
			menu.moveDown();
		if (enterPressed && !menu.getLastEnterPressed()) {
			CombatMenu::MenuOption selectedOption = menu.getSelectedOption();
			switch (selectedOption) {
			case CombatMenu::MenuOption::Attack:
				currentAction = {};
				currentAction.type = ActionType::Attack;
				currentAction.actor = &party[activePlayerIndex];
				menu.setLastEnterPressed(true);
				currentState = CombatState::SelectingEnemy;
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

void Combat::targetEnemy() {
	bool upPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
	bool downPressed  = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
	bool enterPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);

	if (upPressed && !lastUpPressed) {
		activeEnemyIndex = (activeEnemyIndex - 1 + (int)enemies.size()) % (int)enemies.size();
	} 
	if (downPressed && !lastDownPressed) {
		activeEnemyIndex = (activeEnemyIndex + 1) % (int)enemies.size();
	}

	if (enterPressed && !lastEnterPressed) {
		currentAction.target = &enemies[activeEnemyIndex];
		actionQueue.push_back(currentAction);
		currentAction = {};
		advanceActivePlayer();
		menu.setLastEnterPressed(true);
	}
	lastUpPressed = upPressed;
	lastDownPressed = downPressed;
	lastEnterPressed = enterPressed;
}

void Combat::drawTargetPointer(sf::RenderTarget& target) {
	sf::FloatRect bounds = enemies[activeEnemyIndex].getGlobalBounds();
	float x = bounds.left + bounds.width / 2.f - pointerSprite.getGlobalBounds().width / 2.f;
	float y = bounds.top - pointerSprite.getGlobalBounds().height - 4.f;
	pointerSprite.setPosition(x, y);
	target.draw(pointerSprite);
}

int Combat::randomRange(int min, int max) {
	std::uniform_int_distribution<int> dist(min, max);
	return dist(rng);
}

void Combat::performAttack(QueuedAction& action) {
	Player* player = static_cast<Player*>(action.actor);
	Enemy* enemy = static_cast<Enemy*>(action.target);
	int damage = randomRange((player->getAttack() * 90) / 100, (player->getAttack() * 110) / 100);
	enemy->takeDamage(damage);
	chain.registerHit();
	chain.openWindow();
	std::cout << "Chain count: " << chain.getChainCount() << std::endl;
	messageLog.addMessage(player->getName() + " hits the " + enemy->getName() + " for " + std::to_string(damage) + " damage!", sf::Color::Black);
	
	checkEnemyDeath(*enemy);
	if (actionQueue.empty()) eraseDeadEnemies();
}

void Combat::performSkill(QueuedAction& action) {
	Player* player = static_cast<Player*>(action.actor);
	Enemy* enemy = static_cast<Enemy*>(action.target);
	Skill* skill = action.skill;
	if (player->getMp() < skill->getMpCost()) {
		messageLog.addMessage("Not enough MP!", sf::Color::Red);
		return;
	}
	player->setMp(player->getMp() - skill->getMpCost());
	switch (skill->getType()) {
	case SkillType::Attack: {
		int totalDamage = 0;
		
		for (auto& hit : skill->getHits()) {
		int damage = randomRange(((skill->getDamage() + (player->getAttack() * 2 / 10)) * 90) / 100, ((skill->getDamage() + (player->getAttack() * 2 / 10)) * 110) / 100);
		totalDamage += static_cast<int>(skill->getDamage());
		enemy->takeDamage(damage);
		chain.registerHit();
		chain.openWindow();
		std::cout << "Chain count: " << chain.getChainCount() << std::endl;
		}
	
		
		messageLog.addMessage(player->getName() + " uses " + skill->getName() + " on the " + enemy->getName() + " for " + std::to_string(totalDamage) + " damage!", sf::Color::Black);
		
		checkEnemyDeath(*enemy);
		if (actionQueue.empty()) eraseDeadEnemies();
		break;
	}

	case SkillType::Magic {
		int totalDamage = 0;
		
		for (auto& hit : skill->getHits()) {
		int damage = randomRange(((skill->getDamage() + (player->getMagAttack() * 2 / 10)) * 90) / 100, ((skill->getDamage() + (player->getMagAttack() * 2 / 10)) * 110) / 100);
		totalDamage += static_cast<int>(skill->getDamage());
		enemy->takeDamage(damage);
		chain.registerHit();
		chain.openWindow();
		std::cout << "Chain count: " << chain.getChainCount() << std::endl;
		}
	
		
		messageLog.addMessage(player->getName() + " uses " + skill->getName() + " on the " + enemy->getName() + " for " + std::to_string(totalDamage) + " damage!", sf::Color::Black);
		
		checkEnemyDeath(*enemy);
		if (actionQueue.empty()) eraseDeadEnemies();
		break;
	}

	case SkillType::Heal: {
		int healAmount = static_cast<int>(skill->getDamage());
		player->setHp(std::min(player->getHp() + healAmount, player->getMaxHp()));
		messageLog.addMessage(player->getName() + " uses " + skill->getName() + " and recovers " + std::to_string(healAmount) + " HP!", sf::Color::Green);
		break;
	}
	default:
		break;
	}
}

void Combat::executeAction(QueuedAction& action)
{
	switch (action.type)
	{
	case ActionType::Attack:
		performAttack(action);
		break;

	case ActionType::Skill:
		performSkill(action);
		break;

	case ActionType::Item:
		//performItem(action);
		break;

	case ActionType::Defend:
		//performDefend(action);
		break;

	default:
		break;
	}
}

void Combat::executeNextAction() {
	if (actionQueue.empty()) {
		currentState = CombatState::EnemyTurn;
		return;
	}

	QueuedAction action = actionQueue.front();
	if (!action.target->isAlive()) {
		for (auto& enemy : enemies) {
			if (enemy.isAlive()) {
				action.target = &enemy;
				break;
			}
		}
	}
	actionQueue.erase(actionQueue.begin());
	executeAction(action);

	if (currentState == CombatState::Victory || currentState == CombatState::Defeat) {
		actionQueue.clear();
		return;
	}

	currentState = CombatState::PlayerAnimation;
}



void Combat::advanceActivePlayer() {
	playerActed[activePlayerIndex] = true;

	for (int i = 1; i <= (int)party.size(); i++) {
		int next = (activePlayerIndex + i) % (int)party.size();
		if (!playerActed[next]) {
			activePlayerIndex = next;
			menu.reset();
			inSkillMenu = false;
			currentState = CombatState::PlayerTurn;
			return;
		}
	}

	playerActed.assign(party.size(), false);
	activePlayerIndex = 0;
	menu.reset();
	inSkillMenu = false;
	currentState = CombatState::ExecutingActions;
}

void Combat::handleEnemyTurn() {

	for (auto& enemy : enemies) {
		int randomPlayerIndex = randomRange(0, (int)party.size() - 1);
		
		if (enemy.isAlive()) {
			party[randomPlayerIndex].takeDamage(enemy.getAttack());
			
			for (auto& player : party) {
				if (player.getHp() <= 0) player.setHp(0);
			}

			messageLog.addMessage("The " + enemy.getName() + " hits " + party[randomPlayerIndex].getName() + " for " + std::to_string(enemy.getAttack()) + " damage!", sf::Color::Black);
			messageLog.addMessage(party[randomPlayerIndex].getName() + " HP: " + std::to_string(party[randomPlayerIndex].getHp()) + "/" + std::to_string(party[randomPlayerIndex].getMaxHp()), sf::Color::Black);
		}
	}

	currentState = CombatState::EnemyAnimation;
}

void Combat::updatePlayerAnimation(float dt) {
	if (currentState == CombatState::PlayerAnimation) {
		animationTimer += dt;
	}

	if (animationTimer >= 3.0f) {
		animationTimer = 0.f;
		currentState = CombatState::ExecutingActions;
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

std::vector<EnemySpawn> Combat::makeRandomEncounter() {
	std::vector<EnemySpawn> encounter;

	int numEnemies = randomRange(1, 3);
	for (int i = 0; i < numEnemies; ++i) {
		int enemyType = randomRange(0, 2);
		switch (enemyType) {
		case 0:
			encounter.push_back({ &knight, 1});
			break;
		case 1:
			encounter.push_back({ &bat, 1 });
			break;
		case 2:
			encounter.push_back({ &bat, 1 });
			break;
		default:
			break;
		}
	}

	return encounter;
}

void Combat::checkEnemyDeath(Enemy& enemy) {
	if (enemy.getHp() <= 0) {
		enemy.setHp(0);

		messageLog.addMessage(party[activePlayerIndex].getName() + " defeated the " + enemy.getName() + " and gained " + std::to_string(enemy.getExpValue()) + " experience points!", sf::Color::Blue);
		for (auto& player : party) {
			player.addExp(enemy.getExpValue());
		}

		for (auto& enemy : enemies) {
			if (enemy.isAlive()) {
				return;
			}
		}
		currentState = CombatState::Victory;
	}
}

void Combat::eraseDeadEnemies() {
	std::erase_if(enemies, [](const auto& enemy) {
		return enemy.getHp() <= 0;
		});
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

void CombatMenu::draw(sf::RenderTarget& target) {
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

CombatMenu::MenuOption CombatMenu::getSelectedOption() {
	return static_cast<MenuOption>(selectedIndex);
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

void SkillMenu::draw(sf::RenderTarget& target) {
	if (skillCount == 0) {
		sf::Text noSkills("No skills learned.", font, 12);
		noSkills.setPosition(menuX, menuY - optionSpacing);
		noSkills.setFillColor(sf::Color(128, 128, 128));
		target.draw(noSkills);
	}
	for (int i = 0; i < static_cast<int>(optionTexts.size()); i++) {
		optionTexts[i].setFillColor(i == selectedIndex ? sf::Color::White : sf::Color::Black);
		target.draw(optionTexts[i]);
	}
}

int SkillMenu::getSelectedIndex() const {
	if (selectedIndex < skillCount) return selectedIndex;
	return -1;
}


