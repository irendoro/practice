#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <QImage>
#include <QFile>
#include <QFileDialog>
#include <serialportmanager.h>

class ImageProcessing
{
public:
    void processImage(const QString &filePath);
//    explicit ImageProcessing(MainWindow *mainWindow,QObject *parent = nullptr);
//    ~ImageProcessing();
private:
    QImage formatImage(const QImage &image);
    bool saveImageSrc(const QImage &image, const QString &newFileName);
    QString highlightingTheFileName(const QString &filePath);
    QString filenameConversion(const QString &filename, const QString &suffix);
    QImage formatData(const QImage &image, const QString &fileName);
    QString newFileName;
    SerialPortManager serialPortManager;
};

#endif // IMAGEPROCESSING_H
