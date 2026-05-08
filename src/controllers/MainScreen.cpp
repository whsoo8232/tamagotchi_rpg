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
    Renderer::drawTopMenu(player, "2026-05-04");
    
    std::cout << std::endl; // 상단 여백
    Renderer::drawMainCharacter(); // 16줄
    
    // 메시지 영역 (2줄 공간 확보)
    std::cout << "\n\n";
    
    Renderer::drawBottomMenu({"육성", "전투", "장비", "훈련"});
}

void MainScreen::handleInput(GameEngine& engine, Character& player, const InputEvent& event) {
    int choice = -1;

    if (event.type == InputType::KEYBOARD) {
        if (event.key >= '1' && event.key <= '4') {
            choice = event.key - '0';
        } else if (event.key == 'q' || event.key == 27) {
            engine.quit();
        }
    } else if (event.type == InputType::MOUSE_PRESS && event.button == 0) {
        // 하단 메뉴바 영역 (Y=30~34)으로 제한
        if (event.y >= 30) { 
            // 4개 버튼 기준 (각 약 30칸)
            if (event.x >= 1 && event.x <= 30) choice = 1;
            else if (event.x >= 31 && event.x <= 60) choice = 2;
            else if (event.x >= 61 && event.x <= 90) choice = 3;
            else if (event.x >= 91) choice = 4;
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
