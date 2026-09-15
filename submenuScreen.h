#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "menuScreen.h"
#include "menu.h"
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

class DisciplineSelectMenu : public Menu {
public:
    DisciplineSelectMenu();
    void populate(Player& player);
    void draw(sf::RenderTarget& target, Player& player);
    void changeDiscipline(Player& player);
};

class DisciplineScreen : public MenuScreen {
private:
    sf::Sprite disciplineIconSprite;
    std::vector<sf::Text> disciplineTexts;
    DisciplineSelectMenu disciplineSelectMenu;
    float disciplineSpacingY = 20.f;
    bool inDisciplineSelection = false;

public:
    DisciplineScreen();
    void draw(sf::RenderTarget& target, Player& player);
    bool isInDisciplineSelection() const { return inDisciplineSelection; }
    void setInDisciplineSelection(bool inSelection) { inDisciplineSelection = inSelection; }
    DisciplineSelectMenu& getDisciplineSelectMenu() { return disciplineSelectMenu; }
   // std::vector<sf::Text>& buildDisciplineProfile(Player& player);
    //void setDisciplineIconSprite(Player& player);
};