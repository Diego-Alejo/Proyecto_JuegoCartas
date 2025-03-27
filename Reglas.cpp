#include "./Reglas.h"

// Limpia la pantalla
void limpiarPantalla() {
    #ifdef _WIN32
        system("cls");  // Si estamos en Windows
    #else
        system("clear");  // Si estamos en Linux o Mac
    #endif
}

// Devuelve el nombre del color
std::string nombreColor(int color) {
    switch(color) {
        case 0: return "Rojo";
        case 1: return "Azul";
        case 2: return "Verde";
        case 3: return "Amarillo";
        default: return "Desconocido";
    }
}

// Compara dos cartas para ver cuál gana
bool cartaGana(Carta c1, Carta c2, int colorLider) {
    // Si la primera carta no es del color líder, pierde de una
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

