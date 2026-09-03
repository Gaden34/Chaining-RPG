#include "statusScreen.h"
#include "player.h"
#include "textUtils.h"

StatusScreen::StatusScreen() {
    font.loadFromFile("assets/Roboto_Condensed-Black.ttf");
	texture.loadFromFile("assets/statusScreen.png");
	sprite.setTexture(texture);
}

void StatusScreen::draw(sf::RenderTarget& target, Player& player) {
	target.draw(sprite);
	for (const auto& renderText : buildStatTexts(player)) {
		target.draw(renderText); 
	}
}

std::vector<sf::Text>& StatusScreen::buildStatTexts(Player& player) {
	if (!player.getStatsOutdated() && !statTexts.empty()) {
		return statTexts;
	}

	statTexts.clear();
	sf::Text nameText = TextUtils::createText(player.getName(), font, 14, sf::Vector2f(50.f, 50.f), sf::Color::Black);
	statTexts.push_back(nameText);
	sf::Text disciplineText = TextUtils::createText("Discipline: " + player.getDiscipline().getName(), font, 12, sf::Vector2f(50.f, 70.f), sf::Color::Black);
	statTexts.push_back(disciplineText);
	sf::Text levelText = TextUtils::createText("Level: " + std::to_string(player.getLevel()), font, 12, sf::Vector2f(80.f, 70.f), sf::Color::Black);
	statTexts.push_back(levelText);
	sf::Text hpText = TextUtils::createText("HP: " + std::to_string(player.getHp()) + "/" + std::to_string(player.getMaxHp()), font, 10, sf::Vector2f(50.f, 90.f), sf::Color::Black);
	statTexts.push_back(hpText);
	sf::Text mpText = TextUtils::createText("MP: " + std::to_string(player.getMp()) + "/" + std::to_string(player.getMaxMp()), font, 10, sf::Vector2f(50.f, 90.f + statSpacingY), sf::Color::Black);
	statTexts.push_back(mpText);
	sf::Text attackText = TextUtils::createText("Attack: " + std::to_string(player.getAttack()), font, 10, sf::Vector2f(50.f, 90.f + 2 * statSpacingY), sf::Color::Black);
	statTexts.push_back(attackText);
	sf::Text magicAttackText = TextUtils::createText("Magic Attack: " + std::to_string(player.getMagAttack()), font, 10, sf::Vector2f(50.f, 90.f + 3 * statSpacingY), sf::Color::Black);
	statTexts.push_back(magicAttackText);
	sf::Text defenseText = TextUtils::createText("Defense: " + std::to_string(player.getDefense()), font, 10, sf::Vector2f(50.f, 90.f + 4 * statSpacingY), sf::Color::Black);
	statTexts.push_back(defenseText);
	sf::Text magicDefenseText = TextUtils::createText("Magic Defense: " + std::to_string(player.getMagDefense()), font, 10, sf::Vector2f(50.f, 90.f + 5 * statSpacingY), sf::Color::Black);
	statTexts.push_back(magicDefenseText);

	player.setStatsOutdated(false);
	return statTexts;
}
