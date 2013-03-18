#include "qfimfccsfitevaluation_item.h"
#include "qfimfccsfitevaluation_editor.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include "qffitfunction.h"
#include "qftools.h"


QFImFCCSFitEvaluationItem::QFImFCCSFitEvaluationItem(QFProject* parent):
    QFFitResultsByIndexAsVectorEvaluation("fcs_,dls_,fccs_", parent)
{

}

QFImFCCSFitEvaluationItem::~QFImFCCSFitEvaluationItem() {

}


void QFImFCCSFitEvaluationItem::intWriteData(QXmlStreamWriter& w) {
    /* store global evaluation parameters / initial values or anything else to the project tree here */
}

void QFImFCCSFitEvaluationItem::intReadData(QDomElement* e) {
    /* read back anything you've written in intWriteData() in this function */
}


QFEvaluationEditor* QFImFCCSFitEvaluationItem::createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent) {
    return new QFImFCCSFitEvaluationEditor(services, propEditor, parent);
}

int QFImFCCSFitEvaluationItem::getIndexMin(QFRawDataRecord *r) const
{
    return -1;
}

int QFImFCCSFitEvaluationItem::getIndexMax(QFRawDataRecord *r) const
{
    if (!r) return -1;
    QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(r);
    if (fcs->getCorrelationRuns()<=0) return -1;
    else return fcs->getCorrelationRuns()-1;
}

void QFImFCCSFitEvaluationItem::doFit(QFRawDataRecord *record, int run, int defaultMinDatarange, int defaultMaxDatarange, QFFitAlgorithmReporter *dlgFitProgress, bool doLog)
{
}

void QFImFCCSFitEvaluationItem::doFitForMultithread(QFRawDataRecord *record, int run, int defaultMinDatarange, int defaultMaxDatarange, QFPluginLogService *logservice) const
{
}

QFFitStatistics QFImFCCSFitEvaluationItem::calcFitStatistics(bool storeAsResults, QFFitFunction *ffunc, long N, double *tauvals, double *corrdata, double *weights, int datacut_min, int datacut_max, double *fullParams, double *errors, bool *paramsFix, int runAvgWidth, int residualHistogramBins, QFRawDataRecord *record, int run)
{
    return QFFitResultsByIndexEvaluationFitTools::calcFitStatistics(storeAsResults, ffunc,  N, tauvals, corrdata, weights,  datacut_min,  datacut_max, fullParams, errors, paramsFix,  runAvgWidth,  residualHistogramBins, record,  run);
}

bool QFImFCCSFitEvaluationItem::isApplicable(QFRawDataRecord* record) {
    /* CHECK WHETHER A GIVEN RECORD MAY BE USED TOGETHER WITH THE EVALUATION */
    return true;
}


