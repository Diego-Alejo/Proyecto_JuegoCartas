#include "jugador.h"
#include <iostream>

// Constructor del jugador
Jugador::Jugador(std::string _nombre) {
    nombre = _nombre;
    cartasGanadas = 0;  // Arrancamos sin cartas ganadas
}

// Recibe una carta
void Jugador::recibirCarta(Carta c) {
    mano.push_back(c);  // Agregamos la carta a la mano
}

// Muestra todas las cartas en la mano
void Jugador::mostrarMano() {
    std::cout << "Cartas de " << nombre << " (" << mano.size() << "):" << std::endl;
    
    for (int i = 0; i < mano.size(); i++) {
        std::cout << (i + 1) << ". ";  // Mostramos el número 
        mano[i].mostrar();
        
        // Cada 5 cartas saltamos de renglón
        if ((i + 1) % 5 == 0) {
            std::cout << std::endl;
        }
    }
    
    std::cout << std::endl;
}

// Juega una carta de la mano
Carta Jugador::jugarCarta(int indice) {
    if (indice < 0 || indice >= mano.size()) {
        std::cout << "Esa carta no existe." << std::endl;
        return Carta();  // Devolvemos una carta por defecto 
    }
    
    Carta c = mano[indice];  // Guardamos la carta que vamos a jugar
    
    // Quitamos la carta de la mano
    mano.erase(mano.begin() + indice);
    
    return c;  // Devolvemos la carta que jugamos
}



