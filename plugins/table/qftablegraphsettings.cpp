#include "qftablegraphsettings.h"
#include "ui_qftablegraphsettings.h"
#include "programoptions.h"
#include "qfrdrtable.h"
#include "qftools.h"
#include "qfdoubleedit.h"
#include "qfmathparserxfunctionlinegraph.h"
#include "qffitfunctionvalueinputdelegate.h"


QFTableGraphSettings::QFTableGraphSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFTableGraphSettings)
{
    headerModel=new QFTableModelColumnHeaderModel(NULL, this);
    //headerModel=new QStringListModel(this);
    functionRef=new QFFunctionReferenceTool(NULL);
    functionRef->setCompleterFile(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/table/table_expression.txt");
    functionRef->setDefaultWordsMathExpression();

    actFit=new QAction(QIcon(":/table/fit.png"), tr("least squares fit"), this);
    connect(actFit, SIGNAL(triggered()), this, SLOT(doFit()));
    actRegression=new QAction(QIcon(":/table/regression.png"), tr("regression analysis"), this);
    connect(actRegression, SIGNAL(triggered()), this, SLOT(doRegression()));




    updating=true;

    ui->setupUi(this);

    ui->btnRefit->setVisible(false);

    ui->tabFunctionParameters->setItemDelegate(new QFFitFunctionValueInputDelegate(ui->tabFunctionParameters));
    fitfuncValuesTable=new QFFitFunctionValueInputTable(this);
    fitfuncValuesTable->setEditErrors(false);
    fitfuncValuesTable->setEditFix(false);
    fitfuncValuesTable->setEditRanges(false);
    ui->tabFunctionParameters->setModel(fitfuncValuesTable);

    ui->btnFit->addAction(actRegression);
    ui->btnFit->addAction(actFit);
    ui->btnFit->setDefaultAction(actFit);
    ui->btnFit->setVisible(false);
    functionRef->registerEditor(ui->edtFunction);
    ui->edtImageHeight->setCheckBounds(true, false);
    ui->edtImageHeight->setRange(0, 1e10);
    ui->edtImageMax->setCheckBounds(false, false);
    ui->edtImageMin->setCheckBounds(false, false);
    ui->edtImageWidth->setCheckBounds(true, false);
    ui->edtImageWidth->setRange(0, 1e10);
    ui->edtImageX->setCheckBounds(true, false);
    ui->edtImageY->setCheckBounds(true, false);
    ui->cmbGraphType->setCurrentIndex(0);




    ui->cmbGraphType->clear();
    ui->cmbGraphType->addItem(QIcon(":/table/icons/plot_pointslines.png"), tr("lines/scatter plot"), QFRDRTable::gtLines);
    ui->cmbGraphType->addItem(QIcon(":/table/icons/plot_pscatter.png"), tr("parametr. scatter plot"), QFRDRTable::gtParametrizedScatter);
    ui->cmbGraphType->addItem(QIcon(":/table/icons/plot_vimpulses.png"), tr("vertical impulses"), QFRDRTable::gtImpulsesVertical);
    ui->cmbGraphType->addItem(QIcon(":/table/icons/plot_himpulses.png"), tr("horizontal impulses"), QFRDRTable::gtImpulsesHorizontal);
    ui->cmbGraphType->addItem(QIcon(":/table/icons/plot_xfilledcurve.png"), tr("illed curve X"), QFRDRTable::gtFilledCurveX);
    ui->cmbGraphType->addItem(QIcon(":/table/icons/plot_yfilledcurve.png"), tr("filles curve Y"), QFRDRTable::gtFilledCurveY);
    ui->cmbGraphType->addItem(QIcon(":/table/icons/plot_vsteps.png"), tr("vertical steps"), QFRDRTable::gtStepsVertical);
    ui->cmbGraphType->addItem(QIcon(":/table/icons/plot_hsteps.png"), tr("horizontal steps"), QFRDRTable::gtStepsHorizontal);
    ui->cmbGraphType->addItem(QIcon(":/table/icons/plot_vbars.png"), tr("vertical bars"), QFRDRTable::gtBarsVertical);
    ui->cmbGraphType->addItem(QIcon(":/table/icons/plot_hbars.png"), tr("horizontal bars"), QFRDRTable::gtBarsHorizontal);
    ui->cmbGraphType->addItem(QIcon(":/table/icons/plot_boxplotsx.png"), tr("boxplot X"), QFRDRTable::gtBoxplotX);
    ui->cmbGraphType->addItem(QIcon(":/table/icons/plot_boxplotsy.png"), tr("boxplot Y"), QFRDRTable::gtBoxplotY);
    ui->cmbGraphType->addItem(QIcon(":/table/icons/plot_image.png"), tr("image"), QFRDRTable::gtImage);
    ui->cmbGraphType->addItem(QIcon(":/table/icons/plot_rgbimage.png"), tr("RGB image"), QFRDRTable::gtRGBImage);
    ui->cmbGraphType->addItem(QIcon(":/table/icons/plot_maskimage.png"), tr("mask image"), QFRDRTable::gtMaskImage);
    ui->cmbGraphType->addItem(QIcon(":/table/icons/plot_function.png"), tr("function (lines)"), QFRDRTable::gtFunction);
    ui->cmbGraphType->addItem(QIcon(":/table/icons/plot_hrange.png"), tr("horizontal range"), QFRDRTable::gtHorizontalRange);
    ui->cmbGraphType->addItem(QIcon(":/table/icons/plot_vrange.png"), tr("vertical range"), QFRDRTable::gtVerticalRange);



    current=NULL;
    this->plot=-1;
    updating=false;
    if (ProgramOptions::getInstance() && ProgramOptions::getInstance()->getQSettings()) readSettings(*(ProgramOptions::getInstance()->getQSettings()), "table/QFRDRTablePlotWidget/");
    updatePlotWidgetVisibility();
}

QFTableGraphSettings::~QFTableGraphSettings()
{
    updating=true;
    if (ProgramOptions::getInstance() && ProgramOptions::getInstance()->getQSettings()) writeSettings(*(ProgramOptions::getInstance()->getQSettings()), "table/QFRDRTablePlotWidget/");
    delete ui;
}

void QFTableGraphSettings::setRecord(QFRDRTable *record, int plot)
{
    if (current) {
        disconnect(current->model(), SIGNAL(modelReset()), this, SLOT(updateComboboxes()));
        disconnect(current->model(), SIGNAL(layoutChanged()), this, SLOT(updateComboboxes()));
        disconnect(current->model(), SIGNAL(headerDataChanged(Qt::Orientation,int,int)), this, SLOT(updateComboboxes()));
    }
    current=record;
    this->plot=plot;
    updating=true;
    disconnectWidgets();


    headerModel->setHasNone(true);
    headerModel->setModel(current->model());

    /*ui->cmbLinesXData->setModel(headerModel);
    ui->cmbLinesXError->setModel(headerModel);
    ui->cmbLinesYData->setModel(headerModel);
    ui->cmbLinesYError->setModel(headerModel);
    ui->cmbLinesMax->setModel(headerModel);
    ui->cmbLinesMean->setModel(headerModel);
    ui->cmbLinesQ75->setModel(headerModel);*/
    if (current) {
        connect(current->model(), SIGNAL(modelReset()), this, SLOT(updateComboboxes()));
        connect(current->model(), SIGNAL(layoutChanged()), this, SLOT(updateComboboxes()));
        connect(current->model(), SIGNAL(headerDataChanged(Qt::Orientation,int,int)), this, SLOT(updateComboboxes()));
    }
    updateComboboxes();
    updating=false;
    connectWidgets();

    //updateGraph();
}

void QFTableGraphSettings::updateComboboxes()
{
    reloadColumns(ui->cmbLinesXData);
    reloadColumns(ui->cmbLinesXError);
    reloadColumns(ui->cmbLinesYData);
    reloadColumns(ui->cmbLinesYError);
    reloadColumns(ui->cmbLinesMax);
    reloadColumns(ui->cmbLinesMean);
    reloadColumns(ui->cmbLinesQ75);
}

void QFTableGraphSettings::fitFunctionChanged()
{
    cmbFunctionTypeCurrentIndexChanged(ui->cmbFunctionType->currentIndex());
}

void QFTableGraphSettings::rawDataChanged()
{
    if (updating) return;
    //disconnectWidgets();
    /*reloadColumns(ui->cmbLinesXData);
    reloadColumns(ui->cmbLinesXError);
    reloadColumns(ui->cmbLinesYData);
    reloadColumns(ui->cmbLinesYError);*/

    //connectWidgets();
    //updateGraph();
}

void QFTableGraphSettings::readSettings(QSettings &settings, const QString &prefix)
{
}

void QFTableGraphSettings::writeSettings(QSettings &settings, const QString &prefix)
{
}

QIcon QFTableGraphSettings::getGraphIcon(int i) const
{
    return ui->cmbGraphType->itemIcon(i);
}



void QFTableGraphSettings::writeGraphData()
{
    emit graphDataChanged();
}

void QFTableGraphSettings::reloadColumns(QComboBox *combo)
{
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

void QFTableGraphSettings::writeGraphData(QFRDRTable::GraphInfo& graph)
{
    //qDebug()<<"QFTableGraphSettings::graphDataChanged    updating="<<updating;
    if (updating) return;
    if (current) {
        updating=true;
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;

        graph.title=ui->edtGraphTitle->text();

        graph.type=(QFRDRTable::GraphType)ui->cmbGraphType->itemData(ui->cmbGraphType->currentIndex()).toInt();

        updatePlotWidgetVisibility();



        graph.xcolumn=qMax(-1, ui->cmbLinesXData->currentIndex()-1);
        graph.xerrorcolumn=qMax(-1, ui->cmbLinesXError->currentIndex()-1);
        graph.ycolumn=qMax(-1, ui->cmbLinesYData->currentIndex()-1);
        graph.yerrorcolumn=qMax(-1, ui->cmbLinesYError->currentIndex()-1);
        graph.meancolumn=qMax(-1, ui->cmbLinesMean->currentIndex()-1);
        graph.maxcolumn=qMax(-1, ui->cmbLinesMax->currentIndex()-1);
        graph.q75column=qMax(-1, ui->cmbLinesQ75->currentIndex()-1);

        graph.width=ui->edtWidth->value()/100.0;
        graph.shift=ui->edtShift->value()/100.0;
        QColor oldColor=graph.color;
        QColor oldRangeColor=graph.rangeCenterColor;
        QColor oldDefaultErrorColor=oldColor.darker();
        QColor oldDefaultFillColor=oldColor.lighter();
        graph.color=ui->cmbLineColor->currentColor();

        graph.centerColorAuto=ui->chkCenterColAuto->isChecked();
        graph.fillColorAuto=ui->chkFillColAuto->isChecked();
        graph.errorColorAuto=ui->chkErrorColAuto->isChecked();




        if (!ui->chkErrorColAuto->isChecked()) {
            graph.errorColor=ui->cmbErrorColor->currentColor();
        } else {
            graph.errorColor=graph.color.darker();
            ui->cmbErrorColor->setCurrentColor(graph.errorColor);
        }

        if (!ui->chkFillColAuto->isChecked()) {
            graph.fillColor=ui->cmbFillColor->currentColor();
        } else {
            graph.fillColor=graph.color.lighter();
            ui->cmbFillColor->setCurrentColor(graph.fillColor);
        }

        if (!ui->chkCenterColAuto->isChecked()) {
            graph.rangeCenterColor=ui->cmbRangeCenterColor->currentColor();
        } else {
            graph.rangeCenterColor=graph.color.lighter();
            ui->cmbRangeCenterColor->setCurrentColor(graph.rangeCenterColor);
        }
        graph.errorStyle=ui->cmbErrorStyle->getErrorStyle();
        graph.symbol=ui->cmbSymbol->getSymbol();
        graph.style=ui->cmbLineStyle->currentLineStyle();
        graph.fillStyle=ui->cmbFillStyle->currentFillStyle();
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
        graph.imageLegendG=ui->edtColorbarLabelG->text();
        graph.imageLegendB=ui->edtColorbarLabelB->text();
        graph.imageLegendMod=ui->edtColorbarLabelMod->text();



        graph.imageColorbarTicklength=ui->spinColorbarTickLength->value();
        graph.imageColorbarFontsize=ui->spinColorbarFontsize->value();
        graph.imageTicks=ui->spinColorbarTicks->value();
        graph.imageModTicks=ui->spinColorbarModTicks->value();
        graph.imageColorbarLabelType=ui->cmbColorbarLabelType->getLabelType();
        graph.imageColorbarLabelDigits=ui->spinColorbarLabelDigits->value();

        graph.colorbarWidth=ui->spinColorbarWidth->value();
        graph.colorbarRelativeHeight=ui->spinColorbarHeight->value()/100.0;
        graph.function=ui->edtFunction->text();
        graph.isStrided=ui->chkSTrided->isChecked();
        graph.stride=ui->spinStride->value();
        graph.strideStart=ui->spinStrideStart->value();
        graph.modifierMode=(JKQTPMathImage::ModifierMode)ui->cmbModifierMode->currentIndex();

        graph.functionType=(QFRDRTable::GTFunctionType)ui->cmbFunctionType->currentIndex();
        if (graph.functionType==QFRDRTable::gtfQFFunction) {
            graph.function=ui->cmbQFFitFunction->currentFitFunctionID();
        }
        graph.functionParameters=fitfuncValues;


        graph.rangeCenter=ui->edtRangeCenter->value();
        graph.rangeStart=ui->edtRangeStart->value();
        graph.rangeEnd=ui->edtRangeEnd->value();
        graph.rangeCenterWidth=ui->spinRangeCenterWidth->value();
        graph.rangeCenterColor=ui->cmbRangeCenterColor->currentColor();
        graph.rangeCenterColorTransparent=double(ui->sliderRangeCenterTransparency->value())/255.0;
        graph.rangeCenterStyle=ui->cmbRangeCenterStyle->currentLineStyle();
        graph.rangeFill=ui->chkRangeFillRange->isChecked();
        graph.rangeInverted=ui->chkRangeInverted->isChecked();
        graph.rangeDrawCenter=ui->chkRangeDrawCenter->isChecked();


        graph.errorLineStyle=ui->cmbErrorLineStyle->currentLineStyle();
        graph.errorBarSize=ui->spinErrorBarWidth->value();
        graph.errorWidth=ui->spinErrorLineWidth->value();



        updating=false;
    }
}

void QFTableGraphSettings::loadGraphData(const QFRDRTable::GraphInfo &graph)
{
    updating=true;
    ui->edtGraphTitle->setText(graph.title);

    QString fit_type=graph.moreProperties.value("FIT_TYPE", "NONE").toString().toUpper().trimmed();
    isFitResult= ((fit_type=="LEAST_SQUARES")||(fit_type=="FIT"));

    ui->cmbLinesXData->setCurrentIndex(graph.xcolumn+1);
    ui->cmbLinesXError->setCurrentIndex(graph.xerrorcolumn+1);
    ui->cmbLinesYData->setCurrentIndex(graph.ycolumn+1);
    ui->cmbLinesYError->setCurrentIndex(graph.yerrorcolumn+1);
    ui->cmbLinesMax->setCurrentIndex(graph.maxcolumn+1);
    ui->cmbLinesMean->setCurrentIndex(graph.meancolumn+1);
    ui->cmbLinesQ75->setCurrentIndex(graph.q75column+1);
    ui->cmbGraphType->setCurrentIndex(ui->cmbGraphType->findData((int)graph.type));

    ui->edtWidth->setValue(graph.width*100.0);
    ui->edtShift->setValue(graph.shift*100.0);


    ui->cmbErrorColor->setCurrentColor(graph.errorColor);
    ui->cmbLineColor->setCurrentColor(graph.color);
    ui->cmbFillColor->setCurrentColor(graph.fillColor);
    ui->cmbErrorStyle->setSymbol(graph.errorStyle);
    ui->cmbSymbol->setSymbol(graph.symbol);
    ui->cmbLineStyle->setCurrentLineStyle(graph.style);
    ui->cmbFillStyle->setCurrentFillStyle(graph.fillStyle);
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

    ui->cmbColorbarLabelType->setLabelType(graph.imageColorbarLabelType);
    ui->spinColorbarLabelDigits->setValue(graph.imageColorbarLabelDigits);

    ui->spinColorbarTickLength->setValue(graph.imageColorbarTicklength);
    ui->spinColorbarFontsize->setValue(graph.imageColorbarFontsize);
    ui->spinColorbarTicks->setValue(graph.imageTicks);
    ui->spinColorbarModTicks->setValue(graph.imageModTicks);

    ui->chkImageColorbarRight->setChecked(graph.imageColorbarRight);
    ui->chkImageColorbarTop->setChecked(graph.imageColorbarTop);
    ui->edtColorbarLabel->setText(graph.imageLegend);
    ui->edtColorbarLabelG->setText(graph.imageLegendG);
    ui->edtColorbarLabelB->setText(graph.imageLegendB);
    ui->edtColorbarLabelMod->setText(graph.imageLegendMod);
    ui->spinColorbarWidth->setValue(graph.colorbarWidth);
    ui->spinColorbarHeight->setValue(graph.colorbarRelativeHeight*100.0);
    ui->edtFunction->setText(graph.function);
    ui->chkSTrided->setChecked(graph.isStrided);
    ui->spinStride->setValue(graph.stride);
    ui->spinStrideStart->setValue(graph.strideStart);
    ui->cmbModifierMode->setCurrentIndex(graph.modifierMode);

    ui->cmbFunctionType->setCurrentIndex((int)graph.functionType);
    if (graph.functionType==QFRDRTable::gtfQFFunction) {
        ui->cmbQFFitFunction->setCurrentFitFunction(graph.function);
    }
    fitfuncValues=graph.functionParameters;
    ui->tabFunctionParameters->resizeColumnsToContents();
    ui->tabFunctionParameters->resizeRowsToContents();


    ui->edtRangeCenter->setValue(graph.rangeCenter);
    ui->edtRangeStart->setValue(graph.rangeStart);
    ui->edtRangeEnd->setValue(graph.rangeEnd);
    ui->spinRangeCenterWidth->setValue(graph.rangeCenterWidth);
    ui->cmbRangeCenterColor->setCurrentColor(graph.rangeCenterColor);
    ui->sliderRangeCenterTransparency->setValue(graph.rangeCenterColorTransparent*255.0);
    ui->cmbRangeCenterStyle->setCurrentLineStyle(graph.rangeCenterStyle);
    ui->chkRangeFillRange->setChecked(graph.rangeFill);
    ui->chkRangeInverted->setChecked(graph.rangeInverted);
    ui->chkRangeDrawCenter->setChecked(graph.rangeDrawCenter);

    ui->cmbErrorLineStyle->setCurrentLineStyle(graph.errorLineStyle);
    ui->spinErrorBarWidth->setValue(graph.errorBarSize);
    ui->spinErrorLineWidth->setValue(graph.errorWidth);

    ui->chkCenterColAuto->setChecked(graph.centerColorAuto);
    ui->chkFillColAuto->setChecked(graph.fillColorAuto);
    ui->chkErrorColAuto->setChecked(graph.errorColorAuto);


    updating=false;
}

void QFTableGraphSettings::updatePlotWidgetVisibility()
{
    if (current) {
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;

        bool updt=updatesEnabled();
        setUpdatesEnabled(false);

        this->setVisible(true);

        ui->widErrorStyle->setVisible(true);
        ui->widFillColor->setVisible(true);
        ui->widImage->setVisible(true);
        ui->widLineStyle->setVisible(true);
        ui->widSymbol->setVisible(true);
        ui->labErrorStyle->setVisible(true);
        ui->labDataX->setVisible(true);
        ui->labDataY->setVisible(true);
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

        ui->labDataX->setText(tr("X Data Column:"));
        ui->labDataY->setText(tr("Y Data Column:"));
        ui->labErrorX->setText(tr("X Error Column:"));
        ui->labErrorY->setText(tr("Y Error Column:"));
        ui->labMax->setVisible(false);
        ui->labMean->setVisible(false);
        ui->labQ75->setVisible(false);
        ui->labMax->setText(tr("Maximum Column:"));
        ui->labMean->setText(tr("Mean Column:"));
        ui->labQ75->setText(tr("75% Quantile Column:"));
        ui->labImage->setText(tr("image map:"));

        ui->cmbLinesMax->setVisible(false);
        ui->cmbLinesMean->setVisible(false);
        ui->cmbLinesQ75->setVisible(false);

        ui->labTransparencyFalse->setVisible(false);
        ui->widFalseTransparency->setVisible(false);
        ui->labTransparencyTrue->setVisible(false);
        ui->widTrueTransparency->setVisible(false);
        ui->labTrueColor->setVisible(false);
        ui->cmbImageTrueColor->setVisible(false);
        ui->labFalseColor->setVisible(false);
        ui->cmbImageFalseColor->setVisible(false);
        ui->labColormap->setVisible(false);
        ui->cmbColormap->setVisible(false);
        ui->labColorbarLabel->setVisible(false);
        ui->edtColorbarLabel->setVisible(false);
        ui->labModifierMode->setVisible(false);
        ui->cmbModifierMode->setVisible(false);
        ui->labColorbarLabelMod->setVisible(false);
        ui->edtColorbarLabelMod->setVisible(false);
        ui->labColorbarLabelG->setVisible(false);
        ui->edtColorbarLabelG->setVisible(false);
        ui->labColorbarLabelB->setVisible(false);
        ui->edtColorbarLabelB->setVisible(false);
        ui->labColorbarLabel->setText(tr("bar label:"));
        ui->btnFit->setVisible(true);
        ui->labImageHeight->setVisible(true);
        ui->labImageWidth->setVisible(true);
        ui->labImageWidthPixels->setVisible(true);
        ui->spinImageWidth->setVisible(true);
        ui->edtImageWidth->setVisible(true);
        ui->edtImageHeight->setVisible(true);
        ui->edtImageX->setVisible(true);
        ui->edtImageY->setVisible(true);
        ui->labImageX->setVisible(true);
        ui->labImageY->setVisible(true);
        ui->btnAutoImageSizes->setVisible(true);

        ui->labQFFitFuction->setVisible(false);
        ui->labFuctionType->setVisible(false);
        ui->labFuctionParameters->setVisible(false);
        ui->cmbFunctionType->setVisible(false);
        ui->cmbQFFitFunction->setVisible(false);
        ui->tabFunctionParameters->setVisible(false);
        ui->labWidth->setVisible(false);
        ui->labShift->setVisible(false);
        ui->edtWidth->setVisible(false);
        ui->edtShift->setVisible(false);
        ui->widWidth->setVisible(false);
        ui->labWidth->setText(tr("Width:"));

        ui->labRangeData->setVisible(false);
        ui->labRangeStyle->setVisible(false);
        ui->widRangeData->setVisible(false);
        ui->widRangeStyle->setVisible(false);
        ui->btnRefit->setVisible(false);

        switch(ui->cmbGraphType->itemData(ui->cmbGraphType->currentIndex()).toInt()) {


            case QFRDRTable::gtImpulsesVertical:
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
            case QFRDRTable::gtImpulsesHorizontal:
                //graph.type=QFRDRTable::gtImpulsesHorizontal;
                ui->cmbLinesXError->setVisible(false);
                ui->labErrorX->setVisible(false);
                ui->chkDrawLine->setVisible(false);
                ui->cmbLineStyle->setVisible(false);
                ui->labImage->setVisible(false);
                ui->widImage->setVisible(false);
                ui->btnFit->setVisible(false);

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
            case QFRDRTable::gtFilledCurveX:
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
            case QFRDRTable::gtFilledCurveY:
                //graph.type=QFRDRTable::gtFilledCurveY;
                ui->labImage->setVisible(false);
                ui->widImage->setVisible(false);
                ui->cmbLinesXError->setVisible(false);
                ui->labErrorX->setVisible(false);
                ui->labSymbol->setVisible(false);
                ui->widSymbol->setVisible(false);
                ui->btnFit->setVisible(false);

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
            case QFRDRTable::gtStepsVertical:
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
            case QFRDRTable::gtStepsHorizontal:
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
                ui->btnFit->setVisible(false);
/*                    ui->cmbErrorColor->setVisible(false);
                ui->cmbErrorStyle->setVisible(false);
                ui->cmbLinesXError->setVisible(false);
                ui->cmbLinesYError->setVisible(false);
                ui->cmbSymbol->setVisible(false);
                ui->spinSymbolSize->setVisible(false);
                ui->sliderErrorTransparent->setVisible(false);*/
                break;
            case QFRDRTable::gtBarsVertical:
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
                ui->labWidth->setVisible(true);
                ui->labShift->setVisible(true);
                ui->edtWidth->setVisible(true);
                ui->edtShift->setVisible(true);
                ui->widWidth->setVisible(true);

/*                    ui->cmbErrorColor->setVisible(false);
                ui->cmbErrorStyle->setVisible(false);
                ui->cmbLinesXError->setVisible(false);
                ui->cmbLinesYError->setVisible(false);
                ui->chkDrawLine->setVisible(false);
                ui->cmbSymbol->setVisible(false);
                ui->spinSymbolSize->setVisible(false);
                ui->sliderErrorTransparent->setVisible(false);*/
                break;
            case QFRDRTable::gtBarsHorizontal:
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
                ui->btnFit->setVisible(false);
                ui->labWidth->setVisible(true);
                ui->labShift->setVisible(true);
                ui->edtWidth->setVisible(true);
                ui->edtShift->setVisible(true);
                ui->widWidth->setVisible(true);
                break;
            case QFRDRTable::gtParametrizedScatter:
                ui->labImage->setVisible(true);
                ui->labImage->setText(tr("colorbar:"));
                ui->widImage->setVisible(true);
                ui->labMean->setVisible(true);
                ui->labQ75->setVisible(true);
                ui->labMean->setText(tr("Color Column:"));
                ui->labQ75->setText(tr("Size Column:"));
                ui->cmbLinesMean->setVisible(true);
                ui->cmbLinesQ75->setVisible(true);

                ui->labTransparencyFalse->setVisible(false);
                ui->widFalseTransparency->setVisible(false);
                ui->labTransparencyTrue->setVisible(false);
                ui->widTrueTransparency->setVisible(false);
                ui->labTrueColor->setVisible(false);
                ui->cmbImageTrueColor->setVisible(false);
                ui->labFalseColor->setVisible(false);
                ui->cmbImageFalseColor->setVisible(false);
                ui->labColormap->setVisible(true);
                ui->cmbColormap->setVisible(true);
                ui->labColorbarLabel->setVisible(true);
                ui->edtColorbarLabel->setVisible(true);
                ui->labModifierMode->setVisible(false);
                ui->cmbModifierMode->setVisible(false);
                ui->labColorbarLabelMod->setVisible(false);
                ui->edtColorbarLabelMod->setVisible(false);

                ui->labImageHeight->setVisible(false);
                ui->labImageWidth->setVisible(false);
                ui->labImageWidthPixels->setVisible(false);
                ui->spinImageWidth->setVisible(false);
                ui->edtImageWidth->setVisible(false);
                ui->edtImageHeight->setVisible(false);
                ui->edtImageX->setVisible(false);
                ui->edtImageY->setVisible(false);
                ui->labImageX->setVisible(false);
                ui->labImageY->setVisible(false);
                ui->btnAutoImageSizes->setVisible(false);



                break;
            case QFRDRTable::gtBoxplotX:
            case QFRDRTable::gtBoxplotY:
                //graph.type=QFRDRTable::gtBoxplotX, gtBoxplotY;
                ui->labMax->setVisible(true);
                ui->labMean->setVisible(true);
                ui->labQ75->setVisible(true);
                ui->labDataX->setText(tr("position Column:"));
                ui->labErrorX->setText(tr("Minimum Column:"));
                ui->labDataY->setText(tr("25% Quantile Column:"));
                ui->labErrorY->setText(tr("Median Column:"));
                ui->labMax->setText(tr("Maximum Column:"));
                ui->labMean->setText(tr("Mean Column:"));
                ui->labQ75->setText(tr("75% Quantile Column:"));
                ui->cmbLinesMax->setVisible(true);
                ui->cmbLinesMean->setVisible(true);
                ui->cmbLinesQ75->setVisible(true);
                ui->labImage->setVisible(false);
                ui->widImage->setVisible(false);
                ui->labSymbol->setVisible(true);
                ui->widSymbol->setVisible(true);
                ui->widErrorStyle->setVisible(false);
                ui->labErrorStyle->setVisible(false);
                ui->chkDrawLine->setVisible(false);
                ui->btnFit->setVisible(false);
                ui->labWidth->setVisible(true);
                ui->labWidth->setText(tr("Boxplot Width:"));
                ui->labShift->setVisible(false);
                ui->edtWidth->setVisible(true);
                ui->edtShift->setVisible(false);
                ui->widWidth->setVisible(true);
                break;
            case QFRDRTable::gtImage:
                //graph.type=QFRDRTable::gtImage;
                ui->btnFit->setVisible(false);
                ui->cmbLinesXError->setVisible(false);
                ui->labErrorX->setVisible(false);
                ui->cmbLinesYData->setVisible(true);
                ui->labDataY->setVisible(true);
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
                ui->labDataX->setText(tr("Image Column:"));
                ui->labDataY->setText(tr("Modifier Column:"));

                ui->labTransparencyFalse->setVisible(false);
                ui->widFalseTransparency->setVisible(false);
                ui->labTransparencyTrue->setVisible(false);
                ui->widTrueTransparency->setVisible(false);
                ui->labTrueColor->setVisible(false);
                ui->cmbImageTrueColor->setVisible(false);
                ui->labFalseColor->setVisible(false);
                ui->cmbImageFalseColor->setVisible(false);
                ui->labColormap->setVisible(true);
                ui->cmbColormap->setVisible(true);
                ui->labColorbarLabel->setVisible(true);
                ui->edtColorbarLabel->setVisible(true);
                ui->labModifierMode->setVisible(true);
                ui->cmbModifierMode->setVisible(true);
                ui->labColorbarLabelMod->setVisible(true);
                ui->edtColorbarLabelMod->setVisible(true);

                break;
            case QFRDRTable::gtRGBImage:
                //graph.type=QFRDRTable::gtRGBImage;
                ui->btnFit->setVisible(false);
                ui->cmbLinesXError->setVisible(true);
                ui->labErrorX->setVisible(true);
                ui->cmbLinesYData->setVisible(true);
                ui->labDataY->setVisible(true);
                ui->cmbLinesYError->setVisible(true);
                ui->labErrorY->setVisible(true);
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
                ui->labDataX->setText(tr("R Column:"));
                ui->labErrorX->setText(tr("G Column:"));
                ui->labDataY->setText(tr("B Column:"));
                ui->labErrorY->setText(tr("Modifier Column:"));

                ui->labTransparencyFalse->setVisible(false);
                ui->widFalseTransparency->setVisible(false);
                ui->labTransparencyTrue->setVisible(false);
                ui->widTrueTransparency->setVisible(false);
                ui->labTrueColor->setVisible(false);
                ui->cmbImageTrueColor->setVisible(false);
                ui->labFalseColor->setVisible(false);
                ui->cmbImageFalseColor->setVisible(false);
                ui->labColormap->setVisible(false);
                ui->cmbColormap->setVisible(false);
                ui->labColorbarLabel->setVisible(true);
                ui->edtColorbarLabel->setVisible(true);
                ui->labModifierMode->setVisible(true);
                ui->cmbModifierMode->setVisible(true);
                ui->labColorbarLabelMod->setVisible(false);
                ui->edtColorbarLabelMod->setVisible(false);
                ui->labColorbarLabelG->setVisible(true);
                ui->edtColorbarLabelG->setVisible(true);
                ui->labColorbarLabelB->setVisible(true);
                ui->edtColorbarLabelB->setVisible(true);
                ui->labColorbarLabel->setText(tr("R bar label:"));

                break;
            case QFRDRTable::gtMaskImage:
                //graph.type=QFRDRTable::gtMaskImage;
                ui->btnFit->setVisible(false);
                ui->labDataX->setText(tr("Mask Column:"));
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
                ui->cmbModifierMode->setVisible(false);
                ui->labModifierMode->setVisible(false);

                ui->labTransparencyFalse->setVisible(true);
                ui->widFalseTransparency->setVisible(true);
                ui->labTransparencyTrue->setVisible(true);
                ui->widTrueTransparency->setVisible(true);
                ui->labTrueColor->setVisible(true);
                ui->cmbImageTrueColor->setVisible(true);
                ui->labFalseColor->setVisible(true);
                ui->cmbImageFalseColor->setVisible(true);
                ui->labColormap->setVisible(false);
                ui->cmbColormap->setVisible(false);
                ui->labColorbarLabel->setVisible(false);
                ui->edtColorbarLabel->setVisible(false);
                ui->labModifierMode->setVisible(false);
                ui->cmbModifierMode->setVisible(false);
                ui->labColorbarLabelMod->setVisible(false);
                ui->edtColorbarLabelMod->setVisible(false);

                break;
            case QFRDRTable::gtFunction:
                //graph.type=QFRDRTable::gtFunction;
                ui->btnFit->setVisible(false);
                ui->labDataY->setText(tr("function parameter column"));
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
                ui->labSymbol->setVisible(false);
                ui->widSymbol->setVisible(false);
                ui->chkSTrided->setVisible(false);
                ui->widStride->setVisible(false);

                ui->btnRefit->setVisible(isFitResult);

                ui->labFuctionType->setVisible(true);
                ui->cmbFunctionType->setVisible(true);

                if (ui->cmbFunctionType->currentIndex()==0) {
                    ui->widFunction->setVisible(true);
                    ui->labFuction->setVisible(true);
                } else if (ui->cmbFunctionType->currentIndex()==ui->cmbFunctionType->count()-1) {
                    ui->labQFFitFuction->setVisible(true);
                    ui->cmbQFFitFunction->setVisible(true);
                    if (ui->cmbLinesYData->currentIndex()==0) {
                        ui->labFuctionParameters->setVisible(true);
                        ui->tabFunctionParameters->setVisible(true);
                    }
                } else {
                    if (ui->cmbLinesYData->currentIndex()==0) {
                        ui->labFuctionParameters->setVisible(true);
                        ui->tabFunctionParameters->setVisible(true);
                    }
                }
                break;

            case QFRDRTable::gtHorizontalRange: case QFRDRTable::gtVerticalRange: // horicontal/vertical range
                ui->btnFit->setVisible(false);
                ui->labRangeData->setVisible(true);
                ui->labRangeStyle->setVisible(true);
                ui->widRangeData->setVisible(true);
                ui->widRangeStyle->setVisible(true);

                ui->cmbLinesXError->setVisible(false);
                ui->labErrorX->setVisible(false);
                ui->labSymbol->setVisible(false);
                ui->widSymbol->setVisible(false);
                ui->labImage->setVisible(false);
                ui->widImage->setVisible(false);

                ui->widErrorStyle->setVisible(false);
                ui->widFillColor->setVisible(true);
                ui->widImage->setVisible(false);
                ui->widSymbol->setVisible(false);
                ui->labErrorStyle->setVisible(false);
                ui->labDataX->setVisible(false);
                ui->labDataY->setVisible(false);
                ui->labErrorX->setVisible(false);
                ui->labErrorY->setVisible(false);
                ui->labImage->setVisible(false);
                ui->labSymbol->setVisible(false);
                ui->labTitle->setVisible(true);
                ui->labType->setVisible(true);
                ui->chkDrawLine->setVisible(true);
                ui->cmbLineStyle->setVisible(true);
                ui->cmbLinesXData->setVisible(false);
                ui->cmbLinesXError->setVisible(false);
                ui->cmbLinesYData->setVisible(false);
                ui->cmbLinesYError->setVisible(false);
                ui->widFunction->setVisible(false);
                ui->labFuction->setVisible(false);
                ui->chkSTrided->setVisible(false);
                ui->widStride->setVisible(false);



                break;

            case QFRDRTable::gtLines:
            default:
                //graph.type=QFRDRTable::gtLines;
                ui->labImage->setVisible(false);
                ui->widImage->setVisible(false);
                break;
        }

        setUpdatesEnabled(updt);

    }
}

void QFTableGraphSettings::connectWidgets()
{
    //qDebug()<<"connectWidgets";


    connect(ui->edtFunction, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->edtGraphTitle, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->cmbGraphType, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbLinesXData, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbLinesXError, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbLinesYData, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbLinesYError, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));

    connect(ui->cmbLinesMax, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbLinesMean, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbLinesYError, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbLinesQ75, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->edtWidth, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->edtShift, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));

    connect(ui->cmbFillColor, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbErrorColor, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbLineColor, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbLineStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbFillStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbSymbol, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->chkDrawLine, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    connect(ui->spinLineWidth, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->spinSymbolSize, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->sliderErrorTransparent, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->sliderFillTransparent, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->sliderPlotTransparent, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbImageFalseColor, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbImageTrueColor, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbColormap, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbModifierMode, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->spinImageWidth, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->edtImageHeight, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->edtImageMax, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->edtImageMin, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->edtImageHeight, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->edtImageX, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->edtImageY, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->sliderImageFalseColor, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->sliderImageTrueColor, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->edtColorbarLabel, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->edtColorbarLabelB, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->edtColorbarLabelG, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->spinColorbarWidth, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->spinColorbarHeight, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->chkImageColorbarRight, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    connect(ui->chkImageColorbarTop, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    connect(ui->chkImageAutorange, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    connect(ui->chkSTrided, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    connect(ui->spinStride, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->spinStrideStart, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->cmbFunctionType, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbFunctionTypeCurrentIndexChanged(int)));
    connect(ui->cmbQFFitFunction, SIGNAL(currentIndexChanged(int)), this, SLOT(fitFunctionChanged()));


    connect(ui->cmbRangeCenterColor, SIGNAL(currentIndexChanged(int)), this, SLOT(fitFunctionChanged()));
    connect(ui->cmbRangeCenterStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(fitFunctionChanged()));
    connect(ui->chkRangeDrawCenter, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    connect(ui->chkRangeFillRange, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    connect(ui->chkRangeInverted, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    connect(ui->spinRangeCenterWidth, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->edtRangeCenter, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->edtRangeStart, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->edtRangeEnd, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->sliderRangeCenterTransparency, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    connect(fitfuncValuesTable, SIGNAL(fitParamChanged()), this, SLOT(writeGraphData()));

    connect(ui->spinColorbarModTicks, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->spinColorbarTicks, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->spinColorbarTickLength, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->spinColorbarFontsize, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));

    connect(ui->spinErrorLineWidth, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->spinErrorBarWidth, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->cmbErrorLineStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));


    connect(ui->cmbColorbarLabelType, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->spinColorbarLabelDigits, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
}

void QFTableGraphSettings::disconnectWidgets()
{
    //qDebug()<<"disconnectWidgets";

    disconnect(ui->edtWidth, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->edtShift, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));


    disconnect(ui->cmbLinesQ75, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->edtFunction, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->edtGraphTitle, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->cmbGraphType, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbLinesXData, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbLinesXError, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbLinesYData, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbLinesYError, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbLinesMax, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbLinesMean, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbLinesYError, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbErrorColor, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbFillColor, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbLineColor, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbLineStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbFillStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbSymbol, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbModifierMode, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->chkDrawLine, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    disconnect(ui->spinLineWidth, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->spinSymbolSize, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->sliderErrorTransparent, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->sliderFillTransparent, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->sliderPlotTransparent, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbImageFalseColor, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbImageTrueColor, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbColormap, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->spinImageWidth, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->edtImageHeight, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->edtImageMax, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->edtImageMin, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->edtImageHeight, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->edtImageX, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->edtImageY, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->sliderImageFalseColor, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->sliderImageTrueColor, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->edtColorbarLabel, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->edtColorbarLabelB, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->edtColorbarLabelG, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->spinColorbarWidth, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->spinColorbarHeight, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->chkImageColorbarRight, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    disconnect(ui->chkImageColorbarTop, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    disconnect(ui->chkImageAutorange, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    disconnect(ui->chkSTrided, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    disconnect(ui->spinStride, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->spinStrideStart, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));

    disconnect(ui->cmbFunctionType, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbFunctionTypeCurrentIndexChanged(int)));
    disconnect(ui->cmbQFFitFunction, SIGNAL(currentIndexChanged(int)), this, SLOT(fitFunctionChanged()));
    disconnect(ui->cmbRangeCenterColor, SIGNAL(currentIndexChanged(int)), this, SLOT(fitFunctionChanged()));
    disconnect(ui->cmbRangeCenterStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(fitFunctionChanged()));
    disconnect(ui->chkRangeDrawCenter, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    disconnect(ui->chkRangeFillRange, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    disconnect(ui->chkRangeInverted, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    disconnect(ui->spinRangeCenterWidth, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->edtRangeCenter, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->edtRangeStart, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->edtRangeEnd, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->sliderRangeCenterTransparency, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    disconnect(fitfuncValuesTable, SIGNAL(fitParamChanged()), this, SLOT(writeGraphData()));
    disconnect(ui->spinColorbarModTicks, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->spinColorbarTicks, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->spinColorbarTickLength, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->spinColorbarFontsize, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));

    disconnect(ui->cmbColorbarLabelType, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->spinColorbarLabelDigits, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));

    disconnect(ui->spinErrorLineWidth, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->spinErrorBarWidth, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->cmbErrorLineStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
}

void QFTableGraphSettings::doFit()
{
    emit performFit(ui->cmbLinesXData->currentIndex()-1, ui->cmbLinesYData->currentIndex()-1, ui->cmbLinesYError->currentIndex()-1, plot, QString(""));
}

void QFTableGraphSettings::doRefit()
{
    emit performRefit(plot);
}

void QFTableGraphSettings::doRegression()
{
    emit performRegression(ui->cmbLinesXData->currentIndex()-1, ui->cmbLinesYData->currentIndex()-1, ui->cmbLinesYError->currentIndex()-1, plot);
}



void QFTableGraphSettings::initFocus()
{
    ui->edtGraphTitle->setFocus();
}

void QFTableGraphSettings::on_edtFunction_textChanged(const QString &text)
{
    ui->labFunctionOK->setText(tr("<font color=\"darkgreen\">function OK</font>"));
    QFMathParser mp; // instanciate
    QFMathParser::qfmpNode* n;
    // parse some numeric expression
    n=mp.parse(text);
    delete n;
    if (mp.hasErrorOccured()) {
        ui->labFunctionOK->setText(tr("<font color=\"red\">ERROR in function:<br>&nbsp;&nbsp;&nbsp;&nbsp;%1</font>").arg(mp.getLastErrors().join("<br>&nbsp;&nbsp;&nbsp;&nbsp;")));
    }
}

void QFTableGraphSettings::cmbFunctionTypeCurrentIndexChanged(int index)
{
    if (ui->cmbFunctionType->currentIndex()==0) {
        fitfuncValuesTable->setWriteTo(&fitfuncValuesDummy, QStringList());
    } else if (ui->cmbFunctionType->currentIndex()==1) {
        int vsize=11;
        if (fitfuncValues.size()>vsize) fitfuncValues.remove(fitfuncValues.size()-(fitfuncValues.size()-vsize), fitfuncValues.size()-vsize);
        while (fitfuncValues.size()<vsize) fitfuncValues.append(0);
        QStringList pn;
        pn<<tr("offset");
        for (int i=1; i<fitfuncValues.size(); i++) {
            pn.append(tr("p%1").arg(i+1));
        }
        fitfuncValuesTable->setWriteTo(&fitfuncValues, pn);
    } else if (ui->cmbFunctionType->currentIndex()==2) {
        int vsize=3;
        if (fitfuncValues.size()>vsize) fitfuncValues.remove(fitfuncValues.size()-(fitfuncValues.size()-vsize), fitfuncValues.size()-vsize);
        while (fitfuncValues.size()<vsize) fitfuncValues.append(0);
        QStringList pn;
        pn<<tr("offset")<<tr("amplitude")<<tr("exponent_div");
        fitfuncValuesTable->setWriteTo(&fitfuncValues, pn);
    } else if (ui->cmbFunctionType->currentIndex()==3) {
        int vsize=3;
        if (fitfuncValues.size()>vsize) {
            //qDebug()<<"deleting:  oldsize="<<fitfuncValues.size()<<"  newsize="<<vsize<<" startdel="<<fitfuncValues.size()-(vsize-fitfuncValues.size())<<" count="<<vsize-fitfuncValues.size();
            fitfuncValues.remove(fitfuncValues.size()-(fitfuncValues.size()-vsize), fitfuncValues.size()-vsize);
        }
        while (fitfuncValues.size()<vsize) fitfuncValues.append(0);
        QStringList pn;
        pn<<tr("offset")<<tr("amplitude")<<tr("exponent");
        fitfuncValuesTable->setWriteTo(&fitfuncValues, pn);
    } else if (ui->cmbFunctionType->currentIndex()==4) {
        QFFitFunction* ff(ui->cmbQFFitFunction->createCurrentInstance(fitfuncValuesTable));
        fitfuncValues.clear();
        if (ff) {
            for (int i=0; i<ff->paramCount(); i++) {
                fitfuncValues<<ff->getDescription(i).initialValue;
            }
        }
        fitfuncValuesTable->setWriteTo(&fitfuncValues, ff, true);
    }
    updatePlotWidgetVisibility();
    writeGraphData();
}

void QFTableGraphSettings::on_btnAutoImageSizes_clicked()
{

    if (ui->cmbGraphType->currentIndex()==11 || ui->cmbGraphType->currentIndex()==12 || ui->cmbGraphType->currentIndex()==13) {
        if (current->model()->hasColumnHeaderData(ui->cmbLinesXData->currentIndex()-1, QFRDRTable::ColumnImagePixWidth)) ui->spinImageWidth->setValue(current->model()->getColumnHeaderData(ui->cmbLinesXData->currentIndex()-1, QFRDRTable::ColumnImagePixWidth).toInt());
        if (current->model()->hasColumnHeaderData(ui->cmbLinesXData->currentIndex()-1, QFRDRTable::ColumnImageWidth)) ui->edtImageWidth->setValue(current->model()->getColumnHeaderData(ui->cmbLinesXData->currentIndex()-1, QFRDRTable::ColumnImageWidth).toDouble());
        if (current->model()->hasColumnHeaderData(ui->cmbLinesXData->currentIndex()-1, QFRDRTable::ColumnImageHeight)) ui->edtImageHeight->setValue(current->model()->getColumnHeaderData(ui->cmbLinesXData->currentIndex()-1, QFRDRTable::ColumnImageHeight).toDouble());
        if (current->model()->hasColumnHeaderData(ui->cmbLinesXData->currentIndex()-1, QFRDRTable::ColumnImageX)) ui->edtImageX->setValue(current->model()->getColumnHeaderData(ui->cmbLinesXData->currentIndex()-1, QFRDRTable::ColumnImageX).toDouble());
        if (current->model()->hasColumnHeaderData(ui->cmbLinesXData->currentIndex()-1, QFRDRTable::ColumnImageY)) ui->edtImageY->setValue(current->model()->getColumnHeaderData(ui->cmbLinesXData->currentIndex()-1, QFRDRTable::ColumnImageY).toDouble());

    }
}

void QFTableGraphSettings::on_btnFunctionHelp_clicked()
{
     if (current)
         QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelpDirectory("table")+"/mathparser.html");
}
