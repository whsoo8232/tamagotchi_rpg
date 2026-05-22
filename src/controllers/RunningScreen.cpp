#include "RunningScreen.h"
#include "GameEngine.h"
#include "TrainingScreen.h"
#include "../views/Renderer.h"
#include <iostream>
#include <string>
#include <algorithm>

static constexpr int TOTAL_STEPS = 30;
static constexpr int TRACK_WIDTH = 100; // 트랙 표시 너비 (컬럼)

RunningScreen::RunningScreen()
    : state(RNState::IDLE), runCount(0), stepAnim(false) {}

int RunningScreen::charPos() const {
    // 0-based 컬럼, 0~TRACK_WIDTH
    return runCount * TRACK_WIDTH / TOTAL_STEPS;
}

// ══════════════════════════════════════════════════════════════════
//  render
// ══════════════════════════════════════════════════════════════════

void RunningScreen::render(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    // 애니메이션 만료 (200ms)
    if (stepAnim) {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - stepTime).count();
        if (ms >= 200) stepAnim = false;
    }

    std::cout << "\n";
    std::cout << "  [ 달리기 훈련 ]    진행: " << runCount << " / " << TOTAL_STEPS << " 걸음\n";
    std::cout << "  ";
    for (int i = 0; i < 30; ++i)
        std::cout << (i < runCount ? "▶" : "─");
    std::cout << "\n";
    std::cout << "------------------------------------------------------------------------------------------------------------------------\n\n\n";

    // 트랙 렌더
    int pos = charPos();
    std::string face = stepAnim ? "(^o^)" : "(^_^)";

    // 윗줄: 캐릭터 표정
    std::cout << "  |" << std::string(pos, ' ') << face
              << std::string(std::max(0, TRACK_WIDTH - pos - 5), ' ') << "|\n";

    // 트랙 바닥
    std::cout << "  |";
    for (int i = 0; i < TRACK_WIDTH; ++i) {
        if (i == pos || i == pos + 1 || i == pos + 2 || i == pos + 3 || i == pos + 4)
            std::cout << "=";
        else if (i == TRACK_WIDTH - 1)
            std::cout << "G";
        else
            std::cout << "-";
    }
    std::cout << "|\n";

    // 표지
    std::cout << "  |" << "S" << std::string(TRACK_WIDTH - 2, ' ') << "G|\n\n\n";
    std::cout << "  S=출발  G=결승선\n\n";

    if (state == RNState::DONE) {
        std::cout << Renderer::GREEN
                  << "  결승선 통과! 체력(HP) +10 상승!\n" << Renderer::RESET;
        std::cout << "  아무 키나 눌러 훈련 종료\n";
        for (int i = 0; i < 8; ++i) std::cout << "\n";
        Renderer::drawBottomMenu({"훈련 완료 (클릭)"});
    } else {
        std::cout << "  \"달리기\" 버튼을 " << TOTAL_STEPS << "번 눌러 결승선에 도착하세요!\n";
        for (int i = 0; i < 9; ++i) std::cout << "\n";
        Renderer::drawBottomMenu({"달리기", "돌아가기"});
    }
}

// ══════════════════════════════════════════════════════════════════
//  handleInput
// ══════════════════════════════════════════════════════════════════

void RunningScreen::handleInput(GameEngine& engine, Character& player, const InputEvent& event) {
    if (state == RNState::DONE) {
        if (event.type == InputType::MOUSE_PRESS) {
            player.trainHp(10);
            engine.changeScreen(std::make_unique<TrainingScreen>());
        }
        return;
    }

    int choice = -1;
    if (event.type == InputType::MOUSE_PRESS && event.button == 0) {
        if (event.y >= 30) {
            int btnW = 118 / 2;
            int btn  = (event.x - 1) / btnW;
            if (btn == 0) choice = 1; // 달리기
            else          choice = 0; // 돌아가기
        }
    }

    if (choice == 1) {
        ++runCount;
        stepAnim = true;
        stepTime = std::chrono::steady_clock::now();
        state = RNState::RUNNING;
        if (runCount >= TOTAL_STEPS) {
            state = RNState::DONE;
        }
    } else if (choice == 0) {
        engine.changeScreen(std::make_unique<TrainingScreen>());
    }
}
