#include "qfrdrtableplotwidget.h"
#include "ui_qfrdrtableplotwidget.h"
#include "qfrdrtable.h"
#include "qftools.h"

QFRDRTablePlotWidget::QFRDRTablePlotWidget(QFRDRTable *record, int plot, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFRDRTablePlotWidget)
{
    updating=true;
    ui->setupUi(this);
    current=record;
    this->plot=plot;
    updating=false;
    connectWidgets();
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
    disconnectWidgets();
    if (record) {
        if (graph>=0 && graph<record->getPlotCount()) {
            QFRDRTable::PlotInfo g=record->getPlot(plot);
            ui->edtTitle->setText(g.title);
            ui->edtXLabel->setText(g.xlabel);
            ui->edtYLabel->setText(g.ylabel);
            ui->listGraphs->clear();
            QList<QFRDRTable::GraphInfo> graphs=g.graphs;
            for (int i=0; i<g.graphs.size(); i++) {
                ui->listGraphs->addItem(graphs.at(i).title);
            }
        } else {
            ui->edtTitle->setText("");
            ui->edtXLabel->setText("");
            ui->edtYLabel->setText("");
            ui->listGraphs->clear();
        }
    }
    ui->listGraphs->setCurrentRow(0);
    reloadColumns(ui->cmbLinesXData);
    reloadColumns(ui->cmbLinesXError);
    reloadColumns(ui->cmbLinesYData);
    reloadColumns(ui->cmbLinesYError);
    connectWidgets();
    updating=false;
    listGraphs_currentRowChanged(ui->listGraphs->currentRow());
}

void QFRDRTablePlotWidget::rawDataChanged() {
    if (updating) return;
    disconnectWidgets();
    reloadColumns(ui->cmbLinesXData);
    reloadColumns(ui->cmbLinesXError);
    reloadColumns(ui->cmbLinesYData);
    reloadColumns(ui->cmbLinesYError);
    connectWidgets();
    updateData();
}

void QFRDRTablePlotWidget::readSettings(QSettings &settings, const QString &prefix)
{
    loadSplitter(settings, ui->splitter, prefix+"splitter/");
}

void QFRDRTablePlotWidget::writeSettings(QSettings &settings, const QString &prefix)
{
    saveSplitter(settings, ui->splitter, prefix+"splitter/");
}

void QFRDRTablePlotWidget::listGraphs_currentRowChanged(int currentRow) {
    if (updating) return;
    updating=true;
    disconnectWidgets();
    //qDebug()<<"listGraphs_currentRowChanged("<<currentRow<<"/"<<current->getPlot(this->plot).graphs.size()<<",   "<<this->plot<<"/"<<current->getPlotCount()<<")";

    if (!current || this->plot<0 || this->plot>=current->getPlotCount() || currentRow<0 || currentRow>=current->getPlot(this->plot).graphs.size()) {
        ui->widGraphSettings->setEnabled(false);
        ui->edtGraphTitle->setText("");
        ui->cmbLinesXData->setCurrentIndex(0);
        ui->cmbLinesXError->setCurrentIndex(0);
        ui->cmbLinesYData->setCurrentIndex(0);
        ui->cmbLinesYError->setCurrentIndex(0);
        ui->cmbGraphType->setCurrentIndex(0);
    } else {
        ui->widGraphSettings->setEnabled(true);
        QFRDRTable::GraphInfo graph=current->getPlot(this->plot).graphs.at(currentRow);
        ui->edtGraphTitle->setText(graph.title);
        ui->cmbLinesXData->setCurrentIndex(graph.xcolumn+1);
        ui->cmbLinesXError->setCurrentIndex(graph.xerrorcolumn+1);
        ui->cmbLinesYData->setCurrentIndex(graph.ycolumn+1);
        ui->cmbLinesYError->setCurrentIndex(graph.yerrorcolumn+1);
        switch(graph.type) {
            case QFRDRTable::gtImpulsesVertical:
                ui->cmbGraphType->setCurrentIndex(1);
                break;
            case QFRDRTable::gtImpulsesHorizontal:
                ui->cmbGraphType->setCurrentIndex(2);
                break;
            case QFRDRTable::gtFilledCurveX:
                ui->cmbGraphType->setCurrentIndex(3);
                break;
            case QFRDRTable::gtFilledCurveY:
                ui->cmbGraphType->setCurrentIndex(4);
                break;
            case QFRDRTable::gtStepsVertical:
                ui->cmbGraphType->setCurrentIndex(5);
                break;
            case QFRDRTable::gtStepsHorizontal:
                ui->cmbGraphType->setCurrentIndex(6);
                break;
            case QFRDRTable::gtbarsVertical:
                ui->cmbGraphType->setCurrentIndex(7);
                break;
            case QFRDRTable::gtbarsHorizontal:
                ui->cmbGraphType->setCurrentIndex(8);
                break;
            case QFRDRTable::gtLines:
            default:
                ui->cmbGraphType->setCurrentIndex(0);
                break;
        }

    }
    connectWidgets();

    updating=false;
    updateGraph();
}

void QFRDRTablePlotWidget::on_btnDeleteGraph_clicked()
{
    if (!current) return;
    if (current) {
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;
        int r=ui->listGraphs->currentRow();
        QFRDRTable::PlotInfo p=current->getPlot(this->plot);
        if (r>=0 && r<p.graphs.size()) {
            disconnectWidgets();
            p.graphs.removeAt(r);
            updating=true;
            delete ui->listGraphs->takeItem(r);
            current->setPlot(this->plot, p);
            updating=false;
            if (r>=p.graphs.size()) r=p.graphs.size()-1;
            connectWidgets();
            ui->listGraphs->setCurrentRow(r);
        }
    }
}

void QFRDRTablePlotWidget::on_btnAddGraph_clicked() {
    if (!current) return;
    if (current) {
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;
        disconnectWidgets();
        int r=ui->listGraphs->currentRow();
        QFRDRTable::PlotInfo p=current->getPlot(this->plot);
        QFRDRTable::GraphInfo g;
        g.title=tr("graph %1").arg(p.graphs.size()+1);
        p.graphs.append(g);
        updating=true;
        ui->listGraphs->addItem(g.title);
        current->setPlot(this->plot, p);
        updating=false;
        connectWidgets();
        ui->listGraphs->setCurrentRow(p.graphs.size()-1);
        ui->edtGraphTitle->setFocus();
    }
}

void QFRDRTablePlotWidget::reloadColumns(QComboBox *combo) {
    bool updt=updating;
    updating=true;
    int idx=combo->currentIndex();
    //disconnect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    combo->clear();
    if (current) {
        QFTablePluginModel* tab=current->model();
        combo->addItem(tr("--- none ---"));
        for (int i=0; i<tab->columnCount(); i++) {
            combo->addItem(tr("#%1: %2").arg(i+1).arg(tab->columnTitle(i)), tab->columnTitle(i));
        }
    }
    updating=updt;
    //connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    combo->setCurrentIndex(idx);
}

void QFRDRTablePlotWidget::graphDataChanged() {
    if (updating) return;
    if (current) {
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;
        int r=ui->listGraphs->currentRow();
        QFRDRTable::PlotInfo p=current->getPlot(this->plot);
        if (r>=0 && r<p.graphs.size()) {
            QFRDRTable::GraphInfo graph=p.graphs.at(r);
            graph.title=ui->edtGraphTitle->text();

            switch(ui->cmbGraphType->currentIndex()) {
                case 1: graph.type=QFRDRTable::gtImpulsesVertical; break;
                case 2: graph.type=QFRDRTable::gtImpulsesHorizontal; break;
                case 3: graph.type=QFRDRTable::gtFilledCurveX; break;
                case 4: graph.type=QFRDRTable::gtFilledCurveY; break;
                case 5: graph.type=QFRDRTable::gtStepsVertical; break;
                case 6: graph.type=QFRDRTable::gtStepsHorizontal; break;
                case 7: graph.type=QFRDRTable::gtbarsVertical; break;
                case 8: graph.type=QFRDRTable::gtbarsHorizontal; break;
                case 0:
                default:
                    graph.type=QFRDRTable::gtLines;
                    break;
            }
            graph.xcolumn=qMax(-1, ui->cmbLinesXData->currentIndex()-1);
            graph.xerrorcolumn=qMax(-1, ui->cmbLinesXError->currentIndex()-1);
            graph.ycolumn=qMax(-1, ui->cmbLinesYData->currentIndex()-1);
            graph.yerrorcolumn=qMax(-1, ui->cmbLinesYError->currentIndex()-1);

            p.graphs[r]=graph;
            current->setPlot(this->plot, p);

        }
    }
    updateGraph();
}

void QFRDRTablePlotWidget::plotDataChanged() {
    if (updating) return;
    if (current) {
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;
        QFRDRTable::PlotInfo p=current->getPlot(this->plot);
        p.title=ui->edtTitle->text();
        p.xlabel=ui->edtXLabel->text();
        p.ylabel=ui->edtXLabel->text();
        current->setPlot(this->plot, p);
        //QFRDRTable::GraphInfo graph=current->getPlot(this->plot).graphs.value(currentRow, QFRDRTable::GraphInfo());
    }
    updateGraph();
}

void QFRDRTablePlotWidget::updateGraph() {
    if (current) {
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;
        int r=ui->listGraphs->currentRow();
        QFRDRTable::PlotInfo p=current->getPlot(this->plot);

        ui->plotter->set_doDrawing(false);


        ui->plotter->getXAxis()->set_axisLabel(p.xlabel);
        ui->plotter->getXAxis()->set_logAxis(p.xlog);
        ui->plotter->getYAxis()->set_axisLabel(p.ylabel);
        ui->plotter->getYAxis()->set_logAxis(p.ylog);
        ui->plotter->setXY(p.xmin, p.xmax, p.ymin, p.ymax);
        ui->plotter->clearGraphs(true);
        updateData();
        for (int i=0; i<p.graphs.size(); i++) {
            QFRDRTable::GraphInfo g=p.graphs.at(i);
            if (g.type==QFRDRTable::gtImpulsesVertical) {
                JKQTPimpulsesVerticalErrorGraph* pg=new JKQTPimpulsesVerticalErrorGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                if (g.xcolumn>=0 && g.xcolumn<ui->plotter->getDatastore()->getColumnCount())  pg->set_xColumn(g.xcolumn);
                //if (g.xerrorcolumn>=0 && g.xerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) pg->set_xErrorColumn(g.xerrorcolumn);
                if (g.ycolumn>=0 && g.ycolumn<ui->plotter->getDatastore()->getColumnCount()) pg->set_yColumn(g.ycolumn);
                if (g.yerrorcolumn>=0 && g.yerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) pg->set_yErrorColumn(g.yerrorcolumn);
                pg->set_lineWidth(g.linewidth);
                //pg->set_xErrorStyle(g.errorStyle);
                pg->set_yErrorStyle(g.errorStyle);
                pg->set_color(g.color);
                pg->set_errorColor(g.errorColor);
                QColor efc=g.errorColor;
                efc.setAlphaF(0.5);
                pg->set_errorFillColor(efc);
                ui->plotter->addGraph(pg);
            } else { // gtLines etc.
                JKQTPxyLineErrorGraph* pg=new JKQTPxyLineErrorGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                if (g.xcolumn>=0 && g.xcolumn<ui->plotter->getDatastore()->getColumnCount())  pg->set_xColumn(g.xcolumn);
                if (g.xerrorcolumn>=0 && g.xerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) pg->set_xErrorColumn(g.xerrorcolumn);
                if (g.ycolumn>=0 && g.ycolumn<ui->plotter->getDatastore()->getColumnCount()) pg->set_yColumn(g.ycolumn);
                if (g.yerrorcolumn>=0 && g.yerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) pg->set_yErrorColumn(g.yerrorcolumn);
                pg->set_drawLine(g.drawLine);
                pg->set_symbol(g.symbol);
                pg->set_symbolSize(g.symbolSize);
                pg->set_lineWidth(g.linewidth);
                pg->set_xErrorStyle(g.errorStyle);
                pg->set_yErrorStyle(g.errorStyle);
                pg->set_color(g.color);
                pg->set_errorColor(g.errorColor);
                QColor efc=g.errorColor;
                efc.setAlphaF(0.5);
                pg->set_errorFillColor(efc);
                ui->plotter->addGraph(pg);
            }
        }

        ui->plotter->set_doDrawing(true);
        ui->plotter->update_plot();
    }
}

void QFRDRTablePlotWidget::updateData() {
    bool update=ui->plotter->get_doDrawing();
    ui->plotter->set_doDrawing(false);

    JKQTPdatastore* ds=ui->plotter->getDatastore();
    ds->clear();
    if (current) {
        for (int c=0; c<current->model()->columnCount(); c++) {
            double* data=(double*)malloc(current->model()->rowCount()*sizeof(double));

            for (int r=0; r<current->model()->rowCount(); r++) {
                data[r]=NAN;
                bool ok=false;
                double d=current->model()->cell(r, c).toDouble(&ok);
                if (ok) data[r]=d;
            }

            ds->addCopiedColumn(data, current->model()->rowCount(), current->model()->columnTitle(c));
            free(data);
        }
    }

    if (update) {
        ui->plotter->set_doDrawing(true);
        ui->plotter->update_plot();
    }
}

void QFRDRTablePlotWidget::connectWidgets()
{
    //qDebug()<<"connectWidgets";
    connect(ui->listGraphs, SIGNAL(currentRowChanged(int)), this, SLOT(listGraphs_currentRowChanged(int)));

    connect(ui->edtTitle, SIGNAL(textChanged(QString)), this, SLOT(plotDataChanged()));
    connect(ui->edtXLabel, SIGNAL(textChanged(QString)), this, SLOT(plotDataChanged()));
    connect(ui->edtYLabel, SIGNAL(textChanged(QString)), this, SLOT(plotDataChanged()));

    connect(ui->edtGraphTitle, SIGNAL(textChanged(QString)), this, SLOT(graphDataChanged()));
    connect(ui->cmbGraphType, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->cmbLinesXData, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->cmbLinesXError, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->cmbLinesYData, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->cmbLinesYError, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
}

void QFRDRTablePlotWidget::disconnectWidgets()
{
    //qDebug()<<"disconnectWidgets";
    disconnect(ui->listGraphs, SIGNAL(currentRowChanged(int)), this, SLOT(listGraphs_currentRowChanged(int)));

    disconnect(ui->edtTitle, SIGNAL(textChanged(QString)), this, SLOT(plotDataChanged()));
    disconnect(ui->edtXLabel, SIGNAL(textChanged(QString)), this, SLOT(plotDataChanged()));
    disconnect(ui->edtYLabel, SIGNAL(textChanged(QString)), this, SLOT(plotDataChanged()));

    disconnect(ui->edtGraphTitle, SIGNAL(textChanged(QString)), this, SLOT(graphDataChanged()));
    disconnect(ui->cmbGraphType, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->cmbLinesXData, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->cmbLinesXError, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->cmbLinesYData, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->cmbLinesYError, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
}
