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
    static const int CONTENT_H = 36;
    static void fillContent(int usedRows); // CONTENT_H까지 빈 줄로 패딩

    static void clearCanvas();
    static void drawTopMenu(const Character& player);
    static void drawBottomMenu(const std::vector<std::string>& options);
    static void drawMainCharacter();
    static void drawEatingCharacter(); // 밥 먹는 장면
    static void drawBathCharacter();   // 목욕 장면
    static void drawPlayCharacter();   // 놀이 장면
    static void drawTreatCharacter();  // 치료 장면
    // 몬스터 아트 캐시 (스테이지 1-5)
    static const std::vector<std::string>& getMonsterArt(int stage);       // 기본 크기 (~80col)
    static const std::vector<std::string>& getMonsterSmallArt(int stage);  // 스몰 크기 (~60col)
    // 플레이어 / 가위바위보 아트
    static const std::vector<std::string>& getPlayerArt();        // kuchipatchi (60col)
    static const std::vector<std::string>& getRPSArt(int choice); // 1=가위 2=바위 3=보 (40col)
    // 전투 결과 아트
    static const std::vector<std::string>& getWinArt();           // win.txt  (100col)
    static const std::vector<std::string>& getLoseArt();          // lose.txt (100col)
    // 아이템 아트 (0=힘의포션 1=민첩포션 2=밧줄 3=방패, ~22col)
    static const std::vector<std::string>& getItemArt(int itemIdx);
    // 검/갑옷 아트 (tier 1-5)
    static const std::vector<std::string>& getSwordArt(int tier);
    static const std::vector<std::string>& getArmorArt(int tier);
    // 훈련 아트
    static const std::vector<std::string>& getPunchingBagArt();
    static const std::vector<std::string>& getFinishLineArt();
    static const std::vector<std::string>& getTargetArt();
    // 엔딩 아트 (0=hero 1=starve 2=runaway 3=trash 4=hospital)
    static const std::vector<std::string>& getEndingArt(int idx);
    static void drawMessage(const std::string& message);
    static void drawDebug(const InputEvent& event);

    static int         getDisplayWidth(const std::string& str);
    static std::string padRight(const std::string& str, int targetWidth);
    static std::string padLeft(const std::string& str, int targetWidth);
    static std::string center(const std::string& str, int width);
    static std::string cropCenter(const std::string& str, int width); // 중앙 기준 width col로 크롭

private:
    static std::string getProgressBar(int value, const std::string& color);
};

#endif
