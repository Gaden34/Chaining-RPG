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
	std::vector<RenderText> statTexts;
    float statSpacingY = 20.f;
    bool statsOutdated = false;

public:
    StatusScreen();
    void draw(sf::RenderTarget& target, Player& player);
    std::vector<RenderText>& buildStatTexts(Player& player);
};