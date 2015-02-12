/*
    Copyright (c) 2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate: 2014-09-02 17:43:02 +0200 (Di, 02 Sep 2014) $  (revision $Rev: 3433 $)

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



#include "qfevalcameracalibration_item.h"
#include "qfevalcameracalibration_editor.h"
#include "qfrdrimagestackinterface.h"
#include "qfrdrimagemask.h"
#include "qftools.h"
#include "statistics_tools.h"
#include <typeinfo>

#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include <QtCore>


QFEvalCameraCalibrationItem::QFEvalCameraCalibrationItem(QFProject* parent):
    QFEvaluationItem(parent, true, true)
{

}

QFEvalCameraCalibrationItem::~QFEvalCameraCalibrationItem() {

}


void QFEvalCameraCalibrationItem::intWriteData(QXmlStreamWriter& w) {
    /* store global evaluation parameters / initial values or anything else to the project tree here */
}

void QFEvalCameraCalibrationItem::intReadData(QDomElement* e) {
    /* read back anything you've written in intWriteData() in this function */
}


QFEvaluationEditor* QFEvalCameraCalibrationItem::createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent) {
    return new QFEvalCameraCalibrationEditor(services, propEditor, parent);
}

bool QFEvalCameraCalibrationItem::isApplicable(const QFRawDataRecord* record) const {
    /* CHECK WHETHER A GIVEN RECORD MAY BE USED TOGETHER WITH THE EVALUATION */
    return record->inherits("QFRDRImageStackInterface"); //dynamic_cast<const QFRDRImageStackInterface*>(record);
}

bool QFEvalCameraCalibrationItem::hasEvaluation(QFRawDataRecord* r1) {
    QFRawDataRecord* r=r1;
    QString rsid=getEvaluationResultID();
    return r->resultsExistsFromEvaluation(rsid);
}

QString QFEvalCameraCalibrationItem::getEvaluationResultID()
{
    return QString("%1_%2").arg(getType()).arg(getID());
}


void QFEvalCameraCalibrationItem::doEvaluation(QProgressDialog* dlgEvaluationProgress) {
    QApplication::processEvents();
    QApplication::processEvents();

    if (dlgEvaluationProgress && dlgEvaluationProgress->wasCanceled()) return; // canceled by user ?

    int backFrame=getQFProperty("EVAL_BACK_FRAMERDR", -1).toInt();
    if (!getQFProperty("EVAL_BACK_FRAMERDR_AVAILABLE", true).toBool()) backFrame=-1;


    QList<QPointer<QFRawDataRecord> > rdrs_in=getApplicableRecords();
    QList<QFRDRImageStackInterface*> rdrs;
    QFRDRImageStackInterface* rdr_back=NULL;
    for (int i=0; i<rdrs_in.size(); i++) {
        QFRDRImageStackInterface* intf=dynamic_cast<QFRDRImageStackInterface*>(rdrs_in[i].data());
        if (intf) {
            if (rdrs_in[i]->getID()==backFrame) {
                rdr_back=intf;
            } else if (isSelected(rdrs_in[i])) {
                rdrs<<intf;
            }
        }
    }
    int stack=getQFProperty("EVAL_STACK", 0).toInt();
    int channel=getQFProperty("EVAL_CHANNEL", 0).toInt();
    int mode=getQFProperty("EVAL_MODE", 0).toInt();
                // 0: average over each image stack
                // 1: put every pixel into the output

    bool backCorrect=getQFProperty("EVAL_BACK_CORRECTION", true).toBool();
    double excessNoise=getQFProperty("EVAL_EXCESSNOISE", 1).toDouble();
    double backOffset=getQFProperty("EVAL_BACKGROUND_OFFSET", 0).toDouble();

    QFRawDataRecord* recWrite=NULL;

    // write back fit results to record!
    for (int i=0; i<rdrs.size(); i++) {
        QFRawDataRecord* record=dynamic_cast<QFRawDataRecord*>(rdrs[i]);
        if (record) {
            recWrite=record;
            break;
        }
    }

    if (dlgEvaluationProgress) dlgEvaluationProgress->setRange(0, rdrs.size()+1);
    QVector<double> avgb, varb;
    QVector<bool> back_mask;
    if (rdr_back) {
        uint64_t w=rdr_back->getImageStackWidth(stack);
        uint64_t h=rdr_back->getImageStackHeight(stack);
        uint64_t t=rdr_back->getImageStackFrames(stack);
        double* img=rdr_back->getImageStack(stack, 0, channel);
        bool* mask=NULL;
        QFRDRImageMaskInterface* intfm=dynamic_cast<QFRDRImageMaskInterface*>(rdr_back);
        if (intfm) {
            mask=intfm->maskGet();
            if (intfm->maskGetWidth()!=w || intfm->maskGetHeight()!=h) {
                mask=NULL;
            }
        }
        for (uint64_t i=0; i<w*h; i++) {
            QVector<double> d;
            for (uint64_t ti=0; ti<t; ti++) {
                d<<img[ti*w*h+i];
            }
            double s;
            avgb<<qfstatisticsAverageStd(s, d);
            varb<<s;
            if (mask) {
                back_mask<<mask[i];
            } else {
                back_mask<<false;
            }
        }
    }

    QVector<double> res_meanI, res_varI, res_meanIErr, res_varIErr, res_SNR, res_SNRErr, res_meanIE, res_meanIEErr;

    if (mode==0) {
        // flat field illumination, so we average over each image stack
        for (int r=0; r<rdrs.size(); r++) {
            if (dlgEvaluationProgress) dlgEvaluationProgress->setValue(r+1);
            QFRawDataRecord* rdr_cast=dynamic_cast<QFRawDataRecord*>(rdrs[r]);
            if (rdr_cast && dlgEvaluationProgress) dlgEvaluationProgress->setLabelText(rdr_cast->getName());
            QApplication::processEvents();

            if (rdrs[r]!=rdr_back) {
                QVector<double> avgv, varv;
                uint64_t w=rdrs[r]->getImageStackWidth(stack);
                uint64_t h=rdrs[r]->getImageStackHeight(stack);
                uint64_t t=rdrs[r]->getImageStackFrames(stack);
                //const double* img=rdrs[r]->getImageStack(stack, 0, channel);
                bool* mask=NULL;
                QFRDRImageMaskInterface* intfm=dynamic_cast<QFRDRImageMaskInterface*>(rdrs[r]);
                if (intfm) {
                    mask=intfm->maskGet();
                    if (intfm->maskGetWidth()!=w || intfm->maskGetHeight()!=h) {
                        mask=NULL;
                    }
                }
                for (uint64_t i=0; i<w*h; i++) {
                    if (!mask || !mask[i]){
                        QVector<double> d;
                        double backC=avgb.value(i, 0.0)+backOffset;
                        if (!backCorrect) backC=0;

                        for (uint64_t ti=0; ti<t; ti++) {
                            const double* timg=rdrs[r]->getImageStack(stack, ti, channel);
                            d<<(timg[i]-backC);
                        }
                        double s=0;
                        avgv<<qfstatisticsAverageVariance(s, d);
                        varv<<s;
                    }
                }
                double sv=0;
                res_meanI<<qfstatisticsAverageStd(sv,avgv);
                res_meanIErr<<sv;
                sv=0;
                res_varI<<qfstatisticsAverageStd(sv,varv);
                res_varIErr<<sv;
            }


            QApplication::processEvents();
            if (dlgEvaluationProgress && dlgEvaluationProgress->wasCanceled()) return; // canceled by user ?
        }
    } else if (mode==1) {
        // image contain different grey levels, so we use mean/var from every single pixel!
        for (int r=0; r<rdrs.size(); r++) {
            if (dlgEvaluationProgress) dlgEvaluationProgress->setValue(r+1);
            QFRawDataRecord* rdr_cast=dynamic_cast<QFRawDataRecord*>(rdrs[r]);
            if (rdr_cast && dlgEvaluationProgress) dlgEvaluationProgress->setLabelText(rdr_cast->getName());
            QApplication::processEvents();

            if (rdrs[r]!=rdr_back) {
                QVector<double> avgv, varv;
                uint64_t w=rdrs[r]->getImageStackWidth(stack);
                uint64_t h=rdrs[r]->getImageStackHeight(stack);
                uint64_t t=rdrs[r]->getImageStackFrames(stack);
                //const double* img=rdrs[r]->getImageStack(stack, 0, channel);
                bool* mask=NULL;
                QFRDRImageMaskInterface* intfm=dynamic_cast<QFRDRImageMaskInterface*>(rdrs[r]);
                if (intfm) {
                    mask=intfm->maskGet();
                    if (intfm->maskGetWidth()!=w || intfm->maskGetHeight()!=h) {
                        mask=NULL;
                    }
                }
                for (uint64_t i=0; i<w*h; i++) {
                    if (!mask || !mask[i]){
                        QVector<double> d;
                        double backC=avgb.value(i, 0.0)+backOffset;
                        if (!backCorrect) backC=0;

                        for (uint64_t ti=0; ti<t; ti++) {
                            const double* timg=rdrs[r]->getImageStack(stack, ti, channel);
                            d<<(timg[i]-backC);
                        }
                        double s=0;
                        res_meanI<<qfstatisticsAverageVariance(s, d);
                        res_varI<<s;
                    }
                }
            }


            QApplication::processEvents();
            if (dlgEvaluationProgress && dlgEvaluationProgress->wasCanceled()) return; // canceled by user ?
        }
    }



    if (recWrite) {
        recWrite->disableEmitResultsChanged();
        QString evalID=getEvaluationResultID();
        recWrite->resultsClear(evalID);
        QString rn;
        recWrite->resultsSetNumberList(evalID, rn="intensity_avg", res_meanI);
        recWrite->resultsSetNumberList(evalID, rn="intensity_var", res_varI);
        if (res_meanIErr.size()>0) recWrite->resultsSetNumberList(evalID, rn="intensity_avg_error", res_meanIErr);
        if (res_varIErr.size()>0) recWrite->resultsSetNumberList(evalID, rn="intensity_var_error", res_varIErr);
        recWrite->resultsSetNumberList(evalID, rn="background_avg_image", avgb);
        recWrite->resultsSetNumberList(evalID, rn="background_var_image", varb);
        recWrite->resultsSetNumber(evalID, rn="background_avg", qfstatisticsMaskedAverage(back_mask, avgb, false));
        recWrite->resultsSetNumber(evalID, rn="background_var", qfstatisticsMaskedAverage(back_mask, varb, false));
        recWrite->resultsSetBoolean(evalID, rn="background_corrected", backCorrect);


        if (res_meanI.size()>0 && res_varI.size()>0) {
            QVector<double> weights;
            if (res_meanIErr.size()>0 && res_varIErr.size()>0) {
                qfstatisticsSort(res_meanI.data(), res_varI.data(), res_meanIErr.data(), res_varIErr.data(), res_meanI.size());
                double wsum=0;
                double wcnt=0;
                for (int i=0; i<res_varIErr.size(); i++) {
                    double w=1.0/res_varIErr[i];
                    weights<<w;
                    if (QFFloatIsOK(w)) {
                        wsum=wsum+w;
                        wcnt++;
                    }
                }
                wsum=wsum/double(wcnt);
                if (wcnt<1) {
                    weights.clear();
                } else {
                    for (int i=0; i<weights.size(); i++) {
                        if (!QFFloatIsOK(weights[i])) {
                            weights[i]=wsum;
                        }
                    }
                }
            } else {
                qfstatisticsSort(res_meanI.data(), res_varI.data(), res_meanI.size());
            }
            double cOffset=0, cSlope=0;
            double cFac=1;
            if (weights.size()>0) {
                statisticsLinearWeightedRegression(res_meanI.data(), res_varI.data(), weights.data(), res_meanI.size(), cOffset, cSlope);
            } else {
                statisticsLinearRegression(res_meanI.data(), res_varI.data(), res_meanI.size(), cOffset, cSlope);
            }
            recWrite->resultsSetNumber(evalID, rn="fullfit_offset", cOffset, tr("ADU^2"));
            recWrite->resultsSetNumber(evalID, rn="fullfit_slope", cSlope, tr("ADU"));
            recWrite->resultsSetNumber(evalID, rn="fullfit_gain", cSlope/excessNoise, tr("ADU/electron"));
            recWrite->resultsSetNumber(evalID, rn="fullfit_conversion_factor", cFac=1.0/(cSlope/excessNoise), tr("electron/ADU"));
            recWrite->resultsSetNumber(evalID, rn="fullfit_readnoise", sqrt(cOffset), tr("ADU"));
            recWrite->resultsSetNumber(evalID, rn="fullfit_readnoise_electrons", sqrt(cOffset)*cFac, tr("electrons"));

            cOffset=0, cSlope=0;
            if (weights.size()>0) {
                statisticsLinearWeightedRegression(res_meanI.data(), res_varI.data(), weights.data(), res_meanI.size()/2, cOffset, cSlope);
            } else {
                statisticsLinearRegression(res_meanI.data(), res_varI.data(), res_meanI.size()/2, cOffset, cSlope);
            }
            recWrite->resultsSetNumber(evalID, rn="startfit_offset", cOffset, tr("ADU^2"));
            recWrite->resultsSetNumber(evalID, rn="startfit_slope", cSlope, tr("ADU"));
            recWrite->resultsSetNumber(evalID, rn="startfit_gain", cSlope/excessNoise, tr("ADU/electron"));
            recWrite->resultsSetNumber(evalID, rn="startfit_conversion_factor", cFac=1.0/(cSlope/excessNoise), tr("electron/ADU"));
            recWrite->resultsSetNumber(evalID, rn="startfit_readnoise", sqrt(cOffset), tr("ADU"));
            recWrite->resultsSetNumber(evalID, rn="startfit_readnoise_electrons", sqrt(cOffset)*cFac, tr("electrons"));

            for (int i=0; i<res_meanI.size(); i++) {
                res_meanIE<<(res_meanI.value(i, 0)*cFac);
                res_meanIEErr<<(res_meanIErr.value(i, 0)*cFac);
                res_SNR<<(res_meanI.value(i, 0)/sqrt(res_varI.value(i, 0)));
                res_SNRErr<<qfErrorSqrtSumSqr(res_meanIErr.value(i, 0)/sqrt(res_varI.value(i, 0)), res_varIErr.value(i)*res_meanI.value(i, 0)/2.0/pow(res_varI.value(i,0), 3.0/2.0));
            }

            recWrite->resultsSetNumberList(evalID, rn="intensity_avg_elec", res_meanIE);
            recWrite->resultsSetNumberList(evalID, rn="snr", res_SNR);
            if (res_meanIErr.size()>0 && res_varIErr.size()>0) {
                recWrite->resultsSetNumberList(evalID, rn="intensity_avg_elec_error", res_meanIEErr);
                recWrite->resultsSetNumberList(evalID, rn="snr_error", res_SNRErr);
            }

        }


        recWrite->enableEmitResultsChanged();
    }
    //services->log_text(tr("evaluation complete\n"));

}
