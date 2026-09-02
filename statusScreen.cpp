#include "statusScreen.h"
#include "player.h"
#include "textUtils.h"

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

std::vector<RenderText>& StatusScreen::buildStatTexts(Player& player) {
	statTexts.clear();
	RenderText nameText;
	nameText.text.setFont(font);
	nameText.text.setString(player.getName());
	nameText.text.setCharacterSize(16);
	nameText.position = sf::Vector2f(50.f, 50.f);
	statTexts.push_back(nameText);
	RenderText disciplineText;
	disciplineText.text.setFont(font);
	disciplineText.text.setString("Discipline: " + player.getDiscipline());
	disciplineText.text.setCharacterSize(12);
	disciplineText.position = sf::Vector2f(50.f, 70.f);
	statTexts.push_back(disciplineText);
	RenderText levelText;
	levelText.text.setFont(font);
	levelText.text.setString("Level: " + std::to_string(player.getLevel()));
	levelText.text.setCharacterSize(12);
	levelText.position = sf::Vector2f(80.f, 70.f);
	statTexts.push_back(levelText);
	RenderText hpText;
	hpText.text.setFont(font);
	hpText.text.setString("HP: " + std::to_string(player.getHp()) + "/" + std::to_string(player.getMaxHp()));
	hpText.text.setCharacterSize(10);
	hpText.position = sf::Vector2f(50.f, 90.f);
	statTexts.push_back(hpText);
	RenderText mpText;
	mpText.text.setFont(font);
	mpText.text.setString("MP: " + std::to_string(player.getMp()) + "/" + std::to_string(player.getMaxMp()));
	mpText.text.setCharacterSize(10);
	mpText.position = sf::Vector2f(50.f, 90.f + statSpacingY);
	statTexts.push_back(mpText);
}