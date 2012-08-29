#include "qfspimlightsheetevaluation_item.h"
#include "qfspimlightsheetevaluation_editor.h"
#include "qfrdrimagestackinterface.h"
#include "qftools.h"
#include "statistics_tools.h"
#include "lmcurve.h"
#include <typeinfo>

#include <QtGui>
#include <QtCore>





QFSPIMLightsheetEvaluationItem::QFSPIMLightsheetEvaluationItem(QFProject* parent):
    QFEvaluationItem(parent, true, false)
{

}

QFSPIMLightsheetEvaluationItem::~QFSPIMLightsheetEvaluationItem() {

}


void QFSPIMLightsheetEvaluationItem::intWriteData(QXmlStreamWriter& w) {
    /* store global evaluation parameters / initial values or anything else to the project tree here */
}

void QFSPIMLightsheetEvaluationItem::intReadData(QDomElement* e) {
    /* read back anything you've written in intWriteData() in this function */
}


QFEvaluationEditor* QFSPIMLightsheetEvaluationItem::createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent) {
    return new QFSPIMLightsheetEvaluationEditor(services, propEditor, parent);
}

bool QFSPIMLightsheetEvaluationItem::isApplicable(QFRawDataRecord* record) {
    /* CHECK WHETHER A GIVEN RECORD MAY BE USED TOGETHER WITH THE EVALUATION */
    return qobject_cast<QFRDRImageStackInterface*>(record);
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















double fGauss( double t, const double *p )
{
    const double offset=p[0];
    const double A=p[1];
    const double avg=p[2];
    const double var=p[3];
    return offset+A*exp(-2.0*(t-avg)*(t-avg)/var);
}

void QFSPIMLightsheetEvaluationItem::doEvaluation(QFRawDataRecord *record, int stack, int stack_pos, int channel, double deltaX, double deltaY, Orientation orientation, Models model) const {
    QString resultID=getEvaluationResultID(stack, channel);
    QFRDRImageStackInterface* data=qobject_cast<QFRDRImageStackInterface*>(record);
    if (data) {
        int w=data->getImageStackWidth(stack);
        int h=data->getImageStackHeight(stack);
        double* img=(double*)malloc(w*h*sizeof(double));
        int wi=w;
        int hi=h;

        if (orientation==QFSPIMLightsheetEvaluationItem::fitColumns) {
            // rotate image by 90°
            double* m=data->getImageStack(stack, stack_pos, channel);
            for (int x=0; x<w; x++) {
                for (int y=0; y<h; y++) {
                    img[x*h+w]=m[y*w+x];
                }
            }
            wi=h;
            hi=w;
        } else {
            double* m=data->getImageStack(stack, stack_pos, channel);
            for (int x=0; x<w*h; x++) {
                img[x]=m[x];
            }
        }

        QVector<double> offsets, amplitudes, avgs, widths;
        double* dataX=(double*)malloc(wi*sizeof(double));
        double* dataY=(double*)malloc(wi*sizeof(double));
        for (int i=0; i<wi; i++) dataX[i]=i;
        for (int f=0; f<hi; f++) {
            for (int i=0; i<wi; i++) {
                dataY[i]=img[f*wi+i];
            }

            lm_control_struct control=lm_control_double;
            //control.maxcall=500;
            lm_status_struct status;

            double par[4];
            long long maxpos=0;
            par[0]=statisticsMin(dataY, wi);
            par[1]=fabs(statisticsMax(dataY, wi, &maxpos)-par[0]);
            par[2]=maxpos;
            par[3]=double(wi)/10.0;
            qDebug()<<f<<": offset="<<par[0]<<" A="<<par[1]<<" pos="<<par[2]<<" width="<<par[3];
            lmcurve_fit(4, par, wi, dataX, dataY, fGauss, &control, &status);
            offsets.append(par[0]);
            amplitudes.append(par[1]);
            avgs.append(par[2]);
            widths.append(sqrt(par[3]));
        }

        QString param;
        QString group=QString("fit_results");
        qDebug()<<"writing data to record ...";

        record->resultsSetNumberList(resultID, param=QString("offset_frame%1").arg(stack_pos), offsets);
        qDebug()<<1;
        record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, tr(""), tr(""), true);
        qDebug()<<2;
        record->resultsSetNumberList(resultID, param=QString("amplitude_frame%1").arg(stack_pos), amplitudes);
        qDebug()<<3;
        record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, tr(""), tr(""), true);
        qDebug()<<4;
        record->resultsSetNumberList(resultID, param=QString("averagepos_frame%1").arg(stack_pos), avgs, tr("pix"));
        qDebug()<<5;
        record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, tr(""), tr(""), true);
        qDebug()<<6;
        record->resultsSetNumberList(resultID, param=QString("width_frame%1").arg(stack_pos), widths, tr("pix"));
        qDebug()<<7;
        record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, tr(""), tr(""), true);
        qDebug()<<8;

        double v=0;
        double m=qfstatisticsAverageVariance(v, offsets);
        qDebug()<<9;
        record->resultsSetInNumberErrorList(resultID, param="offset", stack_pos, m, v, "");
        qDebug()<<10;
        record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, tr(""), tr(""), true);
        qDebug()<<11;
        v=0;
        m=qfstatisticsAverageVariance(v, amplitudes);
        record->resultsSetInNumberErrorList(resultID, param="amplitude", stack_pos, m, v, "");
        record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, tr(""), tr(""), true);
        qDebug()<<12;
        v=0;
        m=qfstatisticsAverageVariance(v, avgs);
        record->resultsSetInNumberErrorList(resultID, param="averagepos", stack_pos, m, v, "pix");
        record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, tr(""), tr(""), true);
        qDebug()<<13;
        v=0;
        m=qfstatisticsAverageVariance(v, widths);
        record->resultsSetInNumberErrorList(resultID, param="width", stack_pos, m, v, "pix");
        record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, tr(""), tr(""), true);
        qDebug()<<14;

        for (int i=0; i<avgs.size(); i++) {
            avgs[i]=avgs[i]*deltaX/1000.0;
        }
        for (int i=0; i<widths.size(); i++) {
            widths[i]=widths[i]*deltaX/1000.0;
        }
        qDebug()<<15;
        v=0;
        m=qfstatisticsAverageVariance(v, avgs);
        record->resultsSetInNumberErrorList(resultID, param="averagepos_um", stack_pos, m, v, "micron");
        record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, tr(""), tr(""), true);
        qDebug()<<16;
        v=0;
        m=qfstatisticsAverageVariance(v, widths);
        record->resultsSetInNumberErrorList(resultID, param="width_um", stack_pos, m, v, "micron");
        record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, tr(""), tr(""), true);
        qDebug()<<17;



        group=QString("fit_properties");

        record->resultsSetString(resultID, param=QString("fit_model"), "gaussian");
        record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, tr(""), tr(""), false);
        qDebug()<<18;

        if (orientation==QFSPIMLightsheetEvaluationItem::fitColumns)    {
            record->resultsSetString(resultID, param=QString("fit_orientation"), "fit_columns");
        } else {
            record->resultsSetString(resultID, param=QString("fit_orientation"), "fit_rows");
        }
        record->resultsSetGroupLabelsAndSortPriority(resultID, param, group, tr(""), tr(""), false);
        qDebug()<<19;
        free(dataX);
        qDebug()<<20;
        free(dataY);
        qDebug()<<21;
        free(img);
        qDebug()<<22;
        qDebug()<<"data written to record!";
    }
}







