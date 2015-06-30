/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#include "qfspimlightsheetevaluation_item.h"
#include "qfspimlightsheetevaluation_editor.h"
#include "qfrdrimagestackinterface.h"
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





QFSPIMLightsheetEvaluationItem::QFSPIMLightsheetEvaluationItem(QFProject* parent):
    QFEvaluationItem(parent, true, false)
{

}

QFSPIMLightsheetEvaluationItem::~QFSPIMLightsheetEvaluationItem() {

}


void QFSPIMLightsheetEvaluationItem::intWriteData(QXmlStreamWriter&/* w*/) const {
    /* store global evaluation parameters / initial values or anything else to the project tree here */
}

void QFSPIMLightsheetEvaluationItem::intReadData(QDomElement* /*e*/) {
    /* read back anything you've written in intWriteData() in this function */
}


QFEvaluationEditor* QFSPIMLightsheetEvaluationItem::createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent) {
    return new QFSPIMLightsheetEvaluationEditor(services, propEditor, parent);
}

bool QFSPIMLightsheetEvaluationItem::isApplicable(const QFRawDataRecord *record) const {
    /* CHECK WHETHER A GIVEN RECORD MAY BE USED TOGETHER WITH THE EVALUATION */
    return (dynamic_cast<const QFRDRImageStackInterface*>(record)!=NULL);
}

bool QFSPIMLightsheetEvaluationItem::hasEvaluation(QFRawDataRecord* r1, int stack, int channel) const {
    QFRawDataRecord* r=r1;
    QString rsid=getEvaluationResultID(stack, channel);
    return r->resultsExistsFromEvaluation(rsid);
}

QString QFSPIMLightsheetEvaluationItem::getEvaluationResultID(int stack, int channel) const
{
    if (channel<0) {
        return QString("%1_%2_stack%3").arg(getType()).arg(getID()).arg(stack);
    } else {
        return QString("%1_%2_stack%3_ch%4").arg(getType()).arg(getID()).arg(stack).arg(channel);
    }
}















/*double QFSPIMLightsheetEvaluationItem_fGauss( double t, const double *p )
{
    const double offset=p[0];
    const double A=p[1];
    const double avg=p[2];
    const double var=p[3];
    return offset+A*exp(-2.0*(t-avg)*(t-avg)/var);
}*/

void QFSPIMLightsheetEvaluationItem::doEvaluation(QFRawDataRecord *record, int stack, int stack_pos, int channel, double deltaX, double deltaZ, QFFitFunction* model, QFFitAlgorithm* algorithm, Orientation orientation, bool useMask) const {
    QString resultID=getEvaluationResultID(stack, channel);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    QFRDRImageMaskInterface* dataM=qobject_cast<QFRDRImageMaskInterface*>(record);
    if (data) {
        int w=data->getImageStackWidth(stack);
        int h=data->getImageStackHeight(stack);
        double* img=(double*)qfMalloc(w*h*sizeof(double));
        bool* mask=(bool*)qfMalloc(w*h*sizeof(bool));
        for (int i=0; i<w*h; i++) mask[i]=false;
        bool* maskD=NULL;
        if (useMask && dataM) {
            maskD=dataM->maskGet();
        }
        int wi=w;
        int hi=h;
        //qDebug()<<useMask<<dataM<<maskD;
        if (orientation==QFSPIMLightsheetEvaluationItem::fitColumns) {
            // rotate image by 90degree
            double* m=data->getImageStack(stack, stack_pos, channel);
            for (int x=0; x<w; x++) {
                for (int y=0; y<h; y++) {
                    img[x*h+y]=m[y*w+x];
                }
            }
            if (maskD) {
                for (int x=0; x<w; x++) {
                    for (int y=0; y<h; y++) {
                        mask[x*h+y]=maskD[y*w+x];
                    }
                }
            }
            wi=h;
            hi=w;
        } else {
            double* m=data->getImageStack(stack, stack_pos, channel);
            for (int x=0; x<w*h; x++) {
                img[x]=m[x];
            }
            if (maskD) {
                for (int x=0; x<w*h; x++) {
                    mask[x]=maskD[x];
                }
            }
        }


        QStringList paramIDs=model->getParameterIDs();
        QList<QVector<double> > values, errors, avgValues;
        QVector<bool> fitOK;
        while (values.size()<paramIDs.size()) {
            QVector<double> valEmpty;
            values.append(valEmpty);
            errors.append(valEmpty);
            avgValues.append(valEmpty);
        }

        double scaleI=1.0;

        double* dataX=(double*)qfMalloc(wi*sizeof(double));
        double* dataY=(double*)qfMalloc(wi*sizeof(double));
        for (int f=0; f<hi; f++) {
            int data_count=0;
            for (int i=0; i<wi; i++) {
                if (!mask[f*wi+i]) {
                    dataX[data_count]=i;
                    dataY[data_count]=img[f*wi+i];
                    data_count++;
                }
            }
            //qDebug()<<data_count<<wi;

             if (data_count>0 && getQFProperty("FIT_SCALE_INTENSITY", true).toBool()) {
                 scaleI=statisticsMax(dataY, data_count)/double(wi);
                 for (int i=0; i<data_count; i++) {
                     dataY[i]=dataY[i]/scaleI;
                 }
             }


            int pcount=model->paramCount();
            double* parIn=(double*)qfMalloc(pcount*sizeof(double));
            double* paramOut=(double*)qfMalloc(pcount*sizeof(double));
            double* paramErrOut=(double*)qfMalloc(pcount*sizeof(double));

            double par[4];
            long long maxpos=0;
            par[0]=statisticsMin(dataY, data_count);
            par[1]=fabs(statisticsMax(dataY, data_count, &maxpos)-par[0]);
            par[2]=maxpos;
            par[3]=double(wi)/10.0;

            for (int i=0; i<paramIDs.size(); i++) {
                QFFitFunction::ParameterDescription d=model->getDescription(paramIDs[i]);
                parIn[i]=d.initialValue;
                if (paramIDs[i].toUpper()=="OFFSET") parIn[i]=par[0];
                if (paramIDs[i].toUpper()=="AMPLITUDE") parIn[i]=par[1];
                if (paramIDs[i].toUpper()=="POSITION") parIn[i]=par[2];
                if (paramIDs[i].toUpper()=="WIDTH") parIn[i]=par[3];

            }

            if (getQFProperty("ESTIMATE_INITIAL_FF", true).toBool()) model->estimateInitial(parIn, dataX, dataY, data_count);

            /*QString ps;
            for (int i=0; i<paramIDs.size(); i++) {
                ps=ps+QString("%1=%2,  ").arg(paramIDs[i]).arg(parIn[i]);
            }
            qDebug()<<"=== INIT: "<<ps;*/
            //lmcurve_fit(4, par, wi, dataX, dataY, QFSPIMLightsheetEvaluationItem_fGauss, &control, &status);


            QVector<double> pMin=model->getInitialParamMins();
            QVector<double> pMax=model->getInitialParamMaxs();


            QFFitAlgorithm::FitResult res=algorithm->fit(paramOut, paramErrOut, dataX, dataY, NULL, data_count, model, parIn, NULL,pMin.data(),pMax.data());
            //res=algorithm->fit(paramOut, paramErrOut, dataX, dataY, NULL, data_count, model, paramOut, NULL,pMin.data(),pMax.data());
            fitOK.append(res.fitOK);
            /*ps="";
            for (int i=0; i<paramIDs.size(); i++) {
                ps=ps+QString("%1=%2,  ").arg(paramIDs[i]).arg(paramOut[i]);
            }
            qDebug()<<"=== FIT: "<<ps<<"\n\n";*/
            //qDebug()<<f<<res.fitOK<<": offset="<<paramOut[0]<<" A="<<paramOut[1]<<" pos="<<paramOut[2]<<" width="<<paramOut[3];

            for (int i=0; i<paramIDs.size(); i++) {
                if (paramIDs[i].toUpper()=="AMPLITUDE") {
                    paramOut[i]=paramOut[i]*scaleI;
                    paramErrOut[i]=paramErrOut[i]*scaleI;
                }
                if (paramIDs[i].toUpper()=="OFFSET") {
                    paramOut[i]=paramOut[i]*scaleI;
                    paramErrOut[i]=paramErrOut[i]*scaleI;
                }
                values[i].append(paramOut[i]);
                errors[i].append(paramErrOut[i]);
                if (res.fitOK && data_count>4) avgValues[i].append(paramOut[i]);
            }
            /*offsets.append(par[0]);
            amplitudes.append(par[1]);
            avgs.append(par[2]);
            //widths.append(sqrt(par[3]));
            widths.append(par[3]);*/

            qfFree(parIn);
            qfFree(paramOut);
            qfFree(paramErrOut);
        }

        QString param;
        QString group=QString("fit_results");
        //qDebug()<<"writing data to record ...";

        record->resultsSetBooleanList(resultID, param=QString("fitok_frame%1").arg(stack_pos), fitOK);
        record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, param, "", false);
        for (int i=0; i<paramIDs.size(); i++) {
            QFFitFunction::ParameterDescription d=model->getDescription(paramIDs[i]);
            record->resultsSetNumberErrorList(resultID, param=QString("%2_frame%1").arg(stack_pos).arg(paramIDs[i]), values[i], errors[i], d.unit);
            record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, QString("%2, frame%1").arg(stack_pos).arg(d.name), QString("%2, frame %1").arg(stack_pos).arg(d.label), true);

            double v=0;
            double m=qfstatisticsAverageVariance(v, avgValues[i]);
            record->resultsSetInNumberErrorList(resultID, param=paramIDs[i], stack_pos, m, v, "");
            record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, d.name, d.label, true);

            if ((paramIDs[i].toUpper()=="POSITION")||(paramIDs[i].toUpper()=="WIDTH")) {
                record->resultsSetInNumberErrorList(resultID, param=paramIDs[i]+QString("_um"), stack_pos, m*deltaX/1000.0, v*deltaX/1000.0, "micron");
                record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, d.name, d.label, true);
            }


        }

        group=QString("fit_properties");

        record->resultsSetString(resultID, param=QString("fit_model"), model->id());
        record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, tr(""), tr(""), false);
        record->resultsSetString(resultID, param=QString("fit_algorithm"), algorithm->id());
        record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, tr(""), tr(""), false);
        record->resultsSetNumber(resultID, param=QString("deltax"), deltaX, QString("nm"));
        record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, tr(""), tr(""), false);
        record->resultsSetNumber(resultID, param=QString("deltaz"), deltaZ, QString("nm"));
        record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, tr(""), tr(""), false);

        if (orientation==QFSPIMLightsheetEvaluationItem::fitColumns)    {
            record->resultsSetString(resultID, param=QString("fit_orientation"), "fit_columns");
        } else {
            record->resultsSetString(resultID, param=QString("fit_orientation"), "fit_rows");
        }
        record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, tr(""), tr(""), false);
        qfFree(dataX);
        qfFree(dataY);
        qfFree(img);
    }
}







