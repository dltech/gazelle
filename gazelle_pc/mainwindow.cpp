#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // all objects of UI
    toolBar = new QToolBar(this);
    read = new QPushButton(this);
    write = new QPushButton(this);
    open = new QPushButton(this);
    save = new QPushButton(this);
    target = new QComboBox(this);
    mem = new QTextEdit(this);
    stBar = new QStatusBar(this);

    // configurating of UI
    // buttons config
    connect(read, SIGNAL(released()), this, SLOT(readFlash()));
    connect(write, SIGNAL(released()), this, SLOT(writeFlash()));
    connect(open, SIGNAL(released()), this, SLOT(openBin()));
    connect(save, SIGNAL(released()), this, SLOT(saveBin()));
    read->setText("read");
    write->setText("write");
    open->setText("open hex");
    save->setText("save as");
    // types of targets
    target->addItem(QString("m24c64"));
    target->addItem(QString("w25q64"));
    // text redactor
    QFont font("editorFont");
    font.setStyleHint(QFont::TypeWriter);
    font.setPointSize(10);
    mem->setCurrentFont(font);
    mem->setReadOnly(true);

    // ading all stuff to main window
    toolBar->addWidget(read);
    toolBar->addWidget(write);
    toolBar->addWidget(open);
    toolBar->addWidget(save);
    toolBar->addWidget(target);
    addToolBar(toolBar);
    setCentralWidget(mem);
    setStatusBar(stBar);
    resize(1024, 768);

    flasher = new gazelleUsb(this);
    flasher->setFlashType(0);
    // update graphics
//    timer = new QTimer(this);
//    connect(timer, SIGNAL(timeout()), this, SLOT(upgrade()));
//    timer->start(1000);
}

void MainWindow::viewFile(QFile *file)
{
    mem->clear();
    unsigned char line[16];
    file->seek(0);
    while (!file->atEnd()) {
        file->read((char *)line,16);
        mem->insertPlainText(QString::asprintf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n",line[0],line[1],line[2],line[3],line[4],line[5],line[6],line[7],line[8],line[9],line[10],line[11],line[12],line[13],line[14],line[15]));
    }
}


void MainWindow::openBin()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open binary"), ".", tr("Binary files (*.bin *.hex);;Text files (*.txt);;All (*)"));
    QFile*  newFile = new QFile(fileName, this);
    if (!newFile->open(QIODevice::ReadOnly | QIODevice::ExistingOnly)) {
        delete newFile;
        return;
    } else {
        delete binary;
        binary = newFile;
    }
    viewFile(binary);
}

void MainWindow::readFlash()
{
    QFile*  inputFile = new QFile(tempFilename, this);
    if (!inputFile->open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        delete inputFile;
        return;
    }
    flasher->readDump(inputFile);
    viewFile(inputFile);
    inputFile->close();
    delete inputFile;
}

void MainWindow::writeFlash()
{
    if(!binary->exists()) {
        return;
    }
    flasher->writeDump(binary);
}

void MainWindow::saveBin()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save as"), "dump.bin", tr("Binary files (*.bin *.hex);;Text files (*.txt);;All (*)"));
    binary = new QFile(fileName, this);
    QFile::copy(tempFilename, fileName);
}

MainWindow::~MainWindow()
{
    delete ui;
}
