#include "PunchingBagScreen.h"
#include "GameEngine.h"
#include "TrainingScreen.h"
#include "../views/Renderer.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iomanip>

static const char* DIR_SYMBOLS[] = { " ↑ ", " ↓ ", " ← ", " → " };
static const char* DIR_NAMES[]   = { "UP", "DOWN", "LEFT", "RIGHT" };

PunchingBagScreen::PunchingBagScreen()
    : state(PBState::INTRO), round(0), totalRounds(10),
      score(0), combo(0), signalDir(0), signalPos(0), prevDir(-1),
      inputHandled(false), inputReactionMs(0), waitDuration(1500) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    stateEntered = std::chrono::steady_clock::now();
}

void PunchingBagScreen::enterState(PBState s) {
    state = s;
    stateEntered = std::chrono::steady_clock::now();
}

void PunchingBagScreen::generateSignal() {
    int dir;
    do {
        dir = std::rand() % 4;
    } while (dir == prevDir);
    prevDir = signalDir = dir;
    signalPos = 36; // starts at right edge of signal area
    inputHandled = false;
    inputReactionMs = -1;
    signalStart = std::chrono::steady_clock::now();
}

void PunchingBagScreen::nextRound() {
    ++round;
    if (round > totalRounds) {
        enterState(PBState::FINAL);
        return;
    }
    waitDuration = 800 + std::rand() % 1701; // 800..2500 ms
    enterState(PBState::WAITING);
}

int PunchingBagScreen::calcExp() const {
    // max score = totalRounds * 3 = 30. scale to 0-15 exp
    if (score >= 25) return 15;
    if (score >= 18) return 12;
    if (score >= 12) return 8;
    if (score >= 6)  return 4;
    if (score > 0)   return 1;
    return 0;
}

void PunchingBagScreen::render(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player, "2026-05-04");

    auto now = std::chrono::steady_clock::now();
    long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - stateEntered).count();

    // ── title bar
    std::cout << "\n";
    {
        std::string title = "[ 샌드백 치기 ]";
        std::string rounds = " 라운드: " + std::to_string(std::max(round, 1)) + "/" + std::to_string(totalRounds);
        std::string comboStr = " 콤보: " + std::to_string(combo);
        std::string bar = "  " + title + "    " + rounds + "    " + comboStr;
        std::cout << Renderer::padRight(bar, 120) << "\n";
    }
    std::cout << "------------------------------------------------------------------------------------------------------------------------\n";

    // ── character area
    std::cout << "\n";
    {
        const int areaW = 116;
        std::string charPart = "   [캐릭터]";
        std::string vsPart   = "  VS  ";
        std::string bagPart  = "[샌드백]   ";
        int totalArt = Renderer::getDisplayWidth(charPart + vsPart + bagPart);
        int pad = (areaW - totalArt) / 2;
        std::cout << std::string(std::max(0, pad), ' ') << charPart << vsPart << bagPart << "\n";
    }
    std::cout << "\n";

    // ── signal area (40-char wide, arrow scrolls right→left)
    std::cout << "  신호: ";
    if (state == PBState::SIGNAL) {
        long long sigElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - signalStart).count();
        // position: starts at col 36, moves left, ~7 chars/100ms = 0.07 cols/ms
        int pos = 36 - static_cast<int>(sigElapsed * 36 / 500);
        if (pos < 0) pos = 0;
        const char* sym = DIR_SYMBOLS[signalDir];
        int symW = 3; // " ← " etc.
        std::cout << std::string(pos, ' ') << sym;
        int remaining = 40 - pos - symW;
        if (remaining > 0) std::cout << std::string(remaining, ' ');
    } else {
        std::cout << std::string(40, ' ');
    }
    std::cout << "\n\n";

    // ── judgment display
    std::cout << "  판정: ";
    if (!lastJudgment.empty()) {
        if (lastJudgment == "PERFECT") std::cout << "\033[33m" << lastJudgment << "\033[0m";
        else if (lastJudgment == "GOOD")    std::cout << "\033[32m" << lastJudgment << "\033[0m";
        else if (lastJudgment == "OK")      std::cout << "\033[36m" << lastJudgment << "\033[0m";
        else if (lastJudgment == "MISS")    std::cout << "\033[31m" << lastJudgment << "\033[0m";
        else if (lastJudgment == "BAD")     std::cout << "\033[31m" << lastJudgment << "\033[0m";
        else std::cout << lastJudgment;
    }
    std::cout << "\n";
    std::cout << "  점수: " << score << "\n\n";

    // ── round results (icons)
    std::cout << "  기록: ";
    for (const auto& r : roundResults) {
        std::cout << r << " ";
    }
    for (int i = roundResults.size(); i < totalRounds; ++i) {
        std::cout << "○ ";
    }
    std::cout << "\n";

    // ── state messages
    std::cout << "\n";
    if (state == PBState::INTRO) {
        std::cout << "  방향키를 눌러 신호에 맞춰 치세요!  잠시 후 시작합니다...\n";
    } else if (state == PBState::WAITING) {
        std::cout << "  준비하세요...\n";
    } else if (state == PBState::SIGNAL) {
        std::cout << "  지금 누르세요! → " << DIR_NAMES[signalDir] << "\n";
    } else if (state == PBState::FINAL) {
        int exp = calcExp();
        std::cout << "  훈련 완료!  최종 점수: " << score << "  공격력 경험치 +" << exp << "\n";
        std::cout << "  아무 키나 눌러 돌아가기\n";
    }

    Renderer::drawBottomMenu({"포기 (q)"});
}

void PunchingBagScreen::handleInput(GameEngine& engine, Character& player, const InputEvent& event) {
    auto now = std::chrono::steady_clock::now();
    long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - stateEntered).count();

    if (state == PBState::INTRO) {
        if (elapsed >= 2000) {
            round = 0;
            nextRound();
        }
        return;
    }

    if (state == PBState::WAITING) {
        if (elapsed >= waitDuration) {
            generateSignal();
            enterState(PBState::SIGNAL);
        }
        return;
    }

    if (state == PBState::SIGNAL) {
        long long sigElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - signalStart).count();

        bool dirPressed = false;
        int pressedDir = -1;
        if (event.type == InputType::KEYBOARD && event.special != SpecialKey::NONE) {
            if (event.special == SpecialKey::UP)    { dirPressed = true; pressedDir = 0; }
            if (event.special == SpecialKey::DOWN)  { dirPressed = true; pressedDir = 1; }
            if (event.special == SpecialKey::LEFT)  { dirPressed = true; pressedDir = 2; }
            if (event.special == SpecialKey::RIGHT) { dirPressed = true; pressedDir = 3; }
        }

        if (dirPressed && !inputHandled) {
            inputHandled = true;
            inputReactionMs = sigElapsed;
            if (pressedDir == signalDir) {
                if (sigElapsed <= 200)       { lastJudgment = "PERFECT"; score += 3; ++combo; roundResults.push_back("★"); }
                else if (sigElapsed <= 400)  { lastJudgment = "GOOD";    score += 2; ++combo; roundResults.push_back("★"); }
                else                          { lastJudgment = "OK";      score += 1; ++combo; roundResults.push_back("○"); }
            } else {
                lastJudgment = "BAD";
                score -= 1;
                combo = 0;
                roundResults.push_back("✕");
            }
            enterState(PBState::JUDGMENT);
            return;
        }

        if (sigElapsed >= 500) {
            if (!inputHandled) {
                lastJudgment = "MISS";
                combo = 0;
                roundResults.push_back("✕");
            }
            enterState(PBState::JUDGMENT);
        }
        return;
    }

    if (state == PBState::JUDGMENT) {
        if (elapsed >= 800) {
            nextRound();
        }
        return;
    }

    if (event.type == InputType::KEYBOARD && event.key == 'q') {
        engine.changeScreen(std::make_unique<TrainingScreen>());
        return;
    }

    if (state == PBState::FINAL) {
        if (event.type == InputType::KEYBOARD) {
            int exp = calcExp();
            player.trainAttack(exp);
            engine.changeScreen(std::make_unique<TrainingScreen>());
        }
    }
}
