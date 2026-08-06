#include "combat.h"
#include "item.h"
#include "textUtils.h"
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

	chain.update(dt);

	switch (currentState) {
	case CombatState::PlayerTurn:
		handlePlayerTurn();
		break;

	case CombatState::SelectingTarget:
		targetCharacter();
		break;

	case CombatState::ChoosingQueuedActions:
		handleQueuedActionMenu();
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

switch (currentState) {	
case CombatState::PlayerTurn:
	if (!party.empty()) {
		sf::Text nameLabel(party[activePlayerIndex].getName(), font, 12);
		nameLabel.setPosition(320.f, 288.f);
		nameLabel.setFillColor(sf::Color::White);
		target.draw(nameLabel);
	}
		if (inItemMenu)
			itemMenu.draw(target);
		else if (inSkillMenu)
			skillMenu.draw(target);
		else
			menu.draw(target);
		break;

case CombatState::SelectingTarget:
	drawTargetPointer(target);
	break;

case CombatState::ChoosingQueuedActions:
	queuedActionMenu.draw(target);
	break;
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
	queuedConsumableCounts.clear();
	currentAction = {};
	menu.reset();
	inSkillMenu = false;
	inItemMenu = false;
	itemMenu.reset();
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

	if (inItemMenu) {
		if (upPressed && !itemMenu.getLastUpPressed())
			itemMenu.moveUp();
		if (downPressed && !itemMenu.getLastDownPressed())
			itemMenu.moveDown();
		if (enterPressed && !itemMenu.getLastEnterPressed()) {
			const auto& slots = party[activePlayerIndex].getInventory().getItems();
			int index = itemMenu.getSelectedIndex();
			if (!slots.empty() && index >= 0 && index < (int)slots.size()) {
				const ItemData* item = ItemDatabase::getItemByID(slots[index].itemID);
				if (item) {
					if (!canQueueConsumableItem(*item, party[activePlayerIndex].getInventory())) {
						messageLog.addMessage("No more " + item->name + " left to queue.", sf::Color::Red);
					} else {
					currentAction = {};
					currentAction.type = ActionType::Item;
					currentAction.actor = &party[activePlayerIndex];
					currentAction.item = const_cast<ItemData*>(item);
					inItemMenu = false;
					itemMenu.reset();
					beginTargeting();
					}
				}
			}
		}
		if (escapePressed && !itemMenu.getLastEscapePressed()) {
			inItemMenu = false;
			itemMenu.reset();
		}
		itemMenu.setLastUpPressed(upPressed);
		itemMenu.setLastDownPressed(downPressed);
		itemMenu.setLastEnterPressed(enterPressed);
		itemMenu.setLastEscapePressed(escapePressed);
	}
	else if (inSkillMenu) {
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
				beginTargeting();
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
				beginTargeting();
				break;
			case CombatMenu::MenuOption::Skill:
				skillMenu.populate(party[activePlayerIndex].getSkills());
				skillMenu.setLastEnterPressed(true);
				inSkillMenu = true;
				break;
			case CombatMenu::MenuOption::Item:
				itemMenu.populate(party[activePlayerIndex].getInventory());
				itemMenu.setLastEnterPressed(true);
				inItemMenu = true;
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

void Combat::buildValidTargets() {
	validTargets.clear();
	for (auto& player : party) {
		if (player.isAlive())
			validTargets.push_back(&player);
	}

	for (auto& enemy : enemies) {
		if (enemy.isAlive())
			validTargets.push_back(&enemy);
	}
}

void Combat::beginTargeting() {
	buildValidTargets();
	if (currentAction.type == ActionType::Attack || currentAction.type == ActionType::Skill) {
			validTargetIndex = 0;
			for (int i = 0; i < (int)validTargets.size(); ++i) {
				if (dynamic_cast<Enemy*>(validTargets[i])) {
					validTargetIndex = i;
					break;
				}
			}
			currentState = CombatState::SelectingTarget;
	}
	else if (currentAction.type == ActionType::Item) {
		validTargetIndex = 0;
		currentState = CombatState::SelectingTarget;
	}	
	else {
		currentState = CombatState::ChoosingQueuedActions;
	}
}

void Combat::targetCharacter() {
	bool upPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
	bool downPressed  = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
	bool enterPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);

	if (upPressed && !lastUpPressed) {
		validTargetIndex= (validTargetIndex - 1 + (int)validTargets.size()) % (int)validTargets.size();
	} 
	if (downPressed && !lastDownPressed) {
		validTargetIndex = (validTargetIndex + 1) % (int)validTargets.size();
	}

	if (enterPressed && !lastEnterPressed) {
		currentAction.target = validTargets[validTargetIndex];
		if (currentAction.type == ActionType::Item && currentAction.item != nullptr) {
			reserveConsumableItem(*currentAction.item);
		}
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
	sf::FloatRect bounds = validTargets[validTargetIndex]->getGlobalBounds();
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
	Character* target = action.target;
	float damage = static_cast<float>(player->getAttack());
	damage = randomRange(damage * 0.95f, damage * 1.05f);
	damage = damage * chain.getDamagePercent() / 100;
	int finalDamage = static_cast<int>(std::round(damage));
	target->takeDamage(finalDamage);
	chain.registerHit();
	std::cout << "Chain count: " << chain.getChainCount() << std::endl;
	messageLog.addMessage(player->getName() + " hits the " + TextUtils::lowerFirst(target->getName()) + " for " + std::to_string(finalDamage) + " damage!", sf::Color::Black);

	handleDeath(*target);
	if (actionQueue.empty()) {
		eraseDeadEnemies();
		resetEnemyIndex();
	}
}

void Combat::performSkill(QueuedAction& action) {
	Player* player = static_cast<Player*>(action.actor);
	Character* target = action.target;
	Skill* skill = action.skill;
	if (player->getMp() < skill->getMpCost()) {
		messageLog.addMessage("Not enough MP!", sf::Color::Red);
		return;
	}
	player->setMp(player->getMp() - skill->getMpCost());
	calculateSkillDamage(skill, player, target);
	handleSteal(skill, player, target);
	handleDeath(*target);
}

void Combat::performItem(QueuedAction& action) {
	Player* player = static_cast<Player*>(action.actor);
	Character* target = action.target;
	const ItemData* item = action.item;

	if (item && item->isConsumable) {
		releaseConsumableItemReservation(*item);
	}

	ItemUseResult result = ItemSystem::useItem(*item, *player, *target);

	if (!result.success) {
		messageLog.addMessage("Failed to use " + item->name + ": " + result.failureReason, sf::Color::Red);
		return;
	}

	if (item->isConsumable)
		player->getInventory().removeItem(item->id, 1);

	for (const auto& event : result.effectEvents) {
		std::string msg = player->getName() + " uses " + item->name;
		switch (event.kind) {
		case ItemEventKind::Heal:
			if (event.attribute == Attribute::HP)
				msg += " on " + target->getName() + ", restoring " + std::to_string(event.appliedAmount) + " HP!";
			else if (event.attribute == Attribute::MP)
				msg += " on " + target->getName() + ", restoring " + std::to_string(event.appliedAmount) + " MP!";
			break;
		case ItemEventKind::Damage:
			msg += " on " + target->getName() + " for " + std::to_string(event.appliedAmount) + " damage!";
			break;
		case ItemEventKind::StatusHeal:
			msg += ", curing " + target->getName() + " of a status!";
			break;
		case ItemEventKind::Buff:
			msg += " on " + target->getName() + ".";
			break;
		default:
			break;
		}
		messageLog.addMessage(msg, sf::Color::Green);
	}

	handleDeath(*target);
}

void Combat::calculateSkillDamage(Skill* skill, Character* actor, Character* target) {

	int totalDamage = 0;
	std::cout << actor->getName() << std::endl;

	for (auto& hit : skill->getHits()) {
		chain.registerHit();
		float damage = static_cast<float>(skill->getDamage());

		switch (skill->getType()) {
		case SkillType::Attack: {
			damage = damage * (100.0f + actor->getAttack()) / 100.0f;
			break;
		}
		
		case SkillType::Magic: {
			damage = damage * (100.0f + actor->getMagAttack()) / 100.0f;
			break;
		}

		case SkillType::Steal: {
			damage = static_cast<float>(actor->getAttack());
			break;
		}
		default:
			std::cout << "Invalid type" << std::endl;
			break;

		}
		
		damage = randomRange(damage * 0.95f, damage * 1.05f);
		damage = damage * chain.getDamagePercent() / 100.0f;
		int finalDamage = static_cast<int>(std::round(damage));
		totalDamage += finalDamage;
		target->takeDamage(finalDamage);
		std::cout << "Chain count: " << chain.getChainCount() << std::endl;
		

	}
	messageLog.addMessage(actor->getName() + " uses " + skill->getName() + " on the " + TextUtils::lowerFirst(target->getName()) + " for " + std::to_string(totalDamage) + " damage!", sf::Color::Black);
}

void Combat::handleSteal(Skill* skill, Character* actor, Character* target) {
	if (skill->getType() != SkillType::Steal) {
		return;
	}
	StealResult result = skill->useSteal(*actor, *target, rng);
	switch (result.result) {
	case StealResult::Result::Success: {
		const ItemData* stolenItem = ItemDatabase::getItemByID(result.stolenItemID);
		if (stolenItem) {
			messageLog.addMessage(actor->getName() + " successfully stole a " + stolenItem->name + " from " + target->getName() + "!", sf::Color::Black);
		} else {
			messageLog.addMessage(actor->getName() + " successfully stole an unknown item from " + target->getName() + "!", sf::Color::Black);
		}
		break;
	}
	case StealResult::Result::Failed:
		messageLog.addMessage(target->getName() + " thwarted the steal.", sf::Color::Black);
		break;
	case StealResult::Result::NoItems:
		messageLog.addMessage("There was nothing to steal.", sf::Color::Black);
		break;
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
		performItem(action);
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
		queuedConsumableCounts.clear();
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
		queuedConsumableCounts.clear();
		return;
	}

	currentState = CombatState::PlayerAnimation;
}

void Combat::handleQueuedActionMenu() {

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !queuedActionMenu.getLastUpPressed()) {
		queuedActionMenu.moveUp();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && !queuedActionMenu.getLastDownPressed()) {
		queuedActionMenu.moveDown();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && !queuedActionMenu.getLastEnterPressed()) {
		int selectedIndex = queuedActionMenu.getSelectedIndex();
		if (selectedIndex >= 0 && selectedIndex < actionQueue.size()) {
			if (!actionQueue[selectedIndex].target->isAlive()) {
				for (auto& enemy : enemies) {
					if (enemy.isAlive()) {
						actionQueue[selectedIndex].target = &enemy;
						break;
					}
				}
			}
			executeAction(actionQueue[selectedIndex]);
			actionQueue.erase(actionQueue.begin() + selectedIndex);
			queuedActionMenu.populate(actionQueue);
		}
	}

	if (currentState == CombatState::Victory || currentState == CombatState::Defeat) {
		actionQueue.clear();
		queuedConsumableCounts.clear();
		queuedActionMenu.reset();
		return;
	}

	if (actionQueue.empty()) {
		queuedConsumableCounts.clear();
		eraseDeadEnemies();
		resetEnemyIndex();
		currentState = CombatState::PlayerAnimation;
		queuedActionMenu.reset();
	}

	queuedActionMenu.setLastUpPressed(sf::Keyboard::isKeyPressed(sf::Keyboard::Up));
	queuedActionMenu.setLastDownPressed(sf::Keyboard::isKeyPressed(sf::Keyboard::Down));
	queuedActionMenu.setLastEnterPressed(sf::Keyboard::isKeyPressed(sf::Keyboard::Enter));
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
	inItemMenu = false;
	itemMenu.reset();
	queuedActionMenu.populate(actionQueue);
	currentState = CombatState::ChoosingQueuedActions;
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

void Combat::updateAnimations(float dt) {
	for (auto& anim : activeAnimations)
		anim.elapsedTime += dt;

	activeAnimations.erase(
		std::remove_if(activeAnimations.begin(), activeAnimations.end(),
			[](const ActiveAnimation& a) { return a.elapsedTime >= a.duration; }),
		activeAnimations.end());
	
}

void Combat::updatePlayerAnimation(float dt) {
	if (currentState == CombatState::PlayerAnimation) {
		animationTimer += dt;
	}

	if (animationTimer >= 3.0f) {
		animationTimer = 0.f;
		currentState = CombatState::EnemyTurn;
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

void Combat::handleDeath(Character& character) {
	if (character.getHp() > 0) return;
	
		character.setHp(0);

		if (Enemy* enemy = dynamic_cast<Enemy*>(&character)) {
			messageLog.addMessage(party[activePlayerIndex].getName() + " defeated the " + enemy->getName() + " and gained " + std::to_string(enemy->getExpValue()) + " experience points!", sf::Color::Blue);
			for (auto& player : party) {
				player.addExp(enemy->getExpValue());
			}
		}

		for (auto& enemy : enemies) {
			if (enemy.isAlive()) {
				return;
			}
		}
		currentState = CombatState::Victory;
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

QueuedActionMenu::QueuedActionMenu() {
	if (!font.loadFromFile("assets/Roboto_Condensed-Black.ttf")) {
		std::cerr << "Failed to load font!" << std::endl;
	}
}

void QueuedActionMenu::populate(const std::vector<QueuedAction>& actions) {
	optionTexts.clear();
	for (const auto& action : actions) {
		std::string actionName;
		switch (action.type) {
		case ActionType::Attack:
			actionName = action.actor->getName() + ": Attack";
			break;
		case ActionType::Skill:
			actionName = action.actor->getName() + ": " + action.skill->getName();
			break;
		case ActionType::Item:
			actionName = action.actor->getName() + ": Item";
			break;
		case ActionType::Defend:
			actionName = action.actor->getName() + ": Defend";
			break;
		default:
			actionName = "Unknown";
			break;
		}
		sf::Text text(actionName, font, 12);
		text.setPosition(menuX, menuY + optionTexts.size() * optionSpacing);
		text.setFillColor(sf::Color::Black);
		optionTexts.push_back(text);
	}
	selectedIndex = 0;
}

void QueuedActionMenu::draw(sf::RenderTarget& target) {
	for (int i = 0; i < static_cast<int>(optionTexts.size()); i++) {
		optionTexts[i].setFillColor(i == selectedIndex ? sf::Color::White : sf::Color::Black);
		target.draw(optionTexts[i]);
	}
}

void Combat::resetEnemyIndex() {
	activeEnemyIndex = 0;
}

int Combat::getQueuedConsumableCount(ItemID itemID) const {
	auto it = queuedConsumableCounts.find(itemID);
	if (it == queuedConsumableCounts.end()) {
		return 0;
	}
	return it->second;
}

bool Combat::canQueueConsumableItem(const ItemData& item, const Inventory& inventory) const {
	if (!item.isConsumable) {
		return true;
	}
	const int ownedCount = inventory.getQuantity(item.id);
	const int queuedCount = getQueuedConsumableCount(item.id);
	return ownedCount - queuedCount > 0;
}

void Combat::reserveConsumableItem(const ItemData& item) {
	if (!item.isConsumable) {
		return;
	}
	queuedConsumableCounts[item.id]++;
}

void Combat::releaseConsumableItemReservation(const ItemData& item) {
	if (!item.isConsumable) {
		return;
	}
	auto it = queuedConsumableCounts.find(item.id);
	if (it == queuedConsumableCounts.end()) {
		return;
	}
	it->second--;
	if (it->second <= 0) {
		queuedConsumableCounts.erase(it);
	}
}

ItemMenu::ItemMenu() {
	if (!font.loadFromFile("assets/Roboto_Condensed-Black.ttf")) {
		std::cerr << "Failed to load font!" << std::endl;
	}
}

void ItemMenu::populate(const Inventory& inventory) {
	optionTexts.clear();
	selectedIndex = 0;

	for (const auto& slot : inventory.getItems()) {
		const ItemData* data = ItemDatabase::getItemByID(slot.itemID);
		std::string label = data ? data->name + " x" + std::to_string(slot.quantity) : "Unknown";
		sf::Text text(label, font, 12);
		text.setPosition(menuX, menuY + optionTexts.size() * optionSpacing);
		text.setFillColor(sf::Color::Black);
		optionTexts.push_back(text);
	}
}

void ItemMenu::draw(sf::RenderTarget& target) {
	for (int i = 0; i < (int)optionTexts.size(); i++) {
		optionTexts[i].setFillColor(i == selectedIndex ? sf::Color::White : sf::Color::Black);
		target.draw(optionTexts[i]);
	}
}
