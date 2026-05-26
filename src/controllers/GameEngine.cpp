#include "GameEngine.h"
#include "MainScreen.h"
#include "EndingScreen.h"
#include "../views/Renderer.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

GameEngine::GameEngine() : running(true), player("다마고치") {
    std::srand(static_cast<unsigned>(std::time(nullptr))); // 난수 시드 단 한 번만 초기화
    currentScreen = std::make_unique<MainScreen>();
}

void GameEngine::run() {
    currentScreen->render(player);
    InputEvent lastEvent;
    while (running) {
        InputEvent event = inputHandler.pollEvent(); // 100ms 타임아웃
        bool hasInput = (event.type != InputType::NONE);

        if (hasInput) {
            lastEvent = event;
            currentScreen->handleInput(*this, player, event);

            if (player.isEndingCondition()) {
                EndingType et;
                if      (player.getHunger()       <= 0) et = EndingType::STARVE;
                else if (player.getHappiness()    <= 0) et = EndingType::RUNAWAY;
                else if (player.getCleanliness()  <= 0) et = EndingType::TRASH;
                else                                    et = EndingType::HOSPITAL;
                currentScreen = std::make_unique<EndingScreen>(et);
                currentScreen->render(player);
            }
        }

        // 입력이 있거나, 화면이 재렌더를 요청할 때만 렌더 (번쩍임 방지)
        if (running && (hasInput || currentScreen->needsRedraw())) {
            currentScreen->render(player);
            if (hasInput) Renderer::drawDebug(lastEvent);
        }
    }
}

void GameEngine::changeScreen(std::unique_ptr<Screen> nextScreen) {
    currentScreen = std::move(nextScreen);
}

void GameEngine::quit() {
    running = false;
}
