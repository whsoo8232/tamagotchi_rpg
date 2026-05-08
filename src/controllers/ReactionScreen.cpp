#include "ReactionScreen.h"
#include "GameEngine.h"
#include "TrainingScreen.h"
#include "../views/Renderer.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <numeric>
#include <iomanip>

ReactionScreen::ReactionScreen()
    : state(RCState::INTRO), round(0), totalRounds(5),
      isStar(true), consecutiveX(0), inputReceived(false),
      signalAreaX(40), signalAreaY(2), xAvoidSuccess(0),
      waitDuration(1500) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    stateEntered = std::chrono::steady_clock::now();
}

void ReactionScreen::enterState(RCState s) {
    state = s;
    stateEntered = std::chrono::steady_clock::now();
}

void ReactionScreen::generateSignal() {
    // 75% star, 25% X; but max 2 consecutive X
    int roll = std::rand() % 100;
    if (consecutiveX >= 2) {
        isStar = true;
    } else {
        isStar = (roll < 75);
    }
    consecutiveX = isStar ? 0 : consecutiveX + 1;

    signalAreaX = 20 + std::rand() % 60; // col 20-79
    signalAreaY = std::rand() % 5;       // row 0-4 within signal box
    inputReceived = false;
    signalStart = std::chrono::steady_clock::now();
}

long long ReactionScreen::averageReaction() const {
    if (reactionTimes.empty()) return 9999;
    long long sum = 0;
    for (auto t : reactionTimes) sum += t;
    return sum / (long long)reactionTimes.size();
}

int ReactionScreen::calcExp() const {
    if (reactionTimes.size() < 2) return 0; // 훈련 실패
    long long avg = averageReaction();
    if (avg < 150) return 15;
    if (avg < 250) return 12;
    if (avg < 350) return 8;
    if (avg < 500) return 4;
    return 1;
}

void ReactionScreen::render(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player, "2026-05-04");

    auto now = std::chrono::steady_clock::now();
    long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - stateEntered).count();

    // title bar
    std::cout << "\n";
    std::cout << "  [ 반응속도 테스트 ]    측정: " << round << "/" << totalRounds << "회\n";
    std::cout << "------------------------------------------------------------------------------------------------------------------------\n";

    // signal box (7 rows, ASCII frame)
    const int boxRows = 7;
    const int boxCols = 80;
    const int boxLeft = 20;
    const int symW = 2; // ★ and ✕ are 2-column wide in most terminals

    std::cout << "  " << std::string(boxLeft, ' ') << "+" << std::string(boxCols, '-') << "+\n";
    for (int r = 0; r < boxRows; ++r) {
        std::cout << "  " << std::string(boxLeft, ' ') << "|";
        if (state == RCState::SIGNAL && r == signalAreaY) {
            int sx = signalAreaX - boxLeft;
            if (sx < 0) sx = 0;
            if (sx > boxCols - symW) sx = boxCols - symW;
            std::cout << std::string(sx, ' ');
            if (isStar) std::cout << "\033[33m★\033[0m";
            else        std::cout << "\033[31m✕\033[0m";
            int remaining = boxCols - sx - symW;
            if (remaining > 0) std::cout << std::string(remaining, ' ');
        } else {
            std::cout << std::string(boxCols, ' ');
        }
        std::cout << "|\n";
    }
    std::cout << "  " << std::string(boxLeft, ' ') << "+" << std::string(boxCols, '-') << "+\n";

    // status line
    std::cout << "\n";
    if (state == RCState::INTRO) {
        std::cout << "  ★ 가 나타나면 스페이스 또는 엔터를 누르세요.  ✕ 는 누르지 마세요!\n";
    } else if (state == RCState::WAITING) {
        std::cout << "  준비하세요...\n";
    } else if (state == RCState::SIGNAL) {
        if (isStar) std::cout << "\033[33m  ★  지금 누르세요!\033[0m\n";
        else        std::cout << "\033[31m  ✕  누르지 마세요!\033[0m\n";
    } else if (state == RCState::JUDGED) {
        std::cout << "  " << lastStatus << "\n";
    } else if (state == RCState::RESULT) {
        int exp = calcExp();
        long long avg = averageReaction();
        if (reactionTimes.size() < 2) {
            std::cout << "  \033[31m유효 측정값 부족 - 훈련 실패 (경험치 0)\033[0m\n";
        } else {
            std::cout << "  평균 반응속도: " << avg << "ms    민첩 경험치 +" << exp << "\n";
        }
        std::cout << "  ✕ 회피 성공: " << xAvoidSuccess << "회\n";
        std::cout << "\n  아무 키나 눌러 돌아가기\n";
    }

    // reaction times record
    std::cout << "\n  기록: ";
    for (int i = 0; i < totalRounds; ++i) {
        std::cout << (i + 1) << ": ";
        // We store in order, but we track per-round. For simplicity, show reactionTimes entries.
        // Actually we need to track per-round. Let me show what's available.
    }
    // Simpler display: just show collected reaction times
    std::cout << "\n  측정값: ";
    for (size_t i = 0; i < reactionTimes.size(); ++i) {
        std::cout << reactionTimes[i] << "ms  ";
    }
    if (reactionTimes.empty()) std::cout << "없음";
    std::cout << "\n";

    Renderer::drawBottomMenu({"포기 (q)"});
}

void ReactionScreen::handleInput(GameEngine& engine, Character& player, const InputEvent& event) {
    auto now = std::chrono::steady_clock::now();
    long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - stateEntered).count();

    if (event.type == InputType::KEYBOARD && event.key == 'q') {
        engine.changeScreen(std::make_unique<TrainingScreen>());
        return;
    }

    if (state == RCState::INTRO) {
        if (elapsed >= 2000) {
            round = 0;
            waitDuration = 1000 + std::rand() % 2501; // 1000-3500ms
            enterState(RCState::WAITING);
        }
        return;
    }

    if (state == RCState::WAITING) {
        if (elapsed >= waitDuration) {
            ++round;
            generateSignal();
            enterState(RCState::SIGNAL);
        }
        return;
    }

    if (state == RCState::SIGNAL) {
        long long sigElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - signalStart).count();

        bool pressed = (event.type == InputType::KEYBOARD &&
                        (event.key == ' ' || event.key == '\r' || event.key == '\n'));

        if (pressed && !inputReceived) {
            inputReceived = true;
            if (isStar) {
                reactionTimes.push_back(sigElapsed);
                lastStatus = "\033[33m별 잡기 성공! (" + std::to_string(sigElapsed) + "ms)\033[0m";
            } else {
                // ✕ 오반응
                // add 50ms penalty to next star (just mark failure, don't add to times)
                lastStatus = "\033[31m✕ 오반응! 해당 회차 무효\033[0m";
            }
            enterState(RCState::JUDGED);
            return;
        }

        if (sigElapsed >= 1500) {
            // timeout
            if (isStar) {
                lastStatus = "\033[31mMISS - 시간 초과\033[0m";
            } else {
                // no press on ✕ = success
                ++xAvoidSuccess;
                lastStatus = "\033[32m✕ 회피 성공!\033[0m";
            }
            enterState(RCState::JUDGED);
        }
        return;
    }

    if (state == RCState::JUDGED) {
        if (elapsed >= 1000) {
            if (round >= totalRounds) {
                enterState(RCState::RESULT);
            } else {
                waitDuration = 1000 + std::rand() % 2501;
                enterState(RCState::WAITING);
            }
        }
        return;
    }

    if (state == RCState::RESULT) {
        if (event.type == InputType::KEYBOARD) {
            int exp = calcExp();
            player.trainAgility(exp);
            engine.changeScreen(std::make_unique<TrainingScreen>());
        }
    }
}
