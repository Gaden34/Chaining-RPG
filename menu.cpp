#include "Menu.h"
#include "item.h"
#include "InputHandler.h"


void Menu::reset() {
		selectedIndex = 0;
		scrollOffset = 0;
		lastUpPressed = false;
		lastDownPressed = false;
		lastRightPressed = false;
		lastLeftPressed = false;
		lastEnterPressed = true;
		lastEscapePressed = false;
}

void Menu::moveUp() {
	if (optionTexts.empty()) return;
	int total = static_cast<int>(optionTexts.size());
	selectedIndex = (selectedIndex - 1 + total) % total;

	if (selectedIndex == total - 1 && total > maxVisibleOptions) {
		scrollOffset = total - maxVisibleOptions;
	} else {
		updateScrollOffset();
	}
}

void Menu::moveDown() {
	if (optionTexts.empty()) return;
	int total = static_cast<int>(optionTexts.size());
	selectedIndex = (selectedIndex + 1) % total;

	if (selectedIndex == 0 && total > maxVisibleOptions) {
		scrollOffset = 0;
	} else {
		updateScrollOffset();
	}
}

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

void Menu::updateScrollOffset() {
	int total = static_cast<int>(optionTexts.size());
	if (total <= maxVisibleOptions) {
		scrollOffset = 0;
		return;
	}

	if (selectedIndex >= scrollOffset + maxVisibleOptions) {
		scrollOffset = selectedIndex - maxVisibleOptions + 1;
	} else if (selectedIndex < scrollOffset) {
		scrollOffset = selectedIndex;
	}
}

ItemMenu::ItemMenu() {
}

void ItemMenu::populate(const Inventory& inventory, float x, float y) {
	optionTexts.clear();
	selectedIndex = 0;
	scrollOffset = 0;
	setPosition(x, y);

	for (const auto& slot : inventory.getItems()) {
		const ItemData* data = ItemDatabase::getItemByID(slot.itemID);
		std::string label = data ? data->name + " x" + std::to_string(slot.quantity) : "Unknown";
		sf::Text text(label, font, 12);
		text.setFillColor(sf::Color::Black);
		optionTexts.push_back(text);
	}
}

void ItemMenu::draw(sf::RenderTarget& target) {
	int total = static_cast<int>(optionTexts.size());
	int endIndex = std::min(scrollOffset + maxVisibleOptions, total); 

	for (int i = scrollOffset; i < endIndex; i++) {
		int visibleSlot = i - scrollOffset;
		optionTexts[i].setPosition(menuX, menuY + visibleSlot * optionSpacing);
		optionTexts[i].setFillColor(i == selectedIndex ? sf::Color::White : sf::Color::Black);
		
		target.draw(optionTexts[i]);
	}
}
