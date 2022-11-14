#ifndef GAZELLEUSB_H
#define GAZELLEUSB_H

#include <QObject>
#include <QFile>
#include <QThread>
#include "gazelleusbworker.h"

class gazelleUsb : public QObject
{
    Q_OBJECT

public:
    explicit gazelleUsb(QObject *parent = nullptr);
    ~gazelleUsb();
    int getFlashType();
    void setFlashType(int inType);

    void writeDump(QFile *bin);
    void readDump(QFile *bin);

    QString* getMessage(void);
    int getProgress(void);
signals:
    void readComplete(void);
    void writeComplete(void);
    void anotherEvent(void);

private:
    GazelleUsbWorker *flashWorker;
    QThread *flashThread;

signals:
    void runWrite(void);
    void runRead(void);
    void createWorker(void);
    void destroyWorker(void);
};

#endif // GAZELLEUSB_H
