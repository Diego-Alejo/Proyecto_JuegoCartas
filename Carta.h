#include <iostream>
#include <string>


#define  BG_BLACK    "\x1B[40m"
#define  BG_RED      "\x1B[41m"
#define  BG_GREEN    "\x1B[42m"
#define  BG_AMARILLO "\x1B[43m"
#define  BG_BLUE     "\x1B[44m"
#define  BG_MAGENTA  "\x1B[45m"
#define  BG_CYAN     "\x1B[46m"
#define  BG_BLANCO   "\x1B[47m"
#define  BG_NARANJA  "\x1B[48;2;255;128;0m"
#define  BG_LBLUE    "\x1B[48;2;53;149;240m"
#define  BG_LGREEN   "\x1B[48;2;17;245;120m"
#define  BG_GRAY     "\x1B[48;2;176;174;174m"
#define  BG_ROSE     "\x1B[48;2;255;151;203m"
#define  WHITE       "\x1B[37m"
#define  RESET       "\x1b[0m"

class Carta {
    
public:
    std::string paleta[4] = {BG_RED, BG_BLUE, BG_BLACK, BG_GREEN};
    int poder;
    int color;
    Carta();
    Carta(int poder, int color);
    void mostrar();
};
