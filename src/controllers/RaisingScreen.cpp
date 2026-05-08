#include "RaisingScreen.h"
#include "GameEngine.h"
#include "MainScreen.h"
#include "../views/Renderer.h"
#include <iostream>

// [type-1][frame]: 3 frames × 400ms each = 1.2s animation
static const char* ANIM_FRAMES[4][3] = {
    // 1: 밥주기 (yellow)
    {"\033[33m  냠... 냠...  \033[0m",
     "\033[33m  냠냠냠냠냠!!!  \033[0m",
     "\033[32m  배부르다~!  \033[0m"},
    // 2: 목욕하기 (cyan)
    {"\033[36m  ♨  첨벙~  \033[0m",
     "\033[36m  ♨♨  쓱싹 쓱싹~  \033[0m",
     "\033[36m  반짝반짝 깨끗해요!  \033[0m"},
    // 3: 놀아주기 (magenta)
    {"\033[35m  ♪  같이 놀자~  \033[0m",
     "\033[35m  ♪♪  야호~! 신난다!  \033[0m",
     "\033[35m  즐거워요!  \033[0m"},
    // 4: 치료하기 (green)
    {"\033[32m  치료 중...  \033[0m",
     "\033[32m  나아지고 있어요...  \033[0m",
     "\033[32m  완전히 나았어요!  \033[0m"},
};

void RaisingScreen::render(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player, "2026-05-04");

    std::cout << std::endl;
    Renderer::drawMainCharacter();

    if (animating) {
        auto now = std::chrono::steady_clock::now();
        long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - animStart).count();

        if (elapsed >= 1200) {
            animating = false;
            Renderer::drawMessage(lastAction);
        } else {
            int frame = static_cast<int>(elapsed / 400);
            if (frame > 2) frame = 2;
            std::cout << "\n >> " << ANIM_FRAMES[animType - 1][frame] << std::endl;
        }
    } else if (!lastAction.empty()) {
        Renderer::drawMessage(lastAction);
    } else {
        std::cout << "\n\n";
    }

    Renderer::drawBottomMenu({"밥주기 (1)", "목욕하기 (2)", "놀아주기 (3)", "치료하기 (4)", "돌아가기 (0)"});
}

void RaisingScreen::handleInput(GameEngine& engine, Character& player, const InputEvent& event) {
    int choice = -1;

    if (event.type == InputType::KEYBOARD) {
        if (event.key >= '0' && event.key <= '4') {
            choice = event.key - '0';
        }
    } else if (event.type == InputType::MOUSE_PRESS && event.button == 0) {
        if (event.y >= 30) {
            if (event.x >= 1 && event.x <= 24) choice = 1;
            else if (event.x >= 25 && event.x <= 48) choice = 2;
            else if (event.x >= 49 && event.x <= 72) choice = 3;
            else if (event.x >= 73 && event.x <= 96) choice = 4;
            else if (event.x >= 97) choice = 0;
        }
    }

    if (choice == 1) {
        player.feed();
        lastAction = "다마고치에게 밥을 주었습니다! (배고픔 +20, 행복 +5)";
        animating = true; animType = 1; animStart = std::chrono::steady_clock::now();
    } else if (choice == 2) {
        player.bath();
        lastAction = "다마고치를 씻겨주었습니다! (청결 +30, 행복 -5)";
        animating = true; animType = 2; animStart = std::chrono::steady_clock::now();
    } else if (choice == 3) {
        player.play();
        lastAction = "다마고치와 놀아주었습니다! (행복 +20, 배고픔 -10, 청결 -10)";
        animating = true; animType = 3; animStart = std::chrono::steady_clock::now();
    } else if (choice == 4) {
        player.treat();
        lastAction = "다마고치를 치료해주었습니다! (건강 +30, 돈 -100)";
        animating = true; animType = 4; animStart = std::chrono::steady_clock::now();
    } else if (choice == 0) {
        engine.changeScreen(std::make_unique<MainScreen>());
    }
}
