#include "BattleScreen.h"
#include "GameEngine.h"
#include "MainScreen.h"
#include "../views/Renderer.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <algorithm>

BattleScreen::BattleScreen()
    : state(BattleState::STAGE_SELECT),
      enemy(Monster::generateStageMonster(1)),
      selectedStage(1), selectedItemIdx(-1),
      battleLog(""), turnResult(""), turnCount(0) {
}

// ══════════════════════════════════════════════════════════════════
//  render 진입점
// ══════════════════════════════════════════════════════════════════

void BattleScreen::render(const Character& player) {
    switch (state) {
        case BattleState::STAGE_SELECT:   renderStageSelect(player);  break;
        case BattleState::ITEM_SELECT:    renderItemSelect(player);   break;
        case BattleState::COMBAT_MAIN:    renderCombatMain(player);   break;
        case BattleState::COMBAT_RESULT:  renderCombatResult(player); break;
        case BattleState::BATTLE_WIN:     renderBattleWin(player);    break;
        case BattleState::BATTLE_LOSE:    renderBattleLose(player);   break;
    }
}

// ══════════════════════════════════════════════════════════════════
//  렌더 서브함수
// ══════════════════════════════════════════════════════════════════

void BattleScreen::renderStageSelect(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    // 아트 표시 행 수: CONTENT_H(26) - 2행(로그여유) = 24
    // art1=25행→1크롭, art2=20행✓, art3/5=24행✓, art4=22행✓
    static const int ROWS = 24;

    // ── 좌측(26): 스테이지 목록 ────────────────────────────────
    std::vector<std::string> leftCol(ROWS, "");
    leftCol[0] = " [ 스테이지 선택 ]";
    int cleared = player.getClearedStage();
    for (int i = 1; i <= 5; ++i) {
        bool locked  = (i > cleared + 1);
        std::string prefix = (i == selectedStage) ? " > " : "   ";
        std::string label  = prefix + "스테이지 " + std::to_string(i);
        if (locked) label += " [잠금]";
        leftCol[2 + (i - 1) * 2] = label;
    }

    // ── 중앙(64): 몬스터 아트 (빈 줄 트리밍 + 중앙 크롭) ────────
    std::vector<std::string> artLines;
    {
        std::stringstream ss(enemy.getAscii());
        std::string line;
        while (std::getline(ss, line)) artLines.push_back(line);
    }

    // 빈 줄 판별: ⠀(U+2800), 공백, 　(U+3000) 만 있으면 빈 줄
    auto isBlank = [](const std::string& line) -> bool {
        for (size_t i = 0; i < line.size(); ) {
            unsigned char c = (unsigned char)line[i];
            if (c == 0x20) { ++i; continue; }
            if (c == 0xE2 && i+2 < line.size() &&
                (unsigned char)line[i+1] == 0xA0 &&
                (unsigned char)line[i+2] == 0x80) { i += 3; continue; } // ⠀
            if (c == 0xE3 && i+2 < line.size() &&
                (unsigned char)line[i+1] == 0x80 &&
                (unsigned char)line[i+2] == 0x80) { i += 3; continue; } //
            return false;
        }
        return true;
    };

    // 레이블(line0) 스킵 + 앞뒤 빈 줄 트리밍
    int mFirst = artLines.empty() ? 0 : 1;
    while (mFirst < (int)artLines.size() && isBlank(artLines[mFirst])) ++mFirst;
    int mLast = (int)artLines.size() - 1;
    while (mLast > mFirst && isBlank(artLines[mLast])) --mLast;
    int mCount = (mFirst <= mLast) ? (mLast - mFirst + 1) : 0;

    // ROWS 초과 시 중앙 크롭
    if (mCount > ROWS) {
        mFirst += (mCount - ROWS) / 2;
        mCount  = ROWS;
    }

    // 너비 계산 (트리밍된 범위에서)
    int maxArtW = 0;
    for (int i = mFirst; i < mFirst + mCount; ++i)
        maxArtW = std::max(maxArtW, Renderer::getDisplayWidth(artLines[i]));
    int artPad = std::max(0, (64 - maxArtW) / 2);

    // 수직 중앙 정렬
    int artTop = (ROWS - mCount) / 2;
    std::vector<std::string> centerCol(ROWS, "");
    for (int i = 0; i < mCount; ++i)
        centerCol[artTop + i] = std::string(artPad, ' ') + artLines[mFirst + i];

    // ── 우측(28): 몬스터 정보 ──────────────────────────────────
    std::vector<std::string> rightCol(ROWS, "");
    rightCol[0] = " [ 몬스터 정보 ]";
    rightCol[2] = " 이름   : " + enemy.getName();
    rightCol[3] = " HP     : " + std::to_string(enemy.getHp());
    rightCol[4] = " 공격력 : " + std::to_string(enemy.getAttack());
    rightCol[5] = " 회피율 : " + std::to_string(enemy.getEvasion()) + "%";

    for (int i = 0; i < ROWS; ++i) {
        std::cout << Renderer::padRight(leftCol[i], 26) << "│";
        std::cout << Renderer::padRight(centerCol[i], 64) << "│";
        std::cout << rightCol[i] << "\n";
    }

    // ROWS=24: 배틀로그 있으면 24+2=26=CONTENT_H, 없으면 24→26(2행 패딩)
    if (!battleLog.empty()) {
        Renderer::drawMessage(battleLog);
        Renderer::fillContent(ROWS + 2);
    } else {
        Renderer::fillContent(ROWS);
    }

    Renderer::drawBottomMenu({"전투 시작", "돌아가기"});
}

void BattleScreen::renderItemSelect(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    std::cout << "\n";
    std::cout << "  ┌─────────────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "  │                          [ 아이템 선택 ]                                    │\n";
    std::cout << "  │              이번 턴에 사용할 아이템을 고르세요 (턴당 1개)                  │\n";
    std::cout << "  ├─────────────────────────────────────────────────────────────────────────────┤\n";

    for (int i = 0; i < ITEM_COUNT; ++i) {
        const auto& info = ITEM_TABLE[i];
        int cnt  = player.getItemCount((ItemType)i);
        bool sel = (selectedItemIdx == i);
        std::string mark = sel ? " ★ " : "   ";
        std::string line = mark + "[" + std::to_string(i + 1) + "] "
                         + Renderer::padRight(info.name, 14)
                         + " (" + std::to_string(cnt) + "/" + std::to_string(MAX_ITEM_STACK) + ")  "
                         + info.effect;
        std::string row = "  │ " + Renderer::padRight(line, 75) + "│\n";
        if (cnt == 0)
            std::cout << "\033[90m" << row << Renderer::RESET;
        else if (sel)
            std::cout << "\033[33m" << row << Renderer::RESET;
        else
            std::cout << row;

        std::cout << "  │                                                                             │\n";
    }
    std::cout << "  │                                                                             │\n";
    std::cout << "  │   [0] 아이템 사용 안함                                                     │\n";
    std::cout << "  └─────────────────────────────────────────────────────────────────────────────┘\n";

    Renderer::fillContent(16); // 1+4+8+3=16 → 26

    Renderer::drawBottomMenu({"힘의 포션", "민첩의 포션", "밧줄", "방패", "사용 안함"});
}

void BattleScreen::renderCombatMain(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    // ── 몬스터 아트 파싱 ───────────────────────────────────────────
    std::vector<std::string> monsterLines;
    {
        std::stringstream ss(enemy.getAscii());
        std::string line;
        while (std::getline(ss, line)) monsterLines.push_back(line);
    }

    // ── HP 바 출력 헬퍼 (컬러 포함, 시각 너비 22 고정) ─────────────
    // ※ █/░ 는 터미널에서 1col이지만 getDisplayWidth는 2col로 계산하므로
    //   여백 계산은 수동으로 처리
    auto printHpBar = [&](int cur, int max) {
        int blocks = (max > 0) ? (cur * 20 / max) : 0;
        int pct    = (max > 0) ? (cur * 100 / max) : 0;
        const std::string& col = (pct > 50) ? Renderer::GREEN
                                : (pct > 20) ? Renderer::YELLOW
                                :               Renderer::RED;
        std::cout << col << "[";
        for (int i = 0; i < 20; ++i)
            std::cout << (i < blocks ? "█" : "░");
        std::cout << "]" << Renderer::RESET;
    };

    // ── 렌더링 시작 ────────────────────────────────────────────────
    // 레이아웃: HP바(1) + 아트(24) + 턴정보(1) = 26 = CONTENT_H
    // 분리선을 없애 아트 영역을 최대화

    // 1: HP 바 행 (좌 70col │ 우)
    // 좌측 시각 너비: 2 + 12(name) + 5(" HP: ") + 22([bar]) + pHpTxt
    std::string pHpTxt = " " + std::to_string(player.getHp())
                       + "/" + std::to_string(player.getMaxHp());
    std::cout << "  " << Renderer::padRight(player.getName(), 12) << " HP: ";
    printHpBar(player.getHp(), player.getMaxHp());
    std::cout << pHpTxt;
    int leftUsed = 2 + 12 + 5 + 22 + (int)pHpTxt.size();
    std::cout << std::string(std::max(0, 70 - leftUsed), ' ') << "│";
    // 우측
    std::string mHpTxt = " " + std::to_string(enemy.getHp())
                       + "/" + std::to_string(enemy.getMaxHp());
    std::cout << "  " << Renderer::padRight(enemy.getName(), 14) << " HP: ";
    printHpBar(enemy.getHp(), enemy.getMaxHp());
    std::cout << mHpTxt << "\n"; // 1

    Renderer::drawCombatLayout(monsterLines); // 2-25: 24행 대치 아트

    // 26: 턴 정보
    std::cout << "  턴 " << turnCount;
    if (selectedItemIdx >= 0) {
        std::cout << "   " << Renderer::YELLOW
                  << "✦ 아이템: " << ITEM_TABLE[selectedItemIdx].name
                  << Renderer::RESET;
    } else {
        std::cout << "   아이템: 없음";
    }
    std::cout << "\n"; // 26

    // 합계: 1 + 24 + 1 = 26 = CONTENT_H (빈 줄 추가 불필요)
    Renderer::fillContent(26);

    Renderer::drawBottomMenu({"가위", "바위", "보", "아이템", "도망"});
}

void BattleScreen::renderCombatResult(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    std::cout << "\n";
    for (int i = 0; i < 10; ++i) std::cout << "\n";
    std::cout << "  " << turnResult << "\n";
    Renderer::fillContent(12); // 1+10+1=12 → 26

    Renderer::drawBottomMenu({"계속"});
}

void BattleScreen::renderBattleWin(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    int reward = selectedStage * 100;
    std::cout << "\n\n\n\n\n";
    std::cout << "  ╔══════════════════════════════════════╗\n";
    std::cout << "  ║           ★  전투 승리!  ★          ║\n";
    std::cout << "  ╠══════════════════════════════════════╣\n";
    std::cout << "  ║  " << Renderer::padRight(enemy.getName() + " 처치 완료!", 36) << "║\n";
    std::cout << "  ║  " << Renderer::padRight("획득 금액: " + std::to_string(reward) + "원", 36) << "║\n";
    std::cout << "  ║  " << Renderer::padRight("클리어 스테이지: " + std::to_string(selectedStage), 36) << "║\n";
    std::cout << "  ╚══════════════════════════════════════╝\n";
    Renderer::fillContent(12); // 5+7=12 → 26

    Renderer::drawBottomMenu({"계속"});
}

void BattleScreen::renderBattleLose(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    std::cout << "\n\n\n\n\n";
    std::cout << "  ╔══════════════════════════════════════╗\n";
    std::cout << "  ║           ✕  전투 패배...            ║\n";
    std::cout << "  ╠══════════════════════════════════════╣\n";
    std::cout << "  ║  " << Renderer::padRight(player.getName() + " 이(가) 쓰러졌습니다.", 36) << "║\n";
    std::cout << "  ║  " << Renderer::padRight("훈련을 더 해보세요!", 36) << "║\n";
    std::cout << "  ╚══════════════════════════════════════╝\n";
    Renderer::fillContent(11); // 5+6=11 → 26

    Renderer::drawBottomMenu({"돌아가기"});
}

// ══════════════════════════════════════════════════════════════════
//  handleInput 진입점
// ══════════════════════════════════════════════════════════════════

void BattleScreen::handleInput(GameEngine& engine, Character& player, const InputEvent& event) {
    switch (state) {
        case BattleState::STAGE_SELECT:  handleStageSelect(engine, player, event); break;
        case BattleState::ITEM_SELECT:   handleItemSelect(engine, player, event);  break;
        case BattleState::COMBAT_MAIN:   handleCombatMain(engine, player, event);  break;
        case BattleState::COMBAT_RESULT: handleCombatResult(engine, player, event);break;
        case BattleState::BATTLE_WIN:
        case BattleState::BATTLE_LOSE:   handleBattleEnd(engine, player, event);   break;
    }
}

// ══════════════════════════════════════════════════════════════════
//  입력 핸들러 서브함수
// ══════════════════════════════════════════════════════════════════

void BattleScreen::handleStageSelect(GameEngine& engine, Character& player, const InputEvent& event) {
    int choice = -1;
    if (event.type == InputType::MOUSE_PRESS && event.button == 0) {
        if (event.y >= 31) {
            int btnW = 158 / 2;
            int btn  = (event.x - 1) / btnW;
            if (btn == 0) choice = 1;
            else          choice = 0;
        } else if (event.x >= 1 && event.x <= 26) {
            int row   = event.y - 7;
            int stage = (row % 2 == 1) ? (row / 2 + 1) : (row / 2);
            if (stage >= 1 && stage <= 5) {
                if (stage <= player.getClearedStage() + 1) {
                    selectedStage = stage;
                    enemy = Monster::generateStageMonster(selectedStage);
                } else {
                    battleLog = "스테이지 " + std::to_string(stage - 1) + "을(를) 먼저 클리어하세요!";
                }
            }
        }
    }

    if (choice == 1) {
        enemy = Monster::generateStageMonster(selectedStage);
        selectedItemIdx = -1;
        turnCount = 0;
        battleLog = "";
        state = BattleState::COMBAT_MAIN;
    } else if (choice == 0) {
        engine.changeScreen(std::make_unique<MainScreen>());
    }
}

void BattleScreen::handleItemSelect(GameEngine& engine, Character& player, const InputEvent& event) {
    int choice = -1;
    if (event.type == InputType::MOUSE_PRESS && event.button == 0) {
        if (event.y >= 31) {
            int btnW = 158 / 5;
            int btn  = (event.x - 1) / btnW;
            if (btn >= 0 && btn < 4) choice = btn;
            else                     choice = ITEM_COUNT;
        }
    }

    if (choice >= 0 && choice < ITEM_COUNT) {
        if (player.hasItem((ItemType)choice)) {
            selectedItemIdx = choice;
        } else {
            battleLog = ITEM_TABLE[choice].name + " 이(가) 없습니다.";
        }
        state = BattleState::COMBAT_MAIN;
    } else if (choice == ITEM_COUNT) {
        selectedItemIdx = -1;
        state = BattleState::COMBAT_MAIN;
    }
}

void BattleScreen::handleCombatMain(GameEngine& engine, Character& player, const InputEvent& event) {
    int choice = -1;
    if (event.type == InputType::MOUSE_PRESS && event.button == 0) {
        if (event.y >= 31) {
            int btnW = 158 / 5;
            int btn  = (event.x - 1) / btnW + 1; // 1-indexed
            if (btn >= 1 && btn <= 3)       choice = btn;
            else if (btn == 4) { state = BattleState::ITEM_SELECT; return; }
            else if (btn >= 5)              choice = 0;
        }
    }

    if (choice >= 1 && choice <= 3) {
        ++turnCount;
        resolveCombat(player, choice);
    } else if (choice == 0) {
        turnResult = "전투에서 도망쳤습니다.";
        selectedItemIdx = -1;
        state = BattleState::STAGE_SELECT;
        battleLog = turnResult;
    }
}

void BattleScreen::handleCombatResult(GameEngine& engine, Character& player, const InputEvent& event) {
    if (event.type == InputType::MOUSE_PRESS) {
        state = BattleState::COMBAT_MAIN;
        battleLog = "";
    }
}

void BattleScreen::handleBattleEnd(GameEngine& engine, Character& player, const InputEvent& event) {
    if (event.type == InputType::MOUSE_PRESS) {
        player.restoreHp();
        engine.changeScreen(std::make_unique<MainScreen>());
    }
}

// ══════════════════════════════════════════════════════════════════
//  전투 로직
// ══════════════════════════════════════════════════════════════════

std::string BattleScreen::rpsName(int c) {
    if (c == 1) return "가위";
    if (c == 2) return "바위";
    return "보";
}

void BattleScreen::resolveCombat(Character& player, int playerChoice) {
    int enemyChoice = std::rand() % 3 + 1;

    std::string rpsResult = "[" + rpsName(playerChoice) + " vs " + rpsName(enemyChoice) + "] ";

    // 승부 판정
    bool draw      = (playerChoice == enemyChoice);
    bool playerWin = !draw &&
        ((playerChoice == 1 && enemyChoice == 3) ||
         (playerChoice == 2 && enemyChoice == 1) ||
         (playerChoice == 3 && enemyChoice == 2));

    if (draw) {
        turnResult = rpsResult + "무승부! 데미지 없음.";
        if (selectedItemIdx >= 0) {
            player.useItem((ItemType)selectedItemIdx);
            selectedItemIdx = -1;
        }
        state = BattleState::COMBAT_RESULT;
        return;
    }

    if (playerWin) {
        // ── 플레이어 공격 ────────────────────────────────────
        bool skipEvasion  = (selectedItemIdx == (int)ItemType::ROPE);
        bool doubleAttack = (selectedItemIdx == (int)ItemType::STRENGTH_POTION);

        int dmg = player.getTotalAttack();
        if (doubleAttack) dmg *= 2;

        bool evaded = false;
        if (!skipEvasion) {
            evaded = (std::rand() % 100 < enemy.getEvasion());
        }

        if (!evaded) {
            enemy.takeDamage(dmg);
            turnResult = rpsResult + "승리! " + enemy.getName()
                       + "에게 " + std::to_string(dmg) + " 데미지!";
            if (doubleAttack) turnResult += " (힘의 포션)";
        } else {
            turnResult = rpsResult + "승리! 하지만 " + enemy.getName() + " 회피!";
        }

    } else {
        // ── 몬스터 공격 ──────────────────────────────────────
        bool doubleAgi   = (selectedItemIdx == (int)ItemType::AGILITY_POTION);
        bool shieldBlock = (selectedItemIdx == (int)ItemType::SHIELD);

        int agi = player.getAgility();
        if (doubleAgi) agi = std::min(100, agi * 2);

        bool evaded = (std::rand() % 100 < agi);

        if (shieldBlock) {
            turnResult = rpsResult + "패배! 방패로 완전 방어!";
        } else if (evaded) {
            turnResult = rpsResult + "패배! 회피 성공! (민첩 " + std::to_string(agi) + "%)";
        } else {
            int rawDmg    = enemy.getAttack();
            int reduction = player.getArmorReduction();
            int actual    = std::max(0, rawDmg - reduction);
            player.takeDamage(rawDmg); // 갑옷 감소 내부 처리
            turnResult = rpsResult + "패배! " + std::to_string(actual) + " 데미지 받음.";
            if (reduction > 0) turnResult += " (" + std::to_string(reduction) + " 갑옷 방어)";
        }
    }

    // 아이템 소모
    if (selectedItemIdx >= 0) {
        player.useItem((ItemType)selectedItemIdx);
        selectedItemIdx = -1;
    }

    // 전투 종료 판정
    if (!enemy.isAlive()) {
        int reward = selectedStage * 100;
        player.addMoney(reward);
        player.clearStage(selectedStage);
        state = BattleState::BATTLE_WIN;
    } else if (!player.isAlive()) {
        state = BattleState::BATTLE_LOSE;
    } else {
        state = BattleState::COMBAT_RESULT;
    }
}
