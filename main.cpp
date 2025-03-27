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
    // Arrancamos la vuelta de los números aleatorios, ¡qué chimba!
    srand(time(NULL));

    // Armamos el mazo, parce, pa' jugar con las carticas
    Mazo baraja;
    baraja.barajar();  // Le damos una revolvida a las cartas, ¿si o qué?

    // Preguntamos cuántos parceros van a jugar esta vuelta
    int numJugadores;
    do {
        cout << "¿Cuántos parceros van a jugar, pues? (1-4): ";
        cin >> numJugadores;
        if (numJugadores < 1 || numJugadores > 4) {
            cout << "¡Ey, gonorrea! Tienen que ser entre 1 y 4 jugadores." << endl;
        }
    } while (numJugadores < 1 || numJugadores > 4);

    // Creamos los jugadores con este vector, ¡qué nota!
    vector<Jugador> jugadores;
    for (int i = 0; i < numJugadores; i++) {
        string nombre = "Parcero " + to_string(i + 1);
        jugadores.push_back(Jugador(nombre));
        cout << "¡" << nombre << " está listo pa' la vuelta!" << endl;
    }

    // Repartimos todas las carticas, a lo bien
    int cartasPorJugador = baraja.n / numJugadores;
    for (int i = 0; i < cartasPorJugador * numJugadores; i++) {
        jugadores[i % numJugadores].recibirCarta(baraja.repartir());
    }
    cout << "¡Listo parce! Cada uno tiene " << cartasPorJugador << " cartas." << endl;

    // Elegimos quién arranca primero, al azar, ¡qué suerte la del perro!
    int jugadorActual = rand() % numJugadores;
    cout << "¡" << jugadores[jugadorActual].nombre << " arranca el jueguito, pues!" << endl;

    // Empieza el juego, ¡qué nervios, parce!
    int ronda = 1;

    while (true) {
        cout << "\n--- Ronda " << ronda << " (¡Qué adrenalina, parce!) ---" << endl;

        // Miramos si todavía hay cartas pa' jugar, ¿si me entiende?
        bool hayCartas = false;
        for (int i = 0; i < numJugadores; i++) {
            if (!jugadores[i].mano.empty()) {
                hayCartas = true;
                break;
            }
        }

        if (!hayCartas) {
            cout << "¡Se acabaron las carticas, parce! Fin del juego." << endl;
            break;
        }

        // Si un parcero no tiene cartas, pues pasamos al siguiente, ¿si pilla?
        while (jugadores[jugadorActual].mano.empty()) {
            cout << jugadores[jugadorActual].nombre << " ya no tiene cartas, ¡qué boleta!" << endl;
            jugadorActual = (jugadorActual + 1) % numJugadores;
        }

        cout << "Le toca a " << jugadores[jugadorActual].nombre << ", ¡pilas pues!" << endl;

        // Mostramos las cartas que tiene el parcero
        cout << "Tus carticas son, parce:" << endl;
        jugadores[jugadorActual].mostrarMano();

        // El parcero escoge una carta, ¡ojalá la buena!
        int indiceCartaElegida;
        do {
            cout << "¿Cuál carta vas a tirar, pues? (1-" << jugadores[jugadorActual].mano.size() << "): ";
            cin >> indiceCartaElegida;
            indiceCartaElegida--; // Le bajamos 1 porque los arrays empiezan en 0, ¿si sabe?
            
            if (indiceCartaElegida < 0 || indiceCartaElegida >= jugadores[jugadorActual].mano.size()) {
                cout << "¡Esa carta no existe, parce! Escoge bien." << endl;
            }
        } while (indiceCartaElegida < 0 || indiceCartaElegida >= jugadores[jugadorActual].mano.size());

        // Jugamos la carta que escogió el parcero
        Carta cartaLider = jugadores[jugadorActual].jugarCarta(indiceCartaElegida);
        int colorLider = cartaLider.color;

        cout << jugadores[jugadorActual].nombre << " tiró: ";
        cartaLider.mostrar();
        cout << " (Color " << nombreColor(colorLider) << ", ¡qué chimba!)" << endl;

        // Guardamos las cartas que se van tirando, ¿si me entiende?
        vector<Carta> cartasJugadas;
        vector<int> jugadoresTurno;

        cartasJugadas.push_back(cartaLider);
        jugadoresTurno.push_back(jugadorActual);

        // Ahora les toca a los otros parceros responder
        for (int i = 1; i < numJugadores; i++) {
            int siguienteJugador = (jugadorActual + i) % numJugadores;

            // Si un parcero no tiene cartas, pues pasamos al siguiente, ¿si pilla?
            if (jugadores[siguienteJugador].mano.empty()) {
                cout << jugadores[siguienteJugador].nombre << " ya no tiene cartas, ¡qué boleta!" << endl;
                continue;
            }

            cout << "\nAhora le toca a " << jugadores[siguienteJugador].nombre << ", ¡pilas pues!" << endl;
            cout << "Hay que tirar del color: " << nombreColor(colorLider) << ", ¿si o qué?" << endl;
            
            // Mostramos las cartas que tiene el parcero
            jugadores[siguienteJugador].mostrarMano();
            
            // El parcero escoge una carta, ¡ojalá la buena!
            int seleccion;
            do {
                cout << "¿Cuál carta vas a tirar, pues? (1-" << jugadores[siguienteJugador].mano.size() << "): ";
                cin >> seleccion;
                seleccion--; // Le bajamos 1 porque los arrays empiezan en 0, ¿si sabe?
                
                if (seleccion < 0 || seleccion >= jugadores[siguienteJugador].mano.size()) {
                    cout << "¡Esa carta no existe, parce! Escoge bien." << endl;
                }
            } while (seleccion < 0 || seleccion >= jugadores[siguienteJugador].mano.size());
            
            Carta cartaJugada = jugadores[siguienteJugador].jugarCarta(seleccion);
            
            cout << jugadores[siguienteJugador].nombre << " tiró: ";
            cartaJugada.mostrar();
            cout << " (Color " << nombreColor(cartaJugada.color) << ")" << endl;
            
            // Le avisamos al parcero si la embarró con el color
            if (cartaJugada.color != colorLider) {
                cout << "¡Uy parce, tiraste un color diferente! No podés ganar esta ronda, ¡qué boleta!" << endl;
            } else if (cartaJugada.poder <= cartaLider.poder) {
                cout << "Tiraste el color correcto, pero el valor es muy bajito. No podés ganar esta ronda, ¡qué mala suerte!" << endl;
            }
            
            cartasJugadas.push_back(cartaJugada);
            jugadoresTurno.push_back(siguienteJugador);
        }
        
        // Miramos quién ganó la ronda, ¡qué emoción!
        int ganadorRonda = jugadoresTurno[0]; // Por defecto, el parcero que arrancó
        int valorMasAlto = cartasJugadas[0].poder;
        
        for (int i = 1; i < cartasJugadas.size(); i++) {
            if (cartasJugadas[i].color == colorLider && cartasJugadas[i].poder > valorMasAlto) {
                valorMasAlto = cartasJugadas[i].poder;
                ganadorRonda = jugadoresTurno[i];
            }
        }
        
        cout << "\n¡" << jugadores[ganadorRonda].nombre << " ganó la ronda! ¡Qué chimba, parce!" << endl;
        
        // Le damos las carticas al ganador, ¡se las pilló todas!
        jugadores[ganadorRonda].cartasGanadas += cartasJugadas.size();
        cout << jugadores[ganadorRonda].nombre << " ya tiene " << jugadores[ganadorRonda].cartasGanadas << " cartas ganadas en total. ¡Qué crack!" << endl;
        
        // La siguiente ronda la arranca el ganador, ¡bien merecido!
        jugadorActual = ganadorRonda;
        ronda++;
        
        cout << "\nApretá Enter pa' seguir, parce...";
        cin.ignore();
        cin.get();
        limpiarPantalla();
    }
    
    // Miramos quién ganó todo el juego, ¡momento de la verdad!
    int maxCartas = 0;
    vector<int> ganadores;
    
    // Buscamos quién tiene más carticas, ¿si pilla?
    for (int i = 0; i < numJugadores; i++) {
        cout << jugadores[i].nombre << " ganó " << jugadores[i].cartasGanadas << " cartas. ";
        if (jugadores[i].cartasGanadas == 0) {
            cout << "¡Qué boleta, parce!" << endl;
        } else if (jugadores[i].cartasGanadas < 5) {
            cout << "Algo es algo, ¿cierto?" << endl;
        } else {
            cout << "¡Qué chimba!" << endl;
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
        cout << "\n¡Empate, gonorrea! Estos parceros empataron con " << maxCartas << " cartas:" << endl;
        for (int i = 0; i < ganadores.size(); i++) {
            cout << "- " << jugadores[ganadores[i]].nombre << " (¡Qué crack!)" << endl;
        }
    }
    
    cout << "\n¡Gracias por jugar, parceros! ¡Hasta la próxima!" << endl;
    return 0;
}

