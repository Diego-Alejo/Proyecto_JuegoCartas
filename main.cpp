#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include "carta.h"
#include "mazo.h"
#include "jugador.h"
#include "utilidades.h"

using namespace std;

int main() {
// Inicializar el generador de números aleatorios
	srand(time(NULL));

// Crear y barajar el mazo
	Mazo baraja;
	baraja.barajar();

// Solicitar número de jugadores
int numJugadores;
	do {
        	cout << "Ingrese el número de jugadores (1-4): ";
		cin >> numJugadores;
	} while (numJugadores < 1 || numJugadores > 4);

// Crear jugadores
	vector<Jugador> jugadores;
	for (int i = 0; i < numJugadores; i++) {
		string nombre = "Jugador " + to_string(i + 1);
		jugadores.push_back(Jugador(nombre));
		}

// Repartir todas las cartas

	int cartasPorJugador = baraja.n / numJugadores;
		for (int i = 0; i < cartasPorJugador * numJugadores; i++) {
		jugadores[i % numJugadores].recibirCarta(baraja.repartir());
		}

// Elegir jugador inicial al azar
	int jugadorActual = rand() % numJugadores;
	cout << jugadores[jugadorActual].nombre << " comienza el juego." << endl;

// Juego principal
	int ronda = 1;

	while (true) {
		cout << "\n--- Ronda " << ronda << " ---" << endl;

// Verificar si algún jugador tiene cartas
		bool hayCartas = false;
		for (int i = 0; i < numJugadores; i++) {
			if (!jugadores[i].mano.empty()) {
			hayCartas = true;
			break;
			}
		}

			if (!hayCartas) {
			cout << "No quedan más cartas jugables. ¡Fin del juego!" << endl;
			break;
			}

// Saltar jugadores sin cartas

			while (jugadores[jugadorActual].mano.empty()) {
			jugadorActual = (jugadorActual + 1) % numJugadores;
			}

		cout << "Turno de " << jugadores[jugadorActual].nombre << endl;

// Mostrar mano del jugador actual
		jugadores[jugadorActual].mostrarMano();

// Jugador selecciona una carta
		int indiceCartaElegida;
		do {
			cout << "Seleccione una carta (1-" << jugadores[jugadorActual].mano.size() << "): ";
			cin >> indiceCartaElegida;
			indiceCartaElegida--; // Ajustar para índice base 0
			} while (indiceCartaElegida < 0 || indiceCartaElegida >= jugadores[jugadorActual].mano.size());

// Jugar la carta seleccionada
			Carta cartaLider = jugadores[jugadorActual].jugarCarta(indiceCartaElegida);
			int colorLider = cartaLider.color;

			cout << jugadores[jugadorActual].nombre << " juega: ";
			cartaLider.mostrar();
			cout << " (" << nombreColor(colorLider) << ")" << endl;

// Crear vector para almacenar las cartas jugadas

	vector<Carta> cartasJugadas;
	vector<int> jugadoresTurno;

	cartasJugadas.push_back(cartaLider);
	jugadoresTurno.push_back(jugadorActual);

// Otros jugadores responden
		for (int i = 1; i < numJugadores; i++) {
			int siguienteJugador = (jugadorActual + i) % numJugadores;

// Saltar jugadores sin cartas

			if (jugadores[siguienteJugador].mano.empty()) {
			cout << jugadores[siguienteJugador].nombre << " no tiene cartas." << endl;
			continue;
		}

            cout << "\nTurno de " << jugadores[siguienteJugador].nombre << endl;
            cout << "Color requerido: " << nombreColor(colorLider) << endl;
            
            // Mostrar todas las cartas del jugador
            jugadores[siguienteJugador].mostrarMano();
            
            // Jugador selecciona cualquier carta
            int seleccion;
            do {
                cout << "Seleccione una carta (1-" << jugadores[siguienteJugador].mano.size() << "): ";
                cin >> seleccion;
                seleccion--; // Ajustar para índice base 0
            } while (seleccion < 0 || seleccion >= jugadores[siguienteJugador].mano.size());
            
            Carta cartaJugada = jugadores[siguienteJugador].jugarCarta(seleccion);
            
            cout << jugadores[siguienteJugador].nombre << " juega: ";
            cartaJugada.mostrar();
            cout << " (" << nombreColor(cartaJugada.color) << ")" << endl;
            
            // Informar si la carta es de diferente color
            if (cartaJugada.color != colorLider) {
                cout << "¡Carta de diferente color! No puede ganar esta ronda." << endl;
            } else if (cartaJugada.poder <= cartaLider.poder) {
                cout << "Carta del mismo color pero valor insuficiente. No puede ganar esta ronda." << endl;
            }
            
            cartasJugadas.push_back(cartaJugada);
            jugadoresTurno.push_back(siguienteJugador);
        }
        
        // Determinar ganador de la ronda
        int ganadorRonda = jugadoresTurno[0]; // Por defecto, el jugador que inició
        int valorMasAlto = cartasJugadas[0].poder;
        
        for (int i = 1; i < cartasJugadas.size(); i++) {
            if (cartasJugadas[i].color == colorLider && cartasJugadas[i].poder > valorMasAlto) {
                valorMasAlto = cartasJugadas[i].poder;
                ganadorRonda = jugadoresTurno[i];
            }
        }
        
        cout << "\n¡" << jugadores[ganadorRonda].nombre << " gana la ronda!" << endl;
        
        // Otorgar cartas al ganador
        jugadores[ganadorRonda].cartasGanadas += cartasJugadas.size();
        cout << jugadores[ganadorRonda].nombre << " ha ganado " << jugadores[ganadorRonda].cartasGanadas << " cartas en total." << endl;
        
        // La siguiente ronda la inicia el ganador
        jugadorActual = ganadorRonda;
        ronda++;
        
        cout << "\nPresione Enter para continuar...";
        cin.ignore();
        cin.get();
        limpiarPantalla();
    }
    
    // Determinar ganador del juego
    int maxCartas = 0;
    vector<int> ganadores;
    
    // Encontrar el máximo número de cartas ganadas
    for (int i = 0; i < numJugadores; i++) {
        cout << jugadores[i].nombre << " ganó " << jugadores[i].cartasGanadas << " cartas." << endl;
        if (jugadores[i].cartasGanadas > maxCartas) {
            maxCartas = jugadores[i].cartasGanadas;
            ganadores.clear();
            ganadores.push_back(i);
        } else if (jugadores[i].cartasGanadas == maxCartas) {
            ganadores.push_back(i);
        }
    }
    
    // Mostrar resultado final
    if (ganadores.size() == 1) {
        cout << "\n¡" << jugadores[ganadores[0]].nombre << " gana el juego con " << maxCartas << " cartas!" << endl;
    } else {
        cout << "\n¡Empate! Los siguientes jugadores empataron con " << maxCartas << " cartas:" << endl;
        for (int i = 0; i < ganadores.size(); i++) {
            cout << "- " << jugadores[ganadores[i]].nombre << endl;
        }
    }
    
    return 0;
