#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <array>

enum class ItemType {
    STRENGTH_POTION = 0,  // 힘의 포션
    AGILITY_POTION  = 1,  // 민첩의 포션
    ROPE            = 2,  // 밧줄
    SHIELD          = 3,  // 방패
};

static constexpr int ITEM_COUNT     = 4;
static constexpr int MAX_ITEM_STACK = 2;

struct ItemInfo {
    std::string name;
    std::string effect;
    int cost;
};

// 인덱스 = (int)ItemType
static const std::array<ItemInfo, ITEM_COUNT> ITEM_TABLE = {{
    {"힘의 포션",   "승리 시 데미지 2배",           150},
    {"민첩의 포션", "패배 시 회피율 2배 (해당 턴)", 150},
    {"밧줄",        "승리 시 몬스터 회피 무시",      200},
    {"방패",        "패배 시 무조건 방어",           200},
}};

#endif
