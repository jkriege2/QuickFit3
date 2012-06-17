#include "qfrdrtableplotwidget.h"
#include "ui_qfrdrtableplotwidget.h"

QFRDRTablePlotWidget::QFRDRTablePlotWidget(QFRawDataPropertyEditor *propEditor, QFRawDataRecord *record, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFRDRTablePlotWidget)
{
    ui->setupUi(this);
}

QFRDRTablePlotWidget::~QFRDRTablePlotWidget()
{
    delete ui;
}
