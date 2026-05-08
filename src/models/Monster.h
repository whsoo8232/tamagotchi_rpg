#ifndef MONSTER_H
#define MONSTER_H

#include <string>
#include <vector>

class Monster {
public:
    Monster(std::string name, int hp, int attack);
    
    std::string getName() const { return name; }
    int getHp() const { return hp; }
    int getAttack() const { return attack; }
    int getEvasion() const { return evasion; }
    std::string getAscii() const { return ascii; }
    
    void takeDamage(int damage);
    bool isAlive() const { return hp > 0; }
    
    static Monster generateStageMonster(int stage);

private:
    std::string name;
    int hp;
    int attack;
    int evasion;
    std::string ascii;
};

#endif
