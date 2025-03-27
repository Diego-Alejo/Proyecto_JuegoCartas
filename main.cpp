#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include "carta.h"
#include "mazo.h"
#include "jugador.h"
#include "Reglas.h"

using namespace std;

// Función pa limpiar la pantalla
void limpiarPantalla() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Función para saber el color en palabras
string nombreColor(int color) {
    switch(color) {
        case 0: return "Rojo";
        case 1: return "Azul";
        case 2: return "Verde";
        case 3: return "Amarillo";
        default: return "Desconocido";
    }
}

int main() {
    // Arrancamos con los números aleatorios
    srand(time(NULL));

    // Armamos el mazo de cartas
    Mazo baraja;
    baraja.barajar();

    // Preguntamos cuántas personas van a jugas
    int numJugadores;
    do {
        cout << "¿Cuántos jugadores van a jugar (1-4): ";
        cin >> numJugadores;
        if (numJugadores < 1 || numJugadores > 4) {
            cout << "¡Tienen que ser entre 1 y 4 jugadores." << endl;
        }
    } while (numJugadores < 1 || numJugadores > 4);

    // Creamos los jugadores con este vector
    vector<Jugador> jugadores;
    for (int i = 0; i < numJugadores; i++) {
        string nombre = "Jugador " + to_string(i + 1);
        jugadores.push_back(Jugador(nombre));
        cout << "¡" << nombre << " estamos listos para el juego!" << endl;
    }

    // Repartimos todas las carticas
    int cartasPorJugador = baraja.n / numJugadores;
    for (int i = 0; i < cartasPorJugador * numJugadores; i++) {
        jugadores[i % numJugadores].recibirCarta(baraja.repartir());
    }
    cout << "¡Listo, cada jugar tiene " << cartasPorJugador << " cartas." << endl;

    // Elegimos quién arranca primero de los jugadores.
    int jugadorActual = rand() % numJugadores;
    cout << "¡" << jugadores[jugadorActual].nombre << " arranca el juego" << endl;

    // Empezamos el juego
    int ronda = 1;

    while (true) {
        cout << "\n--- Ronda " << ronda << endl;

        // Miramos si todavía hay cartas para jugar
        bool hayCartas = false;
        for (int i = 0; i < numJugadores; i++) {
            if (!jugadores[i].mano.empty()) {
                hayCartas = true;
                break;
            }
        }

        if (!hayCartas) {
            cout << "¡Se acabaron todas las cartas. Fin del juego." << endl;
            break;
        }

        // Si una persona no tiene cartas, pues pasamos al siguiente
        while (jugadores[jugadorActual].mano.empty()) {
            cout << jugadores[jugadorActual].nombre << " ya no tiene cartas, ¡qué boleta!" << endl;
            jugadorActual = (jugadorActual + 1) % numJugadores;
        }

        cout << "Le toca a " << jugadores[jugadorActual].nombre << endl;

        // Mostramos las cartas
        cout << "Tus cartas son:" << endl;
        jugadores[jugadorActual].mostrarMano();

        // Aquí se escoge una carta
        int indiceCartaElegida;
        do {
            cout << "¿Cuál carta vas a tirar? (1-" << jugadores[jugadorActual].mano.size() << "): ";
            cin >> indiceCartaElegida;
            indiceCartaElegida--; // Le bajamos 1 porque los arreglos empiezan en 0
            
            if (indiceCartaElegida < 0 || indiceCartaElegida >= jugadores[jugadorActual].mano.size()) {
                cout << "¡Esa carta no existe! Escoge bien." << endl;
            }
        } while (indiceCartaElegida < 0 || indiceCartaElegida >= jugadores[jugadorActual].mano.size());

        // Jugamos la carta que se escogió
        Carta cartaLider = jugadores[jugadorActual].jugarCarta(indiceCartaElegida);
        int colorLider = cartaLider.color;

        cout << jugadores[jugadorActual].nombre << " tiró: ";
        cartaLider.mostrar();
        cout << " (Color " << nombreColor(colorLider) << endl;

        // Guardamos las cartas que se van tirando
        vector<Carta> cartasJugadas;
        vector<int> jugadoresTurno;

        cartasJugadas.push_back(cartaLider);
        jugadoresTurno.push_back(jugadorActual);

        // Ahora les toca a los otros jugadores responder
        for (int i = 1; i < numJugadores; i++) {
            int siguienteJugador = (jugadorActual + i) % numJugadores;

            // Si un jugador no tiene cartas, pues pasamos al siguiente
            if (jugadores[siguienteJugador].mano.empty()) {
                cout << jugadores[siguienteJugador].nombre << " ya no tiene cartas" << endl;
                continue;
            }

            cout << "\nAhora le toca a " << jugadores[siguienteJugador].nombre << " << endl;
            cout << "Hay que tirar del color: " << nombreColor(colorLider) << endl;
            
            // Mostramos las cartas que tiene el jugador
            jugadores[siguienteJugador].mostrarMano();
            
            // El jugador escoge una carta
            int seleccion;
            do {
                cout << "¿Cuál carta vas a tirar? (1-" << jugadores[siguienteJugador].mano.size() << "): ";
                cin >> seleccion;
                seleccion--; // Le bajamos 1 porque los arrays empiezan en 0.
                
                if (seleccion < 0 || seleccion >= jugadores[siguienteJugador].mano.size()) {
                    cout << "¡Esa carta no existe, escoge bien" << endl;
                }
            } while (seleccion < 0 || seleccion >= jugadores[siguienteJugador].mano.size());
            
            Carta cartaJugada = jugadores[siguienteJugador].jugarCarta(seleccion);
            
            cout << jugadores[siguienteJugador].nombre << " tiró: ";
            cartaJugada.mostrar();
            cout << " (Color " << nombreColor(cartaJugada.color) << ")" << endl;
            
            // Le avisamos al jugador si se equivocó con el color.
            if (cartaJugada.color != colorLider) {
                cout << "¡Uy, tiraste un color diferente! No puedes ganar esta ronda" << endl;
            } else if (cartaJugada.poder <= cartaLider.poder) {
                cout << "Tiraste el color correcto, pero el valor es muy bajo. No puedes ganar esta ronda" << endl;
            }
            
            cartasJugadas.push_back(cartaJugada);
            jugadoresTurno.push_back(siguienteJugador);
        }
        
        // Miramos quién ganó la ronda
        int ganadorRonda = jugadoresTurno[0]; // Por defecto, el jugador que arrancó la partida.
        int valorMasAlto = cartasJugadas[0].poder;
        
        for (int i = 1; i < cartasJugadas.size(); i++) {
            if (cartasJugadas[i].color == colorLider && cartasJugadas[i].poder > valorMasAlto) {
                valorMasAlto = cartasJugadas[i].poder;
                ganadorRonda = jugadoresTurno[i];
            }
        }
        
        cout << "\n¡" << jugadores[ganadorRonda].nombre << " ganó la ronda!" << endl;
        
        // Le damos las carticas al ganador
        jugadores[ganadorRonda].cartasGanadas += cartasJugadas.size();
        cout << jugadores[ganadorRonda].nombre << " ya tiene " << jugadores[ganadorRonda].cartasGanadas << " cartas ganadas en total" << endl;
        
        // La siguiente ronda la arranca el ganador
        jugadorActual = ganadorRonda;
        ronda++;
        
        cout << "\nEnter para que sigamos.";
        cin.ignore();
        cin.get();
        limpiarPantalla();
    }
    
    // Miramos quién ganó todo el juego
    int maxCartas = 0;
    vector<int> ganadores;
    
    // Buscamos quién tiene más cartas
    for (int i = 0; i < numJugadores; i++) {
        cout << jugadores[i].nombre << " ganó " << jugadores[i].cartasGanadas << " cartas. ";
        if (jugadores[i].cartasGanadas == 0) {
        } else if (jugadores[i].cartasGanadas < 5) {
            cout << "La próxima ronda" << endl;
        } else {
            cout << "¡Qué bien!" << endl;
        }
        
        if (jugadores[i].cartasGanadas > maxCartas) {
            maxCartas = jugadores[i].cartasGanadas;
            ganadores.clear();
            ganadores.push_back(i);
        } else if (jugadores[i].cartasGanadas == maxCartas) {
            ganadores.push_back(i);
        }
    }
    
    // Mostramos el resultado final
    if (ganadores.size() == 1) {
        cout << "\n¡" << jugadores[ganadores[0]].nombre << " ganó todo el juego con " << maxCartas << " cartas" << endl;
    } else {
        cout << "\n¡Empate, estos jugadores empataron con " << maxCartas << " cartas:" << endl;
        for (int i = 0; i < ganadores.size(); i++) {
            cout << "- " << jugadores[ganadores[i]].nombre << " (¡Qué bien!)" << endl;
        }
    }
    
    cout << "\n¡Gracias por jugar. Hasta la próxima" << endl;
    return 0;
}

