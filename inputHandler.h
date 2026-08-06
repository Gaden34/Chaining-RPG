#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <cstddef>
#include <memory>
#include "menu.h"

class Command {
private:

public:
    virtual ~Command() = default;
    virtual void execute() = 0;
};

enum class InputAction : std::size_t {
    MoveLeft = 0,
    MoveRight,
    MoveUp,
    MoveDown,
    MenuLeft,
    MenuRight,
    MenuUp,
    MenuDown,
    Confirm,
    Cancel,
    Count
};

class InputHandler {
private:
    static constexpr std::size_t KeyCount = static_cast<std::size_t>(sf::Keyboard::KeyCount);

    std::array<bool, KeyCount> currentKeys{};
    std::array<bool, KeyCount> previousKeys{};

    std::array<sf::Keyboard::Key, static_cast<std::size_t>(InputAction::Count)> bindings{};
    std::array<std::unique_ptr<Command>, static_cast<std::size_t>(InputAction::Count)> pressedCommands{};

    static bool isValidKey(sf::Keyboard::Key key);
    static std::size_t keyToIndex(sf::Keyboard::Key key);
    static std::size_t actionToIndex(InputAction action);


public:
    InputHandler();
    ~InputHandler() = default;

    // Call once per frame to update input transitions.
    void update();

    // Raw key queries.
    bool isDown(sf::Keyboard::Key key) const;
    bool wasPressed(sf::Keyboard::Key key) const;
    bool wasReleased(sf::Keyboard::Key key) const;

    // Action binding and action queries.
    void bind(InputAction action, sf::Keyboard::Key key);
    sf::Keyboard::Key getBinding(InputAction action) const;
    bool isDown(InputAction action) const;
    bool wasPressed(InputAction action) const;
    bool wasReleased(InputAction action) const;

    // Optional command binding. Commands execute when an action is pressed.
    void bindPressedCommand(InputAction action, std::unique_ptr<Command> command);
    void clearPressedCommand(InputAction action);
    void processCommands();

    // Backward-compatible alias for existing skeleton API.
    void handleInput();

    // Helper to apply default controls that match current project input.
    void setDefaultBindings();
};

/*class MenuLeftCommand : public Command {
private:
    Menu& menu;

public:
    explicit MenuLeftCommand(Menu& m) : menu(m) {}
    void execute() override { menu.moveLeft(); }
};

class MenuRightCommand : public Command {
private:
    Menu& menu;

public: 
    explicit MenuRightCommand(Menu& m) : menu(m) {}
    void execute() override { menu.moveRight(); }
};*/

class MenuUpCommand : public Command {
private:
    Menu& menu;

public: 
    explicit MenuUpCommand(Menu& m) : menu(m) {}
    void execute() override { menu.moveUp(); }
};

class MenuDownCommand : public Command {
private:
    Menu& menu;

public: 
    explicit MenuDownCommand(Menu& m) : menu(m) {}
    void execute() override { menu.moveDown(); }
};