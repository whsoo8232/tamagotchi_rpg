#include "Character.h"
#include <algorithm>
#include <cstdlib>

Character::Character(std::string name)
    : name(name),
      hunger(50), happiness(50), cleanliness(50), health(50),
      money(1000),
      attack(10), hp(100), maxHp(100), agility(5),
      level(1), exp(0), clearedStage(0),
      swordTier(SwordTier::BRANCH), armorTier(ArmorTier::SHABBY),
      itemCounts{0, 0, 0, 0} {}

// ── 전투 스탯 계산 ──────────────────────────────────────────────────

int Character::getTotalAttack() const {
    return attack + SWORD_TABLE[(int)swordTier].attackBonus;
}

int Character::getArmorReduction() const {
    return ARMOR_TABLE[(int)armorTier].defenseBonus;
}

// ── 장비 정보 ──────────────────────────────────────────────────────

std::string Character::getSwordName() const {
    return SWORD_TABLE[(int)swordTier].name;
}

std::string Character::getArmorName() const {
    return ARMOR_TABLE[(int)armorTier].name;
}

int Character::getSwordUpgradeCost() const {
    return SWORD_TABLE[(int)swordTier].upgradeCost;
}

int Character::getSwordUpgradeChance() const {
    return SWORD_TABLE[(int)swordTier].upgradeChance;
}

int Character::getArmorUpgradeCost() const {
    return ARMOR_TABLE[(int)armorTier].upgradeCost;
}

int Character::getArmorUpgradeChance() const {
    return ARMOR_TABLE[(int)armorTier].upgradeChance;
}

// ── 육성 액션 ──────────────────────────────────────────────────────

void Character::feed() {
    hunger    += 20;
    happiness += 5;
    clampStats();
}

void Character::bath() {
    cleanliness += 30;
    happiness   -= 5;
    clampStats();
}

void Character::play() {
    happiness   += 20;
    hunger      -= 10;
    cleanliness -= 10;
    clampStats();
}

void Character::treat() {
    health += 30;
    money  -= 100;
    clampStats();
}

// ── 훈련 ───────────────────────────────────────────────────────────

void Character::trainAttack(int amount) {
    attack    += amount;
    hunger    -= 5;
    happiness -= 5;
    clampStats();
}

void Character::trainHp(int amount) {
    maxHp     += amount;
    hp         = maxHp;
    hunger    -= 5;
    happiness -= 5;
    clampStats();
}

void Character::trainAgility(int amount) {
    agility   += amount;
    hunger    -= 5;
    happiness -= 5;
    clampStats();
}

// ── 전투 ───────────────────────────────────────────────────────────

void Character::takeDamage(int rawDamage) {
    int reduced = std::max(0, rawDamage - getArmorReduction());
    hp -= reduced;
    if (hp < 0) hp = 0;
}

void Character::restoreHp() {
    hp = maxHp;
}

bool Character::isEndingCondition() const {
    return hunger <= 0 || happiness <= 0 || cleanliness <= 0 || health <= 0;
}

void Character::clearStage(int stage) {
    if (stage > clearedStage) clearedStage = stage;
}

// ── 장비 강화 ──────────────────────────────────────────────────────

bool Character::tryUpgradeSword() {
    int tier = (int)swordTier;
    if (tier >= 4) return false;
    const auto& info = SWORD_TABLE[tier];
    if (money < info.upgradeCost) return false;
    money -= info.upgradeCost;
    if (std::rand() % 100 < info.upgradeChance) {
        swordTier = static_cast<SwordTier>(tier + 1);
        return true;
    }
    return false;
}

bool Character::tryUpgradeArmor() {
    int tier = (int)armorTier;
    if (tier >= 4) return false;
    const auto& info = ARMOR_TABLE[tier];
    if (money < info.upgradeCost) return false;
    money -= info.upgradeCost;
    if (std::rand() % 100 < info.upgradeChance) {
        armorTier = static_cast<ArmorTier>(tier + 1);
        return true;
    }
    return false;
}

// ── 아이템 관리 ────────────────────────────────────────────────────

bool Character::buyItem(ItemType t) {
    int idx  = (int)t;
    if (itemCounts[idx] >= MAX_ITEM_STACK) return false;
    int cost = ITEM_TABLE[idx].cost;
    if (money < cost) return false;
    money -= cost;
    itemCounts[idx]++;
    return true;
}

bool Character::useItem(ItemType t) {
    int idx = (int)t;
    if (itemCounts[idx] <= 0) return false;
    itemCounts[idx]--;
    return true;
}

// ── 내부 유틸 ──────────────────────────────────────────────────────

void Character::clampStats() {
    hunger      = std::min(100, std::max(0, hunger));
    happiness   = std::min(100, std::max(0, happiness));
    cleanliness = std::min(100, std::max(0, cleanliness));
    health      = std::min(100, std::max(0, health));
    money       = std::max(0, money);
}
