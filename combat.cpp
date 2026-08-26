#include "combat.h"
#include "item.h"
#include "textUtils.h"
#include <iostream>
#include <algorithm>


Combat::Combat(std::vector<Player>& p, MessageLog& m, std::mt19937& rng) : party(p), messageLog(m), rng(rng) {
	currentState = CombatState::PlayerTurn;
	backgroundTexture.loadFromFile("assets/battleBG.png");
	background.setTexture(backgroundTexture);
	font.loadFromFile("assets/Roboto_Condensed-Black.ttf");
	pointerTexture.loadFromFile("assets/targetPointer.png");
	pointerSprite.setTexture(pointerTexture);

}

void Combat::update(float dt, const InputHandler& input) {

	chain.update(dt);
	updateSkillEffect(dt);

	switch (currentState) {
	case CombatState::PlayerTurn:
		handlePlayerTurn(input);
		break;

	case CombatState::SelectingTarget:
		targetCharacter(input);
		break;

	case CombatState::ChoosingQueuedActions:
		handleQueuedActionMenu(input);
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
		player.drawCombat(target);
	}
	for (auto& enemy : enemies) {
		enemy.draw(target);
	}

	drawSkillEffect(target);

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

void Combat::handlePlayerTurn(const InputHandler& input) {
	if (currentState != CombatState::PlayerTurn) return;

	const bool leftJustPressed = input.wasPressed(InputAction::MenuLeft);
	const bool rightJustPressed = input.wasPressed(InputAction::MenuRight);
	const bool upJustPressed = input.wasPressed(InputAction::MenuUp);
	const bool downJustPressed = input.wasPressed(InputAction::MenuDown);
	const bool confirmJustPressed = input.wasPressed(InputAction::Confirm);
	const bool cancelJustPressed = input.wasPressed(InputAction::Cancel);

	if (!inSkillMenu && !inItemMenu) {
		if (leftJustPressed) {
			activePlayerIndex = (activePlayerIndex - 1 + (int)party.size()) % (int)party.size();
			menu.reset();
		}
		if (rightJustPressed) {
			activePlayerIndex = (activePlayerIndex + 1) % (int)party.size();
			menu.reset();
		}
	}

	if (playerActed[activePlayerIndex]) return;

	if (inItemMenu) {
		if (upJustPressed)
			itemMenu.moveUp();
		if (downJustPressed)
			itemMenu.moveDown();
		if (confirmJustPressed) {
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
		if (cancelJustPressed) {
			inItemMenu = false;
			itemMenu.reset();
		}
	}
	else if (inSkillMenu) {
		if (upJustPressed)
			skillMenu.handleInput(sf::Keyboard::Up);
		if (downJustPressed)
			skillMenu.handleInput(sf::Keyboard::Down);
		if (confirmJustPressed) {
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
		if (cancelJustPressed) {
			inSkillMenu = false;
			skillMenu.reset();
		}
	}
	else {
		if (upJustPressed)
			menu.moveUp();
		if (downJustPressed)
			menu.moveDown();
		if (confirmJustPressed) {
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

void Combat::targetCharacter(const InputHandler& input) {
	if (input.wasPressed(InputAction::MenuUp)) {
		validTargetIndex= (validTargetIndex - 1 + (int)validTargets.size()) % (int)validTargets.size();
	} 
	if (input.wasPressed(InputAction::MenuDown)) {
		validTargetIndex = (validTargetIndex + 1) % (int)validTargets.size();
	}

	if (input.wasPressed(InputAction::Confirm)) {
		currentAction.target = validTargets[validTargetIndex];
		if (currentAction.type == ActionType::Item && currentAction.item != nullptr) {
			reserveConsumableItem(*currentAction.item);
		}
		actionQueue.push_back(currentAction);
		currentAction = {};
		advanceActivePlayer();
		menu.setLastEnterPressed(true);
	}
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
	if (!skill->getAnimationName().empty()) {
		triggerSkillEffect(skill->getAnimationName(), *target, static_cast<int>(skill->getHits().size()));
	}
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

void Combat::handleQueuedActionMenu(const InputHandler& input) {

	if (input.wasPressed(InputAction::MenuUp)) {
		queuedActionMenu.moveUp();
	}
	if (input.wasPressed(InputAction::MenuDown)) {
		queuedActionMenu.moveDown();
	}
	if (input.wasPressed(InputAction::Confirm)) {
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

bool Combat::loadEffectAnimation(const std::string& animationName) {
	if (effectAssets.count(animationName)) {
		return true;
	}

	AnimationAsset asset;
	if (!AnimationLoader::loadAssetFromFile("animations.json", animationName, asset)) {
		return false;
	}

	sf::Texture texture;
	if (!texture.loadFromFile(asset.texturePath)) {
		return false;
	}

	effectTextures[animationName] = std::move(texture);
	effectAssets[animationName] = std::move(asset);
	return true;
}

void Combat::triggerSkillEffect(const std::string& animationName, Character& target, int instanceCount) {
	if (!loadEffectAnimation(animationName)) {
		return;
	}

	const AnimationAsset& asset = effectAssets[animationName];
	const auto clipIt = asset.clips.find("cast");
	if (clipIt == asset.clips.end()) {
		return;
	}

	const sf::FloatRect targetBounds = target.getGlobalBounds();
	const sf::Vector2f targetCenter(targetBounds.left + targetBounds.width / 2.f, targetBounds.top + targetBounds.height / 2.f);

	const float staggerInterval = 0.12f; // rapid succession delay between each drop
	const int spreadRadius = 16; // horizontal jitter so multiple drops don't stack in a straight line

	for (int i = 0; i < instanceCount; ++i) {
		CombatVisualEffect effect;
		effect.animation.setAnimation(clipIt->second);
		effect.sprite.setTexture(effectTextures[animationName], true);
		effect.sprite.setTextureRect(effect.animation.getCurrentFrame());

		const sf::FloatRect frameBounds(effect.animation.getCurrentFrame());
		effect.sprite.setOrigin(frameBounds.width / 2.f, frameBounds.height / 2.f);

		const float offsetX = instanceCount > 1 ? static_cast<float>(randomRange(-spreadRadius, spreadRadius)) : 0.f;
		effect.targetPosition = { targetCenter.x + offsetX, targetCenter.y };
		effect.startPosition = { effect.targetPosition.x, effect.targetPosition.y - 200.f };
		effect.dropDuration = 0.5f;
		effect.elapsedTime = 0.f;
		effect.delay = i * staggerInterval;
		effect.sprite.setPosition(effect.startPosition);

		skillEffects.push_back(std::move(effect));
	}
}

void Combat::updateSkillEffect(float dt) {
	for (auto& effect : skillEffects) {
		if (effect.delay > 0.f) {
			effect.delay -= dt;
			continue;
		}

		effect.animation.update(dt);
		effect.sprite.setTextureRect(effect.animation.getCurrentFrame());

		effect.elapsedTime += dt;
		const float t = std::min(effect.elapsedTime / effect.dropDuration, 1.0f);
		effect.sprite.setPosition(
			effect.startPosition.x + (effect.targetPosition.x - effect.startPosition.x) * t,
			effect.startPosition.y + (effect.targetPosition.y - effect.startPosition.y) * t);
	}

	skillEffects.erase(
		std::remove_if(skillEffects.begin(), skillEffects.end(),
			[](const CombatVisualEffect& e) { return e.delay <= 0.f && e.elapsedTime >= e.dropDuration; }),
		skillEffects.end());
}

void Combat::drawSkillEffect(sf::RenderTarget& target) {
	for (auto& effect : skillEffects) {
		if (effect.delay <= 0.f) {
			target.draw(effect.sprite);
		}
	}
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
