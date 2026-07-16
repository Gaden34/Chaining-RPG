#pragma once
#include <array>

struct ChainBonus {
	int hitsRequired;
	int damagePercent;
};


class ChainSystem
{
private:
	int chainCount = 0;
	bool chainActive = false;
	float chainTimer = 0.f;

	static const std::array<ChainBonus, 4> chainBonuses;

public:
	void update(float dt);
	void registerHit();
	void reset() { chainCount = 0; }
	int getChainCount() const { return chainCount; }
	bool isChainActive() const { return chainActive; }
	int getDamagePercent() const;
};

