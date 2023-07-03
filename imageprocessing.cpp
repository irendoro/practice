#include "imageprocessing.h"
#include <QDebug>
#include <QFileInfo>

//Преобразование изображения для отправки на ком-порт
void ImageProcessing::processImage(const QString &filePath)
{
    QImage image(filePath);
    if (image.isNull())
    {
        //Вывод сообщения об ошибке
        return;
    }
    //выделение имени файла
    QString fileName = highlightingTheFileName(filePath);
    //создание имени бинарного файла "Файл_src.bin"
    QString newFilename = filenameConversion(fileName, "_src.bin");
    //если изображение цветное, то дополнительное форматирование
    if (image.format() == QImage::Format_RGB666)
        image = formatImage(image);
    //сохранение полученных результатов в бинарный файл "файл_src.bin"
    if (!saveImageSrc(image, newFilename))
    {
        //сообщение об ошибке
        return;
    }
    newFilename = filenameConversion(fileName, "_data.bin");
    //форматирование каждого пикселя по протоколу
    image = formatData(image, newFilename);
    //добавить проверку что файл вообще есть и он не пустой
    serialPortManager.sendFile(newFilename);
    newFilename = filenameConversion(fileName, "_res_data.bin");
    serialPortManager.receiveFile(newFilename);
}

void ImageProcessing::reverseProcessImage(const QString &filePath)
{
    //??
}

bool ImageProcessing::saveImageSrc(const QImage &image, const QString &newFileName)
{
    //QString newFileName = filename + "_src.bin";
    QFile outputFile(newFileName);
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        //сообщение об ошибке
        return false;
    }
    QDataStream stream(&outputFile);
    stream.setByteOrder(QDataStream::LittleEndian);
    for (int y = 0; y < image.height(); y++)
    {
        for (int x = 0; x < image.width(); x++)
        {
            QRgb rgb = image.pixel(x, y);

            stream << static_cast<quint16>(qRed(rgb));
            stream << static_cast<quint16>(qGreen(rgb));
            stream << static_cast<quint16>(qBlue(rgb));

            int gray = qGray(rgb);
            stream << static_cast<quint8>(gray);
        }
    }
    outputFile.close();
    return true;
}

QImage ImageProcessing::formatImage(const QImage &image)
{
    QImage formattedImage(image.width(), image.height(), QImage::Format_RGB16);
    for (int y = 0; y<image.height(); y++)
    {
        for (int x = 0; x<image.width(); x++)
        {
            QRgb pixel = image.pixel(x,y);
            int colorPixel = (qRed(pixel) >> 3) << 11 | (qGreen(pixel) >> 2) << 5 | (qBlue(pixel) >> 3);
            formattedImage.setPixel(x,y,colorPixel);
        }
    }
    return formattedImage;
}

QString ImageProcessing::highlightingTheFileName(const QString &filePath)
{
    int dotIndex = filePath.lastIndexOf(".");
    if (dotIndex != -1) {
        return filePath.left(dotIndex);
    }
    return filePath;
}

QString ImageProcessing::filenameConversion(const QString &filename, const QString &suffix)
{
    return  filename + suffix;
}

QImage ImageProcessing::formatData(const QImage &image, const QString &newFilename)
{
    QFile outputFile(newFilename);
    QImage formattedImage(image.width(), image.height(), QImage::Format_RGB32);
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        //сообщение об ошибке
        return formattedImage;
    }

    QDataStream stream(&outputFile);
    stream.setByteOrder(QDataStream::LittleEndian);
    for (int y = 0; y<image.height(); y++)
    {
        for (int x = 0; x<image.width(); x++)
        {
            QRgb pixel = image.pixel(x,y);

            quint64 formattedPixel = static_cast<quint64>(pixel) << 32;

            stream << formattedPixel;
            formattedImage.setPixel(x,y,pixel);
        }
    }
    outputFile.close();
    return formattedImage;
}


