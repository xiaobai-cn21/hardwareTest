#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serial(new QSerialPort(this))  // Inicializamos el puerto serial
{
    ui->setupUi(this);
    setWindowTitle("Monitor de Sensores (Sencillo)");

    // Configuramos el puerto serial (predeterminado: /dev/pts/2)
    serial->setPortName("/dev/pts/2");  // Puerto de recepción (vinculado a /dev/pts/1)
    serial->setBaudRate(QSerialPort::Baud9600);  // Mismo baudrate que tu código
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    // Conectamos la señal de "datos disponibles" a nuestra función de lectura
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readSerialData);
}

MainWindow::~MainWindow()
{
    if (serial->isOpen()) {
        serial->close();  // Cerramos el puerto al cerrar la ventana
    }
    delete ui;
}

// Botón para conectar/desconectar el puerto serial
void MainWindow::on_connectButton_clicked()
{
    if (serial->isOpen()) {
        // Si está abierto, lo cerramos
        serial->close();
        ui->connectButton->setText("Conectar");
        ui->statusLabel->setText("Estado: Desconectado");
    } else {
        // Si está cerrado, lo abrimos
        if (serial->open(QIODevice::ReadOnly)) {  // Solo lectura (recibimos datos)
            ui->connectButton->setText("Desconectar");
            ui->statusLabel->setText("Estado: Conectado - Leyendo datos...");
        } else {
            // Si falla la conexión, mostramos error
            QMessageBox::critical(this, "Error", "No se pudo abrir el puerto serial:\n" + serial->errorString());
        }
    }
}

// Función para leer y parsear datos del puerto serial
void MainWindow::readSerialData()
{
    // Leemos todos los datos disponibles en el puerto
    QByteArray data = serial->readAll();
    QString datosRecibidos = QString(data);

    // Mostramos la cadena cruda (opcional, para depuración)
    ui->rawDataLabel->setText("Datos crudos: " + datosRecibidos);

    // Parseamos los datos (formato: "Temp=25.3,Press=1.05\n")
    QStringList partes = datosRecibidos.split(",");  // Separamos por coma
    if (partes.size() == 2) {  // Verificamos que haya 2 partes (Temp y Press)
        // Extraemos temperatura (ej: "Temp=25.3" → "25.3")
        QString tempStr = partes[0].split("=")[1];  // Separamos por "=" y tomamos el valor
        // Extraemos presión (ej: "Press=1.05" → "1.05")
        QString pressStr = partes[1].split("=")[1].replace("\n", "");  // Quitamos salto de línea

        // Mostramos cada dato en su etiqueta individual
        ui->tempLabel->setText("Temperatura: " + tempStr + " °C");
        ui->pressLabel->setText("Presión: " + pressStr + " MPa");
    }
}
