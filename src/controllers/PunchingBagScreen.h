#ifndef PUNCHING_BAG_SCREEN_H
#define PUNCHING_BAG_SCREEN_H

#include "Screen.h"
#include <chrono>
#include <vector>
#include <string>

enum class PBState { INTRO, WAITING, SIGNAL, JUDGMENT, FINAL };

class PunchingBagScreen : public Screen {
public:
    PunchingBagScreen();
    void render(const Character& player) override;
    void handleInput(GameEngine& engine, Character& player, const InputEvent& event) override;

private:
    PBState state;
    std::chrono::steady_clock::time_point stateEntered;
    std::chrono::steady_clock::time_point signalStart;

    int round;        // 1-10
    int totalRounds;
    int score;
    int combo;
    int signalDir;    // 0=up 1=down 2=left 3=right
    int signalPos;    // column offset for scrolling (decreases over time)
    int prevDir;
    bool inputHandled;
    long long inputReactionMs;

    std::string lastJudgment;
    std::vector<std::string> roundResults;

    long long waitDuration; // ms until signal appears

    void nextRound();
    void generateSignal();
    void enterState(PBState s);
    int calcExp() const;
};

#endif
