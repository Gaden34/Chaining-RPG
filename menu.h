#pragma once
#include <vector>
#include <iostream>
#include <SFML/Graphics.hpp>

class InputHandler;
class Inventory;

class Menu {
private:
   
    bool lastUpPressed = false;
    bool lastDownPressed = false;
    bool lastRightPressed = false;
    bool lastLeftPressed = false;
    bool lastEnterPressed = true;
    bool lastEscapePressed = false;

protected:
    sf::Font font;
	std::vector<sf::Text> optionTexts;
	int selectedIndex = 0;

	int scrollOffset = 0;
	int maxVisibleOptions = 4;
	// Common position and spacing for menus. Subclasses can override via setPosition / setOptionSpacing
	float menuX = 320.f;
	float menuY = 300.f;
	float optionSpacing = 12.f;

public:
	Menu() {
		if (!font.loadFromFile("assets/Roboto_Condensed-Black.ttf")) {
			std::cerr << "Failed to load font!" << std::endl;
		}
	}
	virtual ~Menu() = default;
	void reset();
	void moveUp();
	void moveDown();
	virtual void handleInput(InputHandler& inputHandler);
	void updateScrollOffset();
	virtual void onSelect() {}
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
	int getScrollOffset() const { return scrollOffset; }
	int getMaxVisibleOptions() const { return maxVisibleOptions; }

	void setPosition(float x, float y) { menuX = x; menuY = y; }
	void setOptionSpacing(float spacing) { optionSpacing = spacing; }

};

// Shared inventory list menu, reused by Combat and Exploration.
class ItemMenu : public Menu {
public:
	ItemMenu();
	void populate(const Inventory& inventory, float x, float y);
	void draw(sf::RenderTarget& target);
};