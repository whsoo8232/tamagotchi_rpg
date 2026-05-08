#ifndef BATTLE_SCREEN_H
#define BATTLE_SCREEN_H

#include "Screen.h"
#include "../models/Monster.h"

class BattleScreen : public Screen {
public:
    BattleScreen();
    void render(const Character& player) override;
    void handleInput(GameEngine& engine, Character& player, const InputEvent& event) override;
private:
    Monster enemy;
    int selectedStage;
    bool inCombat;
    std::string battleLog;
    
    void renderStageSelection(const Character& player);
    void renderCombat(const Character& player);
};

#endif
