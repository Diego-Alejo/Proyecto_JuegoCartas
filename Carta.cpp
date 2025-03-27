#include "carta.h"

Carta::Carta() {
    this->poder = 0;
    this->color = 0;
}

Carta::Carta(int poder, int color) {
    this->poder = poder;
    this->color = color;
}

void Carta::mostrar() {
    std::cout << WHITE << paleta[color] << poder << RESET << " ";
}

