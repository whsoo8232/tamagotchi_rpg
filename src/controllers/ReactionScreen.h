#ifndef REACTION_SCREEN_H
#define REACTION_SCREEN_H

#include "Screen.h"
#include <chrono>
#include <string>

enum class RCState {
    INTRO,    // 안내 화면 (잠시 대기 후 WAITING)
    WAITING,  // "준비" — 랜덤 1-3초 대기
    SIGNAL,   // "지금!" — 반응 대기
    JUDGED,   // 결과 표시 (1초 후 종료)
    RESULT,   // 최종 결과
};

class ReactionScreen : public Screen {
public:
    ReactionScreen();
    void render(const Character& player) override;
    void handleInput(GameEngine& engine, Character& player, const InputEvent& event) override;
    bool needsRedraw() const override { return state != RCState::RESULT; }

private:
    RCState   state;
    std::chrono::steady_clock::time_point stateEntered;
    std::chrono::steady_clock::time_point signalStart;

    long long waitMs;       // WAITING 지속 시간 (랜덤 1000-3000ms)
    long long reactionMs;   // -1: 미입력, >=0: 반응 시간
    int       agiGain;      // 획득 민첩
    std::string judgeMsg;   // 판정 메시지

    long long elapsed() const;
    long long signalElapsed() const;
    int calcAgiGain(long long ms) const;
};

#endif
