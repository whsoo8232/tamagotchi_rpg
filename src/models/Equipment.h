#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <string>
#include <array>

enum class SwordTier { BRANCH=0, WOOD=1, HARD=2, KNIGHT=3, HERO=4 };
enum class ArmorTier { SHABBY=0, PLAIN=1, SOLID=2, KNIGHT=3, HERO=4 };

struct TierInfo {
    std::string name;
    int attackBonus;    // 검: 기본 공격력에 더해지는 평탄 보너스
    int defenseBonus;   // 갑옷: 받는 피해 감소량
    int upgradeCost;    // 강화 비용 (최대 티어면 0)
    int upgradeChance;  // 강화 성공 확률 % (최대 티어면 0)
};

// 인덱스 = (int)SwordTier
static const std::array<TierInfo, 5> SWORD_TABLE = {{
    {"나뭇가지",   0,  0, 300,  70},  // 티어 0 → 1 : 70%
    {"나무 검",    5,  0, 500,  50},  // 티어 1 → 2 : 50%
    {"단단한 검",  15, 0, 700,  30},  // 티어 2 → 3 : 30%
    {"기사의 검",  30, 0, 1000, 10},  // 티어 3 → 4 : 10%
    {"용사의 검",  50, 0, 0,    0 },  // 최대 티어
}};

// 인덱스 = (int)ArmorTier
static const std::array<TierInfo, 5> ARMOR_TABLE = {{
    {"허름한 갑옷", 0, 0,  300,  70},
    {"평범한 갑옷", 0, 5,  500,  50},
    {"단단한 갑옷", 0, 15, 700,  30},
    {"기사의 갑옷", 0, 30, 1000, 10},
    {"용사의 갑옷", 0, 50, 0,    0 },
}};

#endif
