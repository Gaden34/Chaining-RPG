#include <SFML/Graphics.hpp>
#include <iostream>


int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Moving Player");
    window.setFramerateLimit(60);

    const int TILE_SIZE = 40;

    // Player setup
    sf::Texture playerTexture;
    if (!playerTexture.loadFromFile("assets/main_character.png"))
    {
        std::cout << "Failed to load player texture\n";
    }

    sf::Sprite player;
    player.setTexture(playerTexture);
    player.setPosition(40.f, 40.f);
    player.setPosition(40.f, 520.f);

    const float speed = 200.f; // pixels per second
    sf::Clock clock;

    std::vector<std::string> map = {
    "####################",
    "#..................#",
    "#........##........#",
    "#..................#",
    "#.....####.........#",
    "#..................#",
    "#..................#",
    "#..................#",
    "#..................#",
    "#..................#",
    "#..................#",
    "#..................#",
    "#..................#",
    "#..................#",
    "####################"
    };

    while (window.isOpen())
    {
        // --- Event handling ---
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // --- Time since last frame ---
        float dt = clock.restart().asSeconds();

        // --- Movement input ---
        sf::Vector2f movement(0.f, 0.f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            movement.y -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            movement.y += speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            movement.x -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            movement.x += speed;

        // Calculate new potential position
        sf::Vector2f newPos = player.getPosition() + movement * dt;

        // Get the four corners of the player
        sf::FloatRect bounds = player.getGlobalBounds();

        float left = newPos.x;
        float top = newPos.y;
        float right = newPos.x + bounds.width;
        float bottom = newPos.y + bounds.height;

        // Convert to tile indices
        int leftTile = left / TILE_SIZE;
        int rightTile = right / TILE_SIZE;
        int topTile = top / TILE_SIZE;
        int bottomTile = bottom / TILE_SIZE;

        // Check boundaries first
        bool collision = false;

        if (topTile >= 0 && bottomTile < map.size() &&
            leftTile >= 0 && rightTile < map[0].size())
        {
            // Check all four corners
            if (map[topTile][leftTile] == '#' ||
                map[topTile][rightTile] == '#' ||
                map[bottomTile][leftTile] == '#' ||
                map[bottomTile][rightTile] == '#')
            {
                collision = true;
            }
        }
        else
        {
            collision = true; // outside map = block movement
        }

        // Apply movement only if no collision
        if (!collision)
        {
            player.setPosition(newPos);
        }


        // --- Keep player inside window ---
        sf::Vector2f pos = player.getPosition();

        if (pos.x < 0) pos.x = 0;
        if (pos.y < 0) pos.y = 0;

        if (pos.x + bounds.width > window.getSize().x)
            pos.x = window.getSize().x - bounds.width;

        if (pos.y + bounds.height > window.getSize().y)
            pos.y = window.getSize().y - bounds.height;

        player.setPosition(pos);


        // --- Draw ---
        window.clear(sf::Color::Black);
        
        for (int y = 0; y < map.size(); ++y)
        {
            for (int x = 0; x < map[y].size(); ++x)
            {
                sf::RectangleShape tile(sf::Vector2f(TILE_SIZE, TILE_SIZE));
                tile.setPosition(x * TILE_SIZE, y * TILE_SIZE);

                if (map[y][x] == '#')
                    tile.setFillColor(sf::Color(100, 100, 100)); // wall
                else
                    tile.setFillColor(sf::Color(50, 50, 50)); // floor

                window.draw(tile);
            }
        }

        window.draw(player);
        window.display();
    }

    return 0;
}