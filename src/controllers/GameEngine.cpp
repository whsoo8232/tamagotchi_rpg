#include "GameEngine.h"
#include "MainScreen.h"
#include "EndingScreen.h"
#include "../views/Renderer.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

GameEngine::GameEngine() : running(true), player("다마고치") {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    setvbuf(stdout, nullptr, _IOFBF, 65536); // stdout 완전 버퍼링 (프레임 단위 flush)
    std::cout << "\033[?25l\033[2J\033[H" << std::flush; // 커서 숨김 + 최초 화면 클리어
    currentScreen = std::make_unique<MainScreen>();
}

GameEngine::~GameEngine() {
    std::cout << "\033[?25h" << std::flush; // 커서 복원
}

void GameEngine::run() {
    currentScreen->render(player);
    std::cout << std::flush;
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
                std::cout << std::flush;
            }
        }

        // 입력이 있거나, 화면이 재렌더를 요청할 때만 렌더
        if (running && (hasInput || currentScreen->needsRedraw())) {
            currentScreen->render(player);
            if (hasInput) Renderer::drawDebug(lastEvent);
            std::cout << std::flush; // 프레임 완성 후 한 번에 출력
        }
    }
}

void GameEngine::changeScreen(std::unique_ptr<Screen> nextScreen) {
    currentScreen = std::move(nextScreen);
}

void GameEngine::quit() {
    running = false;
}
