#ifndef PUNCHING_BAG_SCREEN_H
#define PUNCHING_BAG_SCREEN_H

#include "Screen.h"
#include <chrono>

enum class PBState { IDLE, DONE };

class PunchingBagScreen : public Screen {
public:
    PunchingBagScreen();
    void render(const Character& player) override;
    void handleInput(GameEngine& engine, Character& player, const InputEvent& event) override;
    bool needsRedraw() const override { return punchAnim; }

private:
    PBState  state;
    int      punchCount;
    bool     punchAnim;
    std::chrono::steady_clock::time_point punchTime;
};

#endif
