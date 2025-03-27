#ifndef MAZO_H
#define MAZO_H

#include <vector>
#include <algorithm>
#include <cstdlib>
#include "carta.h"

using namespace std;

class Mazo {
public:
    vector<Carta> cartas;  // Aquí guardamos todas las cartas creadas
    int n;  // Cuántas cartas hay en total
    
    // Este es el constructor para armar las cartas desde 0
    Mazo() {
        // Creamos todas las cartas posibles, ¡qué chimba!
        for (int color = 0; color < 4; color++) {
            for (int poder = 1; poder <= 10; poder++) {
                cartas.push_back(Carta(color, poder));
            }
        }
        
        n = cartas.size();  // Guardamos cuántas cartas hay
    }
    
    // Esta función revuelve las cartas
    void barajar() {
        // Usamos random_shuffle una función de internet para mezclar todo bien mezclado
        random_shuffle(cartas.begin(), cartas.end());
        cout << "¡Mazo barajado." << endl;
    }
    
    // Esta función saca una carta del mazo, para repartirla
    Carta repartir() {
        if (cartas.empty()) {
            cout << "Ya no hay cartas" << endl;
            return Carta();  // Devolvemos una carta por defecto si no hay más
        }
        
        Carta c = cartas.back();  // Sacamos la última carta
        cartas.pop_back();  // La quitamos del mazo
        n--;  // Actualizamos el contador
        
        return c;  // Devolvemos la carta que sacamos
    }
    
    // Esta función muestra todas las cartas del mazo
    void mostrar() {
        cout << "El mazo tiene " << n << " cartas:" << endl;
        
        for (int i = 0; i < n; i++) {
            cartas[i].mostrar();
            cout << " ";
            
            // Cada 10 cartas saltamos de líneq, solo es estético
            if ((i + 1) % 10 == 0) {
                cout << endl;
            }
        }
        
        cout << endl;
    }
};

#endif

