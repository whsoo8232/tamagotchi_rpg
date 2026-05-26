#include "RunningScreen.h"
#include "GameEngine.h"
#include "TrainingScreen.h"
#include "../views/Renderer.h"
#include <iostream>
#include <algorithm>

static const int TOTAL_STEPS = 10;

RunningScreen::RunningScreen()
    : state(RNState::IDLE), runCount(0), stepAnim(false) {}

// ══════════════════════════════════════════════════════════════════
//  render
// ══════════════════════════════════════════════════════════════════

void RunningScreen::render(const Character& player) {
    if (stepAnim) {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - stepTime).count();
        if (ms >= 200) stepAnim = false;
    }

    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    const auto& kuchiArt  = Renderer::getPlayerArt();
    const auto& finishArt = Renderer::getFinishLineArt();

    int kRows = (int)kuchiArt.size();
    int fRows = (int)finishArt.size();
    int artRows = std::max(kRows, fRows);

    // 아트 최대 표시 너비 계산
    int kuchiW = 0, finishW = 0;
    for (const auto& l : kuchiArt)  kuchiW  = std::max(kuchiW,  Renderer::getDisplayWidth(l));
    for (const auto& l : finishArt) finishW = std::max(finishW, Renderer::getDisplayWidth(l));

    // 구치파치 이동량: runCount에 따라 0 → maxShift col 이동
    int maxShift = std::max(0, 161 - kuchiW - finishW);
    int leftPad  = runCount * maxShift / TOTAL_STEPS;
    int gap      = maxShift - leftPad;

    // 수직 중앙 정렬 오프셋
    int kTop = (artRows - kRows) / 2;
    int fTop = (artRows - fRows) / 2;

    // ── 출력 (상단 여백 2줄) ─────────────────────────────────────
    std::cout << "\n\n";
    for (int row = 0; row < artRows; ++row) {
        // 구치파치
        {
            int r = row - kTop;
            std::string cell;
            if (r >= 0 && r < kRows) cell = kuchiArt[r];
            std::cout << std::string(leftPad, ' ')
                      << Renderer::padRight(cell, kuchiW);
        }
        // 결승선
        {
            int r = row - fTop;
            if (r >= 0 && r < fRows)
                std::cout << std::string(gap, ' ') << finishArt[r];
        }
        std::cout << "\n";
    }

    // ── 진행 게이지 ───────────────────────────────────────────────
    std::string bar = "[";
    for (int i = 0; i < TOTAL_STEPS; ++i)
        bar += (i < runCount ? "█" : "░");
    bar += "]  " + std::to_string(runCount) + " / " + std::to_string(TOTAL_STEPS) + " 걸음";

    // 2(pad) + artRows + 1(blank) + 1(bar) 행 사용
    int usedRows = 2 + artRows + 2;

    if (state == RNState::DONE) {
        std::cout << "\n" << Renderer::GREEN
                  << Renderer::center(bar, 161)
                  << Renderer::RESET << "\n";
        std::cout << "\n" << Renderer::GREEN
                  << Renderer::center("결승선 통과!  체력(HP) +20 상승!", 161)
                  << Renderer::RESET << "\n";
        Renderer::fillContent(usedRows + 2);
        Renderer::drawBottomMenu({"훈련 완료 (클릭)"});
    } else {
        std::cout << "\n" << Renderer::center(bar, 161) << "\n";
        Renderer::fillContent(usedRows);
        Renderer::drawBottomMenu({"달리기", "돌아가기"});
    }
}

// ══════════════════════════════════════════════════════════════════
//  handleInput
// ══════════════════════════════════════════════════════════════════

void RunningScreen::handleInput(GameEngine& engine, Character& player, const InputEvent& event) {
    if (state == RNState::DONE) {
        if (event.type == InputType::MOUSE_PRESS) {
            player.trainHp(20);
            engine.changeScreen(std::make_unique<TrainingScreen>());
        }
        return;
    }

    if (event.type == InputType::MOUSE_PRESS && event.button == 0 && event.y >= 41) {
        int btn = (event.x - 1) / (158 / 2);
        if (btn == 0) { // 달리기
            ++runCount;
            stepAnim = true;
            stepTime = std::chrono::steady_clock::now();
            if (runCount >= TOTAL_STEPS) state = RNState::DONE;
        } else { // 돌아가기
            engine.changeScreen(std::make_unique<TrainingScreen>());
        }
    }
}
