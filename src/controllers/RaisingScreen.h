#ifndef RAISING_SCREEN_H
#define RAISING_SCREEN_H

#include "Screen.h"
#include <string>
#include <chrono>

class RaisingScreen : public Screen {
public:
    void render(const Character& player) override;
    void handleInput(GameEngine& engine, Character& player, const InputEvent& event) override;
private:
    std::string lastAction;
    bool animating = false;
    int animType = 0;
    std::chrono::steady_clock::time_point animStart;
};

#endif
