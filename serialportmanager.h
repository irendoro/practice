#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H

//#include "mainwindow.h"
#include <QMainWindow>
#include <QObject>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QFile>

class MainWindow;
class SerialPortManager:public QObject
{
    Q_OBJECT

public:
    explicit SerialPortManager(MainWindow *mainWindow,QObject *parent = nullptr);
    ~SerialPortManager();
//    SerialPortManager();

    bool openPort(const QString &portName);
    void closePort();

    bool sendData(const QByteArray &data);
    void receiveData();

    void tosetPortName(const QString &arg1);
    void tosetBaudRate(qint32 baudRate);
    void tosetStopBits(QSerialPort::StopBits stopbits);
    void tosetParity(QSerialPort::Parity parity);
    void tosetDataBits(QSerialPort::DataBits dataBits);

    QString tosettings();

    void sendFile(const QString &filepath);
    void receiveFile(const QString &filepath);

//    void handleError(QSerialPort::SerialPortError error);

private:
    QSerialPort *serialPort;
    MainWindow *mainWindow;
};

#endif // SERIALPORTMANAGER_H
