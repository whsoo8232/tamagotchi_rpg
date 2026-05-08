#ifndef EQUIPMENT_SCREEN_H
#define EQUIPMENT_SCREEN_H

#include "Screen.h"
#include <string>

class EquipmentScreen : public Screen {
public:
    void render(const Character& player) override;
    void handleInput(GameEngine& engine, Character& player, const InputEvent& event) override;
private:
    std::string lastAction;
};

#endif
