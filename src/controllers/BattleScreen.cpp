#include "BattleScreen.h"
#include "GameEngine.h"
#include "MainScreen.h"
#include "EndingScreen.h"
#include "../views/Renderer.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>

BattleScreen::BattleScreen()
    : state(BattleState::STAGE_SELECT),
      enemy(Monster::generateStageMonster(1)),
      selectedStage(1), selectedItemIdx(-1),
      battleLog(""), turnResult(""), turnCount(0),
      playerLastChoice(0), enemyLastChoice(0),
      resultAnimating(false) {
}

// ══════════════════════════════════════════════════════════════════
//  render 진입점
// ══════════════════════════════════════════════════════════════════

void BattleScreen::render(const Character& player) {
    switch (state) {
        case BattleState::STAGE_SELECT:   renderStageSelect(player);  break;
        case BattleState::ITEM_SELECT:    renderItemSelect(player);   break;
        case BattleState::COMBAT_MAIN:    renderCombatMain(player);   break;
        case BattleState::COMBAT_RESULT:  renderCombatMain(player);   break; // 미사용, fallback
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

    // 1:2:1 비율: 40 + │ + 79 + │ + 40 = 161
    static const int ROWS     = 34;
    static const int LEFT_W   = 40;
    static const int CENTER_W = 79;
    static const int RIGHT_W  = 40;

    // ── 좌측: 스테이지 목록 (수직 중앙, 텍스트 가로 중앙) ────────
    // 헤더(1) + 빈줄(1) + 항목5개×(항목+빈줄) - 마지막빈줄(1) = 11줄
    std::vector<std::string> leftContent = { "[ 스테이지 선택 ]", "" };
    int cleared = player.getClearedStage();
    for (int i = 1; i <= 5; ++i) {
        bool locked = (i > cleared + 1);
        std::string label = std::string(i == selectedStage ? "> " : "  ")
                          + "스테이지 " + std::to_string(i);
        if (locked) label += "  [잠금]";
        leftContent.push_back(label);
        if (i < 5) leftContent.push_back(""); // 항목 사이 빈줄
    }
    // leftContent.size() = 2 + 5 + 4 = 11
    std::vector<std::string> leftCol(ROWS, "");
    int lTop = (ROWS - (int)leftContent.size()) / 2; // = (34-11)/2 = 11
    for (int i = 0; i < (int)leftContent.size(); ++i)
        leftCol[lTop + i] = leftContent[i];

    // ── 중앙(79): 몬스터 스몰 아트 (60col 가로 중앙, 수직 중앙) ──────
    const auto& rawArt = Renderer::getMonsterSmallArt(selectedStage);
    std::vector<std::string> centerCol(ROWS, "");
    int aRows = (int)rawArt.size();
    int aTop  = (ROWS - aRows) / 2;
    // 아트의 최대 시각 너비를 구해 CENTER_W 안에서 가로 중앙 정렬
    int maxArtW = 0;
    for (const auto& line : rawArt)
        maxArtW = std::max(maxArtW, Renderer::getDisplayWidth(line));
    int artIndent = std::max(0, (CENTER_W - maxArtW) / 2);
    for (int i = 0; i < aRows; ++i)
        centerCol[aTop + i] = std::string(artIndent, ' ') + rawArt[i];

    // ── 우측: 몬스터 정보 (수직 중앙, 텍스트 가로 중앙) ─────────
    std::vector<std::string> rightContent = {
        "[ 몬스터 정보 ]", "",
        "이름  : " + enemy.getName(),
        "HP    : " + std::to_string(enemy.getHp()),
        "공격력: " + std::to_string(enemy.getAttack()),
        "회피율: " + std::to_string(enemy.getEvasion()) + "%"
    };
    std::vector<std::string> rightCol(ROWS, "");
    int rTop = (ROWS - (int)rightContent.size()) / 2; // = (34-6)/2 = 14
    for (int i = 0; i < (int)rightContent.size(); ++i)
        rightCol[rTop + i] = rightContent[i];

    // ── 출력 ─────────────────────────────────────────────────────
    for (int i = 0; i < ROWS; ++i) {
        std::cout << Renderer::center(leftCol[i],    LEFT_W)   << "│";
        std::cout << Renderer::padRight(centerCol[i], CENTER_W) << "│";
        std::cout << Renderer::center(rightCol[i],   RIGHT_W)  << "\n";
    }

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

    static const int COL_W = 40; // 4 × 40 = 160

    // 헤더 (5행)
    std::cout << "\n\n"
              << Renderer::center("[ 아이템 선택 ]", 161) << "\n"
              << Renderer::center("이번 턴에 사용할 아이템을 고르세요 (턴당 1개)", 161) << "\n\n";

    // 선택 마커 행 (선택된 컬럼에 ★)
    for (int i = 0; i < ITEM_COUNT; ++i) {
        bool sel = (selectedItemIdx == i);
        if (sel) std::cout << "\033[33m";
        std::cout << Renderer::center(sel ? "★" : " ", COL_W);
        if (sel) std::cout << Renderer::RESET;
    }
    std::cout << "\n";

    // 아트 로드 + 최대 높이·너비 계산
    int maxArtRows = 0;
    std::vector<const std::vector<std::string>*> arts;
    std::vector<int> artWidths(ITEM_COUNT, 0);
    for (int i = 0; i < ITEM_COUNT; ++i) {
        arts.push_back(&Renderer::getItemArt(i));
        maxArtRows = std::max(maxArtRows, (int)arts.back()->size());
        for (const auto& line : *arts.back())
            artWidths[i] = std::max(artWidths[i], Renderer::getDisplayWidth(line));
    }

    // 4열 아트 출력
    for (int row = 0; row < maxArtRows; ++row) {
        for (int i = 0; i < ITEM_COUNT; ++i) {
            const auto& art = *arts[i];
            int topPad = (maxArtRows - (int)art.size()) / 2;
            int r = row - topPad;
            std::string cell;
            if (r >= 0 && r < (int)art.size()) {
                int indent = std::max(0, (COL_W - artWidths[i]) / 2);
                cell = std::string(indent, ' ') + art[r];
            }
            bool sel = (selectedItemIdx == i);
            if (sel) std::cout << "\033[33m";
            std::cout << Renderer::padRight(cell, COL_W);
            if (sel) std::cout << Renderer::RESET;
        }
        std::cout << "\n";
    }

    std::cout << "\n";

    // 이름 + 보유 행
    for (int i = 0; i < ITEM_COUNT; ++i) {
        int cnt = player.getItemCount((ItemType)i);
        bool sel = (selectedItemIdx == i);
        std::string cell = ITEM_TABLE[i].name + " (" + std::to_string(cnt) + "/" + std::to_string(MAX_ITEM_STACK) + ")";
        if (cnt == 0) std::cout << "\033[90m";
        else if (sel) std::cout << "\033[33m";
        std::cout << Renderer::center(cell, COL_W);
        if (cnt == 0 || sel) std::cout << Renderer::RESET;
    }
    std::cout << "\n";

    // 효과 행
    for (int i = 0; i < ITEM_COUNT; ++i) {
        int cnt = player.getItemCount((ItemType)i);
        bool sel = (selectedItemIdx == i);
        if (cnt == 0) std::cout << "\033[90m";
        else if (sel) std::cout << "\033[33m";
        std::cout << Renderer::center(ITEM_TABLE[i].effect, COL_W);
        if (cnt == 0 || sel) std::cout << Renderer::RESET;
    }
    std::cout << "\n\n";

    std::cout << Renderer::center("[0] 아이템 사용 안함", 161) << "\n";

    // 2+1+1+1(헤더5) + 1(마커) + maxArtRows + 1(blank) + 2(이름/효과) + 2(blank+사용안함) = maxArtRows+12
    Renderer::fillContent(12 + maxArtRows);

    Renderer::drawBottomMenu({"힘의 포션", "민첩의 포션", "밧줄", "방패", "사용 안함"});
}

void BattleScreen::renderCombatMain(const Character& player) {
    // ── 결과 표시 타이머 체크 (2초 후 캐릭터 아트로 자동 복귀) ─────
    if (resultAnimating) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - resultStart).count();
        if (elapsed >= 2000) {
            resultAnimating  = false;
            playerLastChoice = 0;
            enemyLastChoice  = 0;
        }
    }

    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    // ── 레이아웃 ──────────────────────────────────────────────────
    // 총 161col: 플레이어(70) │ 정보(26) │ 몬스터(63) = 161
    // HP행(1) + 아트행(34) + 턴행(1) = 36 = CONTENT_H
    static const int MAX_ROWS = 34;
    const int LEFT_W   = 70;
    const int CENTER_W = 26;
    const int RIGHT_W  = 63; // 161 - 70 - 1 - 26 - 1

    // ── HP 바 헬퍼 ─────────────────────────────────────────────────
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

    // ── HP 행 (좌70 │ 중26 │ 우63) ────────────────────────────────
    std::string pHpTxt = " " + std::to_string(player.getHp())
                       + "/" + std::to_string(player.getMaxHp());
    std::cout << "  " << Renderer::padRight(player.getName(), 12) << " HP: ";
    printHpBar(player.getHp(), player.getMaxHp());
    std::cout << pHpTxt;
    int leftUsed = 2 + 12 + 5 + 22 + (int)pHpTxt.size();
    std::cout << std::string(std::max(0, LEFT_W - leftUsed), ' ') << "│";

    std::string itemHdr = (selectedItemIdx >= 0)
        ? " [" + ITEM_TABLE[selectedItemIdx].name + "]"
        : " 아이템 없음";
    std::cout << Renderer::padRight(itemHdr, CENTER_W) << "│";

    std::string mHpTxt = " " + std::to_string(enemy.getHp())
                       + "/" + std::to_string(enemy.getMaxHp());
    std::cout << "  " << Renderer::padRight(enemy.getName(), 14) << " HP: ";
    printHpBar(enemy.getHp(), enemy.getMaxHp());
    std::cout << mHpTxt << "\n";

    // ── 중앙 정보 컬럼 빌드 (26col × 34행) ──────────────────────
    std::vector<std::string> centerCol(MAX_ROWS, "");

    if (playerLastChoice == 0) {
        // 대기 상태: 다음 턴 번호 + 선택된 아이템 아트
        centerCol[2]  = Renderer::center("[ 턴 " + std::to_string(turnCount + 1) + " ]", CENTER_W);
        centerCol[4]  = Renderer::center("가위바위보를", CENTER_W);
        centerCol[5]  = Renderer::center("선택하세요", CENTER_W);

        if (selectedItemIdx >= 0) {
            const auto& art = Renderer::getItemArt(selectedItemIdx);
            int artW = 0;
            for (const auto& line : art)
                artW = std::max(artW, Renderer::getDisplayWidth(line));
            int artIndent = std::max(0, (CENTER_W - artW) / 2);
            int artTop = 8; // art 시작 행 (텍스트 아래)
            for (int r = 0; r < (int)art.size() && (artTop + r) < MAX_ROWS; ++r)
                centerCol[artTop + r] = std::string(artIndent, ' ') + art[r];
            int labelRow = artTop + (int)art.size() + 1;
            if (labelRow < MAX_ROWS)
                centerCol[labelRow] = Renderer::center(ITEM_TABLE[selectedItemIdx].name, CENTER_W);
        }
    } else {
        // 결과 상태: 선택 + 결과 표시
        centerCol[5]  = Renderer::center("[ 턴 " + std::to_string(turnCount) + " 결과 ]", CENTER_W);
        centerCol[7]  = Renderer::center("나  : " + rpsName(playerLastChoice), CENTER_W);
        centerCol[8]  = Renderer::center("vs", CENTER_W);
        centerCol[9]  = Renderer::center("적  : " + rpsName(enemyLastChoice), CENTER_W);
        centerCol[10] = Renderer::center("----------", CENTER_W);

        // turnResult = "[X vs Y] 결과메시지..."
        // "[X vs Y] " 이후 텍스트만 추출
        std::string res = turnResult;
        auto pos = res.find("] ");
        if (pos != std::string::npos) res = res.substr(pos + 2);

        // 첫 번째 "!" 기준으로 줄 분리
        auto excl = res.find('!');
        std::string line1 = (excl != std::string::npos) ? res.substr(0, excl + 1) : res;
        std::string line2 = (excl != std::string::npos && excl + 2 < res.size())
                          ? res.substr(excl + 2) : "";

        // line2가 26col 초과 시 첫 번째 "." 이후 잘라내기
        if (Renderer::getDisplayWidth(line2) > CENTER_W) {
            auto dot = line2.find('.');
            if (dot != std::string::npos) line2 = line2.substr(0, dot + 1);
        }

        centerCol[12] = Renderer::center(line1, CENTER_W);
        if (!line2.empty())
            centerCol[13] = Renderer::center(line2, CENTER_W);
    }

    // ── 아트 선택 ─────────────────────────────────────────────────
    // 대기 중 → 캐릭터 아트(60col), 결과 표시 → RPS 손 아트(40col)
    const std::vector<std::string>* leftArtPtr;
    const std::vector<std::string>* rightArtPtr;
    int leftVW, rightVW;

    if (playerLastChoice == 0) {
        leftArtPtr  = &Renderer::getPlayerArt();
        leftVW      = 60;
        rightArtPtr = &Renderer::getMonsterSmallArt(selectedStage);
        rightVW     = 60;
    } else {
        leftArtPtr  = &Renderer::getRPSArt(playerLastChoice);
        leftVW      = 40;
        rightArtPtr = &Renderer::getRPSArt(enemyLastChoice);
        rightVW     = 40;
    }

    const auto& leftArt  = *leftArtPtr;
    const auto& rightArt = *rightArtPtr;
    int leftRows    = (int)leftArt.size();
    int rightRows   = (int)rightArt.size();
    int leftTopPad  = (MAX_ROWS - leftRows)  / 2;
    int rightTopPad = (MAX_ROWS - rightRows) / 2;
    int leftIndent  = (LEFT_W  - leftVW)  / 2;
    int rightIndent = (RIGHT_W - rightVW) / 2;

    // ── 3열 렌더링 ────────────────────────────────────────────────
    for (int row = 0; row < MAX_ROWS; ++row) {
        // 좌측 (LEFT_W col)
        int lr = row - leftTopPad;
        if (lr >= 0 && lr < leftRows) {
            std::cout << std::string(leftIndent, ' ')
                      << Renderer::padRight(leftArt[lr], leftVW)
                      << std::string(LEFT_W - leftIndent - leftVW, ' ');
        } else {
            std::cout << std::string(LEFT_W, ' ');
        }
        // 중앙 (CENTER_W col)
        std::cout << "│" << Renderer::padRight(centerCol[row], CENTER_W) << "│";
        // 우측 (RIGHT_W col)
        int mr = row - rightTopPad;
        if (mr >= 0 && mr < rightRows)
            std::cout << std::string(rightIndent, ' ') << rightArt[mr];
        std::cout << "\n";
    }

    // ── 턴 행 ─────────────────────────────────────────────────────
    std::cout << "  턴 " << turnCount << "\n";

    // 합계: 1(HP) + 34(아트) + 1(턴) = 36 = CONTENT_H
    Renderer::fillContent(36);
    Renderer::drawBottomMenu({"가위", "바위", "보", "아이템", "도망"});
}


void BattleScreen::renderBattleWin(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    // win.txt: 17행, 100col → indent=(161-100)/2=30
    // 레이아웃: 7공백 + 17아트 + 1공백 + 3정보 + 1공백 = 29 → fillContent(29)+7 = 36
    const auto& art    = Renderer::getWinArt();
    const int   indent = (161 - 100) / 2; // 30
    int reward = selectedStage * 200;

    for (int i = 0; i < 7; ++i) std::cout << "\n";
    for (const auto& line : art)
        std::cout << std::string(indent, ' ') << line << "\n";
    std::cout << "\n";
    std::cout << Renderer::center(enemy.getName() + " 처치 완료!", 161) << "\n";
    std::cout << Renderer::center("획득 금액: " + std::to_string(reward) + "원", 161) << "\n";
    std::cout << Renderer::center("클리어 스테이지: " + std::to_string(selectedStage), 161) << "\n";
    std::cout << "\n";
    Renderer::fillContent(29); // 7+17+1+3+1 = 29

    Renderer::drawBottomMenu({"계속"});
}

void BattleScreen::renderBattleLose(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player);

    // lose.txt: 16행, 100col → indent=(161-100)/2=30
    // 레이아웃: 8공백 + 16아트 + 2공백 + 1정보 + 1공백 = 28 → fillContent(28)+8 = 36
    const auto& art    = Renderer::getLoseArt();
    const int   indent = (161 - 100) / 2; // 30

    for (int i = 0; i < 8; ++i) std::cout << "\n";
    for (const auto& line : art)
        std::cout << std::string(indent, ' ') << line << "\n";
    std::cout << "\n\n";
    std::cout << Renderer::center("훈련을 더 해보세요!", 161) << "\n";
    std::cout << "\n";
    Renderer::fillContent(28); // 8+16+2+1+1 = 28

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
        case BattleState::COMBAT_RESULT: handleCombatMain(engine, player, event);  break; // 미사용, fallback
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
        if (event.y >= 41) {
            int btnW = 158 / 2;
            int btn  = (event.x - 1) / btnW;
            if (btn == 0) choice = 1;
            else          choice = 0;
        } else if (event.x >= 1 && event.x <= 40) { // LEFT_W
            // 스테이지 i at event.y = 18 + (i-1)*2
            int stage = (event.y - 18) / 2 + 1;
            if (stage >= 1 && stage <= 5 && event.y >= 18 && event.y <= 26) {
                // 잠금 여부 관계없이 선택 허용 → 아트/정보 미리보기 가능
                selectedStage = stage;
                enemy = Monster::generateStageMonster(selectedStage);
                battleLog = "";
            }
        }
    }

    if (choice == 1) {
        if (selectedStage > player.getClearedStage() + 1) {
            // 잠금된 스테이지 → 안내 문구 표시
            battleLog = "스테이지 " + std::to_string(selectedStage - 1)
                      + "을(를) 먼저 클리어해야 합니다!";
        } else {
            enemy = Monster::generateStageMonster(selectedStage);
            selectedItemIdx  = -1;
            turnCount        = 0;
            battleLog        = "";
            playerLastChoice = 0;
            enemyLastChoice  = 0;
            state = BattleState::COMBAT_MAIN;
        }
    } else if (choice == 0) {
        engine.changeScreen(std::make_unique<MainScreen>());
    }
}

void BattleScreen::handleItemSelect(GameEngine& engine, Character& player, const InputEvent& event) {
    int choice = -1;
    if (event.type == InputType::MOUSE_PRESS && event.button == 0) {
        if (event.y >= 41) {
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
        if (event.y >= 41) {
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
        turnResult       = "전투에서 도망쳤습니다.";
        selectedItemIdx  = -1;
        playerLastChoice = 0;
        enemyLastChoice  = 0;
        state     = BattleState::STAGE_SELECT;
        battleLog = turnResult;
    }
}

void BattleScreen::handleBattleEnd(GameEngine& engine, Character& player, const InputEvent& event) {
    if (event.type == InputType::MOUSE_PRESS) {
        player.restoreHp();
        player.afterBattle();
        if (state == BattleState::BATTLE_WIN && selectedStage >= 5)
            engine.changeScreen(std::make_unique<EndingScreen>(EndingType::HERO));
        else
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
    playerLastChoice = playerChoice; // 화면 표시용 저장
    enemyLastChoice  = enemyChoice;
    resultAnimating  = true;
    resultStart      = std::chrono::steady_clock::now();

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
        state = BattleState::COMBAT_MAIN;
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
            turnResult = rpsResult + "승리! 적에게 " + std::to_string(dmg) + " 데미지!";
            if (doubleAttack) turnResult += " (힘의 포션)";
        } else {
            turnResult = rpsResult + "승리! 하지만 적이 회피!";
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
        int reward = selectedStage * 200;
        player.addMoney(reward);
        player.clearStage(selectedStage);
        state = BattleState::BATTLE_WIN;
    } else if (!player.isAlive()) {
        state = BattleState::BATTLE_LOSE;
    } else {
        state = BattleState::COMBAT_MAIN;
    }
}
