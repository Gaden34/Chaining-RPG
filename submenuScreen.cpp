#include "submenuScreen.h"
#include "player.h"
#include "textUtils.h"

StatusScreen::StatusScreen() : MenuScreen("assets/Roboto_Condensed-Black.ttf", "assets/statusScreen.png") {
}

void StatusScreen::draw(sf::RenderTarget& target, Player& player) {
	target.draw(sprite);
	target.draw(portraitSprite);
	
	for (const auto& renderText : buildPlayerProfile(player)) {
		target.draw(renderText); 
	}
}

std::vector<sf::Text>& StatusScreen::buildPlayerProfile(Player& player) {
	if (!player.getStatsOutdated() && !statTexts.empty() && !needsRebuild) {
		return statTexts;
	}

	setPortraitSprite(player);

	statTexts.clear();
	sf::Text nameText = TextUtils::createText(player.getName(), font, 14, sf::Vector2f(90.f, 22.f), sf::Color::Black);
	statTexts.push_back(nameText);
	sf::Text disciplineText = TextUtils::createText("Discipline: " + player.getDiscipline().getName(), font, 12, sf::Vector2f(90.f, 42.f), sf::Color::Black);
	statTexts.push_back(disciplineText);
	sf::Text levelText = TextUtils::createText("Level: " + std::to_string(player.getLevel()), font, 12, sf::Vector2f(90.f, 62.f), sf::Color::Black);
	statTexts.push_back(levelText);
	sf::Text hpText = TextUtils::createText("HP: " + std::to_string(player.getHp()) + "/" + std::to_string(player.getMaxHp()), font, 10, sf::Vector2f(20.f, 92.f), sf::Color::Black);
	statTexts.push_back(hpText);
	sf::Text mpText = TextUtils::createText("MP: " + std::to_string(player.getMp()) + "/" + std::to_string(player.getMaxMp()), font, 10, sf::Vector2f(20.f, 92.f + statSpacingY), sf::Color::Black);
	statTexts.push_back(mpText);
	sf::Text attackText = TextUtils::createText("Attack: " + std::to_string(player.getAttack()), font, 10, sf::Vector2f(20.f, 92.f + 2 * statSpacingY), sf::Color::Black);
	statTexts.push_back(attackText);
	sf::Text magicAttackText = TextUtils::createText("Magic Attack: " + std::to_string(player.getMagAttack()), font, 10, sf::Vector2f(20.f, 92.f + 3 * statSpacingY), sf::Color::Black);
	statTexts.push_back(magicAttackText);
	sf::Text defenseText = TextUtils::createText("Defense: " + std::to_string(player.getDefense()), font, 10, sf::Vector2f(20.f, 92.f + 4 * statSpacingY), sf::Color::Black);
	statTexts.push_back(defenseText);
	sf::Text magicDefenseText = TextUtils::createText("Magic Defense: " + std::to_string(player.getMagDefense()), font, 10, sf::Vector2f(20.f, 92.f + 5 * statSpacingY), sf::Color::Black);
	statTexts.push_back(magicDefenseText);

	needsRebuild = false;
	player.setStatsOutdated(false);
	return statTexts;
}

void StatusScreen::setPortraitSprite(Player& player) {
	portraitSprite = player.getPortraitSprite();
	sf::FloatRect bounds = portraitSprite.getGlobalBounds();
	portraitSprite.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
	portraitSprite.setPosition(52.f, 52.f);
}

void DisciplineSelectMenu::populate(Player& player) {
	optionTexts.clear();
	reset();
	//setPosition(menuX, menuY);

	for (const auto& disciplineID : player.getUnlockedDisciplines()) {
		std::string label = Disciplines::getDisciplineFromID(disciplineID).getName();
		sf::Text text(label, font, 12);
		text.setFillColor(sf::Color::Black);
		optionTexts.push_back(text);
	}
}

void DisciplineSelectMenu::draw(sf::RenderTarget& target, Player& player) {
	int total = static_cast<int>(optionTexts.size());
	int endIndex = std::min(scrollOffset + maxVisibleOptions, total);

	for (int i = scrollOffset; i < endIndex; ++i) {
		int visibleSlot = i - scrollOffset;
		optionTexts[i].setPosition(menuX, menuY + visibleSlot * optionSpacing);
		optionTexts[i].setFillColor(i == selectedIndex ? sf::Color::White : sf::Color::Black);
		target.draw(optionTexts[i]);
	}
}

DisciplineScreen::DisciplineScreen() : MenuScreen("assets/Roboto_Condensed-Black.ttf", "assets/statusScreen.png") {}

void DisciplineScreen::draw(sf::RenderTarget& target, Player& player) {
	target.draw(sprite);

	/*for (const auto& renderText : buildDisciplineProfile(player)) {
		target.draw(renderText); 
	}*/

	if (inDisciplineSelection) {
		disciplineSelectMenu.draw(target, player);
	}
}

/*std::vector<sf::Text>& DisciplineScreen::buildDisciplineProfile(Player& player) {
	if (!player.getStatsOutdated() && !disciplineTexts.empty() && !needsRebuild) {
		return disciplineTexts;
	}

	setDisciplineIconSprite(player);

	disciplineTexts.clear();
	sf::Text disciplineText = TextUtils::createText("Discipline: " + player.getDiscipline().getName(), font, 12, sf::Vector2f(90.f, 42.f), sf::Color::Black);
	disciplineTexts.push_back(disciplineText);

	needsRebuild = false;
	player.setStatsOutdated(false);
	return disciplineTexts;
}*/

/*void DisciplineScreen::setDisciplineIconSprite(Player& player) {
	disciplineIconSprite = player.getDisciplineIconSprite();
	sf::FloatRect bounds = disciplineIconSprite.getGlobalBounds();
	disciplineIconSprite.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
	disciplineIconSprite.setPosition(52.f, 52.f);
}*/
