/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

#include "qffccsrelativeccfcrosstalkdialog.h"
#include "ui_qffccsrelativeccfcrosstalkdialog.h"
#include "qfmatchrdrfunctor.h"
#include "qffccsfitevaluation_item.h"
#include "qfpluginservices.h"
#include<QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include "qffccsfitevaluation_item.h"
#include "qffcstools.h"
#include "optionswidget.h"
#include "qffccsrelativeccfcrosstalkdialog.h"
#include "qfrdrsimplecountrateinterface.h"
#include <stdint.h>
#include "qfmathtools.h"
#include "programoptions.h"
#include "qftools.h"
#include "jkautooutputtimer.h"

QFFCCSRelativeCCFCrosstalkDialog::QFFCCSRelativeCCFCrosstalkDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFFCCSRelativeCCFCrosstalkDialog)
{

    //qDebug()<<1;
    plt=NULL;
    matchFunctor=new QFFCCSMatchRDRFunctor();
    QFProject* project=QFPluginServices::getInstance()->getCurrentProject();
    QList<QPointer<QFRawDataRecord> > lst=matchFunctor->getFilteredList(project);
    //qDebug()<<2;
    ui->setupUi(this);
    //qDebug()<<3;
    ui->cmbACF0->init(project, matchFunctor);
    ui->cmbACF1->init(project, matchFunctor);
    //qDebug()<<4;
    ui->cmbCCF->init(project, matchFunctor);

    //qDebug()<<5;

    ui->widOverviewACF0->setRDR(NULL);
    ui->widOverviewACF1->setRDR(NULL);
    //qDebug()<<6;
    ui->widOverviewCCF->setRDR(NULL);
    //qDebug()<<7;


    //qDebug()<<8;

    bool okCCF=false;
    for (int i=0; i<lst.size(); i++) {
        if (lst[i]) {
            if (!okCCF && isCCF(lst[i])) {
                ui->cmbCCF->setCurrentRDR(lst[i]);
                okCCF=true;
                QList<QFRawDataRecord*> lsta=lst[i]->getRecordsWithRoleFromGroup("acf0");
                if (!lsta.isEmpty()) {
                    ui->cmbACF0->setCurrentRDR(lsta.first());
                } else {
                    ui->cmbACF0->setCurrentRDR(0);
                }
                lsta=lst[i]->getRecordsWithRoleFromGroup("acf1");
                if (!lsta.isEmpty()) {
                    ui->cmbACF1->setCurrentRDR(lsta.first());
                } else {
                    ui->cmbACF1->setCurrentRDR(0);
                }
            }
        }
        if (okCCF) break;
    }

    ui->cmbACF0ResultSet->setRDR(ui->cmbACF0->currentRDR());
    ui->cmbACF1ResultSet->setRDR(ui->cmbACF1->currentRDR());
    ui->cmbCCFResultSet->setRDR(ui->cmbCCF->currentRDR());
    ui->spinCrosstalk->setValue(ProgramOptions::getConfigValue("QFFCCSRelativeCCFCrosstalkDialog/crosstalk", ui->spinCrosstalk->value()).toDouble());
    ui->chkStoreAmplitudes->setChecked(ProgramOptions::getConfigValue("QFFCCSRelativeCCFCrosstalkDialog/chkStoreAmplitudes", ui->chkStoreAmplitudes->isChecked()).toBool());
    ui->chkBackground->setChecked(ProgramOptions::getConfigValue("QFFCCSRelativeCCFCrosstalkDialog/chkBackground", ui->chkBackground->isChecked()).toBool());
    ui->spinAvg->setValue(ProgramOptions::getConfigValue("QFFCCSRelativeCCFCrosstalkDialog/avg", ui->spinAvg->value()).toDouble());
    ui->cmbCrosstalkDirection->setCurrentIndex(ProgramOptions::getConfigValue("QFFCCSRelativeCCFCrosstalkDialog/crosstalk_dir", ui->cmbCrosstalkDirection->currentIndex()).toInt());
    ui->cmbAmplitudeSource->setCurrentIndex(ProgramOptions::getConfigValue("QFFCCSRelativeCCFCrosstalkDialog/amplitude_source", ui->cmbAmplitudeSource->currentIndex()).toInt());
    connect(ui->cmbACF0, SIGNAL(currentIndexChanged(int)), this, SLOT(replotImages()));
    connect(ui->cmbACF1, SIGNAL(currentIndexChanged(int)), this, SLOT(replotImages()));
    connect(ui->cmbCCF, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbCCF_currentIndexChanged(int)));
    connect(ui->cmbCrosstalkDirection, SIGNAL(currentIndexChanged(int)), this, SLOT(replotImages()));
    connect(ui->spinCrosstalk, SIGNAL(valueChanged(double)), this, SLOT(replotImages()));
    connect(ui->cmbACF0ResultSet, SIGNAL(currentIndexChanged(int)), this, SLOT(replotImages()));
    connect(ui->cmbACF1ResultSet, SIGNAL(currentIndexChanged(int)), this, SLOT(replotImages()));
    connect(ui->cmbCCFResultSet, SIGNAL(currentIndexChanged(int)), this, SLOT(replotImages()));
    connect(ui->chkBackground, SIGNAL(toggled(bool)), this, SLOT(replotImages()));
    cmbCCF_currentIndexChanged(ui->cmbCCF->currentIndex());
    loadWidgetGeometry(*(ProgramOptions::getInstance()->getQSettings()), this, QPoint(10,10), QSize(800,600), "QFFCCSRelativeCCFCrosstalkDialog/size");

    //qDebug()<<10;

}

QFFCCSRelativeCCFCrosstalkDialog::~QFFCCSRelativeCCFCrosstalkDialog()
{
    delete ui;
    delete matchFunctor;
}

void QFFCCSRelativeCCFCrosstalkDialog::closeEvent(QCloseEvent *event)
{
    saveWidgetGeometry(*(ProgramOptions::getInstance()->getQSettings()), this, "QFFCCSRelativeCCFCrosstalkDialog/size");
    ProgramOptions::setConfigValue("QFFCCSRelativeCCFCrosstalkDialog/avg", ui->spinAvg->value());
    ProgramOptions::setConfigValue("QFFCCSRelativeCCFCrosstalkDialog/crosstalk", ui->spinCrosstalk->value());
    ProgramOptions::setConfigValue("QFFCCSRelativeCCFCrosstalkDialog/crosstalk_dir", ui->cmbCrosstalkDirection->currentIndex());
    ProgramOptions::setConfigValue("QFFCCSRelativeCCFCrosstalkDialog/amplitude_source", ui->cmbAmplitudeSource->currentIndex());
    ProgramOptions::setConfigValue("QFFCCSRelativeCCFCrosstalkDialog/chkStoreAmplitudes", ui->chkStoreAmplitudes->isChecked());
    ProgramOptions::setConfigValue("QFFCCSRelativeCCFCrosstalkDialog/chkBackground", ui->chkBackground->isChecked());
    QWidget::closeEvent(event);
}
QFRawDataRecord *QFFCCSRelativeCCFCrosstalkDialog::getACF0() const
{
    return ui->cmbACF0->currentRDR();
}

QFRawDataRecord *QFFCCSRelativeCCFCrosstalkDialog::getACF1() const
{
    return ui->cmbACF1->currentRDR();
}

QFRawDataRecord *QFFCCSRelativeCCFCrosstalkDialog::getCCF() const
{
    return ui->cmbCCF->currentRDR();
}

bool QFFCCSRelativeCCFCrosstalkDialog::calculateRelCCF(QFRawDataRecord *acf0, QFRawDataRecord *acf1, QFRawDataRecord *ccf, double **rel_out0, double **rel_error_out0, double **rel_out1, double **rel_error_out1, double **acf0Amplitude, double **acf0UCAmplitude, double **acf1Amplitude, double **acf1UCAmplitude, double **ccfAmplitude, double **ccfUCAmplitude, int &runs, int avgCount, double crosstalk, int crosstalkDir, bool showErrorMessage, int source, const QString &resultGroupACF0, const QString &resultGroupACF1, const QString &resultGroupCCF, const QString &param, bool subtractBackground)
{
    //qDebug()<<acf0Amplitude<<acf0UCAmplitude<<acf1Amplitude<<acf1UCAmplitude<<ccfAmplitude<<ccfUCAmplitude;
    //JKAutoOutputTimer tim(QString("calculateRelCCF %1").arg((uint64_t)acf0Amplitude));
    if (!acf0 || !acf1 || !ccf) return false;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRDRFCSDataInterface* acfFCS0=dynamic_cast<QFRDRFCSDataInterface*>(acf0);
    QFRDRSimpleCountRatesInterface* acfRUNIMAGE0=dynamic_cast<QFRDRSimpleCountRatesInterface*>(acf0);
    QFRDRFCSDataInterface* acfFCS1=dynamic_cast<QFRDRFCSDataInterface*>(acf1);
    QFRDRSimpleCountRatesInterface* acfRUNIMAGE1=dynamic_cast<QFRDRSimpleCountRatesInterface*>(acf1);
    QFRDRFCSDataInterface* ccfFCS=dynamic_cast<QFRDRFCSDataInterface*>(ccf);
    QFRDRSimpleCountRatesInterface* ccfRUNIMAGE=dynamic_cast<QFRDRSimpleCountRatesInterface*>(ccf);
    QFRDRSimpleCountRatesInterface* ccfCnt=dynamic_cast<QFRDRSimpleCountRatesInterface*>(ccf);
    if (acfFCS0 && acfFCS1 && acfRUNIMAGE0 && acfRUNIMAGE1 && ccfFCS && ccfRUNIMAGE) {
        if (   acfFCS0->getCorrelationRuns()>0 && acfFCS0->getCorrelationRuns()==ccfFCS->getCorrelationRuns()
               && acfFCS1->getCorrelationRuns()>0 && acfFCS1->getCorrelationRuns()==ccfFCS->getCorrelationRuns()
           )
        {
            //tim.write("1");
            runs=ccfFCS->getCorrelationRuns();


            double* acf0D=acfFCS0->getCorrelation();
            double* acf0S=acfFCS0->getCorrelationRunErrors();
            int acf0N=acfFCS0->getCorrelationN();
            double* acf1D=acfFCS1->getCorrelation();
            double* acf1S=acfFCS1->getCorrelationRunErrors();
            int acf1N=acfFCS1->getCorrelationN();
            double* ccfD=ccfFCS->getCorrelation();
            double* ccfS=ccfFCS->getCorrelationRunErrors();
            int ccfN=ccfFCS->getCorrelationN();
            bool ok=true;

            QVector<double> acf0AmplitudeParameter, acf1AmplitudeParameter, ccfAmplitudeParameter;
            if (source==1) {
                bool okOut=false;
                QStringList evals;

                evals=acf0->resultsCalcEvaluationsInGroup(resultGroupACF0);
                for (int ie=0; ie<evals.size(); ie++) {
                    okOut=false;
                    acf0AmplitudeParameter=acf0->resultsGetAsDoubleList(evals[ie], param, &okOut);
                    if (okOut && acf0AmplitudeParameter.size()>=runs) break;
                    okOut=false;
                }
                ok=ok&&okOut;
                //qDebug()<<okOut<<resultGroupACF0<<listToString(acf0AmplitudeParameter);

                evals=acf1->resultsCalcEvaluationsInGroup(resultGroupACF1);
                for (int ie=0; ie<evals.size(); ie++) {
                    okOut=false;
                    acf1AmplitudeParameter=acf1->resultsGetAsDoubleList(evals[ie], param, &okOut);
                    if (okOut && acf1AmplitudeParameter.size()>=runs) break;
                    okOut=false;
                }
                ok=ok&&okOut;
                //qDebug()<<okOut<<resultGroupACF1<<listToString(acf1AmplitudeParameter);

                evals=ccf->resultsCalcEvaluationsInGroup(resultGroupCCF);
                for (int ie=0; ie<evals.size(); ie++) {
                    okOut=false;
                    ccfAmplitudeParameter=ccf->resultsGetAsDoubleList(evals[ie], param, &okOut);
                    if (okOut && ccfAmplitudeParameter.size()>=runs) break;
                    okOut=false;
                }
                ok=ok&&okOut;
                //qDebug()<<okOut<<resultGroupCCF<<listToString(ccfAmplitudeParameter);

                if (!ok && showErrorMessage) QMessageBox::critical(NULL, tr("Calculate Relative CCF"), tr("Can't read g(0) values from all RDRs."));

            }
            //tim.write(QString("1: acfN=%1, ccfN=%2 w=%3 h=%4").arg(acfN).arg(ccfN).arg(w).arg(h));

            if (ok && acf0D && acf0S && acf1D && acf1S && ccfD && ccfS && acf0N>0 && acf1N>0 && ccfN>0) {
                //tim.write("2");
                double* rel0=(double*)qfMalloc(runs*sizeof(double));
                double* rel0_error=(double*)qfMalloc(runs*sizeof(double));
                double* rel1=(double*)qfMalloc(runs*sizeof(double));
                double* rel1_error=(double*)qfMalloc(runs*sizeof(double));
                double* acf0_out=(double*)qfMalloc(runs*sizeof(double));
                double* acf1_out=(double*)qfMalloc(runs*sizeof(double));
                double* ccf_out=(double*)qfMalloc(runs*sizeof(double));
                double* acf0UC_out=(double*)qfMalloc(runs*sizeof(double));
                double* acf1UC_out=(double*)qfMalloc(runs*sizeof(double));
                double* ccfUC_out=(double*)qfMalloc(runs*sizeof(double));
                uint64_t idxA0=0;
                uint64_t idxA1=0;
                uint64_t idxC=0;
                for (int i=0; i<runs; i++) {
                    double D=0, A0=0, A1=0,eD=0, eA1=0, eA0=0;

                    if (source==0) {
                        //tim.write(QString("3: %1 [%2...%3] avgCount=%4").arg(i).arg(w).arg(h).arg(avgCount));
                        D=ccfD[idxC];
                        //tim.write(QString("3:   ---"));
                        A0=acf0D[idxA0];
                        A1=acf1D[idxA1];
                        //tim.write(QString("3:   ---"));
                        eD=ccfS[idxC];
                        //tim.write(QString("3:   ---"));
                        eA0=acf0S[idxA0];
                        eA1=acf1S[idxA1];
                        //tim.write(QString("3:   ---"));

                        if (avgCount>1) {
                            int avg0A=qMin(avgCount, acf0N);
                            int avg1A=qMin(avgCount, acf1N);
                            int avgC=qMin(avgCount, ccfN);

                            for (int j=1; j<avg0A; j++) {
                                A0=A0+acf0D[idxA0+j];
                                eA0=eA0+acf0S[idxA0+j];
                            }
                            A0=A0/double(avg0A);
                            eA0=eA0/double(avg0A);

                            for (int j=1; j<avg1A; j++) {
                                A1=A1+acf1D[idxA1+j];
                                eA1=eA1+acf1S[idxA1+j];
                            }
                            A1=A1/double(avg1A);
                            eA1=eA1/double(avg1A);

                            for (int j=1; j<avgC; j++) {
                                D=D+ccfD[idxC+j];
                                eD=eD+ccfS[idxC+j];
                            }
                            D=D/double(avgC);
                            eD=eD/double(avgC);
                        }
                    } else if (source==1) {
                        eA0=eA1=eD=0;
                        D=ccfAmplitudeParameter.value(idxC, 0);
                        A0=acf0AmplitudeParameter.value(idxA0, 0);
                        A1=acf1AmplitudeParameter.value(idxA1, 0);

                    }
                    //tim.write(QString("3:   ---"));
                    acf0UC_out[i]=A0;
                    acf1UC_out[i]=A1;
                    ccfUC_out[i]=D;

                    if (ccfCnt) {
                        double I0=ccfCnt->getSimpleCountrateAverage(i, 0, false);
                        double I1=ccfCnt->getSimpleCountrateAverage(i, 1, false);

                        if (subtractBackground) {
                            double cnt1=0, cnt2=0, cnte1=0, cnte2=0;
                            double back1=0, back2=0, backe1=0, backe2=0;

                            qfFCSHasSpecial(ccf, i, "background1", back1, backe1);
                            qfFCSHasSpecial(ccf, i, "background2", back2, backe2);
                            qfFCSHasSpecial(ccf, i, "count_rate1", cnt1, cnte1);
                            qfFCSHasSpecial(ccf, i, "count_rate2", cnt2, cnte2);
                            I0=cnt1-back1;
                            I1=cnt2-back2;
                        }
                        qfFCCSCrosstalkCorrection(A0, A1, D, I0, I1, crosstalk, crosstalkDir);
                    }



                    acf0_out[i]=A0;
                    acf1_out[i]=A1;
                    ccf_out[i]=D;

                    rel0[i]=D/A0;
                    rel0_error[i]=0;
                    if (acf0S && ccfS) rel0_error[i]=sqrt(qfSqr(eA0*D/qfSqr(A0)) + qfSqr(eD/A0));
                    if (!QFFloatIsOK(rel0_error[i])) rel0_error[i]=0;

                    rel1[i]=D/A1;
                    rel1_error[i]=0;
                    if (acf1S && ccfS) rel1_error[i]=sqrt(qfSqr(eA1*D/qfSqr(A1)) + qfSqr(eD/A1));
                    if (!QFFloatIsOK(rel1_error[i])) rel1_error[i]=0;

                    if (source==0) {
                        idxA0=idxA0+acf0N;
                        idxA1=idxA1+acf1N;
                        idxC=idxC+ccfN;
                    } else if (source==1) {
                        idxA0++;
                        idxA1++;
                        idxC++;
                    }
                }
                if (rel_out0) *rel_out0=rel0;
                else if (rel0) qfFree(rel0);
                if (rel_error_out0) *rel_error_out0=rel0_error;
                else if (rel0_error) qfFree(rel0_error);

                if (rel_out1) *rel_out1=rel1;
                else if (rel1) qfFree(rel1);
                if (rel_error_out1) *rel_error_out1=rel1_error;
                else if (rel1_error) qfFree(rel1_error);

                if (acf0Amplitude) *acf0Amplitude=acf0_out;
                else if (acf0_out) qfFree(acf0_out);//chkStoreAmplitudes

                if (acf1Amplitude) *acf1Amplitude=acf1_out;
                else if (acf1_out) qfFree(acf1_out);//chkStoreAmplitudes

                if (ccfAmplitude) *ccfAmplitude=ccf_out;
                else if (ccf_out) qfFree(ccf_out);//chkStoreAmplitudes

                if (acf0UCAmplitude) *acf0UCAmplitude=acf0UC_out;
                else if (acf0UC_out) qfFree(acf0UC_out);

                if (acf1UCAmplitude) *acf1UCAmplitude=acf1UC_out;
                else if (acf1UC_out) qfFree(acf1UC_out);

                if (ccfUCAmplitude) *ccfUCAmplitude=ccfUC_out;
                else if (ccfUC_out) qfFree(ccfUC_out);
                QApplication::restoreOverrideCursor();
                return true;
            } else {
                if (showErrorMessage) QMessageBox::critical(NULL, tr("Calculate Relative CCF"), tr("Can't calculate relative CCFs: Memory access error."));
                QApplication::restoreOverrideCursor();
                return false;
            }

        } else {
            if (showErrorMessage) QMessageBox::critical(NULL, tr("Calculate Relative CCF"), tr("Can't calculate relative CCFs: The two selected files contain images of different size."));
            QApplication::restoreOverrideCursor();
            return false;
        }

    } else {
        if (showErrorMessage) QMessageBox::critical(NULL, tr("Calculate Relative CCF"), tr("No files selected."));
        QApplication::restoreOverrideCursor();
        return false;
    }
    QApplication::restoreOverrideCursor();
    return true;
}

void QFFCCSRelativeCCFCrosstalkDialog::cmbCCF_currentIndexChanged(int index)
{
    if (ui->cmbCCF->currentRDR()){
        QList<QFRawDataRecord*> lsta=ui->cmbCCF->currentRDR()->getRecordsWithRoleFromGroup("acf0");
        if (!lsta.isEmpty()) {
            ui->cmbACF0->setCurrentRDR(lsta.first());
        } else {
            ui->cmbACF0->setCurrentRDR(0);
        }
        lsta=ui->cmbCCF->currentRDR()->getRecordsWithRoleFromGroup("acf1");
        if (!lsta.isEmpty()) {
            ui->cmbACF1->setCurrentRDR(lsta.first());
        } else {
            ui->cmbACF1->setCurrentRDR(0);
        }
    }

    replotImages();
}

void QFFCCSRelativeCCFCrosstalkDialog::cmbACF_currentIndexChanged(int /*index*/)
{
    //JKAutoOutputTimer tim(QString("cmbACF_currentIndexChanged"));
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    replotImages();
    QApplication::restoreOverrideCursor();
}


void QFFCCSRelativeCCFCrosstalkDialog::replotImages()
{
    //JKAutoOutputTimer tim(QString("replotImages"));
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->widOverviewACF0->setRDR(ui->cmbACF0->currentRDR());
    ui->widOverviewACF1->setRDR(ui->cmbACF1->currentRDR());
    ui->widOverviewCCF->setRDR(ui->cmbCCF->currentRDR());

    ui->pltData0->set_doDrawing(false);
    ui->pltData0->clearGraphs();
    ui->pltData1->set_doDrawing(false);
    ui->pltData1->clearGraphs();
    double* rel0=NULL;
    double* rel0_error=NULL;
    double* rel1=NULL;
    double* rel1_error=NULL;
    int runs=0;
    ui->labError->setText("");
    if (calculateRelCCF(ui->cmbACF0->currentRDR(), ui->cmbACF1->currentRDR(), ui->cmbCCF->currentRDR(), &rel0, &rel0_error, &rel1, &rel1_error,NULL,NULL,NULL,NULL,NULL,NULL, runs, ui->spinAvg->value(), ui->spinCrosstalk->value()/100.0, ui->cmbCrosstalkDirection->currentIndex(), false, ui->cmbAmplitudeSource->currentIndex(), ui->cmbACF0ResultSet->currentEvaluationGroup(), ui->cmbACF1ResultSet->currentEvaluationGroup(), ui->cmbCCFResultSet->currentEvaluationGroup(), "fitparam_g0", ui->chkBackground->isChecked())) {

        int w=runs, h=1;
        for (int i=0; i<2; i++) {
            QFPlotter* pltData=ui->pltData0;
            double* rel=rel0;
            double* rel_error=rel0_error;
            if (i==1) {
                pltData=ui->pltData1;
                rel=rel1;
                rel_error=rel1_error;
            }

            JKQTPdatastore* ds=pltData->getDatastore();
            ds->clear();
            size_t col=ds->addCopiedColumn(rel, runs, tr("rel. CCF amplitude"));
            size_t cole=ds->addCopiedColumn(rel_error, runs, tr("rel. CCF amplitude error"));
            plt=new JKQTPColumnMathImage(pltData->get_plotter());
            plt->set_palette(JKQTPMathImageMATLAB);
            plt->set_imageColumn(col);
            plt->set_Nx(w);
            plt->set_Ny(h);
            plt->set_x(0);
            plt->set_y(0);
            plt->set_width(w);
            plt->set_height(h);
            plt->set_showColorBar(true);
            plt->set_autoImageRange(false);
            plt->set_imageMin(0);
            plt->set_imageMax(1);
            bool showTop=w>h;
            plt->set_colorBarTopVisible(showTop);
            plt->set_colorBarRightVisible(!showTop);
            plt->set_title("");
            pltData->get_plotter()->set_showKey(false);
            pltData->get_plotter()->set_axisAspectRatio(double(w)/double(h));
            pltData->get_plotter()->set_aspectRatio(double(w)/double(h));
            pltData->get_plotter()->set_maintainAspectRatio(true);
            pltData->get_plotter()->set_maintainAxisAspectRatio(true);
            pltData->addGraph(plt);
            pltData->setAbsoluteXY(0,w,0,h);
            pltData->setXY(0,w,0,h);

            if (rel) qfFree(rel);
            if (rel_error) qfFree(rel_error);
        }
    } else {
        ui->labError->setText("<font color=\"red\"><b>ERROR CALCULATING REL: CCF</b></font>");
    }
    //ui->pltData->zoomToFit();
    ui->pltData0->set_doDrawing(true);
    ui->pltData0->update_plot();
    ui->pltData1->set_doDrawing(true);
    ui->pltData1->update_plot();
    QApplication::restoreOverrideCursor();
}

void QFFCCSRelativeCCFCrosstalkDialog::addResult()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    double* rel0;
    double* rel0_error;
    double* rel1;
    double* rel1_error;
    double *acf0Amplitude;
    double *acf0UCAmplitude;
    double *acf1Amplitude;
    double *acf1UCAmplitude;
    double *ccfAmplitude;
    double *ccfUCAmplitude;
    int runs=0;
    QFRawDataRecord* acf0=getACF0();
    QFRawDataRecord* acf1=getACF1();
    QFRawDataRecord* ccf=getCCF();
    QVector<double> acf10, acf01;
    if (acf0&&acf1&&ccf&&calculateRelCCF(acf0, acf1, ccf, &rel0, &rel0_error, &rel1, &rel1_error, &acf0Amplitude, &acf0UCAmplitude, &acf1Amplitude, &acf1UCAmplitude, &ccfAmplitude, &ccfUCAmplitude, runs, ui->spinAvg->value(), ui->spinCrosstalk->value()/100.0, ui->cmbCrosstalkDirection->currentIndex(), true, ui->cmbAmplitudeSource->currentIndex(), ui->cmbACF0ResultSet->currentEvaluationGroup(), ui->cmbACF1ResultSet->currentEvaluationGroup(), ui->cmbCCFResultSet->currentEvaluationGroup(), "fitparam_g0", ui->chkBackground->isChecked())) {
        //qDebug()<<"store: "<<acf0Amplitude<<acf0UCAmplitude<<acf1Amplitude<<acf1UCAmplitude<<ccfAmplitude<<ccfUCAmplitude;
        acf01=acf10=QVector<double>(runs, 0.0);
        if (acf0Amplitude && acf1Amplitude) {
            for (int i=0; i<runs; i++) {
                acf01[i]=acf0Amplitude[i]/acf1Amplitude[i];
                acf10[i]=acf1Amplitude[i]/acf0Amplitude[i];
            }
        }
        bool cleared=false;
        for (int i=0; i<2; i++) {
            QFRawDataRecord* acf=acf0;
            double* rel=rel0;
            double* rel_error=rel0_error;
            if (i==1) {
                acf=acf1;
                rel=rel1;
                rel_error=rel1_error;
            }

            QString BackMod="", BackModFull="";
            if (ui->chkBackground->isChecked()) {
                BackMod=QString("_backcorr");
                BackModFull=tr(", background-corr.");
            }
            QString evalName=QString("CalcRelCCFCrosstalkCorrected%5_ACF%1_ACF%2_CCF_ct%3_avg%4").arg(acf0->getID()).arg(acf1->getID()).arg(ui->spinCrosstalk->value()).arg(ui->spinAvg->value()).arg(BackMod);
            QString egroup=QString("CalcRelCCFCrosstalkCorrected%3_CCF_ct%1_avg%2").arg(ui->spinCrosstalk->value()).arg(ui->spinAvg->value()).arg(BackMod);
            if (ui->cmbAmplitudeSource->currentIndex()==1) {
                evalName=QString("CalcRelCCFCrosstalkCorrected%4_ACF%1_ACF%2_CCF_ct%3_g0").arg(acf0->getID()).arg(acf1->getID()).arg(ui->spinCrosstalk->value()).arg(BackMod);
                egroup=QString("CalcRelCCFCrosstalkCorrected%2__CCF_ct%1_g0").arg(ui->spinCrosstalk->value()).arg(BackMod);
            }
            QString group="results";
            QString acfName=acf->getRole();
            if (acfName.isEmpty()) acfName=acf->getName();
            QString ccfName=ccf->getRole();
            if (ccfName.isEmpty()) ccfName=ccf->getName();
            QString egrouplabel=tr("rel. CCF amlitude (avg. %3 taus), %1%2 crosstalk corrected%4").arg(ui->spinCrosstalk->value()).arg("%").arg(ui->spinAvg->value()).arg(BackModFull);
            QString rnbase=QString("relative_ccf_acf%1").arg(i);
            QString rnbase_simple=QString("");
            if (ui->cmbAmplitudeSource->currentIndex()==1) {
                egrouplabel=tr("rel. CCF g(0) amlitude, %1%2 crosstalk corrected%3").arg(ui->spinCrosstalk->value()).arg("%").arg(BackModFull);
                rnbase=QString("relative_ccf_acf%1").arg(i);
            }
            QString rn=rnbase+"_g0_amplitude";

            if (!cleared) {
                ccf->resultsClear(evalName);
                cleared=true;
            }

            ccf->resultsSetEvaluationGroup(evalName, egroup);
            ccf->resultsSetEvaluationGroupLabel(egroup, egrouplabel);
            ccf->resultsSetEvaluationGroupIndex(evalName, -1);

            ccf->resultsSetNumberErrorList(evalName, rn, rel, rel_error, runs);
            ccf->resultsSetLabel(evalName, rn, tr("relative amplitude: CCF/%1").arg(acfName));
            ccf->resultsSetGroup(evalName, rn, group);


            if (ui->chkStoreAmplitudes->isChecked()) {
                QString rn;
                rn=rnbase_simple+"acf0_amplitude";
                //qDebug()<<rn<<acf0Amplitude;
                if (acf0Amplitude){

                    ccf->resultsSetNumberList(evalName, rn, acf0Amplitude, runs);
                    ccf->resultsSetLabel(evalName, rn, tr("ACF0 amplitude, corrected"));
                    ccf->resultsSetGroup(evalName, rn, group);

                }
                rn=rnbase_simple+"acf0_amplitude_uncorrected";
                //qDebug()<<rn<<acf0UCAmplitude;

                if (acf0UCAmplitude) {
                    ccf->resultsSetNumberList(evalName, rn, acf0UCAmplitude, runs);
                    ccf->resultsSetLabel(evalName, rn, tr("ACF0 amplitude, uncorrected"));
                    ccf->resultsSetGroup(evalName, rn, group);

                }
                rn=rnbase_simple+"acf1_amplitude";
                //qDebug()<<rn<<acf1Amplitude;

                if (acf1Amplitude) {
                    ccf->resultsSetNumberList(evalName, rn, acf1Amplitude, runs);
                    ccf->resultsSetLabel(evalName, rn, tr("ACF1 amplitude, corrected"));
                    ccf->resultsSetGroup(evalName, rn, group);

                }
                rn=rnbase_simple+"acf1_amplitude_uncorrected";
                //qDebug()<<rn<<acf1UCAmplitude;

                if (acf1UCAmplitude) {
                    ccf->resultsSetNumberList(evalName, rn, acf1UCAmplitude, runs);
                    ccf->resultsSetLabel(evalName, rn, tr("ACF1 amplitude, uncorrected"));
                    ccf->resultsSetGroup(evalName, rn, group);

                }
                rn=rnbase_simple+"ccf_amplitude";
                //qDebug()<<rn<<ccfAmplitude;

                if (ccfAmplitude) {
                    ccf->resultsSetNumberList(evalName, rn, ccfAmplitude, runs);
                    ccf->resultsSetLabel(evalName, rn, tr("CCF amplitude, corrected"));
                    ccf->resultsSetGroup(evalName, rn, group);

                }
                rn=rnbase_simple+"ccf_amplitude_uncorrected";
                //qDebug()<<rn<<ccfUCAmplitude;

                if (ccfUCAmplitude) {
                    ccf->resultsSetNumberList(evalName, rn, ccfUCAmplitude, runs);
                    ccf->resultsSetLabel(evalName, rn, tr("CCF amplitude, uncorrected"));
                    ccf->resultsSetGroup(evalName, rn, group);
                }
                rn=rnbase_simple+"acf1_div_acf0";
                ccf->resultsSetNumberList(evalName, rn, acf10.data(), acf10.size());
                ccf->resultsSetLabel(evalName, rn, tr("ACF1/ACF0 amplitude, corrected"));
                ccf->resultsSetGroup(evalName, rn, group);
                rn=rnbase_simple+"acf0_div_acf1";
                ccf->resultsSetNumberList(evalName, rn, acf01.data(), acf01.size());
                ccf->resultsSetLabel(evalName, rn, tr("ACF0/ACF1 amplitude, corrected"));
                ccf->resultsSetGroup(evalName, rn, group);
            }


            ccf->resultsSetInteger(evalName, "acf0_file_id", acf0->getID());
            ccf->resultsSetString(evalName, "acf0_file_role", acf0->getRole());
            ccf->resultsSetString(evalName, "acf0_file_name", acf0->getName());
            ccf->resultsSetInteger(evalName, "acf1_file_id", acf1->getID());
            ccf->resultsSetString(evalName, "acf1_file_role", acf1->getRole());
            ccf->resultsSetString(evalName, "acf1_file_name", acf1->getName());
            ccf->resultsSetInteger(evalName, "ccf_file_id", ccf->getID());
            ccf->resultsSetString(evalName, "ccf_file_role", ccf->getRole());
            ccf->resultsSetString(evalName, "ccf_file_name", ccf->getName());
            ccf->resultsSetBoolean(evalName, "background_correction", ui->chkBackground->isChecked());
            ccf->resultsSetString(evalName, "crosstalk_direction", ui->cmbCrosstalkDirection->currentText());
            ccf->resultsSetNumber(evalName, "crosstalk", ui->spinCrosstalk->value()/100.0);
            if (ui->cmbAmplitudeSource->currentIndex()==0) {
                ccf->resultsSetString(evalName, "amplitude_source", tr("CF channel average"));
                ccf->resultsSetInteger(evalName, "average_channels", ui->spinAvg->value());
                ccf->resultsSetInteger(evalName, "average_channels", ui->spinAvg->value());
            } else {
                ccf->resultsSetString(evalName, "amplitude_source", tr("g(0) fit result"));
                ccf->resultsSetString(evalName, "acf0_resultset", ui->cmbACF0ResultSet->currentEvaluationGroup());
                ccf->resultsSetString(evalName, "acf1_resultset", ui->cmbACF1ResultSet->currentEvaluationGroup());
                ccf->resultsSetString(evalName, "ccf_resultset", ui->cmbCCFResultSet->currentEvaluationGroup());
            }



            if (rel) qfFree(rel);
            if (rel_error) qfFree(rel_error);
        }

        if(acf0Amplitude) qfFree(acf0Amplitude);
        if(acf0UCAmplitude) qfFree(acf0UCAmplitude);
        if(acf1Amplitude) qfFree(acf1Amplitude);
        if(acf1UCAmplitude) qfFree(acf1UCAmplitude);
        if(ccfAmplitude) qfFree(ccfAmplitude);
        if(ccfUCAmplitude) qfFree(ccfUCAmplitude);
    }
    QApplication::restoreOverrideCursor();
}

void QFFCCSRelativeCCFCrosstalkDialog::on_btnNextCCF_clicked()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    nextCCF();
    QApplication::restoreOverrideCursor();

}



bool QFFCCSRelativeCCFCrosstalkDialog::isACF(const QFRawDataRecord *rec)
{
    if (!rec) return false;
    return rec->getRole().toUpper().startsWith("ACF") || rec->getRole().toUpper().startsWith("FCS") || (rec->getRole().isEmpty() && (rec->getName().toLower().contains("acf") || rec->getName().toLower().contains("fcs")) );
}

bool QFFCCSRelativeCCFCrosstalkDialog::isCCF(const QFRawDataRecord *rec)
{
    if (!rec) return false;
    return rec->getRole().toUpper().contains("FCCS") || rec->getRole().toUpper().contains("DCCF")|| (rec->getRole().isEmpty() && (rec->getName().toLower().contains("fccs") || rec->getName().toLower().contains("dccf")) );
}


bool QFFCCSRelativeCCFCrosstalkDialog::nextCCF()
{
    QFProject* project=QFPluginServices::getInstance()->getCurrentProject();
    if (!(matchFunctor && project&&getCCF()) ) return false;
    QList<QPointer<QFRawDataRecord> > lst=matchFunctor->getFilteredList(project);
    int current=lst.indexOf(getCCF());
    if (current>=0 && current+1<lst.size()) {
        for (int i=current+1; i<lst.size(); i++) {
            if (isCCF(lst[i])) {
                ui->cmbCCF->setCurrentRDR(lst[i]);
                QList<QFRawDataRecord*> lsta=lst[i]->getRecordsWithRoleFromGroup("acf0");
                if (!lsta.isEmpty()) {
                    ui->cmbACF0->setCurrentRDR(lsta.first());
                } else {
                    ui->cmbACF0->setCurrentRDR(0);
                }
                lsta=lst[i]->getRecordsWithRoleFromGroup("acf1");
                if (!lsta.isEmpty()) {
                    ui->cmbACF1->setCurrentRDR(lsta.first());
                } else {
                    ui->cmbACF1->setCurrentRDR(0);
                }
                replotImages();
                return true;
            }
        }
    }
    return false;
}

bool QFFCCSRelativeCCFCrosstalkDialog::prevCCF()
{
    QFProject* project=QFPluginServices::getInstance()->getCurrentProject();
    if (!(matchFunctor && project&&getCCF()) ) return false;
    QList<QPointer<QFRawDataRecord> > lst=matchFunctor->getFilteredList(project);
    int current=lst.indexOf(getCCF());
    if (current>0 && current<lst.size()) {
        for (int i=current-1; i>=0; i--) {
            if (isCCF(lst[i])) {
                ui->cmbCCF->setCurrentRDR(lst[i]);
                QList<QFRawDataRecord*> lsta=lst[i]->getRecordsWithRoleFromGroup("acf0");
                if (!lsta.isEmpty()) {
                    ui->cmbACF0->setCurrentRDR(lsta.first());
                } else {
                    ui->cmbACF0->setCurrentRDR(0);
                }
                lsta=lst[i]->getRecordsWithRoleFromGroup("acf1");
                if (!lsta.isEmpty()) {
                    ui->cmbACF1->setCurrentRDR(lsta.first());
                } else {
                    ui->cmbACF1->setCurrentRDR(0);
                }
                replotImages();
                return true;
            }
        }
    }
    return false;
}



void QFFCCSRelativeCCFCrosstalkDialog::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayPluginHelpWindow("fccs_fit", "calc_ccf_crosstalk_amplitude.html");
}

void QFFCCSRelativeCCFCrosstalkDialog::on_cmbAmplitudeSource_currentIndexChanged(int index)
{
    ui->cmbACF0ResultSet->setEnabled(index>=0);
    ui->cmbACF1ResultSet->setEnabled(index>=0);
    ui->cmbCCFResultSet->setEnabled(index>=0);
    ui->spinAvg->setEnabled(index==0);
}







void QFFCCSRelativeCCFCrosstalkDialog::on_btnStoreDataAll_clicked()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QModernProgressDialog progress(tr("calculating relative CCF amplitudes ..."), tr("&Cancel"), this);
    progress.show();

    addResult();
    QApplication::processEvents();
    while (nextCCF()) {
        QApplication::processEvents();
        addResult();
        if (progress.wasCanceled()) break;
    }
    /*QFRawDataRecord* rdr=ui->cmbACF->currentRDR();
    QString role="";
    QFProject* project=QFPluginServices::getInstance()->getCurrentProject();
    if (rdr&&project) {
        role=rdr->getRole().toUpper();
        QList<QPointer<QFRawDataRecord> > lst=matchFunctor->getFilteredList(project);
        for (int i=0; i<lst.size(); i++) {
            if (lst[i] && isACF(lst[i])) {
                ui->cmbACF->setCurrentRDR(lst[i]);
                break;
            }
        }

    }
    QApplication::processEvents();
    cmbACF_currentIndexChanged(ui->cmbACF->currentIndex());
    addResult();
    while (nextCCF()) {
        cmbACF_currentIndexChanged(ui->cmbACF->currentIndex());
        //QApplication::processEvents();
        addResult();
        if (progress.wasCanceled()) break;
    }*/
    QApplication::restoreOverrideCursor();
}

