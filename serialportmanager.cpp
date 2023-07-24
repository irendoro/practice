#include "serialportmanager.h"
#include <QDebug>
#include "mainwindow.h"

SerialPortManager::SerialPortManager()
{
    serialPort.reset(new QSerialPort);
    //передача сигнала serialPort->SerialPortManager
    connect(serialPort.get(), SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handleError()));
    //инициализация начальных значений
    serialPort->setBaudRate(QSerialPort::Baud115200);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setParity(QSerialPort::NoParity);
}

SerialPortManager::~SerialPortManager()
{
    closePort();
}

//--------------------------------------------------
//  Открытие Com-Port
//  вх: нет
//  выход: bool result - результат открытия ком-порта
//
//--------------------------------------------------
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

//--------------------------------------------------
//  Проверка открытия Com-Port
//  вх: нет
//  выход: bool result - результат открытия ком-порта
//
//--------------------------------------------------
bool SerialPortManager::testOpenPort()
{
    bool result = false;
    if (serialPort->isOpen())
        result = true;
    return result;
}

//--------------------------------------------------
//  Установка настроек Com-port
//  вх: QString portName - имя ком-порта,
//      int baudRate - скорость ком-порта
//      QString stopBits - стоп-биты
//      QString parity - четность
//      QString dataBits - дата биты
//  выход: bool result - результат установки настроек ком-порта
//
//--------------------------------------------------
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

//--------------------------------------------------
//  Проверка закрытия Com-Port
//  вх: нет
//  выход: нет
//
//--------------------------------------------------
void SerialPortManager::closePort()
{
    if (serialPort->isOpen())
    {
        serialPort->close();
    }

}

//--------------------------------------------------
//  Отправка сообщения на Com-Port
//  вх: const QByteArray &data - сообщение для отправки
//  выход: bool result - результат отправки сообщения на ком-порт
//
//--------------------------------------------------
bool SerialPortManager::sendData(const QByteArray &data)
{
    bool result = false;
    n = serialPort->write(data);
    qDebug() << "отправленные данные" << n;
    if (serialPort->waitForBytesWritten())
        result = true;
    return result;
    sentData = data;
}

//--------------------------------------------------
//  Прием сообщений по Com-Port
//  вх: нет
//  выход: нет
//
//--------------------------------------------------
bool SerialPortManager::receiveData()
{
    bool result = false;
    if (serialPort->isOpen())
    {
        QByteArray newData = serialPort->readAll();
        receivedData.append(newData);
        qDebug() << "Размер полученных данных" << receivedData.size();
        if (receivedData.size() >= n)
        {
            if (sentData.operator==(receivedData))
                result = true;


            emit serialReceiveSignal(receivedData);
        }
    }
    return result;
}

void SerialPortManager::resetArr()
{
    receivedData.clear();
}

//--------------------------------------------------
//  Отправка сообщения об ошибке
//  вх: нет
//  выход: нет
//
//--------------------------------------------------
void SerialPortManager::handleError()
{
    QString errorMessage = serialPort->errorString();
    emit serialErrorSignal(errorMessage);
}





