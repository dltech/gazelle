#ifndef GAZELLEREQUEST_H
#define GAZELLEREQUEST_H

#include <QObject>
#include <QSerialPortInfo>
#include <QFile>



class gazelleRequest
{
public:
    gazelleRequest(QSerialPort *port);
    int readAllSpi(QFile *output);
    int readI2cPage(uint8_t *data, uint32_t addr);
    int writeSpiPage(uint8_t *data, uint32_t addr);
    int writeI2cPage(uint8_t *data, uint32_t addr);
    int eraseSpi(void);

private:
    QSerialPort *gazellePort;
    static constexpr int typicalDelay = 10;
    static constexpr int longDelay = 1000;

    void makeRequest(const uint8_t *msg, uint32_t addr = 0, uint16_t payloadSize = 0, uint8_t *data = NULL);
    bool isResponse(const uint8_t *msg);
    static constexpr uint32_t i2cFlashSize = 0x2000;
    static constexpr uint32_t spiFlashSize = 0x7fffff;
    static constexpr int i2cPageSize = 32;
    static constexpr int spiPageSize = 256;
    static constexpr int stmUsbPackSize = 64;
    static constexpr int cfgStrSize = 8;
    static constexpr int headerSize = cfgStrSize + 4;
    static constexpr int maxPacketSize = 256+headerSize;
    uint8_t pack[maxPacketSize];
    int packSize = 0;
    static constexpr uint8_t eraseCmd[cfgStrSize]    = {'w','2','5','q','x','x','e','r'};
    static constexpr uint8_t writeI2cCmd[cfgStrSize] = {'m','2','4','c','x','x','w','r'};
    static constexpr uint8_t writeSpiCmd[cfgStrSize] = {'w','2','5','q','x','x','w','r'};
    static constexpr uint8_t readI2cCmd[cfgStrSize]  = {'m','2','4','c','x','x','r','d'};
    static constexpr uint8_t readSpiCmd[cfgStrSize]  = {'w','2','5','q','x','x','r','d'};
    static constexpr uint8_t okMsg[cfgStrSize]       = {'o','k',' ',' ',' ',' ',' ',0};
    static constexpr uint8_t noAckMsg[cfgStrSize]    = {'n','o',' ','a','c','k',' ',0};
    static constexpr uint8_t spiError[cfgStrSize]    = {'s','p','i',' ','e','r','r',0};
};

#endif // GAZELLEREQUEST_H
