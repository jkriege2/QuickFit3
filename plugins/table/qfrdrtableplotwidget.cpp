#include "qfrdrtableplotwidget.h"
#include "programoptions.h"
#include "ui_qfrdrtableplotwidget.h"
#include "qfrdrtable.h"
#include "qftools.h"
#include "qfdoubleedit.h"



QFRDRTablePlotWidget::QFRDRTablePlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFRDRTablePlotWidget)
{
    autocolors.append(QColor("red"));
    autocolors.append(QColor("green"));
    autocolors.append(QColor("fuchsia"));
    autocolors.append(QColor("darkorange"));
    autocolors.append(QColor("navy"));
    autocolors.append(QColor("firebrick"));
    autocolors.append(QColor("darkgreen"));
    autocolors.append(QColor("darkmagenta"));
    autocolors.append(QColor("darkgreen"));
    autocolors.append(QColor("darkslateblue"));
    autocolors.append(QColor("maroon"));
    autocolors.append(QColor("indianred"));
    autocolors.append(QColor("darkolivegreen"));
    autocolors.append(QColor("mediumpurple"));
    autocolors.append(QColor("darkcyan"));

    updating=true;
    ui->setupUi(this);
    ui->edtXMin->setCheckBounds(false, true);
    ui->edtXMax->setCheckBounds(true, false);
    ui->edtYMin->setCheckBounds(false, true);
    ui->edtYMax->setCheckBounds(true, false);
    connect(ui->edtXMin, SIGNAL(valueChanged(double)), ui->edtXMax, SLOT(setMinimum(double)));
    connect(ui->edtXMax, SIGNAL(valueChanged(double)), ui->edtXMin, SLOT(setMaximum(double)));
    connect(ui->edtYMin, SIGNAL(valueChanged(double)), ui->edtYMax, SLOT(setMinimum(double)));
    connect(ui->edtYMax, SIGNAL(valueChanged(double)), ui->edtYMin, SLOT(setMaximum(double)));
    ui->plotter->set_displayToolbar(false);
    ui->plotter->set_displayMousePosition(false);
    ui->plotter->set_mouseActionMode(JKQtPlotter::NoMouseAction);
    ui->plotter->get_plotter()->set_useAntiAliasingForGraphs(true);
    ui->plotter->get_plotter()->set_useAntiAliasingForSystem(true);
    ui->plotter->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    ui->plotter->get_plotter()->set_userSettigsPrefix("rdr_table/QFRDRTablePlotWidget/");

    toolbarPlot=new QToolBar("tb_plot", this);
    ui->layPlot->insertWidget(0, toolbarPlot);
    toolbarPlot->addAction(ui->plotter->get_plotter()->get_actPrint());
    toolbarPlot->addAction(ui->plotter->get_plotter()->get_actSavePlot());
    toolbarPlot->addAction(ui->plotter->get_plotter()->get_actSaveData());
    toolbarPlot->addSeparator();
    toolbarPlot->addAction(ui->plotter->get_plotter()->get_actCopyPixelImage());
    toolbarPlot->addAction(ui->plotter->get_plotter()->get_actCopyData());
    toolbarPlot->addAction(ui->plotter->get_plotter()->get_actCopyMatlab());
    toolbarPlot->addSeparator();
    labPlotPosition=new QLabel("", this);
    toolbarPlot->addWidget(labPlotPosition);
    current=NULL;
    this->plot=-1;
    updating=false;

    ui->tabWidget->setCurrentIndex(0);

    if (ProgramOptions::getInstance() && ProgramOptions::getInstance()->getQSettings()) readSettings(*(ProgramOptions::getInstance()->getQSettings()), "table/QFRDRTablePlotWidget/");
}

QFRDRTablePlotWidget::~QFRDRTablePlotWidget()
{
    if (ProgramOptions::getInstance() && ProgramOptions::getInstance()->getQSettings()) writeSettings(*(ProgramOptions::getInstance()->getQSettings()), "table/QFRDRTablePlotWidget/");
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
            ui->tabSystem->setVisible(true);
            QFRDRTable::PlotInfo g=record->getPlot(plot);
            ui->edtTitle->setText(g.title);
            ui->edtXLabel->setText(g.xlabel);
            ui->edtYLabel->setText(g.ylabel);
            ui->chkLogX->setChecked(g.xlog);
            ui->chkLogY->setChecked(g.ylog);
            ui->chkGrid->setChecked(g.grid);
            ui->chkShowKey->setChecked(g.showKey);
            ui->edtXMin->setValue(g.xmin);
            ui->edtXMax->setValue(g.xmax);
            ui->edtYMin->setValue(g.ymin);
            ui->edtYMax->setValue(g.ymax);
            ui->cmbFontname->setCurrentFont(QFont(g.fontName));
            ui->spinAxisFontSize->setValue(g.axisFontSize);
            ui->spinAxisLabelFontSize->setValue(g.axisLabelFontSize);
            ui->spinTitleFontSize->setValue(g.labelFontSize);
            ui->spinKeyFontSize->setValue(g.keyFontSize);
            ui->listGraphs->clear();
            QList<QFRDRTable::GraphInfo> graphs=g.graphs;
            for (int i=0; i<g.graphs.size(); i++) {
                QListWidgetItem* w=new QListWidgetItem( ui->listGraphs);
                w->setText(graphs.at(i).title);
                w->setIcon(QFRDRTable::GraphType2Icon(graphs.at(i).type));
                ui->listGraphs->addItem(w);
            }
        } else {
            ui->tabSystem->setVisible(false);
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
        ui->cmbErrorColor->setCurrentColor(graph.errorColor);
        ui->cmbLineColor->setCurrentColor(graph.color);
        ui->cmbFillColor->setCurrentColor(graph.fillColor);
        ui->cmbErrorStyle->setSymbol(graph.errorStyle);
        ui->cmbSymbol->setSymbol(graph.symbol);
        ui->cmbLineStyle->setCurrentLineStyle(graph.style);
        ui->spinLineWidth->setValue(graph.linewidth);
        ui->spinSymbolSize->setValue(graph.symbolSize);
        ui->chkDrawLine->setChecked(graph.drawLine);
        ui->sliderPlotTransparent->setValue(graph.colorTransparent*255.0);
        ui->sliderErrorTransparent->setValue(graph.errorColorTransparent*255.0);
        ui->sliderFillTransparent->setValue(graph.fillColorTransparent*255.0);
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

void QFRDRTablePlotWidget::on_btnCloneGraph_clicked()
{
    if (!current) return;
    if (current) {
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;
        int r=ui->listGraphs->currentRow();
        QFRDRTable::PlotInfo p=current->getPlot(this->plot);
        if (r>=0 && r<p.graphs.size()) {


            disconnectWidgets();
            int r=ui->listGraphs->currentRow();
            QFRDRTable::PlotInfo p=current->getPlot(this->plot);
            QFRDRTable::GraphInfo g=p.graphs.at(r);
            g.title=tr("copy of %1").arg(g.title);
            p.graphs.append(g);
            updating=true;
            QListWidgetItem* w=new QListWidgetItem( ui->listGraphs);
            w->setText(g.title);
            w->setIcon(QFRDRTable::GraphType2Icon(g.type));
            ui->listGraphs->addItem(w);
            current->setPlot(this->plot, p);
            updating=false;
            connectWidgets();
            ui->listGraphs->setCurrentRow(p.graphs.size()-1);
            ui->edtGraphTitle->setFocus();


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
        g.color=autocolors.value(p.graphs.size()+1, QColor("red"));
        g.linewidth=2;
        g.fillColorTransparent=0.5;
        g.fillColor=g.color;
        g.errorColor=g.color.darker();
        p.graphs.append(g);
        updating=true;
        QListWidgetItem* w=new QListWidgetItem( ui->listGraphs);
        w->setText(g.title);
        w->setIcon(QFRDRTable::GraphType2Icon(g.type));
        ui->listGraphs->addItem(w);
        current->setPlot(this->plot, p);
        updating=false;
        connectWidgets();
        ui->listGraphs->setCurrentRow(p.graphs.size()-1);
        ui->edtGraphTitle->setFocus();
    }
}

void QFRDRTablePlotWidget::on_btnAutoscaleX_clicked()
{
    ui->plotter->zoomToFit(true, false);
    updating=true;
    disconnectWidgets();
    ui->edtXMin->setValue(ui->plotter->getXMin());
    ui->edtXMax->setValue(ui->plotter->getXMax());
    updating=false;
    connectWidgets();
    plotDataChanged();
}

void QFRDRTablePlotWidget::on_btnAutoscaleY_clicked() {
    ui->plotter->zoomToFit(false, true);
    updating=true;
    disconnectWidgets();
    ui->edtYMin->setValue(ui->plotter->getYMin());
    ui->edtYMax->setValue(ui->plotter->getYMax());
    updating=false;
    connectWidgets();
    plotDataChanged();
}

void QFRDRTablePlotWidget::on_plotter_plotMouseMove(double x, double y)
{
    labPlotPosition->setText(tr("position: (%1, %2)").arg(floattohtmlstr(x).c_str()).arg(floattohtmlstr(y).c_str()));
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
    qDebug()<<"graphDataChanged    updating="<<updating;
    if (updating) return;
    if (current) {
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;
        int r=ui->listGraphs->currentRow();
        QFRDRTable::PlotInfo p=current->getPlot(this->plot);
        if (r>=0 && r<p.graphs.size()) {
            QFRDRTable::GraphInfo graph=p.graphs.at(r);
            graph.title=ui->edtGraphTitle->text();
            ui->listGraphs->item(r)->setText(graph.title);
            ui->listGraphs->item(r)->setIcon(ui->cmbGraphType->itemIcon(ui->cmbGraphType->currentIndex()));


            ui->cmbErrorColor->setEnabled(true);
            ui->cmbErrorStyle->setEnabled(true);
            ui->cmbLinesXError->setEnabled(true);
            ui->cmbLinesYError->setEnabled(true);
            ui->chkDrawLine->setEnabled(true);
            ui->cmbSymbol->setEnabled(true);
            ui->spinSymbolSize->setEnabled(true);
            ui->cmbFillColor->setEnabled(true);
            ui->cmbLineStyle->setEnabled(true);
            ui->sliderErrorTransparent->setEnabled(true);
            ui->sliderFillTransparent->setEnabled(true);
            ui->sliderPlotTransparent->setEnabled(true);

            switch(ui->cmbGraphType->currentIndex()) {
                case 1:
                    graph.type=QFRDRTable::gtImpulsesVertical;
                    ui->cmbLinesXError->setEnabled(false);
                    ui->chkDrawLine->setEnabled(false);
                    ui->cmbLineStyle->setEnabled(false);
                    break;
                case 2:
                    graph.type=QFRDRTable::gtImpulsesHorizontal;
                    ui->cmbErrorColor->setEnabled(true);
                    ui->cmbErrorStyle->setEnabled(true);
                    ui->cmbLinesXError->setEnabled(false);
                    ui->cmbLinesYError->setEnabled(true);
                    ui->chkDrawLine->setEnabled(false);
                    ui->cmbSymbol->setEnabled(true);
                    ui->spinSymbolSize->setEnabled(true);
                    ui->cmbFillColor->setEnabled(true);
                    ui->cmbLineStyle->setEnabled(false);
                    break;
                case 3:
                    graph.type=QFRDRTable::gtFilledCurveX;
                    ui->cmbErrorColor->setEnabled(true);
                    ui->cmbErrorStyle->setEnabled(true);
                    ui->cmbLinesXError->setEnabled(false);
                    ui->cmbLinesYError->setEnabled(true);
                    ui->chkDrawLine->setEnabled(false);
                    ui->cmbSymbol->setEnabled(false);
                    ui->spinSymbolSize->setEnabled(false);
                    ui->cmbFillColor->setEnabled(true);
                    ui->cmbLineStyle->setEnabled(true);
                    break;
                case 4:
                    graph.type=QFRDRTable::gtFilledCurveY;
                    ui->cmbErrorColor->setEnabled(true);
                    ui->cmbErrorStyle->setEnabled(true);
                    ui->cmbLinesXError->setEnabled(false);
                    ui->cmbLinesYError->setEnabled(true);
                    ui->chkDrawLine->setEnabled(false);
                    ui->cmbSymbol->setEnabled(false);
                    ui->spinSymbolSize->setEnabled(false);
                    ui->cmbFillColor->setEnabled(true);
                    ui->cmbLineStyle->setEnabled(true);
                    break;
                case 5:
                    graph.type=QFRDRTable::gtStepsVertical;
                    ui->cmbErrorColor->setEnabled(false);
                    ui->cmbErrorStyle->setEnabled(false);
                    ui->cmbLinesXError->setEnabled(false);
                    ui->cmbLinesYError->setEnabled(false);
                    ui->chkDrawLine->setEnabled(true);
                    ui->cmbSymbol->setEnabled(false);
                    ui->spinSymbolSize->setEnabled(false);
                    ui->cmbFillColor->setEnabled(true);
                    ui->cmbLineStyle->setEnabled(true);
                    ui->sliderErrorTransparent->setEnabled(false);
                    break;
                case 6:
                    graph.type=QFRDRTable::gtStepsHorizontal;
                    ui->cmbErrorColor->setEnabled(false);
                    ui->cmbErrorStyle->setEnabled(false);
                    ui->cmbLinesXError->setEnabled(false);
                    ui->cmbLinesYError->setEnabled(false);
                    ui->chkDrawLine->setEnabled(true);
                    ui->cmbSymbol->setEnabled(false);
                    ui->spinSymbolSize->setEnabled(false);
                    ui->cmbFillColor->setEnabled(true);
                    ui->cmbLineStyle->setEnabled(true);
                    ui->sliderErrorTransparent->setEnabled(false);
                    break;
                case 7:
                    graph.type=QFRDRTable::gtbarsVertical;
                    ui->cmbErrorColor->setEnabled(false);
                    ui->cmbErrorStyle->setEnabled(false);
                    ui->cmbLinesXError->setEnabled(false);
                    ui->cmbLinesYError->setEnabled(false);
                    ui->chkDrawLine->setEnabled(false);
                    ui->cmbSymbol->setEnabled(false);
                    ui->spinSymbolSize->setEnabled(false);
                    ui->cmbFillColor->setEnabled(true);
                    ui->cmbLineStyle->setEnabled(true);
                    ui->sliderErrorTransparent->setEnabled(false);
                    break;
                case 8:
                    graph.type=QFRDRTable::gtbarsHorizontal;
                    ui->cmbErrorColor->setEnabled(false);
                    ui->cmbErrorStyle->setEnabled(false);
                    ui->cmbLinesXError->setEnabled(false);
                    ui->cmbLinesYError->setEnabled(false);
                    ui->chkDrawLine->setEnabled(false);
                    ui->cmbSymbol->setEnabled(false);
                    ui->spinSymbolSize->setEnabled(false);
                    ui->cmbFillColor->setEnabled(true);
                    ui->cmbLineStyle->setEnabled(true);
                    ui->sliderErrorTransparent->setEnabled(false);
                    break;
                case 0:
                default:
                    graph.type=QFRDRTable::gtLines;
                    break;
            }



            graph.xcolumn=qMax(-1, ui->cmbLinesXData->currentIndex()-1);
            graph.xerrorcolumn=qMax(-1, ui->cmbLinesXError->currentIndex()-1);
            graph.ycolumn=qMax(-1, ui->cmbLinesYData->currentIndex()-1);
            graph.yerrorcolumn=qMax(-1, ui->cmbLinesYError->currentIndex()-1);

            graph.errorColor=ui->cmbErrorColor->currentColor();
            graph.color=ui->cmbLineColor->currentColor();
            graph.fillColor=ui->cmbFillColor->currentColor();
            graph.errorStyle=ui->cmbErrorStyle->getSymbol();
            graph.symbol=ui->cmbSymbol->getSymbol();
            graph.style=ui->cmbLineStyle->currentLineStyle();
            graph.linewidth=ui->spinLineWidth->value();
            graph.symbolSize=ui->spinSymbolSize->value();
            graph.drawLine=ui->chkDrawLine->isChecked();
            graph.colorTransparent=double(ui->sliderPlotTransparent->value())/255.0;
            graph.errorColorTransparent=double(ui->sliderErrorTransparent->value())/255.0;
            graph.fillColorTransparent=double(ui->sliderFillTransparent->value())/255.0;

            p.graphs[r]=graph;
            current->setPlot(this->plot, p);

        }
    }
    updateGraph();
}

void QFRDRTablePlotWidget::plotDataChanged() {
    qDebug()<<"plotDataChanged   updating="<<updating;
    if (updating) return;
    if (current) {
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;

        QFRDRTable::PlotInfo p=current->getPlot(this->plot);
        p.title=ui->edtTitle->text();
        p.xlabel=ui->edtXLabel->text();
        p.ylabel=ui->edtYLabel->text();
        emit plotTitleChanged(this->plot, p.title);
        p.xlog=ui->chkLogX->isChecked();
        p.ylog=ui->chkLogY->isChecked();
        p.grid=ui->chkGrid->isChecked();
        p.showKey=ui->chkShowKey->isChecked();
        p.xmin=ui->edtXMin->value();
        p.xmax=ui->edtXMax->value();
        p.ymin=ui->edtYMin->value();
        p.ymax=ui->edtYMax->value();
        p.fontName=ui->cmbFontname->currentFont().family();
        p.axisFontSize=ui->spinAxisFontSize->value();
        p.axisLabelFontSize=ui->spinAxisLabelFontSize->value();
        p.keyFontSize=ui->spinKeyFontSize->value();
        p.labelFontSize=ui->spinTitleFontSize->value();

        current->setPlot(this->plot, p);
        //QFRDRTable::GraphInfo graph=current->getPlot(this->plot).graphs.value(currentRow, QFRDRTable::GraphInfo());
    }
    updateGraph();
}

void QFRDRTablePlotWidget::updateGraph() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (current) {
        qDebug()<<"updateGraph  plot="<<this->plot+1<<"/"<<current->getPlotCount();
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;
        qDebug()<<"updateGraph";
        QFRDRTable::PlotInfo p=current->getPlot(this->plot);

        ui->plotter->set_doDrawing(false);


        ui->plotter->getXAxis()->set_axisLabel(p.xlabel);
        ui->plotter->getXAxis()->set_logAxis(p.xlog);
        ui->plotter->getXAxis()->set_labelFont(p.fontName);
        ui->plotter->getXAxis()->set_labelFontSize(p.axisLabelFontSize);
        ui->plotter->getXAxis()->set_tickLabelFont(p.fontName);
        ui->plotter->getXAxis()->set_tickLabelFontSize(p.axisFontSize);
        ui->plotter->getYAxis()->set_axisLabel(p.ylabel);
        ui->plotter->getYAxis()->set_logAxis(p.ylog);
        ui->plotter->getYAxis()->set_labelFont(p.fontName);
        ui->plotter->getYAxis()->set_labelFontSize(p.axisLabelFontSize);
        ui->plotter->getYAxis()->set_tickLabelFont(p.fontName);
        ui->plotter->getYAxis()->set_tickLabelFontSize(p.axisFontSize);
        ui->plotter->get_plotter()->set_plotLabelFontname(p.fontName);
        ui->plotter->get_plotter()->set_plotLabelFontSize(p.labelFontSize);
        ui->plotter->get_plotter()->set_plotLabel(p.title);
        ui->plotter->get_plotter()->set_keyFont(p.fontName);
        ui->plotter->get_plotter()->set_keyFontSize(p.keyFontSize);
        ui->plotter->setGrid(p.grid);
        ui->plotter->get_plotter()->set_showKey(p.showKey);
        ui->plotter->setXY(p.xmin, p.xmax, p.ymin, p.ymax);
        ui->plotter->clearGraphs(true);
        updateData();
        for (int i=0; i<p.graphs.size(); i++) {
            QFRDRTable::GraphInfo g=p.graphs.at(i);
            if (g.type==QFRDRTable::gtImpulsesVertical) {
                JKQTPimpulsesVerticalErrorGraph* pg=new JKQTPimpulsesVerticalErrorGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                if (g.xcolumn>=0 && g.xcolumn<ui->plotter->getDatastore()->getColumnCount())  pg->set_xColumn(g.xcolumn);
                if (g.ycolumn>=0 && g.ycolumn<ui->plotter->getDatastore()->getColumnCount()) pg->set_yColumn(g.ycolumn);
                /*if (g.xerrorcolumn>=0 && g.xerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) {
                    pg->set_xErrorColumn(g.xerrorcolumn);
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }*/
                if (g.yerrorcolumn>=0 && g.yerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) {
                    pg->set_yErrorColumn(g.yerrorcolumn);
                    pg->set_yErrorStyle(g.errorStyle);
                } else {
                    pg->set_yErrorStyle(JKQTPnoError);
                }
                //pg->set_drawLine(g.drawLine);
                //pg->set_symbol(g.symbol);
                //pg->set_symbolSize(g.symbolSize);
                pg->set_lineWidth(g.linewidth);
                QColor c=g.color;
                c.setAlphaF(g.colorTransparent);
                pg->set_color(c);
                QColor ec=g.errorColor;
                ec.setAlphaF(g.errorColorTransparent);
                pg->set_errorColor(ec);
                QColor efc=g.errorColor;
                efc.setAlphaF(qBound(0.0,1.0,g.errorColorTransparent-0.2));
                pg->set_errorFillColor(efc);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                //pg->set_fillColor(fc);
                //pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtImpulsesHorizontal) {
                JKQTPimpulsesHorizontalErrorGraph* pg=new JKQTPimpulsesHorizontalErrorGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                if (g.xcolumn>=0 && g.xcolumn<ui->plotter->getDatastore()->getColumnCount())  pg->set_yColumn(g.xcolumn);
                if (g.ycolumn>=0 && g.ycolumn<ui->plotter->getDatastore()->getColumnCount()) pg->set_xColumn(g.ycolumn);
                /*if (g.xerrorcolumn>=0 && g.xerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) {
                    pg->set_xErrorColumn(g.xerrorcolumn);
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }*/
                if (g.yerrorcolumn>=0 && g.yerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) {
                    pg->set_xErrorColumn(g.yerrorcolumn);
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }
                //pg->set_drawLine(g.drawLine);
                //pg->set_symbol(g.symbol);
                //pg->set_symbolSize(g.symbolSize);
                pg->set_lineWidth(g.linewidth);
                QColor c=g.color;
                c.setAlphaF(g.colorTransparent);
                pg->set_color(c);
                QColor ec=g.errorColor;
                ec.setAlphaF(g.errorColorTransparent);
                pg->set_errorColor(ec);
                QColor efc=g.errorColor;
                efc.setAlphaF(qBound(0.0,1.0,g.errorColorTransparent-0.2));
                pg->set_errorFillColor(efc);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                //pg->set_fillColor(fc);
                //pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtbarsHorizontal) {
                JKQTPbarVerticalGraph* pg=new JKQTPbarVerticalGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                if (g.xcolumn>=0 && g.xcolumn<ui->plotter->getDatastore()->getColumnCount())  pg->set_yColumn(g.xcolumn);
                if (g.ycolumn>=0 && g.ycolumn<ui->plotter->getDatastore()->getColumnCount()) pg->set_xColumn(g.ycolumn);
                /*if (g.xerrorcolumn>=0 && g.xerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) {
                    pg->set_xErrorColumn(g.xerrorcolumn);
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }*/
                /*if (g.yerrorcolumn>=0 && g.yerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) {
                    pg->set_yErrorColumn(g.yerrorcolumn);
                    pg->set_yErrorStyle(g.errorStyle);
                } else {
                    pg->set_yErrorStyle(JKQTPnoError);
                }*/
                //pg->set_drawLine(g.drawLine);
                //pg->set_symbol(g.symbol);
                //pg->set_symbolSize(g.symbolSize);
                pg->set_lineWidth(g.linewidth);
                QColor c=g.color;
                c.setAlphaF(g.colorTransparent);
                pg->set_color(c);
                QColor ec=g.errorColor;
                ec.setAlphaF(g.errorColorTransparent);
                //pg->set_errorColor(ec);
                QColor efc=g.errorColor;
                efc.setAlphaF(qBound(0.0,1.0,g.errorColorTransparent-0.2));
                //pg->set_errorFillColor(efc);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                pg->set_fillColor(fc);
                pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtbarsVertical) {
                JKQTPbarHorizontalGraph* pg=new JKQTPbarHorizontalGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                if (g.xcolumn>=0 && g.xcolumn<ui->plotter->getDatastore()->getColumnCount())  pg->set_xColumn(g.xcolumn);
                if (g.ycolumn>=0 && g.ycolumn<ui->plotter->getDatastore()->getColumnCount()) pg->set_yColumn(g.ycolumn);
                /*if (g.xerrorcolumn>=0 && g.xerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) {
                    pg->set_xErrorColumn(g.xerrorcolumn);
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }*/
                /*if (g.yerrorcolumn>=0 && g.yerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) {
                    pg->set_yErrorColumn(g.yerrorcolumn);
                    pg->set_yErrorStyle(g.errorStyle);
                } else {
                    pg->set_yErrorStyle(JKQTPnoError);
                }*/
                //pg->set_drawLine(g.drawLine);
                //pg->set_symbol(g.symbol);
                //pg->set_symbolSize(g.symbolSize);
                pg->set_lineWidth(g.linewidth);
                QColor c=g.color;
                c.setAlphaF(g.colorTransparent);
                pg->set_color(c);
                QColor ec=g.errorColor;
                ec.setAlphaF(g.errorColorTransparent);
                //pg->set_errorColor(ec);
                QColor efc=g.errorColor;
                efc.setAlphaF(qBound(0.0,1.0,g.errorColorTransparent-0.2));
                //pg->set_errorFillColor(efc);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                pg->set_fillColor(fc);
                pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtFilledCurveX) {
                JKQTPfilledCurveXErrorGraph* pg=new JKQTPfilledCurveXErrorGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                if (g.xcolumn>=0 && g.xcolumn<ui->plotter->getDatastore()->getColumnCount())  pg->set_xColumn(g.xcolumn);
                if (g.ycolumn>=0 && g.ycolumn<ui->plotter->getDatastore()->getColumnCount()) pg->set_yColumn(g.ycolumn);
                /*if (g.xerrorcolumn>=0 && g.xerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) {
                    pg->set_xErrorColumn(g.xerrorcolumn);
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }*/
                if (g.yerrorcolumn>=0 && g.yerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) {
                    pg->set_yErrorColumn(g.yerrorcolumn);
                    pg->set_yErrorStyle(g.errorStyle);
                } else {
                    pg->set_yErrorStyle(JKQTPnoError);
                }
                pg->set_drawLine(g.drawLine);
                //pg->set_symbol(g.symbol);
                //pg->set_symbolSize(g.symbolSize);
                pg->set_lineWidth(g.linewidth);
                QColor c=g.color;
                c.setAlphaF(g.colorTransparent);
                pg->set_color(c);
                QColor ec=g.errorColor;
                ec.setAlphaF(g.errorColorTransparent);
                pg->set_errorColor(ec);
                QColor efc=g.errorColor;
                efc.setAlphaF(qBound(0.0,1.0,g.errorColorTransparent-0.2));
                pg->set_errorFillColor(efc);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                pg->set_fillColor(fc);
                pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtFilledCurveY) {
                JKQTPfilledCurveYErrorGraph* pg=new JKQTPfilledCurveYErrorGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                if (g.xcolumn>=0 && g.xcolumn<ui->plotter->getDatastore()->getColumnCount())  pg->set_yColumn(g.xcolumn);
                if (g.ycolumn>=0 && g.ycolumn<ui->plotter->getDatastore()->getColumnCount()) pg->set_xColumn(g.ycolumn);
                /*if (g.xerrorcolumn>=0 && g.xerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) {
                    pg->set_xErrorColumn(g.xerrorcolumn);
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }*/
                if (g.yerrorcolumn>=0 && g.yerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) {
                    pg->set_xErrorColumn(g.yerrorcolumn);
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }
                pg->set_drawLine(g.drawLine);
                //pg->set_symbol(g.symbol);
                //pg->set_symbolSize(g.symbolSize);
                pg->set_lineWidth(g.linewidth);
                QColor c=g.color;
                c.setAlphaF(g.colorTransparent);
                pg->set_color(c);
                QColor ec=g.errorColor;
                ec.setAlphaF(g.errorColorTransparent);
                pg->set_errorColor(ec);
                QColor efc=g.errorColor;
                efc.setAlphaF(qBound(0.0,1.0,g.errorColorTransparent-0.2));
                pg->set_errorFillColor(efc);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                pg->set_fillColor(fc);
                pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtStepsHorizontal) {
                JKQTPstepHorizontalGraph* pg=new JKQTPstepHorizontalGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                if (g.xcolumn>=0 && g.xcolumn<ui->plotter->getDatastore()->getColumnCount())  pg->set_yColumn(g.xcolumn);
                if (g.ycolumn>=0 && g.ycolumn<ui->plotter->getDatastore()->getColumnCount()) pg->set_xColumn(g.ycolumn);
                /*if (g.xerrorcolumn>=0 && g.xerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) {
                    pg->set_xErrorColumn(g.xerrorcolumn);
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }*/
                /*if (g.yerrorcolumn>=0 && g.yerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) {
                    pg->set_yErrorColumn(g.yerrorcolumn);
                    pg->set_yErrorStyle(g.errorStyle);
                } else {
                    pg->set_yErrorStyle(JKQTPnoError);
                }*/
                pg->set_drawLine(g.drawLine);
                //pg->set_symbol(g.symbol);
                //pg->set_symbolSize(g.symbolSize);
                pg->set_lineWidth(g.linewidth);
                QColor c=g.color;
                c.setAlphaF(g.colorTransparent);
                pg->set_color(c);
                QColor ec=g.errorColor;
                ec.setAlphaF(g.errorColorTransparent);
                //pg->set_errorColor(ec);
                QColor efc=g.errorColor;
                efc.setAlphaF(qBound(0.0,1.0,g.errorColorTransparent-0.2));
                //pg->set_errorFillColor(efc);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                pg->set_fillColor(fc);
                pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtStepsVertical) {
                JKQTPstepVerticalGraph* pg=new JKQTPstepVerticalGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                if (g.xcolumn>=0 && g.xcolumn<ui->plotter->getDatastore()->getColumnCount())  pg->set_xColumn(g.xcolumn);
                if (g.ycolumn>=0 && g.ycolumn<ui->plotter->getDatastore()->getColumnCount()) pg->set_yColumn(g.ycolumn);
                /*if (g.xerrorcolumn>=0 && g.xerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) {
                    pg->set_xErrorColumn(g.xerrorcolumn);
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }*/
                /*if (g.yerrorcolumn>=0 && g.yerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) {
                    pg->set_yErrorColumn(g.yerrorcolumn);
                    pg->set_yErrorStyle(g.errorStyle);
                } else {
                    pg->set_yErrorStyle(JKQTPnoError);
                }*/
                pg->set_drawLine(g.drawLine);
                //pg->set_symbol(g.symbol);
                //pg->set_symbolSize(g.symbolSize);
                pg->set_lineWidth(g.linewidth);
                QColor c=g.color;
                c.setAlphaF(g.colorTransparent);
                pg->set_color(c);
                QColor ec=g.errorColor;
                ec.setAlphaF(g.errorColorTransparent);
                //pg->set_errorColor(ec);
                QColor efc=g.errorColor;
                efc.setAlphaF(qBound(0.0,1.0,g.errorColorTransparent-0.2));
                //pg->set_errorFillColor(efc);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                pg->set_fillColor(fc);
                pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            } else { // gtLines etc.
                JKQTPxyLineErrorGraph* pg=new JKQTPxyLineErrorGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                if (g.xcolumn>=0 && g.xcolumn<ui->plotter->getDatastore()->getColumnCount())  pg->set_xColumn(g.xcolumn);
                if (g.ycolumn>=0 && g.ycolumn<ui->plotter->getDatastore()->getColumnCount()) pg->set_yColumn(g.ycolumn);
                if (g.xerrorcolumn>=0 && g.xerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) {
                    pg->set_xErrorColumn(g.xerrorcolumn);
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }
                if (g.yerrorcolumn>=0 && g.yerrorcolumn<ui->plotter->getDatastore()->getColumnCount()) {
                    pg->set_yErrorColumn(g.yerrorcolumn);
                    pg->set_yErrorStyle(g.errorStyle);
                } else {
                    pg->set_yErrorStyle(JKQTPnoError);
                }
                pg->set_drawLine(g.drawLine);
                pg->set_symbol(g.symbol);
                pg->set_symbolSize(g.symbolSize);
                pg->set_lineWidth(g.linewidth);
                QColor c=g.color;
                c.setAlphaF(g.colorTransparent);
                pg->set_color(c);
                QColor ec=g.errorColor;
                ec.setAlphaF(g.errorColorTransparent);
                pg->set_errorColor(ec);
                QColor efc=g.errorColor;
                efc.setAlphaF(qBound(0.0,1.0,g.errorColorTransparent-0.2));
                pg->set_errorFillColor(efc);
                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                pg->set_fillColor(fc);
                pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            }
        }

        ui->plotter->set_doDrawing(true);
        ui->plotter->update_plot();
    }
    QApplication::restoreOverrideCursor();
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
    qDebug()<<"connectWidgets";
    connect(ui->listGraphs, SIGNAL(currentRowChanged(int)), this, SLOT(listGraphs_currentRowChanged(int)));

    connect(ui->edtTitle, SIGNAL(textChanged(QString)), this, SLOT(plotDataChanged()));
    connect(ui->edtXLabel, SIGNAL(textChanged(QString)), this, SLOT(plotDataChanged()));
    connect(ui->edtYLabel, SIGNAL(textChanged(QString)), this, SLOT(plotDataChanged()));
    connect(ui->chkGrid, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    connect(ui->chkLogX, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    connect(ui->chkLogY, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    connect(ui->chkShowKey, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    connect(ui->edtXMin, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->edtXMax, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->edtYMin, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->edtYMax, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->cmbFontname, SIGNAL(currentIndexChanged(QString)), this, SLOT(plotDataChanged()));
    connect(ui->spinAxisFontSize, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->spinAxisLabelFontSize, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->spinKeyFontSize, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->spinTitleFontSize, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));

    connect(ui->edtGraphTitle, SIGNAL(textChanged(QString)), this, SLOT(graphDataChanged()));
    connect(ui->cmbGraphType, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->cmbLinesXData, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->cmbLinesXError, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->cmbLinesYData, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->cmbLinesYError, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->cmbFillColor, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->cmbErrorColor, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->cmbErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->cmbLineColor, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->cmbLineStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->cmbSymbol, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->chkDrawLine, SIGNAL(toggled(bool)), this, SLOT(graphDataChanged()));
    connect(ui->spinLineWidth, SIGNAL(valueChanged(double)), this, SLOT(graphDataChanged()));
    connect(ui->spinSymbolSize, SIGNAL(valueChanged(double)), this, SLOT(graphDataChanged()));
    connect(ui->sliderErrorTransparent, SIGNAL(valueChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->sliderFillTransparent, SIGNAL(valueChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->sliderPlotTransparent, SIGNAL(valueChanged(int)), this, SLOT(graphDataChanged()));
}

void QFRDRTablePlotWidget::disconnectWidgets()
{
    qDebug()<<"disconnectWidgets";
    disconnect(ui->listGraphs, SIGNAL(currentRowChanged(int)), this, SLOT(listGraphs_currentRowChanged(int)));

    disconnect(ui->edtTitle, SIGNAL(textChanged(QString)), this, SLOT(plotDataChanged()));
    disconnect(ui->edtXLabel, SIGNAL(textChanged(QString)), this, SLOT(plotDataChanged()));
    disconnect(ui->edtYLabel, SIGNAL(textChanged(QString)), this, SLOT(plotDataChanged()));
    disconnect(ui->chkGrid, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->chkLogX, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->chkLogY, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->chkShowKey, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->edtXMin, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->edtXMax, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->edtYMin, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->edtYMax, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbFontname, SIGNAL(currentIndexChanged(QString)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinAxisFontSize, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinAxisLabelFontSize, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinKeyFontSize, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinTitleFontSize, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));


    disconnect(ui->edtGraphTitle, SIGNAL(textChanged(QString)), this, SLOT(graphDataChanged()));
    disconnect(ui->cmbGraphType, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->cmbLinesXData, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->cmbLinesXError, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->cmbLinesYData, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->cmbLinesYError, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->cmbErrorColor, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->cmbFillColor, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->cmbErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->cmbLineColor, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->cmbLineStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->cmbSymbol, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->chkDrawLine, SIGNAL(toggled(bool)), this, SLOT(graphDataChanged()));
    disconnect(ui->spinLineWidth, SIGNAL(valueChanged(double)), this, SLOT(graphDataChanged()));
    disconnect(ui->spinSymbolSize, SIGNAL(valueChanged(double)), this, SLOT(graphDataChanged()));
    disconnect(ui->sliderErrorTransparent, SIGNAL(valueChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->sliderFillTransparent, SIGNAL(valueChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->sliderPlotTransparent, SIGNAL(valueChanged(int)), this, SLOT(graphDataChanged()));
}
