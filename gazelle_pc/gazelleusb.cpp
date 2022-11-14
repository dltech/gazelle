#include "gazelleusb.h"
#include <QSerialPortInfo>
#include <QDebug>

gazelleUsb::gazelleUsb(QObject *parent)
{
    // port settings
    gazellePort = new QSerialPort(this);
    QList<QSerialPortInfo> list;
    list = QSerialPortInfo::availablePorts();
    for( int i=0 ; i<list.length() ; ++i) {
        qDebug() << list[i].portName();
        if( list[i].portName().contains("ACM") ) {
            gazellePort->setPortName(list[i].portName());
        }
    }
    gazellePort->setBaudRate(9600,QSerialPort::Input);
    gazellePort->open(QIODevice::ReadWrite);
    flasher = new gazelleRequest(gazellePort);
}

gazelleUsb::~gazelleUsb()
{
    gazellePort->close();
    delete flasher;
}

void gazelleUsb::setFlashType(int inType)
{
    type = inType;
}

int gazelleUsb::getFlashType()
{
    return type;
}

int gazelleUsb::writeDump(QFile *binary)
{
    uint8_t data[pageSize[type]];
    addr = 0;
    binary->seek(0);
    int err = prepareWrite();
    while((!binary->atEnd()) && (addr < flashSize[type]) && (err >= 0)) {
        binary->read((char*)data, pageSize[type]);
        err = writePage(addr, data);
        addr += pageSize[type];
    }
    return err;
}

int gazelleUsb::prepareWrite()
{
    switch (type) {
    case 0:
        return 0;
        break;
    case 1:
        return flasher->eraseSpi();
        break;
    default:
        break;
    }
    return -2;
}


int gazelleUsb::writePage(uint32_t addr, uint8_t *data)
{
    switch (type) {
    case 0:
        return flasher->writeI2cPage(data, addr);
        break;
    case 1:
        return flasher->writeSpiPage(data, addr);
        break;
    default:
        break;
    }
    return -2;
}

int gazelleUsb::readDump(QFile *binary)
{
    outputFile = binary;
    switch (type) {
    case 0:
        return readDumpI2c();
        break;
    case 1:
        return flasher->readAllSpi(binary);
        break;
    default:
        break;
    }
    return -2;
}

int gazelleUsb::readDumpI2c()
{
    uint8_t data[pageSize[0]];
    addr = 0;
    while(addr < flashSize[type]) {
        flasher->readI2cPage(data, addr);
        addr += pageSize[type];
        outputFile->write((char*)data, pageSize[type]);
    }
    return 0;
}
