#include "ui_qfrdrtableplotwidget.h"
#include "qfrdrtableplotwidget.h"
#include "programoptions.h"
#include "qfrdrtable.h"
#include "qftools.h"
#include "qfdoubleedit.h"
#include "qfmathparserxfunctionlinegraph.h"
#include "qffitfunctionplottools.h"
#include "qffitfunctionmanager.h"
#include "qffitfunction.h"
#include "dlgcolorbarcoloring.h"


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

    connect(ui->widGraphSettings, SIGNAL(performFit(int,int,int,int,QString)), this, SLOT(doFit(int,int,int,int,QString)));
    connect(ui->widGraphSettings, SIGNAL(performRegression(int,int,int,int)), this, SLOT(doRegression(int,int,int,int)));

    //ui->formLayout_3->removeWidget(ui->widSaveCoordSettings);
    //ui->tabWidget->setCornerWidget(ui->widSaveCoordSettings);



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

    connect(ui->widSystemSettings, SIGNAL(autoscaleX()), this, SLOT(doAutoscaleX()));
    connect(ui->widSystemSettings, SIGNAL(autoscaleY()), this, SLOT(doAutoscaleY()));


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
        } else {
            ui->tabSystem->setEnabled(false);
        }
    }
    ui->listGraphs->setCurrentRow(0);
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
            ui->listGraphs->item(r)->setIcon(ui->widGraphSettings->getGraphIcon(graph.type));

            ui->widSystemSettings->graphDataChanged(r);


            p.graphs[r]=graph;
            current->setPlot(this->plot, p);

        }
        updating=false;
    }
    updateGraph();
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

void QFRDRTablePlotWidget::updateGraph() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (current) {
        //qDebug()<<"updateGraph  plot="<<this->plot+1<<"/"<<current->getPlotCount();
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;
        //qDebug()<<"updateGraph";
        QFRDRTable::PlotInfo p=current->getPlot(this->plot);

        ui->plotter->set_doDrawing(false);
        ui->plotter->set_emitSignals(false);

        if (p.graphAutosize) {
            ui->scrollArea->setWidgetResizable(true);
        } else {
            ui->scrollArea->setWidgetResizable(false);
            ui->plotter->resize(ui->plotter->get_plotter()->mm2px(p.graphWidth, logicalDpiX()), ui->plotter->get_plotter()->mm2px(p.graphHeight, logicalDpiX()));
        }


        ui->plotter->getXAxis()->set_axisLabel(p.xAxis.label);
        ui->plotter->getXAxis()->set_logAxis(p.xAxis.log);
        ui->plotter->getXAxis()->set_showZeroAxis(p.xAxis.axis0);
        ui->plotter->getYAxis()->set_showZeroAxis(p.yAxis.axis0);
        ui->plotter->getXAxis()->set_labelFont(p.fontName);
        ui->plotter->getXAxis()->set_labelFontSize(p.axisLabelFontSize);
        ui->plotter->getXAxis()->set_tickLabelFont(p.fontName);
        ui->plotter->getXAxis()->set_tickLabelFontSize(p.axisFontSize);
        ui->plotter->getXAxis()->set_gridWidth(p.gridWidth);
        ui->plotter->getYAxis()->set_gridWidth(p.gridWidth);
        ui->plotter->getXAxis()->set_gridStyle(p.gridStyle);
        ui->plotter->getYAxis()->set_gridStyle(p.gridStyle);
        ui->plotter->getXAxis()->set_gridColor(p.gridColor);
        ui->plotter->getYAxis()->set_gridColor(p.gridColor);
        ui->plotter->getXAxis()->set_minTicks(p.xAxis.minTicks);
        ui->plotter->getYAxis()->set_minTicks(p.yAxis.minTicks);
        ui->plotter->getXAxis()->set_minorTicks(p.xAxis.minorTicks);
        ui->plotter->getYAxis()->set_minorTicks(p.yAxis.minorTicks);
        ui->plotter->getXAxis()->set_labelPosition(p.xAxis.labelPos);
        ui->plotter->getYAxis()->set_labelPosition(p.yAxis.labelPos);
        ui->plotter->getXAxis()->set_labelType(p.xAxis.labelType);
        ui->plotter->getYAxis()->set_labelType(p.yAxis.labelType);
        ui->plotter->getXAxis()->set_autoLabelDigits(false);
        ui->plotter->getYAxis()->set_autoLabelDigits(false);
        ui->plotter->getXAxis()->set_labelDigits(p.xAxis.digits);
        ui->plotter->getYAxis()->set_labelDigits(p.yAxis.digits);
        ui->plotter->getXAxis()->set_drawMode1(p.xAxis.labelMode1);
        ui->plotter->getXAxis()->set_drawMode2(p.xAxis.labelMode2);
        ui->plotter->getYAxis()->set_drawMode1(p.yAxis.labelMode1);
        ui->plotter->getYAxis()->set_drawMode2(p.yAxis.labelMode2);


        ui->plotter->getXAxis()->set_userTickSpacing(p.xAxis.TickSpacing);
        ui->plotter->getXAxis()->set_autoAxisSpacing(p.xAxis.AutoTicks);
        ui->plotter->getXAxis()->set_tickSpacing(p.xAxis.TickSpacing);
        ui->plotter->getXAxis()->set_userTickSpacing(p.xAxis.TickSpacing);
        ui->plotter->getXAxis()->set_tickWidth(p.xAxis.AxisTickWidth);
        ui->plotter->getXAxis()->set_minorTickWidth(p.xAxis.AxisMinorTickWidth);
        ui->plotter->getXAxis()->set_inverted(p.xAxis.AxisInverted);
        ui->plotter->getXAxis()->set_tickInsideLength(p.xAxis.TickInsideLength);
        ui->plotter->getXAxis()->set_tickOutsideLength(p.xAxis.TickOutsideLength);
        ui->plotter->getXAxis()->set_minorTickOutsideLength(p.xAxis.MinorTickOutsideLength);
        ui->plotter->getXAxis()->set_minorTickInsideLength(p.xAxis.MinorTickInsideLength);

        ui->plotter->getYAxis()->set_userTickSpacing(p.yAxis.TickSpacing);
        ui->plotter->getYAxis()->set_autoAxisSpacing(p.yAxis.AutoTicks);
        ui->plotter->getYAxis()->set_tickSpacing(p.yAxis.TickSpacing);
        ui->plotter->getYAxis()->set_userTickSpacing(p.yAxis.TickSpacing);
        ui->plotter->getYAxis()->set_tickWidth(p.yAxis.AxisTickWidth);
        ui->plotter->getYAxis()->set_minorTickWidth(p.yAxis.AxisMinorTickWidth);
        ui->plotter->getYAxis()->set_inverted(p.yAxis.AxisInverted);
        ui->plotter->getYAxis()->set_tickInsideLength(p.yAxis.TickInsideLength);
        ui->plotter->getYAxis()->set_tickOutsideLength(p.yAxis.TickOutsideLength);
        ui->plotter->getYAxis()->set_minorTickOutsideLength(p.yAxis.MinorTickOutsideLength);
        ui->plotter->getYAxis()->set_minorTickInsideLength(p.yAxis.MinorTickInsideLength);

        ui->plotter->getYAxis()->set_axisLabel(p.yAxis.label);
        ui->plotter->getYAxis()->set_logAxis(p.yAxis.log);
        ui->plotter->getYAxis()->set_labelFont(p.fontName);
        ui->plotter->getYAxis()->set_labelFontSize(p.axisLabelFontSize);
        ui->plotter->getYAxis()->set_tickLabelFont(p.fontName);
        ui->plotter->getYAxis()->set_tickLabelFontSize(p.axisFontSize);
        ui->plotter->get_plotter()->set_plotLabelFontname(p.fontName);
        ui->plotter->get_plotter()->set_plotLabelFontSize(p.labelFontSize);
        ui->plotter->get_plotter()->set_plotLabel(p.title);
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
        QColor kb=QColor("white");
        kb.setAlphaF(p.keyTransparency);
        ui->plotter->get_plotter()->set_keyBackgroundColor(kb);
        ui->plotter->get_plotter()->set_keyLayout(p.keyLayout);
        ui->plotter->get_plotter()->set_keyPosition(p.keyPosition);
        ui->plotter->get_plotter()->set_showKey(p.showKey);
        ui->plotter->setXY(p.xAxis.min, p.xAxis.max, p.yAxis.min, p.yAxis.max);
        ui->plotter->clearGraphs(true);
        updateData();
        for (int i=0; i<p.graphs.size(); i++) {
            QFRDRTable::GraphInfo g=p.graphs.at(i);
            if (g.type==QFRDRTable::gtImpulsesVertical) {
                JKQTPimpulsesVerticalErrorGraph* pg=new JKQTPimpulsesVerticalErrorGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                pg->set_xColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_yColumn(getColumnWithStride(g.ycolumn, g));
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
                //pg->set_fillStyle(g.fillStyle);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtImpulsesHorizontal) {
                JKQTPimpulsesHorizontalErrorGraph* pg=new JKQTPimpulsesHorizontalErrorGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                pg->set_xColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_yColumn(getColumnWithStride(g.ycolumn, g));
                pg->set_xErrorColumn(getColumnWithStride(g.yerrorcolumn, g));
                if (pg->get_xErrorColumn()>=0) {
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
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

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                //pg->set_fillColor(fc);
                //pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtBarsHorizontal) {
                JKQTPbarVerticalGraph* pg=new JKQTPbarVerticalGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                pg->set_xColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_yColumn(getColumnWithStride(g.ycolumn, g));
                pg->set_width(g.width);
                pg->set_shift(g.shift);
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
                pg->set_fillStyle(g.fillStyle);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtBarsVertical) {
                JKQTPbarHorizontalGraph* pg=new JKQTPbarHorizontalGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                pg->set_xColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_yColumn(getColumnWithStride(g.ycolumn, g));
                pg->set_width(g.width);
                pg->set_shift(g.shift);
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
                pg->set_fillStyle(g.fillStyle);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtFilledCurveX) {
                JKQTPfilledCurveXErrorGraph* pg=new JKQTPfilledCurveXErrorGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                pg->set_xColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_yColumn(getColumnWithStride(g.ycolumn, g));
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
                pg->set_fillStyle(g.fillStyle);
                pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtFilledCurveY) {
                JKQTPfilledCurveYErrorGraph* pg=new JKQTPfilledCurveYErrorGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                pg->set_xColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_yColumn(getColumnWithStride(g.ycolumn, g));
                pg->set_xErrorColumn(getColumnWithStride(g.yerrorcolumn, g));
                if (pg->get_xErrorColumn()>=0) {
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
                pg->set_fillStyle(g.fillStyle);
                pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtStepsHorizontal) {
                JKQTPstepHorizontalGraph* pg=new JKQTPstepHorizontalGraph(ui->plotter->get_plotter());
                pg->set_fillCurve(true);
                pg->set_title(g.title);
                pg->set_xColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_yColumn(getColumnWithStride(g.ycolumn, g));
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

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                pg->set_fillColor(fc);
                pg->set_fillStyle(g.fillStyle);
                pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtStepsVertical) {
                JKQTPstepVerticalGraph* pg=new JKQTPstepVerticalGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                pg->set_xColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_yColumn(getColumnWithStride(g.ycolumn, g));
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

            } else if (g.type==QFRDRTable::gtImage) {
                JKQTPColumnMathImage* pg=new JKQTPColumnMathImage(ui->plotter->get_plotter());
                pg->set_title(g.title);

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
                pg->set_title(g.title);

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
                pg->set_title(g.title);

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
                JKQTPxFunctionLineGraph* pg=NULL;

                int vecItems=0;
                if (g.functionType==QFRDRTable::gtfString) {
                    QFMathParserXFunctionLineGraph* pgf=new QFMathParserXFunctionLineGraph(ui->plotter->get_plotter());
                    pgf->set_function(g.function);
                    pg=pgf;

                } else if (g.functionType==QFRDRTable::gtfPolynomial) {
                    JKQTPxFunctionLineGraph* pgf=new JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
                    pgf->setSpecialFunction(JKQTPxFunctionLineGraph::Polynomial);
                    pg=pgf;
                    vecItems=11;
                } else if (g.functionType==QFRDRTable::gtfExponential) {
                    JKQTPxFunctionLineGraph* pgf=new JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
                    pgf->setSpecialFunction(JKQTPxFunctionLineGraph::Exponential);
                    pg=pgf;
                    vecItems=3;
                } else if (g.functionType==QFRDRTable::gtfPowerLaw) {
                    JKQTPxFunctionLineGraph* pgf=new JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
                    pgf->setSpecialFunction(JKQTPxFunctionLineGraph::PowerLaw);
                    pg=pgf;
                    vecItems=3;
                } else if (g.functionType==QFRDRTable::gtfQFFunction) {
                    JKQTPxQFFitFunctionLineGraph* pgf=new JKQTPxQFFitFunctionLineGraph(ui->plotter->get_plotter());
                    QFFitFunction* ff=NULL;
                    pgf->set_fitFunction(ff=QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(g.function, NULL), true);
                    pg=pgf;
                    if (ff) vecItems=ff->paramCount();
                }


                if (pg) {
                    if (g.ycolumn>=0 && g.ycolumn<(long)ui->plotter->getDatastore()->getColumnCount()) {
                        pg->set_parameterColumn(g.ycolumn);
                    } else {
                        pg->set_params(g.functionParameters);
                    }
                    pg->set_title(g.title);
                    //qDebug()<<"adding function plot "<<g.function;
                    pg->set_drawLine(true);
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
                    pg->set_fillStyle(g.fillStyle);
                    pg->set_style(g.style);
                    ui->plotter->addGraph(pg);
                }
            } else if (g.type==QFRDRTable::gtBoxplotX) {
                JKQTPboxplotVerticalGraph* pg=new JKQTPboxplotVerticalGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                pg->set_posColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_minColumn(getColumnWithStride(g.xerrorcolumn, g));
                pg->set_percentile25Column(getColumnWithStride(g.ycolumn, g));
                pg->set_medianColumn(getColumnWithStride(g.yerrorcolumn, g));
                pg->set_meanColumn(getColumnWithStride(g.meancolumn, g));
                pg->set_percentile75Column(getColumnWithStride(g.q75column, g));
                pg->set_maxColumn(getColumnWithStride(g.maxcolumn, g));

                pg->set_boxWidth(g.width);
                pg->set_meanSymbol(g.symbol);
                pg->set_meanSymbolSize(g.symbolSize);
                pg->set_lineWidth(g.linewidth);
                QColor c=g.color;
                c.setAlphaF(g.colorTransparent);
                pg->set_color(c);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                //pg->set_fillColor(fc);
                //pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtHorizontalRange) {
                JKQTPhorizontalRange* pg=new JKQTPhorizontalRange(ui->plotter->get_plotter());
                pg->set_title(g.title);
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
                pg->set_title(g.title);
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
                pg->set_title(g.title);
                pg->set_posColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_minColumn(getColumnWithStride(g.xerrorcolumn, g));
                pg->set_percentile25Column(getColumnWithStride(g.ycolumn, g));
                pg->set_medianColumn(getColumnWithStride(g.yerrorcolumn, g));
                pg->set_meanColumn(getColumnWithStride(g.meancolumn, g));
                pg->set_percentile75Column(getColumnWithStride(g.q75column, g));
                pg->set_maxColumn(getColumnWithStride(g.maxcolumn, g));

                pg->set_boxWidth(g.width);
                pg->set_meanSymbol(g.symbol);
                pg->set_meanSymbolSize(g.symbolSize);
                pg->set_lineWidth(g.linewidth);
                QColor c=g.color;
                c.setAlphaF(g.colorTransparent);
                pg->set_color(c);

                QColor fc=g.fillColor;
                fc.setAlphaF(g.fillColorTransparent);
                //pg->set_fillColor(fc);
                //pg->set_style(g.style);
                ui->plotter->addGraph(pg);
            } else { // gtLines etc.
                JKQTPxyLineErrorGraph* pg=new JKQTPxyLineErrorGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                pg->set_xColumn(getColumnWithStride(g.xcolumn, g));
                pg->set_yColumn(getColumnWithStride(g.ycolumn, g));
                pg->set_xErrorColumn(getColumnWithStride(g.xerrorcolumn, g));
                if (pg->get_xErrorColumn()>=0) {
                    pg->set_xErrorStyle(g.errorStyle);
                } else {
                    pg->set_xErrorStyle(JKQTPnoError);
                }
                pg->set_yErrorColumn(getColumnWithStride(g.yerrorcolumn, g));
                if (pg->get_yErrorColumn()>=0) {
                    pg->set_yErrorStyle(g.errorStyle);
                } else {
                    pg->set_yErrorStyle(JKQTPnoError);
                }

                //qDebug()<<g.title<<pg->get_xColumn()<<pg->get_yColumn();
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
        ui->plotter->set_emitSignals(true);
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
            int items=current->model()->rowCount();
            int i=current->model()->rowCount()-1;
            while (i>=0 && !QFFloatIsOK(data[i])) {
                items--;
                i--;
            }

            double dummy=NAN;

            if (items>0) ds->addCopiedColumn(data, items, current->model()->columnTitle(c));
            else  ds->addCopiedColumn(&dummy, 1, current->model()->columnTitle(c));
            free(data);
        }
    }

    if (update) {
        ui->plotter->set_doDrawing(true);
        ui->plotter->update_plot();
    }
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
    connect(ui->widGraphSettings, SIGNAL(graphDataChanged()), this, SLOT(graphDataChanged()));
    ui->widSystemSettings->connectWidgets();
}

void QFRDRTablePlotWidget::disconnectWidgets()
{
    //qDebug()<<"disconnectWidgets";
    disconnect(ui->listGraphs, SIGNAL(currentRowChanged(int)), this, SLOT(listGraphs_currentRowChanged(int)));
    disconnect(ui->widGraphSettings, SIGNAL(graphDataChanged()), this, SLOT(graphDataChanged()));
    ui->widSystemSettings->disconnectWidgets();
}

int QFRDRTablePlotWidget::getColumnWithStride(int column, const QFRDRTable::GraphInfo& g)
{
    //qDebug()<<"getColumnWithStride  column="<<column<<"    strided: "<<g.isStrided<<" stride="<<g.stride<<" strideStart="<<g.strideStart;
    if (column>=0 && column<(long)ui->plotter->getDatastore()->getColumnCount()) {
        if (g.isStrided) {
            return ui->plotter->getDatastore()->copyColumn(column, g.strideStart-1, g.stride, tr("(%2,%3)-strided \"%1\"").arg(ui->plotter->getDatastore()->getColumnNames().at(column)).arg(g.strideStart).arg(g.stride));
        } else {
            return column;
        }
    }
    return -1;
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




void QFRDRTablePlotWidget::doFit(int xCol, int yCol, int sigmaCol, int plot, QString function)
{
    emit performFit(xCol, yCol, sigmaCol, plot, function, ui->widSystemSettings->getLogX(), ui->widSystemSettings->getLogY());
}

void QFRDRTablePlotWidget::doRegression(int xCol, int yCol, int sigmaCol, int plot)
{
    emit performRegression(xCol, yCol, sigmaCol, plot, ui->widSystemSettings->getLogX(), ui->widSystemSettings->getLogY());
}

