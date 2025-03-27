#include "jugador.h"
#include <iostream>

Jugador::Jugador(std::string nombre) {
	this->nombre = nombre;
	cartasGanadas = 0;
	}

void Jugador::recibirCarta(Carta carta) {
	mano.push_back(carta);
	}

void Jugador::mostrarMano() {
	std::cout << nombre << " - Cartas en mano: " << mano.size() << std::endl;
	for (int i = 0; i < mano.size(); i++) {
		std::cout << i + 1 << ": ";
		mano[i].mostrar();
		std::cout << std::endl;
		}
	}

Carta Jugador::jugarCarta(int indice) {
	if (indice < 0 || indice >= mano.size()) {
		std::cout << "Índice inválido." << std::endl;
		return Carta(0, 0);
		}

	Carta carta = mano[indice];
	mano.erase(mano.begin() + indice);
	return carta;
	}

