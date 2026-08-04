#include "inputHandler.h"

InputHandler::InputHandler() {
	bindings.fill(sf::Keyboard::Unknown);
	setDefaultBindings();
}

bool InputHandler::isValidKey(sf::Keyboard::Key key) {
	return key >= 0 && key < sf::Keyboard::KeyCount;
}

std::size_t InputHandler::keyToIndex(sf::Keyboard::Key key) {
	return static_cast<std::size_t>(key);
}

std::size_t InputHandler::actionToIndex(InputAction action) {
	return static_cast<std::size_t>(action);
}

void InputHandler::update() {
	previousKeys = currentKeys;

	for (std::size_t i = 0; i < KeyCount; ++i) {
		currentKeys[i] = sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(i));
	}
}

bool InputHandler::isDown(sf::Keyboard::Key key) const {
	if (!isValidKey(key)) {
		return false;
	}

	return currentKeys[keyToIndex(key)];
}

bool InputHandler::wasPressed(sf::Keyboard::Key key) const {
	if (!isValidKey(key)) {
		return false;
	}

	const std::size_t index = keyToIndex(key);
	return currentKeys[index] && !previousKeys[index];
}

bool InputHandler::wasReleased(sf::Keyboard::Key key) const {
	if (!isValidKey(key)) {
		return false;
	}

	const std::size_t index = keyToIndex(key);
	return !currentKeys[index] && previousKeys[index];
}

void InputHandler::bind(InputAction action, sf::Keyboard::Key key) {
	if (!isValidKey(key)) {
		return;
	}

	bindings[actionToIndex(action)] = key;
}

sf::Keyboard::Key InputHandler::getBinding(InputAction action) const {
	return bindings[actionToIndex(action)];
}

bool InputHandler::isDown(InputAction action) const {
	return isDown(getBinding(action));
}

bool InputHandler::wasPressed(InputAction action) const {
	return wasPressed(getBinding(action));
}

bool InputHandler::wasReleased(InputAction action) const {
	return wasReleased(getBinding(action));
}

void InputHandler::bindPressedCommand(InputAction action, std::unique_ptr<Command> command) {
	pressedCommands[actionToIndex(action)] = std::move(command);
}

void InputHandler::clearPressedCommand(InputAction action) {
	pressedCommands[actionToIndex(action)].reset();
}

void InputHandler::processCommands() {
	for (std::size_t i = 0; i < static_cast<std::size_t>(InputAction::Count); ++i) {
		const InputAction action = static_cast<InputAction>(i);
		Command* command = pressedCommands[i].get();

		if (command != nullptr && wasPressed(action)) {
			command->execute();
		}
	}
}

void InputHandler::handleInput() {
	update();
	processCommands();
}

void InputHandler::setDefaultBindings() {
	bind(InputAction::MoveLeft, sf::Keyboard::A);
	bind(InputAction::MoveRight, sf::Keyboard::D);
	bind(InputAction::MoveUp, sf::Keyboard::W);
	bind(InputAction::MoveDown, sf::Keyboard::S);

	bind(InputAction::MenuLeft, sf::Keyboard::Left);
	bind(InputAction::MenuRight, sf::Keyboard::Right);
	bind(InputAction::MenuUp, sf::Keyboard::Up);
	bind(InputAction::MenuDown, sf::Keyboard::Down);

	bind(InputAction::Confirm, sf::Keyboard::Enter);
	bind(InputAction::Cancel, sf::Keyboard::Escape);
}

