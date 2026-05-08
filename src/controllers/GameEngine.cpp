#include "GameEngine.h"
#include "MainScreen.h"
#include "../views/Renderer.h"
#include <iostream>

GameEngine::GameEngine() : running(true), player("다마고치") {
    currentScreen = std::make_unique<MainScreen>();
}

void GameEngine::run() {
    currentScreen->render(player);
    InputEvent lastEvent;
    while (running) {
        InputEvent event = inputHandler.pollEvent();
        if (event.type != InputType::NONE) {
            lastEvent = event;
            currentScreen->handleInput(*this, player, event);
            
            if (player.isEndingCondition()) {
                std::cout << "게임 오버! 다마고치가 살기 힘든 환경이 되었습니다." << std::endl;
                running = false;
                break;
            }
            
            if (running) {
                currentScreen->render(player);
                Renderer::drawDebug(lastEvent);
            }
        }
    }
}

void GameEngine::changeScreen(std::unique_ptr<Screen> nextScreen) {
    currentScreen = std::move(nextScreen);
}

void GameEngine::quit() {
    running = false;
}
