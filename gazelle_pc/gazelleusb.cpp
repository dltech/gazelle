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
}

gazelleUsb::~gazelleUsb()
{
    gazellePort->close();
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
    uint8_t data[pageSize[0]];
    addr = 0;
    binary->seek(0);
    prepareWrite();
    while((!binary->atEnd()) && (addr < flashSize[type])) {
        binary->read((char*)data, pageSize[type]);
        writePage(addr, data);
        addr += pageSize[type];
    }
    return 0;
}

int gazelleUsb::prepareWrite()
{
    switch (type) {
    case 0:
        return 0;
        break;
    case 1:
        return eraseSpiFlash();
        break;
    default:
        break;
    }
    return -2;
}


int gazelleUsb::writePageI2c(uint32_t addr, uint8_t *data)
{
    int ptr = 0;
    std::copy(writeCmd[type], writeCmd[type]+cfgStrSize, pack);
    ptr = cfgStrSize;
    pack[ptr++] = 0x00;
    pack[ptr++] = (uint8_t)(addr>>8);
    pack[ptr++] = (uint8_t)addr;
    pack[ptr++] = pageSize[type];
    std::copy(data, data+pageSize[type], pack+ptr);
    gazellePort->write((const char*)pack, pageSize[type]+ptr);
    if( !gazellePort->waitForBytesWritten(10) ) {
        return -1;
    }
    if( !gazellePort->waitForReadyRead(10) ) {
        return -1;
    }
    gazellePort->read((char*)pack, cfgStrSize);
    gazellePort->readAll();
    if(std::equal(pack, pack+cfgStrSize, noAckMsg)) {
        return -2;
    }
    if(std::equal(pack, pack+cfgStrSize, okMsg)) {
        return 0;
    }
    return -2;
}

int gazelleUsb::writePageSpi(uint32_t addr, uint8_t *data)
{
    int ptr = 0;
    std::copy(writeCmd[type], writeCmd[type]+cfgStrSize, pack);
    ptr = cfgStrSize;
    pack[ptr++] = (uint8_t)(addr>>16);
    pack[ptr++] = (uint8_t)(addr>>8);
    pack[ptr++] = (uint8_t)addr;
    pack[ptr++] = pageSize[type];
    std::copy(data, data+pageSize[type], pack+ptr);
    gazellePort->write((const char*)pack, pageSize[type]+headerSize);
    if( !gazellePort->waitForBytesWritten(10) ) {
        return -1;
    }
    if( !gazellePort->waitForReadyRead(10) ) {
        return -1;
    }
    gazellePort->read((char*)pack, cfgStrSize);
    gazellePort->readAll();
    if(std::equal(pack, pack+cfgStrSize, spiError)) {
        return -2;
    }
    if(std::equal(pack, pack+cfgStrSize, okMsg)) {
        return 0;
    }
    return -2;
}

int gazelleUsb::eraseSpiFlash()
{
    std::copy(eraseCmd, eraseCmd+cfgStrSize, pack);
    gazellePort->write((const char*)pack, cfgStrSize);
    if( !gazellePort->waitForBytesWritten(10) ) {
        return -1;
    }
    if( !gazellePort->waitForReadyRead(10) ) {
        return -1;
    }
    gazellePort->read((char*)pack, cfgStrSize);
    gazellePort->readAll();
    if(std::equal(pack, pack+cfgStrSize, spiError)) {
        return -2;
    }
    if(std::equal(pack, pack+cfgStrSize, okMsg)) {
        return 0;
    }
    return -2;
}

int gazelleUsb::writePage(uint32_t addr, uint8_t *data)
{
    switch (type) {
    case 0:
        return writePageI2c(addr, data);
        break;
    case 1:
        return writePageSpi(addr, data);
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
        return readDumpSpi();
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
        readPageI2c(addr, data);
        addr += pageSize[type];
        outputFile->write((char*)data, pageSize[type]);
    }
    return 0;
}

int gazelleUsb::readDumpSpi()
{
    uint32_t ptr = 0;
    std::copy(readCmd[type], readCmd[type]+cfgStrSize, pack);
    ptr = cfgStrSize;
    pack[ptr++] = 0x00;
    pack[ptr++] = 0x00;
    pack[ptr++] = 0x00;
    pack[ptr++] = 0x00;
    gazellePort->write((const char*)pack, headerSize);
    if( !gazellePort->waitForBytesWritten(1000) ) {
        return -4;
    }

    if( !gazellePort->waitForReadyRead(1000) ) {
        return -5;
    }
    int obtained;
    obtained = gazellePort->read((char*)pack, pageSize[type]);
    qDebug() << pack[0] << pack[1] << pack[2] << pack[3] << pack[4] << pack[5] << pack[6] << pack[7];
    if(std::equal(pack, pack+cfgStrSize-1, spiError)) {
        return -2;
    }
    ptr = 0;
    while( ptr < flashSize[type] ) {
        outputFile->write((char*)pack, obtained);
        ptr += obtained;
        if( !gazellePort->waitForReadyRead(1000) ) {
            return -6;
        }
        obtained = gazellePort->read((char*)pack, pageSize[type]);
        qDebug() << ptr;
    }
    return 0;
}

int gazelleUsb::readPageI2c(uint32_t addr, uint8_t *data)
{
    int ptr = 0;
    switch (type) {
    case 0:
        std::copy(readCmd[type], readCmd[type]+cfgStrSize, pack);
        ptr = cfgStrSize;
        pack[ptr++] = 0x00;
        pack[ptr++] = (uint8_t)(addr>>8);
        pack[ptr++] = (uint8_t)addr;
        pack[ptr++] = (uint8_t)pageSize[type];
        gazellePort->write((const char*)pack, headerSize);
        if( !gazellePort->waitForBytesWritten(10) ) {
            return -1;
        }
        if( !gazellePort->waitForReadyRead(10) ) {
            return -1;
        }
        gazellePort->read((char*)pack, pageSize[type]);
        gazellePort->readAll();
        if(std::equal(pack, pack+cfgStrSize-1, noAckMsg)) {
            return -2;
        }
        std::copy(pack, pack + pageSize[type], data);
        return 0;
        break;
    case 1:
        return -3;
        break;
    default:
        break;
    }
    return -2;
}
