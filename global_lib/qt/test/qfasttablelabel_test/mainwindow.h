#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "../../qfasttablelabel.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();


protected:
    QFastTableLabel* label;
};

#endif // MAINWINDOW_H
