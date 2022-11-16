#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QDebug>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("gazelle usb flasher");
    // all objects of UI
    toolBar = new QToolBar(this);
    read = new QPushButton(this);
    write = new QPushButton(this);
    open = new QPushButton(this);
    save = new QPushButton(this);
    target = new QComboBox(this);
    mem = new QTextEdit(this);
//    textScroll = new QScrollBar(this);
    stBar = new QStatusBar(this);
    progress = new QProgressBar(this);

    // configurating of UI
    // buttons config
    read->setText("read");
    write->setText("write");
    open->setText("open hex");
    save->setText("save as");
    connect(read, SIGNAL(released()), this, SLOT(readFlash()));
    connect(write, SIGNAL(released()), this, SLOT(writeFlash()));
    connect(open, SIGNAL(released()), this, SLOT(openBin()));
    connect(save, SIGNAL(released()), this, SLOT(saveBin()));
    // types of targets
    target->addItem(QString("m24c64"));
    target->addItem(QString("w25q64"));
    connect(target, SIGNAL(currentIndexChanged(int)), this, SLOT(setFlash()));
    //progressbar
    progress->setMinimum(0);
    progress->setMaximum(100);
    // text redactor
    QFont font("editorFont");
    font.setStyleHint(QFont::TypeWriter);
    font.setStyleHint(QFont::Monospace);
    font.setPointSize(10);
    mem->setCurrentFont(font);
    mem->setReadOnly(true);
//    textScroll->setMinimum(0);
//    textScroll->setMaximum(1000);
//    textScroll->setScrollBarPolicy(Qt::ScrollBarAlwaysOn);
//    mem->addScrollBarWidget(textScroll, Qt::AlignRight);

    // ading all stuff to main window
    toolBar->addWidget(read);
    toolBar->addWidget(write);
    toolBar->addWidget(open);
    toolBar->addWidget(save);
    toolBar->addWidget(target);
    toolBar->addWidget(progress);
    addToolBar(toolBar);
    setCentralWidget(mem);
    setStatusBar(stBar);
    resize(640, 480);

    flasher = new gazelleUsb();
    target->setCurrentIndex(0);
    flasher->setFlashType(0);
    // update progress bar
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateProgressBar()));
    connect(flasher, SIGNAL(anotherEvent()), this, SLOT(updateProgressBar()));
    connect(flasher,SIGNAL(writeComplete()), this, SLOT(finishWrite()));
    connect(flasher,SIGNAL(readComplete()), this, SLOT(finishRead()));
    updateProgressBar();
}

void MainWindow::viewFile(QFile *file)
{
    mem->clear();
    unsigned char line[16];
    file->seek(0);
    int cnt = 0;
    while(  (!file->atEnd()) &&  (cnt++ < 2048) ) {
        file->read((char *)line,16);
        mem->setTextColor(QColor(QColorConstants::LightGray));
        mem->insertPlainText(QString::asprintf("%08x  ",cnt));
        mem->setTextColor(QColor(QColorConstants::Black));
        mem->insertPlainText(QString::asprintf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",line[0],line[1],line[2],line[3],line[4],line[5],line[6],line[7],line[8],line[9],line[10],line[11],line[12],line[13],line[14],line[15]));
        mem->setTextColor(QColor(QColorConstants::LightGray));
        for(int i=0 ; i<16 ; ++i) if((line[i]<0x20) || (line[i]>0x7e))  line[i] = '.';
        mem->insertPlainText(QString::asprintf("  %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c \n",line[0],line[1],line[2],line[3],line[4],line[5],line[6],line[7],line[8],line[9],line[10],line[11],line[12],line[13],line[14],line[15]));
    }
}

void MainWindow::disableButtons()
{
    read->setDisabled(true);
    write->setDisabled(true);
    open->setDisabled(true);
    save->setDisabled(true);
    target->setDisabled(true);
}

void MainWindow::enableButtons()
{
    read->setDisabled(false);
    write->setDisabled(false);
    open->setDisabled(false);
    save->setDisabled(false);
    target->setDisabled(false);
}

void MainWindow::openBin()
{
    if( binary != NULL ) {
        binary->close();
        delete binary;
        binary = NULL;
    }
    QFileDialog chooseFile(this, tr("Open binary"), currentDir, tr("Binary files (*.bin *.hex);;Text files (*.txt);;All (*)"));
    chooseFile.setFileMode(QFileDialog::ExistingFile);
    if( !chooseFile.exec() ) {
        return;
    }
    QString fileName = chooseFile.selectedFiles().at(0);
    binary = new QFile(fileName, this);
    if (!binary->open(QIODevice::ReadOnly)) {
        delete binary;
        binary = NULL;
        stBar->showMessage("file not found");
        return;
    } else {
        stBar->showMessage(fileName.append(" opened"));
    }
    currentDir.clear();
    currentDir.append(chooseFile.directory().path());
    viewFile(binary);
}

void MainWindow::readFlash()
{
    if(binary != NULL) {
        binary->close();
        delete binary;
        binary= NULL;
    }
    binary = new QFile(tempFilename, this);
    if (!binary->open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        delete binary;
        binary = NULL;
        return;
    }
    timer->start(300);
    disableButtons();
    flasher->readDump(binary);
}

void MainWindow::writeFlash()
{
    if(binary == NULL) {
        return;
    }
    if(!binary->exists()) {
        return;
    }
    timer->start(300);
    disableButtons();
    flasher->writeDump(binary);
}

void MainWindow::saveBin()
{
    QFileDialog chooseFile(this, tr("Save as"), currentDir, tr("Binary files (*.bin *.hex);;Text files (*.txt);;All (*)"));
    chooseFile.setAcceptMode(QFileDialog::AcceptSave);
    chooseFile.selectFile("dump.bin");
    if( !chooseFile.exec() ) {
        return;
    }
    QString fileName = chooseFile.selectedFiles().at(0);
    QFile::copy(tempFilename, fileName);
    currentDir.clear();
    currentDir.append(chooseFile.directory().path());
}

void MainWindow::setFlash()
{
    int type = target->currentIndex();
    flasher->setFlashType(type);
}

void MainWindow::updateProgressBar()
{
    progress->setValue(flasher->getProgress());
    stBar->showMessage(*flasher->getMessage());
}

void MainWindow::finishWrite()
{
    binary->close();
    updateProgressBar();
    timer->stop();
    enableButtons();
}

void MainWindow::finishRead()
{
    viewFile(binary);
    binary->close();
    updateProgressBar();
    timer->stop();
    enableButtons();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete flasher;
}
