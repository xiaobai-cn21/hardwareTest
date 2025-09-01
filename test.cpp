#include <iostream>
#include <string>
#include <thread>
#include <random>
#include <fcntl.h>   // Para open()
#include <termios.h> // Para configurar el puerto serial
#include <unistd.h>  // Para write(), close(), sleep()

// Función para configurar el puerto serial de Linux (ej: /dev/pts/1)
int setupSerialPort(const char* portPath) {
    // Abre el puerto serial en modo escritura (O_WRONLY) y sin bloqueo (O_NONBLOCK)
    int serialFD = open(portPath, O_WRONLY | O_NONBLOCK);
    if (serialFD == -1) {
        perror("Error opening serial port"); // Muestra el error (ej: "No such file or directory")
        return -1;
    }

    // Configura los parámetros del puerto (9600 baudios, 8 bits, sin paridad)
    struct termios tty;
    if (tcgetattr(serialFD, &tty) != 0) {
        perror("Error getting serial port attributes");
        close(serialFD);
        return -1;
    }

    // Limpiar flags antiguos
    tty.c_cflag &= ~CSIZE;    // Borra el tamaño de byte
    tty.c_cflag |= CS8;       // 8 bits de datos
    tty.c_cflag &= ~PARENB;   // Sin paridad
    tty.c_cflag &= ~CSTOPB;   // 1 bit de parada
    tty.c_cflag &= ~CRTSCTS;  // Sin control de flujo hardware

    // Configurar baudios (9600)
    cfsetospeed(&tty, B9600); // Velocidad de escritura
    cfsetispeed(&tty, B9600); // Velocidad de lectura (no usada aquí, pero se configura)

    // Aplicar configuración
    if (tcsetattr(serialFD, TCSANOW, &tty) != 0) {
        perror("Error setting serial port attributes");
        close(serialFD);
        return -1;
    }

    return serialFD; // Devuelve el "descriptor" del puerto (para enviar datos)
}

// Función para enviar datos por el puerto serial
void sendData(int serialFD, const std::string& data) {
    ssize_t bytesWritten = write(serialFD, data.c_str(), data.size());
    if (bytesWritten == -1) {
        perror("Error writing data");
    } else {
        std::cout << "Sent: " << data; // No usamos std::endl para evitar saltos extra
    }
}

int main() {
    // Configura el puerto virtual de Linux (el que anotaste: /dev/pts/1)
    const char* serialPort = "/dev/pts/1";
    int serialFD = setupSerialPort(serialPort);
    if (serialFD == -1) {
        return 1; // Salir si hay error
    }

    // Generador de números aleatorios para simular datos del sensor
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> tempDist(20.0, 30.0);  // Temp: 20-30°C
    std::uniform_real_distribution<> pressDist(0.8, 1.2);   // Pressure: 0.8-1.2 MPa

    // Enviar datos cada segundo
    std::cout << "Simulating sensor... (Press Ctrl+C to stop)\n";
    while (true) {
        // Generar datos ficticios
        double temperature = tempDist(gen);
        double pressure = pressDist(gen);

        // Formatear datos (ej: "Temp=25.3,Press=1.05\n")
        // Usamos printf para controlar decimales (mejor que std::to_string)
        char dataBuffer[50];
        snprintf(dataBuffer, sizeof(dataBuffer), "Temp=%.1f,Press=%.2f\n", temperature, pressure);
        std::string data = dataBuffer;

        // Enviar datos a /dev/pts/1
        sendData(serialFD, data);

        // Esperar 1 segundo
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Cerrar el puerto (el bucle while(true) nunca llega aquí, pero es buena práctica)
    close(serialFD);
    return 0;
}