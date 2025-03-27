#ifndef MAZO_H
#define MAZO_H

#include <vector>
#include <algorithm>
#include <cstdlib>
#include "carta.h"

class Mazo {
public:
    std::vector<Carta> cartas;  // Aquí guardamos todas las cart
    int n;  // Cuántas cartas hay en total
    
    // Este es el constructor
    Mazo();
    
    // Esta función revuelve las cartas
    void barajar();
    
    
    Carta repartir();
    
    // Esta función muestra
    void mostrar();
};

#endif // MAZO_H


