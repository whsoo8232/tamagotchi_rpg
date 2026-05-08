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
};

#endif
