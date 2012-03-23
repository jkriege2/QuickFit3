#include "qfusesresultsevaluation.h"
#include <cfloat>

QFUsesResultsEvaluation::QFUsesResultsEvaluation(QFProject *parent, bool showRDRList, bool useSelection) :
    QFEvaluationItem(parent, showRDRList, useSelection)
{
    // open ini-file to superseede fit parameter defaults from QFFitFunction
    QString inifn=parent->getServices()->getOptions()->getConfigFileDirectory()+QString("/fitparams_%1.ini").arg(getType());
    fitParamSettings=new QSettings(inifn, QSettings::IniFormat);
    QString inifng=parent->getServices()->getOptions()->getGlobalConfigFileDirectory()+QString("/fitparams_%1.ini").arg(getType());
    fitParamGlobalSettings=new QSettings(inifng, QSettings::IniFormat);

    // clear internal parameter store
    parameterStore.clear();
}

QFUsesResultsEvaluation::~QFUsesResultsEvaluation()
{
}

bool QFUsesResultsEvaluation::hasResults(QFRawDataRecord *r1) const {
    return hasResults(r1, getEvaluationResultID());
}

bool QFUsesResultsEvaluation::hasResults(QFRawDataRecord *r1, QString resultID) const {
    if (!r1) return false;
    return r1->resultsExistsFromEvaluation(resultID);
}

bool QFUsesResultsEvaluation::hasResults() const {
    return hasResults(getHighlightedRecord());
}

QString QFUsesResultsEvaluation::getEvaluationResultID() const {
    return QString("%1_%2").arg(getType()).arg(getID());
}

void QFUsesResultsEvaluation::setFitResultSortPriority(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, bool sortPriority) {
    if (r!=NULL) {
        r->resultsSetSortPriority(transformResultID(resultID), getFitParamID(parameterID), sortPriority);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultGroup(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, const QString &group) {
    if (r!=NULL) {
        r->resultsSetGroup(transformResultID(resultID), getFitParamID(parameterID), group);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultLabel(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, const QString &label, const QString &label_richtext) {
    if (r!=NULL) {
        r->resultsSetLabel(transformResultID(resultID), getFitParamID(parameterID), label, label_richtext);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultEvaluationGroup(QFRawDataRecord *r, const QString &resultID, const QString &group) {
    if (r!=NULL) {
        r->resultsSetEvaluationGroup(transformResultID(resultID), group);
        emitResultsChanged(r, resultID);
    }
}

void QFUsesResultsEvaluation::setFitResultEvaluationDescription(QFRawDataRecord *r, const QString &resultID, const QString &description) {
    if (r!=NULL) {
        r->resultsSetEvaluationDescription(transformResultID(resultID), description);
        emitResultsChanged(r, resultID);
    }
}

void QFUsesResultsEvaluation::setFitValue(const QString &parameterID, double value, const QString &unit) {
    setFitValue(getHighlightedRecord(), getEvaluationResultID(), parameterID, value, unit);
}

void QFUsesResultsEvaluation::setFitValue(const QString &parameterID, double value)
{
    setFitValue(getHighlightedRecord(), getEvaluationResultID(), parameterID, value);
}


double QFUsesResultsEvaluation::getFitValue(const QString &parameterID) const
{
    return getFitValue(getHighlightedRecord(), getEvaluationResultID(), parameterID);
}

double QFUsesResultsEvaluation::getFitError(const QString &parameterID) const
{
    return getFitError(getHighlightedRecord(), getEvaluationResultID(), parameterID);
}

void QFUsesResultsEvaluation::setFitError(const QString &parameterID, double error)
{
    setFitError(getHighlightedRecord(), getEvaluationResultID(), parameterID, error);
}

void QFUsesResultsEvaluation::setFitFix(const QString &parameterID, bool fix)
{
    setFitFix(getHighlightedRecord(), getEvaluationResultID(), parameterID, fix);
}

bool QFUsesResultsEvaluation::getFitFix(const QString &parameterID) const
{
    return getFitFix(getHighlightedRecord(), getEvaluationResultID(), parameterID);
}

void QFUsesResultsEvaluation::setFitValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double value, const QString& unit) {
    if (r!=NULL) {
        QString dsid=getParameterStoreID(parameterID);
        if (hasResults(r, resultID)) {
            setFitResultValue(r, resultID, parameterID, value, getFitError(r, resultID, parameterID), unit);
        } else {
            parameterStore[getParameterStoreID(parameterID)].value=value;
            parameterStore[getParameterStoreID(parameterID)].valueSet=true;
            emitPropertiesChanged();
        }

    }
}

void QFUsesResultsEvaluation::setFitValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double value) {
    if (r!=NULL) {
        QString dsid=getParameterStoreID(parameterID);
        if (hasResults(r, resultID)) {
            setFitResultValue(r, resultID, parameterID, value, getFitError(r, resultID, parameterID));
        } else {
            parameterStore[getParameterStoreID(parameterID)].value=value;
            parameterStore[getParameterStoreID(parameterID)].valueSet=true;
            emitPropertiesChanged();
        }

    }
}

void QFUsesResultsEvaluation::setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double value, const QString& unit) {
    if (r!=NULL) {
        //qDebug()<<transformResultID(resultID)<<": "<<getFitParamID(parameterID)<<" = "<<value<<unit;
        r->resultsSetNumberError(transformResultID(resultID), getFitParamID(parameterID), value, getFitError(r, resultID, parameterID), unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultValueString(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, const QString& value) {
    if (r!=NULL) {
        r->resultsSetString(transformResultID(resultID), getFitParamID(parameterID), value);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultValueInt(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, int64_t value, const QString& unit) {
    if (r!=NULL) {
        r->resultsSetInteger(transformResultID(resultID), getFitParamID(parameterID), value, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultValueBool(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, bool value) {
    if (r!=NULL) {
        r->resultsSetBoolean(transformResultID(resultID), getFitParamID(parameterID), value);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double value, double error, const QString& unit) {
    if (r!=NULL) {
        r->resultsSetNumberError(transformResultID(resultID), getFitParamID(parameterID), value, error, unit);
        //qDebug()<<transformResultID(resultID)<<": "<<getFitParamID(parameterID)<<" = ("<<value<<"+/-"<<error<<")"<<unit;
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double value, double error) {
    if (r!=NULL) {
        //qDebug()<<transformResultID(resultID)<<": "<<getFitParamID(parameterID)<<" = ("<<value<<"+/-"<<error<<")"<<r->resultsGet(transformResultID(resultID), getFitParamID(parameterID)).unit;
        r->resultsSetNumberError(transformResultID(resultID), getFitParamID(parameterID), value, error, r->resultsGet(transformResultID(resultID), getFitParamID(parameterID)).unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultError(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double error) {
    if (r!=NULL) {
        r->resultsSetNumberErrorError(transformResultID(resultID), getFitParamID(parameterID), error);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultValueNumberArray(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double *value, double *error, uint32_t N, const QString &unit) {
    if (r!=NULL) {
        r->resultsSetNumberErrorList(transformResultID(resultID), getFitParamID(parameterID), value, error, N, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultValueNumberArray(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double *value, uint32_t N, const QString &unit) {
    if (r!=NULL) {
        r->resultsSetNumberList(transformResultID(resultID), getFitParamID(parameterID), value, N, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

QVector<double> QFUsesResultsEvaluation::getFitValueNumberArray(QFRawDataRecord *r, const QString &resultID, const QString &parameterID) const {
    QVector<double> res;

    if (r) {
        QFUsesResultsEvaluation::FitParameterDefault defVal;
        if (getParameterDefault(r, resultID, parameterID, defVal)) {
            res=defVal.valueVector;
        }

        if (hasResults(r, resultID)) {
            QString tresultID=transformResultID(resultID);
            QString pid=getFitParamID(parameterID);
            if (r->resultsExists(tresultID, pid)) res=r->resultsGetAsDoubleList(tresultID, pid);
        }
    }
    return res;
}


QVector<double> QFUsesResultsEvaluation::getFitValueErrorArray(QFRawDataRecord *r, const QString &resultID, const QString &parameterID) const {
    QVector<double> res;

    if (r) {
        QFUsesResultsEvaluation::FitParameterDefault defVal;
        if (getParameterDefault(r, resultID, parameterID, defVal)) {
            res=defVal.errorVector;
        }

        if (hasResults(r, resultID)) {
            QString tresultID=transformResultID(resultID);
            QString pid=getFitParamID(parameterID);
            if (r->resultsExists(tresultID, pid)) res=r->resultsGetErrorAsDoubleList(tresultID, pid);
        }
    }
    return res;
}

double QFUsesResultsEvaluation::getFitValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID) const {
    //qDebug()<<"getFitValue("<<resultID<<", "<<parameterID<<")";
    double res=0;
    res=fitParamGlobalSettings->value(getParameterStoreIDInQSettings(parameterID), res).toDouble();
    res=fitParamSettings->value(getParameterStoreIDInQSettings(parameterID), res).toDouble();
    QFUsesResultsEvaluation::FitParameterDefault defVal;
    if (getParameterDefault(r, resultID, parameterID, defVal)) {
        res=defVal.value;
    }    QString psID=getParameterStoreID(parameterID);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].valueSet) {
            res=parameterStore[psID].value;
        }
    }

    double sval=res, serr=0;
    if (hasSpecial(r, resultID, parameterID, sval, serr)) {
        //qDebug()<<"  - hasSpecial "<<sval;
        res=sval;
    } else if (r && hasResults(r, resultID)) {
        QString tresultID=transformResultID(resultID);
        QString pid=getFitParamID(parameterID);
        //qDebug()<<"  - hasResults "<<tresultID<<pid;
        //qDebug()<<"               "<<r->resultsCalcNames(tresultID);
        if (r->resultsExists(tresultID, pid)) {
            res=r->resultsGetAsDouble(tresultID, pid);
            //qDebug()<<"    - resultsExists "<<res;
        }
    }
    //qDebug()<<"  getFitValue("<<resultID<<", "<<parameterID<<") = "<<res;
    return res;
}


double QFUsesResultsEvaluation::getFitError(QFRawDataRecord *r, const QString &resultID, const QString &parameterID) const {
    double sval=0, serr=0;
    if (hasSpecial(r, resultID, parameterID, sval, serr)) {
       return serr;
    }
    if (hasResults(r, resultID)) {
        if (r!=NULL) {
            return r->resultsGetErrorAsDouble(transformResultID(resultID), getFitParamID(parameterID));
        }
    }

    QString psID=getParameterStoreID(parameterID);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].errorSet) {
            return parameterStore[psID].error;
        }
    }

    QFUsesResultsEvaluation::FitParameterDefault defVal;
    if (getParameterDefault(r, resultID, parameterID, defVal)) {
        return defVal.error;
    }
    return 0.0;
}

void QFUsesResultsEvaluation::setFitError(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double error) {
    if (r!=NULL) {
        QString dsid=getParameterStoreID(parameterID);
        if (hasResults(r, resultID)) {
            setFitResultError(r, resultID, parameterID, error);
        } else {
            parameterStore[dsid].error=error;
            parameterStore[dsid].errorSet=true;
            emitPropertiesChanged();
        }
    }
}

void QFUsesResultsEvaluation::setFitFix(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, bool fix) {
    if (r!=NULL) {
        QString dsid=getParameterStoreID(parameterID);
        if (hasResults(r, resultID)) {
            setFitResultFix(r, resultID, parameterID, fix);
        } else {
            parameterStore[dsid].fix=fix;
            parameterStore[dsid].fixSet=true;
            emitPropertiesChanged();
        }

    }
}

void QFUsesResultsEvaluation::setFitResultFix(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, bool fix) {
    if (r!=NULL) {
        QString unit="";
        r->resultsSetBoolean(transformResultID(resultID), getFitParamFixID(parameterID), fix);
        emitResultsChanged(r, resultID, getFitParamFixID(parameterID));
    }
}

bool QFUsesResultsEvaluation::getFitFix(QFRawDataRecord *r, const QString &resultID, const QString &parameterID) const {
    bool res=false;
    res=fitParamGlobalSettings->value(getParameterStoreIDInQSettings(parameterID)+"_fix", res).toBool();
    res=fitParamSettings->value(getParameterStoreIDInQSettings(parameterID)+"_fix", res).toBool();
    QFUsesResultsEvaluation::FitParameterDefault defVal;
    if (getParameterDefault(r, resultID, parameterID, defVal)) {
        res=defVal.fix;
    }
    QString psID=getParameterStoreID(parameterID);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].fixSet) {
            res=parameterStore[psID].fix;
        }
    }

    if (r && hasResults(r, resultID)) {
        QString en=transformResultID(resultID);
        QString pid=getFitParamFixID(parameterID);
        if (r->resultsExists(en, pid)) res=r->resultsGetAsDouble(en, pid);
    }
    return res;
}

QString QFUsesResultsEvaluation::getParameterName(int model, int id, bool html) const
{
    return QString("");
}

QString QFUsesResultsEvaluation::getParameterUnit(int model, int id, bool html) const
{
    return QString("");
}

int QFUsesResultsEvaluation::getParameterCount(int model) const
{
    return 0;
}

QString QFUsesResultsEvaluation::getParameterID(int model, int param) const
{
    return QString("");
}

QString QFUsesResultsEvaluation::getParameterStoreID(QString parameter) const {
    return parameter;
}

QString QFUsesResultsEvaluation::getParameterStoreIDInQSettings(QString parameter) const {
    return parameter;
}


void QFUsesResultsEvaluation::intWriteData(QXmlStreamWriter& w) {
    w.writeStartElement("default_parameters");
    QHashIterator<QString, FitParameter> i(parameterStore);
    while (i.hasNext()) {
        i.next();
        w.writeStartElement("parameter");
        w.writeAttribute("id", i.key());
        if (i.value().valueSet) w.writeAttribute("value", CDoubleToQString(i.value().value));
        if (i.value().errorSet) w.writeAttribute("error", CDoubleToQString(i.value().error));
        if (i.value().minSet) w.writeAttribute("min", CDoubleToQString(i.value().min));
        if (i.value().maxSet) w.writeAttribute("max", CDoubleToQString(i.value().max));
        if (i.value().fixSet) w.writeAttribute("fix", QString((i.value().fix)?QString("1"):QString("0")));
        w.writeEndElement();
    }
    w.writeEndElement();
}

QString QFUsesResultsEvaluation::transformResultID(const QString &resultID) const {
    return resultID;
}

QString QFUsesResultsEvaluation::getFitParamID(QString fitparam) const {
    return QString("%1").arg(fitparam);
}

QString QFUsesResultsEvaluation::getFitParamFixID(QString fitparam) const {
    return QString("%1_fix").arg(fitparam);
}

bool QFUsesResultsEvaluation::hasSpecial(QFRawDataRecord *r, const QString &resultID, const QString &paramid, double &value, double &error) const {
    return false;
}

bool QFUsesResultsEvaluation::getParameterDefault(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, QFUsesResultsEvaluation::FitParameterDefault &defaultValue) const {
    return false;
}

void QFUsesResultsEvaluation::intReadData(QDomElement* e) {

    QDomElement e1=e->firstChildElement("default_parameters");

    // read <parameter> tag
    QDomElement elt=e1.firstChildElement("parameter");
    parameterStore.clear();
    for (; !elt.isNull(); elt = elt.nextSiblingElement("parameter")) {
        if (elt.hasAttribute("id") && ( elt.hasAttribute("value") || elt.hasAttribute("fix") || elt.hasAttribute("min") || elt.hasAttribute("max") ) ) {
            QString a=elt.attribute("id");
            FitParameter p;
            if (elt.hasAttribute("fix")) {
                p.fix=(elt.attribute("fix").toLower()!="0");
                p.fixSet=true;
            }
            if (elt.hasAttribute("value")) {
                p.value=CQStringToDouble(elt.attribute("value"));
                p.valueSet=true;
            }
            if (elt.hasAttribute("error")) {
                p.error=CQStringToDouble(elt.attribute("error"));
                p.errorSet=true;
            }
            if (elt.hasAttribute("min")) {
                p.min=CQStringToDouble(elt.attribute("min"));
                p.minSet=true;
            }
            if (elt.hasAttribute("max")) {
                p.max=CQStringToDouble(elt.attribute("max"));
                p.maxSet=true;
            }
            parameterStore[a]=p;
        }
    }
}


QFUsesResultsEvaluation::FitParameterDefault::FitParameterDefault() {
    value=0;
    error=0;
    min=-DBL_MAX;
    max=DBL_MAX;
    fix=false;
}
