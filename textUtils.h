#pragma once
#include <string>
#include <SFML/Graphics.hpp>

struct RenderText {
	sf::Text text;
	sf::Vector2f position;
};

namespace TextUtils {
	std::string capitalizeFirst(const std::string& text);
	std::string lowerFirst(const std::string& text);
	std::string articleFor(const std::string& text);
}

