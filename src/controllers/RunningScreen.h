#ifndef RUNNING_SCREEN_H
#define RUNNING_SCREEN_H

#include "Screen.h"
#include <chrono>
#include <string>

enum class RNState { IDLE, RUNNING, DONE };

class RunningScreen : public Screen {
public:
    RunningScreen();
    void render(const Character& player) override;
    void handleInput(GameEngine& engine, Character& player, const InputEvent& event) override;

private:
    RNState state;
    int     runCount;    // 0-30
    bool    stepAnim;    // 발걸음 토글(캐릭터 표정)
    std::chrono::steady_clock::time_point stepTime;

    // 트랙 내 캐릭터 컬럼 위치 계산
    int charPos() const;
};

#endif
