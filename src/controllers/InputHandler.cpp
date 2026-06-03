#include "InputHandler.h"
#include <iostream>
#include <unistd.h>
#include <csignal>
#include <cstring>
#include <vector>

// ── 정적 멤버 초기화 ────────────────────────────────────────────────
InputHandler* InputHandler::s_instance = nullptr;

// ── 시그널 핸들러 ───────────────────────────────────────────────────
// async-signal-safe 함수만 사용: write(), tcsetattr(), _exit()
// std::cout, exit() 등은 시그널 핸들러에서 사용하면 안 됨
void InputHandler::signalHandler(int /*sig*/) {
    const char* disableMouse = "\033[?1006l\033[?1000l\033[?25h"; // 마우스 해제 + 커서 복원
    write(STDOUT_FILENO, disableMouse, strlen(disableMouse));

    // raw 모드 → 원래 termios 복원
    if (s_instance) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &s_instance->orig_termios);
    }

    _exit(0); // 소멸자 호출 없이 즉시 종료 (시그널 핸들러에서 안전)
}

// ── 생성자 / 소멸자 ─────────────────────────────────────────────────

InputHandler::InputHandler() {
    s_instance = this;
    // Ctrl+C (SIGINT) 와 kill (SIGTERM) 모두 처리
    signal(SIGINT,  signalHandler);
    signal(SIGTERM, signalHandler);
    enableRawMode();
    enableMouseTracking();
}

InputHandler::~InputHandler() {
    // 정상 종료 시: 핸들러 원복 후 터미널 복원
    s_instance = nullptr;
    signal(SIGINT,  SIG_DFL);
    signal(SIGTERM, SIG_DFL);
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
