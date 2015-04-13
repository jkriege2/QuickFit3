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

#include "qfusesresultsbyindexevaluation.h"

QFUsesResultsByIndexEvaluation::QFUsesResultsByIndexEvaluation(QFProject *parent, bool showRDRList, bool useSelection) :
    QFUsesResultsEvaluation(parent, showRDRList, useSelection)
{
    currentIndex=0;
}

QFUsesResultsByIndexEvaluation::~QFUsesResultsByIndexEvaluation()
{
}

void QFUsesResultsByIndexEvaluation::setPresetProperty(const QString &id, const QVariant &data, bool usereditable, bool visible)
{
    QString uid=id.toUpper();
    if (uid=="PRESET_CURRENT_INDEX" || uid=="PRESET_CURRENT_RUN" || uid=="PRESET_CURRENT_PIXEL") {
        setCurrentIndex(data.toInt());
    } else {
        QFUsesResultsEvaluation::setPresetProperty(id, data, usereditable, visible);
    }
}

bool QFUsesResultsByIndexEvaluation::hasResults(const QFRawDataRecord *r1, int index) const {
    const QFRawDataRecord* r=r1;
    QString rsid=getEvaluationResultID(index);
    return r->resultsExistsFromEvaluation(rsid);
}

QString QFUsesResultsByIndexEvaluation::getEvaluationResultID(int index) const {
    return QString("%1_%2_i%3").arg(getType()).arg(getID()).arg(index);
}



void QFUsesResultsByIndexEvaluation::setFitResultGroup(QFRawDataRecord *r, int index, const QString &parameterID, const QString &group) {
    setFitResultGroup(r, getEvaluationResultID(index), parameterID, group);
}

void QFUsesResultsByIndexEvaluation::setFitResultLabel(QFRawDataRecord *r, int index, const QString &parameterID, const QString &label, const QString &label_richtext)
{
    setFitResultLabel(r, getEvaluationResultID(index), parameterID, label, label_richtext);
}

void QFUsesResultsByIndexEvaluation::setFitResultEvaluationGroup(QFRawDataRecord *r, int index, const QString &group) {
    setFitResultEvaluationGroup(r, getEvaluationResultID(index), group);
}

void QFUsesResultsByIndexEvaluation::setFitResultEvaluationDescription(QFRawDataRecord *r, int index, const QString &description) {
    setFitResultEvaluationDescription(r, getEvaluationResultID(index), description);
}

void QFUsesResultsByIndexEvaluation::resetAllFitResults(QFRawDataRecord *r, int index) {
    resetAllFitResults(r, getEvaluationResultID(index));
}

void QFUsesResultsByIndexEvaluation::setFitValue(QFRawDataRecord *r, int index, const QString &parameterID, double value, const QString &unit) {
    setFitValue(r, getEvaluationResultID(index), parameterID, value, unit);
}

void QFUsesResultsByIndexEvaluation::setFitResultValue(QFRawDataRecord *r, int index, const QString &parameterID, double value, const QString &unit) {
    setFitResultValue(r, getEvaluationResultID(index), parameterID, value, unit);
}

void QFUsesResultsByIndexEvaluation::setFitResultValueString(QFRawDataRecord *r, int index, const QString &parameterID, const QString &value) {
    setFitResultValueString(r, getEvaluationResultID(index), parameterID, value);
}

void QFUsesResultsByIndexEvaluation::setFitResultValueInt(QFRawDataRecord *r, int index, const QString &parameterID, int64_t value, const QString &unit) {
    setFitResultValueInt(r, getEvaluationResultID(index), parameterID, value, unit);
}

void QFUsesResultsByIndexEvaluation::setFitResultValueBool(QFRawDataRecord *r, int index, const QString &parameterID, bool value) {
    setFitResultValueBool(r, getEvaluationResultID(index), parameterID, value);
}

void QFUsesResultsByIndexEvaluation::setFitResultValue(QFRawDataRecord *r, int index, const QString &parameterID, double value, double error, const QString &unit) {
    setFitResultValue(r, getEvaluationResultID(index), parameterID, value, error, unit);
}

void QFUsesResultsByIndexEvaluation::setFitResultError(QFRawDataRecord *r, int index, const QString &parameterID, double error) {
    setFitResultError(r, getEvaluationResultID(index), parameterID, error);
}

void QFUsesResultsByIndexEvaluation::setFitResultValueNumberArray(QFRawDataRecord *r, int index, const QString &parameterID, double *value, double *error, uint32_t N, const QString &unit) {
    setFitResultValueNumberArray(r, getEvaluationResultID(index), parameterID, value, error, N, unit);
}

void QFUsesResultsByIndexEvaluation::setFitResultValueNumberArray(QFRawDataRecord *r, int index, const QString &parameterID, double *value, uint32_t N, const QString &unit) {
    setFitResultValueNumberArray(r, getEvaluationResultID(index), parameterID, value, N, unit);
}

QVector<double> QFUsesResultsByIndexEvaluation::getFitValueNumberArray(const QFRawDataRecord *r, int index, const QString &parameterID) const {
    return getFitValueNumberArray(r, getEvaluationResultID(index), parameterID);
}

QVector<double> QFUsesResultsByIndexEvaluation::getFitValueErrorArray(const QFRawDataRecord *r, int index, const QString &parameterID) const {
    return getFitValueErrorArray(r, getEvaluationResultID(index), parameterID);
}

double QFUsesResultsByIndexEvaluation::getFitValue(const QFRawDataRecord *r, int index, const QString &parameterID) const {
    return getFitValue(r, getEvaluationResultID(index), parameterID);
}

double QFUsesResultsByIndexEvaluation::getFitError(const QFRawDataRecord *r, int index, const QString &parameterID) const {
    return getFitError(r, getEvaluationResultID(index), parameterID);
}

void QFUsesResultsByIndexEvaluation::setFitError(QFRawDataRecord *r, int index, const QString &parameterID, double error) {
    setFitError(r, getEvaluationResultID(index), parameterID, error);
}

void QFUsesResultsByIndexEvaluation::setFitFix(QFRawDataRecord *r, int index, const QString &parameterID, bool fix) {
    setFitFix(r, getEvaluationResultID(index), parameterID, fix);
}

void QFUsesResultsByIndexEvaluation::setFitResultFix(QFRawDataRecord *r, int index, const QString &parameterID, bool fix) {
    setFitResultFix(r, getEvaluationResultID(index), parameterID, fix);
}

bool QFUsesResultsByIndexEvaluation::getFitFix(const QFRawDataRecord *r, int index, const QString &parameterID) const {
    return getFitFix(r, getEvaluationResultID(index), parameterID);
}

void QFUsesResultsByIndexEvaluation::intWriteData(QXmlStreamWriter &w) const {
    QFUsesResultsEvaluation::intWriteData(w);
    w.writeTextElement("current_index", QString::number(currentIndex));
}

void QFUsesResultsByIndexEvaluation::intReadData(QDomElement *e) {
    QFUsesResultsEvaluation::intReadData(e);
    bool OK=false;
    currentIndex=e->firstChildElement("current_index").text().toInt(&OK);
    if (!OK) currentIndex=-1;
}

void QFUsesResultsByIndexEvaluation::setCurrentIndex(int index) {
    currentIndex=index;
}

int QFUsesResultsByIndexEvaluation::getCurrentIndex() const {
    if (currentIndex<getIndexMin(getHighlightedRecord())) return getIndexMin(getHighlightedRecord());
    if (currentIndex>=getIndexMax(getHighlightedRecord())) return getIndexMax(getHighlightedRecord());
    return currentIndex;
}

int QFUsesResultsByIndexEvaluation::getIndexMin(const QFRawDataRecord *r) const {
    return 0;
}

int QFUsesResultsByIndexEvaluation::getIndexMax(const QFRawDataRecord *r) const {
    return 0;
}

void QFUsesResultsByIndexEvaluation::setFitResultFitStatistics(QFRawDataRecord* record, int index, const QFFitStatistics& result, const QString &prefix, const QString &group)
{
    setFitResultFitStatistics(record, getEvaluationResultID(index), result, prefix, group);
}

void QFUsesResultsByIndexEvaluation::setFitResultSortPriority(QFRawDataRecord *r, int index, const QString &parameterID, bool sortPriority) {
    setFitResultSortPriority(r, getEvaluationResultID(index), parameterID, sortPriority);
}

