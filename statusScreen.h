#pragma once
#include <SFML/Graphics.hpp>

class Player;

class StatusScreen {
private:
    sf::Font font;
    float statSpacingY = 20.f;

public:
    StatusScreen();
    void draw(sf::RenderTarget& target, Player& player) const;
};