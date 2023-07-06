#include "imageprocessing.h"

#include <QDebug>
#include <QFileInfo>
#include <QObject>
#include "mainwindow.h"
#include "serialportmanager.h"

ImageProcessing::ImageProcessing()
{
    serialPortManager.reset(new SerialPortManager);
    QObject::connect(serialPortManager.get(), &SerialPortManager::serialErrorSignal,
    this, &ImageProcessing::receiveSerialErrorSignal);
    QObject::connect(serialPortManager.get(), &SerialPortManager::serialReceiveSignal,
    this, &ImageProcessing::receiveSerialSignal);
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
        qDebug() << "Не удалось открыть изображение";
        return;
    }
    QString fileName = highlightingTheFileName(filePath); //выделение имени файла
    QString newFilename = filenameConversion(fileName, "_src.bin");//создание имени бинарного файла "Файл_src.bin"

    if (image.format() != QImage::Format_Grayscale8 && image.format() != QImage::Format_Mono && image.format() != QImage::Format_MonoLSB)//если изображение цветное, то дополнительное форматирование
        image = image.convertToFormat(QImage::Format_RGB16); //форматирование цаетных изображений из 24 в 16-битное представление
    QByteArray arrayImage = formatImage(image);
    if (!saveImage(arrayImage, newFilename))
    {
        qDebug() << "Не удалось сохранить бинарный файл _src.bin!";
        return;
    }
    newFilename = filenameConversion(fileName, "_data.bin");
    arrayImage = formatData(arrayImage);
    if (!saveImage(arrayImage, newFilename))
    {
        qDebug() << "Не удалось сохранить бинарный файл _data.bin!";
        return;
    }
//    //форматирование каждого пикселя по протоколу
//    image = formatData(image, newFilename);
//    //добавить проверку что файл вообще есть и он не пустой
//    serialPortManager->sendFile(newFilename);
//    newFilename = filenameConversion(fileName, "_res_data.bin");
//    serialPortManager->receiveFile(newFilename);
//    emit toMain("вывод текста");
}

//обратное преобразование
void ImageProcessing::reverseProcessImage(const QString &filePath)
{
    //??
}

bool ImageProcessing::transferSettings(QString portName, int baudRate, QString stopBits, QString parity, QString dataBits)
{
    bool result = false;
    if (serialPortManager->settingsOnComPort(portName, baudRate, stopBits, parity, dataBits))
        result = true;
    return result;
}

void ImageProcessing::transferToClosePort()
{
    serialPortManager->closePort();
}

bool ImageProcessing::checkOpenPort()
{
    bool result = false;
    if (serialPortManager->testOpenPort())
        result = true;
    return result;
}

bool ImageProcessing::sendMessage(QByteArray byteArray)
{
    bool result = false;
    if (serialPortManager->sendData(byteArray))
        result = true;
    return result;
}

void ImageProcessing::receiveSerialErrorSignal(QString errorsFromSerial)
{
    emit printError(errorsFromSerial);
}

void ImageProcessing::receiveSerialSignal(QByteArray byteArray)
{
    emit serialReceivedSignal(byteArray);
}

//сохранение файлов после преобразования из 24-ричного представления в 16-ричное
bool ImageProcessing::saveImage(QByteArray image, const QString &newFileName)
{
    bool flag = false;
    QFile outputFile(newFileName);
    if (outputFile.open(QIODevice::WriteOnly))
    {
        //сообщение об ошибке
        flag = true;
        outputFile.write(image);
        outputFile.close();
    }
    return flag;
}


//представление цветных изображений в 16-ричном формате
QByteArray ImageProcessing::formatImage(const QImage &image)
{
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    for (int y = 0; y<image.height(); y++)
    {
        for (int x = 0; x<image.width(); x++)
        {
            QRgb pixel = image.pixel(x,y);
            QColor color(pixel);
            if (image.format() == QImage::Format_RGB16)
            {
                unsigned short r = color.red() >> 3;
                unsigned short g = color.green() >> 2;
                unsigned short b = color.blue() >> 3;

                unsigned short convertedPixel = (r << 11) | (g << 5) | b;

                stream.writeRawData(reinterpret_cast<const char*>(&convertedPixel), sizeof(convertedPixel));
            }
            else
            {
                unsigned char grayscale = color.black();
                stream.writeRawData(reinterpret_cast<const char*>(&grayscale), sizeof(grayscale));
            }
        }
    }
    QRgb pixel = image.pixel(0,0);
    int colorPixel = (qRed(pixel) >> 3) << 11 | (qGreen(pixel) >> 2) << 5 | (qBlue(pixel) >> 3);
    qDebug() << QString("%1").arg(pixel,0,16);
    qDebug() << QString("%1").arg(colorPixel,0,16);
    return byteArray;
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
QByteArray ImageProcessing::formatData(const QByteArray &image)
{
    int dataSize = image.size();
    QByteArray expandedImage;
    for (int i = 0; i < dataSize; i+=4)
    {
        QByteArray data = image.mid(i, 4);

        data.resize(8);
        for (int j = 0; j < 4; j++)
        {
            char c = 0;
            data[4+j] = c;
        }
        expandedImage.append(data);
    }

    return expandedImage;
}


