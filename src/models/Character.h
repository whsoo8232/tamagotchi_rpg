#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include <array>
#include "Item.h"
#include "Equipment.h"

class Character {
public:
    explicit Character(std::string name);

    // ── 육성 스탯 게터 ─────────────────────────────────────────
    std::string getName()        const { return name; }
    int getHunger()              const { return hunger; }
    int getHappiness()           const { return happiness; }
    int getCleanliness()         const { return cleanliness; }
    int getHealth()              const { return health; }
    int getMoney()               const { return money; }

    // ── 전투 스탯 게터 ─────────────────────────────────────────
    int getBaseAttack()          const { return attack; }
    int getTotalAttack()         const; // attack + 검 보너스
    int getHp()                  const { return hp; }
    int getMaxHp()               const { return maxHp; }
    int getAgility()             const { return agility; }
    int getArmorReduction()      const; // 갑옷 피해 감소

    // ── 레벨/진행도 게터 ───────────────────────────────────────
    int getLevel()               const { return level; }
    int getExp()                 const { return exp; }
    int getClearedStage()        const { return clearedStage; }

    // ── 장비 게터 ──────────────────────────────────────────────
    SwordTier getSwordTier()     const { return swordTier; }
    ArmorTier getArmorTier()     const { return armorTier; }
    std::string getSwordName()   const;
    std::string getArmorName()   const;
    int getSwordUpgradeCost()    const;
    int getSwordUpgradeChance()  const;
    int getArmorUpgradeCost()    const;
    int getArmorUpgradeChance()  const;

    // ── 아이템 게터 ────────────────────────────────────────────
    int  getItemCount(ItemType t) const { return itemCounts[(int)t]; }
    bool hasItem(ItemType t)      const { return itemCounts[(int)t] > 0; }

    // ── 육성 액션 ──────────────────────────────────────────────
    void feed();
    void bath();
    void play();
    void treat();

    // ── 훈련 ───────────────────────────────────────────────────
    void trainAttack(int amount);
    void trainHp(int amount);
    void trainAgility(int amount);

    // ── 전투 ───────────────────────────────────────────────────
    void takeDamage(int rawDamage); // 갑옷 감소 적용 후 HP 차감
    void restoreHp();               // 배틀 후 HP 전회복
    void afterBattle();             // 배틀 후 육성 스탯 감소
    bool isAlive()               const { return hp > 0; }
    bool isEndingCondition()     const;
    void clearStage(int stage);     // clearedStage 갱신

    // ── 장비 강화 ──────────────────────────────────────────────
    bool tryUpgradeSword(); // 성공 true / 실패(비용차감) false
    bool tryUpgradeArmor();

    // ── 아이템 관리 ────────────────────────────────────────────
    bool buyItem(ItemType t);  // 돈·소지 한도 체크, 성공 true
    bool useItem(ItemType t);  // 소지 체크, 성공 true

    // ── 경제 ───────────────────────────────────────────────────
    void setMoney(int amount) { money = amount; }
    void addMoney(int amount) { money += amount; }

private:
    std::string name;

    // 육성 스탯 (0-100)
    int hunger;
    int happiness;
    int cleanliness;
    int health;

    int money;

    // 전투 기본 스탯
    int attack;
    int hp;
    int maxHp;
    int agility;

    // 진행도
    int level;
    int exp;
    int clearedStage; // 0 = 없음, 1~5

    // 장비
    SwordTier swordTier;
    ArmorTier armorTier;

    // 아이템 재고 [STRENGTH_POTION, AGILITY_POTION, ROPE, SHIELD]
    std::array<int, ITEM_COUNT> itemCounts;

    void clampStats();
};

#endif
