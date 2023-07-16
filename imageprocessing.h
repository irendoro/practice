#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <QImage>
#include <QFile>
#include <QFileDialog>
#include <serialportmanager.h>
#include <memory>
#include <QObject>

class SerialPortManager;
class MainWindow;

class ImageProcessing: public QObject
{
    Q_OBJECT

public:
    ImageProcessing();
    ~ImageProcessing();

    QByteArray processImage(const QString &filePath, bool result);
    void reverseProcessImage(QByteArray array, bool result);
    bool transferSettings(QString portName, int baudRate, QString stopBits, QString parity, QString dataBits);
    void transferToClosePort();
    bool checkOpenPort();
    bool sendMessage(QByteArray byteArray);
    void resetArray();

public slots:
    void receiveSerialErrorSignal(QString);
    void receiveSerialSignal(QByteArray);
    //void myTmp(int);
//    void checkPortStatusSlot(QString portname);
//    void handlePortStatus(bool success);
signals:
    void printError(QString);
    void serialReceivedSignal(QByteArray);
    void portStatusSignal(bool);
//    void toMain(QString);


private:
    QByteArray formatImage(QImage &image, bool result);
    bool saveImage(QByteArray image, const QString &newFileName);
    QString highlightingTheFileName(const QString &filePath);
    QString filenameConversion(const QString &filename, const QString &suffix);
    QByteArray formatData(const QByteArray &image);
    QByteArray reFormatData(QByteArray &image);
    QString fileName;
    QImage createImage(QByteArray array, QString filename, bool result);
    int heightImage;
    int widthImage;


    //SerialPortManager *serialPortManager;
    std::unique_ptr<SerialPortManager> serialPortManager;
//    MainWindow *mainWindow;
};

#endif // IMAGEPROCESSING_H
