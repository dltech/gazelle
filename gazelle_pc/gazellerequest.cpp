#include "gazellerequest.h"
#include "qserialport.h"
#include <QSerialPortInfo>
#include <QDebug>


gazelleRequest::gazelleRequest(QSerialPort *port)
{
    gazellePort = port;
}

void gazelleRequest::makeRequest(const uint8_t *msg, uint32_t addr, uint16_t payloadSize, uint8_t *data)
{
    std::copy(msg, msg+cfgStrSize, pack);
    int ptr = cfgStrSize;
    pack[ptr++] = (uint8_t)(addr>>16);
    pack[ptr++] = (uint8_t)(addr>>8);
    pack[ptr++] = (uint8_t)addr;
    pack[ptr++] = (uint8_t)(payloadSize>>8);
    pack[ptr++] = (uint8_t)payloadSize;
    packSize = headerSize;
    if(data != NULL) {
        std::copy(data, data+payloadSize, pack+headerSize);
        packSize += payloadSize;
    }
}

bool gazelleRequest::isResponse(const uint8_t *msg)
{
    if(std::equal(pack, pack+cfgStrSize, msg)) {
        return true;
    }
    return false;
}

int gazelleRequest::getReadSpiProgress()
{
    return readSpiProgress;
}

int gazelleRequest::readAllSpi(QFile *output)
{
    makeRequest(readSpiCmd);
    gazellePort->write((const char*)pack, packSize);
    if( !gazellePort->waitForBytesWritten(typicalDelay) ) {
        return -1;
    }
    if( !gazellePort->waitForReadyRead(typicalDelay) ) {
        return -4;
    }
    int obtained = gazellePort->read((char*)pack, maxPacketSize);
    gazellePort->readAll();
    if( isResponse(spiError) ) {
        return -2;
    }
    output->write((char*)pack, obtained);
    int ptr = obtained;
    int cnt = 0;
    while( ptr < spiFlashSize ) {
        ++cnt;
        if( !gazellePort->waitForReadyRead(typicalDelay) ) {
            return -4;
        }
        obtained = gazellePort->read((char*)pack, maxPacketSize);
        output->write((char*)pack, obtained);
        ptr += obtained;
        readSpiProgress = ptr*100/spiFlashSize;
    }
    if(ptr == spiFlashSize) {
        return 0;
    }
    return -3;
}

int gazelleRequest::readI2cPage(uint8_t *data, uint32_t addr)
{
    makeRequest(readI2cCmd, addr, i2cPageSize);
    gazellePort->write((const char*)pack, packSize);
    if( !gazellePort->waitForBytesWritten(typicalDelay) ) {
        return -1;
    }
    if( !gazellePort->waitForReadyRead(typicalDelay) ) {
        return -1;
    }
    gazellePort->read((char*)pack, i2cPageSize);
    gazellePort->readAll();
    std::copy(pack, pack + i2cPageSize, data);
    if( isResponse(noAckMsg) ) {
        return -2;
    }
    return 0;
}

int gazelleRequest::writeSpiPage(uint8_t *data, uint32_t addr)
{
    makeRequest(writeSpiCmd, addr, spiPageSize, data);
    gazellePort->write((const char*)pack, packSize);
    if( !gazellePort->waitForBytesWritten(typicalDelay) ) {
        return -1;
    }
    if( !gazellePort->waitForReadyRead(longDelay) ) {
        return -1;
    }
    gazellePort->read((char*)pack, cfgStrSize);
    gazellePort->readAll();
    if( isResponse(spiError) ) {
        return -2;
    }
    if( isResponse(okMsg) ) {
        return 0;
    }
    return -3;
}

int gazelleRequest::writeI2cPage(uint8_t *data, uint32_t addr)
{
    makeRequest(writeI2cCmd, addr, i2cPageSize, data);
    gazellePort->write((const char*)pack, packSize);
    if( !gazellePort->waitForBytesWritten(typicalDelay) ) {
        return -1;
    }
    if( !gazellePort->waitForReadyRead(typicalDelay) ) {
        return -1;
    }
    gazellePort->read((char*)pack, cfgStrSize);
    gazellePort->readAll();
    if( isResponse(noAckMsg) ) {
        return -2;
    }
    if( isResponse(okMsg) ) {
        return 0;
    }
    return -3;
}

int gazelleRequest::eraseSpi()
{
    makeRequest(eraseCmd);
    gazellePort->write((const char*)pack, packSize);
    if( !gazellePort->waitForBytesWritten(typicalDelay) ) {
        return -1;
    }
    if( !gazellePort->waitForReadyRead(longDelay) ) {
        return -4;
    }
    gazellePort->read((char*)pack, cfgStrSize);
    gazellePort->readAll();
    if( isResponse(spiError) ) {
        return -2;
    }
    if( isResponse(okMsg) ) {
        return 0;
    }
    return -3;
}
