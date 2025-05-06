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

// Cartas jugadas en la ronda actual
Carta cartaJugada[2];
bool hayCartaJugada[2] = {false, false};

// Función para enviar un mensaje al servidor
void enviarMensaje(SOCKET clienteSocket, const string& mensaje) {
    send(clienteSocket, mensaje.c_str(), mensaje.length(), 0);
}

// Función para recibir un mensaje del servidor
string recibirMensaje(SOCKET clienteSocket) {
    char buffer[1024];
    int bytesRecibidos = recv(clienteSocket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesRecibidos <= 0) {
        return "";
    }
    
    buffer[bytesRecibidos] = '\0';
    return string(buffer);
}

// Función para mostrar el estado del juego
void mostrarEstado() {
    limpiarPantalla();
    
    cout << "=== JUEGO DE CARTAS EN RED ===" << endl;
    cout << "Tu nombre: " << miNombre << " (Jugador " << miId + 1 << ")" << endl;
    cout << "Oponente: " << nombreOponente << " (Jugador " << (1 - miId) + 1 << ")" << endl;
    cout << "Cartas ganadas: " << misCartasGanadas << " vs " << cartasGanadasOponente << endl;
    cout << endl;
    
    // Mostramos la zona de batalla
    cout << "=== ZONA DE BATALLA ===" << endl;
    
    if (hayCartaJugada[0]) {
        cout << "Jugador 1: ";
        cartaJugada[0].mostrar();
        cout << " (Color " << nombreColor(cartaJugada[0].color) << ")" << endl;
    } else {
        cout << "Jugador 1: No ha jugado carta" << endl;
    }
    
    if (hayCartaJugada[1]) {
        cout << "Jugador 2: ";
        cartaJugada[1].mostrar();
        cout << " (Color " << nombreColor(cartaJugada[1].color) << ")" << endl;
    } else {
        cout << "Jugador 2: No ha jugado carta" << endl;
    }
    
    cout << endl;
    
    // Mostramos nuestras cartas
    cout << "=== TUS CARTAS ===" << endl;
    
    for (int i = 0; i < miMano.size(); i++) {
        cout << (i + 1) << ". ";
        miMano[i].mostrar();
        cout << " (Color " << nombreColor(miMano[i].color) << ")" << endl;
    }
    
    cout << endl;
    
    // Mostramos de quién es el turno
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

// Función para procesar los mensajes del servidor
void procesarMensaje(const string& mensaje) {
    // Dividimos el mensaje por el delimitador '|'
    vector<string> partes;
    size_t pos = 0;
    string token;
    string mensajeCopia = mensaje;
    
    while ((pos = mensajeCopia.find('|')) != string::npos) {
        token = mensajeCopia.substr(0, pos);
        partes.push_back(token);
        mensajeCopia.erase(0, pos + 1);
    }
    
    if (!mensajeCopia.empty()) {
        partes.push_back(mensajeCopia);
    }
    
    if (partes.empty()) {
        return;
    }
    
    // Procesamos el mensaje según su tipo
    if (partes[0] == "BIENVENIDO") {
        // Formato: "BIENVENIDO|idJugador"
        miId = stoi(partes[1]);
        cout << "Te has conectado como Jugador " << miId + 1 << endl;
    }
    else if (partes[0] == "CARTAS") {
        // Formato: "CARTAS|numCartas|color1|poder1|color2|poder2|..."
        int numCartas = stoi(partes[1]);
        miMano.clear();
        
        for (int i = 0; i < numCartas; i++) {
            int color = stoi(partes[2 + i*2]);
            int poder = stoi(partes[3 + i*2]);
            miMano.push_back(Carta(color, poder));
        }
        
        cout << "Has recibido " << numCartas << " cartas." << endl;
    }
    else if (partes[0] == "TURNO") {
        // Formato: "TURNO|jugadorActual"
        int jugadorActual = stoi(partes[1]);
        miTurno = (jugadorActual == miId);
        
        // Limpiamos las cartas jugadas al inicio de cada turno
        if (jugadorActual == 0) {
            hayCartaJugada[0] = false;
            hayCartaJugada[1] = false;
        }
        
        mostrarEstado();
    }
    else if (partes[0] == "JUGADA") {
        // Formato: "JUGADA|idJugador|colorCarta|poderCarta"
        int idJugador = stoi(partes[1]);
        int colorCarta = stoi(partes[2]);
        int poderCarta = stoi(partes[3]);
        
        cartaJugada[idJugador] = Carta(colorCarta, poderCarta);
        hayCartaJugada[idJugador] = true;
        
        mostrarEstado();
    }
    else if (partes[0] == "RESULTADO") {
        // Formato: "RESULTADO|ganadorRonda|cartasGanadas1|cartasGanadas2"
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
    }
    else if (partes[0] == "FIN") {
        // Formato: "FIN|ganadorJuego|cartasGanadas1|cartasGanadas2"
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
    // Inicializamos Winsock en Windows
    #ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            cerr << "Error al inicializar Winsock" << endl;
            return 1;
        }
    #endif
    
    // Creamos el socket del cliente
    SOCKET clienteSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clienteSocket == INVALID_SOCKET) {
        cerr << "Error al crear el socket" << endl;
        #ifdef _WIN32
            WSACleanup();
        #endif
        return 1;
    }
    
    // Pedimos la dirección IP del servidor
    string direccionIP;
    cout << "Ingresa la dirección IP del servidor (o 'localhost'): ";
    getline(cin, direccionIP);
    
    // Configuramos la dirección del servidor
    sockaddr_in direccionServidor;
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_port = htons(8888); // Puerto 8888
    
    // Convertimos la dirección IP
    if (direccionIP == "localhost") {
        direccionIP = "127.0.0.1";
    }
    
    if (inet_pton(AF_INET, direccionIP.c_str(), &direccionServidor.sin_addr) <= 0) {
        cerr << "Dirección IP inválida" << endl;
        closesocket(clienteSocket);
        #ifdef _WIN32
            WSACleanup();
        #endif
        return 1;
    }
    
    // Nos conectamos al servidor
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
    
    // Recibimos nuestro ID
    string mensajeBienvenida = recibirMensaje(clienteSocket);
    procesarMensaje(mensajeBienvenida);
    
    // Pedimos el nombre al jugador
    cout << "Ingresa tu nombre: ";
    getline(cin, miNombre);
    
    // Enviamos el nombre al servidor
    enviarMensaje(clienteSocket, "NOMBRE|" + miNombre);
    
    // Pedimos el nombre del oponente
    cout << "Ingresa el nombre de tu oponente: ";
    getline(cin, nombreOponente);
    
    // Recibimos las cartas
    string mensajeCartas = recibirMensaje(clienteSocket);
    procesarMensaje(mensajeCartas);
    
    // Recibimos el turno inicial
    string mensajeTurno = recibirMensaje(clienteSocket);
    procesarMensaje(mensajeTurno);
    
    // Bucle principal del juego
    while (!juegoTerminado) {
        if (miTurno) {
            // Es nuestro turno, elegimos una carta
            int indiceCartaElegida;
            cin >> indiceCartaElegida;
            cin.ignore(); // Limpiamos el buffer
            
            indiceCartaElegida--; // Ajustamos al índice base 0
            
            if (indiceCartaElegida >= 0 && indiceCartaElegida < miMano.size()) {
                // Enviamos la carta elegida al servidor
                enviarMensaje(clienteSocket, "CARTA|" + to_string(indiceCartaElegida));
                
                // Quitamos la carta de nuestra mano
                miMano.erase(miMano.begin() + indiceCartaElegida);
                
                // Ya no es nuestro turno
                miTurno = false;
            } else {
                cout << "Carta inválida. Elige una carta entre 1 y " << miMano.size() << ": ";
            }
        } else {
            // No es nuestro turno, esperamos mensajes del servidor
            string mensaje = recibirMensaje(clienteSocket);
            
            if (mensaje.empty()) {
                cout << "Desconectado del servidor." << endl;
                break;
            }
            
            procesarMensaje(mensaje);
        }
    }
    
    // Esperamos a que el usuario presione Enter para salir
    cout << "\nPresiona Enter para salir...";
    cin.get();
    
    // Cerramos el socket
    closesocket(clienteSocket);
    
    // Limpiamos Winsock en Windows
    #ifdef _WIN32
        WSACleanup();
    #endif
    
    return 0;
}
