#ifndef REACTION_SCREEN_H
#define REACTION_SCREEN_H

#include "Screen.h"
#include <chrono>
#include <vector>
#include <string>

enum class RCState { INTRO, WAITING, SIGNAL, JUDGED, RESULT };

class ReactionScreen : public Screen {
public:
    ReactionScreen();
    void render(const Character& player) override;
    void handleInput(GameEngine& engine, Character& player, const InputEvent& event) override;

private:
    RCState state;
    std::chrono::steady_clock::time_point stateEntered;
    std::chrono::steady_clock::time_point signalStart;

    int round;
    int totalRounds;
    bool isStar;          // true = ★, false = ✕
    int consecutiveX;
    bool inputReceived;
    std::string lastStatus;

    int signalAreaX;  // random position within display (col offset 10-80)
    int signalAreaY;  // relative row offset within signal box (0-4)

    std::vector<long long> reactionTimes; // valid ★ reaction times in ms
    int xAvoidSuccess;

    long long waitDuration; // ms until signal appears

    void enterState(RCState s);
    void generateSignal();
    int calcExp() const;
    long long averageReaction() const;
};

#endif
