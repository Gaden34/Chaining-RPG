#include "Menu.h"
#include "item.h"
#include "InputHandler.h"


void Menu::handleInput(InputHandler& inputHandler) {
	if (inputHandler.wasPressed(InputAction::MenuUp)) {
		moveUp();
	}
	if (inputHandler.wasPressed(InputAction::MenuDown)) {
		moveDown();
	}
	if (inputHandler.wasPressed(InputAction::Confirm)) {
		onSelect();
	}
}

ItemMenu::ItemMenu() {
	if (!font.loadFromFile("assets/Roboto_Condensed-Black.ttf")) {
		std::cerr << "Failed to load font!" << std::endl;
	}
}

void ItemMenu::populate(const Inventory& inventory, float x, float y) {
	optionTexts.clear();
	selectedIndex = 0;

	for (const auto& slot : inventory.getItems()) {
		const ItemData* data = ItemDatabase::getItemByID(slot.itemID);
		std::string label = data ? data->name + " x" + std::to_string(slot.quantity) : "Unknown";
		sf::Text text(label, font, 12);
		text.setPosition(x, y + optionTexts.size() * optionSpacing);
		text.setFillColor(sf::Color::Black);
		optionTexts.push_back(text);
	}
}

void ItemMenu::draw(sf::RenderTarget& target) {
	for (int i = 0; i < (int)optionTexts.size(); i++) {
		optionTexts[i].setFillColor(i == selectedIndex ? sf::Color::White : sf::Color::Black);
		target.draw(optionTexts[i]);
	}
}
