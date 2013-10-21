#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->edtLogDouble->setLogScale(true);
    ui->edtInt->setIntegerWidget(true);

    ui->lineEdit->addInsertContextMenuEntry("test1", "test1");
    ui->lineEdit->addInsertContextMenuEntry("test2", "test2");
    ui->lineEdit->addInsertContextMenuEntry("test;;test1", "ttest1");
    ui->lineEdit->addInsertContextMenuEntry("test;;test2", "ttest2");
    ui->lineEdit->addInsertContextMenuEntry("test;;testsub;;test1", "tttest1");
    ui->lineEdit->addInsertContextMenuEntry("test;;test3", "ttest3");
    ui->lineEdit->addInsertContextMenuEntry("test;;testsub2;;test1", "tt2test1");
    ui->lineEdit->addInsertContextMenuEntry("test;;testsub;;test2", "tttest2");
    ui->lineEdit->addInsertContextMenuEntry("sub;;test1", "stest1");

    updateWidgets();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateWidgets()
{
    ui->edtDouble->setRange(ui->spinDRangeMin->value(), ui->spinDRangeMax->value());
    ui->horizontalSlider->setDRange(ui->spinDRangeMin->value(), ui->spinDRangeMax->value());
    ui->edtDouble->setIncrement(ui->spinDInc->value());
    ui->edtLogDouble->setRange(ui->spinDRangeMin_2->value(), ui->spinDRangeMax_2->value());
    ui->edtLogDouble->setIncrement(ui->spinDInc_2->value());
    ui->edtInt->setRange(ui->spinIRangeMin->value(), ui->spinIRangeMax->value());
    ui->edtInt->setIncrement(ui->spinIInc->value());
}
