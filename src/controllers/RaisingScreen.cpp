#include "RaisingScreen.h"
#include "GameEngine.h"
#include "MainScreen.h"
#include "../views/Renderer.h"
#include <iostream>

void RaisingScreen::render(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    // 애니메이션 경과 확인 → 1.2s 지나면 기본 아트로 복귀
    if (animating) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - animStart).count();
        if (elapsed >= 1200)
            animating = false;
    }

    // 각 아트 함수가 내부적으로 36행(= CONTENT_H)을 출력
    if      (animating && animType == 1) Renderer::drawEatingCharacter(); // 3+30+3=36
    else if (animating && animType == 2) Renderer::drawBathCharacter();   // 4+28+4=36
    else if (animating && animType == 3) Renderer::drawPlayCharacter();   // 7+22+7=36
    else if (animating && animType == 4) Renderer::drawTreatCharacter();  // 3+30+3=36
    else                                 Renderer::drawMainCharacter();    // 2+32+2=36

    Renderer::fillContent(36); // 36행 사용 → 패딩 없음

    Renderer::drawBottomMenu({"밥주기", "목욕하기", "놀아주기", "치료하기", "돌아가기"});
}

void RaisingScreen::handleInput(GameEngine& engine, Character& player, const InputEvent& event) {
    int choice = -1;

    if (event.type == InputType::MOUSE_PRESS && event.button == 0) {
        if (event.y >= 41) {
            int btnW = 158 / 5;
            int btn  = (event.x - 1) / btnW;
            if (btn == 0) choice = 1;
            else if (btn == 1) choice = 2;
            else if (btn == 2) choice = 3;
            else if (btn == 3) choice = 4;
            else               choice = 0;
        }
    }

    if (choice == 1) {
        player.feed();
        animating = true; animType = 1; animStart = std::chrono::steady_clock::now();
    } else if (choice == 2) {
        player.bath();
        animating = true; animType = 2; animStart = std::chrono::steady_clock::now();
    } else if (choice == 3) {
        player.play();
        animating = true; animType = 3; animStart = std::chrono::steady_clock::now();
    } else if (choice == 4) {
        if (player.getMoney() >= 100) {
            player.treat();
            animating = true; animType = 4; animStart = std::chrono::steady_clock::now();
        }
    } else if (choice == 0) {
        engine.changeScreen(std::make_unique<MainScreen>());
    }
}
