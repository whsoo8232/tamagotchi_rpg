#ifndef SCREEN_H
#define SCREEN_H

#include "../models/Character.h"
#include "InputHandler.h"

class GameEngine;

class Screen {
public:
    virtual ~Screen() {}
    virtual void render(const Character& player) = 0;
    virtual void handleInput(GameEngine& engine, Character& player, const InputEvent& event) = 0;
    virtual bool needsRedraw() const { return false; } // 입력 없이도 재렌더 필요한 경우
};

#endif
