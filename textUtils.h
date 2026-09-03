#pragma once
#include <string>
#include <SFML/Graphics.hpp>

namespace TextUtils {
	std::string capitalizeFirst(const std::string& text);
	std::string lowerFirst(const std::string& text);
	std::string articleFor(const std::string& text);
	sf::Text createText(const std::string& str, const sf::Font& font, unsigned int characterSize, const sf::Vector2f& position, const sf::Color& color);
}

