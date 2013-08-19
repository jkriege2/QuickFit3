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


    ui->edtXMin->setCheckBounds(false, true);
    ui->edtXMax->setCheckBounds(true, false);
    ui->edtYMin->setCheckBounds(false, true);
    ui->edtYMax->setCheckBounds(true, false);
    ui->edtAxisAspect->setRange(0,1e10);
    ui->edtAxisAspect->setCheckBounds(true, false);
    ui->edtDataAspect->setRange(0,1e10);
    ui->edtDataAspect->setCheckBounds(true, false);
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
            ui->edtTitle->setText(g.title);

            ui->chkPlotAutosize->setChecked(g.graphAutosize);
            ui->spinWidth->setValue(g.graphWidth);
            ui->spinHeight->setValue(g.graphHeight);

            ui->edtXLabel->setText(g.xlabel);
            ui->edtYLabel->setText(g.ylabel);
            ui->chkLogX->setChecked(g.xlog);
            ui->chkLogY->setChecked(g.ylog);
            ui->chkX0Axis->setChecked(g.x0axis);
            ui->chkY0Axis->setChecked(g.y0axis);
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
            ui->chkKeepAxisAspect->setChecked(g.keepAxisAspectRatio);
            ui->chkKeepDataAspect->setChecked(g.keepDataAspectRatio);
            ui->edtAxisAspect->setValue(g.axisAspectRatio);
            ui->edtDataAspect->setValue(g.dataAspectRatio);

            ui->spinXAxisDigits->setValue(g.xdigits);
            ui->spinYAxisDigits->setValue(g.ydigits);
            ui->spinXAxisMinTicks->setValue(g.xminTicks);
            ui->spinXAxisMinorTicks->setValue(g.xminorTicks);
            ui->spinYAxisMinTicks->setValue(g.yminTicks);
            ui->spinYAxisMinorTicks->setValue(g.yminorTicks);
            ui->cmbXLabelPosition->setPosition(g.xlabelPos);
            ui->cmbYLabelPosition->setPosition(g.ylabelPos);
            ui->cmbXLabel->setLabelType(g.xlabelType);
            ui->cmbYLabel->setLabelType(g.ylabelType);
            ui->cmbXMode1->setDrawMode(g.xlabelMode1);
            ui->cmbXMode2->setDrawMode(g.xlabelMode2);
            ui->cmbYMode1->setDrawMode(g.ylabelMode1);
            ui->cmbYMode2->setDrawMode(g.ylabelMode2);
            ui->cmbGridColor->setCurrentColor(g.gridColor);
            ui->cmbBackgroundColor->setCurrentColor(g.backgroundColor);
            ui->cmbGridLinestyle->setCurrentLineStyle(g.gridStyle);
            ui->spinGridWidth->setValue(g.gridWidth);


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


            if (ui->edtXLabel->text()=="x" && graph.xcolumn>=0) {
                QString txt=current->tableGetColumnTitle(graph.xcolumn);
                int idx=txt.indexOf(':');
                if (idx>=0) txt=txt.right(txt.size()-(idx+1));
                ui->edtXLabel->setText(txt);
            }
            if (ui->edtYLabel->text()=="y" && graph.ycolumn>=0) {
                QString txt=current->tableGetColumnTitle(graph.ycolumn);
                int idx=txt.indexOf(':');
                if (idx>=0) txt=txt.right(txt.size()-(idx+1));
                ui->edtYLabel->setText(txt);
            }
            if (ui->edtTitle->text().isEmpty() && graph.ycolumn>=0) {
                QString txt=current->tableGetColumnTitle(graph.ycolumn);
                int idx=txt.indexOf(':');
                if (idx>=0) txt=txt.right(txt.size()-(idx+1));
                ui->edtYLabel->setText(txt);
            }

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

        QFRDRTable::PlotInfo p=current->getPlot(this->plot);
        p.title=ui->edtTitle->text();
        p.xlabel=ui->edtXLabel->text();
        p.ylabel=ui->edtYLabel->text();
        p.graphAutosize=ui->chkPlotAutosize->isChecked();
        p.graphWidth=ui->spinWidth->value();
        p.graphHeight=ui->spinHeight->value();
        p.xlog=ui->chkLogX->isChecked();
        p.ylog=ui->chkLogY->isChecked();
        p.x0axis=ui->chkX0Axis->isChecked();
        p.y0axis=ui->chkY0Axis->isChecked();
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
        p.keyTransparency=double(ui->sliderKeyTransparency->value())/255.0;
        p.keyPosition=ui->cmbKeyPosition->getPosition();
        p.keyLayout=ui->cmbKeyLayout->getKeyLayout();
        p.dataAspectRatio=ui->edtDataAspect->value();
        p.axisAspectRatio=ui->edtAxisAspect->value();
        p.keepAxisAspectRatio=ui->chkKeepAxisAspect->isChecked();
        p.keepDataAspectRatio=ui->chkKeepDataAspect->isChecked();

        p.xdigits=ui->spinXAxisDigits->value();
        p.ydigits=ui->spinYAxisDigits->value();
        p.xminTicks=ui->spinXAxisMinTicks->value();
        p.xminorTicks=ui->spinXAxisMinorTicks->value();
        p.yminTicks=ui->spinYAxisMinTicks->value();
        p.yminorTicks=ui->spinYAxisMinorTicks->value();
        p.xlabelPos=ui->cmbXLabelPosition->getPosition();
        p.ylabelPos=ui->cmbYLabelPosition->getPosition();
        p.xlabelType=ui->cmbXLabel->getLabelType();
        p.ylabelType=ui->cmbYLabel->getLabelType();
        p.xlabelMode1=ui->cmbXMode1->getDrawMode();
        p.xlabelMode2=ui->cmbXMode2->getDrawMode();
        p.ylabelMode1=ui->cmbYMode1->getDrawMode();
        p.ylabelMode2=ui->cmbYMode2->getDrawMode();
        p.gridColor=ui->cmbGridColor->currentColor();
        p.backgroundColor=ui->cmbBackgroundColor->currentColor();
        p.gridStyle=ui->cmbGridLinestyle->currentLineStyle();
        p.gridWidth=ui->spinGridWidth->value();
        emit plotTitleChanged(this->plot, p.title);

        current->setPlot(this->plot, p);
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
            ui->plotter->resize(p.graphWidth, p.graphHeight);
        }


        ui->plotter->getXAxis()->set_axisLabel(p.xlabel);
        ui->plotter->getXAxis()->set_logAxis(p.xlog);
        ui->plotter->getXAxis()->set_showZeroAxis(p.x0axis);
        ui->plotter->getYAxis()->set_showZeroAxis(p.y0axis);
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
        ui->plotter->getXAxis()->set_autoLabelDigits(false);
        ui->plotter->getYAxis()->set_autoLabelDigits(false);
        ui->plotter->getXAxis()->set_labelDigits(p.xdigits);
        ui->plotter->getYAxis()->set_labelDigits(p.ydigits);
        ui->plotter->getXAxis()->set_minTicks(p.xminTicks);
        ui->plotter->getYAxis()->set_minTicks(p.yminTicks);
        ui->plotter->getXAxis()->set_minorTicks(p.xminorTicks);
        ui->plotter->getYAxis()->set_minorTicks(p.yminorTicks);
        ui->plotter->getXAxis()->set_labelPosition(p.xlabelPos);
        ui->plotter->getYAxis()->set_labelPosition(p.ylabelPos);
        ui->plotter->getXAxis()->set_labelType(p.xlabelType);
        ui->plotter->getYAxis()->set_labelType(p.ylabelType);
        ui->plotter->getXAxis()->set_drawMode1(p.xlabelMode1);
        ui->plotter->getXAxis()->set_drawMode2(p.xlabelMode2);
        ui->plotter->getYAxis()->set_drawMode1(p.ylabelMode1);
        ui->plotter->getYAxis()->set_drawMode2(p.ylabelMode2);



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
        ui->plotter->setXY(p.xmin, p.xmax, p.ymin, p.ymax);
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
                /*pg->set_imageName(g.imageLegend);
                pg->set_imageNameFontName(p.fontName);
                pg->set_imageNameFontSize(p.axisLabelFontSize);*/
                pg->get_colorBarRightAxis()->set_tickLabelFont(p.fontName);
                pg->get_colorBarRightAxis()->set_tickLabelFontSize(p.axisFontSize);
                pg->get_colorBarRightAxis()->set_axisLabel(g.imageLegend);
                pg->get_colorBarRightAxis()->set_labelFont(p.fontName);
                pg->get_colorBarRightAxis()->set_labelFontSize(p.axisFontSize);
                pg->get_colorBarTopAxis()->set_tickLabelFont(p.fontName);
                pg->get_colorBarTopAxis()->set_tickLabelFontSize(p.axisFontSize);
                pg->get_colorBarTopAxis()->set_axisLabel(g.imageLegend);
                pg->get_colorBarTopAxis()->set_labelFont(p.fontName);
                pg->get_colorBarTopAxis()->set_labelFontSize(p.axisFontSize);
                pg->get_modifierColorBarRightAxis()->set_tickLabelFont(p.fontName);
                pg->get_modifierColorBarRightAxis()->set_tickLabelFontSize(p.axisFontSize);
                pg->get_modifierColorBarRightAxis()->set_axisLabel(g.imageLegendMod);
                pg->get_modifierColorBarRightAxis()->set_labelFont(p.fontName);
                pg->get_modifierColorBarRightAxis()->set_labelFontSize(p.axisFontSize);
                pg->get_modifierColorBarTopAxis()->set_tickLabelFont(p.fontName);
                pg->get_modifierColorBarTopAxis()->set_tickLabelFontSize(p.axisFontSize);
                pg->get_modifierColorBarTopAxis()->set_axisLabel(g.imageLegendMod);
                pg->get_modifierColorBarTopAxis()->set_labelFont(p.fontName);
                pg->get_modifierColorBarTopAxis()->set_labelFontSize(p.axisFontSize);
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
                pg->set_imageName(g.imageLegend);
                pg->set_imageNameG(g.imageLegendG);
                pg->set_imageNameB(g.imageLegendB);
                /*pg->set_imageName(g.imageLegend);
                pg->set_imageNameFontName(p.fontName);
                pg->set_imageNameFontSize(p.axisLabelFontSize);*/
                pg->get_colorBarRightAxis()->set_tickLabelFont(p.fontName);
                pg->get_colorBarRightAxis()->set_tickLabelFontSize(p.axisFontSize);
                pg->get_colorBarRightAxis()->set_axisLabel(g.imageLegend);
                pg->get_colorBarRightAxis()->set_labelFont(p.fontName);
                pg->get_colorBarRightAxis()->set_labelFontSize(p.axisFontSize);
                pg->get_colorBarTopAxis()->set_tickLabelFont(p.fontName);
                pg->get_colorBarTopAxis()->set_tickLabelFontSize(p.axisFontSize);
                pg->get_colorBarTopAxis()->set_axisLabel(g.imageLegend);
                pg->get_colorBarTopAxis()->set_labelFont(p.fontName);
                pg->get_colorBarTopAxis()->set_labelFontSize(p.axisFontSize);

                pg->get_colorBarRightAxisG()->set_tickLabelFont(p.fontName);
                pg->get_colorBarRightAxisG()->set_tickLabelFontSize(p.axisFontSize);
                pg->get_colorBarRightAxisG()->set_axisLabel(g.imageLegendG);
                pg->get_colorBarRightAxisG()->set_labelFont(p.fontName);
                pg->get_colorBarRightAxisG()->set_labelFontSize(p.axisFontSize);
                pg->get_colorBarTopAxisG()->set_tickLabelFont(p.fontName);
                pg->get_colorBarTopAxisG()->set_tickLabelFontSize(p.axisFontSize);
                pg->get_colorBarTopAxisG()->set_axisLabel(g.imageLegendG);
                pg->get_colorBarTopAxisG()->set_labelFont(p.fontName);
                pg->get_colorBarTopAxisG()->set_labelFontSize(p.axisFontSize);

                pg->get_colorBarRightAxisB()->set_tickLabelFont(p.fontName);
                pg->get_colorBarRightAxisB()->set_tickLabelFontSize(p.axisFontSize);
                pg->get_colorBarRightAxisB()->set_axisLabel(g.imageLegendB);
                pg->get_colorBarRightAxisB()->set_labelFont(p.fontName);
                pg->get_colorBarRightAxisB()->set_labelFontSize(p.axisFontSize);
                pg->get_colorBarTopAxisB()->set_tickLabelFont(p.fontName);
                pg->get_colorBarTopAxisB()->set_tickLabelFontSize(p.axisFontSize);
                pg->get_colorBarTopAxisB()->set_axisLabel(g.imageLegendB);
                pg->get_colorBarTopAxisB()->set_labelFont(p.fontName);
                pg->get_colorBarTopAxisB()->set_labelFontSize(p.axisFontSize);

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

    connect(ui->edtTitle, SIGNAL(editingFinished()), this, SLOT(plotDataChanged()));
    connect(ui->edtXLabel, SIGNAL(editingFinished()), this, SLOT(plotDataChanged()));
    connect(ui->edtYLabel, SIGNAL(editingFinished()), this, SLOT(plotDataChanged()));
    connect(ui->chkGrid, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    connect(ui->chkLogX, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    connect(ui->chkLogY, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    connect(ui->chkX0Axis, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    connect(ui->chkY0Axis, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
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
    connect(ui->sliderKeyTransparency, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->cmbKeyLayout, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->cmbKeyPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->edtAxisAspect, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->edtDataAspect, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->chkKeepAxisAspect, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    connect(ui->chkKeepDataAspect, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));

    connect(ui->spinXAxisDigits, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->spinYAxisDigits, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->spinXAxisMinTicks, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->spinXAxisMinorTicks, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->spinYAxisMinTicks, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->spinYAxisMinorTicks, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->cmbXLabelPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->cmbYLabelPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->cmbXLabel, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->cmbYLabel, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->cmbXMode1, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->cmbXMode2, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->cmbYMode1, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->cmbYMode2, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));

    connect(ui->cmbGridColor, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->cmbBackgroundColor, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->cmbGridLinestyle, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->spinGridWidth, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->widGraphSettings, SIGNAL(graphDataChanged()), this, SLOT(graphDataChanged()));

    connect(ui->chkPlotAutosize, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    connect(ui->spinHeight, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->spinWidth, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
}

void QFRDRTablePlotWidget::disconnectWidgets()
{
    //qDebug()<<"disconnectWidgets";
    disconnect(ui->listGraphs, SIGNAL(currentRowChanged(int)), this, SLOT(listGraphs_currentRowChanged(int)));

    disconnect(ui->edtTitle, SIGNAL(editingFinished()), this, SLOT(plotDataChanged()));
    disconnect(ui->edtXLabel, SIGNAL(editingFinished()), this, SLOT(plotDataChanged()));
    disconnect(ui->edtYLabel, SIGNAL(editingFinished()), this, SLOT(plotDataChanged()));
    disconnect(ui->chkGrid, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->chkLogX, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->chkLogY, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->chkX0Axis, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->chkY0Axis, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
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
    disconnect(ui->sliderKeyTransparency, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbKeyLayout, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbKeyPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->edtAxisAspect, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->edtDataAspect, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->chkKeepAxisAspect, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->chkKeepDataAspect, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinXAxisDigits, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinYAxisDigits, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinXAxisMinTicks, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinXAxisMinorTicks, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinYAxisMinTicks, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinYAxisMinorTicks, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbXLabelPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbYLabelPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbXLabel, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbYLabel, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbXMode1, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbXMode2, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbYMode1, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbYMode2, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbGridColor, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbBackgroundColor, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbGridLinestyle, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinGridWidth, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));

    disconnect(ui->chkPlotAutosize, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinHeight, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinWidth, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));

    disconnect(ui->widGraphSettings, SIGNAL(graphDataChanged()), this, SLOT(graphDataChanged()));
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

void QFRDRTablePlotWidget::on_btnSaveSystem_clicked() {

    QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/table/graph_templates/");
    QString dir=ProgramOptions::getInstance()->getQSettings()->value("QFRDRTablePlotWidget/lasttemplatedir", ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/table/graph_templates/").toString();
    QString filename=qfGetSaveFileName(this, tr("save graph settings template ..."), dir, tr("graph settings template  (*.gst)"))    ;
    if (!filename.isEmpty()) {
        bool ok=true;
        if (QFile::exists(filename)) {
            ok=false;
            if (QMessageBox::question(this, tr("save graph settings template ..."), tr("The file\n  '%1'\nalready exists. Overwrite?").arg(filename), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes) {
                ok=true;
            }
        }
        if (ok) {
            QSettings set(filename, QSettings::IniFormat);

            set.setValue("title", ui->edtTitle->text());
            set.setValue("xlabel",ui->edtXLabel->text());
            set.setValue("ylabel",ui->edtYLabel->text());
            set.setValue("autosize",ui->chkPlotAutosize->isChecked());
            set.setValue("plotwidth",ui->spinWidth->value());
            set.setValue("plotheight",ui->spinHeight->value());
            set.setValue("grid",ui->chkGrid->isChecked());
            set.setValue("logx",ui->chkLogX->isChecked());
            set.setValue("logy",ui->chkLogY->isChecked());
            set.setValue("x0axis",ui->chkX0Axis->isChecked());
            set.setValue("y0axis",ui->chkY0Axis->isChecked());
            set.setValue("showkey",ui->chkShowKey->isChecked());
            set.setValue("fontname", ui->cmbFontname->currentText());
            set.setValue("axisfontsize", ui->spinAxisFontSize->value());
            set.setValue("axislabelfontsize", ui->spinAxisLabelFontSize->value());
            set.setValue("keyfontsize", ui->spinKeyFontSize->value());
            set.setValue("titlefontsize", ui->spinTitleFontSize->value());
            set.setValue("keytransparency", ui->sliderKeyTransparency->value());
            set.setValue("keylayout", ui->cmbKeyLayout->currentIndex());
            set.setValue("keyposition", ui->cmbKeyPosition->currentIndex());
            set.setValue("axisaspect", ui->edtAxisAspect->value());
            set.setValue("dataaspect", ui->edtDataAspect->value());
            set.setValue("keepaxisaspect", ui->chkKeepAxisAspect->isChecked());
            set.setValue("keepdataaspect", ui->chkKeepDataAspect->isChecked());




            set.setValue("xdigits", ui->spinXAxisDigits->value());
            set.setValue("ydigits", ui->spinYAxisDigits->value());
            set.setValue("xminTicks", ui->spinXAxisMinTicks->value());
            set.setValue("xminorTicks", ui->spinXAxisMinorTicks->value());
            set.setValue("yminTicks", ui->spinYAxisMinTicks->value());
            set.setValue("yminorTicks", ui->spinYAxisMinorTicks->value());
            set.setValue("xlabelPos", JKQTPlabelPosition2String(ui->cmbXLabelPosition->getPosition()));
            set.setValue("ylabelPos", JKQTPlabelPosition2String(ui->cmbYLabelPosition->getPosition()));
            set.setValue("xlabelType", JKQTPCAlabelType2String(ui->cmbXLabel->getLabelType()));
            set.setValue("ylabelType", JKQTPCAlabelType2String(ui->cmbYLabel->getLabelType()));
            set.setValue("xlabelMode1", JKQTPCAdrawMode2String(ui->cmbXMode1->getDrawMode()));
            set.setValue("xlabelMode2", JKQTPCAdrawMode2String(ui->cmbXMode2->getDrawMode()));
            set.setValue("ylabelMode1", JKQTPCAdrawMode2String(ui->cmbYMode1->getDrawMode()));
            set.setValue("ylabelMode2", JKQTPCAdrawMode2String(ui->cmbYMode2->getDrawMode()));
            set.setValue("backgroundColor", QColor2String(ui->cmbBackgroundColor->currentColor()));
            set.setValue("gridColor", QColor2String(ui->cmbGridColor->currentColor()));
            set.setValue("gridStyle", QPenStyle2String(ui->cmbGridLinestyle->currentLineStyle()));
            set.setValue("gridWidth", ui->spinGridWidth->value());
        }
    }
    ProgramOptions::getInstance()->getQSettings()->setValue("QFRDRTablePlotWidget/lasttemplatedir", dir);
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




void QFRDRTablePlotWidget::on_btnLoadSystem_clicked() {
    QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/table/graph_templates/");
    QString dir=ProgramOptions::getInstance()->getQSettings()->value("QFRDRTablePlotWidget/lasttemplatedir", ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/table/graph_templates/").toString();
    QString filename=qfGetOpenFileName(this, tr("open graph settings template ..."), dir, tr("graph settings template (*.gst)"))    ;
    if (!filename.isEmpty()) {
        QSettings set(filename, QSettings::IniFormat);


        ui->chkPlotAutosize->setChecked(set.value("autosize",ui->chkPlotAutosize->isChecked()).toBool());
        ui->spinWidth->setValue(set.value("plotwidth", ui->spinWidth->value()).toInt());
        ui->spinHeight->setValue(set.value("plotheight", ui->spinHeight->value()).toInt());

        ui->edtTitle->setText(set.value("title", ui->edtTitle->text()).toString());
        ui->edtXLabel->setText(set.value("xlabel",ui->edtXLabel->text()).toString());
        ui->edtYLabel->setText(set.value("ylabel",ui->edtYLabel->text()).toString());
        ui->chkGrid->setChecked(set.value("grid",ui->chkGrid->isChecked()).toBool());
        ui->chkLogX->setChecked(set.value("logx",ui->chkLogX->isChecked()).toBool());
        ui->chkLogY->setChecked(set.value("logy",ui->chkLogY->isChecked()).toBool());
        ui->chkX0Axis->setChecked(set.value("x0axis",ui->chkX0Axis->isChecked()).toBool());
        ui->chkY0Axis->setChecked(set.value("y0axis",ui->chkY0Axis->isChecked()).toBool());
        ui->chkShowKey->setChecked(set.value("showkey",ui->chkShowKey->isChecked()).toBool());
        ui->cmbFontname->setCurrentFont(QFont(set.value("fontname", ui->cmbFontname->currentFont().family()).toString()));
        ui->spinAxisFontSize->setValue(set.value("axisfontsize", ui->spinAxisFontSize->value()).toDouble());
        ui->spinAxisLabelFontSize->setValue(set.value("axislabelfontsize", ui->spinAxisLabelFontSize->value()).toDouble());
        ui->spinKeyFontSize->setValue(set.value("keyfontsize", ui->spinKeyFontSize->value()).toDouble());
        ui->spinTitleFontSize->setValue(set.value("titlefontsize", ui->spinTitleFontSize->value()).toDouble());
        ui->sliderKeyTransparency->setValue(set.value("keytransparency", ui->sliderKeyTransparency->value()).toDouble());
        ui->cmbKeyLayout->setCurrentIndex(set.value("keylayout", ui->cmbKeyLayout->currentIndex()).toInt());
        ui->cmbKeyPosition->setCurrentIndex(set.value("keyposition", ui->cmbKeyPosition->currentIndex()).toInt());
        ui->edtAxisAspect->setValue(set.value("axisaspect", ui->edtAxisAspect->value()).toDouble());
        ui->edtDataAspect->setValue(set.value("dataaspect", ui->edtDataAspect->value()).toDouble());
        ui->chkKeepAxisAspect->setChecked(set.value("keepaxisaspect", ui->chkKeepAxisAspect->isChecked()).toBool());
        ui->chkKeepDataAspect->setChecked(set.value("keepdataaspect", ui->chkKeepDataAspect->isChecked()).toBool());
        ui->spinXAxisDigits->setValue(set.value("xdigits", ui->spinXAxisDigits->value()).toInt());
        ui->spinYAxisDigits->setValue(set.value("ydigits", ui->spinYAxisDigits->value()).toInt());
        ui->spinXAxisMinTicks->setValue(set.value("xminTicks", ui->spinXAxisMinTicks->value()).toInt());
        ui->spinXAxisMinorTicks->setValue(set.value("xminorTicks", ui->spinXAxisMinorTicks->value()).toInt());
        ui->spinYAxisMinTicks->setValue(set.value("yminTicks", ui->spinYAxisMinTicks->value()).toInt());
        ui->spinYAxisMinorTicks->setValue(set.value("yminorTicks", ui->spinYAxisMinorTicks->value()).toInt());
        ui->cmbXLabelPosition->setPosition(String2JKQTPlabelPosition(set.value("xlabelPos", JKQTPlabelPosition2String(ui->cmbXLabelPosition->getPosition())).toString()));
        ui->cmbYLabelPosition->setPosition(String2JKQTPlabelPosition(set.value("ylabelPos", JKQTPlabelPosition2String(ui->cmbYLabelPosition->getPosition())).toString()));
        ui->cmbXLabel->setLabelType(String2JKQTPCAlabelType(set.value("xlabelType", JKQTPCAlabelType2String(ui->cmbXLabel->getLabelType())).toString()));
        ui->cmbYLabel->setLabelType(String2JKQTPCAlabelType(set.value("ylabelType", JKQTPCAlabelType2String(ui->cmbYLabel->getLabelType())).toString()));
        ui->cmbXMode1->setDrawMode(String2JKQTPCAdrawMode(set.value("xlabelMode1", JKQTPCAdrawMode2String(ui->cmbXMode1->getDrawMode())).toString()));
        ui->cmbXMode2->setDrawMode(String2JKQTPCAdrawMode(set.value("xlabelMode2", JKQTPCAdrawMode2String(ui->cmbXMode2->getDrawMode())).toString()));
        ui->cmbYMode1->setDrawMode(String2JKQTPCAdrawMode(set.value("ylabelMode1", JKQTPCAdrawMode2String(ui->cmbYMode1->getDrawMode())).toString()));
        ui->cmbYMode2->setDrawMode(String2JKQTPCAdrawMode(set.value("ylabelMode2", JKQTPCAdrawMode2String(ui->cmbYMode2->getDrawMode())).toString()));
        ui->cmbGridColor->setCurrentColor(QColor(set.value("gridColor", QColor2String(ui->cmbGridColor->currentColor())).toString()));
        ui->cmbBackgroundColor->setCurrentColor(QColor(set.value("backgroundColor", QColor2String(ui->cmbBackgroundColor->currentColor())).toString()));
        ui->cmbGridLinestyle->setCurrentLineStyle(String2QPenStyle(set.value("gridStyle", QPenStyle2String(ui->cmbGridLinestyle->currentLineStyle())).toString()));
        ui->spinGridWidth->setValue(set.value("gridWidth", ui->spinGridWidth->value()).toDouble());
    }
    ProgramOptions::getInstance()->getQSettings()->setValue("QFRDRTablePlotWidget/lasttemplatedir", dir);
}

void QFRDRTablePlotWidget::doFit(int xCol, int yCol, int sigmaCol, int plot, QString function)
{
    emit performFit(xCol, yCol, sigmaCol, plot, function, ui->chkLogX->isChecked(), ui->chkLogY->isChecked());
}

void QFRDRTablePlotWidget::doRegression(int xCol, int yCol, int sigmaCol, int plot)
{
    emit performRegression(xCol, yCol, sigmaCol, plot, ui->chkLogX->isChecked(), ui->chkLogY->isChecked());
}

