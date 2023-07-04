#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <QImage>
#include <QFile>
#include <QFileDialog>
#include <serialportmanager.h>



class SerialPortManager;
//class MainWindow;
class ImageProcessing: public QObject
{
    Q_OBJECT

public:
    ImageProcessing();
    ~ImageProcessing();

    void processImage(const QString &filePath);
    void reverseProcessImage(const QString &filePath);

public slots:
    void recieveSerialErrorSignal(QString);
signals:
    void sendImgInfo(QString);


private:
    QImage formatImage(const QImage &image);
    bool saveImageSrc(const QImage &image, const QString &newFileName);
    QString highlightingTheFileName(const QString &filePath);
    QString filenameConversion(const QString &filename, const QString &suffix);
    QImage formatData(const QImage &image, const QString &fileName);
    SerialPortManager *serialPortManager;
};

#endif // IMAGEPROCESSING_H
