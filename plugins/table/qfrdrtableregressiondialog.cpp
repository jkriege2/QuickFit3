#include "qfrdrtableregressiondialog.h"
#include "ui_qfrdrtableregressiondialog.h"
#include "qfpluginservices.h"
#include "statistics_tools.h"
#include "float.h"

QFRDRTableRegressionDialog::QFRDRTableRegressionDialog(QFRDRTable *table, int colX, int colY, int colW, QWidget *parent, bool logX, bool logY, int resultColumn, int addGraph) :
    QDialog(parent),
    ui(new Ui::QFRDRTableRegressionDialog)
{
    //qDebug()<<"QFRDRTableRegressionDialog colX="<<colX<<"  colY="<<colY<<"  colW="<<colW<<"  logX="<<logX<<"  logY="<<logY<<"  resultColumn="<<resultColumn<<"  addGraph="<<addGraph;
    ui->setupUi(this);
    this->table=table;
    ui->chkLogX->setChecked(logX);
    ui->chkLogY->setChecked(logY);


    ui->cmbAddGraph->clear();
    ui->cmbAddGraph->addItem(tr("--- no ---"));
    ui->cmbAddGraph->addItem(tr("--- add new graph ---"));
    for (int i=0; i<table->colgraphGetGraphCount(); i++) {
        ui->cmbAddGraph->addItem(tr("add to: %1").arg(table->colgraphGetGraphTitle(i)));
    }
    ui->cmbSaveColumn->clear();
    ui->cmbSaveColumn->addItem(tr("--- no ---"));
    ui->cmbSaveColumn->addItem(tr("--- add new column ---"));
    for (unsigned int i=0; i<table->tableGetColumnCount(); i++) {
        ui->cmbSaveColumn->addItem(tr("add to column %1 [%2]").arg(i+1).arg(table->tableGetColumnTitle(i)));
    }

    ui->pltDistribution->getXAxis()->set_axisLabel(tr("x-data [%1: %2]").arg(colX+1).arg(table->tableGetColumnTitle(colX)));
    ui->pltDistribution->getYAxis()->set_axisLabel(tr("y-data [%1: %2]").arg(colY+1).arg(table->tableGetColumnTitle(colY)));


    if (addGraph<0) {
        ui->cmbAddGraph->setCurrentIndex(0);
    } else {
        ui->cmbAddGraph->setCurrentIndex(addGraph+2);
    }
    if (resultColumn<0) {
        ui->cmbSaveColumn->setCurrentIndex(1);
    } else {
        ui->cmbSaveColumn->setCurrentIndex(resultColumn+2);
    }

    dataX=table->tableGetColumnDataAsDouble(colX);
    dataY=table->tableGetColumnDataAsDouble(colY);
    //qDebug()<<"dataX.size="<<dataX.size();
    //qDebug()<<"dataY.size="<<dataX.size();
    datapoints=qMin(dataX.size(), dataY.size());
    if (colW>=0) {
        dataW=table->tableGetColumnDataAsDouble(colW);
        //qDebug()<<"dataW.size="<<dataW.size();
        if (dataW.size()>0 && dataW.size()<datapoints) datapoints=dataW.size();
    }
    if (dataX.size()>datapoints) dataX.remove(dataX.size()-(dataX.size()-datapoints), (dataX.size()-datapoints));
    if (dataY.size()>datapoints) dataY.remove(dataY.size()-(dataY.size()-datapoints), (dataY.size()-datapoints));
    if (dataW.size()>datapoints) dataW.remove(dataW.size()-(dataW.size()-datapoints), (dataW.size()-datapoints));

    //qDebug()<<"datapoints="<<datapoints;
    for (int i=datapoints-1; i>=0; i--) {
        if (!QFFloatIsOK(dataX[i]) || !QFFloatIsOK(dataY[i]) || (dataW.size()==dataX.size() && !QFFloatIsOK(dataW[i]))) {
            dataX.remove(i, 1);
            dataY.remove(i, 1);
            dataW.remove(i, 1);
        }
    }
    datapoints=dataX.size();
    weights.clear();
    for (int i=0; i<dataW.size(); i++) weights<<dataW[i];
    //qDebug()<<"datapoints_after_clean="<<datapoints;

    if (dataX.size()>0) {
        ui->datacut->set_min(qfstatisticsMin(dataX));
        ui->datacut->set_max(qfstatisticsMax(dataX));
    }
    ui->datacut->setLogScale(logX, 20);

    paramMap["a"].value=0;
    paramMap["a"].fix=false;
    paramMap["b"].value=1;
    paramMap["b"].fix=false;
    paramMap["iterations"].value=100;
    paramMap["irls_param"].value=1.1;

    ui->tabParams->setItemDelegate(new QFFitFunctionValueInputDelegate(ui->tabParams));
    parameterTable=new QFFitFunctionValueInputTable(this);

    ui->tabParams->setModel(parameterTable);

    ui->cmbFitType->addItem(tr("linear regression"), 0);
    if (dataW.size()==datapoints) ui->cmbFitType->addItem(tr("linear regression, weighted"), 1);
    ui->cmbFitType->addItem(tr("linear regression, robust"), 2);

    ui->cmbFitType->setCurrentIndex(0);
    methodChanged(ui->cmbFitType->currentIndex());
    connectSignals(true);
    on_btnFit_clicked();
}


QFRDRTableRegressionDialog::~QFRDRTableRegressionDialog()
{
    delete ui;
}

void QFRDRTableRegressionDialog::saveResults()
{


    int method=ui->cmbFitType->itemData(ui->cmbFitType->currentIndex()).toInt();
    int saveCol=ui->cmbSaveColumn->currentIndex();
    int saveGraph=ui->cmbAddGraph->currentIndex();
    if (lastResults.size()>0) {
        int savedTo=-1;
        if (saveCol==1) {
            savedTo=table->tableGetColumnCount();
            table->tableSetColumnData(savedTo, lastResults);

        } else if (saveCol>=2){
            savedTo=saveCol-2;
            table->tableSetColumnData(savedTo, lastResults);
        }
        if (saveGraph==1) {
            table->colgraphAddGraph(tr("regression results"), ui->pltDistribution->getXAxis()->get_axisLabel(), ui->pltDistribution->getYAxis()->get_axisLabel(), ui->pltDistribution->getXAxis()->get_logAxis(), ui->pltDistribution->getYAxis()->get_logAxis());
            int g=table->colgraphGetGraphCount()-1;
            table->colgraphAddPlot(g, colX, colY, QFRDRColumnGraphsInterface::cgtPoints, tr("data"));
            if (method>=0 && method<=2) {
                if (savedTo>=0) {
                    table->colgraphAddFunctionPlot(g, "", QFRDRColumnGraphsInterface::cgtPolynomial, fitresult, savedTo);
                } else {
                    table->colgraphAddFunctionPlot(g, "", QFRDRColumnGraphsInterface::cgtPolynomial, fitresult, lastResultD);
                }
            }
        } else if (saveGraph>=2){
            if (method>=0 && method<=2) {
                if (savedTo>=0) {
                    table->colgraphAddFunctionPlot(saveGraph-2, "", QFRDRColumnGraphsInterface::cgtPolynomial, fitresult, savedTo);
                } else {
                    table->colgraphAddFunctionPlot(saveGraph-2, "", QFRDRColumnGraphsInterface::cgtPolynomial, fitresult, lastResultD);
                }
            }
        }
    }
    accept();
}

void QFRDRTableRegressionDialog::on_btnFit_clicked()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    connectSignals(false);
    double a=getParamValue("a", 0);
    double aerr=0;
    double b=getParamValue("b", 1);
    double berr=0;
    bool afix=getParamFix("a", false);
    bool bfix=getParamFix("b", false);
    int method=ui->cmbFitType->itemData(ui->cmbFitType->currentIndex()).toInt();
    int rmin=getRangeMin();
    int rmax=getRangeMax();

    double* dx=dataX.data();
    double* dy=dataY.data();
    double* dw=dataW.data();
    double* dww=weights.data();
    int items=rmax-rmin+1;

    bool ok=false;
    QString error="";

    if (items>1) {
        dx=&(dx[rmin]);
        dy=&(dy[rmin]);
        dw=&(dw[rmin]);
        dww=&(dww[rmin]);
        error=tr("unknown regression method!");
        lastResults.clear();
        lastResultD.clear();
        resultComment="";
        if (method==0) {
            statisticsLinearRegression(dx, dy, items, a, b, afix, bfix);
            fitresult=tr("regression: f(x)= %1 + %2 \\cdot x").arg(floattolatexstr(a).c_str()).arg(floattolatexstr(b).c_str());
            lastResults<<a<<b;
            lastResultD<<a<<b;
            resultComment=tr("regression result");
            ok=true;
        } else if (method==1 && dataW.size()==datapoints) {
            statisticsLinearWeightedRegression(dx, dy, dww, items, a, b, afix, bfix);
            fitresult=tr("weighted regression: f(x)= %1 + %2 \\cdot x").arg(floattolatexstr(a).c_str()).arg(floattolatexstr(b).c_str());
            lastResults<<a<<b;
            lastResultD<<a<<b;
            resultComment=tr("weighted regression result");
            ok=true;
        } else if (method==2) {
            int iterations=getParamValue("iterations", 100);
            double param=getParamValue("irls_param", 1.1);
            statisticsIterativelyReweightedLeastSquaresRegression(&(dx[rmin]), &(dy[rmin]), items, a, b, param, iterations, afix, bfix);
            fitresult=tr("robust regression: f(x)= %1 + %2 \\cdot x").arg(floattolatexstr(a).c_str()).arg(floattolatexstr(b).c_str());
            lastResults<<a<<b;
            lastResultD<<a<<b;
            resultComment=tr("robust regression result");
            ok=true;
        } else {
        }
    }

    if (ok) {
        paramMap["a"].value=a;
        paramMap["a"].error=aerr;
        paramMap["a"].fix=afix;
        paramMap["b"].value=b;
        paramMap["b"].error=berr;
        paramMap["b"].fix=bfix;

        parameterTable->rebuildModel();
        QApplication::restoreOverrideCursor();
        replotGraph();
    } else {
        QMessageBox::critical(this, tr("Regression Error"), tr("an error occured during the regression analysis.\nerror message:\n    %1").arg(error));
    }
    connectSignals(true);
}

void QFRDRTableRegressionDialog::replotGraph()
{
    int method=ui->cmbFitType->itemData(ui->cmbFitType->currentIndex()).toInt();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->pltDistribution->set_doDrawing(false);
    ui->pltDistribution->set_emitSignals(false);
    ui->pltDistribution->getXAxis()->set_labelFontSize(11);
    ui->pltDistribution->getXAxis()->set_tickLabelFontSize(10);
    ui->pltDistribution->getXAxis()->set_logAxis(ui->chkLogX->isChecked());
    ui->pltDistribution->getYAxis()->set_labelFontSize(11);
    ui->pltDistribution->getYAxis()->set_tickLabelFontSize(10);
    ui->pltDistribution->getYAxis()->set_logAxis(ui->chkLogX->isChecked());
    ui->pltDistribution->getXAxis()->set_drawMode1(JKQTPCADMcomplete);
    ui->pltDistribution->get_plotter()->setBorder(1,1,1,1);
    ui->pltDistribution->get_plotter()->set_useAntiAliasingForSystem(true);
    ui->pltDistribution->get_plotter()->set_useAntiAliasingForGraphs(true);
    ui->pltDistribution->get_plotter()->set_keyFontSize(9);
    ui->pltDistribution->get_plotter()->set_keyXMargin(2);
    ui->pltDistribution->get_plotter()->set_keyYMargin(2);
    ui->pltDistribution->get_plotter()->set_keyPosition(JKQTPkeyInsideTopLeft);
    QColor cb("white");
    cb.setAlphaF(0.5);
    ui->pltDistribution->get_plotter()->set_keyBackgroundColor(cb);
    ui->pltDistribution->get_plotter()->set_keyPosition(JKQTPkeyInsideTopLeft);


    ui->pltDistribution->get_plotter()->clearGraphs(true);
    ui->pltDistribution->get_plotter()->clearOverlayElement(true);

    JKQTPdatastore* ds=ui->pltDistribution->get_plotter()->getDatastore();
    ds->clear();
    size_t c_X=ds->addCopiedColumn(dataX.data(), dataX.size(), tr("x-data"));
    size_t c_Y=ds->addCopiedColumn(dataY.data(), dataY.size(), tr("y-data"));
    size_t c_W=ds->addCopiedColumn(dataW.data(), dataW.size(), tr("weight"));


    JKQTPxyLineErrorGraph* g_dist=new JKQTPxyLineErrorGraph(ui->pltDistribution->get_plotter());
    g_dist->set_drawLine(true);
    g_dist->set_title(tr("data"));
    g_dist->set_xColumn(c_X);
    g_dist->set_yColumn(c_Y);
    g_dist->set_yErrorColumn(c_W);
    g_dist->set_xErrorStyle(JKQTPnoError);
    g_dist->set_yErrorStyle(JKQTPerrorBars);
    g_dist->set_color(QColor("blue"));
    g_dist->set_errorColor(g_dist->get_color().lighter());
    g_dist->set_drawLine(false);
    g_dist->set_symbol(JKQTPcross);
    g_dist->set_symbolSize(7);
    ui->pltDistribution->addGraph(g_dist);

    JKQTPoverlayVerticalRange* ovlRange=new JKQTPoverlayVerticalRange(ui->datacut->get_userMin(), ui->datacut->get_userMax(), ui->pltDistribution->get_plotter());
    ovlRange->set_inverted(true);
    QColor fillRange=QColor("grey");
    fillRange.setAlphaF(0.75);
    ovlRange->set_fillColor(fillRange);
    ui->pltDistribution->get_plotter()->addOverlayElement(ovlRange);

    JKQTPxFunctionLineGraph* g_fit=new JKQTPxFunctionLineGraph(ui->pltDistribution->get_plotter());
    g_fit->set_drawLine(true);
    g_fit->set_title(fitresult);
    g_fit->set_color(QColor("red"));
    bool ok=false;
    if (method>=0 && method<=2) {
        QVector<double> vecP;
        vecP<<getParamValue("a", 0);
        vecP<<getParamValue("b", 1);
        g_fit->set_params(vecP);
        g_fit->setSpecialFunction(JKQTPxFunctionLineGraph::Polynomial);
        ok=true;
    }
    if (ok) {
        ui->pltDistribution->addGraph(g_fit);
    } else {
        delete g_fit;
    }



    ui->pltDistribution->zoomToFit();
    ui->pltDistribution->set_doDrawing(true);
    ui->pltDistribution->set_emitSignals(true);
    ui->pltDistribution->update_plot();
    QApplication::restoreOverrideCursor();
}

void QFRDRTableRegressionDialog::showHelp()
{
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelpDirectory("table")+"regression.html");
}

void QFRDRTableRegressionDialog::methodChanged(int method)
{
    parameterTable->setDoRebuildModel(false);
    parameterTable->setEditErrors(false);
    parameterTable->setEditFix(true);
    parameterTable->setEditRanges(false);
    parameterNames<<"a";
    parameterLabels<<"offset a = ";
    parameterNames<<"b";
    parameterLabels<<"slope b = ";
    if (method==2) {
        parameterNames<<"irls_param";
        parameterLabels<<"IRLS parameter:";
        parameterNames<<"iterations";
        parameterLabels<<"IRLS iterations:";
    }
    if (method>=0 && method<=2) {
        ui->labEquation->setText("f(x) = a + b\\cdot x");
    } else {
        ui->labEquation->setText("--- --- ---");
    }
    parameterTable->setWriteTo(&paramMap, parameterNames, parameterLabels);
    parameterTable->setDoRebuildModel(true);
    on_btnFit_clicked();
    //replotGraph();
}

void QFRDRTableRegressionDialog::connectSignals(bool connectS)
{
    if (connectS) {
        connect(parameterTable, SIGNAL(fitParamChanged()), this, SLOT(replotGraph()));
        connect(ui->cmbFitType, SIGNAL(currentIndexChanged(int)), this, SLOT(methodChanged(int)));
        connect(ui->chkLogX, SIGNAL(toggled(bool)), this, SLOT(replotGraph()));
        connect(ui->chkLogY, SIGNAL(toggled(bool)), this, SLOT(replotGraph()));
        connect(ui->datacut, SIGNAL(slidersChanged(double,double,double,double)), this, SLOT(replotGraph()));
    } else {
        disconnect(parameterTable, SIGNAL(fitParamChanged()), this, SLOT(replotGraph()));
        disconnect(ui->cmbFitType, SIGNAL(currentIndexChanged(int)), this, SLOT(methodChanged(int)));
        disconnect(ui->chkLogX, SIGNAL(toggled(bool)), this, SLOT(replotGraph()));
        disconnect(ui->chkLogY, SIGNAL(toggled(bool)), this, SLOT(replotGraph()));
        disconnect(ui->datacut, SIGNAL(slidersChanged(double,double,double,double)), this, SLOT(replotGraph()));
    }
}

int QFRDRTableRegressionDialog::getRangeMin()
{
    int rm=0;
    for (int i=0; i<datapoints; i++) {
        if (dataX[i]>=ui->datacut->get_userMin()) {
            rm=i;
            break;
        }
    }
    return qBound(rm, 0, dataX.size()-1);
}

int QFRDRTableRegressionDialog::getRangeMax()
{
    int rm=datapoints-1;
    for (int i=datapoints-1; i>=0; i--) {
        if (dataX[i]>ui->datacut->get_userMax()) {
            rm=i;            
        } else {
            break;
        }
    }

    return qBound(0, rm, dataX.size()-1);
}

double QFRDRTableRegressionDialog::getParamValue(const QString &param, double defaultVal) const
{
    return paramMap.value(param, QFFitFunctionValueInputTableFitParamData(defaultVal,0)).value;
}

double QFRDRTableRegressionDialog::getParamError(const QString &param, double defaultVal) const
{
    return paramMap.value(param, QFFitFunctionValueInputTableFitParamData(0,defaultVal)).error;
}

bool QFRDRTableRegressionDialog::getParamFix(const QString &param, bool defaultVal) const
{
    return paramMap.value(param, QFFitFunctionValueInputTableFitParamData(0,0,defaultVal)).fix;
}

double QFRDRTableRegressionDialog::getParamMin(const QString &param, double defaultVal) const
{
    return paramMap.value(param, QFFitFunctionValueInputTableFitParamData(0,0,false,defaultVal)).min;
}

double QFRDRTableRegressionDialog::getParamMax(const QString &param, double defaultVal) const
{
    return paramMap.value(param, QFFitFunctionValueInputTableFitParamData(0,0,false,defaultVal)).max;
}

