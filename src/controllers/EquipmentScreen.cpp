#include "EquipmentScreen.h"
#include "GameEngine.h"
#include "MainScreen.h"
#include "../views/Renderer.h"
#include <iostream>

void EquipmentScreen::render(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player, "2026-05-04");
    
    // 메인 캐릭터 아스키 아트 높이와 제목 줄을 포함한 공간 확보 (17줄)
    for(int i=0; i<23; ++i) std::cout << std::endl;

    if (!lastAction.empty()) {
        Renderer::drawMessage(lastAction);
    } else {

        std::cout << "\n\n";
    }
    
    Renderer::drawBottomMenu({"검 강화", "갑옷 강화", "아이템 구매", "돌아가기"});
}

void EquipmentScreen::handleInput(GameEngine& engine, Character& player, const InputEvent& event) {
    int choice = -1;
    if (event.type == InputType::KEYBOARD) {
        if (event.key >= '0' && event.key <= '3') choice = event.key - '0';
    } else if (event.type == InputType::MOUSE_PRESS && event.button == 0) {
        if (event.y >= 30) {
            if (event.x >= 1 && event.x <= 30) choice = 1;
            else if (event.x >= 31 && event.x <= 60) choice = 2;
            else if (event.x >= 61 && event.x <= 90) choice = 3;
            else if (event.x >= 91) choice = 0;
        }
    }

    if (choice == 1 || choice == 2) {
        if (player.getMoney() >= 500) {
            player.setMoney(player.getMoney() - 500);
            lastAction = "강화에 성공했습니다! (돈 -500)";
        } else {
            lastAction = "돈이 부족합니다!";
        }
    } else if (choice == 0) {
        engine.changeScreen(std::make_unique<MainScreen>());
    }
}
