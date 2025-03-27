#include "mazo.h"
#include <iostream>

// Constructor del mazo
Mazo::Mazo() {
    // Creamos todas las cartas posibles
    for (int color = 0; color < 4; color++) {
        for (int poder = 1; poder <= 10; poder++) {
            cartas.push_back(Carta(color, poder));
        }
    }
    
    n = cartas.size();  // Guardamos cuántas cartas hay
}

// Barajamos las cartas
void Mazo::barajar() {
    // Usamos random_shuffle pa' mezclar todo bien 
    random_shuffle(cartas.begin(), cartas.end());
    std::cout << "Mazo barajado." << std::endl;
}

// Reparte una carta
Carta Mazo::repartir() {
    if (cartas.empty()) {
        std::cout << "Ya no hay cartas" << std::endl;
        return Carta();  // Devolvemos una carta por defecto si no hay más
    }
    
    Carta c = cartas.back();  // Sacamos la última carta
    cartas.pop_back();  // La quitamos del mazo
    n--;  // Actualizamos el contador
    
    return c;  // Devolvemos la carta que sacamos
}

// Muestra todas las cartas del mazo
void Mazo::mostrar() {
    std::cout << "El mazo tiene " << n << " cartas:" << std::endl;
    
    for (int i = 0; i < n; i++) {
        cartas[i].mostrar();
        
        // Cada 10 cartas saltamos de línea
        if ((i + 1) % 10 == 0) {
            std::cout << std::endl;
        }
    }
    
    std::cout << std::endl;
}

