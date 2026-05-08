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
    static const std::string LBLUE;
    static const std::string BLUE;
    static const std::string RESET;

    static void clearCanvas();
    static void drawTopMenu(const Character& player, const std::string& date);
    static void drawBottomMenu(const std::vector<std::string>& options);
    static void drawMainCharacter();
    static void drawMessage(const std::string& message);
    static void drawDebug(const InputEvent& event);
    
    static int getDisplayWidth(const std::string& str);
    static std::string padRight(const std::string& str, int targetWidth);
    
private:
    static std::string getProgressBar(int value, const std::string& color);
};

#endif
