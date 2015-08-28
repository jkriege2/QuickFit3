/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "qfrdrtableplotwidget.h"
#include "ui_qfrdrtableplotwidget.h"
#include "programoptions.h"
#include "qfrdrtable.h"
#include "qftools.h"
#include "qfdoubleedit.h"
#include "qfmathparserxfunctionlinegraph.h"
#include "qffitfunctionplottools.h"
#include "qffitfunctionmanager.h"
#include "qffitfunction.h"
#include "dlgcolorbarcoloring.h"

#define DELAYEDUPDATEGRAPH_DELAY 200


QFRDRTablePlotWidget::QFRDRTablePlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFRDRTablePlotWidget)
{
    autocolors.append(QColor("red"));
    autocolors.append(QColor("green"));
    autocolors.append(QColor("blue"));
    autocolors.append(QColor("magenta"));
    autocolors.append(QColor("darkcyan"));
    autocolors.append(QColor("darkred"));
    autocolors.append(QColor("darkgreen"));
    autocolors.append(QColor("darkblue"));
    autocolors.append(QColor("darkmagenta"));
    autocolors.append(QColor("fuchsia"));
    autocolors.append(QColor("darkorange"));
    autocolors.append(QColor("firebrick"));
    autocolors.append(QColor("darkgreen"));
    autocolors.append(QColor("darkslateblue"));
    autocolors.append(QColor("maroon"));
    autocolors.append(QColor("indianred"));
    autocolors.append(QColor("darkolivegreen"));
    autocolors.append(QColor("mediumpurple"));



    updating=true;
    ui->setupUi(this);

    //ui->scrollArea->setWidget();

    connect(ui->widGraphSettings, SIGNAL(performRefit(int)), this, SLOT(doRefit(int)));
    connect(ui->widGraphSettings, SIGNAL(performFit(int,int,int,int,QString,QFRDRTable::GraphDataSelection)), this, SLOT(doFit(int,int,int,int,QString,QFRDRTable::GraphDataSelection)));
    connect(ui->widGraphSettings, SIGNAL(performRegression(int,int,int,int, QFRDRTable::GraphDataSelection)), this, SLOT(doRegression(int,int,int,int,QFRDRTable::GraphDataSelection)));
    connect(ui->widSystemSettings, SIGNAL(plotSettingsChanged()), this, SLOT(delayedUpdateGraph()));
    connect(ui->widGraphSettings, SIGNAL(reloadGraph()), this, SLOT(delayedReloadGraphData()));

    //ui->formLayout_3->removeWidget(ui->widSaveCoordSettings);
    //ui->tabWidget->setCornerWidget(ui->widSaveCoordSettings);

    cmbMagnification=new QComboBox(this);
    cmbMagnification->addItem(tr("10%"), 0.1);
    cmbMagnification->addItem(tr("20%"), 0.2);
    cmbMagnification->addItem(tr("25%"), 0.25);
    cmbMagnification->addItem(tr("50%"), 0.5);
    cmbMagnification->addItem(tr("75%"), 0.75);
    cmbMagnification->addItem(tr("100%"), 1);
    cmbMagnification->addItem(tr("125%"), 1.25);
    cmbMagnification->addItem(tr("150%"), 1.5);
    cmbMagnification->addItem(tr("175%"), 1.75);
    cmbMagnification->addItem(tr("200%"), 2.0);
    cmbMagnification->addItem(tr("250%"), 2.5);
    cmbMagnification->addItem(tr("300%"), 3.0);
    cmbMagnification->addItem(tr("400%"), 4.0);
    cmbMagnification->addItem(tr("500%"), 5.0);
    cmbMagnification->addItem(tr("600%"), 6.0);
    connect(cmbMagnification, SIGNAL(currentIndexChanged(int)), this, SLOT(magnificationChanged(int)));



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
    toolbarPlot->addWidget(new QLabel(tr("zoom: ")));
    toolbarPlot->addWidget(cmbMagnification);
    labPlotPosition=new QLabel("", this);
    toolbarPlot->addWidget(labPlotPosition);
    current=NULL;
    this->plot=-1;
    updating=false;

    ui->tabWidget->setCurrentIndex(0);
    timUpdateGraph.setInterval(DELAYEDUPDATEGRAPH_DELAY);
    timUpdateGraph.setSingleShot(true);
    timUpdateData.setInterval(DELAYEDUPDATEGRAPH_DELAY);
    timUpdateData.setSingleShot(true);
    timReloadGraphData.setInterval(DELAYEDUPDATEGRAPH_DELAY);
    timReloadGraphData.setSingleShot(true);
    timPlotDataChanged.setInterval(DELAYEDUPDATEGRAPH_DELAY);
    timPlotDataChanged.setSingleShot(true);
    timGraphDataChanged.setInterval(DELAYEDUPDATEGRAPH_DELAY);
    timGraphDataChanged.setSingleShot(true);
    connect(&timUpdateGraph, SIGNAL(timeout()), this, SLOT(updateGraph()));
    connect(&timUpdateData, SIGNAL(timeout()), this, SLOT(updateData()));
    connect(&timReloadGraphData, SIGNAL(timeout()), this, SLOT(reloadGraphData()));
    connect(&timPlotDataChanged, SIGNAL(timeout()), this, SLOT(plotDataChanged()));
    connect(&timGraphDataChanged, SIGNAL(timeout()), this, SLOT(graphDataChanged()));

    connect(ui->widSystemSettings, SIGNAL(autoscaleX()), this, SLOT(doAutoscaleX()));
    connect(ui->widSystemSettings, SIGNAL(autoscaleY()), this, SLOT(doAutoscaleY()));
    connect(ui->widSystemSettings, SIGNAL(autoscaleXY()), this, SLOT(doAutoscaleXY()));


    if (ProgramOptions::getInstance() && ProgramOptions::getInstance()->getQSettings()) readSettings(*(ProgramOptions::getInstance()->getQSettings()), "table/QFRDRTablePlotWidget/");
}

QFRDRTablePlotWidget::~QFRDRTablePlotWidget()
{
    updating=true;
    if (ProgramOptions::getInstance() && ProgramOptions::getInstance()->getQSettings()) writeSettings(*(ProgramOptions::getInstance()->getQSettings()), "table/QFRDRTablePlotWidget/");
    delete ui;
}

void QFRDRTablePlotWidget::setRecord(QFRDRTable *record, int graph)
{

    current=record;
    this->plot=graph;
    updating=true;
    disconnectWidgets();
    int oldrow=ui->listGraphs->currentRow();
    if (record) {
        if (graph>=0 && graph<record->getPlotCount()) {
            ui->tabSystem->setEnabled(true);
            QFRDRTable::PlotInfo g=record->getPlot(plot);

            ui->widSystemSettings->setRecord(record, graph);



            ui->listGraphs->clear();
            QList<QFRDRTable::GraphInfo> graphs=g.graphs;
            for (int i=0; i<g.graphs.size(); i++) {
                QListWidgetItem* w=new QListWidgetItem( ui->listGraphs);
                w->setText(graphs.at(i).title);
                w->setIcon(QFRDRTable::GraphType2Icon(graphs.at(i).type));
                ui->listGraphs->addItem(w);
            }
            cmbMagnification->setCurrentIndex(cmbMagnification->findData(record->getProperty(QString("GRAPH%1_MAGNIFICATION").arg(graph), 1.0).toDouble()));
        } else {
            ui->tabSystem->setEnabled(false);
        }
    }
    ui->listGraphs->setCurrentRow(qMax(0, oldrow));
    ui->widGraphSettings->setRecord(record, plot);
    updating=false;
    connectWidgets();
    listGraphs_currentRowChanged(ui->listGraphs->currentRow());
    updateGraph();

}

void QFRDRTablePlotWidget::rawDataChanged() {
    if (updating) return;
    disconnectWidgets();
    ui->widGraphSettings->rawDataChanged();
    connectWidgets();
    updateGraph();
}

void QFRDRTablePlotWidget::readSettings(QSettings &settings, const QString &prefix)
{
    loadSplitter(settings, ui->splitter, prefix+"splitter/");
    ui->widGraphSettings->readSettings(settings, prefix);
}

void QFRDRTablePlotWidget::writeSettings(QSettings &settings, const QString &prefix)
{
    saveSplitter(settings, ui->splitter, prefix+"splitter/");
    ui->widGraphSettings->writeSettings(settings, prefix);
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
        ui->widGraphSettings->loadGraphData(graph);
        ui->widGraphSettings->show();
        ui->widGraphSettings->repaint();

        updatePlotWidgetVisibility();
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
            listGraphs_currentRowChanged(ui->listGraphs->currentRow());
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
            ui->widGraphSettings->initFocus();


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
        autoColorGraph(g, p.graphs.size()+1);
        g.linewidth=2;
        g.fillColorTransparent=0.5;
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
        ui->widGraphSettings->initFocus();
    }
}

void QFRDRTablePlotWidget::doAutoscaleX()
{
    ui->plotter->zoomToFit(true, false);
    updating=true;
    //qDebug()<<"autoX: "<<ui->plotter->getXMin()<<ui->plotter->getXMax();
    ui->widSystemSettings->setXRange(ui->plotter->getXMin(), ui->plotter->getXMax());
    updating=false;
    plotDataChanged();
}

void QFRDRTablePlotWidget::doAutoscaleY() {
    ui->plotter->zoomToFit(false, true);
    updating=true;
    //qDebug()<<"autoY: "<<ui->plotter->getYMin()<<ui->plotter->getYMax();
    ui->widSystemSettings->setYRange(ui->plotter->getYMin(), ui->plotter->getYMax());
    updating=false;
    plotDataChanged();
}

void QFRDRTablePlotWidget::doAutoscaleXY()
{
    ui->plotter->zoomToFit(true, true);
    updating=true;
    //qDebug()<<"autoX: "<<ui->plotter->getXMin()<<ui->plotter->getXMax();
    ui->widSystemSettings->setXRange(ui->plotter->getXMin(), ui->plotter->getXMax());
    ui->widSystemSettings->setYRange(ui->plotter->getYMin(), ui->plotter->getYMax());
    updating=false;
    plotDataChanged();

}

void QFRDRTablePlotWidget::on_plotter_plotMouseMove(double x, double y)
{
    labPlotPosition->setText(tr("position: (%1, %2)").arg(floattohtmlstr(x).c_str()).arg(floattohtmlstr(y).c_str()));
}



void QFRDRTablePlotWidget::graphDataChanged() {
    //qDebug()<<"graphDataChanged    updating="<<updating;
    if (updating) return;
    if (current) {
        updating=true;
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;
        int r=ui->listGraphs->currentRow();
        QFRDRTable::PlotInfo p=current->getPlot(this->plot);
        if (r>=0 && r<p.graphs.size()) {
            QFRDRTable::GraphInfo graph=p.graphs.at(r);


            updatePlotWidgetVisibility();


            ui->widGraphSettings->writeGraphData(graph);
            ui->listGraphs->item(r)->setText(graph.title);
            ui->listGraphs->item(r)->setIcon(QFRDRTable::GraphType2Icon(graph.type));

            ui->widSystemSettings->graphDataChanged(r);


            p.graphs[r]=graph;
            current->setPlot(this->plot, p);

        }
        updating=false;
    }
    updateGraph();
}

void QFRDRTablePlotWidget::delayedGraphDataChanged()
{
    timGraphDataChanged.stop();
    timGraphDataChanged.setSingleShot(true);
    timGraphDataChanged.setInterval(DELAYEDUPDATEGRAPH_DELAY);
    timGraphDataChanged.start();
}

void QFRDRTablePlotWidget::plotDataChanged() {
    //qDebug()<<"plotDataChanged   updating="<<updating;
    if (updating) return;
    if (current) {
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;

        ui->widSystemSettings->plotDataChanged();
        //QFRDRTable::GraphInfo graph=current->getPlot(this->plot).graphs.value(currentRow, QFRDRTable::GraphInfo());
    }
    updateGraph();
}

void QFRDRTablePlotWidget::delayedPlotDataChanged()
{
    timPlotDataChanged.stop();
    timPlotDataChanged.setSingleShot(true);
    timPlotDataChanged.setInterval(DELAYEDUPDATEGRAPH_DELAY);
    timPlotDataChanged.start();
}


void QFRDRTablePlotWidget::setAxisProps(JKQTPcoordinateAxis* axis, const QFRDRTable::AxisInfo& axisData, const QFRDRTable::PlotInfo& p, bool minorGrid, bool majorGrid) {
    axis->clearAxisTickLabels();
    axis->set_axisLabel(axisData.label);
    axis->set_logAxis(axisData.log);
    axis->set_showZeroAxis(axisData.axis0);
    axis->set_labelFont(p.fontName);
    axis->set_labelFontSize(p.axisLabelFontSize);
    axis->set_tickLabelFont(p.fontName);
    axis->set_tickLabelFontSize(p.axisFontSize);
    axis->set_minorTickLabelFontSize(p.axisMinorFontSize);
    axis->set_minorTickLabelFullNumber(!axisData.minorTickLabelsOnlyDigit);
    if (axisData.LinTicksForLogAxis && axisData.log) {
        axis->set_tickMode(JKQTPLTMLin);
    } else{
        axis->set_tickMode(JKQTPLTMLinOrPower);
    }
    axis->set_minorTickLabelsEnabled(axisData.minorTickLabels);
    axis->set_drawGrid(p.grid&&majorGrid);
    axis->set_gridWidth(p.gridWidth);
    axis->set_gridStyle(p.gridStyle);
    axis->set_gridColor(p.gridColor);
    axis->set_minorGridWidth(p.gridWidthMinor);
    axis->set_minorGridStyle(p.gridStyleMinor);
    axis->set_minorGridColor(p.gridColorMinor);
    axis->set_drawMinorGrid(p.gridMinor&&minorGrid);
    axis->set_minTicks(axisData.minTicks);
    axis->set_minorTicks(axisData.minorTicks);
    axis->set_labelPosition(axisData.labelPos);
    axis->set_labelType(axisData.labelType);
    axis->set_autoLabelDigits(false);
    axis->set_labelDigits(axisData.digits);
    axis->set_drawMode1(axisData.labelMode1);
    axis->set_drawMode2(axisData.labelMode2);
    axis->set_tickLabelAngle(axisData.labelAngel);
    axis->set_lineWidth(axisData.axis1LineWidth);
    axis->set_lineWidthZeroAxis(axisData.zeroAxisLineWidth);


    axis->set_userTickSpacing(axisData.TickSpacing);
    axis->set_userLogTickSpacing(axisData.TickSpacingLog);
    axis->set_autoAxisSpacing(axisData.AutoTicks);
    axis->set_tickSpacing(axisData.TickSpacing);
    axis->set_userTickSpacing(axisData.TickSpacing);
    axis->set_tickWidth(axisData.AxisTickWidth);
    axis->set_minorTickWidth(axisData.AxisMinorTickWidth);
    axis->set_inverted(axisData.AxisInverted);
    axis->set_tickInsideLength(axisData.TickInsideLength);
    axis->set_tickOutsideLength(axisData.TickOutsideLength);
    axis->set_minorTickOutsideLength(axisData.MinorTickOutsideLength);
    axis->set_minorTickInsideLength(axisData.MinorTickInsideLength);

    if (axisData.columnNamedTickNames!=-1 && axisData.columnNamedTickValues!=-1 && current && current->model()) {
        QVector<double> nums=current->model()->getColumnDataAsNumbers(axisData.columnNamedTickValues);
        QVariantList labels=current->model()->getColumnData(axisData.columnNamedTickNames);
        int cnt=qMin(nums.size(), labels.size());
        //qDebug()<<nums<<labels<<cnt;
        axis->clearAxisTickLabels();
        for (int i=0; i<cnt; i++) {
            axis->addAxisTickLabel(nums.value(i, i), labels.value(i, QString()).toString());
        }
    }

}

void QFRDRTablePlotWidget::updateGraph() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (current) {
        //qDebug()<<"updateGraph  plot="<<this->plot+1<<"/"<<current->getPlotCount();
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;
        //qDebug()<<"updateGraph";
        QFRDRTable::PlotInfo p=current->getPlot(this->plot);

        //qDebug()<<logicalDpiX()<<"dpi:   "<<p.graphWidth<<"mm = "<<ui->plotter->get_plotter()->mm2px(p.graphWidth, logicalDpiX())<<"px";

        ui->plotter->set_doDrawing(false);
        ui->plotter->set_emitSignals(false);

        double m=getMagnification();

        if (p.graphAutosize) {
            ui->scrollArea->setWidgetResizable(true);
        } else {
            ui->scrollArea->setWidgetResizable(false);
            ui->plotter->resize(ui->plotter->get_plotter()->mm2px(p.graphWidth, logicalDpiX())*m, ui->plotter->get_plotter()->mm2px(p.graphHeight, logicalDpiX())*m);
        }

        ui->plotter->setMagnification(m);

        setAxisProps( ui->plotter->getXAxis(), p.xAxis, p, p.gridMinorX, p.gridMajorX);
        setAxisProps( ui->plotter->getYAxis(), p.yAxis, p, p.gridMinorY, p.gridMajorY);


        ui->plotter->get_plotter()->set_plotLabelFontname(p.fontName);
        ui->plotter->get_plotter()->set_plotLabelFontSize(p.labelFontSize);
        if (p.showTitle) ui->plotter->get_plotter()->set_plotLabel(p.title);
        else ui->plotter->get_plotter()->set_plotLabel("");
        ui->plotter->get_plotter()->set_keyFont(p.fontName);
        ui->plotter->get_plotter()->set_keyFontSize(p.keyFontSize);
        ui->plotter->get_plotter()->set_maintainAspectRatio(p.keepDataAspectRatio);
        ui->plotter->get_plotter()->set_maintainAxisAspectRatio(p.keepAxisAspectRatio);
        ui->plotter->get_plotter()->set_aspectRatio(p.dataAspectRatio);
        ui->plotter->get_plotter()->set_axisAspectRatio(p.axisAspectRatio);
        ui->plotter->get_plotter()->set_plotBackgroundColor(p.backgroundColor);
        QFont keyf(p.fontName);
        keyf.setPointSizeF(p.keyFontSize);
        QFontMetricsF keyfm(keyf);
        //ui->plotter->get_plotter()->set_key_item_height(qMax(15.0, 1.0*keyfm.height()));
        ui->plotter->setGrid(p.grid);
        QColor kb=p.keyBackgroundColor;
        kb.setAlphaF(p.keyTransparency);
        ui->plotter->get_plotter()->set_keyBackgroundColor(kb);
        ui->plotter->get_plotter()->set_keyFrameColor(p.keyLineColor);
        ui->plotter->get_plotter()->set_keyFrameWidth(p.keyBoxLineWidth);
        ui->plotter->get_plotter()->set_showKeyFrame(p.keyBox);
        ui->plotter->get_plotter()->set_keyLayout(p.keyLayout);
        ui->plotter->get_plotter()->set_keyPosition(p.keyPosition);
        ui->plotter->get_plotter()->set_showKey(p.showKey);
        ui->plotter->setXY(p.xAxis.min, p.xAxis.max, p.yAxis.min, p.yAxis.max);

        ui->plotter->get_plotter()->set_keyXMargin(p.keyXMargin);
        ui->plotter->get_plotter()->set_keyYMargin(p.keyYMargin);
        ui->plotter->get_plotter()->set_keyXOffset(p.keyXOffset);
        ui->plotter->get_plotter()->set_keyYOffset(p.keyYOffset);
        ui->plotter->get_plotter()->set_keyXSeparation(p.keyXSeparation);
        ui->plotter->get_plotter()->set_keyYSeparation(p.keyYSeparation);
        ui->plotter->get_plotter()->set_key_line_length(p.key_line_length);


        ui->plotter->clearGraphs(true);
        updateData();
        for (int i=0; i<p.graphs.size(); i++) {
            QFRDRTable::GraphInfo g=p.graphs.at(i);
            if (g.type==QFRDRTable::gtImpulsesVertical) {
                JKQTPimpulsesVerticalErrorGraph* pg=new JKQTPimpulsesVerticalErrorGraph(ui->plotter->get_plotter());
                if (g.titleShow) pg->set_title(g.title); else pg->set_title("");
                pg->set_xColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_yColumn(getColumnWithStride(g.ycolumn, g));
                pg->set_sortData(g.dataSortOrder);
                pg->set_baseline(g.offset);
                /*pg->set_xErrorColumn(getColumnWithStride(g.xerrorcolumn, g));
                if (pg->get_xErrorColumn()>=0) {
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }*/

                pg->set_yErrorColumn(getColumnWithStride(g.yerrorcolumn, g));
                if (pg->get_yErrorColumn()>=0) {
                    pg->set_yErrorStyle(g.errorStyle);
                } else {
                    pg->set_yErrorStyle(JKQTPnoError);
                }
                if (!g.yerrorsymmetric) {
                    pg->set_yErrorSymmetric(false);
                    pg->set_yErrorColumnLower(getColumnWithStride(g.yerrorcolumnlower, g));
                    if (pg->get_yErrorColumn()>=0 || pg->get_yErrorColumnLower()>=0 ) {
                        pg->set_yErrorStyle(g.errorStyle);
                    }
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

                pg->set_errorWidth(g.errorWidth);
                pg->set_errorStyle(g.errorLineStyle);
                pg->set_errorbarSize(g.errorBarSize);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                //pg->set_fillColor(fc);
                //pg->set_style(g.style);
                //pg->set_fillStyle(g.fillStyle);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtImpulsesHorizontal) {
                JKQTPimpulsesHorizontalErrorGraph* pg=new JKQTPimpulsesHorizontalErrorGraph(ui->plotter->get_plotter());
                if (g.titleShow) pg->set_title(g.title); else pg->set_title("");
                pg->set_xColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_yColumn(getColumnWithStride(g.ycolumn, g));
                pg->set_xErrorColumn(getColumnWithStride(g.yerrorcolumn, g));
                pg->set_baseline(g.offset);
                pg->set_sortData(g.dataSortOrder);
                if (pg->get_xErrorColumn()>=0) {
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }
                if (!g.xerrorsymmetric) {
                    pg->set_xErrorSymmetric(false);
                    pg->set_xErrorColumnLower(getColumnWithStride(g.xerrorcolumnlower, g));
                    if (pg->get_xErrorColumn()>=0 || pg->get_xErrorColumnLower()>=0 ) {
                        pg->set_xErrorStyle(g.errorStyle);
                    }
                }
                /*pg->set_yErrorColumn(getColumnWithStride(g.yerrorcolumn, g));
                if (pg->get_yErrorColumn()>=0) {
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
                pg->set_errorColor(ec);
                QColor efc=g.errorColor;
                efc.setAlphaF(qBound(0.0,1.0,g.errorColorTransparent-0.2));
                pg->set_errorFillColor(efc);
                pg->set_errorWidth(g.errorWidth);
                pg->set_errorStyle(g.errorLineStyle);
                pg->set_errorbarSize(g.errorBarSize);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                //pg->set_fillColor(fc);
                //pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtBarsHorizontal) {
                JKQTPbarVerticalGraph* pg=new JKQTPbarVerticalGraph(ui->plotter->get_plotter());
                if (g.titleShow) pg->set_title(g.title); else pg->set_title("");
                pg->set_xColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_yColumn(getColumnWithStride(g.ycolumn, g));
                pg->set_width(g.width);
                pg->set_sortData(g.dataSortOrder);
                pg->set_shift(g.shift);
                pg->set_baseline(g.offset);
                pg->set_baseline(g.offset);
                pg->set_xErrorColumn(getColumnWithStride(g.xerrorcolumn, g));
                //pg->set_yErrorColumn(getColumnWithStride(g.yerrorcolumn, g));
                if (pg->get_xErrorColumn()>=0) {
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }
                if (!g.xerrorsymmetric) {
                    pg->set_xErrorSymmetric(false);
                    pg->set_xErrorColumnLower(getColumnWithStride(g.xerrorcolumnlower, g));
                    if (pg->get_xErrorColumn()>=0 || pg->get_xErrorColumnLower()>=0 ) {
                        pg->set_xErrorStyle(g.errorStyle);
                    }
                }
                pg->set_yErrorStyle(JKQTPnoError);
                /*pg->set_yErrorColumn(getColumnWithStride(g.yerrorcolumn, g));
                if (pg->get_yErrorColumn()>=0) {
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
                pg->set_errorColor(ec);
                QColor efc=g.errorColor;
                efc.setAlphaF(qBound(0.0,1.0,g.errorColorTransparent-0.2));
                pg->set_errorFillColor(efc);
                pg->set_errorWidth(g.errorWidth);
                pg->set_errorStyle(g.errorLineStyle);
                pg->set_errorbarSize(g.errorBarSize);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                pg->set_fillColor(fc);
                pg->set_style(g.style);
                pg->set_fillStyle(g.fillStyle);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtBarsVertical) {
                JKQTPbarHorizontalGraph* pg=new JKQTPbarHorizontalGraph(ui->plotter->get_plotter());
                if (g.titleShow) pg->set_title(g.title); else pg->set_title("");
                pg->set_xColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_yColumn(getColumnWithStride(g.ycolumn, g));
                pg->set_baseline(g.offset);
                pg->set_sortData(g.dataSortOrder);
                //pg->set_xErrorColumn(getColumnWithStride(g.xerrorcolumn, g));
                pg->set_yErrorColumn(getColumnWithStride(g.yerrorcolumn, g));
                pg->set_width(g.width);
                pg->set_shift(g.shift);
                /*pg->set_xErrorColumn(getColumnWithStride(g.xerrorcolumn, g));
                if (pg->get_xErrorColumn()>=0) {
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }*/
                pg->set_yErrorColumn(getColumnWithStride(g.yerrorcolumn, g));
                if (pg->get_yErrorColumn()>=0) {
                    pg->set_yErrorStyle(g.errorStyle);
                } else {
                    pg->set_yErrorStyle(JKQTPnoError);
                }
                if (!g.yerrorsymmetric) {
                    pg->set_yErrorSymmetric(false);
                    pg->set_yErrorColumnLower(getColumnWithStride(g.yerrorcolumnlower, g));
                    if (pg->get_yErrorColumn()>=0 || pg->get_yErrorColumnLower()>=0 ) {
                        pg->set_yErrorStyle(g.errorStyle);
                    }
                }
                pg->set_xErrorStyle(JKQTPnoError);
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
                pg->set_errorWidth(g.errorWidth);
                pg->set_errorStyle(g.errorLineStyle);
                pg->set_errorbarSize(g.errorBarSize);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                pg->set_fillColor(fc);
                pg->set_style(g.style);
                pg->set_fillStyle(g.fillStyle);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtFilledCurveX) {
                JKQTPfilledCurveXErrorGraph* pg=new JKQTPfilledCurveXErrorGraph(ui->plotter->get_plotter());
                if (g.titleShow) pg->set_title(g.title); else pg->set_title("");
                pg->set_xColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_yColumn(getColumnWithStride(g.ycolumn, g));
                pg->set_baseline(g.offset);
                pg->set_sortData(g.dataSortOrder);
                /*pg->set_xErrorColumn(getColumnWithStride(g.xerrorcolumn, g));
                if (pg->get_xErrorColumn()>=0) {
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }*/
                pg->set_yErrorColumn(getColumnWithStride(g.yerrorcolumn, g));
                if (pg->get_yErrorColumn()>=0) {
                    pg->set_yErrorStyle(g.errorStyle);
                } else {
                    pg->set_yErrorStyle(JKQTPnoError);
                }
                if (!g.yerrorsymmetric) {
                    pg->set_yErrorSymmetric(false);
                    pg->set_yErrorColumnLower(getColumnWithStride(g.yerrorcolumnlower, g));
                    if (pg->get_yErrorColumn()>=0 || pg->get_yErrorColumnLower()>=0 ) {
                        pg->set_yErrorStyle(g.errorStyle);
                    }
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
                pg->set_errorWidth(g.errorWidth);
                pg->set_errorStyle(g.errorLineStyle);
                pg->set_errorbarSize(g.errorBarSize);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                pg->set_fillColor(fc);
                pg->set_fillStyle(g.fillStyle);
                pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtFilledCurveY) {
                JKQTPfilledCurveYErrorGraph* pg=new JKQTPfilledCurveYErrorGraph(ui->plotter->get_plotter());
                if (g.titleShow) pg->set_title(g.title); else pg->set_title("");
                pg->set_xColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_yColumn(getColumnWithStride(g.ycolumn, g));
                pg->set_xErrorColumn(getColumnWithStride(g.yerrorcolumn, g));
                pg->set_baseline(g.offset);
                pg->set_sortData(g.dataSortOrder);
                if (pg->get_xErrorColumn()>=0) {
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }
                if (!g.xerrorsymmetric) {
                    pg->set_xErrorSymmetric(false);
                    pg->set_xErrorColumnLower(getColumnWithStride(g.xerrorcolumnlower, g));
                    if (pg->get_xErrorColumn()>=0 || pg->get_xErrorColumnLower()>=0 ) {
                        pg->set_xErrorStyle(g.errorStyle);
                    }
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
                pg->set_errorWidth(g.errorWidth);
                pg->set_errorStyle(g.errorLineStyle);
                pg->set_errorbarSize(g.errorBarSize);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                pg->set_fillColor(fc);
                pg->set_fillStyle(g.fillStyle);
                pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtStepsHorizontal) {
                JKQTPstepHorizontalGraph* pg=new JKQTPstepHorizontalGraph(ui->plotter->get_plotter());
                pg->set_fillCurve(true);
                if (g.titleShow) pg->set_title(g.title); else pg->set_title("");
                pg->set_xColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_yColumn(getColumnWithStride(g.ycolumn, g));
                pg->set_sortData(g.dataSortOrder);
                /*pg->set_xErrorColumn(getColumnWithStride(g.xerrorcolumn, g));
                if (pg->get_xErrorColumn()>=0) {
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }*/
                /*pg->set_yErrorColumn(getColumnWithStride(g.yerrorcolumn, g));
                if (pg->get_yErrorColumn()>=0) {
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
//                pg->set_errorWidth(g.errorWidth);
//                pg->set_errorStyle(g.errorLineStyle);
//                pg->set_errorbarSize(g.errorbarSize);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                pg->set_fillColor(fc);
                pg->set_fillStyle(g.fillStyle);
                pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtStepsVertical) {
                JKQTPstepVerticalGraph* pg=new JKQTPstepVerticalGraph(ui->plotter->get_plotter());
                if (g.titleShow) pg->set_title(g.title); else pg->set_title("");
                pg->set_xColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_yColumn(getColumnWithStride(g.ycolumn, g));
                pg->set_sortData(g.dataSortOrder);
                /*pg->set_xErrorColumn(getColumnWithStride(g.xerrorcolumn, g));
                if (pg->get_xErrorColumn()>=0) {
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }*/
                /*pg->set_yErrorColumn(getColumnWithStride(g.yerrorcolumn, g));
                if (pg->get_yErrorColumn()>=0) {
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
                pg->set_fillCurve(true);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                pg->set_fillColor(fc);
                pg->set_fillStyle(g.fillStyle);
                pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtParametrizedScatter) { // gtLines etc.
                JKQTPxyParametrizedErrorScatterGraph* pg=new JKQTPxyParametrizedErrorScatterGraph(ui->plotter->get_plotter());
                if (g.titleShow) pg->set_title(g.title); else pg->set_title("");
                pg->set_xColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_yColumn(getColumnWithStride(g.ycolumn, g));
                pg->set_colorColumn(getColumnWithStride(g.meancolumn, g));
                pg->set_sizeColumn(getColumnWithStride(g.q75column, g));
                pg->set_xErrorColumn(getColumnWithStride(g.xerrorcolumn, g));
                pg->set_sortData(g.dataSortOrder);
                if (pg->get_xErrorColumn()>=0) {
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }
                if (!g.xerrorsymmetric) {
                    pg->set_xErrorSymmetric(false);
                    pg->set_xErrorColumnLower(getColumnWithStride(g.xerrorcolumnlower, g));
                    if (pg->get_xErrorColumn()>=0 || pg->get_xErrorColumnLower()>=0 ) {
                        pg->set_xErrorStyle(g.errorStyle);
                    }
                }
                pg->set_yErrorColumn(getColumnWithStride(g.yerrorcolumn, g));
                if (pg->get_yErrorColumn()>=0) {
                    pg->set_yErrorStyle(g.errorStyle);
                } else {
                    pg->set_yErrorStyle(JKQTPnoError);
                }
                if (!g.yerrorsymmetric) {
                    pg->set_yErrorSymmetric(false);
                    pg->set_yErrorColumnLower(getColumnWithStride(g.yerrorcolumnlower, g));
                    if (pg->get_yErrorColumn()>=0 || pg->get_yErrorColumnLower()>=0 ) {
                        pg->set_yErrorStyle(g.errorStyle);
                    }
                }
                pg->set_errorWidth(g.errorWidth);
                pg->set_errorStyle(g.errorLineStyle);
                pg->set_errorbarSize(g.errorBarSize);

                //qDebug()<<g.title<<pg->get_xColumn()<<pg->get_yColumn();
                pg->set_drawLine(g.drawLine);
                pg->set_symbol(g.symbol);
                pg->set_symbolSize(g.symbolSize);
                pg->set_symbolWidth(g.symbolLineWidth);
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


                pg->set_showColorBar(true);
                pg->set_palette(g.imagePalette);
                pg->set_colorBarRightVisible(g.imageColorbarRight);
                pg->set_colorBarTopVisible(g.imageColorbarTop);
                pg->set_colorBarRelativeHeight(g.colorbarRelativeHeight);
                pg->set_colorBarWidth(g.colorbarWidth);
                pg->get_colorBarRightAxis()->set_tickLabelFont(p.fontName);
                pg->get_colorBarRightAxis()->set_tickLabelFontSize(g.imageColorbarFontsize);
                pg->get_colorBarRightAxis()->set_axisLabel(g.imageLegend);
                pg->get_colorBarRightAxis()->set_labelFont(p.fontName);
                pg->get_colorBarRightAxis()->set_labelFontSize(g.imageColorbarFontsize);
                pg->get_colorBarRightAxis()->set_minorTickInsideLength(0);
                pg->get_colorBarRightAxis()->set_tickInsideLength(0);

                pg->get_colorBarRightAxis()->set_minorTickOutsideLength(g.imageColorbarTicklength/2.0);
                pg->get_colorBarRightAxis()->set_tickOutsideLength(g.imageColorbarTicklength);
                pg->get_colorBarRightAxis()->set_minTicks(g.imageTicks);
                pg->get_colorBarRightAxis()->set_minorTicks(0);
                pg->get_colorBarTopAxis()->set_minorTickOutsideLength(g.imageColorbarTicklength/2.0);
                pg->get_colorBarTopAxis()->set_tickOutsideLength(g.imageColorbarTicklength);
                pg->get_colorBarTopAxis()->set_minTicks(g.imageTicks);
                pg->get_colorBarTopAxis()->set_minorTicks(0);


                pg->get_colorBarRightAxis()->set_labelType(g.imageColorbarLabelType);
                pg->get_colorBarRightAxis()->set_autoLabelDigits(false);
                pg->get_colorBarRightAxis()->set_labelDigits(g.imageColorbarLabelDigits);
                pg->get_colorBarTopAxis()->set_labelType(g.imageColorbarLabelType);
                pg->get_colorBarTopAxis()->set_autoLabelDigits(false);
                pg->get_colorBarTopAxis()->set_labelDigits(g.imageColorbarLabelDigits);


                pg->get_colorBarTopAxis()->set_tickLabelFont(p.fontName);
                pg->get_colorBarTopAxis()->set_tickLabelFontSize(g.imageColorbarFontsize);
                pg->get_colorBarTopAxis()->set_axisLabel(g.imageLegend);
                pg->get_colorBarTopAxis()->set_labelFont(p.fontName);
                pg->get_colorBarTopAxis()->set_labelFontSize(g.imageColorbarFontsize);


                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtImage) {
                JKQTPColumnMathImage* pg=new JKQTPColumnMathImage(ui->plotter->get_plotter());
                if (g.titleShow) pg->set_title(g.title); else pg->set_title("");

                if (g.xcolumn>=0 && g.xcolumn<(long)ui->plotter->getDatastore()->getColumnCount())  pg->set_imageColumn(g.xcolumn);
                if (g.ycolumn>=0 && g.ycolumn<(long)ui->plotter->getDatastore()->getColumnCount())  pg->set_modifierColumn(g.ycolumn);
                pg->set_autoImageRange(g.imageAutoRange);
                pg->set_imageMin(g.imageMin);
                pg->set_imageMax(g.imageMax);
                pg->set_x(g.imageX);
                pg->set_y(g.imageY);
                pg->set_width(g.imageWidth);
                pg->set_height(g.imageHeight);
                pg->set_Nx(g.imagePixelWidth);
                pg->set_palette(g.imagePalette);
                pg->set_colorBarRightVisible(g.imageColorbarRight);
                pg->set_colorBarTopVisible(g.imageColorbarTop);
                pg->set_colorBarRelativeHeight(g.colorbarRelativeHeight);
                pg->set_colorBarWidth(g.colorbarWidth);
                pg->set_colorBarModifiedWidth(g.colorbarWidth);
                /*pg->set_imageName(g.imageLegend);
                pg->set_imageNameFontName(p.fontName);
                pg->set_imageNameFontSize(p.axisLabelFontSize);*/
                pg->get_colorBarRightAxis()->set_tickLabelFont(p.fontName);
                pg->get_colorBarRightAxis()->set_tickLabelFontSize(g.imageColorbarFontsize);
                pg->get_colorBarRightAxis()->set_axisLabel(g.imageLegend);
                pg->get_colorBarRightAxis()->set_labelFont(p.fontName);
                pg->get_colorBarRightAxis()->set_labelFontSize(g.imageColorbarFontsize);
                pg->get_colorBarRightAxis()->set_minorTickInsideLength(0);
                pg->get_colorBarRightAxis()->set_tickInsideLength(0);

                pg->get_colorBarRightAxis()->set_minorTickOutsideLength(g.imageColorbarTicklength/2.0);
                pg->get_colorBarRightAxis()->set_tickOutsideLength(g.imageColorbarTicklength);
                pg->get_colorBarRightAxis()->set_minTicks(g.imageTicks);
                pg->get_colorBarRightAxis()->set_minorTicks(0);
                pg->get_colorBarTopAxis()->set_minorTickOutsideLength(g.imageColorbarTicklength/2.0);
                pg->get_colorBarTopAxis()->set_tickOutsideLength(g.imageColorbarTicklength);
                pg->get_colorBarTopAxis()->set_minTicks(g.imageTicks);
                pg->get_colorBarTopAxis()->set_minorTicks(0);


                pg->get_colorBarRightAxis()->set_labelType(g.imageColorbarLabelType);
                pg->get_colorBarRightAxis()->set_autoLabelDigits(false);
                pg->get_colorBarRightAxis()->set_labelDigits(g.imageColorbarLabelDigits);
                pg->get_colorBarTopAxis()->set_labelType(g.imageColorbarLabelType);
                pg->get_colorBarTopAxis()->set_autoLabelDigits(false);
                pg->get_colorBarTopAxis()->set_labelDigits(g.imageColorbarLabelDigits);


                pg->get_colorBarTopAxis()->set_tickLabelFont(p.fontName);
                pg->get_colorBarTopAxis()->set_tickLabelFontSize(g.imageColorbarFontsize);
                pg->get_colorBarTopAxis()->set_axisLabel(g.imageLegend);
                pg->get_colorBarTopAxis()->set_labelFont(p.fontName);
                pg->get_colorBarTopAxis()->set_labelFontSize(g.imageColorbarFontsize);

                pg->get_modifierColorBarRightAxis()->set_tickLabelFont(p.fontName);
                pg->get_modifierColorBarRightAxis()->set_tickLabelFontSize(g.imageColorbarFontsize);
                pg->get_modifierColorBarRightAxis()->set_axisLabel(g.imageLegendMod);
                pg->get_modifierColorBarRightAxis()->set_labelFont(p.fontName);
                pg->get_modifierColorBarRightAxis()->set_labelFontSize(g.imageColorbarFontsize);
                pg->get_modifierColorBarTopAxis()->set_tickLabelFont(p.fontName);
                pg->get_modifierColorBarTopAxis()->set_tickLabelFontSize(g.imageColorbarFontsize);
                pg->get_modifierColorBarTopAxis()->set_axisLabel(g.imageLegendMod);
                pg->get_modifierColorBarTopAxis()->set_labelFont(p.fontName);
                pg->get_modifierColorBarTopAxis()->set_labelFontSize(g.imageColorbarFontsize);

                pg->get_modifierColorBarRightAxis()->set_minorTickOutsideLength(g.imageColorbarTicklength/2.0);
                pg->get_modifierColorBarRightAxis()->set_tickOutsideLength(g.imageColorbarTicklength);
                pg->get_modifierColorBarRightAxis()->set_minTicks(g.imageModTicks);
                pg->get_modifierColorBarRightAxis()->set_minorTicks(0);
                pg->get_modifierColorBarTopAxis()->set_minorTickOutsideLength(g.imageColorbarTicklength/2.0);
                pg->get_modifierColorBarTopAxis()->set_tickOutsideLength(g.imageColorbarTicklength);
                pg->get_modifierColorBarTopAxis()->set_minTicks(g.imageModTicks);
                pg->get_modifierColorBarTopAxis()->set_minorTicks(0);

                pg->get_modifierColorBarRightAxis()->set_labelType(g.imageColorbarLabelType);
                pg->get_modifierColorBarRightAxis()->set_autoLabelDigits(false);
                pg->get_modifierColorBarRightAxis()->set_labelDigits(g.imageColorbarLabelDigits);
                pg->get_modifierColorBarTopAxis()->set_labelType(g.imageColorbarLabelType);
                pg->get_modifierColorBarTopAxis()->set_autoLabelDigits(false);
                pg->get_modifierColorBarTopAxis()->set_labelDigits(g.imageColorbarLabelDigits);

                pg->set_modifierMode(g.modifierMode);
                ui->plotter->addGraph(pg);

            } else if (g.type==QFRDRTable::gtRGBImage) {
                JKQTPColumnRGBMathImage* pg=new JKQTPColumnRGBMathImage(ui->plotter->get_plotter());
                if (g.titleShow) pg->set_title(g.title); else pg->set_title("");

                if (g.xcolumn>=0 && g.xcolumn<(long)ui->plotter->getDatastore()->getColumnCount())  pg->set_imageRColumn(g.xcolumn);
                if (g.xerrorcolumn>=0 && g.xerrorcolumn<(long)ui->plotter->getDatastore()->getColumnCount())  pg->set_imageGColumn(g.xerrorcolumn);
                if (g.ycolumn>=0 && g.ycolumn<(long)ui->plotter->getDatastore()->getColumnCount())  pg->set_imageBColumn(g.ycolumn);
                if (g.yerrorcolumn>=0 && g.yerrorcolumn<(long)ui->plotter->getDatastore()->getColumnCount())  pg->set_modifierColumn(g.yerrorcolumn);
                pg->set_autoImageRange(g.imageAutoRange);
                pg->set_imageMin(g.imageMin);
                pg->set_imageMax(g.imageMax);
                pg->set_x(g.imageX);
                pg->set_y(g.imageY);
                pg->set_width(g.imageWidth);
                pg->set_height(g.imageHeight);
                pg->set_Nx(g.imagePixelWidth);
                pg->set_colorBarRightVisible(g.imageColorbarRight);
                pg->set_colorBarTopVisible(g.imageColorbarTop);
                pg->set_colorBarRelativeHeight(g.colorbarRelativeHeight);
                pg->set_colorBarWidth(g.colorbarWidth);
                //pg->set_colorBarModifiedWidth(g.colorbarWidth);
                pg->set_imageName(g.imageLegend);
                pg->set_imageNameG(g.imageLegendG);
                pg->set_imageNameB(g.imageLegendB);
                /*pg->set_imageName(g.imageLegend);
                pg->set_imageNameFontName(p.fontName);
                pg->set_imageNameFontSize(p.axisLabelFontSize);*/
                pg->get_colorBarRightAxis()->set_tickLabelFont(p.fontName);
                pg->get_colorBarRightAxis()->set_tickLabelFontSize(g.imageColorbarFontsize);
                pg->get_colorBarRightAxis()->set_axisLabel(g.imageLegend);
                pg->get_colorBarRightAxis()->set_labelFont(p.fontName);
                pg->get_colorBarRightAxis()->set_labelFontSize(g.imageColorbarFontsize);
                pg->get_colorBarTopAxis()->set_tickLabelFont(p.fontName);
                pg->get_colorBarTopAxis()->set_tickLabelFontSize(g.imageColorbarFontsize);
                pg->get_colorBarTopAxis()->set_axisLabel(g.imageLegend);
                pg->get_colorBarTopAxis()->set_labelFont(p.fontName);
                pg->get_colorBarTopAxis()->set_labelFontSize(g.imageColorbarFontsize);
                pg->get_colorBarRightAxis()->set_minorTickOutsideLength(g.imageColorbarTicklength/2.0);
                pg->get_colorBarRightAxis()->set_tickOutsideLength(g.imageColorbarTicklength);
                pg->get_colorBarRightAxis()->set_minTicks(g.imageTicks);
                pg->get_colorBarRightAxis()->set_minorTicks(0);
                pg->get_colorBarTopAxis()->set_minorTickOutsideLength(g.imageColorbarTicklength/2.0);
                pg->get_colorBarTopAxis()->set_tickOutsideLength(g.imageColorbarTicklength);
                pg->get_colorBarTopAxis()->set_minTicks(g.imageTicks);
                pg->get_colorBarTopAxis()->set_minorTicks(0);

                pg->get_colorBarRightAxis()->set_labelType(g.imageColorbarLabelType);
                pg->get_colorBarRightAxis()->set_autoLabelDigits(false);
                pg->get_colorBarRightAxis()->set_labelDigits(g.imageColorbarLabelDigits);
                pg->get_colorBarTopAxis()->set_labelType(g.imageColorbarLabelType);
                pg->get_colorBarTopAxis()->set_autoLabelDigits(false);
                pg->get_colorBarTopAxis()->set_labelDigits(g.imageColorbarLabelDigits);

                pg->get_colorBarRightAxisG()->set_tickLabelFont(p.fontName);
                pg->get_colorBarRightAxisG()->set_tickLabelFontSize(g.imageColorbarFontsize);
                pg->get_colorBarRightAxisG()->set_axisLabel(g.imageLegendG);
                pg->get_colorBarRightAxisG()->set_labelFont(p.fontName);
                pg->get_colorBarRightAxisG()->set_labelFontSize(g.imageColorbarFontsize);
                pg->get_colorBarTopAxisG()->set_tickLabelFont(p.fontName);
                pg->get_colorBarTopAxisG()->set_tickLabelFontSize(g.imageColorbarFontsize);
                pg->get_colorBarTopAxisG()->set_axisLabel(g.imageLegendG);
                pg->get_colorBarTopAxisG()->set_labelFont(p.fontName);
                pg->get_colorBarTopAxisG()->set_labelFontSize(g.imageColorbarFontsize);
                pg->get_colorBarRightAxisG()->set_minorTickOutsideLength(g.imageColorbarTicklength/2.0);
                pg->get_colorBarRightAxisG()->set_tickOutsideLength(g.imageColorbarTicklength);
                pg->get_colorBarRightAxisG()->set_minTicks(g.imageTicks);
                pg->get_colorBarRightAxisG()->set_minorTicks(0);
                pg->get_colorBarTopAxisG()->set_minorTickOutsideLength(g.imageColorbarTicklength/2.0);
                pg->get_colorBarTopAxisG()->set_tickOutsideLength(g.imageColorbarTicklength);
                pg->get_colorBarTopAxisG()->set_minTicks(g.imageTicks);
                pg->get_colorBarTopAxisG()->set_minorTicks(0);

                pg->get_colorBarRightAxisG()->set_labelType(g.imageColorbarLabelType);
                pg->get_colorBarRightAxisG()->set_autoLabelDigits(false);
                pg->get_colorBarRightAxisG()->set_labelDigits(g.imageColorbarLabelDigits);
                pg->get_colorBarTopAxisG()->set_labelType(g.imageColorbarLabelType);
                pg->get_colorBarTopAxisG()->set_autoLabelDigits(false);
                pg->get_colorBarTopAxisG()->set_labelDigits(g.imageColorbarLabelDigits);

                pg->get_colorBarRightAxisB()->set_tickLabelFont(p.fontName);
                pg->get_colorBarRightAxisB()->set_tickLabelFontSize(g.imageColorbarFontsize);
                pg->get_colorBarRightAxisB()->set_axisLabel(g.imageLegendB);
                pg->get_colorBarRightAxisB()->set_labelFont(p.fontName);
                pg->get_colorBarRightAxisB()->set_labelFontSize(g.imageColorbarFontsize);
                pg->get_colorBarTopAxisB()->set_tickLabelFont(p.fontName);
                pg->get_colorBarTopAxisB()->set_tickLabelFontSize(g.imageColorbarFontsize);
                pg->get_colorBarTopAxisB()->set_axisLabel(g.imageLegendB);
                pg->get_colorBarTopAxisB()->set_labelFont(p.fontName);
                pg->get_colorBarTopAxisB()->set_labelFontSize(g.imageColorbarFontsize);
                pg->get_colorBarRightAxisB()->set_minorTickOutsideLength(g.imageColorbarTicklength/2.0);
                pg->get_colorBarRightAxisB()->set_tickOutsideLength(g.imageColorbarTicklength);
                pg->get_colorBarRightAxisB()->set_minTicks(g.imageTicks);
                pg->get_colorBarRightAxisB()->set_minorTicks(0);
                pg->get_colorBarTopAxisB()->set_minorTickOutsideLength(g.imageColorbarTicklength/2.0);
                pg->get_colorBarTopAxisB()->set_tickOutsideLength(g.imageColorbarTicklength);
                pg->get_colorBarTopAxisB()->set_minTicks(g.imageTicks);
                pg->get_colorBarTopAxisB()->set_minorTicks(0);

                pg->get_colorBarRightAxisB()->set_labelType(g.imageColorbarLabelType);
                pg->get_colorBarRightAxisB()->set_autoLabelDigits(false);
                pg->get_colorBarRightAxisB()->set_labelDigits(g.imageColorbarLabelDigits);
                pg->get_colorBarTopAxisB()->set_labelType(g.imageColorbarLabelType);
                pg->get_colorBarTopAxisB()->set_autoLabelDigits(false);
                pg->get_colorBarTopAxisB()->set_labelDigits(g.imageColorbarLabelDigits);

                pg->set_modifierMode(g.modifierMode);
                ui->plotter->addGraph(pg);

            } else if (g.type==QFRDRTable::gtMaskImage) {
                JKQTPColumnOverlayImageEnhanced* pg=new JKQTPColumnOverlayImageEnhanced(ui->plotter->get_plotter());
                if (g.titleShow) pg->set_title(g.title); else pg->set_title("");

                if (g.xcolumn>=0 && g.xcolumn<(long)ui->plotter->getDatastore()->getColumnCount())  pg->set_imageColumn(g.xcolumn);
                pg->set_x(g.imageX);
                pg->set_y(g.imageY);
                pg->set_width(g.imageWidth);
                pg->set_height(g.imageHeight);
                pg->set_Nx(g.imagePixelWidth);
                pg->set_rectanglesAsImageOverlay(true);
                QColor c=g.imageTrueColor;
                c.setAlphaF(g.imageTrueTransparent);
                pg->set_trueColor(c);
                c=g.imageFalseColor;
                c.setAlphaF(g.imageFalseTransparent);
                pg->set_falseColor(c);

                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtFunction) {
                QList<JKQTPxFunctionLineGraph*> pgs;

                int vecItems=0;
                if (g.functionType==QFRDRTable::gtfString) {
                    QFMathParserXFunctionLineGraph* pgf=new QFMathParserXFunctionLineGraph(ui->plotter->get_plotter());
                    pgf->set_function(g.function);
                    pgs<<pgf;

                } else if (g.functionType==QFRDRTable::gtfPolynomial) {
                    JKQTPxFunctionLineGraph* pgf=new JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
                    pgf->setSpecialFunction(JKQTPxFunctionLineGraph::Polynomial);
                    pgs<<pgf;
                    vecItems=11;
                } else if (g.functionType==QFRDRTable::gtfExponential) {
                    JKQTPxFunctionLineGraph* pgf=new JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
                    pgf->setSpecialFunction(JKQTPxFunctionLineGraph::Exponential);
                    pgs<<pgf;
                    vecItems=3;
                } else if (g.functionType==QFRDRTable::gtfPowerLaw) {
                    JKQTPxFunctionLineGraph* pgf=new JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
                    pgf->setSpecialFunction(JKQTPxFunctionLineGraph::PowerLaw);
                    pgs<<pgf;
                    vecItems=3;
                } else if (g.functionType==QFRDRTable::gtfQFFunction) {
                    JKQTPxQFFitFunctionLineGraph* pgf=new JKQTPxQFFitFunctionLineGraph(ui->plotter->get_plotter());
                    QFFitFunction* ff=NULL;
                    pgf->set_fitFunction(ff=QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(g.function), true);
                    pgs<<pgf;
                    if (!g.showallsubfunctions) {
                        pgf->set_subfunction(g.subfunction);
                    } else {
                        for (unsigned int i=0; i<ff->getAdditionalPlotCount(ff->getInitialParamValues()); i++) {
                            JKQTPxQFFitFunctionLineGraph* pgfsub=new JKQTPxQFFitFunctionLineGraph(ui->plotter->get_plotter());
                            QFFitFunction* ff=NULL;
                            pgfsub->set_fitFunction(ff=QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(g.function), true);
                            pgfsub->set_subfunction(i);
                            pgs<<pgfsub;
                        }
                    }
                    if (ff) vecItems=ff->paramCount();
                }


                for (int i=0; i<pgs.size(); i++) {
                    JKQTPxFunctionLineGraph* pg=pgs[i];
                    if (pg) {
                        if (g.ycolumn>=0 && g.ycolumn<(long)ui->plotter->getDatastore()->getColumnCount()) {
                            pg->set_parameterColumn(g.ycolumn);
                        } else {
                            pg->set_params(g.functionParameters);
                        }
                        if (g.titleShow && i==0) pg->set_title(g.title); else pg->set_title("");
                        //qDebug()<<"adding function plot "<<g.function;
                        pg->set_drawLine(true);
                        pg->set_lineWidth(g.linewidth);
                        if (i>0) pg->set_lineWidth(g.linewidth*0.6);
                        QColor c=g.color;
                        c.setAlphaF(g.colorTransparent);
                        pg->set_color(c);
                        QColor ec=g.errorColor;
                        ec.setAlphaF(g.errorColorTransparent);
                        pg->set_errorColor(ec);
                        QColor efc=g.errorColor;
                        efc.setAlphaF(qBound(0.0,1.0,g.errorColorTransparent-0.2));
                        pg->set_errorFillColor(efc);
                        pg->set_errorLineWidth(g.errorWidth);
                        pg->set_errorStyle(g.errorLineStyle);
                        //pg->set_errorbarSize(g.errorbarSize);

                        QColor fc=g.fillColor;
                        fc.setAlphaF(g.fillColorTransparent);
                        pg->set_fillColor(fc);
                        pg->set_fillStyle(g.fillStyle);
                        pg->set_style(g.style);
                        ui->plotter->addGraph(pg);
                    }
                }
            } else if (g.type==QFRDRTable::gtBoxplotX) {
                JKQTPboxplotVerticalGraph* pg=new JKQTPboxplotVerticalGraph(ui->plotter->get_plotter());
                if (g.titleShow) pg->set_title(g.title); else pg->set_title("");
                pg->set_posColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_minColumn(getColumnWithStride(g.xerrorcolumn, g));
                pg->set_percentile25Column(getColumnWithStride(g.ycolumn, g));
                pg->set_medianColumn(getColumnWithStride(g.yerrorcolumn, g));
                pg->set_meanColumn(getColumnWithStride(g.meancolumn, g));
                pg->set_percentile75Column(getColumnWithStride(g.q75column, g));
                pg->set_maxColumn(getColumnWithStride(g.maxcolumn, g));
                pg->set_sortData(g.dataSortOrder);

                pg->set_boxWidth(g.width);
                pg->set_meanSymbol(g.symbol);
                pg->set_meanSymbolSize(g.symbolSize);
                pg->set_meanSymbolWidth(g.symbolLineWidth);
                pg->set_lineWidth(g.linewidth);
                QColor c=g.color;
                c.setAlphaF(g.colorTransparent);
                pg->set_color(c);
                pg->set_whiskerStyle(g.whiskerStyle);

//                pg->set_errorWidth(g.errorWidth);
//                pg->set_errorFillStyle(g.errorFillStyle);
//                pg->set_errorbarSize(g.errorbarSize);


                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                //pg->set_fillColor(fc);
                //pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtHorizontalRange) {
                JKQTPhorizontalRange* pg=new JKQTPhorizontalRange(ui->plotter->get_plotter());
                if (g.titleShow) pg->set_title(g.title); else pg->set_title("");
                pg->set_lineWidth(g.linewidth);
                QColor c=g.color;
                c.setAlphaF(g.colorTransparent);
                pg->set_color(c);

                c=g.rangeCenterColor;
                c.setAlphaF(g.rangeCenterColorTransparent);
                pg->set_centerColor(c);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                pg->set_fillColor(fc);
                pg->set_fillStyle(g.fillStyle);
                pg->set_style(g.style);
                pg->set_fillRange(g.rangeFill);
                pg->set_invertedRange(g.rangeInverted);
                pg->set_plotCenterLine(g.rangeDrawCenter);
                pg->set_plotRangeLines(g.drawLine);
                pg->set_rangeMin(g.rangeStart);
                pg->set_rangeMax(g.rangeEnd);
                pg->set_rangeCenter(g.rangeCenter);
                pg->set_centerStyle(g.rangeCenterStyle);
                pg->set_centerLineWidth(g.rangeCenterStyle);
                pg->set_lineWidth(g.linewidth);

                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtVerticalRange) {
                JKQTPverticalRange* pg=new JKQTPverticalRange(ui->plotter->get_plotter());
                if (g.titleShow) pg->set_title(g.title); else pg->set_title("");
                pg->set_lineWidth(g.linewidth);
                QColor c=g.color;
                c.setAlphaF(g.colorTransparent);
                pg->set_color(c);

                c=g.rangeCenterColor;
                c.setAlphaF(g.rangeCenterColorTransparent);
                pg->set_centerColor(c);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                pg->set_fillColor(fc);
                pg->set_fillStyle(g.fillStyle);
                pg->set_style(g.style);
                pg->set_fillRange(g.rangeFill);
                pg->set_invertedRange(g.rangeInverted);
                pg->set_plotCenterLine(g.rangeDrawCenter);
                pg->set_plotRangeLines(g.drawLine);
                pg->set_rangeMin(g.rangeStart);
                pg->set_rangeMax(g.rangeEnd);
                pg->set_rangeCenter(g.rangeCenter);
                pg->set_centerLineWidth(g.rangeCenterStyle);
                pg->set_centerStyle(g.rangeCenterStyle);
                pg->set_lineWidth(g.linewidth);

                ui->plotter->addGraph(pg);

            } else if (g.type==QFRDRTable::gtBoxplotY) {
                JKQTPboxplotHorizontalGraph* pg=new JKQTPboxplotHorizontalGraph(ui->plotter->get_plotter());
                if (g.titleShow) pg->set_title(g.title); else pg->set_title("");
                pg->set_posColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_minColumn(getColumnWithStride(g.xerrorcolumn, g));
                pg->set_percentile25Column(getColumnWithStride(g.ycolumn, g));
                pg->set_medianColumn(getColumnWithStride(g.yerrorcolumn, g));
                pg->set_meanColumn(getColumnWithStride(g.meancolumn, g));
                pg->set_percentile75Column(getColumnWithStride(g.q75column, g));
                pg->set_maxColumn(getColumnWithStride(g.maxcolumn, g));
                pg->set_sortData(g.dataSortOrder);

                pg->set_boxWidth(g.width);
                pg->set_meanSymbol(g.symbol);
                pg->set_meanSymbolSize(g.symbolSize);
                pg->set_meanSymbolWidth(g.symbolLineWidth);
                pg->set_lineWidth(g.linewidth);
                QColor c=g.color;
                c.setAlphaF(g.colorTransparent);
                pg->set_color(c);
                pg->set_whiskerStyle(g.whiskerStyle);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                //pg->set_fillColor(fc);
                //pg->set_style(g.style);
                ui->plotter->addGraph(pg);

            } else { // gtLines etc.
                JKQTPxyLineErrorGraph* pg=new JKQTPxyLineErrorGraph(ui->plotter->get_plotter());
                if (g.titleShow) pg->set_title(g.title); else pg->set_title("");
                pg->set_xColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_yColumn(getColumnWithStride(g.ycolumn, g));
                pg->set_xErrorColumn(getColumnWithStride(g.xerrorcolumn, g));
                pg->set_sortData(g.dataSortOrder);
                if (pg->get_xErrorColumn()>=0) {
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }
                if (!g.xerrorsymmetric) {
                    pg->set_xErrorSymmetric(false);
                    pg->set_xErrorColumnLower(getColumnWithStride(g.xerrorcolumnlower, g));
                    if (pg->get_xErrorColumn()>=0 || pg->get_xErrorColumnLower()>=0 ) {
                        pg->set_xErrorStyle(g.errorStyle);
                    }
                }
                pg->set_yErrorColumn(getColumnWithStride(g.yerrorcolumn, g));
                if (pg->get_yErrorColumn()>=0) {
                    pg->set_yErrorStyle(g.errorStyle);
                } else {
                    pg->set_yErrorStyle(JKQTPnoError);
                }
                if (!g.yerrorsymmetric) {
                    pg->set_yErrorSymmetric(false);
                    pg->set_yErrorColumnLower(getColumnWithStride(g.yerrorcolumnlower, g));
                    if (pg->get_yErrorColumn()>=0 || pg->get_yErrorColumnLower()>=0 ) {
                        pg->set_yErrorStyle(g.errorStyle);
                    }
                }


                //qDebug()<<g.title<<pg->get_xColumn()<<pg->get_yColumn();
                pg->set_drawLine(g.drawLine);
                pg->set_symbol(g.symbol);
                pg->set_symbolSize(g.symbolSize);
                pg->set_symbolWidth(g.symbolLineWidth);
                pg->set_style(g.style);


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
                pg->set_errorWidth(g.errorWidth);
                pg->set_errorStyle(g.errorLineStyle);
                pg->set_errorbarSize(g.errorBarSize);

                ui->plotter->addGraph(pg);
             }
        }

        ui->plotter->set_doDrawing(true);
        ui->plotter->set_emitSignals(true);
        ui->plotter->update_plot();
    }
    QApplication::restoreOverrideCursor();
}

void QFRDRTablePlotWidget::delayedUpdateGraph()
{
    timUpdateGraph.stop();
    timUpdateGraph.setSingleShot(true);
    timUpdateGraph.setInterval(DELAYEDUPDATEGRAPH_DELAY);
    timUpdateGraph.start();
}

void QFRDRTablePlotWidget::updateData() {
    bool update=ui->plotter->get_doDrawing();
    ui->plotter->set_doDrawing(false);

    JKQTPdatastore* ds=ui->plotter->getDatastore();
    ds->clear();
    if (current) {
        for (int c=0; c<current->model()->columnCount(); c++) {
            double* data=(double*)qfMalloc(current->model()->rowCount()*sizeof(double));

            for (int r=0; r<current->model()->rowCount(); r++) {
                data[r]=NAN;
                bool ok=false;
                QVariant v=current->model()->cell(r, c);
                if (v.type()==QVariant::Date || v.type()==QVariant::DateTime || v.type()==QVariant::Time) {
                    data[r]=v.toDateTime().toMSecsSinceEpoch();
                } else if (v.canConvert(QVariant::Double)) {
                    double d=v.toDouble(&ok);
                    if (ok) data[r]=d;
                }
            }
            int items=current->model()->rowCount();
            int i=current->model()->rowCount()-1;
            while (i>=0 && !QFFloatIsOK(data[i])) {
                items--;
                i--;
            }

            double dummy=NAN;

            if (items>0) ds->addCopiedColumn(data, items, current->model()->columnTitle(c));
            else  ds->addCopiedColumn(&dummy, 1, current->model()->columnTitle(c));
            qfFree(data);
        }
        ds->addLinearColumn(current->model()->rowCount(), 1, current->model()->rowCount(), QString("rowNumColSpecial"));
    }

    if (update) {
        ui->plotter->set_doDrawing(true);
        ui->plotter->update_plot();
    }
}

void QFRDRTablePlotWidget::delayedUpdateData()
{
    timUpdateData.stop();
    timUpdateData.setSingleShot(true);
    timUpdateData.setInterval(DELAYEDUPDATEGRAPH_DELAY);
    timUpdateData.start();
}

void QFRDRTablePlotWidget::updatePlotWidgetVisibility() {
    if (current) {
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;
        int r=ui->listGraphs->currentRow();
        QFRDRTable::PlotInfo p=current->getPlot(this->plot);
        if (r>=0 && r<p.graphs.size()) {
            //QFRDRTable::GraphInfo graph=p.graphs.at(r);

            ui->widGraphSettings->updatePlotWidgetVisibility();


            ui->widGraphSettings->setVisible(true);

        }
    }

}



void QFRDRTablePlotWidget::connectWidgets()
{
    //qDebug()<<"connectWidgets";
    connect(ui->listGraphs, SIGNAL(currentRowChanged(int)), this, SLOT(listGraphs_currentRowChanged(int)));
    connect(ui->widGraphSettings, SIGNAL(graphDataChanged()), this, SLOT(delayedGraphDataChanged()));
    ui->widSystemSettings->connectWidgets();
}

void QFRDRTablePlotWidget::disconnectWidgets()
{
    //qDebug()<<"disconnectWidgets";
    disconnect(ui->listGraphs, SIGNAL(currentRowChanged(int)), this, SLOT(listGraphs_currentRowChanged(int)));
    disconnect(ui->widGraphSettings, SIGNAL(graphDataChanged()), this, SLOT(delayedGraphDataChanged()));
    ui->widSystemSettings->disconnectWidgets();
}

double QFRDRTablePlotWidget::getMagnification() const
{
    int idx=cmbMagnification->currentIndex();
    if (idx>=0 && idx<cmbMagnification->count()){
        //if (current) current->setQFProperty(QString("GRAPH%1_MAGNIFICATION").arg(plot),cmbMagnification->itemData(idx).toDouble());
        return(cmbMagnification->itemData(idx).toDouble());
    } else {
        return(1.0);
        //if (current) current->setQFProperty(QString("GRAPH%1_MAGNIFICATION").arg(plot),1.0);
    }
}

void QFRDRTablePlotWidget::magnificationChanged(int /*idx*/)
{
    double m=getMagnification();
    if (current) current->setQFProperty(QString("GRAPH%1_MAGNIFICATION").arg(plot),m, false, false);
    updateGraph();
}

int QFRDRTablePlotWidget::getColumnWithStride(int column, const QFRDRTable::GraphInfo& g)
{
    QFRDRTable::GraphDataSelection sel;
    sel=g;
    QString newname;
    int newCol=-1;
    if (sel.getDataWithStride(NULL, &newCol, column, NULL, &newname, ui->plotter)) {

    }
    return newCol;
    /*
    //qDebug()<<"getColumnWithStride  column="<<column<<"    strided: "<<g.isStrided<<" stride="<<g.stride<<" strideStart="<<g.strideStart;
    if (column==-2) return ui->plotter->getDatastore()->ensureColumnNum(QString("rowNumColSpecial"));
    if (column>=0 && column<(long)ui->plotter->getDatastore()->getColumnCount()) {
        QVector<double> data=ui->plotter->getDatastore()->getColumn(column).copyData();
        if (g.isDataSelect) {
            QVector<double> dataS, dataS2, dataS3;
            if (g.dataSelect1Column>=0 && g.dataSelect1Column<(long)ui->plotter->getDatastore()->getColumnCount()) {
                dataS=ui->plotter->getDatastore()->getColumn(g.dataSelect1Column).copyData();
            }
            int cnt=qMin(data.size(), dataS.size());
            if (g.dataSelectLogic12!=QFRDRTable::dsoNone && g.dataSelect2Column>=0 && g.dataSelect2Column<(long)ui->plotter->getDatastore()->getColumnCount()) {
                dataS2=ui->plotter->getDatastore()->getColumn(g.dataSelect2Column).copyData();
                cnt=qMin(cnt, dataS2.size());
            }
            if (g.dataSelectLogic12!=QFRDRTable::dsoNone && g.dataSelectLogic23!=QFRDRTable::dsoNone && g.dataSelect3Column>=0 && g.dataSelect3Column<(long)ui->plotter->getDatastore()->getColumnCount()) {
                dataS3=ui->plotter->getDatastore()->getColumn(g.dataSelect3Column).copyData();
                cnt=qMin(cnt, dataS3.size());
            }
            QVector<double> dataO;
            int istart=0;
            int iinc=1;
            if (g.isStrided) {
                istart=g.strideStart-1;
                iinc=g.stride;
            }

            for (int i=istart; i< cnt; i=i+iinc) {
                bool ok=true;
                if (dataS.size()>0) {
                    ok=false;
                    switch(g.dataSelect1Operation) {
                        case QFRDRTable::dsoEquals:
                            if (dataS[i]==g.dataSelect1CompareValue) ok=true;
                            break;
                        case QFRDRTable::dsoUnequal:
                            if (dataS[i]!=g.dataSelect1CompareValue) ok=true;
                            break;
                        case QFRDRTable::dsoGreaterOrEqual:
                            if (dataS[i]>=g.dataSelect1CompareValue) ok=true;
                            break;
                        case QFRDRTable::dsoSmallerOrEqual:
                            if (dataS[i]<=g.dataSelect1CompareValue) ok=true;
                            break;
                        case QFRDRTable::dsoGreater:
                            if (dataS[i]>g.dataSelect1CompareValue) ok=true;
                            break;
                        case QFRDRTable::dsoSmaller:
                            if (dataS[i]<g.dataSelect1CompareValue) ok=true;
                            break;
                        case QFRDRTable::dsoInRange:
                            if (dataS[i]>=g.dataSelect1CompareValue && dataS[i]<=g.dataSelect1CompareValue2) ok=true;
                            break;
                        case QFRDRTable::dsoOutOfRange:
                            if (!(dataS[i]>=g.dataSelect1CompareValue && dataS[i]<=g.dataSelect1CompareValue2)) ok=true;
                            break;
                    }
                    if (g.dataSelectLogic12!=QFRDRTable::dsoNone && dataS2.size()>0) {
                        bool okLocal=false;
                        switch(g.dataSelect2Operation) {
                            case QFRDRTable::dsoEquals:
                                if (dataS2[i]==g.dataSelect2CompareValue) okLocal=true;
                                break;
                            case QFRDRTable::dsoUnequal:
                                if (dataS2[i]!=g.dataSelect2CompareValue) okLocal=true;
                                break;
                            case QFRDRTable::dsoGreaterOrEqual:
                                if (dataS2[i]>=g.dataSelect2CompareValue) okLocal=true;
                                break;
                            case QFRDRTable::dsoSmallerOrEqual:
                                if (dataS2[i]<=g.dataSelect2CompareValue) okLocal=true;
                                break;
                            case QFRDRTable::dsoGreater:
                                if (dataS2[i]>g.dataSelect2CompareValue) okLocal=true;
                                break;
                            case QFRDRTable::dsoSmaller:
                                if (dataS2[i]<g.dataSelect2CompareValue) okLocal=true;
                                break;
                            case QFRDRTable::dsoInRange:
                                if (dataS2[i]>=g.dataSelect2CompareValue && dataS2[i]<=g.dataSelect2CompareValue2) okLocal=true;
                                break;
                            case QFRDRTable::dsoOutOfRange:
                                if (!(dataS2[i]>=g.dataSelect2CompareValue && dataS2[i]<=g.dataSelect2CompareValue2)) okLocal=true;
                                break;
                        }
                        if (g.dataSelectLogic12==QFRDRTable::dsoAnd) ok=ok&&okLocal;
                        else if (g.dataSelectLogic12==QFRDRTable::dsoOr) ok=ok||okLocal;
                        else if (g.dataSelectLogic12==QFRDRTable::dsoXor) ok=(ok&&!okLocal) || (!ok&&okLocal);
                        if (g.dataSelectLogic23!=QFRDRTable::dsoNone && dataS3.size()>0) {
                            okLocal=false;
                            switch(g.dataSelect3Operation) {
                                case QFRDRTable::dsoEquals:
                                    if (dataS3[i]==g.dataSelect3CompareValue) okLocal=true;
                                    break;
                                case QFRDRTable::dsoUnequal:
                                    if (dataS3[i]!=g.dataSelect3CompareValue) okLocal=true;
                                    break;
                                case QFRDRTable::dsoGreaterOrEqual:
                                    if (dataS3[i]>=g.dataSelect3CompareValue) okLocal=true;
                                    break;
                                case QFRDRTable::dsoSmallerOrEqual:
                                    if (dataS3[i]<=g.dataSelect3CompareValue) okLocal=true;
                                    break;
                                case QFRDRTable::dsoGreater:
                                    if (dataS3[i]>g.dataSelect3CompareValue) okLocal=true;
                                    break;
                                case QFRDRTable::dsoSmaller:
                                    if (dataS3[i]<g.dataSelect3CompareValue) okLocal=true;
                                    break;
                                case QFRDRTable::dsoInRange:
                                    if (dataS3[i]>=g.dataSelect3CompareValue && dataS3[i]<=g.dataSelect3CompareValue2) okLocal=true;
                                    break;
                                case QFRDRTable::dsoOutOfRange:
                                    if (!(dataS3[i]>=g.dataSelect3CompareValue && dataS3[i]<=g.dataSelect3CompareValue2)) okLocal=true;
                                    break;
                            }
                            if (g.dataSelectLogic23==QFRDRTable::dsoAnd) ok=ok&&okLocal;
                            else if (g.dataSelectLogic23==QFRDRTable::dsoOr) ok=ok||okLocal;
                            else if (g.dataSelectLogic23==QFRDRTable::dsoXor) ok=(ok&&!okLocal) || (!ok&&okLocal);
                        }
                    }
                }

                if (ok) dataO.append(data[i]);
            }
            QString s1=tr("%1 %2 %3").arg(ui->plotter->getDatastore()->getColumnNames().at(g.dataSelect1Column)).arg(QFRDRTable::DataSelectOperation2String(g.dataSelect1Operation)).arg(g.dataSelect1CompareValue);
            QString s2="";
            QString s3="";
            if (g.dataSelectLogic12!=QFRDRTable::dsoNone) {
                if (g.dataSelectLogic12==QFRDRTable::dsoAnd) s2=s2+" && ";
                if (g.dataSelectLogic12==QFRDRTable::dsoOr) s2=s2+" || ";
                if (g.dataSelectLogic12==QFRDRTable::dsoXor) s2=s2+" ~~ ";
                s2=s2+tr("%1 %2 %3").arg(ui->plotter->getDatastore()->getColumnNames().at(g.dataSelect2Column)).arg(QFRDRTable::DataSelectOperation2String(g.dataSelect2Operation)).arg(g.dataSelect2CompareValue);
                if (g.dataSelectLogic23!=QFRDRTable::dsoNone) {
                    if (g.dataSelectLogic23==QFRDRTable::dsoAnd) s3=s3+" && ";
                    if (g.dataSelectLogic23==QFRDRTable::dsoOr) s3=s3+" || ";
                    if (g.dataSelectLogic23==QFRDRTable::dsoXor) s3=s3+" ~~ ";
                    s3=s3+tr("%1 %2 %3").arg(ui->plotter->getDatastore()->getColumnNames().at(g.dataSelect3Column)).arg(QFRDRTable::DataSelectOperation2String(g.dataSelect3Operation)).arg(g.dataSelect3CompareValue);
                }
            }
            QString nam= tr("(%2,%3)-strided, selected (%4%5%6) \"%1\"").arg(ui->plotter->getDatastore()->getColumnNames().at(column)).arg(g.strideStart).arg(g.stride).arg(s1).arg(s2).arg(s3);
            if (dataO.size()>0) return ui->plotter->getDatastore()->addCopiedColumn(dataO.data(), dataO.size(),nam);
            else return -1;
        } else {
            if (g.isStrided) {
                return ui->plotter->getDatastore()->copyColumn(column, g.strideStart-1, g.stride, tr("(%2,%3)-strided \"%1\"").arg(ui->plotter->getDatastore()->getColumnNames().at(column)).arg(g.strideStart).arg(g.stride));
            } else {
                return column;
            }
        }
    }
    return -1;*/
}

void QFRDRTablePlotWidget::autoColorGraph(QFRDRTable::GraphInfo &g, int autocolor)
{
    g.color=autocolors.value(autocolor%autocolors.size(), QColor("red"));
    g.fillColor=g.color.lighter();
    g.errorColor=g.color.darker();
}

void QFRDRTablePlotWidget::autoColorGraph(QFRDRTable::GraphInfo &g, QColor color)
{
    g.color=color;
    g.fillColor=g.color.lighter();
    g.errorColor=g.color.darker();
}



void QFRDRTablePlotWidget::on_btnMoveUp_clicked()
{
    int r=ui->listGraphs->currentRow();
    if (r>0) {
        disconnectWidgets();
        updating=true;
        QFRDRTable::PlotInfo p=current->getPlot(this->plot);
        p.graphs.swap(r, r-1);
        QListWidgetItem * currentItem = ui->listGraphs->takeItem(r);
        ui->listGraphs->insertItem(r -1, currentItem);
        ui->listGraphs->setCurrentRow(r - 1);

        current->setPlot(this->plot, p);
        updating=false;
        connectWidgets();
        ui->listGraphs->setCurrentRow(r-1);
        ui->widGraphSettings->initFocus();

    }
}

void QFRDRTablePlotWidget::on_btnMoveDown_clicked()
{
    int r=ui->listGraphs->currentRow();
    if (r<ui->listGraphs->count()-1) {
        disconnectWidgets();
        updating=true;
        QFRDRTable::PlotInfo p=current->getPlot(this->plot);
        p.graphs.swap(r, r+1);
        QListWidgetItem * currentItem = ui->listGraphs->takeItem(r);
        ui->listGraphs->insertItem(r +1, currentItem);
        ui->listGraphs->setCurrentRow(r + 1);

        current->setPlot(this->plot, p);
        updating=false;
        connectWidgets();
        ui->listGraphs->setCurrentRow(r+1);
        ui->widGraphSettings->initFocus();

    }
}

void QFRDRTablePlotWidget::on_btnResetColoring_clicked()
{
    int oldidx=ui->listGraphs->currentRow();
    disconnectWidgets();
    updating=true;
    QFRDRTable::PlotInfo p=current->getPlot(this->plot);
    for (int i=0; i<p.graphs.size(); i++) {
        autoColorGraph(p.graphs[i], i);
    }
    current->setPlot(this->plot, p);
    updating=false;
    listGraphs_currentRowChanged(oldidx);
    connectWidgets();
    ui->listGraphs->setCurrentRow(oldidx);
    ui->widGraphSettings->initFocus();
}

void QFRDRTablePlotWidget::on_btnColorByPalette_clicked()
{

    DlgColorbarColoring* dlg=new DlgColorbarColoring(this);

    QStringList sl;

    int oldidx=ui->listGraphs->currentRow();
    disconnectWidgets();
    updating=true;
    QFRDRTable::PlotInfo p=current->getPlot(this->plot);
    for (int i=0; i<p.graphs.size(); i++) {
        sl.append(p.graphs[i].title);
    }
    dlg->init(sl);
    if (dlg->exec()) {
        QList<bool> chk=dlg->getSelected();
        QList<QColor> col=dlg->getColors();
        for (int i=0; i<qMin(col.size(), p.graphs.size()); i++) {
            if (chk.value(i, true)) {
                autoColorGraph(p.graphs[i], col[i]);
            }
        }
        current->setPlot(this->plot, p);
    }
    delete dlg;
    updating=false;
    listGraphs_currentRowChanged(oldidx);
    connectWidgets();
    ui->listGraphs->setCurrentRow(oldidx);
    ui->widGraphSettings->initFocus();
}

void QFRDRTablePlotWidget::reloadGraphData()
{
    QWidget* w=QApplication::focusWidget();
    listGraphs_currentRowChanged(ui->listGraphs->currentRow());
    if (w) w->setFocus(Qt::MouseFocusReason);
}

void QFRDRTablePlotWidget::delayedReloadGraphData()
{
    timReloadGraphData.stop();
    timReloadGraphData.setSingleShot(true);
    timReloadGraphData.setInterval(DELAYEDUPDATEGRAPH_DELAY);
    timReloadGraphData.start();
}




void QFRDRTablePlotWidget::doFit(int xCol, int yCol, int sigmaCol, int plot, QString function, QFRDRTable::GraphDataSelection sel)
{
    emit performFit(xCol, yCol, sigmaCol, plot, ui->listGraphs->currentRow(), function,sel, ui->widSystemSettings->getLogX(), ui->widSystemSettings->getLogY());
}

void QFRDRTablePlotWidget::doRegression(int xCol, int yCol, int sigmaCol, int plot, QFRDRTable::GraphDataSelection sel)
{
    emit performRegression(xCol, yCol, sigmaCol, plot, ui->listGraphs->currentRow(),sel, ui->widSystemSettings->getLogX(), ui->widSystemSettings->getLogY());
}

void QFRDRTablePlotWidget::doRefit(int plot)
{
    int r=ui->listGraphs->currentRow();
    //if (r>=0 && r<p.graphs.size()) {
        emit performRefit(plot, r);
    //}
}

