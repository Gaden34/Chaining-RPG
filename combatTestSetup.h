#pragma once
#include <SFML/Graphics.hpp>
#include "player.h"
#include "messageLog.h"

enum class CombatSetupState {
	ChoosingDiscipline,
	ChoosingLevel,
	Finished
};


class CombatTestSetup {
private:
	int digitCounter = 0;
	int firstDigit = 0;
	std::vector<Player>& party;
	MessageLog& messageLog;

	CombatSetupState currentState = CombatSetupState::ChoosingDiscipline;

	int currentCharacter = 0;


public:
	CombatTestSetup(std::vector<Player>& p, MessageLog& m) : party(p), messageLog(m) {}
	void update();
	void draw(sf::RenderTarget& target);
	void handleEvent(const sf::Event& event);
	void handleDisciplineSelection(const sf::Event& event);
	void handleLevelInput(const sf::Event& event);
	bool isFinished() const { return currentState == CombatSetupState::Finished; }
	void reset() { currentState = CombatSetupState::ChoosingDiscipline; }
};

