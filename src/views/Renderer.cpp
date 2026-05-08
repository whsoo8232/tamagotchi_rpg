#include "Renderer.h"
#include <iostream>
#include <cstdlib>
#include <iomanip>

const std::string Renderer::RED = "\033[31m";
const std::string Renderer::YELLOW = "\033[33m";
const std::string Renderer::LBLUE = "\033[36m";
const std::string Renderer::BLUE = "\033[34m";
const std::string Renderer::RESET = "\033[0m";

void Renderer::clearCanvas() {
    std::cout << "\033[2J\033[H" << std::flush;
}

std::string Renderer::getProgressBar(int value, const std::string& color) {
    int bars = value / 20;
    std::string barStr = color;
    for (int i = 0; i < 5; ++i) {
        if (i < bars) barStr += "#";
        else barStr += " ";
    }
    barStr += RESET;
    return barStr;
}

int Renderer::getDisplayWidth(const std::string& str) {
    int width = 0;
    for (size_t i = 0; i < str.length(); ) {
        unsigned char c = str[i];
        if (c < 0x80) { width += 1; i += 1; }
        else if (c < 0xE0) { width += 1; i += 2; }
        else if (c < 0xF0) { 
            // UTF-8 3-byte sequence
            if (i + 2 < str.length()) {
                unsigned char c2 = str[i+1];
                unsigned char c3 = str[i+2];
                // Braille Patterns (U+2800–U+28FF): narrow, 1 column wide
                if (c == 0xE2 && (c2 >= 0xA0 && c2 <= 0xBF)) {
                    width += 1;
                } else {
                    width += 2; // CJK and other wide 3-byte characters
                }
            } else {
                width += 2;
            }
            i += 3;
        }
        else { width += 2; i += 4; }
    }
    return width;
}


std::string Renderer::padRight(const std::string& str, int targetWidth) {
    int currentWidth = getDisplayWidth(str);
    std::string padded = str;
    for (int i = 0; i < (targetWidth - currentWidth); ++i) {
        padded += " ";
    }
    return padded;
}

void Renderer::drawTopMenu(const Character& player, const std::string& date) {
    std::cout << "========================================================================================================================" << std::endl;
    std::cout << " 다마고치      |      날짜      |  배고픔    행복     청결     건강     |      돈      |            스탯" << std::endl;

    std::string namePart = padRight(player.getName(), 13);
    std::string hungerBar = getProgressBar(player.getHunger(), RED);
    std::string happinessBar = getProgressBar(player.getHappiness(), YELLOW);
    std::string cleanlinessBar = getProgressBar(player.getCleanliness(), LBLUE);
    std::string healthBar = getProgressBar(player.getHealth(), BLUE);

    // 스탯 표시에서 콜론 뒤의 공백을 제거하여 밀착 표시
    std::cout << " " << namePart << " |   " << date << "   |  [" 
              << hungerBar << "]  [" 
              << happinessBar << "]  [" 
              << cleanlinessBar << "]  [" 
              << healthBar << "]   |   " 
              << std::setw(8) << player.getMoney() << "   | 공격력:" 
              << player.getAttack() << " 체력:" 
              << player.getHp() << " 민첩:"
              << player.getAgility() << std::endl;

    std::cout << "========================================================================================================================" << std::endl;
}

void Renderer::drawDebug(const InputEvent& event) {
    if (event.type != InputType::NONE) {
        std::cout << "[DEBUG] Last Input - Type: " << (int)event.type 
                  << " | X: " << event.x << " | Y: " << event.y 
                  << " | Key: " << (int)event.key << std::string(20, ' ') << "\r" << std::flush;
    }
}

void Renderer::drawMainCharacter() {
    std::cout << "" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣀⠤⠤⠤⢄⣀⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀" << std::endl;
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡠⠒⠊⠁⠀⠀⠀⠀⠀⠀⠀⠀⠉⠢⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀" << std::endl;
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠜⢀⠤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠤⡈⢆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀" << std::endl;
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⡜⠀⠣⣀⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠣⠤⠃⠀⢇⠀⠀⠀⠀⠀⠀⠀⠀⠀" << std::endl;
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⢠⠞⠓⠒⠒⠒⠒⠛⠛⢵⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀" << std::endl;
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⢸⣢⣀⣀⣀⣀⣀⣀⣤⣞⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀" << std::endl;
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⢸⡀⠉⠉⠉⠉⠉⠉⢀⢼⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀" << std::endl;
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⡱⠀⠀⠀⠀⠙⠒⠶⠶⠶⠶⠒⠋⠁⠀⠀⠀⠀⠘⢄⠀⠀⠀⠀⠀⠀⠀⠀" << std::endl;
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⢠⠊⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⡀⠣⡀⠀⠀⠀⠀⠀⠀" << std::endl;
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⢰⠁⢸⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣇⠀⢣⠀⠀⠀⠀⠀⠀" << std::endl;
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⢪⠀⣸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠤⠃⠀⠀⠀⠀⠀⠀" << std::endl;
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠉⠘⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀" << std::endl;
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⢣⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠊⠀⠀⠀⠀⠀⠀⠀⠀" << std::endl;
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠑⠤⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣤⠊⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀" << std::endl;
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠈⠉⠙⠛⡍⠉⠉⠉⠉⡏⠉⠉⠀⢠⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀" << std::endl;
    std::cout << "                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠉⠉⠉⠀⠀⠀⠀⠀⠈⠑⠒⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "" << std::endl;
}

void Renderer::drawBottomMenu(const std::vector<std::string>& options) {
    std::cout << "========================================================================================================================" << std::endl;
    
    // Top padding line
    std::cout << "|";
    int totalWidth = 118;
    int buttonWidth = totalWidth / options.size();
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << std::string(buttonWidth, ' ') << "|";
    }
    std::cout << std::endl;

    // Content line
    std::cout << "|";
    for (const auto& opt : options) {
        int labelWidth = getDisplayWidth(opt);
        int padLeft = (buttonWidth - labelWidth) / 2;
        int padRight = buttonWidth - labelWidth - padLeft;
        std::cout << std::string(padLeft, ' ') << opt << std::string(padRight, ' ') << "|";
    }
    std::cout << std::endl;

    // Bottom padding line
    std::cout << "|";
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << std::string(buttonWidth, ' ') << "|";
    }
    std::cout << std::endl;

    std::cout << "========================================================================================================================" << std::endl;
}

void Renderer::drawMessage(const std::string& message) {
    std::cout << "\n >> " << message << std::endl;
}
