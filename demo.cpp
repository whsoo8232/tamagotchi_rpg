#include <iostream>
#include <string>
#include <cstdlib> // For system("clear")

void displayUI(const std::string& RED, const std::string& YELLOW, const std::string& LBLUE, const std::string& BLUE, const std::string& RESET) {
    system("clear"); // Linux/Unix command to clear terminal
    std::cout << "========================================================================================================================" << std::endl;
    std::cout << " 다마고치     |      날짜      |  배고픔    행복     청결     건강     |       돈       |             스탯             |" << std::endl;
    std::cout << " 이름         |   0000-00-00   |  [" << RED << "#####" << RESET << "]  [" << YELLOW << "#####" << RESET << "]  [" << LBLUE << "#####" << RESET << "]  [" << BLUE << "#####" << RESET << "]   |      00000     |    공격력:0000  체력:0000    |" << std::endl;
    std::cout << "========================================================================================================================" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "                                               .-----------------------." << std::endl;
    std::cout << "                                           _.-'         _   _           '-._" << std::endl;
    std::cout << "                                         .'           ( )_( )  _          '." << std::endl;
    std::cout << "                                        /            ( ^_^ )  ( )          \\" << std::endl;
    std::cout << "                                       |    _         (   )   '-'           |" << std::endl;
    std::cout << "                                       |   (o)     _________________    _   |" << std::endl;
    std::cout << "                                       |  /   \\   |                 |  (o)  |" << std::endl;
    std::cout << "                                       |  \\___/   |    ( @ _ @ )    | /   \\ |" << std::endl;
    std::cout << "                                       |          |_________________| \\___/ |" << std::endl;
    std::cout << "                                       |                                    |" << std::endl;
    std::cout << "                                        \\             POMPOMPURIN          /" << std::endl;
    std::cout << "                                         '.         ( )     ( )     ( )   .' " << std::endl;
    std::cout << "                                           '-._                        _.-'  " << std::endl;
    std::cout << "                                               '----------------------'      " << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "========================================================================================================================" << std::endl;
    std::cout << "|                            |                             |                             |                             |              " << std::endl;
    std::cout << "|                            |                             |                             |                             |              " << std::endl;
    std::cout << "|            육성            |             전투            |             장비            |             훈련            |" << std::endl;
    std::cout << "|                            |                             |                             |                             | " << std::endl;
    std::cout << "|                            |                             |                             |                             | " << std::endl;
    std::cout << "========================================================================================================================" << std::endl;
}

int main() {
    // ANSI color codes
    const std::string RED = "\033[31m";
    const std::string YELLOW = "\033[33m";
    const std::string LBLUE = "\033[36m";
    const std::string BLUE = "\033[34m";
    const std::string RESET = "\033[0m";

    std::string input;
    while (true) {
        displayUI(RED, YELLOW, LBLUE, BLUE, RESET);
        std::cout << "명령어를 입력하세요 (종료: exit) >> ";
        std::getline(std::cin, input);

        if (input == "exit") {
            break;
        }
    }

    return 0;
}
