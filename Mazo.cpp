//Clase Mazo.cpp
#include "mazo.h"
#include <cstdlib>
#include <ctime>


Mazo::Mazo(){
	int indice = 0;
	n = 36;

	for(int i = 1; i < 9; i++) {
		for(int j = 0; j < 4; j++) {
			indice = (4*i)+j;
			Cartas[indice].poder = i+1;
			Cartas[indice].color = j;
			}
		}
	}

void Mazo::mostrar() {
	int indice = 0;

	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 4; j++) {
			indice = (4*i)+j;
			Cartas[indice].mostrar();
			}
		std::cout << std::endl;
		}
	}

void Mazo::barajar() {
	srand(time(NULL));
	for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        
	// Intercambiamos Cartas en posición [i] con cartas en posición [j]
        
	Carta temp = Cartas[i];
        Cartas[i] = Cartas[j];
        Cartas[j] = temp;
	}
}

Carta Mazo::repartir() {
	if( n<= 0){
		std::cout << "No hay más cartas en el mazo." << std::endl;
        return Carta(0, 0);
    }

    n--;
    return Cartas[n];
}
