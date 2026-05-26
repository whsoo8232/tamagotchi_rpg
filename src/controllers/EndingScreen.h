#ifndef ENDING_SCREEN_H
#define ENDING_SCREEN_H

#include "Screen.h"
#include <string>

enum class EndingType { HERO, STARVE, RUNAWAY, TRASH, HOSPITAL };

class EndingScreen : public Screen {
public:
    explicit EndingScreen(EndingType type);
    void render(const Character& player) override;
    void handleInput(GameEngine& engine, Character& player, const InputEvent& event) override;

private:
    EndingType  type;
    int         artIdx;
    std::string message;
    std::string color;
};

#endif
