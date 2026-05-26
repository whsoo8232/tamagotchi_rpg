#include "Renderer.h"
#include <iostream>
#include <iomanip>
#include <ctime>

const std::string Renderer::RED     = "\033[31m";
const std::string Renderer::YELLOW  = "\033[33m";
const std::string Renderer::GREEN   = "\033[32m";
const std::string Renderer::LBLUE   = "\033[36m";
const std::string Renderer::BLUE    = "\033[34m";
const std::string Renderer::MAGENTA = "\033[35m";
const std::string Renderer::RESET   = "\033[0m";

// ── 날짜 ───────────────────────────────────────────────────────────

std::string Renderer::getCurrentDate() {
    std::time_t t = std::time(nullptr);
    struct tm* tm_info = std::localtime(&t);
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm_info);
    return std::string(buf);
}

// ── 기본 렌더 유틸 ─────────────────────────────────────────────────

void Renderer::clearCanvas() {
    std::cout << "\033[2J\033[H" << std::flush;
}

void Renderer::fillContent(int usedRows) {
    int n = CONTENT_H - usedRows;
    for (int i = 0; i < n; ++i)
        std::cout << "\n";
}

std::string Renderer::getProgressBar(int value, const std::string& color) {
    int bars = value / 20;
    std::string s = color;
    for (int i = 0; i < 5; ++i)
        s += (i < bars) ? "#" : " ";
    s += RESET;
    return s;
}

int Renderer::getDisplayWidth(const std::string& str) {
    int width = 0;
    for (size_t i = 0; i < str.length(); ) {
        unsigned char c = str[i];
        if (c < 0x80) { width += 1; i += 1; }
        else if (c < 0xE0) { width += 1; i += 2; }
        else if (c < 0xF0) {
            if (i + 2 < str.length()) {
                unsigned char c2 = str[i+1];
                // 브라유 점자 (U+2800-U+28FF): 1컬럼
                if (c == 0xE2 && (c2 >= 0xA0 && c2 <= 0xBF))
                    width += 1;
                else
                    width += 2; // CJK 등 와이드 문자
            } else { width += 2; }
            i += 3;
        }
        else { width += 2; i += 4; }
    }
    return width;
}

std::string Renderer::padRight(const std::string& str, int targetWidth) {
    int cur = getDisplayWidth(str);
    std::string s = str;
    int pad = targetWidth - cur;
    if (pad > 0) s += std::string(pad, ' ');
    return s;
}

std::string Renderer::padLeft(const std::string& str, int targetWidth) {
    int cur = getDisplayWidth(str);
    int pad = targetWidth - cur;
    std::string s;
    if (pad > 0) s = std::string(pad, ' ');
    s += str;
    return s;
}

std::string Renderer::center(const std::string& str, int width) {
    int cur = getDisplayWidth(str);
    int total = width - cur;
    if (total <= 0) return str;
    int left  = total / 2;
    int right = total - left;
    return std::string(left, ' ') + str + std::string(right, ' ');
}

// ── 상단 헤더 ──────────────────────────────────────────────────────
// 총 161컬럼
// 공통 프리픽스(80컬럼): [다마고치명] | [날짜] | [배고픔 행복 청결 건강] | [돈] | (space)
//
// 1줄 (타이틀행):  ... |  공격력        체력              민첩  (42col) | 무기                갑옷
// 2줄 (데이터행):  ... |  공격력:X  체력:X/X  민첩:X       (42col) | 무기: ...  갑옷: ...
//
// | 구분자는 두 행 모두 display col 123 (= 80 프리픽스 + 42 스텟칸 + 1)

void Renderer::drawTopMenu(const Character& player) {
    const std::string sep = "=================================================================================================================================================================";
    std::cout << sep << "\n";

    std::string namePart = padRight(player.getName(), 13);
    std::string datePart = getCurrentDate();
    std::string hungerB  = getProgressBar(player.getHunger(),      RED);
    std::string happyB   = getProgressBar(player.getHappiness(),   YELLOW);
    std::string cleanB   = getProgressBar(player.getCleanliness(), LBLUE);
    std::string healthB  = getProgressBar(player.getHealth(),      BLUE);

    std::string statLine  = "공격력: " + std::to_string(player.getTotalAttack())
                          + "  체력: " + std::to_string(player.getHp())
                          + "/" + std::to_string(player.getMaxHp())
                          + "  민첩: " + std::to_string(player.getAgility());
    std::string equipLine = "무기: " + player.getSwordName()
                          + "  갑옷: " + player.getArmorName();

    // 1줄: 타이틀행 — 스텟 칸 타이틀 + | + 장비 칸 타이틀
    std::cout << " 다마고치      |    날짜    |  배고픔   행복    청결    건강     |      돈     |   "
              << padRight("                스탯             ", 38) << "|"
              << "                 장비         \n";

    // 2줄: 데이터행 — 스텟 값(42col) + | + 장비 값
    std::cout << " " << namePart
              << " | " << datePart
              << " |  [" << hungerB << "]  [" << happyB << "] [" << cleanB << "] [" << healthB << "] "
              << " | " << padLeft(std::to_string(player.getMoney()), 7) << "원  "
              << " |   " << padRight(statLine, 38) << "|" << "  " << equipLine << "\n";

    std::cout << sep << "\n";
}

// ── 하단 메뉴 ──────────────────────────────────────────────────────

void Renderer::drawBottomMenu(const std::vector<std::string>& options) {
    const std::string sep = "=================================================================================================================================================================";
    std::cout << sep << "\n";

    int totalWidth  = 158;
    int buttonWidth = totalWidth / (int)options.size();

    // 상단 여백
    std::cout << "|";
    for (size_t i = 0; i < options.size(); ++i)
        std::cout << std::string(buttonWidth, ' ') << "|";
    std::cout << "\n";

    // 버튼 레이블
    std::cout << "|";
    for (const auto& opt : options) {
        int labelW  = getDisplayWidth(opt);
        int padL    = (buttonWidth - labelW) / 2;
        int padR    = buttonWidth - labelW - padL;
        std::cout << std::string(padL, ' ') << opt << std::string(padR, ' ') << "|";
    }
    std::cout << "\n";

    // 하단 여백
    std::cout << "|";
    for (size_t i = 0; i < options.size(); ++i)
        std::cout << std::string(buttonWidth, ' ') << "|";
    std::cout << "\n";

    std::cout << sep << "\n";
}

// ── 캐릭터 아트 데이터 (drawMainCharacter / drawEatingCharacter 공유) ─
static const char* KUCHI_ART[] = {
    "⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣠⣤⣴⠶⠶⠿⠛⠛⠛⠛⠛⠻⠶⢶⣤⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
    "⠀⠀⠀⠀⠀⠀⢀⣴⠟⠋⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠛⢷⣄⠀⠀⠀⠀⠀⠀⠀",
    "⠀⠀⠀⠀⠀⣰⡟⢁⣤⣤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⣶⣄⠙⣷⡀⠀⠀⠀⠀⠀",
    "⠀⠀⠀⠀⣸⡟⠀⢾⣿⣿⣿⠀⠀⠀⢀⣀⣀⣀⣀⣀⣀⡀⠀⠀⠸⣿⣿⣿⠇⠈⢿⡄⠀⠀⠀⠀",
    "⠀⠀⠀⠀⣿⠁⠀⠀⠙⠛⢉⣴⠿⠟⠛⠛⠛⠛⠛⠛⠛⠛⠻⣶⡄⠈⠉⠁⠀⠀⠸⣷⠀⠀⠀⠀",
    "⠀⠀⠀⠀⣿⠀⠀⠀⠀⠀⢸⣿⡀⢀⣀⣀⣀⣀⣀⣀⣀⣀⢀⣿⡇⠀⠀⠀⠀⠀⠀⣿⠀⠀⠀⠀",
    "⠀⠀⠀⠀⣿⠀⠀⠀⠀⠀⢠⣿⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⣿⡄⠀⠀⠀⠀⠀⢀⣿⠀⠀⠀⠀",
    "⠀⠀⠀⠀⢿⡄⠀⠀⠀⠀⢸⣿⣀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣽⡇⠀⠀⠀⠀⠀⢸⣿⠀⠀⠀⠀",
    "⠀⠀⠀⢀⣼⠷⠀⠀⠀⠀⠀⠙⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠉⠀⠀⠀⠀⠀⠀⠈⠻⣦⡀⠀⠀",
    "⠀⢀⣴⠟⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⣶⠈⢻⣆⠀",
    "⢠⡿⠃⢠⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢿⣇⠀⢹⣧",
    "⣿⠃⠀⢸⡿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⡄⣠⡿",
    "⠹⢧⣤⣾⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⡟⠋⠀",
    "⠀⠀⠀⠘⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠃⠀⠀",
    "⠀⠀⠀⠀⠹⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣼⠏⠀⠀⠀",
    "⠀⠀⠀⠀⠀⠙⠷⣦⣄⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣀⣤⣴⡾⠋⠁⠀⠀⠀⠀",
    "⠀⠀⠀⠀⠀⠀⠀⢿⡏⠛⠛⠻⠿⠿⢿⡿⠿⠛⠛⠛⠛⣿⡛⠛⠛⠛⠋⠉⣸⡇⠀⠀⠀⠀⠀⠀",
    "⠀⠀⠀⠀⠀⠀⠀⠈⠻⠶⣶⣤⣤⡶⠟⠁⠀⠀⠀⠀⠀⠘⠷⣦⣤⣤⣴⠾⠛⠀⠀⠀⠀⠀⠀⠀",
};
static const int  KUCHI_ROWS   = 18;
static const int  KUCHI_INDENT = 62;   // (161 - 36) / 2 = 62 → 중앙 정렬

// ── 메인 캐릭터 아트 (구치파치) ────────────────────────────────────

void Renderer::drawMainCharacter() {
    std::cout << "\n";
    for (int i = 0; i < KUCHI_ROWS; ++i)
        std::cout << std::string(KUCHI_INDENT, ' ') << KUCHI_ART[i] << "\n";
    std::cout << "\n";
}

// ── 육성 애니메이션: 이모지 10개 주변 배치 ─────────────────────────
// 구치파치(col 62~97, 행1~18) 상·하·좌·우 바로 주변에만 배치
// row 0·19: 위아래, col 57: 좌, col 100: 우 (각 2col 여백 확보)
// (row, col) 오름차순 정렬 필수

struct EmojiPos { int row, col; };

static const EmojiPos EMOJI_SCATTER[] = {
    { 0, 69},{ 0, 84},          // ↑ 위쪽 중앙 2개
    { 3, 57},{ 3,100},          // ← → 상단 부근
    { 8, 57},{ 8,100},          // ← → 중간
    {14, 57},{14,100},          // ← → 하단 부근
    {19, 69},{19, 84},          // ↓ 아래쪽 중앙 2개
};

// 이모지 10개를 구치파치 주변에 배치하여 출력
static void drawCharWithScatteredEmoji(const std::string& emoji) {
    // emoji는 터미널에서 2컬럼 너비 (4바이트 UTF-8 이모지 기준)
    for (int row = 0; row < 20; ++row) {
        int curCol   = 0;
        bool artDone = (row < 1 || row > 18); // 행0·19는 아트 없음

        for (const auto& p : EMOJI_SCATTER) {
            if (p.row != row) continue;

            // 이 이모지가 아트 우측이면 먼저 아트를 출력
            if (!artDone && p.col >= KUCHI_INDENT) {
                if (KUCHI_INDENT > curCol)
                    std::cout << std::string(KUCHI_INDENT - curCol, ' ');
                std::cout << KUCHI_ART[row - 1];
                curCol  = KUCHI_INDENT + 36;
                artDone = true;
            }

            if (p.col > curCol)
                std::cout << std::string(p.col - curCol, ' ');
            std::cout << emoji;
            curCol = p.col + 2;
        }

        // 이모지가 전부 좌측이었거나 해당 행에 이모지가 없는 아트 행
        if (!artDone) {
            if (KUCHI_INDENT > curCol)
                std::cout << std::string(KUCHI_INDENT - curCol, ' ');
            std::cout << KUCHI_ART[row - 1];
        }

        std::cout << "\n";
    }
}

// ── 밥 먹는 화면: 🍚 산란 ─────────────────────────────────────────
void Renderer::drawEatingCharacter() {
    drawCharWithScatteredEmoji("🍚");
}

// ── 목욕 화면: 🚿 산란 ───────────────────────────────────────────
void Renderer::drawBathCharacter() {
    drawCharWithScatteredEmoji("🚿");
}

// ── 놀이 화면: 🎉 산란 ───────────────────────────────────────────
void Renderer::drawPlayCharacter() {
    drawCharWithScatteredEmoji("🎉");
}

// ── 치료 화면: 💉 산란 ───────────────────────────────────────────
void Renderer::drawTreatCharacter() {
    drawCharWithScatteredEmoji("💉");
}

// ── 전투 대치 레이아웃 ─────────────────────────────────────────────
//
//  터미널 161컬럼 기준
//  좌측 70col: 구치파치 중앙 정렬 (indent=17, art=36, rpad=17)
//  │ (1col)
//  우측 90col: 몬스터 아트 (3col 좌패딩 + 아트)
//
//  [자동 트리밍]
//  몬스터 아트 line[0] = "공격 모션" 레이블 → 스킵
//  앞/뒤 빈 행(⠀, 공백, 전각공백만 있는 줄) 제거 후 표시
//  MAX_ART_ROWS(24) 초과 시에만 중앙 크롭

void Renderer::drawCombatLayout(const std::vector<std::string>& monsterLines) {
    // HP바(1) + 아트(24) + 턴정보(1) = 26 = CONTENT_H
    // 분리선 제거로 24행 확보:  art2=20 art4=22 art3/5=24 art1=25(1행 크롭)
    static const int MAX_ART_ROWS  = 24;
    const int LEFT_W       = 70;
    const int playerIndent = (LEFT_W - 36) / 2;          // 17
    const int playerRPad   = LEFT_W - playerIndent - 36;  // 17
    const int monsterPad   = 3;

    // ── 빈 줄 판별 (⠀=U+2800, 공백, 　=U+3000 만 있으면 빈 줄) ──
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

    // ── 레이블(line0) 스킵 + 앞뒤 빈 줄 트리밍 ───────────────────
    int mFirst = monsterLines.empty() ? 0 : 1;
    while (mFirst < (int)monsterLines.size() && isBlank(monsterLines[mFirst]))
        ++mFirst;
    int mLast = (int)monsterLines.size() - 1;
    while (mLast > mFirst && isBlank(monsterLines[mLast]))
        --mLast;
    int mCount = (mFirst <= mLast) ? (mLast - mFirst + 1) : 0;

    // MAX_ART_ROWS 초과 시 중앙 크롭
    if (mCount > MAX_ART_ROWS) {
        mFirst += (mCount - MAX_ART_ROWS) / 2;
        mCount  = MAX_ART_ROWS;
    }

    // 구치파치(18행)를 MAX_ART_ROWS 안에서 수직 중앙 정렬
    const int playerTopPad = (MAX_ART_ROWS - KUCHI_ROWS) / 2; // (24-18)/2 = 3

    // 몬스터 아트를 MAX_ART_ROWS 안에서 수직 중앙 정렬
    const int mTopPad = (MAX_ART_ROWS - mCount) / 2;

    for (int row = 0; row < MAX_ART_ROWS; ++row) {
        // 좌측: 구치파치 (상하 3행 패딩으로 중앙 정렬)
        int pRow = row - playerTopPad;
        if (pRow >= 0 && pRow < KUCHI_ROWS) {
            std::cout << std::string(playerIndent, ' ')
                      << KUCHI_ART[pRow]
                      << std::string(playerRPad, ' ');
        } else {
            std::cout << std::string(LEFT_W, ' ');
        }

        // 세로 구분선
        std::cout << "│";

        // 우측: 몬스터 아트 (수직 중앙 정렬)
        int mRow = row - mTopPad;
        if (mRow >= 0 && mRow < mCount)
            std::cout << std::string(monsterPad, ' ') << monsterLines[mFirst + mRow];

        std::cout << "\n";
    }
}

// ── 메시지 / 디버그 ────────────────────────────────────────────────

void Renderer::drawMessage(const std::string& message) {
    std::cout << "\n >> " << message << "\n";
}

void Renderer::drawDebug(const InputEvent& event) {
    if (event.type != InputType::NONE) {
        std::cout << "[DEBUG] Type:" << (int)event.type
                  << " X:" << event.x << " Y:" << event.y
                  << " Key:" << (int)event.key
                  << std::string(20, ' ') << "\r" << std::flush;
    }
}
