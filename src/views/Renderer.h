#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include <vector>
#include "../models/Character.h"
#include "../controllers/InputHandler.h"

class Renderer {
public:
    static const std::string RED;
    static const std::string YELLOW;
    static const std::string GREEN;
    static const std::string LBLUE;
    static const std::string BLUE;
    static const std::string MAGENTA;
    static const std::string RESET;

    static std::string getCurrentDate();

    // 컨텐츠 영역 고정 높이 (헤더 4줄 + 컨텐츠 CONTENT_H줄 + 하단메뉴 5줄)
    // 라벨 행 = 4 + CONTENT_H + 3 = 33  →  event.y 임계값 31
    static const int CONTENT_H = 26;
    static void fillContent(int usedRows); // CONTENT_H까지 빈 줄로 패딩

    static void clearCanvas();
    static void drawTopMenu(const Character& player);
    static void drawBottomMenu(const std::vector<std::string>& options);
    static void drawMainCharacter();
    static void drawEatingCharacter(); // 밥 먹는 장면
    static void drawBathCharacter();   // 목욕 장면
    static void drawPlayCharacter();   // 놀이 장면
    static void drawTreatCharacter();  // 치료 장면
    // 전투 화면: 구치파치(좌) vs 몬스터(우) 대치 레이아웃 (18행)
    static void drawCombatLayout(const std::vector<std::string>& monsterLines);
    static void drawMessage(const std::string& message);
    static void drawDebug(const InputEvent& event);

    static int         getDisplayWidth(const std::string& str);
    static std::string padRight(const std::string& str, int targetWidth);
    static std::string padLeft(const std::string& str, int targetWidth);
    static std::string center(const std::string& str, int width);

private:
    static std::string getProgressBar(int value, const std::string& color);
};

#endif
