#include "BattleScreen.h"
#include "GameEngine.h"
#include "MainScreen.h"
#include "../views/Renderer.h"
#include <iostream>
#include <vector>
#include <sstream>

BattleScreen::BattleScreen() : selectedStage(1), inCombat(false), enemy(Monster::generateStageMonster(1)) {}

void BattleScreen::render(const Character& player) {
    if (inCombat) {
        renderCombat(player);
    } else {
        renderStageSelection(player);
    }
}

void BattleScreen::renderStageSelection(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player, "2026-05-04");

    std::cout << std::endl;

    // ── 좌측 컬럼 (26자): 스테이지 선택 목록
    std::vector<std::string> leftCol(22, "");
    leftCol[0] = " [ 스테이지 선택 ]";
    for (int i = 1; i <= 5; ++i) {
        std::string prefix = (i == selectedStage) ? " > " : "   ";
        leftCol[2 + (i - 1) * 2] = prefix + "스테이지 " + std::to_string(i);
    }
    // 스테이지 항목: leftCol[1,3,5,7,9] (짝수 행은 공백)

    // ── 중앙 컬럼 (64자): 몬스터 아스키 아트 수직 중앙 정렬
    std::vector<std::string> artLines;
    {
        std::stringstream ss(enemy.getAscii());
        std::string line;
        while (std::getline(ss, line)) artLines.push_back(line);
    }
    int artH   = (int)artLines.size();
    int artTop = (22 - artH) / 2;
    if (artTop < 0) artTop = 0;

    int maxArtW = 0;
    for (const auto& line : artLines) {
        int w = Renderer::getDisplayWidth(line);
        if (w > maxArtW) maxArtW = w;
    }
    int artPad = (64 - maxArtW) / 2;
    if (artPad < 0) artPad = 0;

    std::vector<std::string> centerCol(22, "");
    for (int i = 0; i < artH && (artTop + i) < 22; ++i) {
        centerCol[artTop + i] = std::string(artPad, ' ') + artLines[i];
    }

    // ── 우측 컬럼 (28자): 몬스터 스테이터스
    std::vector<std::string> rightCol(22, "");
    rightCol[0] = " [ 몬스터 정보 ]";
    rightCol[2] = " 이름   : " + enemy.getName();
    rightCol[3] = " HP     : " + std::to_string(enemy.getHp());
    rightCol[4] = " 공격력 : " + std::to_string(enemy.getAttack());
    rightCol[5] = " 민첩   : " + std::to_string(enemy.getEvasion());

    // 3컬럼 출력: 좌(26) │ 중(64) │ 우(28) = 총 120자
    for (int i = 0; i < 22; ++i) {
        std::cout << Renderer::padRight(leftCol[i], 26) << "│";
        std::cout << Renderer::padRight(centerCol[i], 64) << "│";
        std::cout << rightCol[i] << "\n";
    }

    if (!battleLog.empty()) {
        Renderer::drawMessage(battleLog);
    } else {
        std::cout << "\n\n";
    }

    Renderer::drawBottomMenu({"전투 시작", "돌아가기"});
}

void BattleScreen::renderCombat(const Character& player) {
    Renderer::clearCanvas();
    Renderer::drawTopMenu(player, "2026-05-04");
    
    std::cout << "\n           [전투 진행 중: 스테이지 " << selectedStage << "]" << std::endl;
    std::cout << "\n      " << player.getName() << " vs " << enemy.getName() << std::endl;
    std::cout << "      HP: " << player.getHp() << "      HP: " << enemy.getHp() << std::endl;
    
    for(int i=0; i<18; ++i) std::cout << std::endl;

    if (!battleLog.empty()) {
        Renderer::drawMessage(battleLog);
    } else {
        std::cout << "\n\n";
    }
    
    Renderer::drawBottomMenu({"공격(가위)", "공격(바위)", "공격(보)", "도망치기"});
}

void BattleScreen::handleInput(GameEngine& engine, Character& player, const InputEvent& event) {
    int choice = -1;

    if (inCombat) {
        if (event.type == InputType::KEYBOARD) {
            if (event.key >= '1' && event.key <= '3') choice = event.key - '0';
            else if (event.key == '0') choice = 0;
        } else if (event.type == InputType::MOUSE_PRESS && event.button == 0) {
            if (event.y >= 30) {
                if (event.x >= 1 && event.x <= 30) choice = 1;
                else if (event.x >= 31 && event.x <= 60) choice = 2;
                else if (event.x >= 61 && event.x <= 90) choice = 3;
                else if (event.x >= 91) choice = 0;
            }
        }

        if (choice >= 1 && choice <= 3) {
            int enemyChoice = rand() % 3 + 1;
            if (choice == enemyChoice) {
                battleLog = "무승부! 서로 데미지를 입히지 못했습니다.";
            } else if ((choice == 1 && enemyChoice == 3) || (choice == 2 && enemyChoice == 1) || (choice == 3 && enemyChoice == 2)) {
                enemy.takeDamage(player.getAttack());
                battleLog = "승리! " + enemy.getName() + "에게 " + std::to_string(player.getAttack()) + " 데미지!";
            } else {
                player.takeDamage(enemy.getAttack());
                battleLog = "패배! " + player.getName() + "가 " + std::to_string(enemy.getAttack()) + " 데미지!";
            }
            if (!enemy.isAlive()) {
                battleLog += " 처치 완료! (클릭 시 스테이지 화면으로)";
                inCombat = false;
            }
        } else if (choice == 0) {
            inCombat = false;
            battleLog = "전투에서 도망쳤습니다.";
        }
    } else {
        if (event.type == InputType::KEYBOARD) {
            if (event.key >= '1' && event.key <= '5') {
                selectedStage = event.key - '0';
                enemy = Monster::generateStageMonster(selectedStage);
            } else if (event.key == 's') choice = 1;
            else if (event.key == '0') choice = 0;
        } else if (event.type == InputType::MOUSE_PRESS && event.button == 0) {
            if (event.y >= 30) {
                if (event.x >= 1 && event.x <= 60) choice = 1; // 전투 시작
                else if (event.x >= 61) choice = 0; // 돌아가기
            } else if (event.x >= 1 && event.x <= 26) {
                // 스테이지 항목: leftCol[1,3,5,7,9] → y=8,10,12,14,16 (공백 행 포함)
                int row = event.y - 7; // topMenu(3줄) + newline(1줄) 이후 오프셋
                int stage = (row % 2 == 1) ? (row / 2 + 1) : (row / 2);
                if (stage >= 1 && stage <= 5) {
                    selectedStage = stage;
                }
                enemy = Monster::generateStageMonster(selectedStage);
            }
        }

        if (choice == 1) {
            inCombat = true;
            battleLog = enemy.getName() + "과의 전투를 시작합니다!";
        } else if (choice == 0) {
            engine.changeScreen(std::make_unique<MainScreen>());
        }
    }
}
