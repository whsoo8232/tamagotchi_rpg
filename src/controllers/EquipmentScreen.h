#ifndef EQUIPMENT_SCREEN_H
#define EQUIPMENT_SCREEN_H

#include "Screen.h"
#include <string>

enum class EquipState { MAIN, ITEM_SHOP };

class EquipmentScreen : public Screen {
public:
    EquipmentScreen();
    void render(const Character& player) override;
    void handleInput(GameEngine& engine, Character& player, const InputEvent& event) override;

private:
    EquipState  state;
    std::string lastMsg;

    void renderMain(const Character& player);
    void renderItemShop(const Character& player);

    void handleMain(GameEngine& engine, Character& player, const InputEvent& event);
    void handleItemShop(GameEngine& engine, Character& player, const InputEvent& event);

    // 버튼 인덱스(0-based) → choice 변환 공통
    int mouseChoice(const InputEvent& event, int btnCount);
};

#endif
