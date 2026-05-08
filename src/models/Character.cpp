#include "Character.h"
#include <algorithm>

Character::Character(std::string name) 
    : name(name), hunger(50), happiness(50), cleanliness(50), health(50),
      money(1000), attack(10), hp(100), maxHp(100), agility(5), level(1), exp(0) {}

void Character::feed() {
    hunger += 20;
    happiness += 5;
    clampStats();
}

void Character::bath() {
    cleanliness += 30;
    happiness -= 5;
    clampStats();
}

void Character::play() {
    happiness += 20;
    hunger -= 10;
    cleanliness -= 10;
    clampStats();
}

void Character::treat() {
    health += 30;
    money -= 100;
    clampStats();
}

void Character::trainAttack(int amount) {
    attack += amount;
    hunger -= 5;
    happiness -= 5;
    clampStats();
}

void Character::trainHp(int amount) {
    maxHp += amount;
    hp = maxHp;
    hunger -= 5;
    happiness -= 5;
    clampStats();
}

void Character::trainAgility(int amount) {
    agility += amount;
    hunger -= 5;
    happiness -= 5;
    clampStats();
}

void Character::takeDamage(int damage) {
    hp -= damage;
    if (hp < 0) hp = 0;
}

bool Character::isAlive() const {
    return hp > 0;
}

bool Character::isEndingCondition() const {
    return hunger <= 0 || happiness <= 0 || cleanliness <= 0 || health <= 0;
}

void Character::clampStats() {
    hunger = std::min(100, std::max(0, hunger));
    happiness = std::min(100, std::max(0, happiness));
    cleanliness = std::min(100, std::max(0, cleanliness));
    health = std::min(100, std::max(0, health));
}
