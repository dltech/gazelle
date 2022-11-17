#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>
#include <QComboBox>
#include <QPushButton>
#include <QToolBar>
#include <QStatusBar>
#include <QTextEdit>
#include <QScrollBar>
#include <QWidget>
#include <QString>
#include <QProgressBar>
#include <QThread>
#include <QBoxLayout>
#include "gazelleusb.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void readFlash(void);
    void writeFlash(void);
    void openBin(void);
    void saveBin(void);
    void setFlash(void);
    void updateProgressBar(void);
    void finishWrite(void);
    void finishRead(void);
    void viewFile(void);
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void disableButtons(void);
    void enableButtons(void);
    Ui::MainWindow *ui;
    // buttons
    QWidget *hexViewer;
    QToolBar *toolBar;
    QPushButton *read;
    QPushButton *write;
    QPushButton *open;
    QPushButton *save;
    QComboBox *target;
    QBoxLayout *viewerLay;
    QTextEdit *mem;
    QScrollBar *textScroll;
    QStatusBar *stBar;
    QProgressBar *progress;
    // timer for udatind interface
    QTimer *timer;
    // file
    QFile *binary = NULL;
    QString tempFilename = "new.bin";
    QString currentDir = ".";

    gazelleUsb *flasher;
};
#endif // MAINWINDOW_H
