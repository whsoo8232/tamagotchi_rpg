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
    ~GameEngine();
    void run();
    void changeScreen(std::unique_ptr<Screen> nextScreen);
    void quit();
    
    // 날짜는 Renderer::getCurrentDate() 사용

private:
    bool running;
    Character player;
    std::unique_ptr<Screen> currentScreen;
    InputHandler inputHandler;
};

#endif
