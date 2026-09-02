#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

struct RenderText;

class Player;

class StatusScreen {
private:
    sf::Font font;
	std::vector<RenderText> statTexts;
    float statSpacingY = 20.f;
    bool statsOutdated = false;

public:
    StatusScreen();
    void draw(sf::RenderTarget& target, Player& player) const;
	std::vector<RenderText>& buildStatTexts(Player& player);
};