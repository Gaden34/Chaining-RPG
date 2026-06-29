#include "player.h"

Player::Player(LevelSystem& l, std::string textureName) : levelSystem(l) {
	texture.loadFromFile("assets/" + textureName + ".png");
	sprite.setTexture(texture);
	moveSpeed = 120.f;
	maxHp = 42;
	hp = maxHp;
	maxMp = 30;
	mp = maxMp;
	attack = 25;
}

void Player::update(float dt) {
	move(dt);
}

void Player::draw(sf::RenderWindow& window) {
	window.draw(sprite);
}

void Player::move(float dt) {
	isMoving = false;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		sprite.move(-moveSpeed * dt, 0.f);
		isMoving = true;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		sprite.move(moveSpeed * dt, 0.f);
		isMoving = true;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		sprite.move(0.f, -moveSpeed * dt);
		isMoving = true;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		sprite.move(0.f, moveSpeed * dt);
		isMoving = true;
	}
}

int Player::getExp() {
	return experience;
}

void Player::addExp(int amount) {
	experience += amount;

	while (experience >= levelSystem.expNeededForLevel(level)) {
		experience -= levelSystem.expNeededForLevel(level);
		incrementLevel();
	}
}

int Player::getLevel() {
	return level;
}

void Player::incrementLevel() {
	level++;
	unlockLevelSkills();
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
	attack = d.getBaseAttack();
	magAttack = d.getBaseMagAttack();
	
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
	// Only unlock skills for Combatant discipline
	if (discipline != DisciplineID::Combatant) return;

	// Level 2: Double Strike
	if (level == 2 && skills.size() == 0) {
		auto doubleStrike = std::make_unique<Skill>(
			"Double Strike",
			"Strike the enemy twice in quick succession",
			5, // mp cost
			2.0f, // cooldown
			SkillType::Attack,
			15 // base damage
		);
		doubleStrike->initializeChaining(1.5f, 25.0f); // 1.5s chain window, 25% damage per chain
		learnSkill(std::move(doubleStrike));
	}

	// Add more level-based skills here as needed
}