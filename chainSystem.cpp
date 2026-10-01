#include "chainSystem.h"


const std::array<ChainBonus, 4> ChainSystem::chainBonuses = {{
	{ 2, 120 },
	{ 5, 130 },
	{ 10, 140 },
	{ 15, 150 }
}};

void ChainSystem::update(float dt) {
	if (!chainActive) return;
	
	chainTimer += dt;

	if (chainTimer >= 0.2f) {
		chainCount = 0;
		chainActive = false;
		chainTimer = 0.f;
	}
}

void ChainSystem::draw(sf::RenderTarget& target, const sf::Font& font) const {
	
	if (chainActive) {
		sf::Text chainText;
		chainText.setFont(font);
		chainText.setCharacterSize(14);
		chainText.setFillColor(sf::Color::White);
		chainText.setString("Chain: " + std::to_string(chainCount));
		chainText.setPosition(300.f, 40.f);
		target.draw(chainText);
	}
}

void ChainSystem::registerHit(const ActionIdentity& action) {
	if (chainActive) {
		if (!(action == lastAction)) {
			chainCount++;
		}
	} else {
		chainCount = 1;
	}
	
	lastAction = action;
	chainActive = true;
	chainTimer = 0.f;
}

int ChainSystem::getDamagePercent() const {
	int damagePercent = 100;
	for (const auto& bonus : chainBonuses) {
		if (chainCount >= bonus.hitsRequired) {
			damagePercent = bonus.damagePercent;
		}
		else break;
	}
	return damagePercent;
}