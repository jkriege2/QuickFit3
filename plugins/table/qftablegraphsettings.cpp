#include "qftablegraphsettings.h"
#include "ui_qftablegraphsettings.h"
#include "programoptions.h"
#include "qfrdrtable.h"
#include "qftools.h"
#include "qfdoubleedit.h"
#include "qfmathparserxfunctionlinegraph.h"

QFTableGraphSettings::QFTableGraphSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFTableGraphSettings)
{
    functionRef=new QFFunctionReferenceTool(NULL);
    functionRef->setCompleterFile(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/table/table_expression.txt");
    functionRef->setDefaultWordsMathExpression();


    updating=true;

    ui->setupUi(this);
    functionRef->registerEditor(ui->edtFunction);
    ui->edtImageHeight->setCheckBounds(true, false);
    ui->edtImageHeight->setRange(0, 1e10);
    ui->edtImageMax->setCheckBounds(false, false);
    ui->edtImageMin->setCheckBounds(false, false);
    ui->edtImageWidth->setCheckBounds(true, false);
    ui->edtImageWidth->setRange(0, 1e10);
    ui->edtImageX->setCheckBounds(true, false);
    ui->edtImageY->setCheckBounds(true, false);
    current=NULL;
    this->plot=-1;
    updating=false;
    if (ProgramOptions::getInstance() && ProgramOptions::getInstance()->getQSettings()) readSettings(*(ProgramOptions::getInstance()->getQSettings()), "table/QFRDRTablePlotWidget/");
}

QFTableGraphSettings::~QFTableGraphSettings()
{
    updating=true;
    if (ProgramOptions::getInstance() && ProgramOptions::getInstance()->getQSettings()) writeSettings(*(ProgramOptions::getInstance()->getQSettings()), "table/QFRDRTablePlotWidget/");
    delete ui;
}

void QFTableGraphSettings::setRecord(QFRDRTable *record, int plot)
{
    current=record;
    this->plot=plot;
    updating=true;
    disconnectWidgets();

    reloadColumns(ui->cmbLinesXData);
    reloadColumns(ui->cmbLinesXError);
    reloadColumns(ui->cmbLinesYData);
    reloadColumns(ui->cmbLinesYError);
    updating=false;
    connectWidgets();

    //updateGraph();
}

void QFTableGraphSettings::rawDataChanged()
{
    if (updating) return;
    disconnectWidgets();
    reloadColumns(ui->cmbLinesXData);
    reloadColumns(ui->cmbLinesXError);
    reloadColumns(ui->cmbLinesYData);
    reloadColumns(ui->cmbLinesYError);
    connectWidgets();
    //updateGraph();
}

void QFTableGraphSettings::readSettings(QSettings &settings, const QString &prefix)
{
}

void QFTableGraphSettings::writeSettings(QSettings &settings, const QString &prefix)
{
}

void QFTableGraphSettings::on_btnFunctionHelp_clicked()
{
    if (current)
        QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelpDirectory("table")+"/mathparser.html");
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

void QFTableGraphSettings::graphDataChanged(QFRDRTable::GraphInfo& graph)
{
    //qDebug()<<"graphDataChanged    updating="<<updating;
    if (updating) return;
    if (current) {
        updating=true;
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;

        graph.title=ui->edtGraphTitle->text();

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
        updatePlotWidgetVisibility(graph);



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

    }
}

void QFTableGraphSettings::updatePlotWidgetVisibility(QFRDRTable::GraphInfo& graph)
{
    if (current) {
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;



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
        this->setVisible(true);
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

void QFTableGraphSettings::connectWidgets()
{
    //qDebug()<<"connectWidgets";


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

void QFTableGraphSettings::disconnectWidgets()
{
    //qDebug()<<"disconnectWidgets";


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
