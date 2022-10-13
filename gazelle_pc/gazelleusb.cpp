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
//        qDebug() << list[i].portName();
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
    while((!binary->atEnd()) && (addr < flashSize[type])) {
        binary->read((char*)data, pageSize[type]);
        writePage(addr, data);
        addr += pageSize[type];
    }
    return 0;
}

int gazelleUsb::writePage(uint32_t addr, uint8_t *data)
{
    int ptr = 0;
    switch (type) {
    case 0:
        std::copy(writeCmd[type], writeCmd[type]+cfgStrSize, pack);
        ptr = cfgStrSize;
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
        break;
        case 1:
        break;
    default:
        break;
    }
    return -2;
}

int gazelleUsb::readDump(QFile *binary)
{
    uint8_t data[pageSize[0]];
    addr = 0;
    while(addr < flashSize[type]) {
        readPage(addr, data);
        addr += pageSize[type];
        binary->write((char*)data, pageSize[type]);
    }
    return 0;
}

int gazelleUsb::readPage(uint32_t addr, uint8_t *data)
{
    int ptr = 0;
    switch (type) {
    case 0:
        std::copy(readCmd[type], readCmd[type]+cfgStrSize, pack);
        ptr = cfgStrSize;
        pack[ptr++] = (uint8_t)(addr>>8);
        pack[ptr++] = (uint8_t)addr;
        pack[ptr++] = (uint8_t)pageSize[type];
        gazellePort->write((const char*)pack, ptr);
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
        break;
    default:
        break;
    }
    return -2;
}
