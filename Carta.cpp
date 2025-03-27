#include "carta.h"

// Constructor de la carta
Carta::Carta(int _color, int _poder) {
    color = _color;
    poder = _poder;
}

void Carta::mostrar() {
    std::cout << WHITE << paleta[color] << poder << RESET << " ";
}

