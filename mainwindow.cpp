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
    ui->send->setEnabled(false);
    ui->outputMessage->setReadOnly(true);
    //переход к слотам
    connect(ui->Connect, &QPushButton::clicked, this, &MainWindow::toConnect);
    connect(ui->Disconnect, &QPushButton::clicked, this, &MainWindow::toDisconnect);
    connect(ui->send, &QPushButton::clicked, this, &MainWindow::send);
    connect(ui->clear, &QPushButton::clicked, this, &MainWindow::clear);
    connect(ui->chooseImage, &QPushButton::clicked, this,&MainWindow::chooseImage);
    connect(ui->sendFile, &QPushButton::clicked, this, &MainWindow::sendDataFile);
    connect(ui->compare, &QPushButton::clicked, this, &MainWindow::comparision);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//--------------------------------------------------
//  Получение результата нажатия кнопки выбора кодировки данных
//  для получения данных с Com-port
//
//  вх: нет
//  выход: bool result - результат нажатия кнопки:
//         ASCII - true; HEX - false
//
//--------------------------------------------------
bool MainWindow::asciiHex()
{
    bool result = false;
    if (ui->ASCII1->isChecked())
        result = true;
    return result;
}

//--------------------------------------------------
//  Вывод сообщения об ошибке, связанной с Com-port
//  в статусную строку
//  вх: QString errorsFromSerial - сообщение с ошибкой
//  выход: нет
//
//--------------------------------------------------
void MainWindow::sendOfError(QString errorsFromSerial)
{
    statusBar()->showMessage("Serial Port Error: " + errorsFromSerial);
}

//--------------------------------------------------
//  Кнопка Connect, установка настроек, передача настроек
//  в класс ImageProcessing, деактивация кнопок
//  вх: нет
//  выход: нет
//
//--------------------------------------------------
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
        ui->send->setEnabled(true);
    }
}

//--------------------------------------------------
//  Кнопка Disconnect, активация кнопок
//  вх: нет
//  выход: нет
//
//--------------------------------------------------
void MainWindow::toDisconnect()
{
    imageProcessing->transferToClosePort();
    statusBar()->showMessage("Com-port was closed successfully");
    ui->Connect->setEnabled(true);
    ui->Disconnect->setEnabled(false);
    ui->groupBox->setEnabled(true);
    ui->sendFile->setEnabled(false);
    ui->send->setEnabled(false);
    ui->compare->setEnabled(false);
}

//--------------------------------------------------
//  Отображение полученного сообщения с Com-Port в поле вывода
//  вх: QByteArray responceData - полученное сообщение
//  выход: нет
//
//--------------------------------------------------
void MainWindow::receiveMessage(QByteArray responceData)
{
    QString Responce;
    if (asciiHex())
        Responce = QString(responceData);
    else
        Responce = QString(responceData.toHex());
    ui->outputMessage->append(Responce);
    if (!flag) {
        bool result = convertateToImage();
        imageProcessing->reverseProcessImage(responceData, result);
    }

}

void MainWindow::reset()
{
    imageProcessing->resetArray();
}

void MainWindow::comparision()
{
    bool result = imageProcessing->comparisonData();
    if (!result)
        statusBar()->showMessage("Принятое изображение не совпадает с отправленным!");
    else
        statusBar()->showMessage("Принятое изображение совпадает с отправленным!");
}

//--------------------------------------------------
//  Кнопка Send, отправка сообщения для пересылки на ComPort
//  в класс ImageProcessing
//
//  вх: нет
//  выход: нет
//
//--------------------------------------------------
void MainWindow::send()
{
    reset();
    flag = true;
    if (imageProcessing->checkOpenPort()) {
        QString message = ui->inputMessage->toPlainText();
        QByteArray byteArray;
        if (ui->ASCII->isChecked())
            byteArray = message.toUtf8();
        else
            byteArray = QByteArray::fromHex(message.toUtf8());
        if (imageProcessing->sendMessage(byteArray))
            statusBar()->showMessage("Data successfully sent via Com-port!!");
        else
            statusBar()->showMessage("Data unsuccessfully sent via Com-port!!");
    } else
        statusBar()->showMessage("Unsuccess! Com-port is closed!");
}

//--------------------------------------------------
//  Очистка полей ввода и вывода
//  вх: нет
//  выход: нет
//
//--------------------------------------------------
void MainWindow::clear()
{
    ui->inputMessage->clear();
    ui->outputMessage->clear();
}

//--------------------------------------------------
//  Кнопка ChooseImage - выбор изображения в формате .bmp
//  вх: нет
//  выход: нет
//
//--------------------------------------------------
void MainWindow::chooseImage()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите изображение", "","BMP Image(*.bmp);");
    if (filePath.isEmpty()) {
        statusBar()->showMessage("Unsuccess! You havn't selected a file");
        return;
    }
    ui->sendFile->setEnabled(true);
    ui->compare->setEnabled(false);
    bool result = convertateToImage();
    imageData = imageProcessing->processImage(filePath, result);
}

//--------------------------------------------------
//  Кнопка SendData - передача файла по ком-порту в класс ImageProcessing
//  вх: нет
//  выход: нет
//
//--------------------------------------------------
void MainWindow::sendDataFile()
{
    reset();
    flag = false;
    bool result = imageProcessing->sendMessage(imageData);
    if (result)
    {
        ui->compare->setEnabled(true);
        statusBar()->showMessage("DataFile successfully sent via Com-port!");
    }
    else
        statusBar()->showMessage("DataFile unsuccessfully sent via Com-port!");
}

bool MainWindow::convertateToImage()
{
    bool result = true;
    if (ui->radioButtonMono->isChecked())
        result = false;
    return result;
}




