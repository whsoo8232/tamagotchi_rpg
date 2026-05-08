#include "Monster.h"
#include <fstream>
#include <sstream>

Monster::Monster(std::string name, int hp, int attack) 
    : name(name), hp(hp), attack(attack), evasion(5), ascii("") {}

void Monster::takeDamage(int damage) {
    hp -= damage;
    if (hp < 0) hp = 0;
}

Monster Monster::generateStageMonster(int stage) {
    Monster m("슬라임", 10, 1);
    switch(stage) {
        case 1: m = Monster("꼬마 공룡", 1000, 100); m.evasion = 10; break;
        case 2: m = Monster("뿔 공룡", 2000, 200); m.evasion = 20; break;
        case 3: m = Monster("날개 공룡", 3000, 300); m.evasion = 30; break;
        case 4: m = Monster("목 긴 공룡", 4000, 400); m.evasion = 40; break;
        case 5: m = Monster("티라노사우르스", 5000, 500); m.evasion = 50; break;
    }

    std::ifstream file("dots/monster" + std::to_string(stage) + ".txt");
    if (file.is_open()) {
        std::stringstream ss;
        ss << file.rdbuf();
        m.ascii = ss.str();
    }
    return m;
}
