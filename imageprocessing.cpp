#include "imageprocessing.h"

#include <QDebug>
#include <QFileInfo>
//#include <QObject>
#include "mainwindow.h"

ImageProcessing::ImageProcessing()
{
    qDebug()<<"";
    serialPortManager = new SerialPortManager();

   // connect(serialPortManager, SIGNAL(serialErrorSignal(QString)), this, SLOT(recieveSerialErrorSignal(QString)));
}


ImageProcessing:: ~ImageProcessing()
{
    qDebug()<<"!!";
}




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
    qDebug() << image.format();
    //if (image.format() == QImage::Format_RGB666)
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
    serialPortManager->sendFile(newFilename);
    newFilename = filenameConversion(fileName, "_res_data.bin");
    serialPortManager->receiveFile(newFilename);
}

//обратное преобразование
void ImageProcessing::reverseProcessImage(const QString &filePath)
{
    //??
}

void ImageProcessing::recieveSerialErrorSignal(QString errorsFromSerial)
{
    qDebug() <<"ImageProcessing получил сигнал с ошибками, высылаю сигнал дальше" ;
    //emit printError(errorsFromSerial);
}

//сохранение файлов после преобразования из 24-ричного представления в 16-ричное
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


//представление цветных изображений в 16-ричном формате
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
    QRgb pixel = image.pixel(0,0);

    int colorPixel = (qRed(pixel) >> 3) << 11 | (qGreen(pixel) >> 2) << 5 | (qBlue(pixel) >> 3);

    qDebug() << QString("%1").arg(pixel,0,16);

    qDebug() << QString("%1").arg(colorPixel,0,16);
    QString string = "j has done!";
    emit sendImgInfo(string);
    return formattedImage;
}




//выделение имени файла (отсечение расширения)
QString ImageProcessing::highlightingTheFileName(const QString &filePath)
{
    int dotIndex = filePath.lastIndexOf(".");
    if (dotIndex != -1) {
        return filePath.left(dotIndex);
    }
    return filePath;
}

//добавление расширения к имени файла
QString ImageProcessing::filenameConversion(const QString &filename, const QString &suffix)
{
    return  filename + suffix;
}

//преобразование каждого пикселя по протоколу и сохранение в бинарный файл для отправки по ком-порту
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


