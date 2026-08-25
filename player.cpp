#include "player.h"
#include "skillDatabase.h"

namespace {
	bool loadWalkFrontAnimation(Animation& animation, sf::Texture& texture) {
		AnimationAsset asset;
		if (!AnimationLoader::loadAssetFromFile("animations.json", "player", asset)) {
			return false;
		}

		if (!texture.loadFromFile(asset.texturePath)) {
			return false;
		}

		const auto clipIt = asset.clips.find("walk_front");
		if (clipIt == asset.clips.end()) {
			return false;
		}

		animation.setAnimation(clipIt->second);
		return true;
	}

	AnimationClip makeFallbackWalkFrontClip() {
		SpriteSheetGridSpec spec;
		spec.totalFrames = 10;
		spec.columns = 4;
		spec.frameWidth = 32;
		spec.frameHeight = 32;
		spec.frameDuration = 0.1f;
		return buildClipFromGrid(spec, true);
	}

}

namespace 
{ 
const AxisInput horizontalAxisInput{
		InputAction::MoveLeft, TransitionDirection::Left,
		InputAction::MoveRight, TransitionDirection::Right
	};
const AxisInput verticalAxisInput = {
	InputAction::MoveUp, TransitionDirection::Up,
	InputAction::MoveDown, TransitionDirection::Down
	}; 
}

Player::Player(MessageLog& m, std::string n, std::string textureName, Inventory& inv) : messageLog(m), partyInventory(inv) {
	name = n;
	texture.loadFromFile("assets/" + textureName + ".png");
	sprite.setTexture(texture);
	moveSpeed = 120.f;
	maxHp = 42;
	hp = maxHp;
	maxMp = 30;
	mp = maxMp;
	attack = 25;


	if (!loadWalkFrontAnimation(walkAnimation, walkTexture)) {
		walkTexture.loadFromFile("assets/spikyWalkFront-Sheet.png");
		walkAnimation.setAnimation(makeFallbackWalkFrontClip());
	}
	
}


void Player::update(float dt, InputHandler& inputHandler, const Map& map) {
	move(dt, inputHandler, map);
	if (isMoving) {
		walkAnimation.update(dt);
	}
}

void Player::drawExploring(sf::RenderTarget& target) {
	sprite.setTexture(walkTexture);
	
	if (isMoving) {
		sprite.setTextureRect(walkAnimation.getCurrentFrame());
		}

	target.draw(sprite);

}

void Player::drawCombat(sf::RenderTarget& target) {
	sprite.setTexture(texture, true);
	target.draw(sprite);
}

void Player::move(float dt, const Map& map) {
	isMoving = false;

	sf::Vector2f movement(0.f, 0.f);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		movement.x -= moveSpeed * dt;
		isMoving = true;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		movement.x += moveSpeed * dt;
		isMoving = true;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		movement.y -= moveSpeed * dt;
		isMoving = true;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		movement.y += moveSpeed * dt;
		isMoving = true;
	}

	// Check for collisions before moving
	sf::Vector2f newPosition = sprite.getPosition() + movement;

	sf::FloatRect collisionBox = getCollisionBox(newPosition);

	bool blocked = map.isBlocked(static_cast<int>(collisionBox.left), static_cast<int>(collisionBox.top)) ||
		map.isBlocked(static_cast<int>(collisionBox.left + collisionBox.width), static_cast<int>(collisionBox.top)) ||
		map.isBlocked(static_cast<int>(collisionBox.left), static_cast<int>(collisionBox.top + collisionBox.height)) ||
		map.isBlocked(static_cast<int>(collisionBox.left + collisionBox.width), static_cast<int>(collisionBox.top + collisionBox.height));
	if (!blocked) {
		sprite.move(movement);
	}
}

void Player::move(float dt, InputHandler& inputHandler, const Map& map) {
	isMoving = false;


	resolveAxis(horizontalDirection, inputHandler, horizontalAxisInput);
	resolveAxis(verticalDirection, inputHandler, verticalAxisInput);

	sf::Vector2f movement(0.f, 0.f);

	if (horizontalDirection == TransitionDirection::Left) {
		movement.x -= moveSpeed * dt;
	}

	else if (horizontalDirection == TransitionDirection::Right) {
		movement.x += moveSpeed * dt;
	}

	if (verticalDirection == TransitionDirection::Up) {
		movement.y -= moveSpeed * dt;
	}

	else if (verticalDirection == TransitionDirection::Down) {
		movement.y += moveSpeed * dt;
	}

	if (horizontalDirection || verticalDirection) {
		isMoving = true;
	}

	// Check for collisions before moving
	sf::Vector2f newPosition = sprite.getPosition() + movement;

	sf::FloatRect collisionBox = getCollisionBox(newPosition);

	bool blocked = map.isBlocked(static_cast<int>(collisionBox.left), static_cast<int>(collisionBox.top)) ||
		map.isBlocked(static_cast<int>(collisionBox.left + collisionBox.width), static_cast<int>(collisionBox.top)) ||
		map.isBlocked(static_cast<int>(collisionBox.left), static_cast<int>(collisionBox.top + collisionBox.height)) ||
		map.isBlocked(static_cast<int>(collisionBox.left + collisionBox.width), static_cast<int>(collisionBox.top + collisionBox.height)) ||
		map.canTriggerTransition(collisionBox, *map.getTransitionAtPosition(collisionBox, horizontalDirection, verticalDirection), horizontalDirection ? *horizontalDirection : verticalDirection ? *verticalDirection : TransitionDirection::Down);
	if (!blocked) {
		sprite.move(movement);
	}
}

void Player::resolveAxis(std::optional<TransitionDirection>& axis, InputHandler& input, const AxisInput& axisInput) {
	bool negDown = input.isDown(axisInput.negativeAction);
	bool posDown = input.isDown(axisInput.positiveAction);

	if (input.wasPressed(axisInput.negativeAction)) axis = axisInput.negativeDirection;
	if (input.wasPressed(axisInput.positiveAction)) axis = axisInput.positiveDirection;
	
	if (axis == axisInput.negativeDirection && !negDown) axis = posDown ? std::optional(axisInput.positiveDirection) : std::nullopt;
	if (axis == axisInput.positiveDirection && !posDown) axis = negDown ? std::optional(axisInput.negativeDirection) : std::nullopt;

	if (!axis) {
		if (negDown) axis = axisInput.negativeDirection;
		else if (posDown) axis = axisInput.positiveDirection;
	}
}

int Player::getExp() {
	return experience;
}

void Player::addExp(int amount) {
	experience += amount;

	while (experience >= expNeededForNextLevel(level)) {
		experience -= expNeededForNextLevel(level);
		levelUp();
	}
}

int Player::expNeededForNextLevel(int level) {
	return (level - 1) * 27 + 50;
}

int Player::getLevel() {
	return level;
}

void Player::levelUp() {
	level++;
	unlockLevelSkills();
	maxHp += getDiscipline().getStatGrowth().healthGrowth;
	maxMp += getDiscipline().getStatGrowth().mpGrowth;
	attack += getDiscipline().getStatGrowth().attackGrowth;
	magAttack += getDiscipline().getStatGrowth().magAttackGrowth;
	hp = maxHp;
	mp = maxMp;
	messageLog.addMessage(name + " has reached level " + std::to_string(level) + "!", sf::Color::Black);
}

void Player::setLevelFromTest(int targetLevel) {
	for (int i = 1; i < targetLevel; ++i) {
		levelUp();
	}
}

bool Player::getIsMoving() {
	return isMoving;
}

Discipline& Player::getDiscipline() {
	return Disciplines::getDisciplineFromID(discipline);
}

void Player::setDiscipline(DisciplineID id) {
	discipline = id;
	Discipline& d = Disciplines::getDisciplineFromID(id);

	maxHp = d.getBaseHealth();
	hp = maxHp;
	maxMp = d.getBaseMp();
	mp = maxMp;
	attack = d.getBaseAttack();
	magAttack = d.getBaseMagAttack();
	unlockLevelSkills();
	
}

void Player::addCharacter(char c) {
	Character::name += c;
}

void Player::removeLastCharacter() {
	if (!name.empty()) name.pop_back();
}

void Player::learnSkill(std::unique_ptr<Skill> skill) {
	skills.push_back(std::move(skill));
}

const std::vector<std::unique_ptr<Skill>>& Player::getSkills() const {
	return skills;
}

Skill* Player::getSkillByIndex(int index) {
	if (index >= 0 && index < skills.size()) {
		return skills[index].get();
	}
	return nullptr;
}

void Player::unlockLevelSkills() {
    // Convert your DisciplineID enum to a string matching the JSON keys
    std::string disciplineStr = getDiscipline().getName(); 

    // Fetch whatever skills are waiting for this exact milestone
    auto newSkills = SkillDatabase::getSkillsForLevel(disciplineStr, level);

    for (auto& skill : newSkills) {
        if (!hasSkill(skill->getName())) {
            learnSkill(std::move(skill));
        }
    }
}

bool Player::hasSkill(const std::string& skillName) const {
	for (const auto& skill : skills) {
		if (skill->getName() == skillName) {
			return true;
		}
	}
	return false;
}

sf::FloatRect Player::getCollisionBox(sf::Vector2f position) const {
	return sf::FloatRect(position.x + 12.f, position.y + 28.f, 9.f, 4.f);
}