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

	case CombatState::SkillMenu:
		handleSkillMenu(input);
		break;

	case CombatState::ItemMenu:
		handleItemMenu(input);
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
		if (skillEffects.empty()) {
			victoryDelayActive = true;
			victoryDelayTimer += dt;
		}
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
	chain.draw(target, font);

switch (currentState) {	
case CombatState::PlayerTurn:
	
	drawActivePlayerName(target);
	menu.draw(target);
	break;

case CombatState::SkillMenu:
	drawActivePlayerName(target);
	skillMenu.draw(target);
	break;

case CombatState::ItemMenu:
	drawActivePlayerName(target);	
	itemMenu.draw(target);
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
	skillEffects.clear();
	// Clear any pending visual/skill state from a previous combat so pointers to old
	// characters don't get used when a new battle starts.
	pendingSkillActions.clear();
	activeAnimations.clear();
	animationTimer = 0.f;
	nextEffectID = 0;
	menu.reset();
	itemMenu.reset();
	enemies.clear();

	for (const EnemySpawn& spawn : encounter) {
		if (spawn.data == nullptr || spawn.count <= 0) {
			continue; // Skip invalid spawns
		}
		for (int i = 0; i < spawn.count; ++i) {
			enemies.emplace_back(*spawn.data);
			// assign a stable instance id so queued actions can later resolve
			enemies.back().setInstanceId(nextEnemyInstanceId++);
		}
	}

	if (enemies.empty()) return;


	for (int i = 0; i < party.size(); i++) {
		party[i].setBattleSpritePosition(155.f, 180.f + i * 55);
		party[i].setStatsOutdated(true);
	}

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

	if (input.wasPressed(InputAction::MenuLeft)) {
			activePlayerIndex = (activePlayerIndex - 1 + (int)party.size()) % (int)party.size();
			menu.reset();
		}
	if (input.wasPressed(InputAction::MenuRight)) {
			activePlayerIndex = (activePlayerIndex + 1) % (int)party.size();
			menu.reset();
		}

	if (input.wasPressed(InputAction::Cancel)) {
		CombatState returnState = CombatState::PlayerTurn;

		if (currentAction.type == ActionType::Skill) {
			returnState = CombatState::SkillMenu;
		} else if (currentAction.type == ActionType::Item) {
			returnState = CombatState::ItemMenu;
		}
		currentAction = {};
		currentState = returnState;
	}
	

	if (playerActed[activePlayerIndex]) return;

	/*if (inItemMenu) {
		if (input.wasPressed(InputAction::MenuUp))
			itemMenu.moveUp();
		if (input.wasPressed(InputAction::MenuDown))
			itemMenu.moveDown();
		if (input.wasPressed(InputAction::Confirm)) {
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
					beginTargeting();
					}
				}
			}
		}
		if (input.wasPressed(InputAction::Cancel)) {
			inItemMenu = false;
			itemMenu.reset();
		}
	}
	else if (inSkillMenu) {
		if (input.wasPressed(InputAction::MenuUp))
			skillMenu.moveUp();
		if (input.wasPressed(InputAction::MenuDown))
			skillMenu.moveDown();
		if (input.wasPressed(InputAction::Confirm)) {
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
		if (input.wasPressed(InputAction::Cancel)) {
			inSkillMenu = false;
			skillMenu.reset();
		}
	}*/
	else {
		if (input.wasPressed(InputAction::MenuUp))
			menu.moveUp();
		if (input.wasPressed(InputAction::MenuDown))
			menu.moveDown();
		if (input.wasPressed(InputAction::Confirm)) {
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
				currentState = CombatState::SkillMenu;
				break;
			case CombatMenu::MenuOption::Item:
				itemMenu.populate(party[activePlayerIndex].getInventory(), 352.f, 300.f, 4);
				itemMenu.setLastEnterPressed(true);
				currentState = CombatState::ItemMenu;
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

void Combat::handleSkillMenu(const InputHandler& input) {
	if (input.wasPressed(InputAction::MenuUp))
		skillMenu.moveUp();
	if (input.wasPressed(InputAction::MenuDown))
		skillMenu.moveDown();
	if (input.wasPressed(InputAction::Confirm)) {
		int index = skillMenu.getSelectedIndex();
		if (index == -1) {
			currentState = CombatState::PlayerTurn;
			skillMenu.reset();
		}
		else {
			Skill* selectedSkill = party[activePlayerIndex].getSkillByIndex(index);
			if (selectedSkill == nullptr) {
				return;
			}

			if (party[activePlayerIndex].getMp() < selectedSkill->getMpCost()) {
				messageLog.addMessage("Not enough MP!", sf::Color::Red);
				return;
			}

			currentAction = {};
			currentAction.type = ActionType::Skill;
			currentAction.actor = &party[activePlayerIndex];
			currentAction.skill = selectedSkill;
			beginTargeting();
		}
	}
	if (input.wasPressed(InputAction::Cancel)) {
		currentState = CombatState::PlayerTurn;
		skillMenu.reset();
	}
}

void Combat::handleItemMenu(const InputHandler& input) {
	if (input.wasPressed(InputAction::MenuUp))
		itemMenu.moveUp();
	if (input.wasPressed(InputAction::MenuDown))
		itemMenu.moveDown();
	if (input.wasPressed(InputAction::Confirm)) {
		const auto& slots = party[activePlayerIndex].getInventory().getItems();
		int index = itemMenu.getSelectedIndex();
		if (!slots.empty() && index >= 0 && index < (int)slots.size()) {
			const ItemData* item = ItemDatabase::getItemByID(slots[index].itemID);
			if (item) {
				if (!canQueueConsumableItem(*item, party[activePlayerIndex].getInventory())) {
					messageLog.addMessage("No more " + item->name + " left to queue.", sf::Color::Red);
				}
				else {
					currentAction = {};
					currentAction.type = ActionType::Item;
					currentAction.actor = &party[activePlayerIndex];
					currentAction.item = const_cast<ItemData*>(item);
					beginTargeting();
				}
			}
		}
	}
	if (input.wasPressed(InputAction::Cancel)) {
		currentState = CombatState::PlayerTurn;
		itemMenu.reset();
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
		// Store a stable id for this target so we can re-resolve it later if
		// the enemies vector gets reallocated or elements move.
		if (currentAction.target) {
			currentAction.targetInstanceId = currentAction.target->getInstanceId();
		}
		
		if (currentAction.type == ActionType::Item && currentAction.item != nullptr) {
			reserveConsumableItem(*currentAction.item);
		}
		actionQueue.push_back(currentAction);
		currentAction = {};
		advanceActivePlayer();
		menu.setLastEnterPressed(true);
	}
	if (input.wasPressed(InputAction::Cancel)) {
		if (currentState == CombatState::ItemMenu) {
			releaseConsumableItemReservation(*currentAction.item);
		} else {
			menu.setLastEscapePressed(true);
		}
		currentAction = {};
		currentState = CombatState::PlayerTurn;
		menu.setLastEscapePressed(true);
	}
}

void Combat::drawTargetPointer(sf::RenderTarget& target) {
	sf::FloatRect bounds = validTargets[validTargetIndex]->getGlobalBounds();
	float x = bounds.left + bounds.width / 2.f - pointerSprite.getGlobalBounds().width / 2.f;
	float y = bounds.top - pointerSprite.getGlobalBounds().height - 4.f;
	pointerSprite.setPosition(x, y);
	target.draw(pointerSprite);
}

void Combat::drawActivePlayerName(sf::RenderTarget& target) {
	sf::Text nameLabel(party[activePlayerIndex].getName(), font, 12);
	nameLabel.setPosition(320.f, 288.f);
	nameLabel.setFillColor(sf::Color::White);
	target.draw(nameLabel);
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
	chain.registerHit({ActionType::Attack, nullptr});
	std::cout << "Chain count: " << chain.getChainCount() << std::endl;
	messageLog.addMessage(player->getName() + " hits the " + TextUtils::lowerFirst(target->getName()) + " for " + std::to_string(finalDamage) + " damage!", sf::Color::Black);

	handleDeath(*target);
}

void Combat::initiateSkill(QueuedAction& action) {
	Player* player = static_cast<Player*>(action.actor);
	Skill* skill = action.skill;
	if (player == nullptr || skill == nullptr || action.target == nullptr) {
		return;
	}
	if (player->getMp() < skill->getMpCost()) {
		messageLog.addMessage("Not enough MP!", sf::Color::Red);
		return;
	}
	player->setMp(player->getMp() - skill->getMpCost());

	if (skill && !skill->getScreenEffectName().empty()) {
		int screenEffectID = triggerScreenEffect(skill->getScreenEffectName());
		if (screenEffectID != -1) {
			pendingSkillActions.push_back({ action, screenEffectID, PendingSkillPhase::ScreenEffect });
			return;
		}
	}


	if (!skill->getAnimationName().empty()) {
		int effectID = triggerSkillEffect(skill->getAnimationName(), *action.target, static_cast<int>(skill->getHits().size()));
		if (effectID != -1) {
			pendingSkillActions.push_back({ action, effectID, PendingSkillPhase::SkillAnimation });
			return;
		}
	}

	resolveSkill(action);
}

void Combat::resolveSkill(QueuedAction& action) {
	Player* player = static_cast<Player*>(action.actor);
	Skill* skill = action.skill;
	Character* target = action.target;
	if (player == nullptr || skill == nullptr || target == nullptr) {
		return;
	}

	//calculateSkillDamage(skill, player, target);
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
		// Register a skill hit in the chain system. Use the skill pointer as the key so
		// different skills don't chain together mistakenly.
		chain.registerHit({ ActionType::Skill, static_cast<const void*>(skill) });
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

int Combat::applySkillHit(Skill* skill, Character* actor, Character* target) {

	chain.registerHit({ ActionType::Skill, static_cast<const void*>(skill) });
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

	damage = randomRange(static_cast<int>(damage * 0.95f), static_cast<int>(damage * 1.05f));
	damage = damage * chain.getDamagePercent() / 100.0f;
	const int finalDamage = static_cast<int>(std::round(damage));
	target->takeDamage(finalDamage);
	return finalDamage;
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
		initiateSkill(action);
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
			// Ensure target pointer is resolved to a current enemy object. The enemies
			// vector may have been reallocated/moved since the action was queued, so
			// prefer resolving by the stored instance id if available.
			if ((actionQueue[selectedIndex].target == nullptr || actionQueue[selectedIndex].target->getInstanceId() != actionQueue[selectedIndex].targetInstanceId) &&
				actionQueue[selectedIndex].targetInstanceId >= 0) {
				for (auto& enemy : enemies) {
					if (enemy.getInstanceId() == actionQueue[selectedIndex].targetInstanceId) {
						actionQueue[selectedIndex].target = &enemy;
						break;
					}
				}
			}

			if (actionQueue[selectedIndex].target == nullptr || !actionQueue[selectedIndex].target->isAlive()) {
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

	if (actionQueue.empty() && pendingSkillActions.empty()) {
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
			itemMenu.reset();
			currentState = CombatState::PlayerTurn;
			return;
		}
	}

	playerActed.assign(party.size(), false);
	activePlayerIndex = 0;
	menu.reset();
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

int Combat::triggerEffect(const std::string& animationName, Character* caster, Character* target, int instanceCount) {
	if (!loadEffectAnimation(animationName)) {
		return -1;
	}

	const AnimationAsset& asset = effectAssets[animationName];
	const auto clipIt = asset.clips.find("cast");
	if (clipIt == asset.clips.end()) {
		return -1;
	}
	const AnimationClip& clip = clipIt->second;

	sf::Vector2f anchorCenter{ 0.f, 0.f };
	if (clip.anchor == EffectAnchor::Target && target) {
		const sf::FloatRect targetBounds = target->getGlobalBounds();
		anchorCenter = sf::Vector2f(targetBounds.left + targetBounds.width / 2.f, targetBounds.top + targetBounds.height / 2.f);
	} else if (clip.anchor == EffectAnchor::Caster && caster) {
		const sf::FloatRect casterBounds = caster->getGlobalBounds();
		anchorCenter = sf::Vector2f(casterBounds.left + casterBounds.width / 2.f, casterBounds.top + casterBounds.height / 2.f);
	}

	const int effectID = nextEffectID++;
	const float staggerInterval = 0.12f; // rapid succession delay between each drop
	const int spreadRadius = 8; // horizontal jitter so multiple drops don't stack in a straight line

	for (int i = 0; i < instanceCount; ++i) {
		CombatVisualEffect effect;
		effect.effectID = effectID;
		// copy the clip into the animation (frames & loop)
		effect.anchor = clip.anchor;
		effect.animation.setAnimation(clip);
		effect.sprite.setTexture(effectTextures[animationName], true);
		effect.sprite.setTextureRect(effect.animation.getCurrentFrame());
		// determine origin based on clip metadata if present
		const sf::IntRect currentRect = effect.animation.getCurrentFrame();

		if (clip.anchor == EffectAnchor::Screen) {
			effect.dropDuration = clip.displayDuration > 0.0f ? clip.displayDuration : 0.2f;
			effect.startPosition = effect.targetPosition = { 0.f, 0.f };
		} else {
		if (clip.originX >= 0.0f && clip.originY >= 0.0f) {
			effect.sprite.setOrigin(clip.originX, clip.originY);
		} else {
			effect.sprite.setOrigin(currentRect.width / 2.f, currentRect.height / 2.f);
		}

		const float offsetX = instanceCount > 1 ? static_cast<float>(randomRange(-spreadRadius, spreadRadius)) : 0.f;
		effect.targetPosition = { anchorCenter.x + offsetX, anchorCenter.y };

		if (clip.instant) {
			// draw immediately at target and disappear after displayDuration
			effect.startPosition = effect.targetPosition;
			effect.dropDuration = clip.displayDuration > 0.0f ? clip.displayDuration : 0.08f;
		} else {
			effect.startPosition = { effect.targetPosition.x - 200.f, effect.targetPosition.y - 200.f };
			effect.dropDuration = 0.7f;
		}
			effect.sprite.setPosition(effect.startPosition);
	}
		effect.elapsedTime = 0.f;
		effect.delay = i * staggerInterval;

		skillEffects.push_back(std::move(effect));
	}

	return effectID;
}

int Combat::triggerSkillEffect(const std::string& animationName, Character& target, int instanceCount) {
	// target should be passed as the target parameter to triggerEffect (caster == nullptr)
	return triggerEffect(animationName, nullptr, &target, instanceCount);
}

int Combat::triggerScreenEffect(const std::string& animationName) {
	return triggerEffect(animationName, nullptr, nullptr, 1);
}

bool Combat::animationFinished(int effectID) const {
	for (const auto& effect : skillEffects) {
		if (effect.effectID == effectID) {
			return false;
		}
	}
	return true;
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

        if (effect.anchor != EffectAnchor::Screen) {
            const float t = std::min(effect.elapsedTime / effect.dropDuration, 1.0f);
            effect.sprite.setPosition(
                effect.startPosition.x + (effect.targetPosition.x - effect.startPosition.x) * t,
                effect.startPosition.y + (effect.targetPosition.y - effect.startPosition.y) * t);
        }
    }

	std::erase_if(skillEffects, [](const CombatVisualEffect& e) {
		return e.delay <= 0.f && e.elapsedTime >= e.dropDuration;
		});
		
	// Advance pending skills from their screen effect to their skill animation,
	// then resolve damage only after the final animation has finished.
	if (!pendingSkillActions.empty()) {
		for (int i = static_cast<int>(pendingSkillActions.size()) - 1; i >= 0; --i) {
			PendingSkillAction& pending = pendingSkillActions[i];
			pending.elapsedTime += dt;

			const auto& hitTimes = pending.action.skill ? pending.action.skill->getHitTimes() : std::vector<float>();

			// Apply any scheduled hits based on elapsed time (timing-driven approach).
			std::cout << "Skill: " << pending.action.skill->getName()
				<< " hitTimes: " << hitTimes.size()
				<< " elapsed: " << pending.elapsedTime
				<< '\n';

			while (pending.appliedHits < static_cast<int>(hitTimes.size()) && pending.elapsedTime >= hitTimes[pending.appliedHits]) {
				// Resolve the target in case the original pointer is stale.
				QueuedAction action = pending.action;
				if ((action.target == nullptr || action.target->getInstanceId() != action.targetInstanceId) &&
					action.targetInstanceId >= 0) {
					for (auto& enemy : enemies) {
						if (enemy.getInstanceId() == action.targetInstanceId) {
							action.target = &enemy;
							break;
						}
					}
				}

				if (action.target == nullptr || !action.target->isAlive()) {
					action.target = nullptr;
					for (auto& enemy : enemies) {
						if (enemy.isAlive()) {
							action.target = &enemy;
							break;
						}
					}
				}

				// Apply one hit for this scheduled time
				if (action.target) {
					int dmg = applySkillHit(pending.action.skill, pending.action.actor, action.target);
					pending.totalDamage += dmg;
					++pending.appliedHits;
				} else {
					// No valid target to apply hit to; still count the hit as applied to progress the timing
					++pending.appliedHits;
				}
			}

			// If the effect animation (screen or skill) has not finished, wait.
			if (!animationFinished(pending.effectID)) continue;

			QueuedAction action = pending.action;
			Skill* skill = action.skill;

			// Resolve target by instance id if available. The copied QueuedAction may
			// contain a stale pointer if enemies moved; prefer resolving using
			// targetInstanceId recorded when the action was queued.
			if ((action.target == nullptr || action.target->getInstanceId() != action.targetInstanceId) &&
				action.targetInstanceId >= 0) {
				for (auto& enemy : enemies) {
					if (enemy.getInstanceId() == action.targetInstanceId) {
						action.target = &enemy;
						break;
					}
				}
			}

			// If the resolved target is dead or missing, fall back to first alive enemy.
			if (action.target == nullptr || !action.target->isAlive()) {
				action.target = nullptr;
				for (auto& enemy : enemies) {
					if (enemy.isAlive()) {
						action.target = &enemy;
						break;
					}
				}
			}

			if (pending.phase == PendingSkillPhase::ScreenEffect) {
				if (skill && !skill->getAnimationName().empty() && action.target) {
					int skillEffectID = triggerSkillEffect(
						skill->getAnimationName(),
						*action.target,
						static_cast<int>(skill->getHits().size()));
					if (skillEffectID != -1) {
						pendingSkillActions[i].effectID = skillEffectID;
						pendingSkillActions[i].phase = PendingSkillPhase::SkillAnimation;
						continue;
					}
				}

				// If we already applied individual hits via timing, perform post-hit effects
				if (pending.appliedHits > 0) {
					// Use resolved action.target
					if (action.target) {
						Character* actor = pending.action.actor;
						messageLog.addMessage(actor->getName() + " uses " + skill->getName() + " on the " + TextUtils::lowerFirst(action.target->getName()) + " for " + std::to_string(pending.totalDamage) + " damage!", sf::Color::Black);
						handleSteal(skill, actor, action.target);
						handleDeath(*action.target);
					}
				} else {
					resolveSkill(action);
				}
			} else {
				// Skill animation finished; if hits were applied by timing, finalize, otherwise fallback
				if (pending.appliedHits > 0) {
					if (action.target) {
						Character* actor = pending.action.actor;
						messageLog.addMessage(actor->getName() + " uses " + skill->getName() + " on the " + TextUtils::lowerFirst(action.target->getName()) + " for " + std::to_string(pending.totalDamage) + " damage!", sf::Color::Black);
						handleSteal(skill, actor, action.target);
						handleDeath(*action.target);
					}
				} else {
					resolveSkill(action);
				}
			}

			// Remove the pending entry
			pendingSkillActions.erase(pendingSkillActions.begin() + i);
		}
	}
}

void Combat::drawSkillEffect(sf::RenderTarget& target) {
    for (auto& effect : skillEffects) {
        if (effect.delay > 0.f) continue;

		if (effect.anchor == EffectAnchor::Screen) {
			// Use the current frame size (texture rect) when computing origin and scale.
			// The texture may be an atlas, and using the full texture size will cause
			// the visible rect (single frame) to cover only a portion of the view.
			const sf::IntRect rect = effect.animation.getCurrentFrame();
			const sf::Vector2f rectSize(static_cast<float>(rect.width), static_cast<float>(rect.height));
			const sf::View& view = target.getView();

			if (rect.width > 0 && rect.height > 0) {
				effect.sprite.setOrigin(rectSize.x / 2.f, rectSize.y / 2.f);
				effect.sprite.setPosition(view.getCenter());
				effect.sprite.setScale(view.getSize().x / rectSize.x, view.getSize().y / rectSize.y);
			} else {
				// Fallback to whole texture size if frame rect is invalid
				const sf::Vector2u textureSize = effect.sprite.getTexture()->getSize();
				effect.sprite.setOrigin(textureSize.x / 2.f, textureSize.y / 2.f);
				effect.sprite.setPosition(view.getCenter());
				if (textureSize.x > 0 && textureSize.y > 0)
					effect.sprite.setScale(view.getSize().x / textureSize.x, view.getSize().y / textureSize.y);
			}

			const float t = effect.dropDuration > 0.f ? std::min(effect.elapsedTime / effect.dropDuration, 1.0f) : 1.0f;
			sf::Color c = effect.sprite.getColor();
			c.a = static_cast<sf::Uint8>(255.f * (1.f - t));
			effect.sprite.setColor(c);
		}

        target.draw(effect.sprite);
    }
}

bool Combat::isVictoryDisplayComplete() { 
	if (victoryDelayTimer >= 1.0f) {
		victoryDelayTimer = 0.f;
		victoryDelayActive = false;
		return true;
	}
	return false;
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
			encounter.push_back({ &mandaro, 1 });
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


CombatMenu::CombatMenu() {
	// Use base Menu font and set default position/spacing for this menu
	setPosition(320.f, 300.f);
	setOptionSpacing(12.f);

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


QueuedActionMenu::QueuedActionMenu() {
	// Use base Menu font and set default position/spacing for this menu
	setPosition(320.f, 300.f);
	setOptionSpacing(12.f);
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

