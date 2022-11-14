#ifndef GAZELLEUSBWORKER_H
#define GAZELLEUSBWORKER_H

#include <QObject>
#include <QFile>
#include <QSerialPort>
#include "gazellerequest.h"

class GazelleUsbWorker : public QObject
{
    Q_OBJECT
public:
    explicit GazelleUsbWorker(QObject *parent = nullptr);
    ~GazelleUsbWorker(void);

    int progress = 0;
    QString *message;
    QFile *binary;
    gazelleRequest *flasher;
    bool isReadingSpi = false;
    int type;
    QSerialPort *gazellePort;

public slots:
    void writeDumpWorker(void);
    void readDumpWorker(void);
    void createConnection(void);
    void destroyConnection(void);
signals:
    void readIsComplete(void);
    void writeIsComplete(void);
    void yetAnotherEvent(void);
private:
    int prepareWrite(void);
    int writePage(uint32_t address, uint8_t *data);
    int readDumpI2c(void);

    static constexpr int nTypes = 2;
    static constexpr int pageSize[nTypes] = {32,256};
    static constexpr uint32_t flashSize[nTypes] = {0x2000,0x800000};
    static constexpr char flashName[nTypes][8] = {{"m24c64"},{"w25q64"}};
};

#endif // GAZELLEUSBWORKER_H
