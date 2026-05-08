#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <memory>
#include <string>
#include "../models/Character.h"
#include "Screen.h"
#include "InputHandler.h"

class GameEngine {
public:
    GameEngine();
    void run();
    void changeScreen(std::unique_ptr<Screen> nextScreen);
    void quit();
    
    std::string getDate() const { return "2026-05-04"; }

private:
    bool running;
    Character player;
    std::unique_ptr<Screen> currentScreen;
    InputHandler inputHandler;
};

#endif
