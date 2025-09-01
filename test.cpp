#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <random>

// Función para configurar el puerto serial
HANDLE setupSerialPort(const char* portName) {
    // Abre el puerto serial (ej: "COM3")
    HANDLE hSerial = CreateFileA(portName,
        GENERIC_WRITE,  // Solo necesitamos enviar datos
        0,              // No compartir el puerto
        NULL,           // Sin seguridad
        OPEN_EXISTING,  // Abrir puerto existente
        0,              // Sin atributos especiales
        NULL);          // Sin plantilla

    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "Error al abrir el puerto " << portName << std::endl;
        return NULL;
    }

    // Configurar parámetros del puerto (9600 baudios, 8 bits, sin paridad)
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Error al leer configuración del puerto" << std::endl;
        CloseHandle(hSerial);
        return NULL;
    }

    dcbSerialParams.BaudRate = CBR_9600;    // Velocidad: 9600 baudios
    dcbSerialParams.ByteSize = 8;           // 8 bits de datos
    dcbSerialParams.StopBits = ONESTOPBIT;  // 1 bit de parada
    dcbSerialParams.Parity = NOPARITY;      // Sin paridad

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Error al configurar el puerto" << std::endl;
        CloseHandle(hSerial);
        return NULL;
    }

    // Configurar temporizadores (no esencial para este ejemplo)
    COMMTIMEOUTS timeouts = {0};
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hSerial, &timeouts);

    return hSerial;
}

// Función para enviar datos por el puerto serial
void sendData(HANDLE hSerial, const std::string& data) {
    DWORD bytesWritten;
    // Convertir string a bytes y enviar
    if (!WriteFile(hSerial, data.c_str(), data.size(), &bytesWritten, NULL)) {
        std::cerr << "Error al enviar datos" << std::endl;
    } else {
        std::cout << "Enviado: " << data << std::endl;
    }
}

int main() {
    // Configurar el puerto COM3 (el puerto virtual que creaste con VSPD)
    HANDLE hSerial = setupSerialPort("COM3");
    if (hSerial == NULL) {
        return 1; // Salir si hay error
    }

    // Generador de números aleatorios para simular variaciones en los datos
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> tempDist(20.0, 30.0);  // Temp: 20-30°C
    std::uniform_real_distribution<> pressDist(0.8, 1.2);   // Presión: 0.8-1.2 MPa

    // Enviar datos cada segundo
    std::cout << "Simulando sensor... (Presiona Ctrl+C para detener)" << std::endl;
    while (true) {
        // Generar datos ficticios
        double temperature = tempDist(gen);
        double pressure = pressDist(gen);

        // Formatear los datos como string (ej: "Temp=25.3,Press=1.05\n")
        std::string data = "Temp=" + std::to_string(temperature) + 
                          ",Press=" + std::to_string(pressure) + "\n";

        // Enviar por COM3
        sendData(hSerial, data);

        // Esperar 1 segundo antes de enviar el siguiente dato
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Cerrar el puerto (en práctica, el bucle while(true) nunca llega aquí)
    CloseHandle(hSerial);
    return 0;
}
