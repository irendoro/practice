#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serialManager = new SerialPortManager(this);


    //Заполнение списка доступных ком-портов
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        ui->setPortName->addItem(serialPortInfo.portName());
    }
    //Заполнение списка скоростей
    foreach (const qint32 baudRate, QSerialPortInfo::standardBaudRates())
    {
        ui->setBaudrate->addItem(QString::number(baudRate));
    }
    ui->setPortName->setCurrentIndex(0);
    ui->setBaudrate->setCurrentIndex(12);
    ui->setDatabits->setCurrentIndex(3);
    ui->setParity->setCurrentIndex(0);
    ui->setStopbits->setCurrentIndex(0);
    //Обработчик события кнопки "Выход"
    connect(ui->Exit, &QPushButton::clicked, this, &MainWindow::exitProgram);
    connect(ui->Settings, &QPushButton::clicked, this, &MainWindow::settings);
    connect(ui->Connect, &QPushButton::clicked, this, &MainWindow::toconnect);
    connect(ui->Disconnect, &QPushButton::clicked, this, &MainWindow::disconnect);
    connect(ui->send, &QPushButton::clicked, this, &MainWindow::send);
    connect(ui->clear, &QPushButton::clicked, this, &MainWindow::clear);
    connect(ui->setPortName, QOverload<const QString&>::of(&QComboBox::activated),this, &MainWindow::onsetPortName);
    connect(ui->setBaudrate, QOverload<const QString&>::of(&QComboBox::activated),this, &MainWindow::onsetBaudRate);
    connect(ui->setParity, QOverload<const QString&>::of(&QComboBox::activated),this, &MainWindow::onsetParity);
    connect(ui->setDatabits, QOverload<const QString&>::of(&QComboBox::activated),this, &MainWindow::onsetDataBits);
    connect(ui->setStopbits, QOverload<const QString&>::of(&QComboBox::activated),this, &MainWindow::onsetStopBits);
    connect(ui->chooseImage, &QPushButton::clicked, this,&MainWindow::chooseImage);
    connect(ui->sendFile, &QPushButton::clicked, this, &MainWindow::sendDataFile);

}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::asciiHex()
{
    if (ui->ASCII1->isChecked())
        return true;
    else
        return false;
}

//Функция выхода из программы
void MainWindow::exitProgram()
{
    close();
}

void MainWindow::toconnect()
{
    QString portName = ui->setPortName->currentText();
    if (serialManager->openPort(portName))
    {
        ui->setPortName->setEnabled(false);
        ui->setBaudrate->setEnabled(false);
        ui->setDatabits->setEnabled(false);
        ui->setParity->setEnabled(false);
        ui->setStopbits->setEnabled(false);
        ui->Connect->setEnabled(false);
        ui->Disconnect->setEnabled(true);
        statusBar()->showMessage("Success! COM-PORT is opened.");
    }
    else
    {
        statusBar()->showMessage("Unsuccess! COM-PORT is closed.");
        return;
    }
}

//отключение компорта
void MainWindow::disconnect()
{
    serialManager->closePort();
    ui->Connect->setEnabled(true);
    ui->Disconnect->setEnabled(false);
    ui->setPortName->setEnabled(true);
    ui->setBaudrate->setEnabled(true);
    ui->setDatabits->setEnabled(true);
    ui->setParity->setEnabled(true);
    ui->setStopbits->setEnabled(true);
}



//Вывод настроек компорта
void MainWindow::settings()
{
    QString portSettings = serialManager->tosettings();
    QMessageBox::information(this, "Port Settings", portSettings);
}

void MainWindow::send()
{
    QString portName = ui->setPortName->currentText();
    if (serialManager->openPort(portName))
    {
        QString message = ui->inputMessage->toPlainText();
        QByteArray byteArray;
        if (ui->ASCII->isChecked())
            byteArray = message.toUtf8();
        else
            byteArray = QByteArray::fromHex(message.toUtf8());
        if (serialManager->sendData(byteArray))
            statusBar()->showMessage("Success!");
        else
            statusBar()->showMessage("Unsuccess!");
    }
    else
        statusBar()->showMessage("Unsuccess! Com-port is closed!");
}

void MainWindow::clear()
{
    ui->inputMessage->clear();
}

void MainWindow::chooseImage()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите изображение", "","BMP Image(*.bmp);");
    if (filePath.isEmpty()) {
        statusBar()->showMessage("Unsuccess! You havn't selected a file");
        return;
    }
    imageProcessing.processImage(filePath);

}

void MainWindow::onsetPortName(const QString &arg1)
{
    serialManager->tosetPortName(arg1);
}

void MainWindow::onsetBaudRate(const QString &arg1)
{
    int n = arg1.toInt();
    serialManager->tosetBaudRate(n);
}

void MainWindow::onsetStopBits(const QString &arg1)
{
    QSerialPort::StopBits stopBits = QSerialPort::OneStop;
    if (arg1 == "1")
        stopBits = QSerialPort::OneStop;
    else if (arg1 == "1.5")
        stopBits = QSerialPort::OneAndHalfStop;
    else if (arg1 == "2")
        stopBits = QSerialPort::TwoStop;
    serialManager->tosetStopBits(stopBits);
}

void MainWindow::onsetParity(const QString &arg1)
{
    QSerialPort::Parity parity = QSerialPort::NoParity;
    if (arg1 == "None")
        parity = QSerialPort::NoParity;
    else if (arg1 == "Odd")
        parity = QSerialPort::OddParity;
    else if (arg1 == "Even")
        parity = QSerialPort::EvenParity;
    else if (arg1 == "Space")
        parity = QSerialPort::SpaceParity;
    else if (arg1 == "Mark")
        parity = QSerialPort::MarkParity;
    serialManager->tosetParity(parity);
}
void MainWindow::onsetDataBits(const QString &arg1)
{
    QSerialPort::DataBits dataBits = QSerialPort::Data8;
    if (arg1 == "5")
        dataBits = QSerialPort::Data5;
    else if (arg1 == "6")
        dataBits = QSerialPort::Data6;
    else if (arg1 == "7")
        dataBits = QSerialPort::Data7;
    else if (arg1 == "8")
        dataBits = QSerialPort::Data8;
    serialManager->tosetDataBits(dataBits);
}

//void MainWindow::sendOfError()
//{

//}



