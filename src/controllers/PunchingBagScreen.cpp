#include "PunchingBagScreen.h"
#include "GameEngine.h"
#include "TrainingScreen.h"
#include "../views/Renderer.h"
#include <iostream>
#include <string>

// ── 캐릭터 ASCII (6행) ─────────────────────────────────────────────
const char* PunchingBagScreen::CHAR_IDLE[6] = {
    "   (^_^)  ",
    "   ( || ) ",
    "   /|  |  ",
    "  / |  |  ",
    "    |  |  ",
    "   _|  |_ ",
};
const char* PunchingBagScreen::CHAR_PUNCH[6] = {
    "   (>_<)  ",
    "   ( ||---",
    "   /|     ",
    "  / |     ",
    "    |     ",
    "   _|__   ",
};

// ── 샌드백 ASCII 4단계 × 8행 ─────────────────────────────────────
const char* PunchingBagScreen::BAG_ART[4][8] = {
    // 0: 온전
    {
        "    _______   ",
        "   |       |  ",
        "   |       |  ",
        "   | 샌드백 | ",
        "   |       |  ",
        "   |_______|  ",
        "      | |     ",
        "      | |     ",
    },
    // 1: 균열 (10+)
    {
        "    _//_____  ",
        "   |  //    | ",
        "   | //균열 | ",
        "   |// 백   | ",
        "   |/       | ",
        "   |_//_____| ",
        "      | |     ",
        "      | |     ",
    },
    // 2: 파손 (20+)
    {
        "    _x___x__  ",
        "   |x  x   |  ",
        "   | x 파  |  ",
        "   |x  손  x| ",
        "   | x   x |  ",
        "   |x__x___x| ",
        "      | |     ",
        "      | |     ",
    },
    // 3: 파괴 (30)
    {
        "    * * * *   ",
        "   * 완전  *  ",
        "  *  파괴   * ",
        "   *  !!   *  ",
        "    * * * *   ",
        "              ",
        "              ",
        "              ",
    },
};

PunchingBagScreen::PunchingBagScreen()
    : state(PBState::IDLE), punchCount(0), punchAnim(false) {}

int PunchingBagScreen::bagStage() const {
    if (punchCount >= 30) return 3;
    if (punchCount >= 20) return 2;
    if (punchCount >= 10) return 1;
    return 0;
}

// ══════════════════════════════════════════════════════════════════
//  render
// ══════════════════════════════════════════════════════════════════

void PunchingBagScreen::render(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    // 애니메이션 만료 확인 (300ms)
    if (punchAnim) {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - punchTime).count();
        if (ms >= 300) punchAnim = false;
    }

    const char** charArt = punchAnim ? CHAR_PUNCH : CHAR_IDLE;
    int stage = bagStage();

    std::cout << "\n";
    std::cout << "  [ 샌드백 치기 ]    진행: " << punchCount << " / 30\n";
    std::cout << "  ";
    for (int i = 0; i < 30; ++i)
        std::cout << (i < punchCount ? "█" : "░");
    std::cout << "\n";
    std::cout << "------------------------------------------------------------------------------------------------------------------------\n\n";

    // 좌측: 캐릭터(40컬럼), 우측: 샌드백(40컬럼)
    int rows = 8;
    for (int r = 0; r < rows; ++r) {
        std::string charLine = (r < 6) ? charArt[r] : "          ";
        std::string bagLine  = BAG_ART[stage][r];

        std::cout << "          "
                  << Renderer::padRight(charLine, 35)
                  << "     "
                  << bagLine << "\n";
    }

    std::cout << "\n";
    if (state == PBState::DONE) {
        std::cout << Renderer::GREEN << "  샌드백을 완전히 부쉈습니다! 공격력 +5 상승!\n" << Renderer::RESET;
        std::cout << "  아무 키나 눌러 훈련 종료\n";
        for (int i = 0; i < 8; ++i) std::cout << "\n";
        Renderer::drawBottomMenu({"훈련 완료 (클릭)"});
    } else {
        if (punchAnim)
            std::cout << Renderer::YELLOW << "  펀치! (" << punchCount << "/30)\n" << Renderer::RESET;
        else
            std::cout << "  \"펀치\" 버튼을 30번 눌러 샌드백을 부수세요!\n";
        for (int i = 0; i < 8; ++i) std::cout << "\n";
        Renderer::drawBottomMenu({"펀치", "돌아가기"});
    }
}

// ══════════════════════════════════════════════════════════════════
//  handleInput
// ══════════════════════════════════════════════════════════════════

void PunchingBagScreen::handleInput(GameEngine& engine, Character& player, const InputEvent& event) {
    if (state == PBState::DONE) {
        if (event.type == InputType::MOUSE_PRESS) {
            player.trainAttack(5);
            engine.changeScreen(std::make_unique<TrainingScreen>());
        }
        return;
    }

    int choice = -1;
    if (event.type == InputType::MOUSE_PRESS && event.button == 0) {
        if (event.y >= 30) {
            int btnW = 118 / 2;
            int btn  = (event.x - 1) / btnW;
            if (btn == 0) choice = 1; // 펀치
            else          choice = 0; // 돌아가기
        }
    }

    if (choice == 1) {
        ++punchCount;
        punchAnim = true;
        punchTime = std::chrono::steady_clock::now();
        if (punchCount >= 30) {
            state = PBState::DONE;
        }
    } else if (choice == 0) {
        engine.changeScreen(std::make_unique<TrainingScreen>());
    }
}
