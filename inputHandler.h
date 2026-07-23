#pragma once


class Command {
private:

public:
    virtual ~Command()
    virtual void execute();
}



class InputHandler {
private:
    Command*


public:
    InputHandler();
    virtual ~InputHandler();

    void handleInput();
};
