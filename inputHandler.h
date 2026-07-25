#pragma once


class Command {
private:

public:
    virtual ~Command() = default;
    virtual void execute() = 0;
};



class InputHandler {
private:
    //Command*


public:
    InputHandler();
    virtual ~InputHandler();

    void handleInput();
};
