#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include "Screen.h"
#include <chrono>

class MainScreen : public Screen {
public:
    MainScreen();
    void render(const Character& player) override;
    void handleInput(GameEngine& engine, Character& player, const InputEvent& event) override;
    bool needsRedraw() const override { return bubbleActive; }

private:
    bool bubbleActive;
    int  bubbleIdx;
    std::chrono::steady_clock::time_point bubbleStart;
};

#endif
