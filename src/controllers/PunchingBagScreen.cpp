#include "PunchingBagScreen.h"
#include "GameEngine.h"
#include "TrainingScreen.h"
#include "../views/Renderer.h"
#include <iostream>
#include <algorithm>

static const int TOTAL_PUNCHES = 10;

PunchingBagScreen::PunchingBagScreen()
    : state(PBState::IDLE), punchCount(0), punchAnim(false) {}

// ══════════════════════════════════════════════════════════════════
//  render
// ══════════════════════════════════════════════════════════════════

void PunchingBagScreen::render(const Character& player) {
    if (punchAnim) {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - punchTime).count();
        if (ms >= 300) punchAnim = false;
    }

    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    // ── 레이아웃: left(65) │ center(29) │ right(65) = 161 ───────────
    static const int LEFT_W   = 65;
    static const int CENTER_W = 29;
    static const int RIGHT_W  = 65;

    const auto& playerArt = Renderer::getPlayerArt();
    const auto& bagArt    = Renderer::getPunchingBagArt();

    int pRows   = (int)playerArt.size();
    int bRows   = (int)bagArt.size();
    int artRows = std::max(pRows, bRows);

    // 수직 중앙 정렬 오프셋
    int pTop = (artRows - pRows) / 2;
    int bTop = (artRows - bRows) / 2;

    // 아트 수평 너비 → 열 안에서 중앙 정렬
    int pArtW = 0, bArtW = 0;
    for (const auto& l : playerArt) pArtW = std::max(pArtW, Renderer::getDisplayWidth(l));
    for (const auto& l : bagArt)    bArtW = std::max(bArtW, Renderer::getDisplayWidth(l));
    int pIndent = std::max(0, (LEFT_W  - pArtW) / 2);
    int bIndent = std::max(0, (RIGHT_W - bArtW) / 2);

    // ── 중앙 컬럼 내용 빌드 ─────────────────────────────────────────
    std::vector<std::string> centerCol(artRows, "");

    int remaining = TOTAL_PUNCHES - punchCount;

    // 상단: 남은 펀치 횟수
    centerCol[2] = Renderer::center("[ 남은 펀치 ]", CENTER_W);
    centerCol[4] = Renderer::center(std::to_string(remaining) + "번 남음", CENTER_W);

    // 진행 바
    std::string bar = "[";
    for (int i = 0; i < TOTAL_PUNCHES; ++i)
        bar += (i < punchCount ? "█" : "░");
    bar += "]";
    centerCol[6] = Renderer::center(bar, CENTER_W);

    // 하단: 펀치 피드백
    if (punchAnim) {
        centerCol[10] = Renderer::center("퍽!", CENTER_W);
        centerCol[11] = Renderer::center("(" + std::to_string(punchCount) + "/" +
                                         std::to_string(TOTAL_PUNCHES) + ")", CENTER_W);
    }

    // ── 3열 출력 (상단 여백 2줄) ─────────────────────────────────────
    std::cout << "\n\n";
    for (int row = 0; row < artRows; ++row) {
        // 좌: 플레이어
        {
            int r = row - pTop;
            std::string cell;
            if (r >= 0 && r < pRows)
                cell = std::string(pIndent, ' ') + playerArt[r];
            std::cout << Renderer::padRight(cell, LEFT_W);
        }
        // 중: 정보
        std::cout << "│" << Renderer::padRight(centerCol[row], CENTER_W) << "│";
        // 우: 샌드백
        {
            int r = row - bTop;
            std::string cell;
            if (r >= 0 && r < bRows)
                cell = std::string(bIndent, ' ') + bagArt[r];
            std::cout << Renderer::padRight(cell, RIGHT_W);
        }
        std::cout << "\n";
    }

    // 2(pad) + artRows 행 사용
    int usedRows = 2 + artRows;

    if (state == PBState::DONE) {
        std::cout << "\n" << Renderer::GREEN
                  << Renderer::center("훈련 완료!  공격력 +8", 161)
                  << Renderer::RESET << "\n";
        Renderer::fillContent(usedRows + 2);
        Renderer::drawBottomMenu({"훈련 완료 (클릭)"});
    } else {
        Renderer::fillContent(usedRows);
        Renderer::drawBottomMenu({"펀치", "돌아가기"});
    }
}

// ══════════════════════════════════════════════════════════════════
//  handleInput
// ══════════════════════════════════════════════════════════════════

void PunchingBagScreen::handleInput(GameEngine& engine, Character& player, const InputEvent& event) {
    if (state == PBState::DONE) {
        if (event.type == InputType::MOUSE_PRESS) {
            player.trainAttack(8);
            engine.changeScreen(std::make_unique<TrainingScreen>());
        }
        return;
    }

    if (event.type == InputType::MOUSE_PRESS && event.button == 0 && event.y >= 41) {
        int btn = (event.x - 1) / (158 / 2);
        if (btn == 0) { // 펀치
            ++punchCount;
            punchAnim = true;
            punchTime = std::chrono::steady_clock::now();
            if (punchCount >= TOTAL_PUNCHES) state = PBState::DONE;
        } else { // 돌아가기
            engine.changeScreen(std::make_unique<TrainingScreen>());
        }
    }
}
