#include "ReactionScreen.h"
#include "GameEngine.h"
#include "TrainingScreen.h"
#include "../views/Renderer.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>

ReactionScreen::ReactionScreen()
    : state(RCState::INTRO), waitMs(0), reactionMs(-1), agiGain(0) {
    stateEntered = std::chrono::steady_clock::now();
    signalStart  = stateEntered;
}

long long ReactionScreen::elapsed() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - stateEntered).count();
}

long long ReactionScreen::signalElapsed() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - signalStart).count();
}

int ReactionScreen::calcAgiGain(long long ms) const {
    if (ms <  100) return 6;
    if (ms <  200) return 5;
    if (ms <  300) return 4;
    if (ms <  400) return 3;
    if (ms <  500) return 2;
    return 1;
}

// ══════════════════════════════════════════════════════════════════
//  render
// ══════════════════════════════════════════════════════════════════

void ReactionScreen::render(const Character& player) {
    // ── 타이머 기반 상태 전환 ──────────────────────────────────────
    auto now = std::chrono::steady_clock::now();
    if (state == RCState::INTRO && elapsed() >= 1500) {
        waitMs       = 1000 + std::rand() % 2001;
        state        = RCState::WAITING;
        stateEntered = now;
    } else if (state == RCState::WAITING && elapsed() >= waitMs) {
        signalStart  = now;
        state        = RCState::SIGNAL;
        stateEntered = now;
    } else if (state == RCState::SIGNAL && signalElapsed() >= 600) {
        reactionMs   = 600;
        agiGain      = 0;
        state        = RCState::JUDGED;
        stateEntered = now;
    } else if (state == RCState::JUDGED && elapsed() >= 1500) {
        state        = RCState::RESULT;
        stateEntered = now;
    }

    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    // ── 레이아웃: left(80) │ right(80) = 161 ─────────────────────
    static const int LEFT_W  = 80;
    static const int RIGHT_W = 80;

    const auto& playerArt = Renderer::getPlayerArt();
    const auto& targetArt = Renderer::getTargetArt();

    int pRows   = (int)playerArt.size();
    int tRows   = (int)targetArt.size();
    int artRows = pRows;

    int pArtW = 0;
    for (const auto& l : playerArt) pArtW = std::max(pArtW, Renderer::getDisplayWidth(l));
    int pIndent = std::max(0, (LEFT_W - pArtW) / 2);

    int tArtW = 0;
    for (const auto& l : targetArt) tArtW = std::max(tArtW, Renderer::getDisplayWidth(l));
    int tIndent = std::max(0, (RIGHT_W - tArtW) / 2);

    // ── 상태별 색상 / 메시지 ──────────────────────────────────────
    std::string artColor;
    std::string stateMsg;
    std::string detailMsg;

    switch (state) {
    case RCState::INTRO:
        artColor  = Renderer::BLUE;
        stateMsg  = "준비하세요!";
        detailMsg = "신호가 나오면 즉시 클릭";
        break;
    case RCState::WAITING:
        artColor  = Renderer::LBLUE;
        stateMsg  = "집중...";
        detailMsg = "";
        break;
    case RCState::SIGNAL:
        artColor  = Renderer::YELLOW;
        stateMsg  = "★  지금!  ★";
        detailMsg = std::to_string(signalElapsed()) + "ms";
        break;
    case RCState::JUDGED:
        artColor  = (agiGain > 0) ? Renderer::GREEN : Renderer::RED;
        stateMsg  = (agiGain > 0) ? "성공!" :
                    (reactionMs == -2 ? "신호 전에 눌렀어요!" : "너무 느려요!");
        detailMsg = (reactionMs >= 0) ?
                    (std::to_string(reactionMs) + "ms  →  민첩 +" + std::to_string(agiGain)) :
                    "실패!";
        break;
    case RCState::RESULT:
        artColor  = (agiGain > 0) ? Renderer::GREEN : Renderer::RED;
        stateMsg  = (agiGain > 0) ?
                    ("훈련 완료!  민첩 +" + std::to_string(agiGain) + " 상승!") :
                    "훈련 실패.  민첩 변화 없음.";
        detailMsg = (agiGain > 0 && reactionMs >= 0) ?
                    ("반응 시간: " + std::to_string(reactionMs) + "ms") : "";
        break;
    }

    // ── 우측 컬럼 빌드 ────────────────────────────────────────────
    std::vector<std::string> rightCol(artRows, "");

    // 제목
    rightCol[1] = Renderer::center("[ 반응속도 훈련 ]", RIGHT_W);

    // 상태 메시지 (row 4)
    rightCol[4] = artColor + Renderer::center(stateMsg, RIGHT_W) + Renderer::RESET;

    // 과녁 아트 (row 7부터)
    int tStart = 7;
    for (int r = 0; r < tRows && (tStart + r) < artRows; ++r)
        rightCol[tStart + r] = artColor + std::string(tIndent, ' ') + targetArt[r] + Renderer::RESET;

    // 상세 메시지
    int detailRow = tStart + tRows + 1;
    if (!detailMsg.empty() && detailRow < artRows)
        rightCol[detailRow] = artColor + Renderer::center(detailMsg, RIGHT_W) + Renderer::RESET;

    // ── 출력 (상단 여백 2줄) ──────────────────────────────────────
    std::cout << "\n\n";
    for (int row = 0; row < artRows; ++row) {
        // 좌: 플레이어
        {
            std::string cell;
            if (row < pRows)
                cell = std::string(pIndent, ' ') + playerArt[row];
            std::cout << Renderer::padRight(cell, LEFT_W);
        }
        // 우: 과녁 + 정보
        std::cout << "│" << rightCol[row] << "\n";
    }

    int usedRows = 2 + artRows;

    if (state == RCState::RESULT) {
        Renderer::fillContent(usedRows);
        Renderer::drawBottomMenu({"훈련 종료 (클릭)"});
    } else {
        Renderer::fillContent(usedRows);
        Renderer::drawBottomMenu({"지금!", "돌아가기"});
    }
}

// ══════════════════════════════════════════════════════════════════
//  handleInput
// ══════════════════════════════════════════════════════════════════

void ReactionScreen::handleInput(GameEngine& engine, Character& player, const InputEvent& event) {
    if (event.type != InputType::MOUSE_PRESS || event.button != 0) return;

    // RESULT: 아무 클릭 → 종료
    if (state == RCState::RESULT) {
        if (agiGain > 0) player.trainAgility(agiGain);
        engine.changeScreen(std::make_unique<TrainingScreen>());
        return;
    }

    // 버튼 영역 밖 클릭은 무시
    if (event.y < 41) return;

    int btn = (event.x - 1) / (158 / 2); // 0=지금! 1=돌아가기

    // 돌아가기
    if (btn == 1) {
        engine.changeScreen(std::make_unique<TrainingScreen>());
        return;
    }

    // "지금!" 버튼 (btn == 0)
    auto now = std::chrono::steady_clock::now();

    if (state == RCState::WAITING) {
        // 신호 전 클릭 → 실패
        reactionMs   = -2;
        agiGain      = 0;
        state        = RCState::JUDGED;
        stateEntered = now;
        return;
    }

    if (state == RCState::SIGNAL) {
        long long se = signalElapsed();
        reactionMs   = se;
        agiGain      = calcAgiGain(se);
        state        = RCState::JUDGED;
        stateEntered = now;
        return;
    }
}
