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
    void populate(Player& player);
    void draw(sf::RenderTarget& target, Player& player);
    void changeDiscipline(Player& player);
};

class DisciplineScreen : public MenuScreen {
private:
    sf::Sprite disciplineIconSprite;
    std::vector<sf::Text> disciplineTexts;
    DisciplineSelectMenu disciplineSelectMenu;
    SkillMenu skillMenu;
    sf::Text skillDescriptionText;
    float disciplineSpacingY = 20.f;
    bool inDisciplineSelection = false;
    bool skillListFocused = false;

    void buildSkillDescription(Player& player);

public:
    DisciplineScreen();
    void draw(sf::RenderTarget& target, Player& player);
    bool isInDisciplineSelection() const { return inDisciplineSelection; }
    void setInDisciplineSelection(bool inSelection) { inDisciplineSelection = inSelection; setNeedsRebuild(true); }
    DisciplineSelectMenu& getDisciplineSelectMenu() { return disciplineSelectMenu; }
    SkillMenu& getSkillMenu() { return skillMenu; }
    bool isSkillListFocused() const { return skillListFocused; }
    void setSkillListFocused(bool focused) { skillListFocused = focused; setNeedsRebuild(true); }
    std::vector<sf::Text>& buildDisciplineProfile(Player& player);
    void setDisciplineIconSprite(Player& player);
};