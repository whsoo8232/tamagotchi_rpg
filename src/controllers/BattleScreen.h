#ifndef BATTLE_SCREEN_H
#define BATTLE_SCREEN_H

#include "Screen.h"
#include "../models/Monster.h"
#include "../models/Item.h"
#include <string>

enum class BattleState {
    STAGE_SELECT,   // 스테이지 선택
    ITEM_SELECT,    // 아이템 선택 (전투 중 서브화면)
    COMBAT_MAIN,    // 가위바위보 선택
    COMBAT_RESULT,  // 턴 결과 표시
    BATTLE_WIN,     // 전투 승리
    BATTLE_LOSE,    // 전투 패배
};

class BattleScreen : public Screen {
public:
    BattleScreen();
    void render(const Character& player) override;
    void handleInput(GameEngine& engine, Character& player, const InputEvent& event) override;

private:
    BattleState state;
    Monster     enemy;
    int         selectedStage;
    int         selectedItemIdx; // -1=없음, 0-3=ItemType
    std::string battleLog;
    std::string turnResult;
    int         turnCount;

    // ── 렌더 서브함수 ──────────────────────────────────────────
    void renderStageSelect(const Character& player);
    void renderItemSelect(const Character& player);
    void renderCombatMain(const Character& player);
    void renderCombatResult(const Character& player);
    void renderBattleWin(const Character& player);
    void renderBattleLose(const Character& player);

    // ── 입력 핸들러 서브함수 ───────────────────────────────────
    void handleStageSelect(GameEngine& engine, Character& player, const InputEvent& event);
    void handleItemSelect(GameEngine& engine, Character& player, const InputEvent& event);
    void handleCombatMain(GameEngine& engine, Character& player, const InputEvent& event);
    void handleCombatResult(GameEngine& engine, Character& player, const InputEvent& event);
    void handleBattleEnd(GameEngine& engine, Character& player, const InputEvent& event);

    // ── 전투 로직 ──────────────────────────────────────────────
    // playerChoice: 1=가위 2=바위 3=보
    void resolveCombat(Character& player, int playerChoice);

    static std::string rpsName(int choice); // 1→"가위" 등
};

#endif
