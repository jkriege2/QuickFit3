#include "qfpevalimfcsfit.h"
#include <QtGui>
#include "qfimfcsfitevaluation.h"
#include "imfcscalibrationdialog.h"
#include "optionswidget.h"
#include "qfrdrtableinterface.h"
#include "qftools.h"
#include "qfrdrcolumngraphsinterface.h"
#include "qfrdrrunselection.h"
#include "qfevaluationeditor.h"
#include "qfimfcsfitevaluationeditor.h"

QFPEvalIMFCSFit::QFPEvalIMFCSFit(QObject* parent):
    QObject(parent)
{
    //ctor
    QFPluginServices::getInstance()->registerSettingsPane(this);
    menuCalibration=NULL;
}

QFPEvalIMFCSFit::~QFPEvalIMFCSFit()
{
    //dtor
}

QFEvaluationItem* QFPEvalIMFCSFit::createRecord(QFProject* parent) {
    return new QFImFCSFitEvaluation(parent);
}


void QFPEvalIMFCSFit::registerToMenu(QMenu* menu) {
    QAction* actFCS=new QAction(QIcon(":/imfcs_fit.png"), tr("imFCS Curve Fitting"), parentWidget);
    actFCS->setStatusTip(tr("Insert a new imFCS least-squares fit evaluation"));
    connect(actFCS, SIGNAL(triggered()), this, SLOT(insertFCSFit()));
    menu->addAction(actFCS);

    /*QAction* actFCSCalib=new QAction(QIcon(":/imfcsfit/imfcs_fitcalib.png"), tr("imFCS Calibration"), parentWidget);
    actFCSCalib->setStatusTip(tr("Insert a set of imFCS least-squares fit evaluations for a SPIM calibration"));
    connect(actFCSCalib, SIGNAL(triggered()), this, SLOT(insertFCSFitForCalibration()));
    menu->addAction(actFCSCalib);*/
    menu->addMenu(menuCalibration);


}

void QFPEvalIMFCSFit::init()
{
    if (services) {
        QMenu* menu=new QMenu(tr("imFCS &Calibration Tool"));
        menu->setIcon(QIcon(":/imfcsfit/imfcs_fitcalib.png"));

        QAction* actHelp=new QAction(QIcon(":/lib/help.png"), tr("Calbration Tutorial"), this);
        connect(actHelp, SIGNAL(triggered()), this, SLOT(showCalibrationTutorial()));
        menu->addAction(actHelp);
        menu->addSeparator();

        QAction* actFCSCalib=new QAction(QIcon(":/imfcsfit/imfcs_fitcalib.png"), tr("&0: add imFCS Calibration Fits"), this);
        actFCSCalib->setStatusTip(tr("Insert a set of imFCS least-squares fit evaluations for a SPIM calibration"));
        connect(actFCSCalib, SIGNAL(triggered()), this, SLOT(insertFCSFitForCalibration()));
        menu->addAction(actFCSCalib);

        QAction* actImFCSCalibFit=new QAction(tr("&1: Fit D's"), this);
        menu->addAction(actImFCSCalibFit);
        connect(actImFCSCalibFit, SIGNAL(triggered()), this, SLOT(imFCSCalibrationTool1()));
        QAction* actImFCSCalib=new QAction(tr("&2: Collect D Data"), this);
        menu->addAction(actImFCSCalib);
        connect(actImFCSCalib, SIGNAL(triggered()), this, SLOT(imFCSCalibrationTool2()));
        QAction* actImFCSCalib2=new QAction(tr("&3: Collect wxy Data"), this);
        menu->addAction(actImFCSCalib2);
        connect(actImFCSCalib2, SIGNAL(triggered()), this, SLOT(imFCSCalibrationTool3()));

        QMenu* extm=services->getMenu("tools");
        if (extm) {
            //extm->addAction(actImFCSCalib);
            extm->addMenu(menu);
        }
        menuCalibration=menu;
    }
}

QString QFPEvalIMFCSFit::pluginOptionsName() const
{
    return getName();
}

QIcon QFPEvalIMFCSFit::pluginOptionsIcon() const
{
    return QIcon(getIconFilename());
}

QFPluginOptionsWidget *QFPEvalIMFCSFit::createOptionsWidget(QWidget *parent)
{
    return new OptionsWidget(this, parent);
}



void QFPEvalIMFCSFit::insertFCSFit() {
    if (project) {
        project->addEvaluation(getID(), "imFCS Fit");
    }
}

void QFPEvalIMFCSFit::insertFCSFitForCalibration() {
    if (project) {

        ImFCSCalibrationDialog* dlg=new ImFCSCalibrationDialog(NULL);
        QFEvaluationItem* edummy=project->addEvaluation(getID(), "imFCS Fit");
        QFImFCSFitEvaluation* imFCS=qobject_cast<QFImFCSFitEvaluation*>(edummy);
        if (imFCS) dlg->setFitModels(imFCS->getAvailableFitFunctions(), imFCS->getFitFunctionID());
        if (dlg->exec()) {
            QList<double> vals=dlg->getValues();
            for (int i=0; i<vals.size(); i++) {
                QFEvaluationItem* e=edummy;
                if (i>0) e=project->addEvaluation(getID(), "imFCS Fit");
                QFImFCSFitEvaluation* eimFCS=qobject_cast<QFImFCSFitEvaluation*>(e);
                e->setQFProperty("PRESET_FOCUS_WIDTH", vals[i], false, false);
                e->setQFProperty("PRESET_FOCUS_HEIGHT", dlg->getFocusHeight(), false, false);
                e->setQFProperty("PRESET_FOCUS_HEIGHT_ERROR", dlg->getFocusHeightError(), false, false);
                e->setQFProperty("PRESET_FOCUS_WIDTH_FIX", true, false, false);
                e->setQFProperty("PRESET_FOCUS_HEIGHT_FIX", true, false, false);
                e->setQFProperty("PRESET_D1_FIX", false, false, false);
                e->setQFProperty("IMFCS_CALIBRATION_FOCUSWIDTH", vals[i], false, false);
                e->setQFProperty("IMFCS_CALIBRATION_FOCUSHEIGHT", dlg->getFocusHeight(), false, false);
                e->setQFProperty("IMFCS_CALIBRATION_FOCUSHEIGHT_ERROR", dlg->getFocusHeightError(), false, false);
                e->setQFProperty("IMFCS_CALIBRATION_MODEL", dlg->getFitModel(), false, false);
                e->setName(tr("wxy=%1 nm").arg(vals[i]));
                if (eimFCS) eimFCS->setFitFunction(dlg->getFitModel());
            }
            if (vals.size()<=0) delete edummy;
            QFRawDataRecord* et=project->addRawData("table", "imFCS Calibration results");
            et->setQFProperty("IMFCS_CALIBRATION_RESULTTABLE", true, false, false);

         } else {
            delete edummy;
        }
        delete dlg;
    }
}

void QFPEvalIMFCSFit::imFCSCalibrationTool1()
{
    if (!project || !services) {
        QMessageBox::critical(parentWidget, tr("imFCS Calibration"), tr("No project loaded!"));
        return;
    }
    log_text(tr("imFCS calibration tool 1: fitting D's ... \n"));
    for (int i=0; i<project->getEvaluationCount(); i++) {
        QFEvaluationItem* e=project->getEvaluationByNum(i);
        QFImFCSFitEvaluation* imFCS=qobject_cast<QFImFCSFitEvaluation*>(e);
        if (imFCS) {
            log_text(tr("   - fitting %1 \n").arg(e->getName()));
            QFEvaluationPropertyEditor* pedt=services->openEvaluationEditor(e);
            QFEvaluationEditor* edt=pedt->getEditor();
            QFImFCSFitEvaluationEditor* eedt=qobject_cast<QFImFCSFitEvaluationEditor*>(edt);
            if (eedt && e->getName().toLower().contains("wxy")) {
                eedt->fitEverythingThreaded();
            }
            if (pedt) pedt->close();
            log_text(tr("        DONE!\n"));
        }
    }
    log_text(tr("imFCS calibration tool 1: fitting D's ... DONE!\n"));
}

void QFPEvalIMFCSFit::imFCSCalibrationTool2()
{
    if (!project)  {
        QMessageBox::critical(parentWidget, tr("imFCS Calibration"), tr("No project loaded!"));
        return;
    }
     log_text(tr("imFCS calibration tool 2: collecting D data ... \n"));

    // find table for resultsQFRDRRunSelectionsInterface
    QFRawDataRecord* etab=NULL;
    QFRDRTableInterface* tab=NULL;
    for (int i=0; i<project->getRawDataCount(); i++) {
        QFRawDataRecord* e=project->getRawDataByNum(i);
        QFRDRTableInterface* eetab=qobject_cast<QFRDRTableInterface*>(e);
        //qDebug()<<i<<e<<eetab;
        if (e && eetab) {
            if (e->getProperty("IMFCS_CALIBRATION_RESULTTABLE", false).toBool()) {
                etab=e;
                tab=eetab;
            }
        }
    }
    if (!etab || !tab) {
        etab=project->addRawData("table", "imFCS Calibration results");
        if (etab) {
            etab->setQFProperty("IMFCS_CALIBRATION_RESULTTABLE", true, false, false);
        }
        tab=qobject_cast<QFRDRTableInterface*>(etab);
        //qDebug()<<"create: "<<etab<<tab;
    }
    if (!etab || !tab) {
        QMessageBox::critical(parentWidget, tr("imFCS Calibration"), tr("Could not create or find proper results table in project!"));
        return;
    }
    QFRDRColumnGraphsInterface* graph=qobject_cast<QFRDRColumnGraphsInterface*>(etab);

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    tab->tableSetColumnTitle(0, tr("pixel size [nm]"));
    int counter=0;
    bool first=true;
    QList<QFRawDataRecord*> rdrs;
    QList<double> focus_widths, Ds, Ds2, pixel_sizes;
    QString model="";
    double wz=0;
    double ewz=0;
    double focus_height=0;
    double xmin=0;
    double xmax=0;
    double ymin=0;
    double ymax=0;
    for (int i=0; i<project->getEvaluationCount(); i++) {
        QFEvaluationItem* e=project->getEvaluationByNum(i);
        QFImFCSFitEvaluation* imFCS=qobject_cast<QFImFCSFitEvaluation*>(e);
        if (e) {
            double fw=e->getProperty("IMFCS_CALIBRATION_FOCUSWIDTH", -1.0).toDouble();
            QString colName=tr("D%1 [µm²/s]").arg(round(fw));
            QString colNameE=tr("D%1_error [µm²/s]").arg(round(fw));
            if (fw>0) {
                wz=focus_height=e->getProperty("PRESET_FOCUS_HEIGHT", 0).toDouble();
                ewz=e->getProperty("PRESET_FOCUS_HEIGHT_ERROR", 0).toDouble();
                model=e->getProperty("IMFCS_CALIBRATION_MODEL", "").toString();
                focus_widths<<fw;
                if (first) {
                    // build list of fit RDRs
                    int rcounter=0;
                    for (int ri=0; ri<project->getRawDataCount(); ri++) {
                        QFRawDataRecord* r=project->getRawDataByNum(ri);
                        if (imFCS->isFilteredAndApplicable(r)) {
                            rdrs<<r;
                            double bin=r->getProperty("BINNING", 1.0).toDouble();
                            double width=r->getProperty("PIXEL_WIDTH", 0).toDouble();


                            //tab->tableSetData(rcounter, 0, imFCS->getFitValue(r, imFCS->getEvaluationResultID(-1), "pixel_size"));
                            tab->tableSetData(rcounter, 0, bin*width);
                            if (bin*width>xmax) xmax=1.1*bin*width;

                            rcounter++;
                            Ds<<0;
                            Ds2<<0;
                            pixel_sizes<<bin*width;
                        }
                    }
                }
                first=false;

                for (int ri=0; ri<rdrs.size(); ri++) {
                    QFRawDataRecord* r=rdrs[ri];
                    QString erid=imFCS->getEvaluationResultID(0);
                    QString en=imFCS->transformResultID(erid);
                    //qDebug()<<r->getName()<<":  "<<en<<"    ["<<erid<<"]"<<r->resultsExistsFromEvaluation(en)<<r->resultsExists(en, "diff_coeff1")<<r->resultsCalcNames(en)<<r->resultsCalcNames(erid);
                    QVector<double> D=r->resultsGetAsDoubleList(en,  "fitparam_diff_coeff1");

                    QFRDRRunSelectionsInterface* runsel=qobject_cast<QFRDRRunSelectionsInterface*>(r);
                    if (runsel) {
                        QVector<double> DD;
                        for (int di=D.size()-1; di>=0; di--) {
                            if (!runsel->leaveoutRun(di)) DD<<D[di];
                        }
                        D=DD;
                    }
                    qSort(D);
                    double dmin=qfstatisticsSortedMin(D);
                    double dmax=qfstatisticsSortedMax(D);

                    if (dmax>ymax) ymax=dmax*1.1;
                    if (dmin<ymin) ymin=dmin*1.1;

                    double Dvar=0;
                    double Dmean=qfstatisticsAverageVariance(Dvar, D);
                    tab->tableSetData(ri, 1+counter*2, Dmean);
                    tab->tableSetData(ri, 1+counter*2+1, sqrt(Dvar));
                    Ds[ri]=Ds[ri]+Dmean;
                    Ds2[ri]=Ds2[ri]+Dmean*Dmean;
                }

                tab->tableSetColumnTitle(1+counter*2, colName);
                tab->tableSetColumnTitle(1+counter*2+1, colNameE);

                counter++;
            }
        }
    }


    if (graph) {
        int i=0;
        while (graph->colgraphGetGraphCount()<1 && i<1) {
            graph->colgraphAddGraph("title");
            i++;
        }
        graph->colgraphSetGraphTitle(0, tr("pixel size vs. diffusion coefficient"));
        graph->colgraphSetGraphXAxisProps(0, tr("pixel size [nm]"));
        graph->colgraphSetGraphYAxisProps(0, tr("diffusion coefficient [µm²/s]"));
        while (graph->colgraphGetPlotCount(0)>0){
            graph->colgraphRemovePlot(0, 0);
        }
        for (i=0; i<focus_widths.size(); i++) {
            graph->colgraphAddPlot(0, 0, 1+i*2, QFRDRColumnGraphsInterface::cgtLinesPoints, tr("w_{xy}=%1nm").arg(focus_widths[i]));
        }
        graph->colgraphsetXRange(0, xmin, xmax);
        graph->colgraphsetYRange(0, ymin, ymax);


    }

    QApplication::restoreOverrideCursor();


    QStringList DsNames;
    for (int ri=0; ri<Ds.size(); ri++) {
        if (counter>1) Ds2[ri]=sqrt((Ds2[ri]-Ds[ri]*Ds[ri]/double(counter))/double(counter-1));
        else Ds2[ri]=0;
        Ds[ri]=Ds[ri]/double(counter);
        DsNames<<tr("a=%1nm:  D= (%2 +/- %3) µm²/s").arg(pixel_sizes[ri]).arg(Ds[ri]).arg(Ds2[ri]);
    }

    bool dlgOK=false;
    int DItem=DsNames.indexOf(QInputDialog::getItem(parentWidget, tr("imFCS Calibration"), tr("Please choose the binning at which to measure the \"true\" diffusion coefficient for the calibration:"), DsNames, 4, false, &dlgOK));

    if (dlgOK) {

        double Dcalib=Ds.value(DItem, 0);
        double DcalibE=Ds2.value(DItem, 0);
        QFEvaluationItem* e=project->addEvaluation(getID(), tr("calibration D=%1µm²/s").arg(Dcalib));
        e->setQFProperty("PRESET_D1", Dcalib, false, false);
        e->setQFProperty("PRESET_D1_ERROR", DcalibE, false, false);
        e->setQFProperty("PRESET_D1_FIX", true, false, false);
        e->setQFProperty("PRESET_FOCUS_WIDTH_FIX", false, false, false);
        e->setQFProperty("PRESET_FOCUS_HEIGHT", focus_height, false, false);
        e->setQFProperty("PRESET_FOCUS_HEIGHT_ERROR", ewz, false, false);
        e->setQFProperty("IMFCS_CALIBRATION_D", Dcalib, false, false);
        e->setQFProperty("IMFCS_CALIBRATION_D_ERROR", DcalibE, false, false);
        e->setQFProperty("IMFCS_CALIBRATION_FITWXY", true, false, false);
        e->setName(tr("calibration D=%1µm²/s").arg(Dcalib));
        QFImFCSFitEvaluation* eimFCS=qobject_cast<QFImFCSFitEvaluation*>(e);
        if (eimFCS && (!model.isEmpty())) eimFCS->setFitFunction(model);
    }

    log_text(tr("imFCS calibration tool 2: collecting D data ... DONE!\n"));
}

void QFPEvalIMFCSFit::imFCSCalibrationTool3()
{
    if (!project)  {
        QMessageBox::critical(parentWidget, tr("imFCS Calibration"), tr("No project loaded!"));
        return;
    }
    log_text(tr("imFCS calibration tool 3: collecting wxy data ... \n"));

    // find table for resultsQFRDRRunSelectionsInterface
    QFRawDataRecord* etab=NULL;
    QFRDRTableInterface* tab=NULL;
    for (int i=0; i<project->getRawDataCount(); i++) {
        QFRawDataRecord* e=project->getRawDataByNum(i);
        QFRDRTableInterface* eetab=qobject_cast<QFRDRTableInterface*>(e);
        //qDebug()<<i<<e<<eetab;
        if (e && eetab) {
            if (e->getProperty("IMFCS_CALIBRATION_RESULTTABLE", false).toBool()) {
                etab=e;
                tab=eetab;
            }
        }
    }
    if (!etab || !tab) {
        etab=project->addRawData("table", "imFCS Calibration results");
        if (etab) {
            etab->setQFProperty("IMFCS_CALIBRATION_RESULTTABLE", true, false, false);
        }
        tab=qobject_cast<QFRDRTableInterface*>(etab);
        //qDebug()<<"create: "<<etab<<tab;
    }
    if (!etab || !tab) {
        QMessageBox::critical(parentWidget, tr("imFCS Calibration"), tr("Could not create or find proper results table in project!"));
        return;
    }
    QFRDRColumnGraphsInterface* graph=qobject_cast<QFRDRColumnGraphsInterface*>(etab);

    QList<QFRawDataRecord*> rdrs;
    double xmin=0;
    double xmax=0;
    double ymin=0;
    double ymax=0;
    double Dcalib=0;
    double DcalibE=0;
    double wxymean=0, wxyvar=0;
    for (int i=0; i<project->getEvaluationCount(); i++) {
        QFEvaluationItem* e=project->getEvaluationByNum(i);
        QFImFCSFitEvaluation* imFCS=qobject_cast<QFImFCSFitEvaluation*>(e);
        if (e) {
            bool fitwxy=e->getProperty("IMFCS_CALIBRATION_FITWXY", false).toBool();
            Dcalib=e->getProperty("IMFCS_CALIBRATION_D", Dcalib).toDouble();
            DcalibE=e->getProperty("IMFCS_CALIBRATION_D_ERROR", DcalibE).toDouble();
            QString colName=tr("wxy [nm]");
            QString colNameE=tr("wxy_error [nm]");
            if (fitwxy) {
                int cols=tab->tableGetColumnCount();
                //qDebug()<<"cols="<<cols;
                int rcounter=0;
                tab->tableSetColumnTitle(cols, tr("pixel size [nm]"));
                tab->tableSetColumnTitle(cols+1, colName);
                tab->tableSetColumnTitle(cols+2, colNameE);
                QVector<double> wxyvec;
                for (int ri=0; ri<project->getRawDataCount(); ri++) {
                    QFRawDataRecord* r=project->getRawDataByNum(ri);
                    if (imFCS->isFilteredAndApplicable(r)) {
                        rdrs<<r;
                        double bin=r->getProperty("BINNING", 1.0).toDouble();
                        double width=r->getProperty("PIXEL_WIDTH", 0).toDouble();

                        tab->tableSetData(rcounter, cols, bin*width);
                        if (bin*width>xmax) xmax=1.1*bin*width;


                        QString erid=imFCS->getEvaluationResultID(0);
                        QString en=imFCS->transformResultID(erid);
                        QVector<double> wxy=r->resultsGetAsDoubleList(en,  "fitparam_focus_width");
                        //if (r->resultsExists(en,  "fitparam_diff_coeff1"))  Dcalib=r->resultsGetAsDouble(en,  "fitparam_diff_coeff1");

                        QFRDRRunSelectionsInterface* runsel=qobject_cast<QFRDRRunSelectionsInterface*>(r);
                        if (runsel) {
                            QVector<double> DD;
                            for (int di=wxy.size()-1; di>=0; di--) {
                                if (!runsel->leaveoutRun(di)) DD<<wxy[di];
                            }
                            wxy=DD;
                        }
                        /*qSort(wxy);
                        double dmin=qfstatisticsSortedMin(wxy);
                        double dmax=qfstatisticsSortedMax(wxy);*/


                        wxyvar=0;
                        wxymean=qfstatisticsAverageVariance(wxyvar, wxy);
                        tab->tableSetData(rcounter, cols+1, wxymean);
                        tab->tableSetData(rcounter, cols+2, sqrt(wxyvar));
                        wxyvec<<wxymean;
                        //qDebug()<<rcounter<<cols+1<<wxymean<<sqrt(wxyvar);

                        if (wxymean+sqrt(wxyvar)>ymax) ymax=(wxymean+sqrt(wxyvar))*1.1;
                        if (wxymean-sqrt(wxyvar)<ymin) ymin=(wxymean-sqrt(wxyvar));

                        rcounter++;
                    }
                }
                if (rcounter>0) {
                    tab->tableSetData(0, cols+3, qfstatisticsAverage(wxyvec));
                    tab->tableSetData(0, cols+4, sqrt(qfstatisticsVariance(wxyvec)));
                    tab->tableSetData(rcounter-1, cols+3, qfstatisticsAverage(wxyvec));
                    tab->tableSetData(rcounter-1, cols+4, sqrt(qfstatisticsVariance(wxyvec)));
                    if (graph) {
                        graph->colgraphAddGraph(tr("pixel size vs. lat. focus size, D=%2µm²/s").arg(Dcalib));
                        int ggraph=graph->colgraphGetGraphCount()-1;
                        graph->colgraphSetGraphTitle(ggraph, tr("pixel size vs. lat. focus size"));
                        graph->colgraphSetGraphXAxisProps(ggraph, tr("pixel size [nm]"));
                        graph->colgraphSetGraphYAxisProps(ggraph, tr("lateral focus size w_{xy} [nm]"));
                        graph->colgraphAddErrorPlot(ggraph, cols, -1, cols+1, cols+2, QFRDRColumnGraphsInterface::cgtLinesPoints, tr("calibration D=(%1\\pm %2)µm²/s").arg(Dcalib).arg(DcalibE));
                        graph->colgraphSetPlotColor(ggraph, graph->colgraphGetPlotCount(ggraph)-1, QColor("red"));
                        graph->colgraphAddPlot(ggraph, cols, cols+3, QFRDRColumnGraphsInterface::cgtLines, tr("w_{xy}=(%1\\pm %2)nm").arg(qfstatisticsAverage(wxyvec)).arg(sqrt(qfstatisticsVariance(wxyvec))));
                        graph->colgraphSetPlotColor(ggraph, graph->colgraphGetPlotCount(ggraph)-1, QColor("blue"));
                        graph->colgraphsetXRange(ggraph, xmin, xmax);
                        graph->colgraphsetYRange(ggraph, ymin, ymax);

                    }
                }
            }
        }


    }

    log_text(tr("imFCS calibration tool 3: collecting wxy data ... DONE!\n"));

}

void QFPEvalIMFCSFit::showCalibrationTutorial()
{
    if (services) {
        services->displayHelpWindow(services->getPluginHelpDirectory(getID())+"/calibration.html");
    }
}


Q_EXPORT_PLUGIN2(imfcsfit, QFPEvalIMFCSFit)
