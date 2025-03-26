//Clase jugador.h

#ifndef JUGADOR_H
#define JUGADOR_H

#include <vector>
#include <string>
#include "carta.h"

class Jugador {
	public:
	std::vector<Carta> mano;
	int cartasGanadas;
	std::string nombre;

	Jugador(std::string nombre);
	void recibirCarta(Carta carta);
	void mostrarMano();
	Carta jugarCarta(int indice);

};


#endif //
