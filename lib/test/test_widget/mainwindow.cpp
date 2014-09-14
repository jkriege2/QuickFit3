#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->coll1->setTitle("Title 1");
    ui->coll1->setIcon(QPixmap(":/lib/print.png"));
    ui->coll2->setTitle("Title 2");
    ui->coll3->setIcon(QPixmap(":/lib/print.png"));
    ui->coll3->setTitle("Title 3");

    ui->labR1->setText("text, <b>normal</b>");
    ui->labR2->setText("text, clock");
    ui->labR2->setOrientation(QFRotatableLabel::ClockwiseRotation);
    ui->labR3->setText("text, <b>counter-clock</b>");
    ui->labR3->setOrientation(QFRotatableLabel::CounterClockwiseRotation);
    ui->labR4->setText("text,\nupside-down");
    ui->labR4->setOrientation(QFRotatableLabel::UpsideDownRotation);

    ui->labR1_2->setPixmap(QPixmap(":/lib/help/autolinkweb.png"));
    ui->labR2_2->setPixmap(QPixmap(":/lib/help/autolinkweb.png"));
    ui->labR2_2->setOrientation(QFRotatableLabel::ClockwiseRotation);
    ui->labR3_2->setPixmap(QPixmap(":/lib/help/autolinkweb.png"));
    ui->labR3_2->setOrientation(QFRotatableLabel::CounterClockwiseRotation);
    ui->labR4_2->setPixmap(QPixmap(":/lib/help/autolinkweb.png"));
    ui->labR4_2->setOrientation(QFRotatableLabel::UpsideDownRotation);


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
