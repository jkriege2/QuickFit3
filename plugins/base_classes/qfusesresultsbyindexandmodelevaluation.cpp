#include "qfusesresultsbyindexandmodelevaluation.h"

QFUsesResultsByIndexAndModelEvaluation::QFUsesResultsByIndexAndModelEvaluation(QFProject *parent, bool showRDRList, bool useSelection):
    QFUsesResultsByIndexEvaluation(parent, showRDRList, useSelection)
{
    currentModel=0;
}

bool QFUsesResultsByIndexAndModelEvaluation::hasResults(QFRawDataRecord *r1, int index, int model) const {
    QFRawDataRecord* r=r1;
    QString rsid=getEvaluationResultID(index, model);
    return r->resultsExistsFromEvaluation(rsid);

}

QString QFUsesResultsByIndexAndModelEvaluation::getEvaluationResultID(int currentIndex) const {
    return getEvaluationResultID(currentIndex, getCurrentModel());
}

QString QFUsesResultsByIndexAndModelEvaluation::getEvaluationResultID() const {
    return getEvaluationResultID(getCurrentIndex(), getCurrentModel());
}


QString QFUsesResultsByIndexAndModelEvaluation::getEvaluationResultID(int currentIndex, int model) const {
    return QString("%1_%2_m%3_run%4").arg(getType()).arg(getID()).arg(model).arg(currentIndex);
}

void QFUsesResultsByIndexAndModelEvaluation::intWriteData(QXmlStreamWriter &w) {
    QFUsesResultsByIndexEvaluation::intWriteData(w);
    w.writeTextElement("current_model", QString::number(currentModel));
}

void QFUsesResultsByIndexAndModelEvaluation::intReadData(QDomElement *e) {
    QFUsesResultsByIndexEvaluation::intReadData(e);
    bool OK=false;
    currentModel=e->firstChildElement("current_model").text().toInt(&OK);
    if (!OK) currentModel=0;
}

int QFUsesResultsByIndexAndModelEvaluation::getCurrentModel() const {
    if (currentModel<0) return 0;
    if (currentModel>=getModelCount()) return getModelCount()-1;
    return currentModel;
}

void QFUsesResultsByIndexAndModelEvaluation::setCurrentModel(int model) {
    currentModel=model;
}



int QFUsesResultsByIndexAndModelEvaluation::getModelCount(QFRawDataRecord *r, int /*index*/) const {
    return 0;

}

QString QFUsesResultsByIndexAndModelEvaluation::getModelName(int model) const {
    return tr("model %1").arg(model);
}

int QFUsesResultsByIndexAndModelEvaluation::getModelCount(QFRawDataRecord *r) const
{
    return getModelCount(r, getCurrentIndex());
}

int QFUsesResultsByIndexAndModelEvaluation::getModelCount() const
{
    return getModelCount(getHighlightedRecord(), getCurrentIndex());
}



void QFUsesResultsByIndexAndModelEvaluation::setFitResultGroup(QFRawDataRecord *r, int index, int model, const QString &parameterID, const QString &group) {
    setFitResultGroup(r, getEvaluationResultID(index, model), parameterID, group);
}

void QFUsesResultsByIndexAndModelEvaluation::setFitResultSortPriority(QFRawDataRecord *r, int index, int model, const QString &parameterID, bool sortPriority) {
    setFitResultSortPriority(r, getEvaluationResultID(index, model), parameterID, sortPriority);
}

void QFUsesResultsByIndexAndModelEvaluation::resetAllFitResults(QFRawDataRecord *r, int index, int model) {
    resetAllFitResults(r, getEvaluationResultID(index, model));
}

void QFUsesResultsByIndexAndModelEvaluation::setFitResultLabel(QFRawDataRecord *r, int index, int model, const QString &parameterID, const QString &label, const QString &label_richtext)
{
    setFitResultLabel(r, getEvaluationResultID(index, model), parameterID, label, label_richtext);
}

void QFUsesResultsByIndexAndModelEvaluation::setFitResultEvaluationGroup(QFRawDataRecord *r, int index, int model, const QString &group) {
    setFitResultEvaluationGroup(r, getEvaluationResultID(index, model), group);
}

void QFUsesResultsByIndexAndModelEvaluation::setFitResultEvaluationDescription(QFRawDataRecord *r, int index, int model, const QString &description) {
    setFitResultEvaluationDescription(r, getEvaluationResultID(index, model), description);
}

void QFUsesResultsByIndexAndModelEvaluation::setFitValue(QFRawDataRecord *r, int index, int model, const QString &parameterID, double value, const QString &unit) {
    setFitValue(r, getEvaluationResultID(index, model), parameterID, value, unit);
}

void QFUsesResultsByIndexAndModelEvaluation::setFitResultValue(QFRawDataRecord *r, int index, int model, const QString &parameterID, double value, const QString &unit) {
    setFitResultValue(r, getEvaluationResultID(index, model), parameterID, value, unit);
}

void QFUsesResultsByIndexAndModelEvaluation::setFitResultValueString(QFRawDataRecord *r, int index, int model, const QString &parameterID, const QString &value) {
    setFitResultValueString(r, getEvaluationResultID(index, model), parameterID, value);
}

void QFUsesResultsByIndexAndModelEvaluation::setFitResultValueInt(QFRawDataRecord *r, int index, int model, const QString &parameterID, int64_t value, const QString &unit) {
    setFitResultValueInt(r, getEvaluationResultID(index, model), parameterID, value, unit);
}

void QFUsesResultsByIndexAndModelEvaluation::setFitResultValueBool(QFRawDataRecord *r, int index, int model, const QString &parameterID, bool value) {
    setFitResultValueBool(r, getEvaluationResultID(index, model), parameterID, value);
}

void QFUsesResultsByIndexAndModelEvaluation::setFitResultValue(QFRawDataRecord *r, int index, int model, const QString &parameterID, double value, double error, const QString &unit) {
    setFitResultValue(r, getEvaluationResultID(index, model), parameterID, value, error, unit);
}

void QFUsesResultsByIndexAndModelEvaluation::setFitResultError(QFRawDataRecord *r, int index, int model, const QString &parameterID, double error) {
    setFitResultError(r, getEvaluationResultID(index, model), parameterID, error);
}

void QFUsesResultsByIndexAndModelEvaluation::setFitResultValueNumberArray(QFRawDataRecord *r, int index, int model, const QString &parameterID, double *value, double *error, uint32_t N, const QString &unit) {
    setFitResultValueNumberArray(r, getEvaluationResultID(index, model), parameterID, value, error, N, unit);
}

void QFUsesResultsByIndexAndModelEvaluation::setFitResultValueNumberArray(QFRawDataRecord *r, int index, int model, const QString &parameterID, double *value, uint32_t N, const QString &unit) {
    setFitResultValueNumberArray(r, getEvaluationResultID(index, model), parameterID, value, N, unit);
}

QVector<double> QFUsesResultsByIndexAndModelEvaluation::getFitValueNumberArray(QFRawDataRecord *r, int index, int model, const QString &parameterID) const {
    return getFitValueNumberArray(r, getEvaluationResultID(index, model), parameterID);
}

QVector<double> QFUsesResultsByIndexAndModelEvaluation::getFitValueErrorArray(QFRawDataRecord *r, int index, int model, const QString &parameterID) const {
    return getFitValueErrorArray(r, getEvaluationResultID(index, model), parameterID);
}

double QFUsesResultsByIndexAndModelEvaluation::getFitValue(QFRawDataRecord *r, int index, int model, const QString &parameterID) const {
    return getFitValue(r, getEvaluationResultID(index, model), parameterID);
}

bool QFUsesResultsByIndexAndModelEvaluation::fitValueExists(QFRawDataRecord *r, int index, int model, const QString &parameterID) const
{
    return fitValueExists(r, getEvaluationResultID(index, model), parameterID);
}

double QFUsesResultsByIndexAndModelEvaluation::getFitError(QFRawDataRecord *r, int index, int model, const QString &parameterID) const {
    return getFitError(r, getEvaluationResultID(index, model), parameterID);
}

void QFUsesResultsByIndexAndModelEvaluation::setFitError(QFRawDataRecord *r, int index, int model, const QString &parameterID, double error) {
    setFitError(r, getEvaluationResultID(index, model), parameterID, error);
}

void QFUsesResultsByIndexAndModelEvaluation::setFitFix(QFRawDataRecord *r, int index, int model, const QString &parameterID, bool fix) {
    setFitFix(r, getEvaluationResultID(index, model), parameterID, fix);
}

void QFUsesResultsByIndexAndModelEvaluation::setFitResultFix(QFRawDataRecord *r, int index, int model, const QString &parameterID, bool fix) {
    setFitResultFix(r, getEvaluationResultID(index, model), parameterID, fix);
}

bool QFUsesResultsByIndexAndModelEvaluation::getFitFix(QFRawDataRecord *r, int index, int model, const QString &parameterID) const {
    return getFitFix(r, getEvaluationResultID(index, model), parameterID);
}
