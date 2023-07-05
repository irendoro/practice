#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QFileDialog>
#include "imageprocessing.h"
#include "serialportmanager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    imageProcessing = new ImageProcessing();
//    connect(imageProcessing, SIGNAL(toMain(QString)), this, SLOT(fromImage(QString)));

    connect(imageProcessing, SIGNAL(printError(QString)), this, SLOT(sendOfError(QString)));
    connect(imageProcessing, SIGNAL(serialReceivedSignal(QByteArray)), this, SLOT(receiveMessage(QByteArray)));

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
    //инициализация начальных значений для вывода comboBox
    ui->setPortName->setCurrentIndex(0);
    ui->setBaudrate->setCurrentIndex(12);
    ui->setDatabits->setCurrentIndex(3);
    ui->setParity->setCurrentIndex(0);
    ui->setStopbits->setCurrentIndex(0);
    //переход к слотам
    connect(ui->Connect, &QPushButton::clicked, this, &MainWindow::toConnect);
    connect(ui->Disconnect, &QPushButton::clicked, this, &MainWindow::toDisconnect);
    connect(ui->send, &QPushButton::clicked, this, &MainWindow::send);
    connect(ui->clear, &QPushButton::clicked, this, &MainWindow::clear);
    connect(ui->chooseImage, &QPushButton::clicked, this,&MainWindow::chooseImage);
    connect(ui->sendFile, &QPushButton::clicked, this, &MainWindow::sendDataFile);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//функция возвращения значения нажатой кнопки
bool MainWindow::asciiHex()
{
    if (ui->ASCII1->isChecked())
        return true;
    else
        return false;
}

void MainWindow::sendOfError(QString errorsFromSerial)
{
    statusBar()->showMessage("Serial Port Error: " + errorsFromSerial);
}


//кнопка Connect действия при присоединении к ком-порту
void MainWindow::toConnect()
{
    QString portName = ui->setPortName->currentText();
    int baudRate = ui->setBaudrate->currentText().toInt();
    QString stopBits = ui->setStopbits->currentText();
    QString parity = ui->setParity->currentText();
    QString dataBits = ui->setDatabits->currentText();
    if (imageProcessing->transferSettings(portName, baudRate, stopBits, parity, dataBits))
    {
        ui->groupBox->setEnabled(false);
        ui->Connect->setEnabled(false);
        ui->Disconnect->setEnabled(true);
    }
}

//отключение компорта
void MainWindow::toDisconnect()
{
    imageProcessing->transferToClosePort();
    ui->Connect->setEnabled(true);
    ui->Disconnect->setEnabled(false);
    ui->groupBox->setEnabled(true);
}

void MainWindow::receiveMessage(QByteArray responceData)
{
    QString Responce;
    if (asciiHex())
        Responce = QString(responceData);
    else
        Responce = QString(responceData.toHex());
    ui->outputMessage->setText(Responce);
}

//отправка сообщения по ком-порту
void MainWindow::send()
{
    if (imageProcessing->checkOpenPort()) {
        QString message = ui->inputMessage->toPlainText();
        QByteArray byteArray;
        if (ui->ASCII->isChecked())
            byteArray = message.toUtf8();
        else
            byteArray = QByteArray::fromHex(message.toUtf8());
        if (imageProcessing->sendMessage(byteArray))
            statusBar()->showMessage("Success!");
        else
            statusBar()->showMessage("Unsuccess!");
    } else
        statusBar()->showMessage("Unsuccess! Com-port is closed!");
}

//очистка поля ввода сообщения
void MainWindow::clear()
{
    ui->inputMessage->clear();
}

//выбор файла bmp в диалоговом окне
void MainWindow::chooseImage()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите изображение", "","BMP Image(*.bmp);");
    if (filePath.isEmpty()) {
        statusBar()->showMessage("Unsuccess! You havn't selected a file");
        return;
    }
    imageProcessing->processImage(filePath);
}

//отправка файла по ком-порту
void MainWindow::sendDataFile()
{
    ui->chooseImage->setEnabled(false);
//    if (serialManager->openPort(portName))
//    {
//        imageProcessing.processImage();
//    }
}




