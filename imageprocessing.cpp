#include "imageprocessing.h"

#include <QDebug>
#include <QFileInfo>
#include <QObject>
#include "mainwindow.h"
#include "serialportmanager.h"

ImageProcessing::ImageProcessing()
{

    //    serialPortManager = new SerialPortManager();

//    connect(serialPortManager, SIGNAL(serialErrorSignal(QString)), this, SLOT(receiveSerialErrorSignal(QString)));
//    connect(serialPortManager,SIGNAL(serialNum(int)) , this, SLOT(myTmp(int)) );
    serialPortManager.reset(new SerialPortManager);

   // QObject::connect(serialPortManager.get(), SIGNAL(serialErrorSignal(QString)), this, SLOT(receiveSerialErrorSignal(QString)),  Qt::AutoConnection);
    QObject::connect(serialPortManager.get(), &SerialPortManager::serialErrorSignal,
    this, &ImageProcessing::receiveSerialErrorSignal    );
    QObject::connect(serialPortManager.get(), &SerialPortManager::serialReceiveSignal,
    this, &ImageProcessing::receiveSerialSignal);



   /// connect(serialPortManager.get(), SIGNAL(serialNum(int)) , this, SLOT(myTmp(int)) );
//    connect(serialPortManager, SIGNAL(portOpened(bool)), this, SLOT(handlePortOpened(bool)));
//    connect(mainWindow, SIGNAL(checkPortStatusSignal()), this, SLOT(checkPortStatusSlot()));
//    connect(serialPortManager, SIGNAL(portStatusSignal(bool)), this, SLOT(handlePortStatus(bool)));
}


ImageProcessing:: ~ImageProcessing()
{
    qDebug()<<"!!";
}



//void ImageProcessing::myTmp(int num)
//{
//    qDebug()<<"NUM="<<num;
//}
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
    qDebug() <<"format=" <<image.format();
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

//void ImageProcessing::checkPortStatusSlot(QString portName)
//{
//    bool portStatus = serialPortManager->checkPortStatus(portName);
//    emit portStatusSignal(portStatus);
//}

//void ImageProcessing::handlePortStatus(bool success)
//{
//    if (success)
//        qDebug() << "Port is Open";
//    else
//        qDebug() << "Port is Closed";


//}

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
//    QRgb pixel = image.pixel(0,0);

//    int colorPixel = (qRed(pixel) >> 3) << 11 | (qGreen(pixel) >> 2) << 5 | (qBlue(pixel) >> 3);

//    qDebug() << QString("%1").arg(pixel,0,16);

//    qDebug() << QString("%1").arg(colorPixel,0,16);
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


