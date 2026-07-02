#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

Class Menu {
private:
    sf::Font font;
    std::vector<Text> optionTexts;
    int selectedIndex = 0;
    const float menuX;
    const float menuY;
    bool lastUpPressed = false;
    bool lastDownPressed = false;
    bool lastRightPressed = false;
    bool lastLeftPressed = false;
    bool lastEnterPressed = false;
    bool lastEscapePressed = false;


public:

void reset();
bool getLastUpPressed() const { return lastUpPressed; }
bool getLastDownPressed() const { return lastDownPressed; }
bool getLastRightPressed() const { return lastRightPressed; }
bool getLastLeftPressed() const { return lastLeftPressed; }
bool getLastEnterPressed() const { return lastEnterPressed; }
bool getLastEscapePressed() const { return lastEscapePressed; }
void setMenuX(xCoord) { menuX = xCoord; }

};