#ifndef RUNNING_SCREEN_H
#define RUNNING_SCREEN_H

#include "Screen.h"
#include <chrono>
#include <vector>
#include <string>

enum class RNState { INTRO, COUNTDOWN, RUNNING, RESULT };

class RunningScreen : public Screen {
public:
    RunningScreen();
    void render(const Character& player) override;
    void handleInput(GameEngine& engine, Character& player, const InputEvent& event) override;

private:
    RNState state;
    std::chrono::steady_clock::time_point stateEntered;
    std::chrono::steady_clock::time_point runStart;

    int keyCount;
    int countdown;     // 3→2→1→0(GO)
    int animFrame;     // 0 or 1
    char lastKey;
    int totalTimeMs;   // 12000ms

    void enterState(RNState s);
    int calcExp() const;

    static const std::vector<std::vector<std::string>> FRAMES;
};

#endif
