#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "menuScreen.h"
#include "textUtils.h"

class Player;

class StatusScreen : public MenuScreen {
private:
    sf::Sprite portraitSprite;
	std::vector<sf::Text> statTexts;
    float statSpacingY = 20.f;

public:
    StatusScreen();
    void draw(sf::RenderTarget& target, Player& player);
    std::vector<sf::Text>& buildPlayerProfile(Player& player);
    void setPortraitSprite(Player& player);
};