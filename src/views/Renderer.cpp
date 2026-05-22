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
// 총 120컬럼
// [다마고치명 14] | [날짜 10] | [배고픔 행복 청결 건강 바] | [돈 9] | [공격/HP/민첩/검/갑옷]

void Renderer::drawTopMenu(const Character& player) {
    const std::string sep = "========================================================================================================================";
    std::cout << sep << "\n";
    std::cout << " 다마고치      |    날짜    |  배고픔   행복    청결    건강  |     돈     |  전투 스탯\n";

    std::string namePart  = padRight(player.getName(), 13);
    std::string datePart  = getCurrentDate();
    std::string hungerB   = getProgressBar(player.getHunger(),      RED);
    std::string happyB    = getProgressBar(player.getHappiness(),   YELLOW);
    std::string cleanB    = getProgressBar(player.getCleanliness(), LBLUE);
    std::string healthB   = getProgressBar(player.getHealth(),      BLUE);

    // 장비 축약 표시
    std::string swordShort = player.getSwordName();
    std::string armorShort = player.getArmorName();

    std::cout << " " << namePart
              << " | " << datePart
              << " | [" << hungerB << "][" << happyB << "][" << cleanB << "][" << healthB << "]"
              << " | " << padLeft(std::to_string(player.getMoney()), 7) << "원"
              << " | ATK:" << player.getTotalAttack()
              << " HP:" << player.getHp() << "/" << player.getMaxHp()
              << " AGI:" << player.getAgility()
              << " [" << swordShort << "/" << armorShort << "]"
              << "\n";
    std::cout << sep << "\n";
}

// ── 하단 메뉴 ──────────────────────────────────────────────────────

void Renderer::drawBottomMenu(const std::vector<std::string>& options) {
    const std::string sep = "========================================================================================================================";
    std::cout << sep << "\n";

    int totalWidth  = 118;
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

// ── 메인 캐릭터 아트 ───────────────────────────────────────────────

void Renderer::drawMainCharacter() {
    std::cout << "\n\n\n";
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣀⠤⠤⠤⢄⣀⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡠⠒⠊⠁⠀⠀⠀⠀⠀⠀⠀⠀⠉⠢⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠜⢀⠤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠤⡈⢆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⡜⠀⠣⣀⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠣⠤⠃⠀⢇⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⢠⠞⠓⠒⠒⠒⠒⠛⠛⢵⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⢸⣢⣀⣀⣀⣀⣀⣀⣤⣞⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⢸⡀⠉⠉⠉⠉⠉⠉⢀⢼⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⡱⠀⠀⠀⠀⠙⠒⠶⠶⠶⠶⠒⠋⠁⠀⠀⠀⠀⠘⢄⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⢠⠊⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⡀⠣⡀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⢰⠁⢸⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣇⠀⢣⠀⠀⠀⠀⠀⠀\n";
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⢪⠀⣸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠤⠃⠀⠀⠀⠀⠀⠀\n";
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠉⠘⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⢣⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠊⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠑⠤⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣤⠊⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠈⠉⠙⠛⡍⠉⠉⠉⠉⡏⠉⠉⠀⢠⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠉⠉⠉⠀⠀⠀⠀⠀⠈⠑⠒⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "\n\n\n";
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
