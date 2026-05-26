#include "EquipmentScreen.h"
#include "GameEngine.h"
#include "MainScreen.h"
#include "../views/Renderer.h"
#include "../models/Equipment.h"
#include <iostream>
#include <cstdlib>

EquipmentScreen::EquipmentScreen()
    : state(EquipState::MAIN), lastMsg("") {
}

int EquipmentScreen::mouseChoice(const InputEvent& event, int btnCount) {
    if (event.type == InputType::MOUSE_PRESS && event.button == 0 && event.y >= 31) {
        int w = 158 / btnCount;
        return (event.x - 1) / w; // 0-indexed
    }
    return -1;
}

// ══════════════════════════════════════════════════════════════════
//  render
// ══════════════════════════════════════════════════════════════════

void EquipmentScreen::render(const Character& player) {
    if (state == EquipState::MAIN)      renderMain(player);
    else                                renderItemShop(player);
}

void EquipmentScreen::renderMain(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    // ── 2컬럼 레이아웃: 검(58) │ 갑옷(58) ─────────────────────
    auto tierBar = [](int tier) -> std::string {
        std::string s = "[";
        for (int i = 0; i < 5; ++i) s += (i <= tier) ? "■" : "□";
        return s + "]";
    };

    int swordT = (int)player.getSwordTier();
    int armorT = (int)player.getArmorTier();
    bool swordMax = (swordT >= 4);
    bool armorMax = (armorT >= 4);

    std::vector<std::pair<std::string,std::string>> rows = {
        {"  [ 검 ]", "  [ 갑옷 ]"},
        {"", ""},
        {"  현재: " + SWORD_TABLE[swordT].name + " " + tierBar(swordT),
         "  현재: " + ARMOR_TABLE[armorT].name + " " + tierBar(armorT)},
        {"  공격 보너스: +" + std::to_string(SWORD_TABLE[swordT].attackBonus),
         "  방어 감소  : -" + std::to_string(ARMOR_TABLE[armorT].defenseBonus)},
        {"", ""},
        {swordMax ? "  [최대 강화 완료]"
                  : ("  다음: " + SWORD_TABLE[swordT+1].name),
         armorMax ? "  [최대 강화 완료]"
                  : ("  다음: " + ARMOR_TABLE[armorT+1].name)},
        {swordMax ? "" : ("  비용: " + std::to_string(SWORD_TABLE[swordT].upgradeCost) + "원"),
         armorMax ? "" : ("  비용: " + std::to_string(ARMOR_TABLE[armorT].upgradeCost) + "원")},
        {swordMax ? "" : ("  성공: " + std::to_string(SWORD_TABLE[swordT].upgradeChance) + "%"),
         armorMax ? "" : ("  성공: " + std::to_string(ARMOR_TABLE[armorT].upgradeChance) + "%")},
    };

    for (int i = 0; i < (int)rows.size(); ++i) {
        std::cout << Renderer::padRight(rows[i].first, 59) << "│"
                  << rows[i].second << "\n";
    }
    std::cout << "─────────────────────────────────────────────────────────┼────────────────────────────────────────────────────────────\n";

    // ── 아이템 인벤토리 ─────────────────────────────────────────
    std::cout << "  [ 아이템 인벤토리 ]\n\n";
    for (int i = 0; i < ITEM_COUNT; ++i) {
        const auto& info = ITEM_TABLE[i];
        int cnt = player.getItemCount((ItemType)i);
        std::string line = "  [" + std::to_string(i+1) + "] "
                         + Renderer::padRight(info.name, 14)
                         + " " + std::to_string(cnt) + "/" + std::to_string(MAX_ITEM_STACK)
                         + "  " + info.effect;
        std::cout << (cnt == 0 ? "\033[90m" : "") << line << (cnt == 0 ? Renderer::RESET : "") << "\n";
    }
    std::cout << "\n  소지금: " << player.getMoney() << "원\n";
    // 8(rows)+1(─)+2(인벤 헤더)+4(아이템)+2(소지금) = 17줄 사용
    if (!lastMsg.empty()) {
        Renderer::drawMessage(lastMsg);  // 2줄
        Renderer::fillContent(19);        // 17+2=19 → 26
    } else {
        Renderer::fillContent(17);        // 17 → 26
    }

    Renderer::drawBottomMenu({"검 강화", "갑옷 강화", "아이템 구매", "돌아가기"});
}

void EquipmentScreen::renderItemShop(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    std::cout << "\n";
    std::cout << "  ┌─────────────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "  │                          [ 아이템 상점 ]                                    │\n";
    std::cout << "  │                     종류별 최대 " << MAX_ITEM_STACK << "개 소지 가능                              │\n";
    std::cout << "  ├──────┬────────────────┬──────┬──────┬────────────────────────────────────────┤\n";
    std::cout << "  │ 번호 │ 이름           │ 보유 │ 가격 │ 효과                                   │\n";
    std::cout << "  ├──────┼────────────────┼──────┼──────┼────────────────────────────────────────┤\n";

    for (int i = 0; i < ITEM_COUNT; ++i) {
        const auto& info = ITEM_TABLE[i];
        int cnt  = player.getItemCount((ItemType)i);
        bool max = (cnt >= MAX_ITEM_STACK);
        std::string row = "  │  [" + std::to_string(i+1) + "] │ "
                        + Renderer::padRight(info.name, 14) + " │  "
                        + std::to_string(cnt) + "/" + std::to_string(MAX_ITEM_STACK) + " │ "
                        + Renderer::padLeft(std::to_string(info.cost), 4) + "원│ "
                        + Renderer::padRight(info.effect, 38) + "│";
        if (max)
            std::cout << "\033[90m" << row << Renderer::RESET << "\n";
        else
            std::cout << row << "\n";
    }
    std::cout << "  └──────┴────────────────┴──────┴──────┴────────────────────────────────────────┘\n";

    std::cout << "\n  소지금: " << player.getMoney() << "원\n";
    // 1+6+4+1+2 = 14줄 사용

    if (!lastMsg.empty()) {
        Renderer::drawMessage(lastMsg);  // 2줄
        Renderer::fillContent(16);        // 14+2=16 → 26
    } else {
        Renderer::fillContent(14);        // 14 → 26
    }

    Renderer::drawBottomMenu({"힘의 포션", "민첩의 포션", "밧줄", "방패", "돌아가기"});
}

// ══════════════════════════════════════════════════════════════════
//  handleInput
// ══════════════════════════════════════════════════════════════════

void EquipmentScreen::handleInput(GameEngine& engine, Character& player, const InputEvent& event) {
    if (state == EquipState::MAIN)      handleMain(engine, player, event);
    else                                handleItemShop(engine, player, event);
}

void EquipmentScreen::handleMain(GameEngine& engine, Character& player, const InputEvent& event) {
    int choice = -1;
    {
        int btn = mouseChoice(event, 4);
        if (btn == 0) choice = 1;
        else if (btn == 1) choice = 2;
        else if (btn == 2) choice = 3;
        else if (btn == 3) choice = 0;
    }

    if (choice == 1) {
        if ((int)player.getSwordTier() >= 4) {
            lastMsg = "이미 최대 단계 검입니다!";
        } else if (player.getMoney() < player.getSwordUpgradeCost()) {
            lastMsg = "돈이 부족합니다! (" + std::to_string(player.getSwordUpgradeCost()) + "원 필요)";
        } else {
            bool success = player.tryUpgradeSword();
            if (success)
                lastMsg = "검 강화 성공! → " + player.getSwordName();
            else
                lastMsg = "검 강화 실패... (비용 차감됨)";
        }
    } else if (choice == 2) {
        if ((int)player.getArmorTier() >= 4) {
            lastMsg = "이미 최대 단계 갑옷입니다!";
        } else if (player.getMoney() < player.getArmorUpgradeCost()) {
            lastMsg = "돈이 부족합니다! (" + std::to_string(player.getArmorUpgradeCost()) + "원 필요)";
        } else {
            bool success = player.tryUpgradeArmor();
            if (success)
                lastMsg = "갑옷 강화 성공! → " + player.getArmorName();
            else
                lastMsg = "갑옷 강화 실패... (비용 차감됨)";
        }
    } else if (choice == 3) {
        lastMsg = "";
        state = EquipState::ITEM_SHOP;
    } else if (choice == 0) {
        engine.changeScreen(std::make_unique<MainScreen>());
    }
}

void EquipmentScreen::handleItemShop(GameEngine& engine, Character& player, const InputEvent& event) {
    int choice = -1;
    {
        int btn = mouseChoice(event, 5);
        if (btn >= 0 && btn < ITEM_COUNT) choice = btn;
        else if (btn == ITEM_COUNT)       choice = ITEM_COUNT;
    }

    if (choice >= 0 && choice < ITEM_COUNT) {
        ItemType t = (ItemType)choice;
        if (player.getItemCount(t) >= MAX_ITEM_STACK) {
            lastMsg = ITEM_TABLE[choice].name + " 은(는) 이미 최대 보유 중입니다.";
        } else if (player.getMoney() < ITEM_TABLE[choice].cost) {
            lastMsg = "돈이 부족합니다! (" + std::to_string(ITEM_TABLE[choice].cost) + "원 필요)";
        } else {
            player.buyItem(t);
            lastMsg = ITEM_TABLE[choice].name + " 구매 완료!";
        }
    } else if (choice == ITEM_COUNT) {
        lastMsg = "";
        state = EquipState::MAIN;
    }
}
