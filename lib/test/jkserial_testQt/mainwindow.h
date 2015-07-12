#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QMainWindow>
#include "../../jkserialconnection.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected slots:
    void on_btnConnect_clicked();
    void on_btnDisconnect_clicked();
    void on_btnSendLB_clicked();
    void on_btnSendNoLB_clicked();
    void readBytesFromSerial();
private:
    Ui::MainWindow *ui;
    JKSerialConnection* serial;
    QTimer* tim;
};

#endif // MAINWINDOW_H
