#include "serialportmanager.h"
#include <QDebug>

SerialPortManager::SerialPortManager(MainWindow *mainWindow, QObject *parent):QObject(parent), mainWindow(mainWindow)
{
    serialPort = new QSerialPort(this);

    //connect(serialPort, &QSerialPort::errorOccurred, this, &SerialPortManager::handleError);
    serialPort->setBaudRate(QSerialPort::Baud115200);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setParity(QSerialPort::NoParity);

}

SerialPortManager::~SerialPortManager()
{
    closePort();
}

bool SerialPortManager::openPort(const QString &portName)
{
    serialPort->setPortName(portName);
    if (serialPort->open(QIODevice::ReadWrite))
    {
//        connect(serialPort, &QSerialPort::readyRead, this, &SerialPortManager::receiveData);
        return true;
    }
    return false;
}

void SerialPortManager::closePort()
{
    if (serialPort->isOpen())
    {
        serialPort->close();
    }
}

bool SerialPortManager::sendData(const QByteArray &data)
{
    serialPort->write(data);
    if (serialPort->waitForBytesWritten())
        return true;
    else
        return false;
}

//void SerialPortManager::receiveData()
//{

//    if (serialPort->isOpen())
//    {
//        QByteArray responceData = serialPort->readAll();
//        if (!responceData.isEmpty())
//        {
//            QString Responce;
//            if (mainWindow->asciiHex())
//                Responce = QString(responceData);
//            else
//                Responce = QString(responceData.toHex());

//        }
//        else
//            return;
//    }
//}

void SerialPortManager::tosetPortName(const QString &arg1)
{
    serialPort->setPortName(arg1);
}

void SerialPortManager::tosetBaudRate(qint32 baudRate)
{
    serialPort->setBaudRate(baudRate);
}

void SerialPortManager::tosetStopBits(QSerialPort::StopBits stopbits)
{
    serialPort->setStopBits(stopbits);
}

void SerialPortManager::tosetParity(QSerialPort::Parity parity)
{
    serialPort->setParity(parity);
}

void SerialPortManager::tosetDataBits(QSerialPort::DataBits dataBits)
{
    serialPort->setDataBits(dataBits);
}

QString SerialPortManager::tosettings()
{
    QString portSettings = QString("Port Name: %1\n""Baud Rate: %2\nData Bits: %3\nParity: %4\n"
                                           "Stop Bits: %5\n")
                                           .arg(serialPort->portName())
                                           .arg(serialPort->baudRate())
                                           .arg(serialPort->dataBits())
                                           .arg(serialPort->parity())
                                           .arg(serialPort->stopBits());
    return portSettings;
}

void SerialPortManager::sendFile(const QString &filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly)) {
        //сообщение об ошибке
        return;
    }

    QByteArray fileData = file.readAll();

    serialPort->write(fileData);
    if (serialPort->waitForBytesWritten())
    {
        //все хорошо
    }
    else
    {
        //сообщение об ошибке
        return;
    }

}

void SerialPortManager::receiveFile(const QString &filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly)) {
        //сообщение об ошибке
        return;
    }
    QTextStream stream(&file);
    QByteArray responceData = serialPort->readAll();
    if (!responceData.isEmpty())
        stream << responceData;
    file.close();


}

//void SerialPortManager::handleError(QSerialPort::SerialPortError error)
//{
//    if (error == QSerialPort::NoError)
//        return;

//   // statusBar()->showMessage("Serial Port Error: " + serialPort->errorString());
//}


