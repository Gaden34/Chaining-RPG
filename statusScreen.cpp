#include "statusScreen.h"
#include "player.h"

StatusScreen::StatusScreen() {
    font.loadFromFile("assets/Roboto_Condensed-Black.ttf");
}

void StatusScreen::draw(sf::RenderTarget& target, Player& player) const {
    sf::Text nameText(player.getName(), font, 16);
    nameText.setPosition(50.f, 50.f);

    sf::Text disciplineText("Discipline: " + player.getDiscipline(), font, 12);
    disciplineText.setPosition(50.f, 70.f);

    sf::Text levelText("Level: " + std::to_string(player.getLevel()), font, 12);
    levelText.setPosition(80.f, 70.f);

    sf::Text hpText("HP: " + std::to_string(player.getHp()) + "/" + std::to_string(player.getMaxHp()), font, 10);
    hpText.setPosition(50.f, 90.f);

    sf::Text mpText("MP: " + std::to_string(player.getMp()) + "/" + std::to_string(player.getMaxMp()), font, 10);
    mpText.setPosition(50.f, 90.f + statSpacingY);

    sf::Text attackText("Attack: " + std::to_string(player.getAttack()), font, 10);
    attackText.setPosition(50.f, 90.f + 2 * statSpacingY);

    sf::Text magicAttackText("Magic Attack: " + std::to_string(player.getMagicAttack()), font, 10);
    magicAttackText.setPosition(50.f, 90.f + 3 * statSpacingY);

    sf::Text defenseText("Defense: " + std::to_string(player.getDefense()), font, 10);
    defenseText.setPosition(50.f, 90.f + 4 * statSpacingY);

    sf::Text magicDefenseText("Magic Defense: " + std::to_string(player.getMagicDefense()), font, 10);
    magicDefenseText.setPosition(50.f, 90.f + 5 * statSpacingY);

    target.draw(nameText);
    target.draw(disciplineText);
    target.draw(levelText);
    target.draw(hpText);
    target.draw(mpText);
    target.draw(attackText);
    target.draw(magicAttackText);
    target.draw(defenseText);
    target.draw(magicDefenseText);
}