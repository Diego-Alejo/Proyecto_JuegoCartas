#include "Reglas.h"
#include <cstdlib>

void limpiarPantalla() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

std::string nombreColor(int color) {
    std::string nombres[4] = {"Rojo", "Azul", "Negro", "Verde"};
    return nombres[color];
}
