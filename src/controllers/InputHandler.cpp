#include "InputHandler.h"
#include <iostream>
#include <unistd.h>
#include <vector>

InputHandler::InputHandler() {
    enableRawMode();
    enableMouseTracking();
}

InputHandler::~InputHandler() {
    disableMouseTracking();
    disableRawMode();
}

void InputHandler::enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void InputHandler::disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void InputHandler::enableMouseTracking() {
    std::cout << "\033[?1000h\033[?1006h" << std::flush;
}

void InputHandler::disableMouseTracking() {
    std::cout << "\033[?1006l\033[?1000l" << std::flush;
}

InputEvent InputHandler::pollEvent() {
    InputEvent event;
    char c;
    
    // Set non-blocking read
    struct termios tmp;
    tcgetattr(STDIN_FILENO, &tmp);
    tmp.c_cc[VMIN] = 0;
    tmp.c_cc[VTIME] = 1; // 100ms timeout
    tcsetattr(STDIN_FILENO, TCSANOW, &tmp);

    if (read(STDIN_FILENO, &c, 1) == 0) {
        // Reset to blocking for next time if needed, but here we just return NONE
        return event;
    }

    if (c == '\033') { // Escape sequence
        char next;
        if (read(STDIN_FILENO, &next, 1) == 0) {
            event.type = InputType::KEYBOARD;
            event.key = c;
            return event;
        }

        if (next == '[') {
            std::string seq;
            while (read(STDIN_FILENO, &c, 1) == 1) {
                seq += c;
                if (c == 'M' || c == 'm') break;
            }

            if (seq.substr(0, 1) == "<") {
                // SGR Mouse format: <Pb;Px;PyM/m
                bool isRelease = (seq.back() == 'm');
                std::string data = seq.substr(1, seq.size() - 2);

                size_t first = data.find(';');
                size_t second = data.find(';', first + 1);

                if (first != std::string::npos && second != std::string::npos) {
                    event.type = isRelease ? InputType::MOUSE_RELEASE : InputType::MOUSE_PRESS;
                    event.button = std::stoi(data.substr(0, first));
                    event.x = std::stoi(data.substr(first + 1, second - first - 1));
                    event.y = std::stoi(data.substr(second + 1));
                }
            } else if (seq.size() == 1) {
                // Arrow keys: ESC [ A/B/C/D
                event.type = InputType::KEYBOARD;
                if (seq[0] == 'A') event.special = SpecialKey::UP;
                else if (seq[0] == 'B') event.special = SpecialKey::DOWN;
                else if (seq[0] == 'C') event.special = SpecialKey::RIGHT;
                else if (seq[0] == 'D') event.special = SpecialKey::LEFT;
            }
        }
    } else {
        event.type = InputType::KEYBOARD;
        event.key = c;
    }

    return event;
}
