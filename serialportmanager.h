#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H

#include <QMainWindow>
#include <QObject>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QFile>

//#include "mainwindow.h"

class ImageProcessing;
class SerialPortManager:public QObject
{
    Q_OBJECT

public:
    SerialPortManager();
    ~SerialPortManager();

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

signals:
    void serialErrorSignal(QString);
    void readyReadSignal(QString);

private slots:
    void handleError();
private:
    QSerialPort *serialPort;
    ImageProcessing *imageProcessing;

};

#endif // SERIALPORTMANAGER_H
