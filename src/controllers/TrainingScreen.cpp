#include "TrainingScreen.h"
#include "GameEngine.h"
#include "MainScreen.h"
#include "PunchingBagScreen.h"
#include "RunningScreen.h"
#include "ReactionScreen.h"
#include "../views/Renderer.h"
#include <iostream>

void TrainingScreen::render(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player, "2026-05-04");

    std::cout << std::endl;
    Renderer::drawMainCharacter();

    if (!lastAction.empty()) {
        Renderer::drawMessage(lastAction);
    } else {
        std::cout << "\n\n";
    }

    Renderer::drawBottomMenu({"샌드백치기 (1)", "달리기 (2)", "반응속도 (3)", "돌아가기 (0)"});
}

void TrainingScreen::handleInput(GameEngine& engine, Character& player, const InputEvent& event) {
    int choice = -1;

    if (event.type == InputType::KEYBOARD) {
        if (event.key == '1') choice = 1;
        else if (event.key == '2') choice = 2;
        else if (event.key == '3') choice = 3;
        else if (event.key == '0' || event.key == 'q') choice = 0;
    } else if (event.type == InputType::MOUSE_PRESS && event.button == 0) {
        if (event.y >= 30) {
            if (event.x >= 1 && event.x <= 30) choice = 1;
            else if (event.x >= 31 && event.x <= 60) choice = 2;
            else if (event.x >= 61 && event.x <= 90) choice = 3;
            else if (event.x >= 91) choice = 0;
        }
    }

    if (choice == 1) {
        engine.changeScreen(std::make_unique<PunchingBagScreen>());
    } else if (choice == 2) {
        engine.changeScreen(std::make_unique<RunningScreen>());
    } else if (choice == 3) {
        engine.changeScreen(std::make_unique<ReactionScreen>());
    } else if (choice == 0) {
        engine.changeScreen(std::make_unique<MainScreen>());
    }
}
