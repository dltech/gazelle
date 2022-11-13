#ifndef GAZELLEUSB_H
#define GAZELLEUSB_H

#include <QObject>
#include <QSerialPort>
#include <QFileDialog>

class gazelleUsb : public QObject
{
    Q_OBJECT

public:
    gazelleUsb(QObject *parent = 0);
    ~gazelleUsb();
    int getFlashType();
    void setFlashType(int inType);

    int prepareWrite(void);
    int writePage(uint32_t address, uint8_t *data);
    int readPageI2c(uint32_t address, uint8_t *data);

    int writeDump(QFile *binary);
    int readDump(QFile *binary);

private:
    int eraseSpiFlash(void);
    int writePageI2c(uint32_t address, uint8_t *data);
    int writePageSpi(uint32_t address, uint8_t *data);
    int readDumpI2c();
    int readDumpSpi();

    static constexpr int nTypes = 2;
    static constexpr int cfgStrSize = 8;
    static constexpr int headerSize = cfgStrSize + 4;
    static constexpr uint8_t eraseCmd[cfgStrSize] = {'w','2','5','q','x','x','e','r'};
    static constexpr uint8_t writeCmd[nTypes][cfgStrSize] = {{'m','2','4','c','x','x','w','r'},
                                                             {'w','2','5','q','x','x','w','r'} };
    static constexpr uint8_t readCmd[nTypes][cfgStrSize] = {{'m','2','4','c','x','x','r','d'},
                                                            {'w','2','5','q','x','x','r','d'} };
    static constexpr uint8_t okMsg[cfgStrSize] = {'o','k',' ',' ',' ',' ',' ',0};
    static constexpr uint8_t noAckMsg[cfgStrSize] = {'n','o',' ','a','c','k',' ',0};
    static constexpr uint8_t spiError[cfgStrSize] = {'s','p','i',' ','e','r','r',0};

    static constexpr int pageSize[nTypes] = {32,256};
    static constexpr uint32_t flashSize[nTypes] = {0x2000,0x7fffff};
    int type;
    uint32_t addr;
    static constexpr int maxPacketSize = 256+headerSize;
    static constexpr int stmUsbPackSize = 64;
    uint8_t pack[maxPacketSize];
    QSerialPort *gazellePort;
    QFile *outputFile;
};

#endif // GAZELLEUSB_H
