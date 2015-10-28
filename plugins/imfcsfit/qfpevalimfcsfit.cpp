/*
    Copyright (c) 2008-2015 Jan W. Krieger, German Cancer Research Center

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfpevalimfcsfit.h"
#include<QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include "qfimfcsfitevaluation.h"
#include "imfcscalibrationdialog.h"
#include "optionswidget.h"
#include "qfrdrtableinterface.h"
#include "qftools.h"
#include "qfrdrcolumngraphsinterface.h"
#include "qfrdrrunselection.h"
#include "qfevaluationeditor.h"
#include "qfimfcsfitevaluationeditor.h"
#include "qfrdrimfcscorrelatorremote.h"
#include "qfrawdatarecordfactory.h"


QFPEvalIMFCSFit::QFPEvalIMFCSFit(QObject* parent):
    QObject(parent)
{
    //ctor
    QFPluginServices::getInstance()->registerSettingsPane(this);
    menuCalibration=NULL;
    calibrationWizard=NULL;
}

QFPEvalIMFCSFit::~QFPEvalIMFCSFit()
{
    //dtor
    if (calibrationWizard) {
        calibrationWizard->close();
        delete calibrationWizard;
    }
}

QFEvaluationItem* QFPEvalIMFCSFit::createRecord(QFProject* parent) {
    return new QFImFCSFitEvaluation(parent);
}


void QFPEvalIMFCSFit::registerToMenu(QMenu* menu) {
    QAction* actFCS=new QFActionWithNoMenuRole(QIcon(":/imfcs_fit.png"), tr("imFCS Curve Fitting"), parentWidget);
    actFCS->setStatusTip(tr("Insert a new imFCS least-squares fit evaluation"));
    connect(actFCS, SIGNAL(triggered()), this, SLOT(insertFCSFit()));
    menu->addAction(actFCS);

    menu->addMenu(menuCalibration);


}

void QFPEvalIMFCSFit::init()
{
    if (services) {
        QMenu* menu=new QMenu(tr("imFCS &Calibration Tool"), parentWidget);
        menu->setIcon(QIcon(":/imfcsfit/imfcs_fitcalib.png"));

        QAction* actHelp=new QFActionWithNoMenuRole(QIcon(":/lib/help.png"), tr("Calibration Tutorial"), this);
        connect(actHelp, SIGNAL(triggered()), this, SLOT(showCalibrationTutorial()));
        menu->addAction(actHelp);
        QAction* actWizard=new QFActionWithNoMenuRole(QIcon(":/imfcsfit/imfcs_fitcalib.png"), tr("Calibration Wizard"), this);
        connect(actWizard, SIGNAL(triggered()), this, SLOT(insertFCSCalibrationWizard()));
        menu->addAction(actWizard);
        QMenu* menu1=menu->addMenu("steps ...");

        QAction* actFCSCalib=new QFActionWithNoMenuRole(QIcon(":/imfcsfit/imfcs_calib_addfiles.png"), tr("&0: add imFCS Calibration Fits"), this);
        actFCSCalib->setStatusTip(tr("Insert a set of imFCS least-squares fit evaluations for a SPIM calibration"));
        connect(actFCSCalib, SIGNAL(triggered()), this, SLOT(insertFCSFitForCalibration()));
        menu1->addAction(actFCSCalib);

        QAction* actImFCSCalibFit=new QFActionWithNoMenuRole(tr("&1: Fit D's"), this);
        menu1->addAction(actImFCSCalibFit);
        connect(actImFCSCalibFit, SIGNAL(triggered()), this, SLOT(imFCSCalibrationTool1()));
        QAction* actImFCSCalib=new QFActionWithNoMenuRole(tr("&2: Collect D Data"), this);
        menu1->addAction(actImFCSCalib);
        connect(actImFCSCalib, SIGNAL(triggered()), this, SLOT(imFCSCalibrationTool2()));
        QAction* actImFCSCalib1=new QFActionWithNoMenuRole(tr("&3: Fit wxy"), this);
        menu1->addAction(actImFCSCalib1);
        connect(actImFCSCalib1, SIGNAL(triggered()), this, SLOT(imFCSCalibrationTool3()));
        QAction* actImFCSCalib2=new QFActionWithNoMenuRole(tr("&4: Collect wxy Data"), this);
        menu1->addAction(actImFCSCalib2);
        connect(actImFCSCalib2, SIGNAL(triggered()), this, SLOT(imFCSCalibrationTool4()));

        QMenu* extm=services->getMenu("tools");
        if (extm) {
            //extm->addAction(actImFCSCalib);
            extm->addMenu(menu);

        }
        menuCalibration=menu;
        services->registerWizard("eval_wizards", actWizard);
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

QVariant QFPEvalIMFCSFit::sendPluginCommand(const QString &command, const QVariant &param1, const QVariant &param2, const QVariant &param3, const QVariant &param4, const QVariant &param5)
{
    if (command.trimmed().toLower()=="run_calibration") {
        insertFCSCalibrationWizard();
        QList<double> vals;
        QVariantList vlist=param5.toList();
        for (int i=0; i<vlist.size(); i++) {
            vals<<vlist[i].toDouble();
        }
        insertFCSFitForCalibration(NULL, param1.toDouble(), param2.toDouble(), param3.toBool(), param4.toString(), vals);
        calibrationWizard->enableStep2();
        calibrationWizard->hideStep01(true);
        return QVariant(true);
    }
    return QVariant(false);
}



void QFPEvalIMFCSFit::insertFCSFit() {
    if (project) {
        project->addEvaluation(getID(), "imFCS Fit");
    }
}

void QFPEvalIMFCSFit::insertFCSCalibrationWizard()
{
    if (!calibrationWizard) {
        calibrationWizard=new ImFCSCalibrationWizard(NULL);
        connect(calibrationWizard, SIGNAL(showTutorial()), this, SLOT(showCalibrationTutorial()));
        connect(calibrationWizard, SIGNAL(run1()), this, SLOT(insertFCSFitForCalibration()));
        connect(calibrationWizard, SIGNAL(run2()), this, SLOT(imFCSCalibrationTool1()));
        connect(calibrationWizard, SIGNAL(run3()), this, SLOT(imFCSCalibrationTool2()));
        connect(calibrationWizard, SIGNAL(run4()), this, SLOT(imFCSCalibrationTool3()));
        connect(calibrationWizard, SIGNAL(run5()), this, SLOT(imFCSCalibrationTool4()));
        connect(calibrationWizard, SIGNAL(loadFile()), this, SLOT(imFCSCalibrationSelectFile()));
        connect(calibrationWizard, SIGNAL(correlate()), this, SLOT(imFCSCalibrationCorrelate()));
    }
    calibrationWizard->hideStep01(false);
    calibrationWizard->show();
    calibrationWizard->activateWindow();
    calibrationWizard->raise();
}

void QFPEvalIMFCSFit::insertFCSFitForCalibration() {
    if (project) {


        ImFCSCalibrationDialog* dlg=new ImFCSCalibrationDialog(NULL);
        QFEvaluationItem* edummy=project->addEvaluation(getID(), "imFCS Fit");
        QFImFCSFitEvaluation* imFCS=qobject_cast<QFImFCSFitEvaluation*>(edummy);
        if (imFCS) dlg->setFitModels(imFCS->getAvailableFitFunctions(), imFCS->getFitFunctionID());
        if (dlg->exec()) {
            QList<double> vals=dlg->getValues();
            insertFCSFitForCalibration(edummy, dlg->getFocusHeight(), dlg->getFocusHeightError(), dlg->getFixOffset(), dlg->getFitModel(), vals);

            if (vals.size()<=0) delete edummy;


         } else {
            delete edummy;
        }
        delete dlg;

    }
}

void QFPEvalIMFCSFit::insertFCSFitForCalibration(QFEvaluationItem* edummy, double height, double err_height, bool fixOffset, const QString &model, const QList<double> &vals)
{
    for (int i=0; i<vals.size(); i++) {
        QFEvaluationItem* e=edummy;
        if ((i>0) || (edummy==NULL)) e=project->addEvaluation(getID(), "imaging FCS Fit for calibration");
        QFImFCSFitEvaluation* eimFCS=qobject_cast<QFImFCSFitEvaluation*>(e);
        e->setQFProperty("FIT_REPEATS", 2, false, false);
        e->setQFProperty("PRESET_N_PARTICLE", 1, false, false);
        e->setQFProperty("PRESET_FOCUS_WIDTH", vals[i], false, false);
        e->setQFProperty("PRESET_FOCUS_HEIGHT", height, false, false);
        e->setQFProperty("PRESET_FOCUS_HEIGHT_ERROR", err_height, false, false);
        e->setQFProperty("PRESET_FOCUS_WIDTH_FIX", true, false, false);
        e->setQFProperty("PRESET_FOCUS_HEIGHT_FIX", true, false, false);
        e->setQFProperty("PRESET_FOCUS_DISTANCE_X_FIX", true, false, false);
        e->setQFProperty("PRESET_FOCUS_DISTANCE_Y_FIX", true, false, false);
        e->setQFProperty("PRESET_VFLOWX", 0, false, false);
        e->setQFProperty("PRESET_VFLOWY", 0, false, false);
        e->setQFProperty("PRESET_VFLOWX_FIX", true, false, false);
        e->setQFProperty("PRESET_VFLOWY_FIX", true, false, false);
        e->setQFProperty("PRESET_D1_FIX", false, false, false);
        e->setQFProperty("PRESET_OFFSET_FIX", fixOffset, false, false);
        e->setQFProperty("PRESET_DIFF_COEFF1_FIX", false, false, false);
        e->setQFProperty("IMFCS_CALIBRATION_FOCUSWIDTH", vals[i], false, false);
        e->setQFProperty("IMFCS_CALIBRATION_FOCUSHEIGHT", height, false, false);
        e->setQFProperty("IMFCS_CALIBRATION_FOCUSHEIGHT_ERROR", err_height, false, false);
        e->setQFProperty("IMFCS_CALIBRATION_MODEL", model, false, false);
        e->setQFProperty("PRESET_FIT_MODEL", model, false, false);
        e->setName(tr("wxy=%1 nm").arg(vals[i]));
        if (eimFCS) {
            eimFCS->setFitDataWeighting(QFFCSWeightingTools::RunErrorWeighting);
            eimFCS->setFitFunction(model);
        }
    }



    QFRawDataRecord* et=project->addRawData("table", "imFCS Calibration results");
    et->setQFProperty("IMFCS_CALIBRATION_RESULTTABLE", true, false, false);
}

void QFPEvalIMFCSFit::imFCSCalibrationSelectFile()
{
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if(p&&p->getRawDataRecordFactory()->contains("imaging_fcs")) {
        QFRDRIMFCSCorrelatorRemote* r=dynamic_cast<QFRDRIMFCSCorrelatorRemote*>(p->getRawDataRecordFactory()->getPlugin("imaging_fcs"));
        r->imfcsCorrRemoteUserSelectFile();
    }
}

void QFPEvalIMFCSFit::imFCSCalibrationCorrelate()
{
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if(p&&p->getRawDataRecordFactory()->contains("imaging_fcs")) {
        QFRDRIMFCSCorrelatorRemote* r=dynamic_cast<QFRDRIMFCSCorrelatorRemote*>(p->getRawDataRecordFactory()->getPlugin("imaging_fcs"));
        r->imfcsCorrRemoteAddJobSeries("binning", 1, 5, 1);
    }
}

void QFPEvalIMFCSFit::imFCSCalibrationTool1()
{
    if (!project || !services) {
        QMessageBox::critical(parentWidget, tr("imFCS Calibration"), tr("No project loaded!"));
        return;
    }
    log_text(tr("imFCS calibration tool 1: fitting D's ... \n"));

    if (calibrationWizard)  {
        calibrationWizard->getPltD()->set_doDrawing(false);
        JKQTPdatastore* ds=calibrationWizard->getPltD()->getDatastore();
        ds->clear();
        calibrationWizard->getPltD()->clearGraphs();
        calibrationWizard->getPltD()->set_doDrawing(true);
        calibrationWizard->getPltD()->update_plot();
        calibrationWizard->getPltWxy()->set_doDrawing(false);
        ds=calibrationWizard->getPltWxy()->getDatastore();
        ds->clear();
        calibrationWizard->getPltWxy()->clearGraphs();
        calibrationWizard->getPltWxy()->set_doDrawing(true);
        calibrationWizard->getPltWxy()->update_plot();
    }


        for (int i=0; i<project->getEvaluationCount(); i++) {
            QFEvaluationItem* e=project->getEvaluationByNum(i);
            QFImFCSFitEvaluation* imFCS=qobject_cast<QFImFCSFitEvaluation*>(e);
            if (imFCS) {
                log_text(tr("   - fitting %1 \n").arg(e->getName()));
                QFEvaluationPropertyEditor* pedt=services->openEvaluationEditor(e);
                QFEvaluationEditor* edt=pedt->getEditor();
                QFImFCSFitEvaluationEditor* eedt=qobject_cast<QFImFCSFitEvaluationEditor*>(edt);
                if (eedt && e->getName().toLower().contains("wxy")) {
                    eedt->fitEverythingThreadedWriter();
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


    int counter=0;
    bool first=true;
    QList<QFRawDataRecord*> rdrs;
    QVector<double> focus_widths, Ds, Ds2;
    QString model="";
    double wz=0;
    double ewz=0;
    double focus_height=0;
    bool fixOffset=true;
    double xmin=0;
    double xmax=0;
    double xmax_shift=0;
    double ymin=0;
    double ymax=0;
    QVector<double> pixwidths, pixshifts;
    QList<QVector<double> > Dvals, Derrs;
    bool isshifted=false;

    for (int i=0; i<project->getEvaluationCount(); i++) {
        QFEvaluationItem* e=project->getEvaluationByNum(i);
        QFImFCSFitEvaluation* imFCS=qobject_cast<QFImFCSFitEvaluation*>(e);
        if (e) {
            double fw=e->getProperty("IMFCS_CALIBRATION_FOCUSWIDTH", -1.0).toDouble();
            QString colName=QString::fromLatin1("D%1 [\xB5m^2/s]").arg(round(fw));
            QString colNameE=QString::fromLatin1("D%1_error [\xB5m^2/s]").arg(round(fw));
            if (fw>0) {
                wz=focus_height=e->getProperty("PRESET_FOCUS_HEIGHT", 0).toDouble();
                fixOffset=e->getProperty("PRESET_OFFSET_FIX", fixOffset).toBool();
                ewz=e->getProperty("PRESET_FOCUS_HEIGHT_ERROR", 0).toDouble();
                model=e->getProperty("IMFCS_CALIBRATION_MODEL", "").toString();
                focus_widths<<fw;
                if (first) {
                    // build list of fit RDRs
                    int rcounter=0;
                    double oldshift=0;
                    for (int ri=0; ri<project->getRawDataCount(); ri++) {
                        QFRawDataRecord* r=project->getRawDataByNum(ri);
                        if (imFCS->isFilteredAndApplicable(r)) {
                            rdrs<<r;
                            double bin=r->getProperty("BINNING", 1.0).toDouble();
                            double width=r->getProperty("PIXEL_WIDTH", 0).toDouble();
                            double shift=sqrt(qfSqr(r->getProperty("DCCF_DELTAX", 0).toDouble())+qfSqr(r->getProperty("DCCF_DELTAY", 0).toDouble()));


                            //tab->tableSetData(rcounter, 0, imFCS->getFitValue(r, imFCS->getEvaluationResultID(-1), "pixel_size"));
                            //tab->tableSetData(rcounter, 0, bin*width);
                            pixwidths<<bin*width;
                            pixshifts<<shift*width;
                            if (bin*width>xmax) xmax=1.1*bin*width;
                            if (shift>xmax_shift) xmax_shift=shift*width*1.1;

                            if (rcounter==0) {
                                oldshift=shift;
                            } else {
                                isshifted=isshifted||(oldshift!=shift);
                            }
                            oldshift=shift;

                            rcounter++;
                            Ds<<0;
                            Ds2<<0;
                            //pixel_sizes<<bin*width;

                        }
                    }
                    if (!isshifted) {
                        tab->tableSetColumnTitle(0, tr("pixel size [nm]"));
                        tab->tableSetColumnDataAsDouble(0, pixwidths);
                    } else {
                        tab->tableSetColumnTitle(0, tr("pixel shift [nm]"));
                        tab->tableSetColumnDataAsDouble(0, pixshifts);
                    }

                }
                first=false;

                Dvals.append(QVector<double>());
                Derrs.append(QVector<double>());

                for (int ri=0; ri<rdrs.size(); ri++) {
                    QFRawDataRecord* r=rdrs[ri];
                    QString erid=imFCS->getEvaluationResultID(0, r);
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
                    //tab->tableSetData(ri, 1+counter*2, Dmean);
                    //qDebug()<<"Dvals["<<counter<<"/"<<Dvals.size()<<"]="<<Dmean;
                    Dvals[counter]<<Dmean;
                    Derrs[counter]<<sqrt(Dvar);
                    //tab->tableSetData(ri, 1+counter*2+1, sqrt(Dvar));
                    Ds[ri]=Ds[ri]+Dmean;
                    Ds2[ri]=Ds2[ri]+Dmean*Dmean;
                }

                tab->tableSetColumnDataAsDouble(1+counter*2, Dvals[counter]);
                tab->tableSetColumnDataAsDouble(1+counter*2+1, Derrs[counter]);
                tab->tableSetColumnTitle(1+counter*2, colName);
                tab->tableSetColumnTitle(1+counter*2+1, colNameE);

                counter++;
            }
        }
    }

    QStringList DsNames;
    for (int ri=0; ri<Ds.size(); ri++) {
        if (counter>1) Ds2[ri]=sqrt((Ds2[ri]-Ds[ri]*Ds[ri]/double(counter))/double(counter-1));
        else Ds2[ri]=0;
        Ds[ri]=Ds[ri]/double(counter);
        if (!isshifted) {
            DsNames<<QString::fromLatin1("a=%1nm:  D= (%2 +/- %3) \xB5m^2/s").arg(pixwidths[ri]).arg(Ds[ri]).arg(Ds2[ri]);
        } else {
            DsNames<<QString::fromLatin1("delta=%1nm:  D= (%2 +/- %3) \xB5m^2/s").arg(pixshifts[ri]).arg(Ds[ri]).arg(Ds2[ri]);
        }
    }
    if (calibrationWizard)  {
        calibrationWizard->getPltWxy()->set_doDrawing(false);
        JKQTPdatastore* ds=calibrationWizard->getPltWxy()->getDatastore();
        ds->clear();
        calibrationWizard->getPltWxy()->clearGraphs();
        calibrationWizard->getPltWxy()->set_doDrawing(true);
        calibrationWizard->getPltWxy()->update_plot();
    }
    if (calibrationWizard)  {
        calibrationWizard->getPltD()->set_doDrawing(false);
        JKQTPdatastore* ds=calibrationWizard->getPltD()->getDatastore();
        ds->clear();
        calibrationWizard->getPltD()->clearGraphs();
        if (!isshifted) {
            calibrationWizard->getPltD()->get_plotter()->set_plotLabel(tr("pixel size vs. diffusion coefficient"));
            calibrationWizard->getPltD()->get_plotter()->getXAxis()->set_axisLabel(tr("pixel size [nm]"));
        } else {
            calibrationWizard->getPltD()->get_plotter()->set_plotLabel(tr("pixel shift vs. diffusion coefficient"));
            calibrationWizard->getPltD()->get_plotter()->getXAxis()->set_axisLabel(tr("pixel shift [nm]"));
        }
        calibrationWizard->getPltD()->get_plotter()->getYAxis()->set_axisLabel(tr("diffusion coefficient [{\\mu}m^2/s]"));
        if (pixwidths.size()>0 && counter>0) {
            size_t c_ps=0;
            if (!isshifted) {
                c_ps=ds->addCopiedColumn(pixwidths.data(), pixwidths.size(), tr("pixel widths"));
            }else {
                c_ps=ds->addCopiedColumn(pixshifts.data(), pixshifts.size(), tr("pixel shift"));
            }
            size_t c_Davg=ds->addCopiedColumn(Ds.data(), Ds.size(), QLatin1String("avg(D) [\xB5m^2/s]"));
            size_t c_Dsd=ds->addCopiedColumn(Ds2.data(), Ds2.size(), QLatin1String("std(D) [\xB5m^2/s]"));
            for (int i=0; i<counter; i++) {
                size_t c_D=ds->addCopiedColumn(Dvals[i].data(), Dvals[i].size(), tr("D(w_{xy}=%1nm)").arg(focus_widths[i]));
                size_t c_ED=ds->addCopiedColumn(Derrs[i].data(), Derrs[i].size(), tr("error: D(w_{xy}=%1nm)").arg(focus_widths[i]));
                JKQTPxyLineErrorGraph* plt=new JKQTPxyLineErrorGraph(calibrationWizard->getPltD()->get_plotter());
                plt->set_title( tr("w_{xy}=%1nm").arg(focus_widths[i]));
                plt->set_xColumn(c_ps);
                plt->set_yColumn(c_D);
                plt->set_yErrorColumn(c_ED);
                plt->set_xErrorStyle(JKQTPnoError);
                plt->set_yErrorStyle(JKQTPerrorBars);
                plt->set_symbolSize(10);
                plt->set_symbol(JKQTPfilledCircle);
                calibrationWizard->getPltD()->get_plotter()->addGraph(plt);
            }
        }

        calibrationWizard->getPltD()->set_doDrawing(true);
        calibrationWizard->getPltD()->zoomToFit();
        calibrationWizard->raise();
    }

    int gr=-1;
    if (graph) {
        /*int i=0;
        while (graph->colgraphGetGraphCount()<1 && i<1) {
            graph->colgraphAddGraph("title");
            i++;
        }*/
        bool emitSigG=graph->colgraphGetDoEmitSignals();
        graph->colgraphSetDoEmitSignals(false);

        graph->colgraphAddPlot("title");
        gr=graph->colgraphGetPlotCount()-1;

        if (!isshifted) {
            graph->colgraphSetPlotTitle(gr, tr("pixel size vs. diffusion coefficient"));
            graph->colgraphSetPlotXAxisProps(gr, tr("pixel size [nm]"));
            graph->colgraphSetPlotXRange(gr, xmin, xmax);
        } else {
            graph->colgraphSetPlotTitle(gr, tr("pixel shift vs. diffusion coefficient"));
            graph->colgraphSetPlotXAxisProps(gr, tr("pixel shift [nm]"));
            graph->colgraphSetPlotXRange(gr, xmin, xmax_shift);
        }
        graph->colgraphSetPlotYAxisProps(gr, tr("diffusion coefficient [{\\mu}m^2/s]"));
        while (graph->colgraphGetGraphCount(gr)>0){
            graph->colgraphRemoveGraph(gr, 0);
        }
        for (int i=0; i<focus_widths.size(); i++) {
            graph->colgraphAddGraph(gr, 0, 1+i*2, QFRDRColumnGraphsInterface::cgtLinesPoints, tr("w_{xy}=%1nm").arg(focus_widths[i]));
        }
        graph->colgraphSetPlotYRange(gr, ymin, ymax);
        graph->colgraphSetDoEmitSignals(emitSigG);

    }

    QFRawDataPropertyEditor* editor=QFPluginServices::getInstance()->openRawDataEditor(etab, false);
    if (editor) {
        if (gr>=0) editor->sendEditorCommand("showPlot", gr);
        editor->showTab(2);
        editor->raise();
    }
    if (calibrationWizard) calibrationWizard->raise();

    QApplication::restoreOverrideCursor();



    bool dlgOK=false;
    int DItem=DsNames.indexOf(QInputDialog::getItem(calibrationWizard, tr("imFCS Calibration"), tr("Please choose the binning at which to measure the \"true\" diffusion coefficient for the calibration:"), DsNames, 4, false, &dlgOK));

    if (dlgOK) {

        double Dcalib=Ds.value(DItem, 0);
        double DcalibE=Ds2.value(DItem, 0);
        QFEvaluationItem* e=project->addEvaluation(getID(), QString::fromLatin1("calibration D= %1 \xB5m^2/s").arg(Dcalib));
        e->setQFProperty("FIT_REPEATS", 2, false, false);
        e->setQFProperty("PRESET_N_PARTICLE", 1, false, false);
        e->setQFProperty("PRESET_D1", Dcalib, false, false);
        e->setQFProperty("PRESET_D1_ERROR", DcalibE, false, false);
        e->setQFProperty("PRESET_D1_FIX", true, false, false);
        e->setQFProperty("PRESET_DIFF_COEFF1", Dcalib, false, false);
        e->setQFProperty("PRESET_DIFF_COEFF1_ERROR", DcalibE, false, false);
        e->setQFProperty("PRESET_DIFF_COEFF1_FIX", true, false, false);
        e->setQFProperty("PRESET_FOCUS_WIDTH_FIX", false, false, false);
        e->setQFProperty("PRESET_OFFSET_FIX", fixOffset, false, false);
        e->setQFProperty("PRESET_FOCUS_HEIGHT", focus_height, false, false);
        e->setQFProperty("PRESET_FOCUS_HEIGHT_ERROR", ewz, false, false);
        e->setQFProperty("IMFCS_CALIBRATION_D", Dcalib, false, false);
        e->setQFProperty("IMFCS_CALIBRATION_D_ERROR", DcalibE, false, false);
        e->setQFProperty("IMFCS_CALIBRATION_FITWXY", true, false, false);
        e->setQFProperty("PRESET_FOCUS_DISTANCE_X_FIX", true, false, false);
        e->setQFProperty("PRESET_FOCUS_DISTANCE_Y_FIX", true, false, false);
        e->setQFProperty("PRESET_VFLOWX", 0, false, false);
        e->setQFProperty("PRESET_VFLOWY", 0, false, false);
        e->setQFProperty("PRESET_VFLOWX_FIX", true, false, false);
        e->setQFProperty("PRESET_VFLOWY_FIX", true, false, false);
        e->setQFProperty("PRESET_FIT_MODEL", model, false, false);
        e->setName(QString(QLatin1String("calibration D=%1\xB5m^2/s")).arg(Dcalib));
        QFImFCSFitEvaluation* eimFCS=qobject_cast<QFImFCSFitEvaluation*>(e);
        if (eimFCS && (!model.isEmpty())) eimFCS->setFitFunction(model);
        if (eimFCS) eimFCS->setFitDataWeighting(QFFCSWeightingTools::RunErrorWeighting);
    }

    log_text(tr("imFCS calibration tool 2: collecting D data ... DONE!\n"));
}

void QFPEvalIMFCSFit::imFCSCalibrationTool3()
{
    if (!project || !services) {
        QMessageBox::critical(parentWidget, tr("imFCS Calibration"), tr("No project loaded!"));
        return;
    }
    log_text(tr("imFCS calibration tool 3: fitting wxy ... \n"));
    for (int i=0; i<project->getEvaluationCount(); i++) {
        QFEvaluationItem* e=project->getEvaluationByNum(i);
        QFImFCSFitEvaluation* imFCS=qobject_cast<QFImFCSFitEvaluation*>(e);
        if (imFCS && e->getProperty("IMFCS_CALIBRATION_FITWXY", false).toBool()) {
            log_text(tr("   - fitting %1 \n").arg(e->getName()));
            QFEvaluationPropertyEditor* pedt=services->openEvaluationEditor(e);
            QFEvaluationEditor* edt=pedt->getEditor();
            QFImFCSFitEvaluationEditor* eedt=qobject_cast<QFImFCSFitEvaluationEditor*>(edt);
            if (eedt) {
                eedt->fitEverythingThreadedWriter();
            }
            if (pedt) pedt->close();
            log_text(tr("        DONE!\n"));
        }
    }
    if (calibrationWizard)  {
        calibrationWizard->getPltWxy()->set_doDrawing(false);
        JKQTPdatastore* ds=calibrationWizard->getPltWxy()->getDatastore();
        ds->clear();
        calibrationWizard->getPltWxy()->clearGraphs();
        calibrationWizard->getPltWxy()->set_doDrawing(true);
        calibrationWizard->getPltWxy()->update_plot();
    }
    log_text(tr("imFCS calibration tool 3: fitting wxy's ... DONE!\n"));
}

void QFPEvalIMFCSFit::imFCSCalibrationTool4()
{
    if (!project)  {
        QMessageBox::critical(parentWidget, tr("imFCS Calibration"), tr("No project loaded!"));
        return;
    }
    log_text(tr("imFCS calibration tool 4: collecting wxy data ... \n"));

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

    if (calibrationWizard)  {
        calibrationWizard->getPltWxy()->set_doDrawing(false);
        JKQTPdatastore* ds=calibrationWizard->getPltWxy()->getDatastore();
        ds->clear();
        calibrationWizard->getPltWxy()->clearGraphs();
    }

    QFRDRColumnGraphsInterface* graph=qobject_cast<QFRDRColumnGraphsInterface*>(etab);
    int gr=-1;

    QList<QFRawDataRecord*> rdrs;
    double xmin=0;
    double xmax=0;
    double xmax_shift=0;
    double ymin=0;
    double ymax=0;
    double Dcalib=0;
    double DcalibE=0;
    double wxymean=0, wxyvar=0;
    QVector<double> pixwidths, pixshifts;
    double oldshift=0;
    bool isshifted=false;
    for (int i=0; i<project->getEvaluationCount(); i++) {
        QFEvaluationItem* e=project->getEvaluationByNum(i);
        QFImFCSFitEvaluation* imFCS=qobject_cast<QFImFCSFitEvaluation*>(e);
        if (e) {
            bool fitwxy=e->getProperty("IMFCS_CALIBRATION_FITWXY", false).toBool();
            Dcalib=e->getProperty("IMFCS_CALIBRATION_D", Dcalib).toDouble();
            DcalibE=e->getProperty("IMFCS_CALIBRATION_D_ERROR", DcalibE).toDouble();
            QString colName=tr("wxy [nm]");
            QString colNameE=tr("wxy_error [nm]");
            QVector<double> wxys, wxyerrors;
            if (fitwxy) {
                int cols=e->getProperty("IMFCS_CALIBRATION_COLS", tab->tableGetColumnCount()).toInt();
                e->setQFProperty("IMFCS_CALIBRATION_COLS", cols);
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
                        double shift=sqrt(qfSqr(r->getProperty("DCCF_DELTAX", 0).toDouble())+qfSqr(r->getProperty("DCCF_DELTAY", 0).toDouble()));

                        //tab->tableSetData(rcounter, cols, bin*width);
                        pixwidths<<bin*width;
                        if (bin*width>xmax) xmax=1.1*bin*width;
                        pixshifts<<shift*width;
                        if (shift*width>xmax_shift) xmax_shift=1.1*shift*width;

                        if (rcounter==0) {
                            oldshift=shift;
                        } else {
                            isshifted=isshifted||(oldshift!=shift);
                        }
                        oldshift=shift;



                        QString erid=imFCS->getEvaluationResultID(0, r);
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
                        wxys<<wxymean;
                        wxyvec<<wxymean;
                        wxyerrors<<sqrt(wxyvar);
                        //qDebug()<<rcounter<<cols+1<<wxymean<<sqrt(wxyvar);

                        if (wxymean+sqrt(wxyvar)>ymax) ymax=(wxymean+sqrt(wxyvar))*1.1;
                        if (wxymean-sqrt(wxyvar)<ymin) ymin=(wxymean-sqrt(wxyvar));

                        rcounter++;
                    }
                }
                if (!isshifted) {
                    tab->tableSetColumnTitle(cols, tr("pixel size [nm]"));
                    tab->tableSetColumnDataAsDouble(cols, pixwidths);
                } else {
                    tab->tableSetColumnTitle(cols, tr("pixel shift [nm]"));
                    tab->tableSetColumnDataAsDouble(cols, pixshifts);
                }
                if (rcounter>0) {
                    tab->tableSetData(0, cols+3, qfstatisticsAverage(wxyvec));
                    tab->tableSetData(0, cols+4, sqrt(qfstatisticsVariance(wxyvec)));
                    tab->tableSetData(rcounter-1, cols+3, qfstatisticsAverage(wxyvec));
                    tab->tableSetData(rcounter-1, cols+4, sqrt(qfstatisticsVariance(wxyvec)));
                    if (graph) {
                        bool emitSigG=graph->colgraphGetDoEmitSignals();
                        graph->colgraphSetDoEmitSignals(false);
                        int ggraph=e->getProperty("IMFCS_CALIBRATION_GRAPHS", -1).toInt();
                        if (ggraph<0) {
                            if (!isshifted) {
                                graph->colgraphAddPlot(tr("pixel size vs. lat. focus size, D=%2{\\mu}m^2/s").arg(Dcalib));
                            } else {
                                graph->colgraphAddPlot(tr("pixel shift vs. lat. focus size, D=%2{\\mu}m^2/s").arg(Dcalib));
                            }
                            ggraph=graph->colgraphGetPlotCount()-1;
                        } else {
                            if (!isshifted) {
                                graph->colgraphSetPlotTitle(ggraph, tr("pixel size vs. lat. focus size, D=%2{\\mu}m^2/s").arg(Dcalib));
                            } else {
                                graph->colgraphSetPlotTitle(ggraph, tr("pixel shift vs. lat. focus size, D=%2{\\mu}m^2/s").arg(Dcalib));
                            }
                        }
                        e->setQFProperty("IMFCS_CALIBRATION_GRAPHS", ggraph);
                        graph->colgraphSetPlotTitle(ggraph, tr("pixel size vs. lat. focus size"));
                        if (!isshifted) {
                            graph->colgraphSetPlotXAxisProps(ggraph, tr("pixel size [nm]"));
                        } else {
                            graph->colgraphSetPlotXAxisProps(ggraph, tr("pixel shift [nm]"));
                        }
                        graph->colgraphSetPlotYAxisProps(ggraph, tr("lateral focus size w_{xy} [nm]"));
                        graph->colgraphAddErrorGraph(ggraph, cols, -1, cols+1, cols+2, QFRDRColumnGraphsInterface::cgtLinesPoints, tr("calibration D=(%1\\pm %2){\\mu}m^2/s").arg(Dcalib).arg(DcalibE));
                        graph->colgraphSetGraphColor(ggraph, graph->colgraphGetGraphCount(ggraph)-1, QColor("red"));
                        graph->colgraphAddGraph(ggraph, cols, cols+3, QFRDRColumnGraphsInterface::cgtLines, tr("w_{xy}=(%1\\pm %2)nm").arg(qfstatisticsAverage(wxyvec)).arg(sqrt(qfstatisticsVariance(wxyvec))));
                        graph->colgraphSetGraphColor(ggraph, graph->colgraphGetGraphCount(ggraph)-1, QColor("blue"));
                        graph->colgraphSetPlotXRange(ggraph, xmin, xmax);
                        graph->colgraphSetPlotYRange(ggraph, ymin, ymax);
                        gr=ggraph;
                        graph->colgraphSetDoEmitSignals(emitSigG);

                    }





                    if (calibrationWizard)  {
                        JKQTPdatastore* ds=calibrationWizard->getPltWxy()->getDatastore();
                        if (!isshifted) {
                            calibrationWizard->getPltWxy()->get_plotter()->set_plotLabel(tr("pixel size vs. lat. focus size, D=%2{\\mu}m^2/s").arg(Dcalib));
                            calibrationWizard->getPltWxy()->getXAxis()->set_axisLabel(tr("pixel size [nm]"));
                        } else {
                            calibrationWizard->getPltWxy()->get_plotter()->set_plotLabel(tr("pixel shift vs. lat. focus size, D=%2{\\mu}m^2/s").arg(Dcalib));
                            calibrationWizard->getPltWxy()->getXAxis()->set_axisLabel(tr("pixel shift [nm]"));
                        }
                        calibrationWizard->getPltWxy()->getYAxis()->set_axisLabel(tr("lateral focus size w_{xy} [nm]"));

                        JKQTPhorizontalRange* range=new JKQTPhorizontalRange(calibrationWizard->getPltWxy());
                        range->set_title(tr("\\overline{w_{xy}}=( %1 \\pm %2 ) nm for D=%3{\\mu}m^2/s").arg(roundWithError(qfstatisticsAverage(wxys), qfstatisticsStd(wxys), 2)).arg(roundError(qfstatisticsStd(wxys),2)).arg(Dcalib));
                        range->set_plotCenterLine(true);
                        range->set_rangeCenter(qfstatisticsAverage(wxys));
                        range->set_rangeMin(qfstatisticsAverage(wxys)-qfstatisticsStd(wxys));
                        range->set_rangeMax(qfstatisticsAverage(wxys)+qfstatisticsStd(wxys));
                        range->set_unlimitedSizeMin(true);
                        range->set_unlimitedSizeMax(true);
                        calibrationWizard->getPltWxy()->addGraph(range);

                        JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(calibrationWizard->getPltWxy());
                        g->set_drawLine(false);
                        g->set_symbol(JKQTPfilledCircle);
                        g->set_symbolSize(12);
                        calibrationWizard->getPltWxy()->addGraph(g);
                        if (!isshifted) {
                            g->set_xColumn(ds->addCopiedColumn(pixwidths, tr("pixel size a [nm], D=%2{\\mu}m^2/s").arg(Dcalib) ));
                            g->set_yColumn(ds->addCopiedColumn(wxys, tr("wxy [nm], D=%2{\\mu}m^2/s").arg(Dcalib)));
                            g->set_yErrorColumn(ds->addCopiedColumn(wxyerrors, tr("wxy_error [nm], D=%2{\\mu}m^2/s").arg(Dcalib)));
                            int ismall=0;
                            if (pixwidths.size()>1) {
                                for (int i=1; i<pixwidths.size(); i++) {
                                    if (pixwidths[i]<pixwidths[ismall]) ismall=i;
                                }
                            }
                            calibrationWizard->getPltWxy()->addGraph(new JKQTPgeoText(calibrationWizard->getPltWxy(), pixwidths.value(ismall), wxys.value(ismall), tr("\\ \\ \\leftarrow\\ \\textbf{w_{xy}=( %1 \\pm %2 ) nm}").arg(roundWithError(wxys.value(ismall),wxyerrors.value(ismall),2)).arg(roundError(wxyerrors.value(ismall),2)), 12));
                        } else {
                            g->set_xColumn(ds->addCopiedColumn(pixshifts, tr("pixel shift \\delta [nm], D=%2{\\mu}m^2/s").arg(Dcalib) ));
                            g->set_yColumn(ds->addCopiedColumn(wxys, tr("wxy [nm], D=%2{\\mu}m^2/s").arg(Dcalib)));
                            g->set_yErrorColumn(ds->addCopiedColumn(wxyerrors, tr("wxy_error [nm], D=%2{\\mu}m^2/s").arg(Dcalib)));
                            int ismall=0;
                            if (pixshifts.size()>1) {
                                for (int i=1; i<pixshifts.size(); i++) {
                                    if (pixshifts[i]<pixshifts[ismall]) ismall=i;
                                }
                            }
                            calibrationWizard->getPltWxy()->addGraph(new JKQTPgeoText(calibrationWizard->getPltWxy(), pixshifts.value(ismall), wxys.value(ismall), tr("\\ \\ \\leftarrow\\ \\textbf{w_{xy}=( %1 \\pm %2 ) nm}").arg(roundWithError(wxys.value(ismall),wxyerrors.value(ismall),2)).arg(roundError(wxyerrors.value(ismall),2)), 12));
                        }
                        g->set_yErrorStyle(JKQTPerrorBars);
                        g->set_title(tr("w_{xy}(a) for D=%2{\\mu}m^2/s").arg(Dcalib));


                    }
                }

            }




        }


    }

//    QFRawDataPropertyEditor* editor=QFPluginServices::getInstance()->openRawDataEditor(etab, false);
//    if (editor) {
//        if (gr>=0) editor->sendEditorCommand("showPlot", gr);
//        editor->showTab(2);
//        editor->raise();
//    }

    if (calibrationWizard)  {
        calibrationWizard->getPltWxy()->zoomToFit();
        calibrationWizard->getPltWxy()->getYAxis()->setRange(-0.02*calibrationWizard->getPltWxy()->getYMax(), calibrationWizard->getPltWxy()->getYMax());
        calibrationWizard->getPltWxy()->set_doDrawing(true);
        calibrationWizard->getPltWxy()->update_plot();
        calibrationWizard->raise();
    }

    log_text(tr("imFCS calibration tool 4: collecting wxy data ... DONE!\n"));

}

void QFPEvalIMFCSFit::showCalibrationTutorial()
{
    if (services) {
        services->displayHelpWindow(services->getPluginHelpDirectory(getID())+"/calibration.html");
    }
}


Q_EXPORT_PLUGIN2(imfcsfit, QFPEvalIMFCSFit)
