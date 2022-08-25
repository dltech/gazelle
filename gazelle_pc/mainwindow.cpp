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
//    connect(read, SIGNAL(released()), this, SLOT());
//    connect(write, SIGNAL(released()), this, SLOT());
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

    // port settings
    flaserPort = new QSerialPort(this);
    QList<QSerialPortInfo> list;
    list = QSerialPortInfo::availablePorts();
    for( int i=0 ; i<list.length() ; ++i) {
        qDebug() << list[i].portName();
        if( list[i].portName().contains("ACM") ) {
            flaserPort->setPortName(list[i].portName());
        }
    }
    flaserPort->setBaudRate(9600,QSerialPort::Input);
    if( flaserPort->open(QIODevice::ReadWrite) ) {
        stBar->showMessage("ready");
    }
    // update graphics
//    timer = new QTimer(this);
//    connect(timer, SIGNAL(timeout()), this, SLOT(upgrade()));
//    timer->start(1000);
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
    mem->clear();
    int cnt = 0;
    char data;
    unsigned char line[16];
    qDebug() << fileName;
    while (!binary->atEnd()) {
        binary->read((char *)line,16);
        mem->insertPlainText(QString::asprintf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n",line[0],line[1],line[2],line[3],line[4],line[5],line[6],line[7],line[8],line[9],line[10],line[11],line[12],line[13],line[14],line[15]));

//        mem->insertPlainText(QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14 %15 %16").arg(line[0],2,16,QLatin1Char('0'))
//                                                                                              .arg(line[1],2,16,QLatin1Char('0'))
//                                                                                              .arg(line[2],2,16,QLatin1Char('0'))
//                                                                                              .arg(line[3],2,16,QLatin1Char('0'))
//                                                                                              .arg(line[4],2,16,QLatin1Char('0'))
//                                                                                              .arg(line[5],2,16,QLatin1Char('0'))
//                                                                                              .arg(line[6],2,16,QLatin1Char('0'))
//                                                                                              .arg(line[7],2,16,QLatin1Char('0'))
//                                                                                              .arg(line[8],2,16,QLatin1Char('0'))
//                                                                                              .arg(line[9],2,16,QLatin1Char('0'))
//                                                                                              .arg(line[10],2,16,QLatin1Char('0'))
//                                                                                              .arg(line[11],2,16,QLatin1Char('0'))
//                                                                                              .arg(line[12],2,16,QLatin1Char('0'))
//                                                                                              .arg(line[13],2,16,QLatin1Char('0'))
//                                                                                              .arg(line[14],2,16,QLatin1Char('0'))
//                                                                                              .arg(line[15],2,16,QLatin1Char('0')));

//        QByteArray byteStr = binary->readLine(16);
//        mem->insertPlainText(QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14 %15 %16").arg(byteStr.at(0),2,16,QLatin1Char('0'))
//                                                                                              .arg(byteStr.at(1),2,16,QLatin1Char('0'))
//                                                                                              .arg(byteStr.at(2),2,16,QLatin1Char('0'))
//                                                                                              .arg(byteStr.at(3),2,16,QLatin1Char('0'))
//                                                                                              .arg(byteStr.at(4),2,16,QLatin1Char('0'))
//                                                                                              .arg(byteStr.at(5),2,16,QLatin1Char('0'))
//                                                                                              .arg(byteStr.at(6),2,16,QLatin1Char('0'))
//                                                                                              .arg(byteStr.at(7),2,16,QLatin1Char('0'))
//                                                                                              .arg(byteStr.at(8),2,16,QLatin1Char('0'))
//                                                                                              .arg(byteStr.at(9),2,16,QLatin1Char('0'))
//                                                                                              .arg(byteStr.at(10),2,16,QLatin1Char('0'))
//                                                                                              .arg(byteStr.at(11),2,16,QLatin1Char('0'))
//                                                                                              .arg(byteStr.at(12),2,16,QLatin1Char('0'))
//                                                                                              .arg(byteStr.at(13),2,16,QLatin1Char('0'))
//                                                                                              .arg(byteStr.at(14),2,16,QLatin1Char('0'))
//                                                                                              .arg(byteStr.at(15),2,16,QLatin1Char('0')));
//        qDebug() << "str!";
//        binary->read(&data,sizeof(char));
//        mem->insertPlainText(QString("%1 ").arg((unsigned char)data,2,16,QLatin1Char('0')));
//        if( ++cnt >= 16 ) {
//            cnt = 0;
//            mem->append(QString());
//            qDebug() << "str!";
//        }
    }
}

void MainWindow::saveBin()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open binary"), ".", tr("Binary files (*.bin *.hex);;Text files (*.txt);;All (*)"));
    binary = new QFile(fileName, this);
    if (!binary->open(QIODevice::ReadOnly | QIODevice::ExistingOnly)) {
        delete binary;
        return;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
