#include "MainScreen.h"
#include "GameEngine.h"
#include "RaisingScreen.h"
#include "TrainingScreen.h"
#include "BattleScreen.h"
#include "EquipmentScreen.h"
#include "../views/Renderer.h"
#include <iostream>
#include <string>

void MainScreen::render(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);
    
    std::cout << std::endl; // 상단 여백
    Renderer::drawMainCharacter(); // 16줄
    
    // 메시지 영역 (2줄 공간 확보)
    std::cout << "\n\n";
    
    Renderer::drawBottomMenu({"육성", "전투", "장비", "훈련"});
}

void MainScreen::handleInput(GameEngine& engine, Character& player, const InputEvent& event) {
    int choice = -1;

    if (event.type == InputType::MOUSE_PRESS && event.button == 0) {
        if (event.y >= 30) {
            int btnW = 118 / 4;
            int btn  = (event.x - 1) / btnW + 1;
            if (btn >= 1 && btn <= 4) choice = btn;
        }
    }

    if (choice == 1) {
        engine.changeScreen(std::make_unique<RaisingScreen>());
    } else if (choice == 2) {
        engine.changeScreen(std::make_unique<BattleScreen>());
    } else if (choice == 3) {
        engine.changeScreen(std::make_unique<EquipmentScreen>());
    } else if (choice == 4) {
        engine.changeScreen(std::make_unique<TrainingScreen>());
    }
}
