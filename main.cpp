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

int main() {
    // Arrancamos con los números aleatorios
    srand(time(NULL));

    // Armamos el mazo
    Mazo baraja;
    baraja.barajar();  // Le damos una revolvida a las cartas

    // Preguntamos cuántas personas van a jugar 
    int numJugadores;
    do {
        cout << "¿Cuantos jugadores van a jugar? (1-4): ";
        cin >> numJugadores;
        if (numJugadores < 1 || numJugadores > 4) {
            cout << "Tienen que ser entre 1 y 4 jugadores." << endl;
        }
    } while (numJugadores < 1 || numJugadores > 4);

    // Creamos los jugadores con este vector y los guardamos en el mismo 
    vector<Jugador> jugadores;
    for (int i = 0; i < numJugadores; i++) {
        string nombre = "Jugador " + to_string(i + 1);
        jugadores.push_back(Jugador(nombre));
        cout << "¡" << nombre << " está listo pa' la vuelta!" << endl;
    }

    // Repartimos todas las cartas
    int cartasPorJugador = baraja.n / numJugadores;
    for (int i = 0; i < cartasPorJugador * numJugadores; i++) {
        jugadores[i % numJugadores].recibirCarta(baraja.repartir());
    }
    cout << "Listo, cada jugador tiene " << cartasPorJugador << " cartas." << endl;

    // Elegimos quién arranca primero, al azar
    int jugadorActual = rand() % numJugadores;
    cout << jugadores[jugadorActual].nombre << " arranca la partida" << endl;

    // Empieza el juego
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
            cout << "¡Se acabaron las cartas, fin del juego." << endl;
            break;
        }

        // Si un jugador no tiene cartas, pues pasamos al siguiente
        while (jugadores[jugadorActual].mano.empty()) {
            cout << jugadores[jugadorActual].nombre << " ya no tiene cartas" << endl;
            jugadorActual = (jugadorActual + 1) % numJugadores;
        }

        cout << "Le toca a " << jugadores[jugadorActual].nombre << endl;

        // Mostramos las cartas que tiene 
        cout << "Tus carticas son, parce:" << endl;
        jugadores[jugadorActual].mostrarMano();

        // Se escoge una carta
        int indiceCartaElegida;
        do {
            cout << "¿Cuál carta vas a tirar? (1-" << jugadores[jugadorActual].mano.size() << "): ";
            cin >> indiceCartaElegida;
            indiceCartaElegida--; // Le bajamos 1 porque los arrays empiezan en 0
            
            if (indiceCartaElegida < 0 || indiceCartaElegida >= jugadores[jugadorActual].mano.size()) {
                cout << "¡Esa carta no existe, escoge bien." << endl;
            }
        } while (indiceCartaElegida < 0 || indiceCartaElegida >= jugadores[jugadorActual].mano.size());

        // Jugamos la carta que escogió 
        Carta cartaLider = jugadores[jugadorActual].jugarCarta(indiceCartaElegida);
        int colorLider = cartaLider.color;

        cout << jugadores[jugadorActual].nombre << " tiró: ";
        cartaLider.mostrar();
        cout << " (Color " << nombreColor(colorLider) << ", ¡qué chimba!)" << endl;

        // Guardamos las cartas que se van tirando
        vector<Carta> cartasJugadas;
        vector<int> jugadoresTurno;

        cartasJugadas.push_back(cartaLider);
        jugadoresTurno.push_back(jugadorActual);

        // Ahora les toca a los otros
        for (int i = 1; i < numJugadores; i++) {
            int siguienteJugador = (jugadorActual + i) % numJugadores;

            // Si una persona no tiene cartas, pues pasamos al siguiente
            if (jugadores[siguienteJugador].mano.empty()) {
                cout << jugadores[siguienteJugador].nombre << " ya no tiene cartas" << endl;
                continue;
            }

            cout << "\nAhora le toca a " << jugadores[siguienteJugador].nombre << endl;
            cout << "Hay que tirar del color: " << nombreColor(colorLider)  << endl;
            
            // Mostramos las cartas que tiene 
            jugadores[siguienteJugador].mostrarMano();
            
            // Selección de la carta
            int seleccion;
            do {
                cout << "¿Cuál carta vas a tirar? (1-" << jugadores[siguienteJugador].mano.size() << "): ";
                cin >> seleccion;
                seleccion--;
                
                if (seleccion < 0 || seleccion >= jugadores[siguienteJugador].mano.size()) {
                    cout << "¡Esa carta no existe, parce! Escoge bien." << endl;
                }
            } while (seleccion < 0 || seleccion >= jugadores[siguienteJugador].mano.size());
            
            Carta cartaJugada = jugadores[siguienteJugador].jugarCarta(seleccion);
            
            cout << jugadores[siguienteJugador].nombre << " tiró: ";
            cartaJugada.mostrar();
            cout << " (Color " << nombreColor(cartaJugada.color) << ")" << endl;
            
            // Validamos si el color es el mismo 
            if (cartaJugada.color != colorLider) {
                cout << "Tiraste un color diferente, no puedes ganar esta ronda" << endl;
            } else if (cartaJugada.poder <= cartaLider.poder) {
                cout << "Tiraste el color correcto pero el valor es muy bajito. No puedes ganar esta ronda" << endl;
            }
            
            cartasJugadas.push_back(cartaJugada);
            jugadoresTurno.push_back(siguienteJugador);
        }
        
        // Miramos quién ganó la ronda,
        int ganadorRonda = jugadoresTurno[0]; // Por defecto colocamos el jugador que arrancó 
        int valorMasAlto = cartasJugadas[0].poder;
        
        for (int i = 1; i < cartasJugadas.size(); i++) {
            if (cartasJugadas[i].color == colorLider && cartasJugadas[i].poder > valorMasAlto) {
                valorMasAlto = cartasJugadas[i].poder;
                ganadorRonda = jugadoresTurno[i];
            }
        }
        
        cout << "\n¡" << jugadores[ganadorRonda].nombre << " ganó la ronda" << endl;
        
        // Le guardamos las cartas al jugador que ganó
        jugadores[ganadorRonda].cartasGanadas += cartasJugadas.size();
        cout << jugadores[ganadorRonda].nombre << " ya tiene " << jugadores[ganadorRonda].cartasGanadas << " cartas ganadas en total" << endl;
        
        // La siguiente ronda la arranca el ganador
        jugadorActual = ganadorRonda;
        ronda++;
        
        cout << "\nEnter para continuar";
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
            cout << "No ganó ninguna ronda" << endl;
        } else if (jugadores[i].cartasGanadas < 5) {
            cout << "Algo es algo" << endl;
        } else {
            cout << "Bastantes cartas" << endl;
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
        cout << "\n¡" << jugadores[ganadores[0]].nombre << " ganó todo el juego con " << maxCartas << " cartas!" << endl;
    } else {
        cout << "\nEstos jugadores empataron con " << maxCartas << " cartas:" << endl;
        for (int i = 0; i < ganadores.size(); i++) {
            cout << "- " << jugadores[ganadores[i]].nombre << " (Muy bien)" << endl;
        }
    }
    
    cout << "\nGracias por jugar" << endl;
    return 0;
}