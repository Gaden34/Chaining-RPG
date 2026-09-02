#pragma once
#include <string>
#include <SFML/Graphics.hpp>

struct RenderText {
	sf::Text text;
	sf::Vector2f position;

	RenderText(const std::string& str, const sf::Font& font, unsigned int characterSize, const sf::Vector2f& pos)
		: text(str, font, characterSize), position(pos) {}
};

namespace TextUtils {
	std::string capitalizeFirst(const std::string& text);
	std::string lowerFirst(const std::string& text);
	std::string articleFor(const std::string& text);
}

