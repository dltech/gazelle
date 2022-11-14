#include "gazelleusbworker.h"
#include <QSerialPortInfo>
#include <QDebug>

GazelleUsbWorker::GazelleUsbWorker(QObject *parent)
    : QObject{parent}
{
    message = new QString("not connected");
}

GazelleUsbWorker::~GazelleUsbWorker()
{
    delete message;
    delete flasher;
}

void GazelleUsbWorker::createConnection(void)
{
    // port settings
    gazellePort = new QSerialPort(0);
    QList<QSerialPortInfo> list;
    list = QSerialPortInfo::availablePorts();
    for( int i=0 ; i<list.length() ; ++i) {
        if( list[i].portName().contains("ACM") ) {
            gazellePort->setPortName(list[i].portName());
            message->clear();
            message->append("connected ");
            message->append(list[i].portName());
        }
    }
    gazellePort->setBaudRate(9600,QSerialPort::Input);
    if( !gazellePort->open(QIODevice::ReadWrite) ) {
        message->clear();
        message->append("port error");
    }
    flasher = new gazelleRequest(gazellePort);
    emit yetAnotherEvent();
}

void GazelleUsbWorker::destroyConnection(void)
{
    gazellePort->close();
    delete gazellePort;
}

void GazelleUsbWorker::writeDumpWorker()
{
    progress = 0;
    uint8_t data[pageSize[type]];
    int addr = 0;
    binary->seek(0);
    int err = prepareWrite();
    message->clear();
    message->append("writing dump of ");
    message->append(flashName[type]);
    while((!binary->atEnd()) && (addr < flashSize[type]) && (err >= 0)) {
        binary->read((char*)data, pageSize[type]);
        err = writePage(addr, data);
        addr += pageSize[type];
        progress = addr*100/flashSize[type];
    }
    if(err == -1) {
        message->clear();
        message->append("programmator not found");
    }
    if(err == -2) {
        message->clear();
        message->append(flashName[type]);
        message->append(" not connected");
    }
    if(err == -4) {
        message->clear();
        message->append("programmator failed operation");
    }
    if(err == -3) {
        message->clear();
        message->append("failed to write all dump");
    }
    if(err == 0) {
        message->clear();
        message->append(flashName[type]);
        message->append(" written successfully");
    }
    emit writeIsComplete();
}

int GazelleUsbWorker::prepareWrite()
{
    switch (type) {
    case 0:
        return 0;
        break;
    case 1:
        message->clear();
        message->append("erasing");
        return flasher->eraseSpi();
        break;
    default:
        break;
    }
    return -2;
}


int GazelleUsbWorker::writePage(uint32_t addr, uint8_t *data)
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

void GazelleUsbWorker::readDumpWorker()
{
    progress = 0;
    message->clear();
    message->append("reading dump of ");
    message->append(flashName[type]);
    int err = -10;
    switch (type) {
    case 0:
        err = readDumpI2c();
        break;
    case 1:
        isReadingSpi = true;
        err = flasher->readAllSpi(binary);
        isReadingSpi = false;
        break;
    default:
        break;
    }

    if(err == -1) {
        message->clear();
        message->append("programmator not found");
    }
    if(err == -2) {
        message->clear();
        message->append(flashName[type]);
        message->append(" not connected");
    }
    if(err == -4) {
        message->clear();
        message->append("programmator failed operation");
    }
    if(err == -3) {
        message->clear();
        message->append("failed to read all dump");
    }
    if(err == 0) {
        message->clear();
        message->append(flashName[type]);
        message->append(" readed successfully");
    }
    emit readIsComplete();
}

int GazelleUsbWorker::readDumpI2c()
{
    uint8_t data[pageSize[0]];
    int addr = 0;
    int err = 0;
    while((addr < flashSize[type]) && (err>=0)) {
        err = flasher->readI2cPage(data, addr);
        addr += pageSize[type];
        binary->write((char*)data, pageSize[type]);
        progress = addr*100/flashSize[type];
    }
    if(err<=0) return err;
    return 0;
}
