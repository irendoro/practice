#include "serialportmanager.h"
#include <QDebug>
#include "mainwindow.h"

SerialPortManager::SerialPortManager()
{
    serialPort.reset(new QSerialPort);
    //передача сигнала serialPort->SerialPortManager
    connect(serialPort.get(), SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handleError()));
//    connect(imageProcessing, SIGNAL(portStatusSignal(bool)), this, SLOT(checkPortStatus()));
    //инициализация начальных значений
    serialPort->setBaudRate(QSerialPort::Baud115200);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setParity(QSerialPort::NoParity);
//    connect(serialPort, &QSerialPort::readyRead, this, &SerialPortManager::receiveData);

}

SerialPortManager::~SerialPortManager()
{
    closePort();
}

//проверка открытия порта
bool SerialPortManager::openPort()
{
    bool rezult = false;
    if (serialPort->open(QIODevice::ReadWrite))
    {
        connect(serialPort.get(), &QSerialPort::readyRead, this, &SerialPortManager::receiveData);
        rezult = true;
    }
    return rezult;
}

bool SerialPortManager::testOpenPort()
{
    bool result = false;
    if (serialPort->isOpen())
        result = true;
    return result;
}

bool SerialPortManager::settingsOnComPort(QString portName, int baudRate, QString stopBits, QString parity, QString dataBits)
{
    bool result = false;

    serialPort->setPortName(portName);
    serialPort->setBaudRate(baudRate);

    QSerialPort::StopBits StopBits;
    if (stopBits == "1")
        StopBits = QSerialPort::OneStop;
    else if (stopBits == "1.5")
        StopBits = QSerialPort::OneAndHalfStop;
    else
        StopBits = QSerialPort::TwoStop;
    serialPort->setStopBits(StopBits);

    QSerialPort::Parity Parity;
    if (parity == "None")
        Parity = QSerialPort::NoParity;
    else if (parity == "Odd")
        Parity = QSerialPort::OddParity;
    else if (parity == "Even")
        Parity = QSerialPort::EvenParity;
    else if (parity == "Space")
        Parity = QSerialPort::SpaceParity;
    else
        Parity = QSerialPort::MarkParity;
    serialPort->setParity(Parity);

    QSerialPort::DataBits DataBits;
    if (dataBits == "5")
        DataBits = QSerialPort::Data5;
    else if (dataBits == "6")
        DataBits = QSerialPort::Data6;
    else if (dataBits == "7")
        DataBits = QSerialPort::Data7;
    else
        DataBits = QSerialPort::Data8;
    serialPort->setDataBits(DataBits);

    if (openPort())
        result = true;
    return result;
}

//проверка закрытия порта
void SerialPortManager::closePort()
{
    if (serialPort->isOpen())
    {
        serialPort->close();
    }
}

//отправка сообщения по ком-порту
bool SerialPortManager::sendData(const QByteArray &data)
{
    bool result = false;
    serialPort->write(data);
    if (serialPort->waitForBytesWritten())
        result = true;
    return result;
}

//прием сообщений по ком-порту
void SerialPortManager::receiveData()
{
    if (serialPort->isOpen())
    {
        QByteArray responceData = serialPort->readAll();
        if (!responceData.isEmpty())
        {
            emit serialReceiveSignal(responceData);
        }
        else
            qDebug() << "Not good";
    }
}

//отправка файла по ком-порту
void SerialPortManager::sendFile(const QString &filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly)) {
        //сообщение об ошибке
        return;
    }

   QByteArray fileData = file.readAll();

//    if(serialPort->isOpen())
//    {

//        serialPort->write(fileData);
//        if (serialPort->waitForBytesWritten())
//        {
//            //все хорошо
//        }
//        else
//        {
//            //сообщение об ошибке
//            return;
//        }
//    }


}

//прием файлов по ком-порту
void SerialPortManager::receiveFile(const QString &filepath)
{
    emit serialErrorSignal("шлем");
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

void SerialPortManager::handleError()
{
    QString errorMessage = serialPort->errorString();
    emit serialErrorSignal(errorMessage);
}





