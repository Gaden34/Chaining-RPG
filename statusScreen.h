#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "textUtils.h"

class Player;

class StatusScreen {
private:
    sf::Font font;
    sf::Sprite sprite;
    sf::Texture texture;
	std::vector<sf::Text> statTexts;
    float statSpacingY = 20.f;
    bool statsOutdated = false;

public:
    StatusScreen();
    void draw(sf::RenderTarget& target, Player& player);
    std::vector<sf::Text>& buildStatTexts(Player& player);
};