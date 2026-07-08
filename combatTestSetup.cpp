#include "combatTestSetup.h"





void CombatTestSetup::update()
{
	switch (currentState) {
	case CombatSetupState::ChoosingDiscipline:
		
		break;
	case CombatSetupState::ChoosingLevel:
		// Handle level selection logic
		break;
	case CombatSetupState::Finished:
		// Setup is complete
		break;
	}
}

void CombatTestSetup::handleEvent(const sf::Event& event)
{
	switch (currentState) {
	case CombatSetupState::ChoosingDiscipline:
		handleDisciplineSelection(event);
		break;
	case CombatSetupState::ChoosingLevel:
		handleLevelInput(event);
		break;
	case CombatSetupState::Finished:
		// No further input needed
		break;
	}
}

void CombatTestSetup::handleDisciplineSelection(const sf::Event& event)
{
	messageLog.setCurrentMessage(party[currentCharacter].getName() + ": 1. Mage - 2. Thief - 3. Combatant", sf::Color::Black);

	if (event.type == sf::Event::KeyPressed)
	{
		switch (event.key.code)
		{
		case sf::Keyboard::Num1:
			party[currentCharacter].setDiscipline(DisciplineID::Mage);
			currentState = CombatSetupState::ChoosingLevel;
			break;
		case sf::Keyboard::Num2:
			party[currentCharacter].setDiscipline(DisciplineID::Thief);
			currentState = CombatSetupState::ChoosingLevel;
			break;
		case sf::Keyboard::Num3:
			party[currentCharacter].setDiscipline(DisciplineID::Combatant);
			currentState = CombatSetupState::ChoosingLevel;
			break;
		default:
			break;
		}
		messageLog.addMessage(party[currentCharacter].getName() + "'s discipline: " + party[currentCharacter].getDisicpline().getName(), sf::Color::Black);
		messageLog.addMessage("Level: ", sf::Color::Black);
		currentState = CombatSetupState::ChoosingLevel;
	}

}

void CombatTestSetup::handleLevelInput(const sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed &&
        event.key.code >= sf::Keyboard::Num0 &&
        event.key.code <= sf::Keyboard::Num9)
    {
        int currentDigit = event.key.code - sf::Keyboard::Num0;

        if (digitCounter == 0)
        {
            firstDigit = currentDigit;
            digitCounter++;
        }
        else
        {
            int finalLevel = firstDigit * 10 + currentDigit;
            party[currentCharacter].setLevel(finalLevel);
			messageLog.addMessage(party[currentCharacter].getName() + " is level " + std::to_string(party[currentCharacter].getLevel()), sf::Color::Black);
            digitCounter = 0;
			currentCharacter++;

			if (currentCharacter < party.size()) {
				currentState = CombatSetupState::ChoosingDiscipline; 
				messageLog.addMessage(party[currentCharacter].getName() + ": 1. Mage - 2. Thief - 3. Combatant", sf::Color::Black);
			} else currentState = CombatSetupState::Finished;
        }
    }
}