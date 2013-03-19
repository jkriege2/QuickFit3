#include "qfimfccsfitevaluation_item.h"
#include "qfimfccsfitevaluation_editor.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include "qffitfunction.h"
#include "qftools.h"
#include "qffcstools.h"


QFImFCCSFitEvaluationItem::QFImFCCSFitEvaluationItem(QFProject* parent):
    QFFitResultsByIndexAsVectorEvaluation("fcs_,dls_,fccs_", parent)
{
    m_weighting=EqualWeighting;
    m_currentIndex=-1;
    //mutexThreadedFit=new QMutex(QMutex::Recursive);

    if (m_fitFunctions.contains("fcs_spim_diff")) {
        m_fitFunction="fcs_spim_diff";
    } /*else if (m_fitFunctions.contains("fcs_diff1")) {
        m_fitFunction="fcs_diff1";
    }*/

    if (m_fitAlgorithms.contains("fit_lmfit")) {
        m_fitAlgorithm="fit_lmfit";
    }
}

QFImFCCSFitEvaluationItem::~QFImFCCSFitEvaluationItem() {

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




bool QFImFCCSFitEvaluationItem::isApplicable(QFRawDataRecord *record)
{
    return record->inherits("QFRDRFCSDataInterface") && record->inherits("QFRDRImageToRunInterface");
}

void QFImFCCSFitEvaluationItem::intWriteDataAlgorithm(QXmlStreamWriter &w) const
{
    w.writeAttribute("weighting", dataWeightToString(getFitDataWeighting()));
}

void QFImFCCSFitEvaluationItem::intReadDataAlgorithm(QDomElement &e)
{
    if (e.hasAttribute("weighting")) {
        QString a=e.attribute("weighting").toLower();
        setFitDataWeighting(stringToDataWeight(a));
    }
}

bool QFImFCCSFitEvaluationItem::hasSpecial(const QFRawDataRecord *r, const QString &id, const QString &paramid, double &value, double &error) const
{
    int run=getIndexFromEvaluationResultID(id);
    return qfFCSHasSpecial(r, run, paramid, value, error);
}

bool QFImFCCSFitEvaluationItem::hasSpecial(const QFRawDataRecord *r, int index, const QString &paramid, double &value, double &error) const
{
    return qfFCSHasSpecial(r, index, paramid, value, error);
}

bool QFImFCCSFitEvaluationItem::overrideFitFunctionPreset(QString paramName, double &value) const
{
    if (qfFCSOverrideFitFunctionPreset(this, paramName, value)) {
        return true;
    }
    return QFFitResultsByIndexAsVectorEvaluation::overrideFitFunctionPreset(paramName, value);
}

bool QFImFCCSFitEvaluationItem::overrideFitFunctionPresetError(QString paramName, double &value) const
{
    if (qfFCSOverrideFitFunctionPresetError(this, paramName, value)) {
        return true;
    }
    return QFFitResultsByIndexAsVectorEvaluation::overrideFitFunctionPresetError(paramName, value);
}

bool QFImFCCSFitEvaluationItem::overrideFitFunctionPresetFix(QString paramName, bool &value) const
{
    if (qfFCSOverrideFitFunctionPresetFix(this, paramName, value)) {
        return true;
    }
    return QFFitResultsByIndexAsVectorEvaluation::overrideFitFunctionPresetFix(paramName, value);
}
