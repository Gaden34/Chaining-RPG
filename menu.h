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
void setMenuX(float xCoord) { menuX = xCoord; }
void setMenuY(float yCoord) { menuY = yCoord; }
void setLastUpPressed(bool pressed) { lastUpPressed = pressed; }
void setLastDownPressed(bool pressed) { lastDownPressed = pressed; }
void setLastRightPressed(bool pressed) { lastRightPressed = pressed; }
void setLastLeftPressed(bool pressed) { lastLeftPressed = pressed; }
void setLastEnterPressed(bool pressed) { lastEnterPressed = pressed; }
void setLastEscapePressed(bool pressed) { lastEscapePressed = pressed; }

};