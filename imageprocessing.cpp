#include "imageprocessing.h"

#include <QDebug>
#include <QFileInfo>
#include <QObject>
#include <QPixmap>
#include "mainwindow.h"
#include "serialportmanager.h"

ImageProcessing::ImageProcessing()
{
    serialPortManager.reset(new SerialPortManager);
    //установка соединения между сигналом ошибки serialErrorSignal и слотом receiveSerialErrorSignal
    QObject::connect(serialPortManager.get(), &SerialPortManager::serialErrorSignal,
    this, &ImageProcessing::receiveSerialErrorSignal);
    //установка соединения между сигналом чтения получаемых данных serialReceiveSignal и слотом receiveSerialSignal
    QObject::connect(serialPortManager.get(), &SerialPortManager::serialReceiveSignal,
    this, &ImageProcessing::receiveSerialSignal);
}


ImageProcessing:: ~ImageProcessing()
{
    qDebug()<<"!!";
}

//--------------------------------------------------
//Преобразование изображения для отправки на ком-порт
//
//  вх: const QString &filePath - путь к файлу (изображение .bmp)
//  выход: QByteArray arrayImage - массив байт преобразованного изображения
//                                 для отправки на COM-port
//
//--------------------------------------------------
QByteArray ImageProcessing::processImage(const QString &filePath, bool result)
{
    QImage image(filePath);
    if (image.isNull())
    {
        qDebug() << "Не удалось открыть изображение";
        return "Error";
    }

    fileName = highlightingTheFileName(filePath); //выделение имени файла
    QString newFilename = filenameConversion(fileName, "_under_src.bin");//создание имени бинарного файла "Файл_src.bin"
    QByteArray arrayImage = underFormatImage(image);
    if (!saveImage(arrayImage, newFilename))
    {
        qDebug() << "Не удалось сохранить бинарный файл _under_src.bin!";
        return "Error";
    }

    newFilename = filenameConversion(fileName, "_src.bin");//создание имени бинарного файла "Файл_src.bin"
    arrayImage = formatImage(image, result);
    if (!saveImage(arrayImage, newFilename))
    {
        qDebug() << "Не удалось сохранить бинарный файл _src.bin!";
        return "Error";
    }
    newFilename = filenameConversion(fileName, "_data.bin");
    arrayImage = formatData(arrayImage);
    if (!saveImage(arrayImage, newFilename))
    {
        qDebug() << "Не удалось сохранить бинарный файл _data.bin!";
        return "Error";
    }
    return arrayImage;
}


//--------------------------------------------------
//Преобразование массива данных в изображение полученного с ком-порта
//
//  вх: QByteArray array - массив байт полученный с COM-port
//  выход: нет
//
//--------------------------------------------------
//обратное преобразование
void ImageProcessing::reverseProcessImage(QByteArray array, bool result)
{

    QString newFilename = filenameConversion(fileName, "_res_data.bin");
    if (!saveImage(array, newFilename))
    {
        qDebug() << "Не удалось сохранить бинарный файл _res_data.bin!";

    }
    array = reFormatData(array);
    newFilename = filenameConversion(fileName, "_res.bin");
    if (!saveImage(array, newFilename))
    {
        qDebug() << "Не удалось сохранить бинарный файл _res.bin!";

    }
    newFilename = filenameConversion(fileName, "_res.bmp");
    array = createImage(array, newFilename, result);



    QImage image(newFilename);
    array = underFormatImage(image);
    newFilename = filenameConversion(fileName, "_after_res.bin");
    if (!saveImage(array, newFilename))
    {
        qDebug() << "Не удалось сохранить бинарный файл _after_res.bin!";
    }
}


//--------------------------------------------------
//Передача настроек Com-port из класса MainWindow в класс SerialPortManager
//
//  вх: QString portName - имя ком-порта,
//      int baudRate - скорость ком-порта
//      QString stopBits - стоп-биты
//      QString parity - четность
//      QString dataBits - дата биты
//  выход: bool result - результат отправки настроек на ком-порт
//
//--------------------------------------------------
bool ImageProcessing::transferSettings(QString portName, int baudRate, QString stopBits, QString parity, QString dataBits)
{
    bool result = false;
    if (serialPortManager->settingsOnComPort(portName, baudRate, stopBits, parity, dataBits))
        result = true;
    return result;
}


//--------------------------------------------------
//Передача сообщения о закрытии Com-port из класса MainWindow в класс SerialPortManager
//  вх: нет
//  выход: нет
//
//--------------------------------------------------
void ImageProcessing::transferToClosePort()
{
    serialPortManager->closePort();
}

//--------------------------------------------------
// Проверка открытия Com-port, передача сообщения из класса MainWindow в класс SerialPortManager
//  вх: нет
//  выход: bool result - результат открытия Com-port
//
//--------------------------------------------------
bool ImageProcessing::checkOpenPort()
{
    bool result = false;
    if (serialPortManager->testOpenPort())
        result = true;
    return result;
}

//--------------------------------------------------
// Передача сообщения на Com-port из класса MainWindow в класс SerialPortManager
//  вх: QByteArray byteArray - сообщение для отправки на ComPort
//  выход: bool result - результат передачи сообщения на Com-port
//
//--------------------------------------------------
bool ImageProcessing::sendMessage(QByteArray byteArray)
{
    bool result = false;
    if (serialPortManager->sendData(byteArray))
        result = true;
    return result;
}

void ImageProcessing::resetArray()
{
    serialPortManager->resetArr();
}

bool ImageProcessing::comparisonData()
{
    bool result;
    result = serialPortManager->receiveData();
    return result;
}

//--------------------------------------------------
//  Передача сообщения об ошибке, связанной с Com-port
//  из класса SerialPortManager в класс MainWindow
//  вх: нет
//  выход: нет
//
//--------------------------------------------------
void ImageProcessing::receiveSerialErrorSignal(QString errorsFromSerial)
{
    emit printError(errorsFromSerial);
}

//--------------------------------------------------
//  Передача сообщения о полученных данных с Com-port
//  из класса SerialPortManager в класс MainWindow
//  вх: нет
//  выход: нет
//
//--------------------------------------------------
void ImageProcessing::receiveSerialSignal(QByteArray byteArray)
{
    emit serialReceivedSignal(byteArray);
}


//--------------------------------------------------
//  Сохранение массива данных в бинарный файл
//  вх: QByteArray image - массив данных
//      const QString &newFileName - путь к бинарному файлу
//
//  выход: bool result - результат сохранения в бинарный файл
//
//--------------------------------------------------
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

//--------------------------------------------------
//  Преобразование 24-ричных изображений в 16-ричное изображение
//  и 8-ричное изображение
//  вх: const QImage &image - исходное изображение
//      bool result - в какой формат переводить изображение (8-ричный = false,
//                    16-ричный = true)
//  выход: QByteArray byteArray - преобразованный массив данных изображения
//
//--------------------------------------------------
QByteArray ImageProcessing::formatImage(QImage &image, bool result)
{
    QByteArray byteArray;
    heightImage = image.height();
    widthImage = image.width();

    if (!result)
    {
        for (int y = 0; y < image.height(); y++)
        {
            for (int x = 0; x < image.width(); x++)
            {
                QRgb pixel = image.pixel(x, y);
                uchar value = qGreen(pixel);
                byteArray.append(value);
            }
        }
    }
    else
    {
        for (int y = 0; y < image.height(); ++y)
        {
            for (int x = 0; x < image.width(); ++x)
            {
                QColor pixel = image.pixel(x, y);
                int r = pixel.red();
                int g = pixel.green();
                int b = pixel.blue();
                ushort red = (r >> 3) & 0x1F;
                ushort green = (g >> 2) & 0x3F;
                ushort blue = (b >> 3 ) & 0x1F;
                ushort rgb565 = 0;
                rgb565 = (red << 11) | (green << 5) | blue;
                QByteArray num;
                QByteArray temp_num = num.setNum(rgb565,16);

                QByteArray byte_pixel = QByteArray::fromHex(temp_num);

                if (byte_pixel.size() < 2)
                    byte_pixel.prepend('\x00');
                byteArray.append(byte_pixel);
            }
        }
    }
    return byteArray;
}

//--------------------------------------------------
//  Отсечение расширения .bmp от пути файла
//
//  вх: const QString &filePath - полный путь до файла
//  выход: const QString &filePath - путь к файлу без расширения .bmp
//
//--------------------------------------------------
QString ImageProcessing::highlightingTheFileName(const QString &filePath)
{
    int dotIndex = filePath.lastIndexOf(".");
    if (dotIndex != -1) {
        return filePath.left(dotIndex);
    }
    return filePath;
}

//--------------------------------------------------
//  Добавление суффикса к имени файла
//
//  вх: const QString &fileName - имя файла
//      const QString &suffix - суффикс, который необходимо добавить к имени файла
//  выход: const QString - полученное имя файла
//
//--------------------------------------------------
QString ImageProcessing::filenameConversion(const QString &filename, const QString &suffix)
{
    return  filename + suffix;
}

QByteArray ImageProcessing::underFormatImage(QImage image)
{
    QByteArray byteArray;
    heightImage = image.height();
    widthImage = image.width();

    for (int y = 0; y < image.height(); y++)
    {
        for (int x = 0; x < image.width(); x++)
        {
            QRgb pixel = image.pixel(x, y);
            int r = qRed(pixel);
            int g = qGreen(pixel);
            int b = qBlue(pixel);
            quint32 color = qRgb(r, g, b);
            QByteArray num;
            QByteArray temp_num = num.setNum(color,16);
            QByteArray byte_pixel = QByteArray::fromHex(temp_num);
            byteArray.append(byte_pixel);
        }
    }
    return byteArray;
}

//--------------------------------------------------
//  Преобразование пикселей по протоколу, добавление нулей
//
//  вх: const QByteArray &image - массив данных изображения
//  выход: const QByteArray &image - преобразованный по протоколу массив данных
//
//--------------------------------------------------
QByteArray ImageProcessing::formatData(const QByteArray &image)
{
    int dataSize = image.size();
    QByteArray expandedImage;
    for (int i = 0; i < dataSize; i+=4)
    {
        QByteArray data1 = image.mid(i, 4);
        if (data1.size()!=4)
            Size = data1.size();
        QByteArray data;
        for (int j = 0; j < 4; j++)
        {
            data.append('\x00');
        }
        data.append(data1);
        if (data.size() < 8)
        {
            data1.remove(0,data1.size());
            for (int i=0;i<8-data.size();i++)
                data1.append('\x00');
            data1.append(data);
            data.remove(0,data1.size());
            data = data1;
        }

        expandedImage.append(data);
    }
    return expandedImage;
}

QByteArray ImageProcessing::reFormatData(QByteArray &image)
{
    int count = image.size()/8;
    QByteArray resultArray;
    if (Size!=0)
        count--;
    for (int i=0; i<count; i++)
    {
        QByteArray group = image.mid(i*8, 8);
        QByteArray firstFour = group.right(4);
        resultArray.append(firstFour);
    }
    if (Size!=0)
    {
        int i = count++;
        QByteArray group = image.mid(i*8, 8);
        QByteArray firstFour = group.right(Size);
        resultArray.append(firstFour);
    }
    Size = 0;
    return resultArray;
}

QByteArray ImageProcessing::createImage(QByteArray array, QString filename, bool result)
{
    QImage imageNew(widthImage, heightImage, QImage::Format_RGB32);
    QByteArray arrayImage;
    if (!result)
    {
        //8-ричное
        for (int i = 0; i < heightImage; i++)
        {
            for (int j = 0; j < widthImage; j++)
            {
                int pixelIndex;
                pixelIndex = i * widthImage + j;
                uchar pixelValue = array.at(pixelIndex);
                uchar red = pixelValue;
                uchar green, blue;
                blue = red;
                green = red;
                QColor color;
                color.setRgb(red, green, blue);
                imageNew.setPixelColor(j,i,color);
            }
        }
    }
    else
    {
        //16-ричное
        for (int i = 0; i < heightImage; i++)
        {
            for (int j = 0; j < widthImage; j++)
            {
                int pixelIndex;
                pixelIndex = i * widthImage*2 + j*2;
                QByteArray newByteArray = array.mid(pixelIndex, 2);
                QByteArray byte = newByteArray.toHex();
                ushort pixelValue = static_cast<ushort>(byte.toInt(nullptr,16));
                uchar red = static_cast<uchar>(((pixelValue  >> 11) & 0x001F)<<3);
                uchar green = static_cast<uchar>(((pixelValue >> 5) & 0x003F)<<2);
                uchar blue = static_cast<uchar>((pixelValue  & 0x001F) << 3);
                QColor color;
                color.setRgb(red, green, blue);

                imageNew.setPixelColor(j,i,color);
            }
        }
    }
    imageNew.save(filename);
    return arrayImage;
}

