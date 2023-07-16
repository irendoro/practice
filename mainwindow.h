#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QSettings>
#include <QMessageBox>

#include "serialportmanager.h"
#include "imageprocessing.h"


namespace Ui {
class MainWindow;
}

class ImageProcessing;
class SerialPortManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool asciiHex();

public slots:
    void sendOfError(QString);
//    void FromImg(QString str);
    void toConnect();
    void fromImage(QString);
    void toDisconnect();
    void receiveMessage(QByteArray);
    void reset();

//    void handleForwardPortOpened();
signals:
    void checkportStatusSignal(QString);
//    void imageDataReady(const QByteArray &array);

private slots:
    void onSetPortName(const QString &arg1);
    void onSetBaudRate(const QString &arg1);
    void onSetStopBits(const QString &arg1);
    void onSetParity(const QString &arg1);
    void onSetDataBits(const QString &arg1);
//    void exitProgram();


//    void settings();
    void send();
    void clear();
    void chooseImage();
    void sendDataFile();
    bool convertateToImage();

private:
    Ui::MainWindow *ui;
    //SerialPortManager *serialManager;
    ImageProcessing *imageProcessing;
    QByteArray imageData;


};

#endif // MAINWINDOW_H
