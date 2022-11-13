#ifndef GAZELLEUSB_H
#define GAZELLEUSB_H

#include <QObject>
#include <QSerialPort>
#include <QFileDialog>
#include "gazellerequest.h"

class gazelleUsb : public QObject
{
    Q_OBJECT

public:
    gazelleUsb(QObject *parent = 0);
    ~gazelleUsb();
    int getFlashType();
    void setFlashType(int inType);

    int writeDump(QFile *binary);
    int readDump(QFile *binary);

private:
    int prepareWrite(void);
    int writePage(uint32_t address, uint8_t *data);
    int readDumpI2c();

    static constexpr int nTypes = 2;
    static constexpr int pageSize[nTypes] = {32,256};
    static constexpr uint32_t flashSize[nTypes] = {0x2000,0x7fffff};
    int type;
    uint32_t addr;
    QSerialPort *gazellePort;
    QFile *outputFile;
    gazelleRequest *flasher;
};

#endif // GAZELLEUSB_H
