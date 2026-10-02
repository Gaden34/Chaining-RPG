#pragma once
#include <array>
#include <vector>
#include <SFML/Graphics.hpp>

// Define the action types used by the chain system. Placing this here avoids a
// circular include between combat.h and chainSystem.h.
enum class ActionType {
	None,
	Attack,
	Skill,
	Item,
	Defend
};

struct ChainBonus {
	int hitsRequired;
	int damagePercent;
};

struct ActionIdentity {
	ActionType type = ActionType::None;
	const void* key = nullptr;

	bool operator==(const ActionIdentity& other) const {
		return type == other.type && key == other.key;
	}
};


class ChainSystem
{
private:
	int chainCount = 0;
	bool chainActive = false;
	float chainTimer = 0.f;
	ActionIdentity lastAction;


	static const std::array<ChainBonus, 4> chainBonuses;

public:
	void update(float dt);
	void draw(sf::RenderTarget& target, const sf::Font& font) const;
	void registerHit(const ActionIdentity& action);
	void reset() { chainCount = 0; }
	int getChainCount() const { return chainCount; }
	bool isChainActive() const { return chainActive; }
	int getDamagePercent() const;
};

