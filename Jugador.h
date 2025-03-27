#ifndef JUGADOR_H
#define JUGADOR_H

#include <string>
#include <vector>
#include "carta.h"

using namespace std;

class Jugador {
public:
    string nombre; 
    vector<Carta> mano;  // Las cartas que tiene en la mano
    int cartasGanadas;  // Cuántas cartas ha ganado
    
    // Este es el constructorpara crear un jugador nuevo
    Jugador(string _nombre = "Parcero") {
        nombre = _nombre;
        cartasGanadas = 0;  // Arrancamos sin cartas ganadas
    }
    
    // Esta función es para recibir una carta nueva
    void recibirCarta(Carta c) {
        mano.push_back(c);  // Agregamos la carta a la mano
    }
    
    // Esta función muestra todas las cartas que tiene el jugador
    void mostrarMano() {
        cout << "Cartas de " << nombre << " (" << mano.size() << "):" << endl;
        
        for (int i = 0; i < mano.size(); i++) {
            cout << (i + 1) << ". ";  // Mostramos el número para elegir
            mano[i].mostrar();
            cout << "  ";
            
            // Cada 5 cartas saltamos de línea, creo que se ve mejor
            if ((i + 1) % 5 == 0) {
                cout << endl;
            }
        }
        
        cout << endl;
    }
    
    // Esta función es para jugar una carta de la mano
    Carta jugarCarta(int indice) {
        if (indice < 0 || indice >= mano.size()) {
            cout << "Esa carta no existe" << endl;
            return Carta();  // Devolvemos una carta por defecto si el índice está mal
        }
        
        Carta c = mano[indice];  // Guardamos la carta que vamos a jugar
        
        // Quitamos la carta de la mano
        mano.erase(mano.begin() + indice);
        
        return c;  // Devolvemos la carta que jugamos
    }
};

#endif

