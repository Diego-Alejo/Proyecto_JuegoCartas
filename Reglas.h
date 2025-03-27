#ifndef REGLAS_H
#define REGLAS_H

#include <string>
#include <cstdlib>
#include "carta.h"

using namespace std;

// Esta función nos dice el nombre del color según su número asignado
string nombreColor(int color) {
    switch(color) {
        case 0: return "Rojo";
        case 1: return "Azul";
        case 2: return "Verde";
        case 3: return "Amarillo";
        default: return "Desconocido";
    }
}

// Esta función limpia la pantalla (Se le preguntó al profesor ya que se sacó de internet
void limpiarPantalla() {
    #ifdef _WIN32
        system("cls");  // Si estamos en Windows
    #else
        system("clear");  // Si estamos en Linux o Mac
    #endif
}

// Esta función compara dos cartas para ver cual gana
bool cartaGana(Carta c1, Carta c2, int colorLider) {
    // Si la primera carta no es del color líder, pierde
    if (c1.color != colorLider) {
        return false;
    }
    
    // Si la segunda carta no es del color líder, la primera gana
    if (c2.color != colorLider) {
        return true;
    }
    
    // Si ambas son del mismo color, gana la de mayor poder
    return c1.poder > c2.poder;
}

#endif

