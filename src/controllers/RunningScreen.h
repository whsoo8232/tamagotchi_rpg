#ifndef RUNNING_SCREEN_H
#define RUNNING_SCREEN_H

#include "Screen.h"
#include <chrono>

enum class RNState { IDLE, DONE };

class RunningScreen : public Screen {
public:
    RunningScreen();
    void render(const Character& player) override;
    void handleInput(GameEngine& engine, Character& player, const InputEvent& event) override;
    bool needsRedraw() const override { return stepAnim; }

private:
    RNState state;
    int     runCount;
    bool    stepAnim;
    std::chrono::steady_clock::time_point stepTime;
};

#endif
