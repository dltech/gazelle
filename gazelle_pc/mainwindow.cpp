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
    connect(read, SIGNAL(released()), this, SLOT());
    connect(write, SIGNAL(released()), this, SLOT());
    connect(open, SIGNAL(released()), this, SLOT());
    connect(save, SIGNAL(released()), this, SLOT());
    read->setText("read");
    write->setText("write");
    open->setText("open hex");
    save->setText("save as");
    toolBar->addWidget(read);
    toolBar->addWidget(write);
    toolBar->addWidget(open);
    toolBar->addWidget(save);
    // all other stuff
    stBar->showMessage("0:1000");
    mem->setReadOnly(true);
    // ading to main window
    addToolBar(toolBar);
    setCentralWidget(mem);
    setStatusBar(stBar);
    resize(1024, 768);

    // port settings
    rxPort = new QSerialPort(this);
    QList<QSerialPortInfo> list;
    list = QSerialPortInfo::availablePorts();
    for( int i=0 ; i<list.length() ; ++i) {
        qDebug() << list[i].portName();
        if( list[i].portName().contains("ACM") ) {
            rxPort->setPortName(list[i].portName());
        }
    }
    rxPort->setBaudRate(9600,QSerialPort::Input);
    rxPort->open(QIODevice::ReadOnly);

    // update graphics
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(upgrade()));
    timer->start(1000);
}



MainWindow::~MainWindow()
{
    delete ui;
}
