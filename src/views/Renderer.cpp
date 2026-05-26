#include "Renderer.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <algorithm>

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
                // 블록 요소 (U+2580-U+25FF: ░▒▓█▀▄ 등): 1컬럼
                else if (c == 0xE2 && c2 == 0x96)
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

std::string Renderer::cropCenter(const std::string& str, int width) {
    int total = getDisplayWidth(str);
    if (total <= width) return str;
    int skip = (total - width) / 2; // 양쪽에서 균등하게 제거

    // skip display cols 이후 시작 바이트 탐색
    int w = 0;
    size_t start = 0;
    for (size_t i = 0; i < str.size(); ) {
        unsigned char c = (unsigned char)str[i];
        int cw, step;
        if      (c < 0x80) { cw=1; step=1; }
        else if (c < 0xE0) { cw=1; step=2; }
        else if (c < 0xF0) { cw=2; step=3; }
        else               { cw=2; step=4; }
        if (w + cw > skip) { start = i; break; }
        w += cw; i += step;
        if (w >= skip)     { start = i; break; }
    }

    // start 이후 width display cols 수집
    int shown = 0;
    size_t end = start;
    for (size_t i = start; i < str.size(); ) {
        unsigned char c = (unsigned char)str[i];
        int cw, step;
        if      (c < 0x80) { cw=1; step=1; }
        else if (c < 0xE0) { cw=1; step=2; }
        else if (c < 0xF0) { cw=2; step=3; }
        else               { cw=2; step=4; }
        if (shown + cw > width) break;
        shown += cw; i += step; end = i;
    }
    return str.substr(start, end - start);
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

// ══════════════════════════════════════════════════════════════════
//  아트 파일 로더 (AA/ 디렉토리 기준)
// ══════════════════════════════════════════════════════════════════

// 파일에서 아트 줄을 읽어 앞뒤 빈 줄 제거 후 반환
// maxRows > 0 이면 중앙 크롭
static std::vector<std::string> loadArtFile(const std::string& path, int maxRows = 0) {
    std::vector<std::string> lines;
    std::ifstream f(path);
    if (!f.is_open()) return lines;
    std::string line;
    while (std::getline(f, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        lines.push_back(line);
    }
    // 앞뒤 빈 줄 제거
    auto isBlank = [](const std::string& s) {
        for (unsigned char c : s) if (c > ' ') return false;
        return true;
    };
    while (!lines.empty() && isBlank(lines.front())) lines.erase(lines.begin());
    while (!lines.empty() && isBlank(lines.back()))  lines.pop_back();
    // 중앙 크롭
    if (maxRows > 0 && (int)lines.size() > maxRows) {
        int skip = ((int)lines.size() - maxRows) / 2;
        lines.erase(lines.begin(), lines.begin() + skip);
        lines.resize(maxRows);
    }
    return lines;
}

// ── 아트 캐시 (최초 호출 시 파일 로드) ──────────────────────────────
static const std::vector<std::string>& kuchiArt() {
    static auto v = loadArtFile("AA/kuchipatchi.txt"); // 32행, 크롭 불필요
    return v;
}
static const std::vector<std::string>& eatingArt() {
    static auto v = loadArtFile("AA/chicken.txt");  // 30행
    return v;
}
static const std::vector<std::string>& bathArt() {
    static auto v = loadArtFile("AA/bath.txt");     // 28행
    return v;
}
static const std::vector<std::string>& playArt() {
    static auto v = loadArtFile("AA/toy.txt");      // 22행
    return v;
}
static const std::vector<std::string>& treatArt() {
    static auto v = loadArtFile("AA/syringe.txt");  // 30행
    return v;
}

// ── 아트 수평 중앙 정렬 출력 ─────────────────────────────────────────
// artVWidth: 아트 파일의 최대 시각 너비(컬럼)
// topBlanks/bottomBlanks: 아트 위아래에 삽입할 빈 줄 수
static void printArtCentered(const std::vector<std::string>& art,
                             int artVWidth, int topBlanks, int bottomBlanks) {
    int indent = std::max(0, (161 - artVWidth) / 2);
    for (int i = 0; i < topBlanks;    ++i) std::cout << "\n";
    for (const auto& line : art)
        std::cout << std::string(indent, ' ') << line << "\n";
    for (int i = 0; i < bottomBlanks; ++i) std::cout << "\n";
}

// ── 메인 캐릭터 아트 (구치파치, kuchipatchi.txt) ──────────────────────
// 2+32+2 = 36행 = CONTENT_H, 시각 너비 60col → indent=(161-60)/2=50
void Renderer::drawMainCharacter() {
    printArtCentered(kuchiArt(), 60, 2, 2);
}

// ── 밥 먹는 화면 (chicken.txt, 30행) ─────────────────────────────────
// 3+30+3 = 36행 = CONTENT_H
void Renderer::drawEatingCharacter() {
    printArtCentered(eatingArt(), 60, 3, 3);
}

// ── 목욕 화면 (bath.txt, 28행) ───────────────────────────────────────
// 4+28+4 = 36행 = CONTENT_H
void Renderer::drawBathCharacter() {
    printArtCentered(bathArt(), 60, 4, 4);
}

// ── 놀이 화면 (toy.txt, 22행) ────────────────────────────────────────
// 7+22+7 = 36행 = CONTENT_H
void Renderer::drawPlayCharacter() {
    printArtCentered(playArt(), 60, 7, 7);
}

// ── 치료 화면 (syringe.txt, 30행) ────────────────────────────────────
// 3+30+3 = 36행 = CONTENT_H
void Renderer::drawTreatCharacter() {
    printArtCentered(treatArt(), 60, 3, 3);
}

// ── 몬스터 아트 캐시 (스테이지 1-5, 기본/스몰) ──────────────────────

static const std::vector<std::string>& monsterArtCache(int stage) {
    static std::vector<std::vector<std::string>> cache(5);
    static std::vector<bool> loaded(5, false);
    int idx = stage - 1;
    if (idx < 0 || idx > 4) { static std::vector<std::string> empty; return empty; }
    if (!loaded[idx]) {
        cache[idx] = loadArtFile("AA/monster" + std::to_string(stage) + ".txt");
        loaded[idx] = true;
    }
    return cache[idx];
}

static const std::vector<std::string>& monsterSmallArtCache(int stage) {
    static std::vector<std::vector<std::string>> cache(5);
    static std::vector<bool> loaded(5, false);
    int idx = stage - 1;
    if (idx < 0 || idx > 4) { static std::vector<std::string> empty; return empty; }
    if (!loaded[idx]) {
        cache[idx] = loadArtFile("AA/monster" + std::to_string(stage) + "_small.txt");
        loaded[idx] = true;
    }
    return cache[idx];
}

const std::vector<std::string>& Renderer::getMonsterArt(int stage) {
    return monsterArtCache(stage);
}
const std::vector<std::string>& Renderer::getMonsterSmallArt(int stage) {
    return monsterSmallArtCache(stage);
}

// ── 가위바위보 아트 캐시 ──────────────────────────────────────────────

static const std::vector<std::string>& scissorsArt() {
    static auto v = loadArtFile("AA/scissors.txt"); // 15행, 40col
    return v;
}
static const std::vector<std::string>& rockArt() {
    static auto v = loadArtFile("AA/rock.txt");     // 10행, 40col
    return v;
}
static const std::vector<std::string>& paperArt() {
    static auto v = loadArtFile("AA/paper.txt");    // 14행, 40col
    return v;
}

const std::vector<std::string>& Renderer::getPlayerArt() {
    return kuchiArt();
}
const std::vector<std::string>& Renderer::getRPSArt(int choice) {
    if (choice == 1) return scissorsArt();
    if (choice == 2) return rockArt();
    return paperArt();
}

// ── 검/갑옷 아트 캐시 (sword1-5 / armor1-5) ─────────────────────────────
static const std::vector<std::string>& swordArtCached(int tier) {
    static std::vector<std::vector<std::string>> cache(5);
    static std::vector<bool> loaded(5, false);
    int idx = tier - 1;
    if (idx < 0 || idx > 4) { static std::vector<std::string> empty; return empty; }
    if (!loaded[idx]) { cache[idx] = loadArtFile("AA/sword" + std::to_string(tier) + ".txt"); loaded[idx] = true; }
    return cache[idx];
}
const std::vector<std::string>& Renderer::getSwordArt(int tier) { return swordArtCached(tier); }

static const std::vector<std::string>& armorArtCached(int tier) {
    static std::vector<std::vector<std::string>> cache(5);
    static std::vector<bool> loaded(5, false);
    int idx = tier - 1;
    if (idx < 0 || idx > 4) { static std::vector<std::string> empty; return empty; }
    if (!loaded[idx]) { cache[idx] = loadArtFile("AA/armor" + std::to_string(tier) + ".txt"); loaded[idx] = true; }
    return cache[idx];
}
const std::vector<std::string>& Renderer::getArmorArt(int tier) { return armorArtCached(tier); }

// ── 훈련 아트 캐시 ───────────────────────────────────────────────────────
static const std::vector<std::string>& punchingBagArtCache() {
    static auto v = loadArtFile("AA/punchingbag.txt");
    return v;
}
const std::vector<std::string>& Renderer::getPunchingBagArt() { return punchingBagArtCache(); }

static const std::vector<std::string>& finishLineArtCache() {
    static auto v = loadArtFile("AA/finishline.txt");
    return v;
}
const std::vector<std::string>& Renderer::getFinishLineArt() { return finishLineArtCache(); }

static const std::vector<std::string>& targetArtCache() {
    static auto v = loadArtFile("AA/target.txt");
    return v;
}
const std::vector<std::string>& Renderer::getTargetArt() { return targetArtCache(); }

static const std::vector<std::string>& endingArtCache(int idx) {
    static const char* files[5] = {
        "AA/hero_kuchipatchi.txt",
        "AA/starve_kuchipatchi.txt",
        "AA/runaway_kuchipatchi.txt",
        "AA/trash_kuchipatchi.txt",
        "AA/hospital_kuchipatchi.txt"
    };
    static std::vector<std::vector<std::string>> cache(5);
    static std::vector<bool> loaded(5, false);
    if (idx < 0 || idx >= 5) { static std::vector<std::string> empty; return empty; }
    if (!loaded[idx]) { cache[idx] = loadArtFile(files[idx]); loaded[idx] = true; }
    return cache[idx];
}
const std::vector<std::string>& Renderer::getEndingArt(int idx) { return endingArtCache(idx); }

// ── 아이템 아트 캐시 (potion_power / potion_agility / tether / shield) ───
static const std::vector<std::string>& itemArtCached(int idx) {
    static const char* files[4] = {
        "AA/potion_power.txt", "AA/potion_agility.txt",
        "AA/tether.txt", "AA/shield.txt"
    };
    static std::vector<std::vector<std::string>> cache(4);
    static std::vector<bool> loaded(4, false);
    if (idx < 0 || idx >= 4) { static std::vector<std::string> empty; return empty; }
    if (!loaded[idx]) { cache[idx] = loadArtFile(files[idx]); loaded[idx] = true; }
    return cache[idx];
}
const std::vector<std::string>& Renderer::getItemArt(int idx) { return itemArtCached(idx); }

// ── 전투 결과 아트 ─────────────────────────────────────────────────
static const std::vector<std::string>& winArt() {
    static auto v = loadArtFile("AA/win.txt");  // 17행, 100col
    return v;
}
static const std::vector<std::string>& loseArt() {
    static auto v = loadArtFile("AA/lose.txt"); // 16행, 100col
    return v;
}
const std::vector<std::string>& Renderer::getWinArt()  { return winArt();  }
const std::vector<std::string>& Renderer::getLoseArt() { return loseArt(); }


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
