#pragma once
#include <vector>
#include <iostream>
#include <SFML/Graphics.hpp>

class Menu {
private:
   
    bool lastUpPressed = false;
    bool lastDownPressed = false;
    bool lastRightPressed = false;
    bool lastLeftPressed = false;
    bool lastEnterPressed = false;
    bool lastEscapePressed = false;

protected:
    sf::Font font;
	std::vector<sf::Text> optionTexts;
	int selectedIndex = 0;


public:
	Menu() {
		if (!font.loadFromFile("assets/Roboto_Condensed-Black.ttf")) {
			std::cerr << "Failed to load font!" << std::endl;
		}
	}
	virtual ~Menu() = default;
	void reset() {
		selectedIndex = 0;
		lastUpPressed = false;
		lastDownPressed = false;
		lastRightPressed = false;
		lastLeftPressed = false;
		lastEnterPressed = true;
		lastEscapePressed = false;
}
    void moveUp() { if (optionTexts.empty()) return;
    selectedIndex = (selectedIndex - 1 + static_cast<int>(optionTexts.size())) % static_cast<int>(optionTexts.size()); }
	void moveDown() { if (optionTexts.empty()) return;
    selectedIndex = (selectedIndex + 1) % static_cast<int>(optionTexts.size()); }
	int getSelectedIndex() const { return selectedIndex; }
    bool getLastUpPressed() const { return lastUpPressed; }
    bool getLastDownPressed() const { return lastDownPressed; }
    bool getLastRightPressed() const { return lastRightPressed; }
    bool getLastLeftPressed() const { return lastLeftPressed; }
    bool getLastEnterPressed() const { return lastEnterPressed; }
    bool getLastEscapePressed() const { return lastEscapePressed; }
    void setLastUpPressed(bool pressed) { lastUpPressed = pressed; }
    void setLastDownPressed(bool pressed) { lastDownPressed = pressed; }
    void setLastRightPressed(bool pressed) { lastRightPressed = pressed; }
    void setLastLeftPressed(bool pressed) { lastLeftPressed = pressed; }
    void setLastEnterPressed(bool pressed) { lastEnterPressed = pressed; }
    void setLastEscapePressed(bool pressed) { lastEscapePressed = pressed; }

};