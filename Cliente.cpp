#include <iostream>
#include <string>
#include <vector>
#include "Carta.h"
#include "Reglas.h"

// Incluimos las bibliotecas para sockets
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
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

// Variables globales
vector<Carta> miMano;
int miId = -1;
string miNombre;
string nombreOponente;
int misCartasGanadas = 0;
int cartasGanadasOponente = 0;
bool miTurno = false;
bool juegoTerminado = false;

Carta cartaJugada[2];
bool hayCartaJugada[2] = {false, false};

// Función para enviar un mensaje al servidor
void enviarMensajeCliente(SOCKET clienteSocket, const string& mensaje) {
    send(clienteSocket, mensaje.c_str(), mensaje.length(), 0);
}

// Función para recibir un mensaje del servidor
string recibirMensajeCliente(SOCKET clienteSocket) {
    char buffer[1024];
    int bytesRecibidos = recv(clienteSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesRecibidos <= 0) {
        return "";
    }

    buffer[bytesRecibidos] = '\0';
    return string(buffer);
}

void mostrarEstado() {
    limpiarPantalla();

    cout << "=== JUEGO DE CARTAS EN RED ===" << endl;
    cout << "Tu nombre: " << miNombre << " (Jugador " << miId + 1 << ")" << endl;
    cout << "Oponente: " << nombreOponente << " (Jugador " << (1 - miId) + 1 << ")" << endl;
    cout << "Cartas ganadas: " << misCartasGanadas << " vs " << cartasGanadasOponente << endl;
    cout << endl;

    cout << "=== ZONA DE BATALLA ===" << endl;
    for (int i = 0; i < 2; i++) {
        if (hayCartaJugada[i]) {
            cout << "Jugador " << i + 1 << ": ";
            cartaJugada[i].mostrar();
            cout << " (Color " << nombreColor(cartaJugada[i].color) << ")" << endl;
        } else {
            cout << "Jugador " << i + 1 << ": No ha jugado carta" << endl;
        }
    }

    cout << endl << "=== TUS CARTAS ===" << endl;
    for (int i = 0; i < miMano.size(); i++) {
        cout << (i + 1) << ". ";
        miMano[i].mostrar();
        cout << " (Color " << nombreColor(miMano[i].color) << ")" << endl;
    }

    cout << endl;

    if (juegoTerminado) {
        cout << "=== JUEGO TERMINADO ===" << endl;
        if (misCartasGanadas > cartasGanadasOponente) {
            cout << "¡Has ganado el juego con " << misCartasGanadas << " cartas!" << endl;
        } else if (cartasGanadasOponente > misCartasGanadas) {
            cout << "Has perdido. " << nombreOponente << " ganó con " << cartasGanadasOponente << " cartas." << endl;
        } else {
            cout << "¡Empate! Ambos tienen " << misCartasGanadas << " cartas." << endl;
        }
    } else if (miTurno) {
        cout << "=== ES TU TURNO ===" << endl;
        cout << "Elige una carta para jugar (1-" << miMano.size() << "): ";
    } else {
        cout << "=== TURNO DEL OPONENTE ===" << endl;
        cout << "Esperando a que " << nombreOponente << " juegue su carta..." << endl;
    }
}

void procesarMensaje(const string& mensaje, SOCKET clienteSocket) {
    vector<string> partes;
    size_t pos = 0;
    string token;
    string mensajeCopia = mensaje;

    while ((pos = mensajeCopia.find('|')) != string::npos) {
        token = mensajeCopia.substr(0, pos);
        partes.push_back(token);
        mensajeCopia.erase(0, pos + 1);
    }
    if (!mensajeCopia.empty()) partes.push_back(mensajeCopia);
    if (partes.empty()) return;

    if (partes[0] == "BIENVENIDO") {
        miId = stoi(partes[1]);
        cout << "Te has conectado como Jugador " << miId + 1 << endl;
    } else if (partes[0] == "CARTAS") {
        int numCartas = stoi(partes[1]);
        miMano.clear();
        for (int i = 0; i < numCartas; i++) {
            int color = stoi(partes[2 + i * 2]);
            int poder = stoi(partes[3 + i * 2]);
            miMano.push_back(Carta(color, poder));
        }
        cout << "Has recibido " << numCartas << " cartas." << endl;
        mostrarEstado();
    } else if (partes[0] == "TURNO") {
        int jugadorActual = stoi(partes[1]);
        miTurno = (jugadorActual == miId);
        hayCartaJugada[0] = false;
        hayCartaJugada[1] = false;
        mostrarEstado();
    } else if (partes[0] == "JUGADA") {
        int idJugador = stoi(partes[1]);
        int colorCarta = stoi(partes[2]);
        int poderCarta = stoi(partes[3]);
        cartaJugada[idJugador] = Carta(colorCarta, poderCarta);
        hayCartaJugada[idJugador] = true;
        mostrarEstado();
    } else if (partes[0] == "RESULTADO") {
        int ganadorRonda = stoi(partes[1]);
        int cartasGanadas1 = stoi(partes[2]);
        int cartasGanadas2 = stoi(partes[3]);

        if (miId == 0) {
            misCartasGanadas = cartasGanadas1;
            cartasGanadasOponente = cartasGanadas2;
        } else {
            misCartasGanadas = cartasGanadas2;
            cartasGanadasOponente = cartasGanadas1;
        }

        cout << "Ronda terminada. ";
        if (ganadorRonda == miId) {
            cout << "¡Has ganado esta ronda!" << endl;
        } else {
            cout << nombreOponente << " ha ganado esta ronda." << endl;
        }

        cout << "Presiona Enter para continuar...";
        cin.get();
        mostrarEstado();
    } else if (partes[0] == "FIN") {
        string ganadorJuego = partes[1];
        int cartasGanadas1 = stoi(partes[2]);
        int cartasGanadas2 = stoi(partes[3]);

        if (miId == 0) {
            misCartasGanadas = cartasGanadas1;
            cartasGanadasOponente = cartasGanadas2;
        } else {
            misCartasGanadas = cartasGanadas2;
            cartasGanadasOponente = cartasGanadas1;
        }

        juegoTerminado = true;
        mostrarEstado();

        cout << "\nJuego terminado. ";
        if (ganadorJuego == "Empate") {
            cout << "¡Ha sido un empate!" << endl;
        } else if (ganadorJuego == miNombre) {
            cout << "¡Has ganado el juego!" << endl;
        } else {
            cout << ganadorJuego << " ha ganado el juego." << endl;
        }
    }
}

int main() {
    #ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            cerr << "Error al inicializar Winsock" << endl;
            return 1;
        }
    #endif

    SOCKET clienteSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clienteSocket == INVALID_SOCKET) {
        cerr << "Error al crear el socket" << endl;
        #ifdef _WIN32
            WSACleanup();
        #endif
        return 1;
    }

    string direccionIP;
    cout << "Ingresa la dirección IP del servidor (o 'localhost'): ";
    getline(cin, direccionIP);

    sockaddr_in direccionServidor;
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_port = htons(8888);

    if (direccionIP == "localhost") {
        direccionIP = "127.0.0.1";
    }

    direccionServidor.sin_addr.s_addr = inet_addr(direccionIP.c_str());
    if (direccionServidor.sin_addr.s_addr == INADDR_NONE) {
        cerr << "Dirección IP inválida" << endl;
        closesocket(clienteSocket);
        #ifdef _WIN32
            WSACleanup();
        #endif
        return 1;
    }

    cout << "Conectando al servidor..." << endl;
    if (connect(clienteSocket, (sockaddr*)&direccionServidor, sizeof(direccionServidor)) == SOCKET_ERROR) {
        cerr << "Error al conectar al servidor" << endl;
        closesocket(clienteSocket);
        #ifdef _WIN32
            WSACleanup();
        #endif
        return 1;
    }

    cout << "Conectado al servidor." << endl;

    string mensajeBienvenida = recibirMensajeCliente(clienteSocket);
    procesarMensaje(mensajeBienvenida, clienteSocket);

    // ✅ Enviar nombre justo después de conectarse
    cout << "Ingresa tu nombre: ";
    getline(cin, miNombre);
    enviarMensajeCliente(clienteSocket, "NOMBRE|" + miNombre);

    string mensajeCartas = recibirMensajeCliente(clienteSocket);
    procesarMensaje(mensajeCartas, clienteSocket);

    string mensajeTurno = recibirMensajeCliente(clienteSocket);
    procesarMensaje(mensajeTurno, clienteSocket);

    // Opcional: solo para mostrarlo localmente
    cout << "Ingresa el nombre de tu oponente (solo para mostrar): ";
    getline(cin, nombreOponente);

    while (!juegoTerminado) {
        if (miTurno) {
            int indiceCartaElegida;
            cin >> indiceCartaElegida;
            cin.ignore();

            indiceCartaElegida--;
            if (indiceCartaElegida >= 0 && indiceCartaElegida < miMano.size()) {
                enviarMensajeCliente(clienteSocket, "CARTA|" + to_string(indiceCartaElegida));
                miMano.erase(miMano.begin() + indiceCartaElegida);
                miTurno = false;
            } else {
                cout << "Carta inválida. Elige una carta entre 1 y " << miMano.size() << ": ";
            }
        } else {
            string mensaje = recibirMensajeCliente(clienteSocket);
            if (mensaje.empty()) {
                cout << "Desconectado del servidor." << endl;
                break;
            }
            procesarMensaje(mensaje, clienteSocket);
        }
    }

    cout << "\nPresiona Enter para salir...";
    cin.get();

    closesocket(clienteSocket);
    #ifdef _WIN32
        WSACleanup();
    #endif

    return 0;
}
