#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>  // Para manejar puerto serial

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_connectButton_clicked();  // Slot para el botón de conexión
    void readSerialData();            // Slot para leer datos del puerto

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;  // Objeto para manejar el puerto serial
};
#endif // MAINWINDOW_H
