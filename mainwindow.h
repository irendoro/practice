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

class ImageProcessing;
class SerialPortManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool asciiHex();

public slots:
    void sendOfError(QString str);
    void FromImg(QString str);

private slots:
    void onsetPortName(const QString &arg1);
    void onsetBaudRate(const QString &arg1);
    void onsetStopBits(const QString &arg1);
    void onsetParity(const QString &arg1);
    void onsetDataBits(const QString &arg1);
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
    ImageProcessing *imageProcessing;
};

#endif // MAINWINDOW_H
