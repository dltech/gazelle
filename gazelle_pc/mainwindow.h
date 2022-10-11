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
#include <QWidget>

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
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    // buttons
    QToolBar *toolBar;
    QPushButton *read;
    QPushButton *write;
    QPushButton *open;
    QPushButton *save;
    QComboBox *target;
    QTextEdit *mem;
    QStatusBar *stBar;
    // port
    QSerialPort *flaserPort;
    // костыль
    QTimer *timer;
    // file
    QFile *binary;
};
#endif // MAINWINDOW_H
