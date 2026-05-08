#ifndef TRAINING_SCREEN_H
#define TRAINING_SCREEN_H

#include "Screen.h"
#include <string>

class TrainingScreen : public Screen {
public:
    void render(const Character& player) override;
    void handleInput(GameEngine& engine, Character& player, const InputEvent& event) override;
private:
    std::string lastAction;
};

#endif
