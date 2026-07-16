#include "chainSystem.h"


const std::array<ChainBonus, 4> ChainSystem::chainBonuses = {{
	{ 2, 115 },
	{ 5, 120 },
	{ 10, 130 },
	{ 15, 135 }
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

void ChainSystem::registerHit() {
	if (chainActive) {
		chainCount++;
	}
	else chainCount = 1;

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