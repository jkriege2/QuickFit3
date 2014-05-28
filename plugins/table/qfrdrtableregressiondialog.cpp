#include "qfrdrtableregressiondialog.h"
#include "ui_qfrdrtableregressiondialog.h"
#include "qfpluginservices.h"
#include "statistics_tools.h"
#include "float.h"

QFRDRTableRegressionDialog::QFRDRTableRegressionDialog(QFRDRTable *table, int colX, int colY, int colW, QWidget *parent, bool logX, bool logY, int resultColumn, int addGraph) :
    QDialog(parent),
    ui(new Ui::QFRDRTableRegressionDialog)
{
    qDebug()<<"QFRDRTableRegressionDialog colX="<<colX<<"  colY="<<colY<<"  colW="<<colW<<"  logX="<<logX<<"  logY="<<logY<<"  resultColumn="<<resultColumn<<"  addGraph="<<addGraph;
    parameterTable=new QFFitFunctionValueInputTable(this);
    ui->setupUi(this);
    overwriteGraph=-1;
    overwritePlot=-1;
    intInit(table,  colX,  colY,  colW, parent,  logX,  logY,  resultColumn,  addGraph);
}

QFRDRTableRegressionDialog::QFRDRTableRegressionDialog(QFRDRTable *table, int plotid, int graphid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRTableRegressionDialog)
{
    parameterTable=new QFFitFunctionValueInputTable(this);
    ui->setupUi(this);
    int resultColumn=-1,  addGraph=-1;
    this->table=table;
    readFitProperties(plotid, graphid, &resultColumn, &addGraph);
    overwriteGraph=graphid;
    overwritePlot=plotid;
    qDebug()<<"QFRDRTableRegressionDialog("<<plotid<<graphid<<"): "<<overwriteGraph<<colX<<colY;
    intInit(table,  colX,  colY,  colW, parent,  ui->chkLogX->isChecked(),  ui->chkLogY->isChecked(),  resultColumn,  addGraph, true);
}


QFRDRTableRegressionDialog::~QFRDRTableRegressionDialog()
{
    QSettings* set=ProgramOptions::getInstance()->getQSettings();
    if (set) {
        saveWidgetGeometry(*set, this, "QFRDRTableRegressionDialog/windowsize");
        set->setValue("QFRDRTableRegressionDialog/ploterrors", ui->chkPlotErrors->isChecked());
        set->setValue("QFRDRTableRegressionDialog/weightedresiduals", ui->chkWeightedResiduals->isChecked());
        set->setValue("QFRDRTableRegressionDialog/method", ui->cmbFitType->currentIndex());
        saveSplitter(*set, ui->splitter, "QFRDRTableRegressionDialog/splitter/");
    }
    delete ui;
}

void QFRDRTableRegressionDialog::saveResults()
{


    int method=ui->cmbFitType->itemData(ui->cmbFitType->currentIndex()).toInt();
    int saveCol=ui->cmbSaveColumn->currentIndex();
    int saveGraph=ui->cmbAddGraph->currentIndex();
    QFRDRColumnGraphsInterface::ColumnGraphTypes graphType=QFRDRColumnGraphsInterface::cgtPolynomial;
    if (method>=0 && method<=2) {
        graphType=QFRDRColumnGraphsInterface::cgtPolynomial;
    } else if (method>=3 && method<=4) {
        graphType=QFRDRColumnGraphsInterface::cgtPowerLaw;
    }
    if (lastResults.size()>0) {
        int savedTo=-1;
        if (saveCol==1) {
            savedTo=table->tableGetColumnCount();
            table->tableSetColumnData(savedTo, lastResults);
            table->tableSetColumnTitle(savedTo, tr("regression results, %1").arg(ui->cmbFitType->currentText()));
            table->tableSetColumnData(savedTo+1, lastResultErrors);
            table->tableSetColumnTitle(savedTo+1, tr("regression errors, %1").arg(ui->cmbFitType->currentText()));
            for (int i=0; i<lastResults.size(); i++) {
                table->tableSetComment(i, savedTo, parameterNames.value(i, "")+"/"+parameterLabels.value(i, ""));
                table->tableSetComment(i, savedTo+1, parameterNames.value(i, "")+"/"+parameterLabels.value(i, ""));
            }
        } else if (saveCol>=2){
            savedTo=saveCol-2;
            table->tableSetColumnData(savedTo, lastResults);
            table->tableSetColumnTitle(savedTo, tr("regression results, %1").arg(ui->cmbFitType->currentText()));
            table->tableSetColumnData(savedTo+1, lastResultErrors);
            table->tableSetColumnTitle(savedTo+1, tr("regression errors, %1").arg(ui->cmbFitType->currentText()));
            for (int i=0; i<lastResults.size(); i++) {
                table->tableSetComment(i, savedTo, parameterNames.value(i, "")+"/"+parameterLabels.value(i, ""));
                table->tableSetComment(i, savedTo+1, parameterNames.value(i, "")+"/"+parameterLabels.value(i, ""));
            }        }
        if (saveGraph==1) {
            table->colgraphAddPlot(tr("regression results"), ui->pltDistribution->getXAxis()->get_axisLabel(), ui->pltDistribution->getYAxis()->get_axisLabel(), ui->pltDistribution->getXAxis()->get_logAxis(), ui->pltDistribution->getYAxis()->get_logAxis());
            int g=table->colgraphGetPlotCount()-1;
            table->colgraphAddGraph(g, colX, colY, QFRDRColumnGraphsInterface::cgtPoints, tr("data"));
            if (savedTo>=0) {
                table->colgraphAddFunctionGraph(g, "",graphType, fitresult, savedTo);
            } else {
                table->colgraphAddFunctionGraph(g, "", graphType, fitresult, lastResultD);
            }
            table->colgraphSetGraphTitle(g, table->colgraphGetGraphCount(g)-1, fitresult+", "+resultStat);
            writeFitProperties(g, table->colgraphGetGraphCount(g)-1, savedTo);
        } else if (saveGraph>=2 && saveGraph<ui->cmbAddGraph->count()-1){
                if (savedTo>=0) {
                    table->colgraphAddFunctionGraph(saveGraph-2, "", graphType, fitresult, savedTo);
                } else {
                    table->colgraphAddFunctionGraph(saveGraph-2, "", graphType, fitresult, lastResultD);
                }
            table->colgraphSetGraphTitle(saveGraph-2, table->colgraphGetGraphCount(saveGraph-2)-1, fitresult+", "+resultStat);
            writeFitProperties(saveGraph-2, table->colgraphGetGraphCount(saveGraph-2)-1, savedTo);
        } else if (saveGraph==ui->cmbAddGraph->count()-1) {
            if (savedTo>=0) {
                table->colgraphSetFunctionGraph(overwritePlot, overwriteGraph, "", graphType, fitresult, savedTo);
            } else {
                table->colgraphSetFunctionGraph(overwritePlot, overwriteGraph, "", graphType, fitresult, lastResultD);
            }
            table->colgraphSetGraphTitle(overwritePlot, overwriteGraph, fitresult+", "+resultStat);
            writeFitProperties(overwritePlot, overwriteGraph, savedTo);
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




    QVector<double> datX;
    QVector<double> datY;
    QVector<double> datXLog;
    QVector<double> datYLog;
    QVector<double> datW;
    QVector<double> datWW;


    /*for (int i=rmin; i<=rmax; i++) {
        datX<<dataX[i];
        datY<<dataY[i];
        datW<<dataW[i];
    }*/
    for (int i=0; i<datapoints; i++) {
        if (dataX[i]>=ui->datacut->get_userMin() && dataX[i]<=ui->datacut->get_userMax()) {
            datX<<dataX[i];
            datY<<dataY[i];
            datW<<dataW[i];
            datWW<<weights[i];
            datXLog<<log(dataX[i]);
            datYLog<<log(dataY[i]);
        }
    }


    double* dx=datX.data();
    double* dy=datY.data();
    double* dxl=datXLog.data();
    double* dyl=datYLog.data();
    double* dw=datW.data();
    double* dww=datWW.data();
    int items=datX.size();
    //qDebug()<<"items="<<items;

    QVector<double> funeval, residuals, residuals_weighted;

    bool ok=false;
    QString error="";

    if (items>1) {
        error=tr("unknown regression method!");
        lastResults.clear();
        lastResultD.clear();
        lastResultErrors.clear();
        resultComment="";
        resultParams="";
        if (method==0) {
            statisticsLinearRegression(dx, dy, items, a, b, afix, bfix);
            fitresult=tr("regression: f(x)= %1 + %2 \\cdot x").arg(floattolatexstr(a).c_str()).arg(floattolatexstr(b).c_str());
            lastResults<<a<<b;
            lastResultD<<a<<b;
            lastResultErrors<<0<<0;
            resultComment=tr("regression ");
            ok=true;
        } else if (method==1 && dataW.size()==datapoints) {
            statisticsLinearWeightedRegression(dx, dy, dww, items, a, b, afix, bfix);
            fitresult=tr("weighted regression: f(x)= %1 + %2 \\cdot x").arg(floattolatexstr(a).c_str()).arg(floattolatexstr(b).c_str());
            lastResults<<a<<b;
            lastResultD<<a<<b;
            lastResultErrors<<0<<0;
            resultComment=tr("weighted regression ");
            ok=true;
        } else if (method==2) {
            int iterations=getParamValue("iterations", 100);
            double param=getParamValue("irls_param", 1.1);
            statisticsIterativelyReweightedLeastSquaresRegression(dx, dy, items, a, b, param, iterations, afix, bfix);
            fitresult=tr("robust regression: f(x)= %1 + %2 \\cdot x").arg(floattolatexstr(a).c_str()).arg(floattolatexstr(b).c_str());
            lastResults<<a<<b;
            lastResultD<<a<<b;
            lastResultErrors<<0<<0;

            resultComment=tr("robust regression ");
            ok=true;
        } else  if (method==3) {
            double ia=log(a);
            double ib=b;
            statisticsLinearRegression(dxl, dyl, items, ia, ib, afix, bfix);
            a=exp(ia);
            b=ib;
            fitresult=tr("regression: f(x)= %1 \\cdot x^{%2}").arg(floattolatexstr(a).c_str()).arg(floattolatexstr(b).c_str());
            lastResults<<a<<b;
            lastResultD<<a<<b;
            lastResultErrors<<0<<0;

            resultComment=tr("regression ");
            ok=true;
        } else if (method==4) {
            int iterations=getParamValue("iterations", 100);
            double param=getParamValue("irls_param", 1.1);
            double ia=log(a);
            double ib=b;
            statisticsIterativelyReweightedLeastSquaresRegression(dxl, dyl, items, ia, ib, param, iterations, afix, bfix);
            a=exp(ia);
            b=ib;
            fitresult=tr("regression: f(x)= %1 \\cdot x^{%2}").arg(floattolatexstr(a).c_str()).arg(floattolatexstr(b).c_str());
            lastResults<<a<<b;
            lastResultD<<a<<b;
            lastResultErrors<<0<<0;

            resultComment=tr("robust regression ");
            ok=true;

        } else {
        }

    }

    updateFitStatistics();

    if (ok) {
        paramMap["a"].value=a;
        paramMap["a"].error=aerr;
        paramMap["a"].fix=afix;
        paramMap["b"].value=b;
        paramMap["b"].error=berr;
        paramMap["b"].fix=bfix;

        parameterTable->rebuildModel();
        QApplication::restoreOverrideCursor();
     } else {
        QMessageBox::critical(this, tr("Regression Error"), tr("an error occured during the regression analysis.\nerror message:\n    %1").arg(error));
    }
    replotGraph();
    connectSignals(true);
}

void QFRDRTableRegressionDialog::updateFitStatistics()
{
    double a=getParamValue("a", 0);
    double b=getParamValue("b", 1);
    int method=ui->cmbFitType->itemData(ui->cmbFitType->currentIndex()).toInt();

    int items=dataX.size();
    QVector<double> funeval, residuals, residuals_weighted;



    if (items>1) {
        if (method>=0 && method<=2) {
            for (int i=0; i<dataX.size(); i++) {
                fitresult=tr("regression: f(x)= %1 + %2 \\cdot x").arg(floattolatexstr(a).c_str()).arg(floattolatexstr(b).c_str());
                funeval<<a+b*dataX[i];
                residuals<<dataY[i]-funeval[i];
                residuals_weighted<<residuals[i]/dataW[i];
            }
        } else  if (method>=3 && method<=4) {
            fitresult=tr("regression: f(x)= %1 \\cdot x^{%2}").arg(floattolatexstr(a).c_str()).arg(floattolatexstr(b).c_str());
            for (int i=0; i<dataX.size(); i++) {
                funeval<<a*pow(dataX[i],b);
                residuals<<dataY[i]-funeval[i];
                residuals_weighted<<residuals[i]/dataW[i];
            }
        } else {
        }

        residualsY=residuals;
        residualsYW=residuals_weighted;

        // estimate goodness-of-fit
        resultStat="";
        if (residuals.size()>0) {
            double chi2=0;
            double chi2w=0;
            double TSS=0;
            double obsMean=0;
            for (int i=0; i<items; i++) {
                obsMean+=dataY[i];
            }
            obsMean=obsMean/double(items);
            for (int i=0; i<items; i++) {
                chi2+=qfSqr(residuals[i]);
                chi2w+=qfSqr(residuals_weighted[i]);
                TSS+=qfSqr(dataY[i]-obsMean);
            }
            paramMap["chi2"].value=chi2;
            paramMap["chi2"].error=0;
            paramMap["chi2"].fix=false;
            paramMap["chi2"].editable=false;
            resultStat+=tr("\\chi^2=%1").arg(chi2);

            double R2=1.0-chi2/TSS;
            paramMap["R2"].value=R2;
            paramMap["R2"].error=0;
            paramMap["R2"].fix=false;
            paramMap["R2"].editable=false;
            resultStat+=tr(", R^2=%1").arg(R2);

            ui->labFitResult->setText(resultStat);

        }
    }

}

void QFRDRTableRegressionDialog::replotGraph()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    int method=ui->cmbFitType->itemData(ui->cmbFitType->currentIndex()).toInt();
    updateFitStatistics();
    ui->pltDistribution->set_doDrawing(false);
    ui->pltDistribution->set_emitSignals(false);
    ui->pltResiduals->set_doDrawing(false);
    ui->pltResiduals->set_emitSignals(false);
    ui->pltDistribution->getXAxis()->set_logAxis(ui->chkLogX->isChecked());
    ui->pltResiduals->getXAxis()->set_logAxis(ui->chkLogX->isChecked());
    ui->pltDistribution->getYAxis()->set_logAxis(ui->chkLogY->isChecked());


    ui->pltDistribution->get_plotter()->clearGraphs(true);
    ui->pltDistribution->get_plotter()->clearOverlayElement(true);
    ui->pltResiduals->get_plotter()->clearGraphs(true);
    ui->pltResiduals->get_plotter()->clearOverlayElement(true);

    JKQTPdatastore* ds=ui->pltDistribution->get_plotter()->getDatastore();
    ds->clear();
    size_t c_X=ds->addCopiedColumn(dataX.data(), dataX.size(), tr("x-data"));
    size_t c_Y=ds->addCopiedColumn(dataY.data(), dataY.size(), tr("y-data"));
    size_t c_W=ds->addCopiedColumn(dataW.data(), dataW.size(), tr("weight"));
    size_t c_ResY=ds->addCopiedColumn(residualsY.data(), residualsY.size(), tr("residuals"));
    size_t c_ResYW=ds->addCopiedColumn(residualsYW.data(), residualsYW.size(), tr("weighted residuals"));


    JKQTPxyLineErrorGraph* g_dist=new JKQTPxyLineErrorGraph(ui->pltDistribution->get_plotter());
    g_dist->set_drawLine(true);
    g_dist->set_title(tr("data"));
    g_dist->set_xColumn(c_X);
    g_dist->set_yColumn(c_Y);
    g_dist->set_yErrorColumn(c_W);
    g_dist->set_xErrorStyle(JKQTPnoError);
    if (ui->chkPlotErrors->isChecked()) g_dist->set_yErrorStyle(JKQTPerrorBars);
    else g_dist->set_yErrorStyle(JKQTPnoError);
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
    if (method>=0 && method<=4) {
        QVector<double> vecP;
        vecP<<getParamValue("a", 0);
        vecP<<getParamValue("b", 1);
        g_fit->set_params(vecP);
        g_fit->setSpecialFunction(JKQTPxFunctionLineGraph::Polynomial);
        if (method>=3 && method<=4) g_fit->setSpecialFunction(JKQTPxFunctionLineGraph::PowerLaw);
        ok=true;
    }
    if (ok) {
        ui->pltDistribution->addGraph(g_fit);
    } else {
        delete g_fit;
    }


    JKQTPxyLineGraph* g_res=new JKQTPxyLineGraph(ui->pltResiduals->get_plotter());
    g_res->set_drawLine(true);
    g_res->set_xColumn(c_X);
    g_res->set_yColumn(c_ResY);
    if (ui->chkWeightedResiduals->isChecked()) {
        g_res->set_title(tr("weighted residuals"));
        g_res->set_yColumn(c_ResYW);
    } else {
        g_res->set_title(tr("residuals"));
        g_res->set_yColumn(c_ResY);
    }
    g_res->set_color(g_dist->get_color());
    g_res->set_symbol(JKQTPcross);
    g_res->set_symbolSize(7);
    ui->pltResiduals->addGraph(g_res);

    ui->pltResiduals->set_doDrawing(true);
    ui->pltResiduals->set_emitSignals(true);
    ui->pltResiduals->zoomToFit();
    ui->pltResiduals->update_plot();
    ui->pltDistribution->set_doDrawing(true);
    ui->pltDistribution->set_emitSignals(true);
    ui->pltDistribution->zoomToFit();
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
    fillFitParams(method);
    if (method>=0 && method<=2) {
        ui->labEquation->setText("f(x) = a + b\\cdot x");
    } else if (method>=3 && method<=4) {
        ui->labEquation->setText("f(x) = a \\cdot x^b");
    } else {
        ui->labEquation->setText("--- --- ---");
    }
    on_btnFit_clicked();
    //replotGraph();
}

void QFRDRTableRegressionDialog::fillFitParams(int method)
{
    parameterNames.clear();
    parameterLabels.clear();
    parameterNames<<"a";
    parameterLabels<<"offset a = ";
    parameterNames<<"b";
    parameterLabels<<"slope b = ";
    if (method==2) {
        parameterNames<<"irls_param";
        parameterLabels<<"IRLS parameter: ";
        parameterNames<<"iterations";
        parameterLabels<<"IRLS iterations: ";
    }
    parameterNames<<"R2";
    parameterLabels<<"R² = ";
    parameterTable->setWriteTo(&paramMap, parameterNames, parameterLabels);
    parameterTable->setDoRebuildModel(true);
}

void QFRDRTableRegressionDialog::connectSignals(bool connectS)
{
    if (connectS) {
        connect(parameterTable, SIGNAL(fitParamChanged()), this, SLOT(replotGraph()));
        connect(ui->cmbFitType, SIGNAL(currentIndexChanged(int)), this, SLOT(methodChanged(int)));
        connect(ui->chkLogX, SIGNAL(toggled(bool)), this, SLOT(replotGraph()));
        connect(ui->chkLogY, SIGNAL(toggled(bool)), this, SLOT(replotGraph()));
        connect(ui->chkPlotErrors, SIGNAL(toggled(bool)), this, SLOT(replotGraph()));
        connect(ui->chkWeightedResiduals, SIGNAL(toggled(bool)), this, SLOT(replotGraph()));
        connect(ui->datacut, SIGNAL(slidersChanged(double,double,double,double)), this, SLOT(replotGraph()));
    } else {
        disconnect(parameterTable, SIGNAL(fitParamChanged()), this, SLOT(replotGraph()));
        disconnect(ui->cmbFitType, SIGNAL(currentIndexChanged(int)), this, SLOT(methodChanged(int)));
        disconnect(ui->chkLogX, SIGNAL(toggled(bool)), this, SLOT(replotGraph()));
        disconnect(ui->chkLogY, SIGNAL(toggled(bool)), this, SLOT(replotGraph()));
        disconnect(ui->chkPlotErrors, SIGNAL(toggled(bool)), this, SLOT(replotGraph()));
        disconnect(ui->chkWeightedResiduals, SIGNAL(toggled(bool)), this, SLOT(replotGraph()));
        disconnect(ui->datacut, SIGNAL(slidersChanged(double,double,double,double)), this, SLOT(replotGraph()));
    }
}

void QFRDRTableRegressionDialog::readDataFromTable()
{

    dataX=table->tableGetColumnDataAsDouble(colX);
    dataY=table->tableGetColumnDataAsDouble(colY);
    qDebug()<<colX<<"dataX.size="<<dataX.size();
    qDebug()<<colX<<"dataY.size="<<dataX.size();
    datapoints=qMin(dataX.size(), dataY.size());
    if (colW>=0) {
        dataW=table->tableGetColumnDataAsDouble(colW);
        //qDebug()<<"dataW.size="<<dataW.size();
        if (dataW.size()>0 && dataW.size()<datapoints) datapoints=dataW.size();
    } else {
        dataW=QVector<double>(datapoints, 1);
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
    for (int i=0; i<dataW.size(); i++) weights<<1.0/qfSqr(dataW[i]);
    //qDebug()<<"datapoints_after_clean="<<datapoints;

    if (dataX.size()>0) {
        double mi=qfstatisticsMin(dataX);
        double ma=qfstatisticsMax(dataX);
        ui->datacut->set_min(mi-(ma-mi)/2.0);
        ui->datacut->set_max(ma+(ma-mi)/2.0);
    }
}

int QFRDRTableRegressionDialog::getRangeMin()
{
    int rm=0;
    for (int i=0; i<dataX.size(); i++) {
        if (dataX[i]>=ui->datacut->get_userMin()) {
            rm=i;
            break;
        }
    }
    return qBound(0, rm, dataX.size()-1);
}

int QFRDRTableRegressionDialog::getRangeMax()
{
    int rm=getRangeMin();
    for (int i=rm; i<dataX.size(); i++) {
        if (dataX[i]<=ui->datacut->get_userMax()) {
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

void QFRDRTableRegressionDialog::writeFitProperties(int pid, int gid, int saveToColumn)
{
    bool emits=table->colgraphGetDoEmitSignals();
    table->colgraphSetDoEmitSignals(false);
    table->colgraphSetGraphProperty(pid, gid, "FIT_TYPE", "REGRESSION");
    table->colgraphSetGraphProperty(pid, gid, "FIT_COLX", colX);
    table->colgraphSetGraphProperty(pid, gid, "FIT_COLY", colY);
    table->colgraphSetGraphProperty(pid, gid, "FIT_COLW", colW);
    table->colgraphSetGraphProperty(pid, gid, "FIT_METHOD", ui->cmbFitType->itemData(ui->cmbFitType->currentIndex()).toInt());
    table->colgraphSetGraphProperty(pid, gid, "FIT_CUTLOW", ui->datacut->get_userMin());
    table->colgraphSetGraphProperty(pid, gid, "FIT_CUTHIGH", ui->datacut->get_userMax());
    table->colgraphSetGraphProperty(pid, gid, "FIT_LOGX", ui->chkLogX->isChecked());
    table->colgraphSetGraphProperty(pid, gid, "FIT_LOGY", ui->chkLogY->isChecked());
    table->colgraphSetGraphProperty(pid, gid, "FIT_RESULTCOLUMN", ui->cmbSaveColumn->currentIndex());
    table->colgraphSetGraphProperty(pid, gid, "FIT_PLOT", pid);
    table->colgraphSetGraphProperty(pid, gid, "FIT_GRAPH", gid);
    table->colgraphSetGraphProperty(pid, gid, "FIT_SAVEDTOCOLUMN", saveToColumn);
        QList<QVariant> fixparams, errparams;
        for (int i=0; i<parameterNames.size(); i++) {
            fixparams<<paramMap[parameterNames[i]].fix;
            errparams<<paramMap[parameterNames[i]].error;
        }
        table->colgraphSetGraphProperty(pid, gid, "FIT_FIXEDPARAMS", fixparams);
        table->colgraphSetGraphProperty(pid, gid, "FIT_ERRORPARAMS", errparams);

    table->colgraphSetDoEmitSignals(emits);

}

void QFRDRTableRegressionDialog::readFitProperties(int pid, int gid, int *resultColumn, int *addGraph)
{
    colX=table->colgraphGetGraphProperty(pid, gid, "FIT_COLX", colX).toInt();
    colY=table->colgraphGetGraphProperty(pid, gid, "FIT_COLY", colY).toInt();
    colW=table->colgraphGetGraphProperty(pid, gid, "FIT_COLW", colW).toInt();
    ui->cmbFitType->setCurrentIndex(ui->cmbFitType->findData(table->colgraphGetGraphProperty(pid, gid, "FIT_METHOD", ui->cmbFitType->itemData(ui->cmbFitType->currentIndex()).toInt()).toInt()));
    ui->datacut->set_userMin(table->colgraphGetGraphProperty(pid, gid, "FIT_CUTLOW", ui->datacut->get_userMin()).toDouble());
    ui->datacut->set_userMax(table->colgraphGetGraphProperty(pid, gid, "FIT_CUTHIGH", ui->datacut->get_userMax()).toDouble());
    bool logX=false;
    ui->chkLogX->setChecked(logX=table->colgraphGetGraphProperty(pid, gid, "FIT_LOGX", ui->chkLogX->isChecked()).toBool());
    ui->chkLogY->setChecked(table->colgraphGetGraphProperty(pid, gid, "FIT_LOGY", ui->chkLogY->isChecked()).toBool());
    ui->datacut->setLogScale(logX, 20);

    int savedToColumn=table->colgraphGetGraphProperty(pid, gid, "FIT_SAVEDTOCOLUMN", -1).toInt();
    QList<QVariant> fixparams=table->colgraphGetGraphProperty(pid, gid, "FIT_FIXEDPARAMS").toList();
    QList<QVariant> errorparams=table->colgraphGetGraphProperty(pid, gid, "FIT_ERRORPARAMS").toList();

    fillFitParams(ui->cmbFitType->itemData(ui->cmbFitType->currentIndex()).toInt());
    if (savedToColumn>=0)  {
        QList<QVariant> oldparams=table->tableGetColumnData(savedToColumn);
        QList<QVariant> oldparams1;
        if (savedToColumn+1<table->tableGetColumnCount()) oldparams1=table->tableGetColumnData(savedToColumn+1);
        for (int i=0; i<parameterNames.size(); i++) {
            paramMap[parameterNames[i]].value=oldparams.value(i, 0).toDouble();
            paramMap[parameterNames[i]].error=oldparams1.value(i, 0).toDouble();
            paramMap[parameterNames[i]].fix=fixparams.value(i, false).toBool();
        }
    } else {
        if (pid>=0 && pid<table->getPlotCount()) {
            const QFRDRTable::PlotInfo pi=table->getPlot(pid);
            if (gid>=0 && gid<pi.graphs.size()) {
                for (int i=0; i<parameterNames.size(); i++) {
                    paramMap[parameterNames[i]].value=pi.graphs[gid].functionParameters.value(i, 0);
                    paramMap[parameterNames[i]].error=errorparams.value(i, 0).toDouble();
                    paramMap[parameterNames[i]].fix=fixparams.value(i, false).toBool();
                }

            }
        }
    }

    if (addGraph) *addGraph=gid;
    if (resultColumn) *resultColumn=savedToColumn;

    readDataFromTable();
}

void QFRDRTableRegressionDialog::intInit(QFRDRTable *table, int colX, int colY, int colW, QWidget *parent, bool logX, bool logY, int resultColumn, int addGraph, bool propsAlreadySet)
{
    this->colX=colX;
    this->colY=colY;
    this->colW=colW;
    this->table=table;
    ui->chkLogX->setChecked(logX);
    ui->chkLogY->setChecked(logY);


    ui->cmbAddGraph->clear();
    ui->cmbAddGraph->addItem(tr("--- no ---"));
    ui->cmbAddGraph->addItem(tr("--- add new graph ---"));
    for (int i=0; i<table->colgraphGetPlotCount(); i++) {
        ui->cmbAddGraph->addItem(tr("add to: %1").arg(table->colgraphGetPlotTitle(i)));
    }
    ui->cmbAddGraph->addItem(tr("--- overwrite old graph ---"));
    ui->cmbSaveColumn->clear();
    ui->cmbSaveColumn->addItem(tr("--- no ---"));
    ui->cmbSaveColumn->addItem(tr("--- add new column ---"));
    for (unsigned int i=0; i<table->colgraphGetPlotCount(); i++) {
        ui->cmbSaveColumn->addItem(tr("save to column %1 [%2]").arg(i+1).arg(table->tableGetColumnTitle(i)));
    }

    ui->pltDistribution->getXAxis()->set_axisLabel(tr("x-data [%1: %2]").arg(colX+1).arg(table->tableGetColumnTitle(colX)));
    ui->pltDistribution->getYAxis()->set_axisLabel(tr("y-data [%1: %2]").arg(colY+1).arg(table->tableGetColumnTitle(colY)));


    if (addGraph<0) {
        ui->cmbAddGraph->setCurrentIndex(0);
    } else {
        ui->cmbAddGraph->setCurrentIndex(addGraph+2);
    }
    if (overwriteGraph>=0) {
        ui->cmbAddGraph->setCurrentIndex(ui->cmbAddGraph->count()-1);
        //qDebug()<<"cmbAddGraph: "<<ui->cmbAddGraph->currentIndex();
    }

    if (resultColumn<0) {
        ui->cmbSaveColumn->setCurrentIndex(0);
    } else {
        ui->cmbSaveColumn->setCurrentIndex(resultColumn+2);
    }

    readDataFromTable();

    ui->datacut->setLogScale(logX, 20);

    paramMap["a"].value=0;
    paramMap["a"].fix=false;
    paramMap["b"].value=1;
    paramMap["b"].fix=false;
    paramMap["iterations"].value=100;
    paramMap["irls_param"].value=1.1;
    paramMap["R2"].value=0;
    paramMap["R2"].editable=false;

    ui->tabParams->setItemDelegate(new QFFitFunctionValueInputDelegate(ui->tabParams));

    ui->tabParams->setModel(parameterTable);

    ui->cmbFitType->addItem(tr("linear regression"), 0);
    if (dataW.size()==datapoints) ui->cmbFitType->addItem(tr("linear regression, weighted"), 1);
    ui->cmbFitType->addItem(tr("linear regression, robust"), 2);
    ui->cmbFitType->addItem(tr("power-law regression"), 3);
    //if (dataW.size()==datapoints) ui->cmbFitType->addItem(tr("power-law regression, weighted"), 4);
    ui->cmbFitType->addItem(tr("power-law regression, robust"), 4);

    if (!propsAlreadySet) {
        ui->cmbFitType->setCurrentIndex(0);
    }






    ui->pltResiduals->set_displayToolbar(false);
    ui->pltResiduals->getXAxis()->set_axisLabel(tr("X"));
    //ui->pltResiduals->getXAxis()->set_logAxis(true);
    ui->pltResiduals->getYAxis()->set_axisLabel(tr("residuals"));
    ui->pltDistribution->getXAxis()->set_axisLabel("");
    ui->pltDistribution->getYAxis()->set_axisLabel(tr("Y"));
    ui->pltDistribution->getXAxis()->set_drawMode1(JKQTPCADMticks);
    ui->pltResiduals->getXAxis()->set_drawMode1(JKQTPCADMcomplete);
    ui->pltResiduals->get_plotter()->setBorder(1,1,1,1);
    ui->pltDistribution->get_plotter()->setBorder(1,1,1,1);
    ui->pltResiduals->synchronizeToMaster(ui->pltDistribution, true, false);
    ui->pltDistribution->get_plotter()->set_useAntiAliasingForSystem(true);
    ui->pltDistribution->get_plotter()->set_useAntiAliasingForGraphs(true);
    ui->pltResiduals->get_plotter()->set_useAntiAliasingForSystem(true);
    ui->pltResiduals->get_plotter()->set_useAntiAliasingForGraphs(true);
    ui->pltResiduals->set_displayMousePosition(false);
    ui->pltDistribution->set_displayMousePosition(false);
    ui->pltDistribution->get_plotter()->set_keyFontSize(9);
    ui->pltResiduals->get_plotter()->set_keyFontSize(9);
    ui->pltResiduals->useExternalDatastore(ui->pltDistribution->getDatastore());
    ui->pltResiduals->setMinimumHeight(75);
    ui->pltResiduals->get_plotter()->set_showKey(false);
    ui->pltDistribution->setMinimumHeight(75);
    ui->pltDistribution->get_plotter()->setBorder(1,1,1,1);
    ui->pltDistribution->get_plotter()->set_useAntiAliasingForSystem(true);
    ui->pltDistribution->get_plotter()->set_useAntiAliasingForGraphs(true);
    ui->pltDistribution->get_plotter()->set_keyFontSize(9);
    ui->pltDistribution->get_plotter()->set_keyPosition(JKQTPkeyInsideTopLeft);
    QColor cb("white");
    cb.setAlphaF(0.5);
    ui->pltDistribution->get_plotter()->set_keyBackgroundColor(cb);
    ui->pltDistribution->get_plotter()->set_keyPosition(JKQTPkeyInsideTopLeft);

    connect(ui->pltDistribution->get_plotter()->get_actZoomAll(), SIGNAL(triggered()), ui->pltResiduals, SLOT(zoomToFit()));
    connect(ui->pltResiduals->get_plotter(), SIGNAL(zoomChangedLocally(double,double,double,double,JKQtBasePlotter*)), ui->pltDistribution, SLOT(pzoomChangedLocally(double,double,double,double,JKQtBasePlotter*)));
    connect(ui->pltDistribution->get_plotter(), SIGNAL(zoomChangedLocally(double,double,double,double,JKQtBasePlotter*)), ui->pltResiduals, SLOT(pzoomChangedLocally(double,double,double,double,JKQtBasePlotter*)));




    QSettings* set=ProgramOptions::getInstance()->getQSettings();
    if (set && !propsAlreadySet) {
        loadWidgetGeometry(*set, this, pos(), size(), "QFRDRTableRegressionDialog/windowsize");
        ui->chkPlotErrors->setChecked(set->value("QFRDRTableRegressionDialog/ploterrors", ui->chkPlotErrors->isChecked()).toBool());
        ui->chkWeightedResiduals->setChecked(set->value("QFRDRTableRegressionDialog/weightedresiduals", ui->chkWeightedResiduals->isChecked()).toBool());
        ui->cmbFitType->setCurrentIndex(set->value("QFRDRTableRegressionDialog/method", ui->cmbFitType->currentIndex()).toInt());
        loadSplitter(*set, ui->splitter, "QFRDRTableRegressionDialog/splitter/");
    }



    methodChanged(ui->cmbFitType->currentIndex());
    connectSignals(true);
    on_btnFit_clicked();
}

