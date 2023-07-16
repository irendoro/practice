#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H

#include <QMainWindow>
#include <QObject>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QFile>

#include <memory>

//#include "mainwindow.h"

class ImageProcessing;
class SerialPortManager:public QObject
{
    Q_OBJECT


signals:
    void serialErrorSignal(QString);
    void serialReceiveSignal(QByteArray);
    void portOpened(bool);
    void serialNum(int);

public:
    SerialPortManager();
    ~SerialPortManager();

    bool openPort();
    bool testOpenPort();
    bool settingsOnComPort(QString portName, int baudRate, QString stopBits, QString parity, QString dataBits);
    void closePort();

    bool sendData(const QByteArray &data);
    void receiveData();
    void resetArr();

//    void tosetPortName(const QString &arg1);
//    void tosetBaudRate(qint32 baudRate);
//    void tosetStopBits(QSerialPort::StopBits stopbits);
//    void tosetParity(QSerialPort::Parity parity);
//    void tosetDataBits(QSerialPort::DataBits dataBits);

//    QString tosettings();

    void sendFile(const QString &filepath);
    void receiveFile(const QString &filepath);

public slots:
    void handleError();
//    bool checkPortStatus(QString portname);


private:
    //QSerialPort *serialPort;
    std::unique_ptr<QSerialPort> serialPort;
    qint64 n;
    QByteArray receivedData;
    //ImageProcessing *imageProcessing;

};

#endif // SERIALPORTMANAGER_H
