#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include <vector>

class Character {
public:
    Character(std::string name);

    // Getters
    std::string getName() const { return name; }
    int getHunger() const { return hunger; }
    int getHappiness() const { return happiness; }
    int getCleanliness() const { return cleanliness; }
    int getHealth() const { return health; }
    int getMoney() const { return money; }
    int getAttack() const { return attack; }
    int getHp() const { return hp; }
    int getAgility() const { return agility; }
    int getExp() const { return exp; }
    int getLevel() const { return level; }

    // Setters & Actions
    void setMoney(int amount) { money = amount; }
    void addMoney(int amount) { money += amount; }
    
    void feed();
    void bath();
    void play();
    void treat();
    
    void trainAttack(int amount);
    void trainHp(int amount);
    void trainAgility(int amount);

    void takeDamage(int damage);
    bool isAlive() const;
    bool isEndingCondition() const;

private:
    std::string name;
    int hunger;      // 0-100
    int happiness;   // 0-100
    int cleanliness; // 0-100
    int health;      // 0-100
    
    int money;
    int attack;
    int hp;
    int maxHp;
    int agility;
    
    int level;
    int exp;

    void clampStats();
};

#endif
