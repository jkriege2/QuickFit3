#include "qffcsfitevaluation.h"
#include "qffcsfitevaluationeditor.h"
#include "../interfaces/qfrdrfcsdatainterface.h"



QFFCSFitEvaluation::QFFCSFitEvaluation(QFProject* parent):
    QFFitResultsByIndexEvaluation("fcs_", parent)
    //QFEvaluationItem(parent, true, false)
{

    m_weighting=EqualWeighting;
    m_currentIndex=-1;

    if (m_fitFunctions.contains("fcs_diff")) {
        m_fitFunction="fcs_diff";
    } else if (m_fitFunctions.contains("fcs_diff1")) {
        m_fitFunction="fcs_diff1";
    } else if (m_fitFunctions.contains("fcs_multidiff")) {
        m_fitFunction="fcs_multidiff";
    }

    if (m_fitAlgorithms.contains("fit_levmar")) {
        m_fitAlgorithm="fit_levmar";
    }

}

QFFCSFitEvaluation::~QFFCSFitEvaluation() {
}


void QFFCSFitEvaluation::intWriteDataAlgorithm(QXmlStreamWriter& w) const {
    if (m_weighting==EqualWeighting) w.writeAttribute("weighting", "equal");
    if (m_weighting==StdDevWeighting) w.writeAttribute("weighting", "stddev");
    if (m_weighting==RunErrorWeighting) w.writeAttribute("weighting", "runerror");
}

void QFFCSFitEvaluation::intReadDataAlgorithm(QDomElement& e) {
    if (e.hasAttribute("weighting")) {
        QString a=e.attribute("weighting").toLower();
        m_weighting=EqualWeighting;
        if (a=="stddev") m_weighting=StdDevWeighting;
        if (a=="runerror") m_weighting=RunErrorWeighting;
    }
}

QFEvaluationEditor* QFFCSFitEvaluation::createEditor(QFPluginServices* services, QWidget* parent) {
    return new QFFCSFitEvaluationEditor(services, parent);
};

bool QFFCSFitEvaluation::isApplicable(QFRawDataRecord* record) {
    return record->inherits("QFRDRFCSDataInterface");
}

bool QFFCSFitEvaluation::hasSpecial(QFRawDataRecord* r, const QString& id, const QString& paramid, double& value, double& error) {
    int run=getIndexFromEvaluationResultID(id);
    QFRDRCountRatesInterface* crintf=qobject_cast<QFRDRCountRatesInterface*>(r);
    if (crintf && (paramid=="count_rate")) {
        error=crintf->getRateStdDev(run)*1000;
        value=crintf->getRateMean(run)*1000;
        return true;
    }
    return false;
}

int QFFCSFitEvaluation::getIndexFromEvaluationResultID(const QString& resultID) {
    if (resultID.size()<=0) return -1;
    if (resultID.endsWith("avg")) return -1;
    int l=0;
    while (resultID[resultID.size()-1-l].isDigit()) {
        l++;
    }
    if (l>0) return resultID.right(l).toInt();
    return -1;
}

int QFFCSFitEvaluation::getIndexMin(QFRawDataRecord* r) {
    return -1;
}

int QFFCSFitEvaluation::getIndexMax(QFRawDataRecord* r) {
    if (!r) return -1;
    QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(r);
    if (fcs->getCorrelationRuns()<=0) return -1;
    else return fcs->getCorrelationRuns()-1;
}


QString QFFCSFitEvaluation::getEvaluationResultID(QString fitFunction, int currentRun) {
    if (currentRun<0) return QString("%1_%2_%3_runavg").arg(getType()).arg(getID()).arg(fitFunction);
    return QString("%1_%2_%3_run%4").arg(getType()).arg(getID()).arg(fitFunction).arg(currentRun);
}
