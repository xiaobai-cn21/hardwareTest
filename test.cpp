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
    const char* serialPort = "/dev/pts/2";
    int serialFD = setupSerialPort(serialPort);
    if (serialFD == -1) {
        return 1; // Salir si hay error
    }

    // Generador de números aleatorios para simular datos del sensor
    //选要用的设备
    std::cout << "选您的设备:(1)重型机械电机, (2)能源站输油泵(3)汽车装配机器人" << std::endl;
    int equipment_number;
    std::cin >> equipment_number;
    if (equipment_number == 1) {
        std::uniform_real_distribution<> temp1(40.0, 80.0);  
        std::uniform_real_distribution<> vibr1(0.5, 2.0);   
        std::uniform_real_distribution<> elect1(150.0, 250.0);
        std::uniform_real_distribution<> electPress1(2.5, 4.0);
        std::uniform_real_distribution<> vel1(15000, 3000);
        std::uniform_real_distribution<> temp21(35, 60);
    }
    else if(equipment_number == 2) {
        std::uniform_real_distribution<> temp1(40.0, 80.0);  
        std::uniform_real_distribution<> vibr1(0.5, 2.0);   
        std::uniform_real_distribution<> elect1(150.0, 250.0);
        std::uniform_real_distribution<> electPress1(2.5, 4.0);
        std::uniform_real_distribution<> vel1(15000, 3000);
        std::uniform_real_distribution<> temp21(35, 60);
    } else {
        std::uniform_real_distribution<> temp1(40.0, 80.0);  
        std::uniform_real_distribution<> vibr1(0.5, 2.0);   
        std::uniform_real_distribution<> elect1(150.0, 250.0);
        std::uniform_real_distribution<> electPress1(2.5, 4.0);
        std::uniform_real_distribution<> vel1(15000, 3000);
        std::uniform_real_distribution<> temp21(35, 60);
    }
    std::random_device rd;
    std::mt19937 gen(rd());
   

    // Enviar datos cada segundo
    std::cout << "Simulating sensor... (Press Ctrl+C to stop)\n";
    while (true) {
        // Generar datos ficticios
        double temp = temp1(gen);
        double vibr = vibr1(gen);
        double elect = elect1(gen);
        double electPress = electPress1(gen);
        double vel = vel1(gen);
        double temp2 = temp21(gen);

        // Formatear datos (ej: "Temp=25.3,Press=1.05\n")
        // Usamos printf para controlar decimales (mejor que std::to_string)
        char dataBuffer[500];
        snprintf(dataBuffer, sizeof(dataBuffer), "电机绕组温度=%.1f,电机机壳振动=%.2f,电机启动电流=%.2f润滑系统压力=%.2f,电机转速=%.2f,润滑油温度=%.2f", temp, vibr, elect, electPress, vel, temp2 );
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