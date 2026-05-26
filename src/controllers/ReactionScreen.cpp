#include "ReactionScreen.h"
#include "GameEngine.h"
#include "TrainingScreen.h"
#include "../views/Renderer.h"
#include <iostream>
#include <cstdlib>

ReactionScreen::ReactionScreen()
    : state(RCState::INTRO), waitMs(0), reactionMs(-1), agiGain(0) {
    stateEntered = std::chrono::steady_clock::now();
}

long long ReactionScreen::elapsed() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - stateEntered).count();
}

long long ReactionScreen::signalElapsed() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - signalStart).count();
}

// 반응 시간 → 민첩 획득량
int ReactionScreen::calcAgiGain(long long ms) const {
    if (ms <  100) return 5;
    if (ms <  200) return 4;
    if (ms <  400) return 3;
    if (ms <  500) return 2;
    if (ms <  600) return 1;
    return 0; // 실패
}

// ══════════════════════════════════════════════════════════════════
//  render
// ══════════════════════════════════════════════════════════════════

void ReactionScreen::render(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    std::cout << "\n";
    std::cout << "  [ 반응속도 훈련 ]\n";
    std::cout << "------------------------------------------------------------------------------------------------------------------------\n\n\n\n";

    // 말풍선 + 캐릭터
    std::string bubble;
    std::string bubbleColor;
    if (state == RCState::INTRO || state == RCState::WAITING) {
        bubble      = "  ╔══════════╗\n"
                      "  ║  준비!  ║\n"
                      "  ╚══════════╝";
        bubbleColor = Renderer::BLUE;
    } else if (state == RCState::SIGNAL) {
        bubble      = "  ╔══════════╗\n"
                      "  ║  지금!  ║\n"
                      "  ╚══════════╝";
        bubbleColor = Renderer::YELLOW;
    } else if (state == RCState::JUDGED) {
        bubble      = "  ╔══════════╗\n"
                      "  ║ " + judgeMsg + " ║\n"
                      "  ╚══════════╝";
        bubbleColor = (agiGain > 0) ? Renderer::GREEN : Renderer::RED;
    } else {
        bubble = "";
        bubbleColor = "";
    }

    if (!bubble.empty()) {
        std::cout << bubbleColor << bubble << Renderer::RESET << "\n\n";
    }

    // 캐릭터 아트 (간략)
    std::cout << "           (^_^)\n";
    std::cout << "           ( || )\n";
    std::cout << "           /|  |\\\n\n";

    // 상태 안내
    if (state == RCState::INTRO) {
        std::cout << "  \"지금!\" 말풍선이 바뀌면 \"지금!\" 버튼을 누르세요.\n";
        std::cout << "  신호 전에 누르면 실패!\n";
    } else if (state == RCState::WAITING) {
        std::cout << "  집중하세요... 말풍선이 바뀌는 순간 누르세요!\n";
    } else if (state == RCState::SIGNAL) {
        long long se = signalElapsed();
        std::cout << Renderer::YELLOW << "  ★ 지금 눌러! (" << se << "ms 경과)\n" << Renderer::RESET;
    } else if (state == RCState::JUDGED) {
        if (agiGain > 0)
            std::cout << Renderer::GREEN << "  반응 시간: " << reactionMs << "ms → 민첩 +" << agiGain << "\n" << Renderer::RESET;
        else if (reactionMs == -2)
            std::cout << Renderer::RED << "  신호 전에 눌렀습니다! 실패\n" << Renderer::RESET;
        else
            std::cout << Renderer::RED << "  반응 시간: " << reactionMs << "ms → 너무 느려요! 실패\n" << Renderer::RESET;
    } else if (state == RCState::RESULT) {
        if (agiGain > 0) {
            std::cout << Renderer::GREEN << "  훈련 완료! 민첩 +" << agiGain << " 상승!\n" << Renderer::RESET;
            std::cout << "  반응 시간: " << reactionMs << "ms\n";
        } else {
            std::cout << Renderer::RED << "  훈련 실패. 민첩 변화 없음.\n" << Renderer::RESET;
        }
        std::cout << "\n  아무 키나 눌러 훈련 종료\n";
    }

    if (state == RCState::RESULT) {
        // 6(공통) + 0(버블없음) + 4(캐릭터) + 4(성공) or 3(실패) = 14 or 13
        Renderer::fillContent(agiGain > 0 ? 14 : 13);
        Renderer::drawBottomMenu({"훈련 종료 (아무 키)"});
    } else if (state == RCState::INTRO) {
        // 6(공통) + 4(버블) + 4(캐릭터) + 2(상태) = 16
        Renderer::fillContent(16);
        Renderer::drawBottomMenu({"지금! (스페이스)", "돌아가기 (q)"});
    } else {
        // WAITING/SIGNAL/JUDGED: 6+4+4+1 = 15
        Renderer::fillContent(15);
        Renderer::drawBottomMenu({"지금! (스페이스)", "돌아가기 (q)"});
    }
}

// ══════════════════════════════════════════════════════════════════
//  handleInput
// ══════════════════════════════════════════════════════════════════

void ReactionScreen::handleInput(GameEngine& engine, Character& player, const InputEvent& event) {
    auto enterState = [&](RCState s) {
        state = s;
        stateEntered = std::chrono::steady_clock::now();
    };

    // RESULT: 클릭 → 훈련 종료
    if (state == RCState::RESULT) {
        if (event.type == InputType::MOUSE_PRESS) {
            if (agiGain > 0) player.trainAgility(agiGain);
            engine.changeScreen(std::make_unique<TrainingScreen>());
        }
        return;
    }

    // INTRO: 1.5초 후 WAITING으로
    if (state == RCState::INTRO) {
        if (elapsed() >= 1500) {
            waitMs = 1000 + std::rand() % 2001; // 1000-3000ms
            enterState(RCState::WAITING);
        }
        // 돌아가기 버튼 클릭
        if (event.type == InputType::MOUSE_PRESS && event.button == 0 &&
            event.y >= 31 && event.x > 80) {
            engine.changeScreen(std::make_unique<TrainingScreen>());
        }
        return;
    }

    // WAITING: 대기 시간 경과 → SIGNAL / 조기 클릭 → 실패
    if (state == RCState::WAITING) {
        bool earlyClick = (event.type == InputType::MOUSE_PRESS && event.button == 0 &&
                           event.y >= 31 && event.x <= 80); // "지금!" 버튼 영역
        bool backClick  = (event.type == InputType::MOUSE_PRESS && event.button == 0 &&
                           event.y >= 31 && event.x > 80);

        if (backClick) {
            engine.changeScreen(std::make_unique<TrainingScreen>());
            return;
        }
        if (earlyClick) {
            reactionMs = -2;
            agiGain    = 0;
            judgeMsg   = "실패";
            enterState(RCState::JUDGED);
            return;
        }
        if (elapsed() >= waitMs) {
            signalStart = std::chrono::steady_clock::now();
            enterState(RCState::SIGNAL);
        }
        return;
    }

    // SIGNAL: 0.6초 이내 "지금!" 버튼 클릭
    if (state == RCState::SIGNAL) {
        long long se = signalElapsed();

        bool pressed = (event.type == InputType::MOUSE_PRESS && event.button == 0 &&
                        event.y >= 31 && event.x <= 80);

        if (pressed) {
            reactionMs = se;
            agiGain    = calcAgiGain(se);
            judgeMsg   = (agiGain > 0) ? "성공!" : "실패";
            enterState(RCState::JUDGED);
            return;
        }

        // 0.6초 초과 → 실패
        if (se >= 600) {
            reactionMs = se;
            agiGain    = 0;
            judgeMsg   = "실패";
            enterState(RCState::JUDGED);
        }
        return;
    }

    // JUDGED: 1.5초 후 RESULT
    if (state == RCState::JUDGED) {
        if (elapsed() >= 1500) {
            enterState(RCState::RESULT);
        }
        return;
    }
}
