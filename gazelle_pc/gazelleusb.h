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

    int writePage(uint32_t address, uint8_t *data);
    int readPage(uint32_t address, uint8_t *data);

    int writeDump(QFile *binary);
    int readDump(QFile *binary);

private:
    static constexpr int nTypes = 2;
    static constexpr int cfgStrSize = 8;
    static constexpr uint8_t writeCmd[nTypes][cfgStrSize] = {{'m','2','4','c','x','x','w','r'},
                                                             {'w','2','5','q','x','x','w','r'} };
    static constexpr uint8_t readCmd[nTypes][cfgStrSize] = {{'m','2','4','c','x','x','r','d'},
                                                            {'w','2','5','q','x','x','r','d'} };
    static constexpr uint8_t okMsg[cfgStrSize] = {'o','k',' ',' ',' ',' ',' ',0};
    static constexpr uint8_t noAckMsg[cfgStrSize] = {'n','o',' ','a','c','k',' ',0};

    static constexpr int pageSize[nTypes] = {32,256};
    static constexpr uint32_t flashSize[nTypes] = {0x2000,0xffffff};
    int type;
    uint32_t addr;
    static constexpr int stmUsbPackSize = 64;
    uint8_t pack[stmUsbPackSize];
    QSerialPort *gazellePort;
};

#endif // GAZELLEUSB_H
