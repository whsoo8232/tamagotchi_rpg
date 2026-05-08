#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include "Screen.h"

class MainScreen : public Screen {
public:
    void render(const Character& player) override;
    void handleInput(GameEngine& engine, Character& player, const InputEvent& event) override;
};

#endif
