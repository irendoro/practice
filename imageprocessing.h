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

    void processImage(const QString &filePath);
    void reverseProcessImage(const QString &filePath);
    bool transferSettings(QString portName, int baudRate, QString stopBits, QString parity, QString dataBits);
    void transferToClosePort();
    bool checkOpenPort();
    bool sendMessage(QByteArray byteArray);

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
    QByteArray formatImage(const QImage &image);
    bool saveImageSrc(QByteArray image, const QString &newFileName);
    QString highlightingTheFileName(const QString &filePath);
    QString filenameConversion(const QString &filename, const QString &suffix);
    QImage formatData(const QImage &image, const QString &fileName);

    //SerialPortManager *serialPortManager;
    std::unique_ptr<SerialPortManager> serialPortManager;
//    MainWindow *mainWindow;
};

#endif // IMAGEPROCESSING_H
