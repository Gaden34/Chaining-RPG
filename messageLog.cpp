#include "messageLog.h"

MessageLog::MessageLog()
{
	font.loadFromFile("assets/Roboto_Condensed-Black.ttf");

	text.setFont(font);
	text.setCharacterSize(12);
	text.setFillColor(sf::Color::White);
	text.setPosition(16.f, 300.f);

	texture.loadFromFile("assets/messageLog.png");
	sprite.setTexture(texture);
	sprite.setPosition(14.f, 299.f);
}

void MessageLog::addMessage(const std::string& message, const sf::Color& color) {
	messages.push_back(LogMessage({ message, color }));

	if (messages.size() > 5) {
		messages.erase(messages.begin());
	}
}

void MessageLog::setCurrentMessage(const std::string& message, const sf::Color& color) {
	// If the log is completely empty, add a baseline message first
	if (messages.empty()) {
		messages.push_back(LogMessage({ message, color }));
		return;
	}

	// Overwrite the final element in the vector instead of pushing a new one
	messages.back() = LogMessage({ message, color });
}


void MessageLog::draw(sf::RenderTarget& target) {
	float y = 300.f;

	target.draw(sprite);

	for (auto& message : messages) {
		text.setString(message.text);
		text.setPosition(16.f, y);
		text.setFillColor(message.color);

		target.draw(text);

		y += 15.f;
	}

}