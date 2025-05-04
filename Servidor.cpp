#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include "Carta.h"
#include "mazo.h"
#include "jugador.h"
#include "Reglas.h"

// Incluimos las bibliotecas para sockets
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

using namespace std;

// Función para enviar un mensaje a un cliente
void enviarMensaje(SOCKET clienteSocket, const string& mensaje) {
    send(clienteSocket, mensaje.c_str(), mensaje.length(), 0);
}

// Función para recibir un mensaje de un cliente
string recibirMensaje(SOCKET clienteSocket) {
    char buffer[1024];
    int bytesRecibidos = recv(clienteSocket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesRecibidos <= 0) {
        return "";
    }
    
    buffer[bytesRecibidos] = '\0';
    return string(buffer);
}

// Función para enviar una carta a un cliente
void enviarCarta(SOCKET clienteSocket, const Carta& carta) {
    string mensaje = "CARTA|" + to_string(carta.color) + "|" + to_string(carta.poder);
    enviarMensaje(clienteSocket, mensaje);
}

// Función para enviar varias cartas a un cliente
void enviarCartas(SOCKET clienteSocket, const vector<Carta>& cartas) {
    string mensaje = "CARTAS|" + to_string(cartas.size());
    
    for (const auto& carta : cartas) {
        mensaje += "|" + to_string(carta.color) + "|" + to_string(carta.poder);
    }
    
    enviarMensaje(clienteSocket, mensaje);
}

int main() {
    // Inicializamos los números aleatorios
    srand(time(NULL));
    
    // Inicializamos Winsock en Windows
    #ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            cerr << "Error al inicializar Winsock" << endl;
            return 1;
        }
    #endif
    
    // Creamos el socket del servidor
    SOCKET servidorSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (servidorSocket == INVALID_SOCKET) {
        cerr << "Error al crear el socket" << endl;
        #ifdef _WIN32
            WSACleanup();
        #endif
        return 1;
    }
    
    // Configuramos la dirección del servidor
    sockaddr_in direccionServidor;
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = INADDR_ANY;
    direccionServidor.sin_port = htons(8888); // Puerto 8888
    
    // Vinculamos el socket
    if (bind(servidorSocket, (sockaddr*)&direccionServidor, sizeof(direccionServidor)) == SOCKET_ERROR) {
        cerr << "Error al vincular el socket" << endl;
        closesocket(servidorSocket);
        #ifdef _WIN32
            WSACleanup();
        #endif
        return 1;
    }
    
    // Escuchamos conexiones entrantes
    if (listen(servidorSocket, 2) == SOCKET_ERROR) {
        cerr << "Error al escuchar" << endl;
        closesocket(servidorSocket);
        #ifdef _WIN32
            WSACleanup();
        #endif
        return 1;
    }
    
    cout << "Servidor iniciado. Esperando conexiones..." << endl;
    
    // Aceptamos la conexión del primer jugador
    sockaddr_in direccionCliente1;
    socklen_t tamDireccionCliente1 = sizeof(direccionCliente1);
    SOCKET clienteSocket1 = accept(servidorSocket, (sockaddr*)&direccionCliente1, &tamDireccionCliente1);
    
    if (clienteSocket1 == INVALID_SOCKET) {
        cerr << "Error al aceptar la conexión del primer jugador" << endl;
        closesocket(servidorSocket);
        #ifdef _WIN32
            WSACleanup();
        #endif
        return 1;
    }
    
    cout << "Primer jugador conectado." << endl;
    enviarMensaje(clienteSocket1, "BIENVENIDO|0"); // Le enviamos su ID (0)
    
    // Recibimos el nombre del primer jugador
    string mensajeNombre1 = recibirMensaje(clienteSocket1);
    string nombreJugador1 = mensajeNombre1.substr(mensajeNombre1.find("|") + 1);
    cout << "Primer jugador: " << nombreJugador1 << endl;
    
    // Aceptamos la conexión del segundo jugador
    sockaddr_in direccionCliente2;
    socklen_t tamDireccionCliente2 = sizeof(direccionCliente2);
    SOCKET clienteSocket2 = accept(servidorSocket, (sockaddr*)&direccionCliente2, &tamDireccionCliente2);
    
    if (clienteSocket2 == INVALID_SOCKET) {
        cerr << "Error al aceptar la conexión del segundo jugador" << endl;
        closesocket(clienteSocket1);
        closesocket(servidorSocket);
        #ifdef _WIN32
            WSACleanup();
        #endif
        return 1;
    }
    
    cout << "Segundo jugador conectado." << endl;
    enviarMensaje(clienteSocket2, "BIENVENIDO|1"); // Le enviamos su ID (1)
    
    // Recibimos el nombre del segundo jugador
    string mensajeNombre2 = recibirMensaje(clienteSocket2);
    string nombreJugador2 = mensajeNombre2.substr(mensajeNombre2.find("|") + 1);
    cout << "Segundo jugador: " << nombreJugador2 << endl;
    
    // Creamos los jugadores
    Jugador jugador1(nombreJugador1);
    Jugador jugador2(nombreJugador2);
    
    // Creamos y barajamos el mazo
    Mazo baraja;
    baraja.barajar();
    
    // Repartimos las cartas
    int cartasPorJugador = baraja.n / 2;
    
    for (int i = 0; i < cartasPorJugador; i++) {
        jugador1.recibirCarta(baraja.repartir());
        jugador2.recibirCarta(baraja.repartir());
    }
    
    cout << "Cartas repartidas. Cada jugador tiene " << cartasPorJugador << " cartas." << endl;
    
    // Enviamos las cartas a los jugadores
    enviarCartas(clienteSocket1, jugador1.mano);
    enviarCartas(clienteSocket2, jugador2.mano);
    
    // Elegimos quién arranca primero, al azar
    int jugadorActual = rand() % 2;
    cout << "Comienza el jugador " << (jugadorActual == 0 ? nombreJugador1 : nombreJugador2) << endl;
    
    // Enviamos quién comienza
    enviarMensaje(clienteSocket1, "TURNO|" + to_string(jugadorActual));
    enviarMensaje(clienteSocket2, "TURNO|" + to_string(jugadorActual));
    
    // Empieza el juego
    int ronda = 1;
    
    while (true) {
        cout << "\n--- Ronda " << ronda << " ---" << endl;
        
        // Verificamos si hay cartas para jugar
        if (jugador1.mano.empty() && jugador2.mano.empty()) {
            cout << "Se acabaron las cartas. Fin del juego." << endl;
            break;
        }
        
        // Si el jugador actual no tiene cartas, cambiamos al otro
        if ((jugadorActual == 0 && jugador1.mano.empty()) || 
            (jugadorActual == 1 && jugador2.mano.empty())) {
            jugadorActual = 1 - jugadorActual;
            cout << "El jugador actual no tiene cartas. Cambiamos al otro." << endl;
            enviarMensaje(clienteSocket1, "TURNO|" + to_string(jugadorActual));
            enviarMensaje(clienteSocket2, "TURNO|" + to_string(jugadorActual));
        }
        
        // Esperamos a que el jugador actual juegue una carta
        cout << "Esperando a que " << (jugadorActual == 0 ? nombreJugador1 : nombreJugador2) << " juegue una carta..." << endl;
        
        string mensajeCarta;
        int indiceCartaElegida;
        
        if (jugadorActual == 0) {
            mensajeCarta = recibirMensaje(clienteSocket1);
            indiceCartaElegida = stoi(mensajeCarta.substr(mensajeCarta.find("|") + 1));
            
            // Jugamos la carta
            Carta cartaJugada = jugador1.jugarCarta(indiceCartaElegida);
            cout << nombreJugador1 << " jugó: ";
            cartaJugada.mostrar();
            cout << " (Color " << nombreColor(cartaJugada.color) << ")" << endl;
            
            // Enviamos la carta jugada a ambos jugadores
            enviarMensaje(clienteSocket1, "JUGADA|0|" + to_string(cartaJugada.color) + "|" + to_string(cartaJugada.poder));
            enviarMensaje(clienteSocket2, "JUGADA|0|" + to_string(cartaJugada.color) + "|" + to_string(cartaJugada.poder));
            
            // Ahora le toca al otro jugador
            jugadorActual = 1;
            enviarMensaje(clienteSocket1, "TURNO|" + to_string(jugadorActual));
            enviarMensaje(clienteSocket2, "TURNO|" + to_string(jugadorActual));
            
            // Esperamos a que el segundo jugador juegue
            cout << "Esperando a que " << nombreJugador2 << " juegue una carta..." << endl;
            mensajeCarta = recibirMensaje(clienteSocket2);
            indiceCartaElegida = stoi(mensajeCarta.substr(mensajeCarta.find("|") + 1));
            
            // Jugamos la carta
            Carta cartaJugada2 = jugador2.jugarCarta(indiceCartaElegida);
            cout << nombreJugador2 << " jugó: ";
            cartaJugada2.mostrar();
            cout << " (Color " << nombreColor(cartaJugada2.color) << ")" << endl;
            
            // Enviamos la carta jugada a ambos jugadores
            enviarMensaje(clienteSocket1, "JUGADA|1|" + to_string(cartaJugada2.color) + "|" + to_string(cartaJugada2.poder));
            enviarMensaje(clienteSocket2, "JUGADA|1|" + to_string(cartaJugada2.color) + "|" + to_string(cartaJugada2.poder));
            
            // Determinamos el ganador de la ronda
            int colorLider = cartaJugada.color;
            int ganadorRonda;
            
            if (cartaJugada2.color != colorLider) {
                // El segundo jugador no siguió el color, gana el primero
                ganadorRonda = 0;
                cout << "El segundo jugador no siguió el color líder. Gana el primer jugador." << endl;
            } else if (cartaJugada2.poder > cartaJugada.poder) {
                // El segundo jugador tiene mayor poder, gana
                ganadorRonda = 1;
                cout << "El segundo jugador tiene mayor poder. Gana el segundo jugador." << endl;
            } else {
                // El primer jugador tiene mayor o igual poder, gana
                ganadorRonda = 0;
                cout << "El primer jugador tiene mayor o igual poder. Gana el primer jugador." << endl;
            }
            
            // Actualizamos las cartas ganadas
            if (ganadorRonda == 0) {
                jugador1.cartasGanadas += 2;
            } else {
                jugador2.cartasGanadas += 2;
            }
            
            // Enviamos el resultado de la ronda
            enviarMensaje(clienteSocket1, "RESULTADO|" + to_string(ganadorRonda) + "|" + to_string(jugador1.cartasGanadas) + "|" + to_string(jugador2.cartasGanadas));
            enviarMensaje(clienteSocket2, "RESULTADO|" + to_string(ganadorRonda) + "|" + to_string(jugador1.cartasGanadas) + "|" + to_string(jugador2.cartasGanadas));
            
            // El ganador comienza la siguiente ronda
            jugadorActual = ganadorRonda;
            
        } else {
            mensajeCarta = recibirMensaje(clienteSocket2);
            indiceCartaElegida = stoi(mensajeCarta.substr(mensajeCarta.find("|") + 1));
            
            // Jugamos la carta
            Carta cartaJugada = jugador2.jugarCarta(indiceCartaElegida);
            cout << nombreJugador2 << " jugó: ";
            cartaJugada.mostrar();
            cout << " (Color " << nombreColor(cartaJugada.color) << ")" << endl;
            
            // Enviamos la carta jugada a ambos jugadores
            enviarMensaje(clienteSocket1, "JUGADA|1|" + to_string(cartaJugada.color) + "|" + to_string(cartaJugada.poder));
            enviarMensaje(clienteSocket2, "JUGADA|1|" + to_string(cartaJugada.color) + "|" + to_string(cartaJugada.poder));
            
            // Ahora le toca al otro jugador
            jugadorActual = 0;
            enviarMensaje(clienteSocket1, "TURNO|" + to_string(jugadorActual));
            enviarMensaje(clienteSocket2, "TURNO|" + to_string(jugadorActual));
            
            // Esperamos a que el primer jugador juegue
            cout << "Esperando a que " << nombreJugador1 << " juegue una carta..." << endl;
            mensajeCarta = recibirMensaje(clienteSocket1);
            indiceCartaElegida = stoi(mensajeCarta.substr(mensajeCarta.find("|") + 1));
            
            // Jugamos la carta
            Carta cartaJugada2 = jugador1.jugarCarta(indiceCartaElegida);
            cout << nombreJugador1 << " jugó: ";
            cartaJugada2.mostrar();
            cout << " (Color " << nombreColor(cartaJugada2.color) << ")" << endl;
            
            // Enviamos la carta jugada a ambos jugadores
            enviarMensaje(clienteSocket1, "JUGADA|0|" + to_string(cartaJugada2.color) + "|" + to_string(cartaJugada2.poder));
            enviarMensaje(clienteSocket2, "JUGADA|0|" + to_string(cartaJugada2.color) + "|" + to_string(cartaJugada2.poder));
            
            // Determinamos el ganador de la ronda
            int colorLider = cartaJugada.color;
            int ganadorRonda;
            
            if (cartaJugada2.color != colorLider) {
                // El primer jugador no siguió el color, gana el segundo
                ganadorRonda = 1;
                cout << "El primer jugador no siguió el color líder. Gana el segundo jugador." << endl;
            } else if (cartaJugada2.poder > cartaJugada.poder) {
                // El primer jugador tiene mayor poder, gana
                ganadorRonda = 0;
                cout << "El primer jugador tiene mayor poder. Gana el primer jugador." << endl;
            } else {
                // El segundo jugador tiene mayor o igual poder, gana
                ganadorRonda = 1;
                cout << "El segundo jugador tiene mayor o igual poder. Gana el segundo jugador." << endl;
            }
            
            // Actualizamos las cartas ganadas
            if (ganadorRonda == 0) {
                jugador1.cartasGanadas += 2;
            } else {
                jugador2.cartasGanadas += 2;
            }
            
            // Enviamos el resultado de la ronda
            enviarMensaje(clienteSocket1, "RESULTADO|" + to_string(ganadorRonda) + "|" + to_string(jugador1.cartasGanadas) + "|" + to_string(jugador2.cartasGanadas));
            enviarMensaje(clienteSocket2, "RESULTADO|" + to_string(ganadorRonda) + "|" + to_string(jugador1.cartasGanadas) + "|" + to_string(jugador2.cartasGanadas));
            
            // El ganador comienza la siguiente ronda
            jugadorActual = ganadorRonda;
        }
        
        ronda++;
        
        // Verificamos si el juego ha terminado
        if (jugador1.mano.empty() && jugador2.mano.empty()) {
            cout << "Se acabaron las cartas. Fin del juego." << endl;
            
            // Determinamos el ganador del juego
            string ganadorJuego;
            if (jugador1.cartasGanadas > jugador2.cartasGanadas) {
                ganadorJuego = nombreJugador1;
                cout << nombreJugador1 << " gana el juego con " << jugador1.cartasGanadas << " cartas." << endl;
            } else if (jugador2.cartasGanadas > jugador1.cartasGanadas) {
                ganadorJuego = nombreJugador2;
                cout << nombreJugador2 << " gana el juego con " << jugador2.cartasGanadas << " cartas." << endl;
            } else {
                ganadorJuego = "Empate";
                cout << "¡Empate! Ambos jugadores tienen " << jugador1.cartasGanadas << " cartas." << endl;
            }
            
            // Enviamos el resultado final
            enviarMensaje(clienteSocket1, "FIN|" + ganadorJuego + "|" + to_string(jugador1.cartasGanadas) + "|" + to_string(jugador2.cartasGanadas));
            enviarMensaje(clienteSocket2, "FIN|" + ganadorJuego + "|" + to_string(jugador1.cartasGanadas) + "|" + to_string(jugador2.cartasGanadas));
            
            break;
        }
    }
    
    // Cerramos los sockets
    closesocket(clienteSocket1);
    closesocket(clienteSocket2);
    closesocket(servidorSocket);
    
    // Limpiamos Winsock en Windows
    #ifdef _WIN32
        WSACleanup();
    #endif
    
    cout << "Servidor cerrado." << endl;
    return 0;
}
