#include "serialportmanager.h"
#include <QDebug>
#include "mainwindow.h"

SerialPortManager::SerialPortManager()
{
    serialPort = new QSerialPort();
    //инициализация начальных значений
    connect(serialPort, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handleError()));
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
    serialPort->write(data);
    if (serialPort->waitForBytesWritten())
        return true;
    else
        return false;
}

//прием сообщений по ком-порту
void SerialPortManager::receiveData()
{

    if (serialPort->isOpen())
    {
        QByteArray responceData = serialPort->readAll();
        if (!responceData.isEmpty())
        {
            QString Responce;
//            if (mainWindow->asciiHex())
//                Responce = QString(responceData);
//            else
                Responce = QString(responceData.toHex());
            //emit Res
        }
        else
            return;
    }
}

//присваивание имени ком-порту
void SerialPortManager::tosetPortName(const QString &arg1)
{
    serialPort->setPortName(arg1);
}

//присваивание скорости
void SerialPortManager::tosetBaudRate(qint32 baudRate)
{
    serialPort->setBaudRate(baudRate);
}

//присваивание стоп-битов
void SerialPortManager::tosetStopBits(QSerialPort::StopBits stopbits)
{
    serialPort->setStopBits(stopbits);
}

//присваивание четности
void SerialPortManager::tosetParity(QSerialPort::Parity parity)
{
    serialPort->setParity(parity);
}

//присваивание датабитов
void SerialPortManager::tosetDataBits(QSerialPort::DataBits dataBits)
{
    serialPort->setDataBits(dataBits);
}


//вывод настроек
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
//    if (error == QSerialPort::NoError)
//        return;
    QString errorMessage = serialPort->errorString();
    emit serialErrorSignal(errorMessage);
}



