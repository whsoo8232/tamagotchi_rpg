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
    Renderer::drawTopMenu(player);

    Renderer::drawMainCharacter();   // 2+32+2 = 36행 = CONTENT_H

    if (!lastAction.empty()) {
        Renderer::drawMessage(lastAction);  // 2줄 (\n + >> msg\n)
        Renderer::fillContent(38);           // 36+2=38 (fillContent가 음수면 0줄 추가)
    } else {
        Renderer::fillContent(36);           // 36행 사용 → 패딩 없음
    }

    Renderer::drawBottomMenu({"샌드백치기", "달리기", "반응속도", "돌아가기"});
}

void TrainingScreen::handleInput(GameEngine& engine, Character& player, const InputEvent& event) {
    int choice = -1;

    if (event.type == InputType::MOUSE_PRESS && event.button == 0) {
        if (event.y >= 41) {
            int btnW = 158 / 4;
            int btn  = (event.x - 1) / btnW;
            if (btn == 0) choice = 1;
            else if (btn == 1) choice = 2;
            else if (btn == 2) choice = 3;
            else               choice = 0;
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
