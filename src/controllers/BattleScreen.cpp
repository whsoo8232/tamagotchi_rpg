#include "BattleScreen.h"
#include "GameEngine.h"
#include "MainScreen.h"
#include "../views/Renderer.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

BattleScreen::BattleScreen()
    : state(BattleState::STAGE_SELECT),
      enemy(Monster::generateStageMonster(1)),
      selectedStage(1), selectedItemIdx(-1),
      battleLog(""), turnResult(""), turnCount(0) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
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

    // ── 좌측(26): 스테이지 목록 ────────────────────────────────
    std::vector<std::string> leftCol(22, "");
    leftCol[0] = " [ 스테이지 선택 ]";
    int cleared = player.getClearedStage();
    for (int i = 1; i <= 5; ++i) {
        bool locked  = (i > cleared + 1);
        std::string prefix = (i == selectedStage) ? " > " : "   ";
        std::string label  = prefix + "스테이지 " + std::to_string(i);
        if (locked) label += " [잠금]";
        leftCol[2 + (i - 1) * 2] = label;
    }

    // ── 중앙(64): 몬스터 ASCII 아트 ────────────────────────────
    std::vector<std::string> artLines;
    {
        std::stringstream ss(enemy.getAscii());
        std::string line;
        while (std::getline(ss, line)) artLines.push_back(line);
    }
    int artH   = (int)artLines.size();
    int artTop = std::max(0, (22 - artH) / 2);
    int maxArtW = 0;
    for (const auto& l : artLines) maxArtW = std::max(maxArtW, Renderer::getDisplayWidth(l));
    int artPad = std::max(0, (64 - maxArtW) / 2);
    std::vector<std::string> centerCol(22, "");
    for (int i = 0; i < artH && (artTop + i) < 22; ++i)
        centerCol[artTop + i] = std::string(artPad, ' ') + artLines[i];

    // ── 우측(28): 몬스터 정보 ──────────────────────────────────
    std::vector<std::string> rightCol(22, "");
    rightCol[0] = " [ 몬스터 정보 ]";
    rightCol[2] = " 이름   : " + enemy.getName();
    rightCol[3] = " HP     : " + std::to_string(enemy.getHp());
    rightCol[4] = " 공격력 : " + std::to_string(enemy.getAttack());
    rightCol[5] = " 회피율 : " + std::to_string(enemy.getEvasion()) + "%";

    for (int i = 0; i < 22; ++i) {
        std::cout << Renderer::padRight(leftCol[i], 26) << "│";
        std::cout << Renderer::padRight(centerCol[i], 64) << "│";
        std::cout << rightCol[i] << "\n";
    }

    if (!battleLog.empty()) Renderer::drawMessage(battleLog);
    else std::cout << "\n\n";

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

    for (int i = 0; i < 6; ++i) std::cout << "\n";

    Renderer::drawBottomMenu({"힘의 포션", "민첩의 포션", "밧줄", "방패", "사용 안함"});
}

void BattleScreen::renderCombatMain(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    std::cout << "\n";
    std::cout << "  ┌─────────────────────────────────────────────────────────────────────────────────────────────────┐\n";

    // HP 바
    auto hpBar = [](int cur, int max) -> std::string {
        int blocks = (max > 0) ? (cur * 20 / max) : 0;
        std::string s = "[";
        for (int i = 0; i < 20; ++i)
            s += (i < blocks) ? "█" : "░";
        s += "] " + std::to_string(cur) + "/" + std::to_string(max);
        return s;
    };

    std::cout << "  │  " << Renderer::padRight(player.getName(), 12)
              << " HP: " << hpBar(player.getHp(), player.getMaxHp())
              << Renderer::padRight("", 30) << "│\n";
    std::cout << "  │  " << Renderer::padRight(enemy.getName(), 12)
              << " HP: " << hpBar(enemy.getHp(), enemy.getHp() + 1)
              << Renderer::padRight("", 30) << "│\n";

    std::cout << "  ├─────────────────────────────────────────────────────────────────────────────────────────────────┤\n";
    std::cout << "  │  턴 " << turnCount;

    if (selectedItemIdx >= 0) {
        std::cout << "   ✦ 선택된 아이템: "
                  << Renderer::YELLOW << ITEM_TABLE[selectedItemIdx].name << Renderer::RESET;
    } else {
        std::cout << "   아이템: 없음";
    }
    std::cout << Renderer::padRight("", 50) << "│\n";

    std::cout << "  │  [4] 아이템 선택  [0] 도망치기"
              << Renderer::padRight("", 65) << "│\n";
    std::cout << "  └─────────────────────────────────────────────────────────────────────────────────────────────────┘\n";

    for (int i = 0; i < 14; ++i) std::cout << "\n";

    if (!battleLog.empty()) Renderer::drawMessage(battleLog);
    else std::cout << "\n";

    Renderer::drawBottomMenu({"가위", "바위", "보", "아이템", "도망"});
}

void BattleScreen::renderCombatResult(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    std::cout << "\n";
    for (int i = 0; i < 10; ++i) std::cout << "\n";
    std::cout << "  " << turnResult << "\n";
    for (int i = 0; i < 10; ++i) std::cout << "\n";

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
    for (int i = 0; i < 14; ++i) std::cout << "\n";

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
    for (int i = 0; i < 14; ++i) std::cout << "\n";

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
        if (event.y >= 30) {
            int btnW = 118 / 2;
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
        if (event.y >= 30) {
            int btnW = 118 / 5;
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
        if (event.y >= 30) {
            int btnW = 118 / 5;
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
