#ifndef REGLAS_H
#define REGLAS_H

#include <string>
#include <cstdlib>
#include "./Carta.h"

// Esta función limpia la pantalla
void limpiarPantalla();

// Esta función nos dice el nombre del color según su número
std::string nombreColor(int color);

// Esta función compara dos cartas 
bool cartaGana(Carta c1, Carta c2, int colorLider);

#endif // REGLAS_H
