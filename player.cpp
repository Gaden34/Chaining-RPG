#include "player.h"
#include "skillDatabase.h"

Player::Player(MessageLog& m, std::string n, std::string textureName) : messageLog(m) {
	name = n;
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
