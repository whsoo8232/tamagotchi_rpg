#include "EndingScreen.h"
#include "GameEngine.h"
#include "../views/Renderer.h"
#include <iostream>
#include <algorithm>

EndingScreen::EndingScreen(EndingType t) : type(t) {
    switch (t) {
    case EndingType::HERO:
        artIdx  = 0;
        message = "구치파치가 세계의 영웅이 되었습니다!";
        color   = Renderer::YELLOW;
        break;
    case EndingType::STARVE:
        artIdx  = 1;
        message = "구치파치가 배고파 쓰러졌습니다";
        color   = Renderer::RED;
        break;
    case EndingType::RUNAWAY:
        artIdx  = 2;
        message = "구치파치가 심심해 가출했습니다";
        color   = Renderer::RED;
        break;
    case EndingType::TRASH:
        artIdx  = 3;
        message = "구치파치가 쓰레기더미에 파묻혔습니다";
        color   = Renderer::RED;
        break;
    case EndingType::HOSPITAL:
        artIdx  = 4;
        message = "구치파치가 병원에 입원했습니다";
        color   = Renderer::RED;
        break;
    }
}

// ══════════════════════════════════════════════════════════════════
//  render
// ══════════════════════════════════════════════════════════════════

void EndingScreen::render(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    const auto& art = Renderer::getEndingArt(artIdx);
    int artRows = (int)art.size();

    // 아트 + 공백 + 메시지 + 공백 = artRows + 3
    // 상단 여백: 컨텐츠 영역에서 수직 중앙 정렬
    int topPad = std::max(1, (Renderer::CONTENT_H - artRows - 3) / 2);
    int usedRows = topPad + artRows + 3;

    for (int i = 0; i < topPad; ++i) std::cout << "\n";

    for (const auto& line : art)
        std::cout << color << Renderer::center(line, 161) << Renderer::RESET << "\n";

    std::cout << "\n";
    std::cout << color << Renderer::center(message, 161) << Renderer::RESET << "\n";
    std::cout << "\n";

    Renderer::fillContent(usedRows);
    Renderer::drawBottomMenu({"게임 종료 (클릭)"});
}

// ══════════════════════════════════════════════════════════════════
//  handleInput
// ══════════════════════════════════════════════════════════════════

void EndingScreen::handleInput(GameEngine& engine, Character& /*player*/, const InputEvent& event) {
    if (event.type == InputType::MOUSE_PRESS)
        engine.quit();
}
