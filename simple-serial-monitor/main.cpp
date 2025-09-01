#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();  // Mostramos la ventana principal
    return a.exec();  // Iniciamos el bucle de eventos de Qt
}
