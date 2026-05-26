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
    if (event.type == InputType::MOUSE_PRESS && event.button == 0 && event.y >= 41) {
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

    // ── 검/갑옷 섹션 ──────────────────────────────────────────────
    // [아트:44][텍스트:36]│[아트:44][텍스트:36] = 161col
    static const int ART_W   = 38;
    static const int TEXT_W  = 42;
    static const int MAX_ART = 16; // 아트 최대 표시 행

    int sT = (int)player.getSwordTier();
    int aT = (int)player.getArmorTier();
    bool sMax = (sT >= 4);
    bool aMax = (aT >= 4);

    auto tierBar = [](int tier) -> std::string {
        std::string s = "[";
        for (int i = 0; i < 5; ++i) s += (i <= tier) ? "■" : "□";
        return s + "]";
    };

    std::vector<std::string> sTxt = {
        "[ 검 ]", "",
        "현재: " + SWORD_TABLE[sT].name + " " + tierBar(sT),
        "공격 보너스: +" + std::to_string(SWORD_TABLE[sT].attackBonus), "",
        sMax ? "[최대 강화 완료]" : ("다음: " + SWORD_TABLE[sT+1].name),
        sMax ? "" : ("비용: " + std::to_string(SWORD_TABLE[sT].upgradeCost) + "원"),
        sMax ? "" : ("성공: " + std::to_string(SWORD_TABLE[sT].upgradeChance) + "%"),
    };
    std::vector<std::string> aTxt = {
        "[ 갑옷 ]", "",
        "현재: " + ARMOR_TABLE[aT].name + " " + tierBar(aT),
        "방어 감소: -" + std::to_string(ARMOR_TABLE[aT].defenseBonus), "",
        aMax ? "[최대 강화 완료]" : ("다음: " + ARMOR_TABLE[aT+1].name),
        aMax ? "" : ("비용: " + std::to_string(ARMOR_TABLE[aT].upgradeCost) + "원"),
        aMax ? "" : ("성공: " + std::to_string(ARMOR_TABLE[aT].upgradeChance) + "%"),
    };

    const auto& sArt = Renderer::getSwordArt(sT + 1);
    const auto& aArt = Renderer::getArmorArt(aT + 1);
    int sRows = std::min((int)sArt.size(), MAX_ART);
    int aRows = std::min((int)aArt.size(), MAX_ART);
    int maxRows = std::max(std::max(sRows, aRows), (int)sTxt.size());

    // 아트 크롭 시작 인덱스 (전체 아트 중 중앙 MAX_ART행 추출)
    int sSrcOff = ((int)sArt.size() - sRows) / 2;
    int aSrcOff = ((int)aArt.size() - aRows) / 2;

    // 수직 중앙 정렬 오프셋
    int sArtTop = (maxRows - sRows) / 2;
    int aArtTop = (maxRows - aRows) / 2;

    // 아트 수평 너비 → ART_W 안에서 중앙 정렬
    int sArtW = 0, aArtW = 0;
    for (const auto& l : sArt) sArtW = std::max(sArtW, Renderer::getDisplayWidth(l));
    for (const auto& l : aArt) aArtW = std::max(aArtW, Renderer::getDisplayWidth(l));
    int sIndent = std::max(0, (ART_W - sArtW) / 2);
    int aIndent = std::max(0, (ART_W - aArtW) / 2);

    std::cout << "\n"; // 검/갑옷 위 여백
    for (int row = 0; row < maxRows; ++row) {
        // 검 아트
        {
            int r = row - sArtTop;
            std::string cell;
            if (r >= 0 && r < sRows)
                cell = std::string(sIndent, ' ') + sArt[sSrcOff + r];
            std::cout << Renderer::padRight(cell, ART_W);
        }
        // 검 텍스트
        {
            std::string cell = (row < (int)sTxt.size()) ? " " + sTxt[row] : "";
            std::cout << Renderer::padRight(cell, TEXT_W);
        }
        std::cout << "│";
        // 갑옷 아트
        {
            int r = row - aArtTop;
            std::string cell;
            if (r >= 0 && r < aRows)
                cell = std::string(aIndent, ' ') + aArt[aSrcOff + r];
            std::cout << Renderer::padRight(cell, ART_W);
        }
        // 갑옷 텍스트
        {
            std::string cell = (row < (int)aTxt.size()) ? " " + aTxt[row] : "";
            std::cout << Renderer::padRight(cell, TEXT_W);
        }
        std::cout << "\n";
    }

    std::cout << "\n"; // 검/갑옷 아래 여백
    // 구분선 (ART_W+TEXT_W=80, ─×80┼─×80)
    {
        std::string hline;
        for (int i = 0; i < ART_W + TEXT_W; ++i) hline += "─";
        std::cout << hline << "┼" << hline << "\n";
    }

    // ── 아이템 인벤토리 ─────────────────────────────────────────
    static const int ITEM_COL = 40; // 4 × 40 = 160
    std::cout << "\n  [ 아이템 인벤토리 ]\n\n"; // 위아래 여백

    int maxItemArtRows = 0;
    std::vector<const std::vector<std::string>*> itemArts;
    std::vector<int> itemArtWidths(ITEM_COUNT, 0);
    for (int i = 0; i < ITEM_COUNT; ++i) {
        itemArts.push_back(&Renderer::getItemArt(i));
        maxItemArtRows = std::max(maxItemArtRows, (int)itemArts.back()->size());
        for (const auto& line : *itemArts.back())
            itemArtWidths[i] = std::max(itemArtWidths[i], Renderer::getDisplayWidth(line));
    }

    for (int row = 0; row < maxItemArtRows; ++row) {
        for (int i = 0; i < ITEM_COUNT; ++i) {
            const auto& art = *itemArts[i];
            int topPad = (maxItemArtRows - (int)art.size()) / 2;
            int r = row - topPad;
            std::string cell;
            if (r >= 0 && r < (int)art.size()) {
                int indent = std::max(0, (ITEM_COL - itemArtWidths[i]) / 2);
                cell = std::string(indent, ' ') + art[r];
            }
            std::cout << Renderer::padRight(cell, ITEM_COL);
        }
        std::cout << "\n";
    }

    for (int i = 0; i < ITEM_COUNT; ++i) {
        int cnt = player.getItemCount((ItemType)i);
        std::string cell = ITEM_TABLE[i].name + " " + std::to_string(cnt) + "/" + std::to_string(MAX_ITEM_STACK);
        if (cnt == 0) std::cout << "\033[90m";
        std::cout << Renderer::center(cell, ITEM_COL);
        if (cnt == 0) std::cout << Renderer::RESET;
    }
    std::cout << "\n";

    for (int i = 0; i < ITEM_COUNT; ++i) {
        int cnt = player.getItemCount((ItemType)i);
        if (cnt == 0) std::cout << "\033[90m";
        std::cout << Renderer::center(ITEM_TABLE[i].effect, ITEM_COL);
        if (cnt == 0) std::cout << Renderer::RESET;
    }
    std::cout << "\n";

    // 1(검위) + maxRows(≤16) + 1(검아래) + 1(sep) + 1(인벤위) + 1(인벤헤더) + 1(인벤아래)
    // + maxItemArtRows(≤11) + 1(이름) + 1(효과) = maxRows + maxItemArtRows + 8 ≤ 35
    int usedRows = maxRows + maxItemArtRows + 8;
    if (!lastMsg.empty()) {
        Renderer::drawMessage(lastMsg);
        Renderer::fillContent(usedRows + 2);
    } else {
        Renderer::fillContent(usedRows);
    }

    Renderer::drawBottomMenu({"검 강화", "갑옷 강화", "아이템 구매", "돌아가기"});
}

void EquipmentScreen::renderItemShop(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    static const int COL_W = 40; // 4 × 40 = 160

    // 헤더 (3행)
    std::cout << "\n" << Renderer::center("[ 아이템 상점 ]", 161) << "\n\n";

    // 아트 로드 + 최대 높이·너비 계산
    int maxArtRows = 0;
    std::vector<const std::vector<std::string>*> arts;
    std::vector<int> artWidths(ITEM_COUNT, 0);
    for (int i = 0; i < ITEM_COUNT; ++i) {
        arts.push_back(&Renderer::getItemArt(i));
        maxArtRows = std::max(maxArtRows, (int)arts.back()->size());
        for (const auto& line : *arts.back())
            artWidths[i] = std::max(artWidths[i], Renderer::getDisplayWidth(line));
    }

    // 4열 아트 출력
    for (int row = 0; row < maxArtRows; ++row) {
        for (int i = 0; i < ITEM_COUNT; ++i) {
            const auto& art = *arts[i];
            int topPad = (maxArtRows - (int)art.size()) / 2;
            int r = row - topPad;
            std::string cell;
            if (r >= 0 && r < (int)art.size()) {
                int indent = std::max(0, (COL_W - artWidths[i]) / 2);
                cell = std::string(indent, ' ') + art[r];
            }
            std::cout << Renderer::padRight(cell, COL_W);
        }
        std::cout << "\n";
    }

    std::cout << "\n";

    // 이름 행
    for (int i = 0; i < ITEM_COUNT; ++i) {
        int cnt = player.getItemCount((ItemType)i);
        bool full = (cnt >= MAX_ITEM_STACK);
        if (full) std::cout << "\033[90m";
        std::cout << Renderer::center(ITEM_TABLE[i].name, COL_W);
        if (full) std::cout << Renderer::RESET;
    }
    std::cout << "\n";

    // 보유/가격 행
    for (int i = 0; i < ITEM_COUNT; ++i) {
        int cnt = player.getItemCount((ItemType)i);
        bool full = (cnt >= MAX_ITEM_STACK);
        std::string info = std::to_string(cnt) + "/" + std::to_string(MAX_ITEM_STACK)
                         + "개  " + std::to_string(ITEM_TABLE[i].cost) + "원";
        if (full) std::cout << "\033[90m";
        std::cout << Renderer::center(info, COL_W);
        if (full) std::cout << Renderer::RESET;
    }
    std::cout << "\n";

    // 효과 행
    for (int i = 0; i < ITEM_COUNT; ++i) {
        int cnt = player.getItemCount((ItemType)i);
        bool full = (cnt >= MAX_ITEM_STACK);
        if (full) std::cout << "\033[90m";
        std::cout << Renderer::center(ITEM_TABLE[i].effect, COL_W);
        if (full) std::cout << Renderer::RESET;
    }
    std::cout << "\n";

    // 1+1+1(헤더) + maxArtRows + 1(blank) + 3(이름/가격/효과) = maxArtRows+7
    int usedRows = 7 + maxArtRows;
    if (!lastMsg.empty()) {
        Renderer::drawMessage(lastMsg);
        Renderer::fillContent(usedRows + 2);
    } else {
        Renderer::fillContent(usedRows);
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
