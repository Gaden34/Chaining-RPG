#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

struct LogMessage {
	std::string text;
	sf::Color color;
};


class MessageLog {
private:
	sf::Font font;
	sf::Text text;
	sf::Texture texture;
	sf::Sprite sprite;

	std::vector<LogMessage> messages;

public:
	MessageLog();
	void addMessage(const std::string& message, const sf::Color& color);
	void setCurrentMessage(const std::string& message, const sf::Color& color);
	void draw(sf::RenderWindow& window);

};
