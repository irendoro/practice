#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QSettings>
#include <QMessageBox>

#include "serialportmanager.h"
#include "imageprocessing.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
//    void SaveSettings(); //сохранение настроек
//    void loadSettings();//загрузка настроек
    bool asciiHex();

private slots:
    void onsetPortName(const QString &arg1);
    void onsetBaudRate(const QString &arg1);
    void onsetStopBits(const QString &arg1);
    void onsetParity(const QString &arg1);
    void onsetDataBits(const QString &arg1);
//    void sendOfError();
    void exitProgram();
    void toconnect();
    void disconnect();
    void settings();
    void send();
    void clear();
    void chooseImage();
    void sendDataFile();

private:
    Ui::MainWindow *ui;
    SerialPortManager *serialManager;
    ImageProcessing imageProcessing;
//    QSettings* settings;
//    QSerialPort *serial;
};

#endif // MAINWINDOW_H
