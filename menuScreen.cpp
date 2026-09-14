#include "menuScreen.h"

MenuScreen::MenuScreen(const std::string& fontPath, const std::string& backgroundTexturePath) {
    font.loadFromFile(fontPath);
    texture.loadFromFile(backgroundTexturePath);
    sprite.setTexture(texture);
}
