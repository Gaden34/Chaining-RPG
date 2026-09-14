#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// Base class for full-screen menu overlays (status, discipline, equipment, etc.)
// that share a font and a background sprite plus a rebuild-on-demand flag.
class MenuScreen {
protected:
    sf::Font font;
    sf::Texture texture;
    sf::Sprite sprite;
    bool needsRebuild = true;

public:
    MenuScreen(const std::string& fontPath, const std::string& backgroundTexturePath);
    virtual ~MenuScreen() = default;

    void setNeedsRebuild(bool rebuild) { needsRebuild = rebuild; }
    bool getNeedsRebuild() const { return needsRebuild; }
};
