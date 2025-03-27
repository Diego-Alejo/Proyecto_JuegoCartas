// Esta es la clase Carta, es como la cédula de cada carta del juego
#ifndef CARTA_H
#define CARTA_H

#include <iostream>
#include <string>
using namespace std;

#define  BG_BLACK    "\x1B[40m"
#define  BG_RED      "\x1B[41m"
#define  BG_GREEN    "\x1B[42m"
#define  BG_AMARILLO "\x1B[43m"
#define  BG_BLUE     "\x1B[44m"
#define  BG_MAGENTA  "\x1B[45m"
#define  BG_CYAN     "\x1B[46m"
#define  BG_BLANCO   "\x1B[47m"
#define  BG_NARANJA  "\x1B[48;2;255;128;0m"
#define  WHITE       "\x1B[37m"
#define  RESET       "\x1b[0m"

class Carta {
public:
    // Estos son los datos de la carta
    int color;  // 0=Rojo, 1=Azul, 2=Verde, 3=Amarillo
    int poder;  // Qué tan fuerte es la carta, entre 1 y 10
    
    string paleta[4] = {BG_RED, BG_BLUE, BG_GREEN, BG_AMARILLO};
    
    // Este es el constructor para "construir las cartas"
    Carta(int _color = 0, int _poder = 1) {
        color = _color;
        poder = _poder;
    }
    
    // Esta función muestra la carta con colores
    void mostrar() {
        cout << WHITE << paleta[color] << poder << RESET << " ";
    }
};

#endif

