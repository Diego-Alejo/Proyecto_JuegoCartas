#ifndef JUGADOR_H
#define JUGADOR_H

#include <string>
#include <vector>
#include "./Carta.h"

class Jugador {
public:
    std::string nombre;  // Cómo se llama 
    std::vector<Carta> mano;  // Las cartas que tiene en la mano
    int cartasGanadas;  // Cuántas cartas ha ganado
    
    // Este es el constructor
    Jugador(std::string _nombre = "Jugador");
    
    
    void recibirCarta(Carta c);
    
    // Esta función muestra todas las cartas que tiene 
    void mostrarMano();
    
    // Esta función es pa' jugar una carta de la mano
    Carta jugarCarta(int indice);
};

#endif // JUGADOR_H

