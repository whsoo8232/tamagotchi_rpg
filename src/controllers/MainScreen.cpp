#include "MainScreen.h"
#include "GameEngine.h"
#include "RaisingScreen.h"
#include "TrainingScreen.h"
#include "BattleScreen.h"
#include "EquipmentScreen.h"
#include "../views/Renderer.h"
#include <iostream>
#include <string>
#include <cstdlib>

static const char* DIALOGUES[] = {
    "오늘도 잘 부탁해!",
    "배고파... 밥 줘!",
    "같이 놀아줘~",
    "깨끗하게 씻고 싶어!",
    "운동 열심히 할게!",
    "몬스터 무서워...",
    "강해지고 싶어!",
    "기분이 좋아!",
    "졸려... zzz",
    "나 오늘 잘생겼지?"
};
static const int  DIALOGUE_COUNT = 10;
// 말풍선 위치 (kuchi art 0-indexed row 기준)
static const int BUBBLE_TOP = 5;
static const int BUBBLE_MID = 6;
static const int BUBBLE_BOT = 7;
static const int INNER_W    = 22;  // 말풍선 내부 너비 (display col, 양쪽 1칸 여백 포함)

static std::string bubbleTop() {
    std::string h;
    for (int i = 0; i < INNER_W; ++i) h += "═";
    return "  ╔" + h + "╗";
}

static std::string bubbleMid(int idx) {
    std::string inner = " " + Renderer::padRight(DIALOGUES[idx], INNER_W - 2) + " ";
    return "< ║" + inner + "║";
}

static std::string bubbleBot() {
    std::string h;
    for (int i = 0; i < INNER_W; ++i) h += "═";
    return "  ╚" + h + "╝";
}

// ══════════════════════════════════════════════════════════════════

MainScreen::MainScreen() : bubbleActive(false), bubbleIdx(0) {}

void MainScreen::render(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    if (!bubbleActive) {
        Renderer::drawMainCharacter();   // 2+32+2 = 36행 = CONTENT_H
    } else {
        const auto& art = Renderer::getPlayerArt();
        int kRows = (int)art.size();
        static const int INDENT = 50;   // (161-60)/2

        std::string top = bubbleTop();
        std::string mid = bubbleMid(bubbleIdx);
        std::string bot = bubbleBot();

        std::cout << "\n\n";
        for (int row = 0; row < kRows; ++row) {
            std::cout << std::string(INDENT, ' ')
                      << Renderer::padRight(art[row], 60);
            if      (row == BUBBLE_TOP) std::cout << top;
            else if (row == BUBBLE_MID) std::cout << mid;
            else if (row == BUBBLE_BOT) std::cout << bot;
            std::cout << "\n";
        }
        std::cout << "\n\n";
    }

    Renderer::fillContent(36);
    Renderer::drawBottomMenu({"육성", "전투", "장비", "훈련"});
}

void MainScreen::handleInput(GameEngine& engine, Character& player, const InputEvent& event) {
    if (event.type != InputType::MOUSE_PRESS || event.button != 0) return;

    // 하단 메뉴
    if (event.y >= 41) {
        int btn = (event.x - 1) / (158 / 4) + 1;
        if      (btn == 1) engine.changeScreen(std::make_unique<RaisingScreen>());
        else if (btn == 2) engine.changeScreen(std::make_unique<BattleScreen>());
        else if (btn == 3) engine.changeScreen(std::make_unique<EquipmentScreen>());
        else if (btn == 4) engine.changeScreen(std::make_unique<TrainingScreen>());
        return;
    }

    // 구치파치 클릭 영역: x 51-110, y 7-38 (header 4줄 + blank 2줄 + kuchi 32줄)
    if (event.x >= 51 && event.x <= 110 && event.y >= 7 && event.y <= 38) {
        bubbleIdx    = std::rand() % DIALOGUE_COUNT;
        bubbleActive = true;
    }
}
