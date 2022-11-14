#include "gazelleusb.h"
#include "gazellerequest.h"

#include <QTimer>


gazelleUsb::gazelleUsb(QObject *parent)
    : QObject{parent}
{
    flashThread =  new QThread();
    flashWorker = new GazelleUsbWorker();
    flashWorker->moveToThread(flashThread);
//    flashWorker->gazellePort->moveToThread(flashThread);
    flashThread->start();
//    flashWorker->createConnection();
    connect(this,SIGNAL(runRead()),flashWorker,SLOT(readDumpWorker()));
    connect(this,SIGNAL(runWrite()),flashWorker,SLOT(writeDumpWorker()));
    connect(flashWorker,SIGNAL(readIsComplete()),this,SIGNAL(readComplete()));
    connect(flashWorker,SIGNAL(writeIsComplete()),this,SIGNAL(writeComplete()));
    connect(flashWorker,SIGNAL(yetAnotherEvent()),this,SIGNAL(anotherEvent()));
    connect(this,SIGNAL(createWorker()),flashWorker,SLOT(createConnection()));
    connect(this,SIGNAL(destroyWorker()),flashWorker,SLOT(destroyConnection()));
    emit createWorker();
}

gazelleUsb::~gazelleUsb()
{
    emit destroyWorker();
    flashThread->quit();
    flashThread->wait(100);
    delete flashWorker;
    delete flashThread;
}

QString* gazelleUsb::getMessage()
{
    return flashWorker->message;
}

int gazelleUsb::getProgress()
{
    if(flashWorker->isReadingSpi) {
        return flashWorker->flasher->getReadSpiProgress();
    }
    return flashWorker->progress;
}

void gazelleUsb::setFlashType(int inType)
{
    flashWorker->type = inType;

}

int gazelleUsb::getFlashType()
{
    return flashWorker->type;
}

void gazelleUsb::writeDump(QFile *bin)
{
    flashWorker->binary = bin;
    emit runWrite();
}

void gazelleUsb::readDump(QFile *bin)
{
    flashWorker->binary = bin;
    emit runRead();
}

