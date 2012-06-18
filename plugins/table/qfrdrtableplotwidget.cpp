#include "qfrdrtableplotwidget.h"
#include "ui_qfrdrtableplotwidget.h"
#include "qfrdrtable.h"

QFRDRTablePlotWidget::QFRDRTablePlotWidget(QFRDRTable *record, int plot, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFRDRTablePlotWidget)
{
    updating=true;
    ui->setupUi(this);
    current=record;
    this->plot=plot;
    updating=false;
}

QFRDRTablePlotWidget::~QFRDRTablePlotWidget()
{
    delete ui;
}

void QFRDRTablePlotWidget::setRecord(QFRDRTable *record, int graph)
{
    current=record;
    this->plot=graph;
    updating=true;
    if (record) {
        ui->edtTitle->setText(record->getPlot(plot).title);
        ui->edtXLabel->setText(record->getPlot(plot).xlabel);
        ui->edtYLabel->setText(record->getPlot(plot).ylabel);
        ui->listGraphs->clear();
        QList<QFRDRTable::GraphInfo> graphs=record->getPlot(plot).graphs;
        for (int i=0; i<graphs.size(); i++) {
            ui->listGraphs->addItem(graphs[i].title);
        }
    }
    updating=false;
    ui->listGraphs->setCurrentRow(0);
}
