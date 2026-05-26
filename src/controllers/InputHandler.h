#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <termios.h>
#include <string>

enum class InputType {
    NONE,
    KEYBOARD,
    MOUSE_PRESS,
    MOUSE_RELEASE
};

enum class SpecialKey {
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct InputEvent {
    InputType type;
    int button;
    int x;
    int y;
    char key;
    SpecialKey special;

    InputEvent() : type(InputType::NONE), button(-1), x(-1), y(-1), key(0), special(SpecialKey::NONE) {}
};

class InputHandler {
public:
    InputHandler();
    ~InputHandler();

    InputEvent pollEvent();
    void enableMouseTracking();
    void disableMouseTracking();

private:
    struct termios orig_termios;
    void enableRawMode();
    void disableRawMode();

    // ── 시그널 처리 ────────────────────────────────────────────
    // Ctrl+C / kill 등 비정상 종료 시에도 터미널 상태를 복원하기 위해
    // 정적 인스턴스 포인터와 시그널 핸들러를 사용한다.
    static InputHandler* s_instance;
    static void signalHandler(int sig);
};

#endif
