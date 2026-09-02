#include "statusScreen.h"
#include "player.h"
#include "textUtils.h"

StatusScreen::StatusScreen() {
    font.loadFromFile("assets/Roboto_Condensed-Black.ttf");
}

void StatusScreen::draw(sf::RenderTarget& target, Player& player) const {
  
    for (const auto& renderText : buildStatTexts(player)) {
        target.draw(renderText.text);
    }
}

std::vector<RenderText>& StatusScreen::buildStatTexts(Player& player) {
	if (!player.getStatsOutdated() && !statTexts.empty()) {
		return statTexts;
	}

	statTexts.clear();
	RenderText nameText(player.getName(), font, 14, sf::Vector2f(50.f, 50.f));
	statTexts.push_back(nameText);
	RenderText disciplineText("Discipline: " + player.getDiscipline(), font, 12, sf::Vector2f(50.f, 70.f));
	statTexts.push_back(disciplineText);
	RenderText levelText("Level: " + std::to_string(player.getLevel()), font, 12, sf::Vector2f(80.f, 70.f));
	statTexts.push_back(levelText);
	RenderText hpText("HP: " + std::to_string(player.getHp()) + "/" + std::to_string(player.getMaxHp()), font, 10, sf::Vector2f(50.f, 90.f));
	statTexts.push_back(hpText);
	RenderText mpText("MP: " + std::to_string(player.getMp()) + "/" + std::to_string(player.getMaxMp()), font, 10, sf::Vector2f(50.f, 90.f + statSpacingY));
	statTexts.push_back(mpText);
	RenderText attackText("Attack: " + std::to_string(player.getAttack()), font, 10, sf::Vector2f(50.f, 90.f + 2 * statSpacingY));
	statTexts.push_back(attackText);
	RenderText magicAttackText("Magic Attack: " + std::to_string(player.getMagAttack()), font, 10, sf::Vector2f(50.f, 90.f + 3 * statSpacingY));
	statTexts.push_back(magicAttackText);
	RenderText defenseText("Defense: " + std::to_string(player.getDefense()), font, 10, sf::Vector2f(50.f, 90.f + 4 * statSpacingY));
	statTexts.push_back(defenseText);
	RenderText magicDefenseText("Magic Defense: " + std::to_string(player.getMagDefense()), font, 10, sf::Vector2f(50.f, 90.f + 5 * statSpacingY));
	statTexts.push_back(magicDefenseText);

	player.setStatsOutdated(false);
	return statTexts;
}
