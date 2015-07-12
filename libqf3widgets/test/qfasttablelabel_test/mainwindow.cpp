#include "mainwindow.h"
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout* lay=new QVBoxLayout(this);
    setLayout(lay);
    label=new QFastTableLabel(this);
    label->setData(QString("|col1|col 2\nrow1|1|2\nrow2|3|4\nrow 3|5|6|7"));
    lay->addWidget(label, 1);
    label=new QFastTableLabel(this);
    label->setColumnSize(0,50);
    //label->setColumnSize(1,30);
    //label->setColumnSize(2,50);
    label->setData(QString("|col1|col 2\nrow1|1|2\nrow2|3|4\nrow 3|5|6|7"));
    lay->addWidget(label, 1);
    label=new QFastTableLabel(this);
    label->setData(QString(">b25|25|25|25\n|col1|col 2\nrow1|1|2\nrow2|3|4\nrow 3|5|6|7"));
    lay->addWidget(label, 1);
}

MainWindow::~MainWindow()
{

}
