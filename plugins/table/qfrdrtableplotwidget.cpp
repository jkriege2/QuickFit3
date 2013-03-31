#include "qfrdrtableplotwidget.h"
#include "programoptions.h"
#include "ui_qfrdrtableplotwidget.h"
#include "qfrdrtable.h"
#include "qftools.h"
#include "qfdoubleedit.h"
#include "jkqtpparsedfunctionelements.h"



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

    functionRef=new QFFunctionReferenceTool(NULL);
    functionRef->setCompleterFile(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/table/table_expression.txt");
    functionRef->setDefaultWordsMathExpression();


    updating=true;
    ui->setupUi(this);

    //ui->formLayout_3->removeWidget(ui->widSaveCoordSettings);
    //ui->tabWidget->setCornerWidget(ui->widSaveCoordSettings);

    functionRef->registerEditor(ui->edtFunction);
    ui->edtXMin->setCheckBounds(false, true);
    ui->edtXMax->setCheckBounds(true, false);
    ui->edtYMin->setCheckBounds(false, true);
    ui->edtYMax->setCheckBounds(true, false);
    ui->edtImageHeight->setCheckBounds(true, false);
    ui->edtImageHeight->setRange(0, 1e10);
    ui->edtImageMax->setCheckBounds(false, false);
    ui->edtImageMin->setCheckBounds(false, false);
    ui->edtImageWidth->setCheckBounds(true, false);
    ui->edtImageWidth->setRange(0, 1e10);
    ui->edtImageX->setCheckBounds(true, false);
    ui->edtImageY->setCheckBounds(true, false);
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
    reloadColumns(ui->cmbLinesXData);
    reloadColumns(ui->cmbLinesXError);
    reloadColumns(ui->cmbLinesYData);
    reloadColumns(ui->cmbLinesYError);
    updating=false;
    connectWidgets();
    listGraphs_currentRowChanged(ui->listGraphs->currentRow());
    updateGraph();
}

void QFRDRTablePlotWidget::rawDataChanged() {
    if (updating) return;
    disconnectWidgets();
    reloadColumns(ui->cmbLinesXData);
    reloadColumns(ui->cmbLinesXError);
    reloadColumns(ui->cmbLinesYData);
    reloadColumns(ui->cmbLinesYError);
    connectWidgets();
    updateGraph();
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
            case QFRDRTable::gtBarsVertical:
                ui->cmbGraphType->setCurrentIndex(7);
                break;
            case QFRDRTable::gtBarsHorizontal:
                ui->cmbGraphType->setCurrentIndex(8);
                break;
            case QFRDRTable::gtImage:
                ui->cmbGraphType->setCurrentIndex(9);
                break;
            case QFRDRTable::gtMaskImage:
                ui->cmbGraphType->setCurrentIndex(10);
                break;
            case QFRDRTable::gtFunction:
                ui->cmbGraphType->setCurrentIndex(11);
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
        ui->cmbImageFalseColor->setCurrentColor(graph.imageFalseColor);
        ui->sliderImageFalseColor->setValue(graph.imageFalseTransparent*255.0);
        ui->cmbImageTrueColor->setCurrentColor(graph.imageTrueColor);
        ui->sliderImageTrueColor->setValue(graph.imageTrueTransparent*255.0);
        ui->edtImageHeight->setValue(graph.imageHeight);
        ui->edtImageWidth->setValue(graph.imageWidth);
        ui->edtImageMax->setValue(graph.imageMax);
        ui->edtImageMin->setValue(graph.imageMin);
        ui->edtImageX->setValue(graph.imageX);
        ui->edtImageY->setValue(graph.imageY);
        ui->spinImageWidth->setValue(graph.imagePixelWidth);
        ui->cmbColormap->setColorPalette(graph.imagePalette);
        ui->chkImageAutorange->setChecked(graph.imageAutoRange);


        ui->chkImageColorbarRight->setChecked(graph.imageColorbarRight);
        ui->chkImageColorbarTop->setChecked(graph.imageColorbarTop);
        ui->edtColorbarLabel->setText(graph.imageLegend);
        ui->spinColorbarWidth->setValue(graph.colorbarWidth);
        ui->spinColorbarHeight->setValue(graph.colorbarRelativeHeight*100.0);
        ui->edtFunction->setText(graph.function);
        ui->chkSTrided->setChecked(graph.isStrided);
        ui->spinStride->setValue(graph.stride);
        ui->spinStrideStart->setValue(graph.strideStart);

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

    if (current) {
        QFTablePluginModel* tab=current->model();
        while (combo->count()-1>tab->columnCount() && combo->count()>1) {
            combo->removeItem(combo->count()-1);
        }
        while (combo->count()-1<tab->columnCount() && tab->columnCount()>0) {
            combo->addItem("");
        }
        combo->setItemText(0, tr("--- none ---"));
        combo->setItemData(0, QVariant());
        for (int i=0; i<tab->columnCount(); i++) {
            combo->setItemText(i+1, tr("#%1: %2").arg(i+1).arg(tab->columnTitle(i)));
            combo->setItemData(i+1, tab->columnTitle(i));
        }
    } else {
        combo->clear();
        combo->addItem(tr("--- none ---"));
    }
    updating=updt;
    //connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    combo->setCurrentIndex(idx);
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
            graph.title=ui->edtGraphTitle->text();
            ui->listGraphs->item(r)->setText(graph.title);
            ui->listGraphs->item(r)->setIcon(ui->cmbGraphType->itemIcon(ui->cmbGraphType->currentIndex()));

            switch(ui->cmbGraphType->currentIndex()) {
                case 1: graph.type=QFRDRTable::gtImpulsesVertical; break;
                case 2: graph.type=QFRDRTable::gtImpulsesHorizontal; break;
                case 3: graph.type=QFRDRTable::gtFilledCurveX; break;
                case 4: graph.type=QFRDRTable::gtFilledCurveY; break;
                case 5: graph.type=QFRDRTable::gtStepsVertical; break;
                case 6: graph.type=QFRDRTable::gtStepsHorizontal; break;
                case 7: graph.type=QFRDRTable::gtBarsVertical; break;
                case 8: graph.type=QFRDRTable::gtBarsHorizontal; break;
                case 9: graph.type=QFRDRTable::gtImage; break;
                case 10: graph.type=QFRDRTable::gtMaskImage; break;
                case 11: graph.type=QFRDRTable::gtFunction; break;
                case 0:
                default: graph.type=QFRDRTable::gtLines; break;
            }
            updatePlotWidgetVisibility();



            graph.xcolumn=qMax(-1, ui->cmbLinesXData->currentIndex()-1);
            graph.xerrorcolumn=qMax(-1, ui->cmbLinesXError->currentIndex()-1);
            graph.ycolumn=qMax(-1, ui->cmbLinesYData->currentIndex()-1);
            graph.yerrorcolumn=qMax(-1, ui->cmbLinesYError->currentIndex()-1);

            QColor oldColor=graph.color;
            QColor oldDefaultErrorColor=oldColor.darker();
            QColor oldDefaultFillColor=oldColor.lighter();
            graph.color=ui->cmbLineColor->currentColor();
            if (graph.errorColor!=oldDefaultErrorColor) graph.errorColor=ui->cmbErrorColor->currentColor();
            else {
                graph.errorColor=graph.color.darker();
                ui->cmbErrorColor->setCurrentColor(graph.errorColor);
            }
            //qDebug()<<graph.fillColor.name()<<oldDefaultFillColor.name();

            if (graph.fillColor!=oldDefaultFillColor) graph.fillColor=ui->cmbFillColor->currentColor();
            else {
                graph.fillColor=graph.color.lighter();
                ui->cmbFillColor->setCurrentColor(graph.fillColor);
            }
            graph.errorStyle=ui->cmbErrorStyle->getErrorStyle();
            graph.symbol=ui->cmbSymbol->getSymbol();
            graph.style=ui->cmbLineStyle->currentLineStyle();
            graph.linewidth=ui->spinLineWidth->value();
            graph.symbolSize=ui->spinSymbolSize->value();
            graph.drawLine=ui->chkDrawLine->isChecked();
            graph.colorTransparent=double(ui->sliderPlotTransparent->value())/255.0;
            graph.errorColorTransparent=double(ui->sliderErrorTransparent->value())/255.0;
            graph.fillColorTransparent=double(ui->sliderFillTransparent->value())/255.0;
            graph.imageFalseTransparent=double(ui->sliderImageFalseColor->value())/255.0;
            graph.imageTrueTransparent=double(ui->sliderImageTrueColor->value())/255.0;
            graph.imageFalseColor=ui->cmbImageFalseColor->currentColor();
            graph.imageTrueColor=ui->cmbImageTrueColor->currentColor();
            graph.imageHeight=ui->edtImageHeight->value();
            graph.imageWidth=ui->edtImageWidth->value();
            graph.imageMax=ui->edtImageMax->value();
            graph.imageMin=ui->edtImageMin->value();
            graph.imageX=ui->edtImageX->value();
            graph.imageY=ui->edtImageY->value();
            graph.imagePixelWidth=ui->spinImageWidth->value();
            graph.imagePalette=ui->cmbColormap->colorPalette();
            graph.imageAutoRange=ui->chkImageAutorange->isChecked();
            graph.imageColorbarRight=ui->chkImageColorbarRight->isChecked();
            graph.imageColorbarTop=ui->chkImageColorbarTop->isChecked();
            graph.imageLegend=ui->edtColorbarLabel->text();
            graph.colorbarWidth=ui->spinColorbarWidth->value();
            graph.colorbarRelativeHeight=ui->spinColorbarHeight->value()/100.0;
            graph.function=ui->edtFunction->text();
            graph.isStrided=ui->chkSTrided->isChecked();
            graph.stride=ui->spinStride->value();
            graph.strideStart=ui->spinStrideStart->value();

            if (ui->edtXLabel->text()=="x" && ui->cmbLinesXData->currentIndex()>0) {
                QString txt=ui->cmbLinesXData->currentText();
                int idx=txt.indexOf(':');
                if (idx>=0) txt=txt.right(txt.size()-(idx+1));
                ui->edtXLabel->setText(txt);
            }
            if (ui->edtYLabel->text()=="y" && ui->cmbLinesYData->currentIndex()>0) {
                QString txt=ui->cmbLinesYData->currentText();
                int idx=txt.indexOf(':');
                if (idx>=0) txt=txt.right(txt.size()-(idx+1));
                ui->edtYLabel->setText(txt);
            }
            if (ui->edtTitle->text().isEmpty() && ui->cmbLinesYData->currentIndex()>0) {
                QString txt=ui->cmbLinesYData->currentText();
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
        emit plotTitleChanged(this->plot, p.title);
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
        ui->plotter->get_plotter()->set_key_item_height(qMax(15.0, 1.0*keyfm.height()));
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
                ui->plotter->addGraph(pg);
            } else if (g.type==QFRDRTable::gtBarsVertical) {
                JKQTPbarHorizontalGraph* pg=new JKQTPbarHorizontalGraph(ui->plotter->get_plotter());
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
                pg->set_style(g.style);
                ui->plotter->addGraph(pg);

            } else if (g.type==QFRDRTable::gtImage) {
                JKQTPColumnMathImage* pg=new JKQTPColumnMathImage(ui->plotter->get_plotter());
                pg->set_title(g.title);

                if (g.xcolumn>=0 && g.xcolumn<(long)ui->plotter->getDatastore()->getColumnCount())  pg->set_imageColumn(g.xcolumn);
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
                JKQTPxParsedFunctionLineGraph* pg=new JKQTPxParsedFunctionLineGraph(ui->plotter->get_plotter());
                pg->set_title(g.title);
                pg->set_function(g.function);
                //qDebug()<<"adding function plot "<<g.function;
                if (g.ycolumn>=0 && g.ycolumn<(long)ui->plotter->getDatastore()->getColumnCount()) {
                    pg->set_parameterColumn(g.ycolumn);
                }
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
                pg->set_style(g.style);
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

void QFRDRTablePlotWidget::updatePlotWidgetVisibility() {
    if (current) {
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;
        int r=ui->listGraphs->currentRow();
        QFRDRTable::PlotInfo p=current->getPlot(this->plot);
        if (r>=0 && r<p.graphs.size()) {
            QFRDRTable::GraphInfo graph=p.graphs.at(r);


            /*ui->cmbErrorColor->setVisible(true);
            ui->cmbErrorStyle->setVisible(true);
            ui->cmbLinesXError->setVisible(true);
            ui->cmbLinesYError->setVisible(true);
            ui->chkDrawLine->setVisible(true);
            ui->cmbSymbol->setVisible(true);
            ui->spinSymbolSize->setVisible(true);
            ui->cmbFillColor->setVisible(true);
            ui->cmbLineStyle->setVisible(true);
            ui->sliderErrorTransparent->setVisible(true);
            ui->sliderFillTransparent->setVisible(true);
            ui->sliderPlotTransparent->setVisible(true);*/
            ui->widErrorStyle->setVisible(true);
            ui->widFillColor->setVisible(true);
            ui->widGraphSettings->setVisible(true);
            ui->widImage->setVisible(true);
            ui->widLineStyle->setVisible(true);
            ui->widSymbol->setVisible(true);
            ui->labDataX->setVisible(true);
            ui->labDataY->setVisible(true);
            ui->labErrorStyle->setVisible(true);
            ui->labErrorX->setVisible(true);
            ui->labErrorY->setVisible(true);
            ui->labFillColor->setVisible(true);
            ui->labImage->setVisible(true);
            ui->labLinestyle->setVisible(true);
            ui->labSymbol->setVisible(true);
            ui->labTitle->setVisible(true);
            ui->labType->setVisible(true);
            ui->chkDrawLine->setVisible(true);
            ui->cmbLineStyle->setVisible(true);
            ui->cmbLinesXData->setVisible(true);
            ui->cmbLinesXError->setVisible(true);
            ui->cmbLinesYData->setVisible(true);
            ui->cmbLinesYError->setVisible(true);
            ui->widFunction->setVisible(false);
            ui->labFuction->setVisible(false);
            ui->chkSTrided->setVisible(true);
            ui->widStride->setVisible(true);

            switch(ui->cmbGraphType->currentIndex()) {


                case 1:
                    //graph.type=QFRDRTable::gtImpulsesVertical;
                    ui->cmbLinesXError->setVisible(false);
                    ui->labErrorX->setVisible(false);
                    ui->chkDrawLine->setVisible(false);
                    ui->cmbLineStyle->setVisible(false);
                    ui->labImage->setVisible(false);
                    ui->widImage->setVisible(false);

                    /*ui->widLineStyle->setVisible(false);
                    ui->cmbLinesXError->setVisible(false);
                    ui->chkDrawLine->setVisible(false);
                    ui->cmbLineStyle->setVisible(false);*/
                    break;
                case 2:
                    //graph.type=QFRDRTable::gtImpulsesHorizontal;
                    ui->cmbLinesXError->setVisible(false);
                    ui->labErrorX->setVisible(false);
                    ui->chkDrawLine->setVisible(false);
                    ui->cmbLineStyle->setVisible(false);
                    ui->labImage->setVisible(false);
                    ui->widImage->setVisible(false);

                    /*ui->cmbErrorColor->setVisible(true);
                    ui->cmbErrorStyle->setVisible(true);
                    ui->cmbLinesXError->setVisible(false);
                    ui->cmbLinesYError->setVisible(true);
                    ui->chkDrawLine->setVisible(false);
                    ui->cmbSymbol->setVisible(true);
                    ui->spinSymbolSize->setVisible(true);
                    ui->cmbFillColor->setVisible(true);
                    ui->cmbLineStyle->setVisible(false);                */

                    break;
                case 3:
                    //graph.type=QFRDRTable::gtFilledCurveX;
                    ui->cmbLinesXError->setVisible(false);
                    ui->labErrorX->setVisible(false);
                    ui->labSymbol->setVisible(false);
                    ui->widSymbol->setVisible(false);
                    ui->labImage->setVisible(false);
                    ui->widImage->setVisible(false);

                    /*ui->cmbErrorColor->setVisible(true);
                    ui->cmbErrorStyle->setVisible(true);
                    ui->cmbLinesXError->setVisible(false);
                    ui->cmbLinesYError->setVisible(true);
                    ui->chkDrawLine->setVisible(false);
                    ui->cmbSymbol->setVisible(false);
                    ui->spinSymbolSize->setVisible(false);
                    ui->cmbFillColor->setVisible(true);
                    ui->cmbLineStyle->setVisible(true);*/
                    break;
                case 4:
                    //graph.type=QFRDRTable::gtFilledCurveY;
                    ui->labImage->setVisible(false);
                    ui->widImage->setVisible(false);
                    ui->cmbLinesXError->setVisible(false);
                    ui->labErrorX->setVisible(false);
                    ui->labSymbol->setVisible(false);
                    ui->widSymbol->setVisible(false);

                    /*ui->cmbErrorColor->setVisible(true);
                    ui->cmbErrorStyle->setVisible(true);
                    ui->cmbLinesXError->setVisible(false);
                    ui->cmbLinesYError->setVisible(true);
                    ui->chkDrawLine->setVisible(false);
                    ui->cmbSymbol->setVisible(false);
                    ui->spinSymbolSize->setVisible(false);
                    ui->cmbFillColor->setVisible(true);
                    ui->cmbLineStyle->setVisible(true);*/
                    break;
                case 5:
                    //graph.type=QFRDRTable::gtStepsVertical;
                    ui->labImage->setVisible(false);
                    ui->widImage->setVisible(false);
                    ui->cmbLinesXError->setVisible(false);
                    ui->labErrorX->setVisible(false);
                    ui->cmbLinesYError->setVisible(false);
                    ui->labErrorY->setVisible(false);
                    ui->labSymbol->setVisible(false);
                    ui->widSymbol->setVisible(false);
                    ui->widErrorStyle->setVisible(false);
                    ui->labErrorStyle->setVisible(false);

                    /*ui->cmbErrorColor->setVisible(false);
                    ui->cmbErrorStyle->setVisible(false);
                    ui->cmbLinesXError->setVisible(false);
                    ui->cmbLinesYError->setVisible(false);
                    ui->cmbSymbol->setVisible(false);
                    ui->spinSymbolSize->setVisible(false);
                    ui->sliderErrorTransparent->setVisible(false);*/
                    break;
                case 6:
                    //graph.type=QFRDRTable::gtStepsHorizontal;
                    ui->labImage->setVisible(false);
                    ui->widImage->setVisible(false);
                    ui->labImage->setVisible(false);
                    ui->widImage->setVisible(false);
                    ui->cmbLinesXError->setVisible(false);
                    ui->labErrorX->setVisible(false);
                    ui->cmbLinesYError->setVisible(false);
                    ui->labErrorY->setVisible(false);
                    ui->labSymbol->setVisible(false);
                    ui->widSymbol->setVisible(false);
                    ui->widErrorStyle->setVisible(false);
                    ui->labErrorStyle->setVisible(false);
/*                    ui->cmbErrorColor->setVisible(false);
                    ui->cmbErrorStyle->setVisible(false);
                    ui->cmbLinesXError->setVisible(false);
                    ui->cmbLinesYError->setVisible(false);
                    ui->cmbSymbol->setVisible(false);
                    ui->spinSymbolSize->setVisible(false);
                    ui->sliderErrorTransparent->setVisible(false);*/
                    break;
                case 7:
                    //graph.type=QFRDRTable::gtbarsVertical;
                    ui->labImage->setVisible(false);
                    ui->widImage->setVisible(false);
                    ui->cmbLinesXError->setVisible(false);
                    ui->labErrorX->setVisible(false);
                    ui->cmbLinesYError->setVisible(false);
                    ui->labErrorY->setVisible(false);
                    ui->labSymbol->setVisible(false);
                    ui->widSymbol->setVisible(false);
                    ui->widErrorStyle->setVisible(false);
                    ui->labErrorStyle->setVisible(false);
                    ui->chkDrawLine->setVisible(false);

/*                    ui->cmbErrorColor->setVisible(false);
                    ui->cmbErrorStyle->setVisible(false);
                    ui->cmbLinesXError->setVisible(false);
                    ui->cmbLinesYError->setVisible(false);
                    ui->chkDrawLine->setVisible(false);
                    ui->cmbSymbol->setVisible(false);
                    ui->spinSymbolSize->setVisible(false);
                    ui->sliderErrorTransparent->setVisible(false);*/
                    break;
                case 8:
                    //graph.type=QFRDRTable::gtbarsHorizontal;
                    ui->labImage->setVisible(false);
                    ui->widImage->setVisible(false);
                    ui->cmbLinesXError->setVisible(false);
                    ui->labErrorX->setVisible(false);
                    ui->cmbLinesYError->setVisible(false);
                    ui->labErrorY->setVisible(false);
                    ui->labSymbol->setVisible(false);
                    ui->widSymbol->setVisible(false);
                    ui->widErrorStyle->setVisible(false);
                    ui->labErrorStyle->setVisible(false);
                    ui->chkDrawLine->setVisible(false);
                    break;
                case 9:
                    //graph.type=QFRDRTable::gtImage;
                    ui->cmbLinesXError->setVisible(false);
                    ui->labErrorX->setVisible(false);
                    ui->cmbLinesYData->setVisible(false);
                    ui->labDataY->setVisible(false);
                    ui->cmbLinesYError->setVisible(false);
                    ui->labErrorY->setVisible(false);
                    ui->labSymbol->setVisible(false);
                    ui->widSymbol->setVisible(false);
                    ui->widErrorStyle->setVisible(false);
                    ui->labErrorStyle->setVisible(false);
                    ui->widColor->setVisible(false);
                    ui->labColor->setVisible(false);
                    ui->widFillColor->setVisible(false);
                    ui->labFillColor->setVisible(false);
                    ui->widLineStyle->setVisible(false);
                    ui->labLinestyle->setVisible(false);
                    break;
                case 10:
                    //graph.type=QFRDRTable::gtMaskImage;
                    ui->cmbLinesXError->setVisible(false);
                    ui->labErrorX->setVisible(false);
                    ui->cmbLinesYData->setVisible(false);
                    ui->labDataY->setVisible(false);
                    ui->cmbLinesYError->setVisible(false);
                    ui->labErrorY->setVisible(false);
                    ui->labSymbol->setVisible(false);
                    ui->widSymbol->setVisible(false);
                    ui->widErrorStyle->setVisible(false);
                    ui->labErrorStyle->setVisible(false);
                    ui->widColor->setVisible(false);
                    ui->labColor->setVisible(false);
                    ui->widFillColor->setVisible(false);
                    ui->labFillColor->setVisible(false);
                    ui->widLineStyle->setVisible(false);
                    ui->labLinestyle->setVisible(false);
                    break;
                case 11:
                    //graph.type=QFRDRTable::gtFunction;
                    ui->labImage->setVisible(false);
                    ui->widImage->setVisible(false);
                    ui->cmbLinesXError->setVisible(false);
                    ui->labErrorX->setVisible(false);
                    ui->cmbLinesXData->setVisible(false);
                    ui->labDataX->setVisible(false);
                    ui->cmbLinesYError->setVisible(false);
                    ui->labErrorY->setVisible(false);
                    ui->labImage->setVisible(false);
                    ui->widImage->setVisible(false);
                    ui->widFunction->setVisible(true);
                    ui->labFuction->setVisible(true);
                    ui->labSymbol->setVisible(false);
                    ui->widSymbol->setVisible(false);
                    ui->chkSTrided->setVisible(false);
                    ui->widStride->setVisible(false);

                    /*ui->widLineStyle->setVisible(false);
                    ui->cmbLinesXError->setVisible(false);
                    ui->chkDrawLine->setVisible(false);
                    ui->cmbLineStyle->setVisible(false);*/
                    break;

                case 0:
                default:
                    //graph.type=QFRDRTable::gtLines;
                    ui->labImage->setVisible(false);
                    ui->widImage->setVisible(false);
                    break;
            }
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


    connect(ui->edtFunction, SIGNAL(editingFinished()), this, SLOT(graphDataChanged()));
    connect(ui->edtGraphTitle, SIGNAL(editingFinished()), this, SLOT(graphDataChanged()));
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
    connect(ui->cmbImageFalseColor, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->cmbImageTrueColor, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->cmbColormap, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->spinImageWidth, SIGNAL(valueChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->edtImageHeight, SIGNAL(valueChanged(double)), this, SLOT(graphDataChanged()));
    connect(ui->edtImageMax, SIGNAL(valueChanged(double)), this, SLOT(graphDataChanged()));
    connect(ui->edtImageMin, SIGNAL(valueChanged(double)), this, SLOT(graphDataChanged()));
    connect(ui->edtImageHeight, SIGNAL(valueChanged(double)), this, SLOT(graphDataChanged()));
    connect(ui->edtImageX, SIGNAL(valueChanged(double)), this, SLOT(graphDataChanged()));
    connect(ui->edtImageY, SIGNAL(valueChanged(double)), this, SLOT(graphDataChanged()));
    connect(ui->sliderImageFalseColor, SIGNAL(valueChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->sliderImageTrueColor, SIGNAL(valueChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->edtColorbarLabel, SIGNAL(editingFinished()), this, SLOT(graphDataChanged()));
    connect(ui->spinColorbarWidth, SIGNAL(valueChanged(double)), this, SLOT(graphDataChanged()));
    connect(ui->spinColorbarHeight, SIGNAL(valueChanged(double)), this, SLOT(graphDataChanged()));
    connect(ui->chkImageColorbarRight, SIGNAL(toggled(bool)), this, SLOT(graphDataChanged()));
    connect(ui->chkImageColorbarTop, SIGNAL(toggled(bool)), this, SLOT(graphDataChanged()));
    connect(ui->chkImageAutorange, SIGNAL(toggled(bool)), this, SLOT(graphDataChanged()));
    connect(ui->chkSTrided, SIGNAL(toggled(bool)), this, SLOT(graphDataChanged()));
    connect(ui->spinStride, SIGNAL(valueChanged(int)), this, SLOT(graphDataChanged()));
    connect(ui->spinStrideStart, SIGNAL(valueChanged(int)), this, SLOT(graphDataChanged()));
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


    disconnect(ui->edtFunction, SIGNAL(editingFinished()), this, SLOT(graphDataChanged()));
    disconnect(ui->edtGraphTitle, SIGNAL(editingFinished()), this, SLOT(graphDataChanged()));
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
    disconnect(ui->cmbImageFalseColor, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->cmbImageTrueColor, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->cmbColormap, SIGNAL(currentIndexChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->spinImageWidth, SIGNAL(valueChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->edtImageHeight, SIGNAL(valueChanged(double)), this, SLOT(graphDataChanged()));
    disconnect(ui->edtImageMax, SIGNAL(valueChanged(double)), this, SLOT(graphDataChanged()));
    disconnect(ui->edtImageMin, SIGNAL(valueChanged(double)), this, SLOT(graphDataChanged()));
    disconnect(ui->edtImageHeight, SIGNAL(valueChanged(double)), this, SLOT(graphDataChanged()));
    disconnect(ui->edtImageX, SIGNAL(valueChanged(double)), this, SLOT(graphDataChanged()));
    disconnect(ui->edtImageY, SIGNAL(valueChanged(double)), this, SLOT(graphDataChanged()));
    disconnect(ui->sliderImageFalseColor, SIGNAL(valueChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->sliderImageTrueColor, SIGNAL(valueChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->edtColorbarLabel, SIGNAL(editingFinished()), this, SLOT(graphDataChanged()));
    disconnect(ui->spinColorbarWidth, SIGNAL(valueChanged(double)), this, SLOT(graphDataChanged()));
    disconnect(ui->spinColorbarHeight, SIGNAL(valueChanged(double)), this, SLOT(graphDataChanged()));
    disconnect(ui->chkImageColorbarRight, SIGNAL(toggled(bool)), this, SLOT(graphDataChanged()));
    disconnect(ui->chkImageColorbarTop, SIGNAL(toggled(bool)), this, SLOT(graphDataChanged()));
    disconnect(ui->chkImageAutorange, SIGNAL(toggled(bool)), this, SLOT(graphDataChanged()));
    disconnect(ui->chkSTrided, SIGNAL(toggled(bool)), this, SLOT(graphDataChanged()));
    disconnect(ui->spinStride, SIGNAL(valueChanged(int)), this, SLOT(graphDataChanged()));
    disconnect(ui->spinStrideStart, SIGNAL(valueChanged(int)), this, SLOT(graphDataChanged()));

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
        ui->edtGraphTitle->setFocus();

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
        ui->edtGraphTitle->setFocus();

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
    ui->edtGraphTitle->setFocus();
}

void QFRDRTablePlotWidget::on_edtFunction_textChanged(const QString &text)
{
    try {
        ui->labFunctionOK->setText(tr("<font color=\"darkgreen\">function OK</font>"));
        jkMathParser mp; // instanciate
        jkMathParser::jkmpNode* n;
        // parse some numeric expression
        n=mp.parse(text.toStdString());
        delete n;
    } catch(std::exception& E) {
        ui->labFunctionOK->setText(tr("<font color=\"red\">ERROR in function:</font> %1").arg(E.what()));
    }
}

void QFRDRTablePlotWidget::on_btnFunctionHelp_clicked()
{
     if (current)
         QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelpDirectory("table")+"/mathparser.html");
}


void QFRDRTablePlotWidget::on_btnLoadSystem_clicked() {
    QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/table/graph_templates/");
    QString dir=ProgramOptions::getInstance()->getQSettings()->value("QFRDRTablePlotWidget/lasttemplatedir", ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/table/graph_templates/").toString();
    QString filename=qfGetOpenFileName(this, tr("open graph settings template ..."), dir, tr("graph settings template (*.gst)"))    ;
    if (!filename.isEmpty()) {
        QSettings set(filename, QSettings::IniFormat);


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

