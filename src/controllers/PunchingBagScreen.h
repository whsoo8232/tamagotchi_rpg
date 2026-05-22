#ifndef PUNCHING_BAG_SCREEN_H
#define PUNCHING_BAG_SCREEN_H

#include "Screen.h"
#include <chrono>
#include <string>

enum class PBState { IDLE, PUNCHING, DONE };

class PunchingBagScreen : public Screen {
public:
    PunchingBagScreen();
    void render(const Character& player) override;
    void handleInput(GameEngine& engine, Character& player, const InputEvent& event) override;

private:
    PBState   state;
    int       punchCount;  // 0-30
    bool      punchAnim;   // 펀치 직후 애니메이션 프레임
    std::chrono::steady_clock::time_point punchTime;

    // 샌드백 단계: 0=온전, 1=균열(10+), 2=파손(20+), 3=파괴(30)
    int bagStage() const;

    static const char* BAG_ART[4][8];   // 4단계 × 8행
    static const char* CHAR_IDLE[6];
    static const char* CHAR_PUNCH[6];
};

#endif
