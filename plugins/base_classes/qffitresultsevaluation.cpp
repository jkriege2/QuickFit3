#include "qffitresultsevaluation.h"







QFFitResultsEvaluation::QFFitResultsEvaluation(const QString& fitFunctionPrefix, QFProject* parent) :
    QFEvaluationItem(parent, true, false)
{
    // open ini-file to superseede fit parameter defaults from QFFitFunction
    QString inifn=parent->getServices()->getOptions()->getConfigFileDirectory()+QString("/fitparams_%1.ini").arg(getType());
    fitParamSettings=new QSettings(inifn, QSettings::IniFormat);

    m_fitAlgorithm="";
    m_fitFunction="";


    // get list of applicable fit functions
    m_fitFunctions=parent->getServices()->getFitFunctionManager()->getModels(fitFunctionPrefix, this);
    // select first fit function
    if (m_fitFunctions.size()>0) m_fitFunction=m_fitFunctions.keys().at(0);

    // get list of available fit algorithms and store their parameters in the internal algorithm_parameterstore
    algorithm_parameterstore.clear();
    QStringList fita=parent->getServices()->getFitAlgorithmManager()->getIDList();
    for (int i=0; i<fita.size(); i++) {
        QFFitAlgorithm* falg=parent->getServices()->getFitAlgorithmManager()->createAlgorithm(fita[i], this);
        m_fitAlgorithms[fita[i]]=falg;
        storeQFFitAlgorithmParameters(falg);
    }

    // select first fit algorithm (set as current)
    if (m_fitAlgorithms.size()>0) m_fitAlgorithm=m_fitAlgorithms.keys().at(0);

    // clear internal parameter store
    parameterStore.clear();

}

QFFitResultsEvaluation::~QFFitResultsEvaluation() {
    // the fit algorithm and fit function instances are private to this class, so we have to delete the objects!
    QMapIterator<QString, QFFitFunction*> i(m_fitFunctions);
    while (i.hasNext()) {
        i.next();
        delete i.value();
    }
    m_fitFunctions.clear();
    QMapIterator<QString, QFFitAlgorithm*> j(m_fitAlgorithms);
    while (j.hasNext()) {
        j.next();
        delete j.value();
    }
    m_fitAlgorithms.clear();
}



QStringList QFFitResultsEvaluation::getAvailableFitAlgorithms() const {
    return m_fitAlgorithms.keys();
}

QFFitAlgorithm* QFFitResultsEvaluation::getFitAlgorithm(const QString& id) const {
    if (m_fitAlgorithms.contains(id)) return m_fitAlgorithms[id];
    return NULL;
}

QStringList QFFitResultsEvaluation::getAvailableFitFunctions() const {
    return m_fitFunctions.keys();
}

QFFitFunction* QFFitResultsEvaluation::getFitFunction(const QString& id) const {
    if (m_fitFunctions.contains(id)) return m_fitFunctions[id];
    else return NULL;
}

QFFitAlgorithm* QFFitResultsEvaluation::getFitAlgorithm() const {
    return getFitAlgorithm(m_fitAlgorithm);
}

void QFFitResultsEvaluation::setFitAlgorithm(QString fitAlgorithm) {
    m_fitAlgorithm=fitAlgorithm;
}


QFFitFunction* QFFitResultsEvaluation::getFitFunction() {
    return getFitFunction(m_fitFunction);
}

void QFFitResultsEvaluation::setFitFunction(QString fitFunction) {
    if (!getAvailableFitFunctions().contains(fitFunction)) return;
    if (m_fitFunction!=fitFunction) {
        QString oldff=m_fitFunction;
        QFFitFunction* ffold=getFitFunction(m_fitFunction);
        QMap<QString, FitParameter> params;
        if (ffold) {
            for (int i=0; i<ffold->paramCount(); i++) {
                QString pid=ffold->getParameterID(i);
                params[pid].value=getFitValue(pid);
                params[pid].min=getFitMin(pid);
                params[pid].max=getFitMax(pid);
                params[pid].fix=getFitFix(pid);
            }
        }
        m_fitFunction=fitFunction;
        QFFitFunction* ff=getFitFunction(fitFunction);
        for (int i=0; i<ff->paramCount(); i++) {
            QString pid=ff->getParameterID(i);
            QString newid=getParameterStoreID(fitFunction, ff->getParameterID(i));
            if (params.contains(pid)) {
                if (params[pid].value!=getFitValue(pid)) setFitValue(pid, params[pid].value);
                if (params[pid].fix!=getFitFix(pid)) setFitFix(pid, params[pid].fix);
                if (params[pid].min!=getFitMin(pid)) setFitMin(pid, params[pid].min);
                if (params[pid].max!=getFitMax(pid)) setFitMax(pid, params[pid].max);
            }
        }
        emit propertiesChanged();
    }
}

void QFFitResultsEvaluation::storeQFFitAlgorithmParameters(QFFitAlgorithm* algorithm) {
    QString aid=algorithm->id();
    QStringList params=algorithm->getParameterIDs();
    for (int i=0; i<params.size(); i++) {
        QString pid=params[i];
        algorithm_parameterstore[aid].insert(pid, algorithm->getParameter(pid));
    }
    emit propertiesChanged();
}

void QFFitResultsEvaluation::restoreQFFitAlgorithmParameters(QFFitAlgorithm* algorithm) {
    QString aid=algorithm->id();
    QStringList params=algorithm->getParameterIDs();
    for (int i=0; i<params.size(); i++) {
        QString pid=params[i];
        algorithm->setParameter(pid, algorithm_parameterstore[aid].value(pid, algorithm->getParameter(pid)));
    }
}


void QFFitResultsEvaluation::intWriteData(QXmlStreamWriter& w) {
    w.writeStartElement("algorithms");
    w.writeAttribute("current", m_fitAlgorithm);
    intWriteDataAlgorithm(w);
    /*if (m_weighting==EqualWeighting) w.writeAttribute("weighting", "equal");
    if (m_weighting==StdDevWeighting) w.writeAttribute("weighting", "stddev");
    if (m_weighting==RunErrorWeighting) w.writeAttribute("weighting", "runerror");*/

    QMapIterator<QString, QMap<QString, QVariant> > fapit(algorithm_parameterstore);
    while (fapit.hasNext()) {
        fapit.next();
        w.writeStartElement("algorithm");
        w.writeAttribute("id", fapit.key());
        QMapIterator<QString, QVariant> mi(fapit.value());
        while (mi.hasNext()) {
            mi.next();
            w.writeStartElement("parameter");
            w.writeAttribute("id", mi.key());
            w.writeAttribute("type", getQVariantType(mi.value()));
            w.writeAttribute("data", getQVariantData(mi.value()));
            w.writeEndElement();
        }
        w.writeEndElement();
    }

    w.writeEndElement();
    w.writeStartElement("functions");
    w.writeAttribute("current", m_fitFunction);
    QMapIterator<QString, FitParameter> i(parameterStore);
    while (i.hasNext()) {
        i.next();
        w.writeStartElement("parameter");
        w.writeAttribute("id", i.key());
        if (i.value().valueSet) w.writeAttribute("value", QString::number(i.value().value, 'g', 12));
        if (i.value().errorSet) w.writeAttribute("error", QString::number(i.value().error, 'g', 12));
        if (i.value().minSet) w.writeAttribute("min", QString::number(i.value().min, 'g', 12));
        if (i.value().maxSet) w.writeAttribute("max", QString::number(i.value().max, 'g', 12));
        if (i.value().fixSet) w.writeAttribute("fix", QString((i.value().fix)?QString("1"):QString("0")));
        intWriteDataFitParam(i.key(), w);
        w.writeEndElement();
    }
    w.writeEndElement();
}

void QFFitResultsEvaluation::intReadData(QDomElement* e) {
    /*
        <algorithms current="..." weighting="equal|stddev">
          <algorithm id="name1">
            <parameter id="pname1" type="ptype" data="pdata" />
            <parameter id="pname2" type="ptype" data="pdata" />
          </algorithm>
          <algorithm id="name2">
            <parameter id="pname1" type="ptype" data="pdata" />
            <parameter id="pname2" type="ptype" data="pdata" />
          </algorithm>
        </algorithms>
    */

    QDomElement e1=e->firstChildElement("algorithms");
    if (e1.hasAttribute("current")) {
        QString a=e1.attribute("current");
        if (getAvailableFitAlgorithms().contains(a)) {
            setFitAlgorithm(a);
        } else {
            // STOP if unsupported fit algorithm was found
            setError(tr("found unsupported fitting algorithm with ID '%1', maybe you are missing a plugin").arg(a));
        }
    }
    intReadDataAlgorithm(e1);
    /*if (e1.hasAttribute("weighting")) {
        QString a=e1.attribute("weighting").toLower();
        m_weighting=EqualWeighting;
        if (a=="stddev") m_weighting=StdDevWeighting;
        if (a=="runerror") m_weighting=RunErrorWeighting;
    }*/
    QDomElement elt=e1.firstChildElement("algorithm");
    algorithm_parameterstore.clear();
    for (; !elt.isNull(); elt = elt.nextSiblingElement("algorithm")) {
        if (elt.hasAttribute("id")) {
            QString id=elt.attribute("id");
            //std::cout<<"  fit algorithm "<<id.toStdString()<<std::endl;
            QDomElement eltt=elt.firstChildElement("parameter");
            for (; !eltt.isNull(); eltt = eltt.nextSiblingElement("parameter")) {
                if (eltt.hasAttribute("id") && eltt.hasAttribute("type") && eltt.hasAttribute("data")) {
                    QString pid=eltt.attribute("id");
                    QString ptype=eltt.attribute("type");
                    QString pdata=eltt.attribute("data");
                    //std::cout<<"    param "<<pid.toStdString()<<" type="<<ptype.toStdString()<<" data="<<pdata.toStdString()<<std::endl;
                    algorithm_parameterstore[id].insert(pid, getQVariantFromString(ptype, pdata));
                }
            }

            if (m_fitAlgorithms.contains(id)) restoreQFFitAlgorithmParameters(m_fitAlgorithms[id]);
        }
    }




    e1=e->firstChildElement("functions");
    if (e1.hasAttribute("current")) {
        QString a=e1.attribute("current");
        if (getAvailableFitFunctions().contains(a)) {
            setFitFunction(a);
        } else {
            // STOP if unsupported fit algorithm was found
            setError(tr("found unsupported fitting function with ID '%1', maybe you are missing a plugin").arg(a));
        }
    }

    // read <parameter> tag
    elt=e1.firstChildElement("parameter");
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
                p.value=elt.attribute("value").toDouble();
                p.valueSet=true;
            }
            if (elt.hasAttribute("error")) {
                p.error=elt.attribute("error").toDouble();
                p.errorSet=true;
            }
            if (elt.hasAttribute("min")) {
                p.min=elt.attribute("min").toDouble();
                p.minSet=true;
            }
            if (elt.hasAttribute("max")) {
                p.max=elt.attribute("max").toDouble();
                p.maxSet=true;
            }
            parameterStore[a]=p;
            intReadDataFitParam(a, p, elt);
        }
    }
}

QString QFFitResultsEvaluation::getParameterStoreID(QString parameter) {
    QString ff="";
    if (getFitFunction()!=NULL) {
        ff=getFitFunction()->id();
    }
    return getParameterStoreID(ff, parameter);
};


QString QFFitResultsEvaluation::getParameterStoreID(QString fitfunction, QString parameter) {
    return QString(fitfunction+"___"+parameter).trimmed().toLower();
};


bool QFFitResultsEvaluation::hasFit(QFRawDataRecord* r1, const QString& resultID) {
    QFRawDataRecord* r=r1;
    if (getFitFunction()==NULL) return false;
    if (r==NULL) r=getHighlightedRecord();
    if (r==NULL) return false;
    return r->resultsExistsFromEvaluation(resultID);
}

bool QFFitResultsEvaluation::hasFit(QFRawDataRecord* r) {
    return hasFit(r, getEvaluationResultID());
}

bool QFFitResultsEvaluation::hasFit() {
    return hasFit(getHighlightedRecord(), getEvaluationResultID());
}









void QFFitResultsEvaluation::setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetNumber(resultID, getFitParamID(parameterID), value, unit);
        emit resultsChanged();
    }
}
void QFFitResultsEvaluation::setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, QString unit) {
    if (r!=NULL) {
        //QFFitFunction* f=getFitFunction();
        r->resultsSetNumber(resultID, getFitParamID(parameterID), value, unit);
        emit resultsChanged();
    }
}

void QFFitResultsEvaluation::setFitResultValueString(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, QString value) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetString(resultID, getFitParamID(parameterID), value);
        emit resultsChanged();
    }
}

void QFFitResultsEvaluation::setFitResultValueBool(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, bool value) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetBoolean(resultID, getFitParamID(parameterID), value);
        emit resultsChanged();
    }
}

void QFFitResultsEvaluation::setFitResultValueInt(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, int64_t value) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetInteger(resultID, getFitParamID(parameterID), value, unit);
        emit resultsChanged();
    }
}

void QFFitResultsEvaluation::setFitResultValueInt(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, int64_t value, QString unit) {
    if (r!=NULL) {
        //QFFitFunction* f=getFitFunction();
        r->resultsSetInteger(resultID, getFitParamID(parameterID), value, unit);
        emit resultsChanged();
    }
}

void QFFitResultsEvaluation::setFitValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value) {
    if (r!=NULL) {
        QString dsid=getParameterStoreID(parameterID);
        if (hasFit(r, resultID)) {
            setFitResultValue(r, resultID, parameterID, value, getFitError(parameterID));
        } else {
            QFFitFunction* f=getFitFunction();
            if (f) {
                QFFitFunction::ParameterDescription d=f->getDescription(parameterID);
                if (d.userEditable) {
                    parameterStore[getParameterStoreID(parameterID)].value=value;
                    parameterStore[getParameterStoreID(parameterID)].valueSet=true;
                    emit propertiesChanged();
                }
            }
        }

    }
}

void QFFitResultsEvaluation::setFitError(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double error) {
    if (r!=NULL) {
        QString dsid=getParameterStoreID(parameterID);
        if (hasFit(r, resultID)) {
            setFitResultError(r, resultID, parameterID, error);
        } else {
            QFFitFunction* f=getFitFunction();
            if (f) {
                QFFitFunction::ParameterDescription d=f->getDescription(parameterID);
                if (d.userEditable) {
                    parameterStore[getParameterStoreID(parameterID)].error=error;
                    parameterStore[getParameterStoreID(parameterID)].errorSet=true;
                    emit propertiesChanged();
                }
            }
        }

    }
}

void QFFitResultsEvaluation::setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, double error) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetNumberError(resultID, getFitParamID(parameterID), value, error, unit);
        emit resultsChanged();
    }
}

void QFFitResultsEvaluation::setFitResultError(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double error) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetNumberError(resultID, getFitParamID(parameterID), getFitValue(parameterID), error, unit);
        emit resultsChanged();
    }
}

void QFFitResultsEvaluation::setFitResultValues(QFRawDataRecord* r, const QString& resultID, double* values, double* errors) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        if (f) {
            for (int i=0; i<f->paramCount(); i++) {
                QString pid=f->getParameterID(i);
                //setFitResultValue(pid, values[i], errors[i]);
                QString unit=f->getDescription(pid).unit;
                r->resultsSetNumberError(resultID, getFitParamID(pid), values[i], errors[i], unit);
            }
            emit propertiesChanged();
            emit resultsChanged();
        }

    }
}

void QFFitResultsEvaluation::setFitResultValuesVisible(QFRawDataRecord* r, const QString& resultID, double* values, double* errors) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        if (f) {
            for (int i=0; i<f->paramCount(); i++) {
                if (f->isParameterVisible(i, values)) {
                    QString pid=f->getParameterID(i);
                    //setFitResultValue(pid, values[i], errors[i]);
                    QString unit=f->getDescription(pid).unit;
                    r->resultsSetNumberError(resultID, getFitParamID(pid), values[i], errors[i], unit);
                }
            }
            emit propertiesChanged();
            emit resultsChanged();
        }

    }
}

bool QFFitResultsEvaluation::hasSpecial(QFRawDataRecord* r, const QString& id, const QString& paramid, double& value, double& error) {
    return false;
}

double QFFitResultsEvaluation::getFitValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID/*, const QMap<QString, double>& specials*/) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) {
        return 0;
    }
    if (!f->hasParameter(parameterID)) return 0;
    int pid=f->getParameterNum(parameterID);
    double res=0;
    if (pid>-1) res=f->getDescription(pid).initialValue;
    res=fitParamSettings->value(QString(m_fitFunction+"/"+parameterID), res).toDouble();
    QString psID=getParameterStoreID(parameterID);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].valueSet) {
            res=parameterStore[psID].value;
        }
    }
    //if (specials.contains(parameterID)) res=specials[parameterID];
    double sval=res, serr=0;
    if (hasSpecial(r, resultID, parameterID, sval, serr)) {
        res=sval;
    }
    if (hasFit(r, resultID)) {
        QString en=resultID;
        QString pid=getFitParamID(parameterID);
        if (r->resultsExists(en, pid)) res=r->resultsGetAsDouble(en, pid);
    }
    return res;
}

double QFFitResultsEvaluation::getFitError(QFRawDataRecord* r, const QString& resultID, const QString& parameterID/*, const QMap<QString, double>& specials*/)  {
    if (hasFit(r, resultID)) {
        if (r!=NULL) {
            return r->resultsGetErrorAsDouble(resultID, getFitParamID(parameterID));
        }
    }
    //if (specials.contains(parameterID)) return specials[parameterID];
    double sval=0, serr=0;
    if (hasSpecial(r, resultID, parameterID, sval, serr)) {
       return serr;
    }
    QString psID=getParameterStoreID(parameterID);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].errorSet) {
            return parameterStore[psID].error;
        }
    }
    return 0.0;
}

void QFFitResultsEvaluation::setFitFix(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, bool fix) {
    if (r!=NULL) {
        QString dsid=getParameterStoreID(parameterID);
        if (hasFit(r, resultID)) {
            setFitResultFix(r, resultID, parameterID, fix);
        } else {
            QFFitFunction* f=getFitFunction();
            if (f) {
                QFFitFunction::ParameterDescription d=f->getDescription(parameterID);
                if (d.userEditable) {
                    parameterStore[getParameterStoreID(parameterID)].fix=fix;
                    parameterStore[getParameterStoreID(parameterID)].fixSet=true;
                    emit propertiesChanged();
                }
            }
        }

    }
}

void QFFitResultsEvaluation::setFitResultFix(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, bool fix) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetBoolean(resultID, getFitParamFixID(parameterID), fix);
        emit resultsChanged();
    }
}

bool QFFitResultsEvaluation::getFitFix(QFRawDataRecord* r, const QString& resultID, const QString& parameterID) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return 0;
    bool res=false;
    res=fitParamSettings->value(QString(m_fitFunction+"/"+parameterID+"_fix"), res).toBool();
    QString psID=getParameterStoreID(parameterID);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].fixSet) {
            res=parameterStore[psID].fix;
        }
    }
    if (hasFit(r, resultID)) {
        QString en=resultID;
        QString pid=getFitParamFixID(parameterID);
        if (r->resultsExists(en, pid)) res=r->resultsGetAsDouble(en, pid);
    }
    return res;
}


void QFFitResultsEvaluation::setFitResultValue(const QString& id, double value) {
    setFitResultValue(getHighlightedRecord(), getEvaluationResultID(), id, value);
}


void QFFitResultsEvaluation::setFitResultValue(const QString& id, double value, QString unit) {
    setFitResultValue(getHighlightedRecord(), getEvaluationResultID(), id, value, unit);
}

void QFFitResultsEvaluation::setFitResultValueString(const QString& id, QString value) {
    setFitResultValueString(getHighlightedRecord(), getEvaluationResultID(), id, value);
}

void QFFitResultsEvaluation::setFitResultValueBool(const QString& id, bool value) {
    setFitResultValueBool(getHighlightedRecord(), getEvaluationResultID(), id, value);
}

void QFFitResultsEvaluation::setFitResultValueInt(const QString& id, int64_t value) {
    setFitResultValueInt(getHighlightedRecord(), getEvaluationResultID(), id, value);
}

void QFFitResultsEvaluation::setFitResultValueInt(const QString& id, int64_t value, QString unit) {
    setFitResultValueInt(getHighlightedRecord(), getEvaluationResultID(), id, value, unit);
}

void QFFitResultsEvaluation::setFitValue(const QString& id, double value) {
    setFitValue(getHighlightedRecord(), getEvaluationResultID(), id, value);
}

void QFFitResultsEvaluation::setFitError(const QString& id, double error) {
    setFitError(getHighlightedRecord(), getEvaluationResultID(), id, error);
}

void QFFitResultsEvaluation::setFitResultValue(const QString& id, double value, double error) {
    setFitResultValue(getHighlightedRecord(), getEvaluationResultID(), id, value, error);
}

void QFFitResultsEvaluation::setFitResultError(const QString& id, double error) {
    setFitResultError(getHighlightedRecord(), getEvaluationResultID(), id, error);
}

void QFFitResultsEvaluation::setFitResultValues(double* values, double* errors) {
    setFitResultValues(getHighlightedRecord(), getEvaluationResultID(), values, errors);
}

void QFFitResultsEvaluation::setFitResultValuesVisible(double* values, double* errors) {
    setFitResultValuesVisible(getHighlightedRecord(), getEvaluationResultID(), values, errors);
}

double QFFitResultsEvaluation::getFitValue(const QString& id) {
    return getFitValue(getHighlightedRecord(), getEvaluationResultID(), id);
}

double QFFitResultsEvaluation::getFitError(const QString& id)  {
    return getFitError(getHighlightedRecord(), getEvaluationResultID(), id);
}

void QFFitResultsEvaluation::setFitFix(const QString& id, bool fix) {
    setFitFix(getHighlightedRecord(), getEvaluationResultID(), id, fix);
}

void QFFitResultsEvaluation::setFitResultFix(const QString& id, bool fix) {
    setFitResultFix(getHighlightedRecord(), getEvaluationResultID(), id, fix);
}

bool QFFitResultsEvaluation::getFitFix(const QString& id) {
    return getFitFix(getHighlightedRecord(), getEvaluationResultID(), id);
}

void QFFitResultsEvaluation::fillParameters(double* param) {
    fillParameters(getHighlightedRecord(), getEvaluationResultID(), param);
}

void QFFitResultsEvaluation::fillParameterErrors(double* param) {
    fillParameterErrors(getHighlightedRecord(), getEvaluationResultID(), param);
}

void QFFitResultsEvaluation::fillFix(bool* param) {
    fillFix(getHighlightedRecord(), getEvaluationResultID(), param);
}


double* QFFitResultsEvaluation::allocFillParameters() {
    return allocFillParameters(getHighlightedRecord(), getEvaluationResultID());

}

double* QFFitResultsEvaluation::allocFillParameterErrors() {
    return allocFillParameterErrors(getHighlightedRecord(), getEvaluationResultID());

}

bool* QFFitResultsEvaluation::allocFillFix() {
    return allocFillFix(getHighlightedRecord(), getEvaluationResultID());
}















void QFFitResultsEvaluation::fillParameters(QFRawDataRecord* r, const QString& resultID, double* param) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitValue(r, resultID, id);
        }
    }
}

void QFFitResultsEvaluation::fillParameterErrors(QFRawDataRecord* r, const QString& resultID, double* param) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitError(r, resultID, id);
        }
    }
}


void QFFitResultsEvaluation::fillFix(QFRawDataRecord* r, const QString& resultID, bool* param) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitFix(r, resultID, id);
        }
    }
}


double* QFFitResultsEvaluation::allocFillParameters(QFRawDataRecord* r, const QString& resultID) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        double* res=(double*)calloc(f->paramCount(), sizeof(double));
        fillParameters(r, resultID, res);
        return res;
    }
    return NULL;
}

double* QFFitResultsEvaluation::allocFillParameterErrors(QFRawDataRecord* r, const QString& resultID) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        double* res=(double*)calloc(f->paramCount(), sizeof(double));
        fillParameterErrors(r, resultID, res);
        return res;
    }
    return NULL;
}




bool* QFFitResultsEvaluation::allocFillFix(QFRawDataRecord* r, const QString& resultID) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        bool* res=(bool*)calloc(f->paramCount(), sizeof(bool));
        fillFix(r, resultID, res);
        return res;
    }
    return NULL;
}


double* QFFitResultsEvaluation::allocFillParametersMin() {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        double* res=(double*)calloc(f->paramCount(), sizeof(double));
        fillParametersMin(res);
        return res;
    }
    return NULL;
}

double* QFFitResultsEvaluation::allocFillParametersMax() {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        double* res=(double*)calloc(f->paramCount(), sizeof(double));
        fillParametersMax(res);
        return res;
    }
    return NULL;
}

void QFFitResultsEvaluation::fillParametersMin(double* param) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitMin(id);
        }
    }
}

void QFFitResultsEvaluation::fillParametersMax(double* param) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitMax(id);
        }
    }
}

/*! \brief return the default/initial/global value of a given parameter        */
double QFFitResultsEvaluation::getDefaultFitValue(const QString& id) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) {
        //std::cout<<"getFitValue("<<id.toStdString()<<") = "<<0<<" [getFitFunction()==NULL]\n";
        return 0;
    }
    if (!f->hasParameter(id)) return 0;
    int pid=f->getParameterNum(id);
    double res=0;
    if (pid>-1) res=f->getDescription(pid).initialValue;
    res=fitParamSettings->value(QString(m_fitFunction+"/"+id), res).toDouble();
    QString psID=getParameterStoreID(id);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].valueSet) {
            res=parameterStore[psID].value;
        }
    }
    return res;
}

/*! \brief return the default/initial/global fix of a given parameter        */
bool QFFitResultsEvaluation::getDefaultFitFix(const QString& id) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return 0;
    bool res=false;
    res=fitParamSettings->value(QString(m_fitFunction+"/"+id+"_fix"), res).toBool();
    QString psID=getParameterStoreID(id);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].fixSet) {
            res=parameterStore[psID].fix;
        }
    }
    return res;
}

/*! \brief reset the given parameter \a id to the initial/global/default value */
void QFFitResultsEvaluation::resetDefaultFitValue(const QString& id) {
    if (hasFit()) {
        QFRawDataRecord* r=getHighlightedRecord();
        QString en=getEvaluationResultID();
        QString pid=getFitParamID(id);
        if (r->resultsExists(en, pid)) r->resultsRemove(en, pid);
    }
}

/*! \brief reset the given parameter \a id to the initial/global/default fix */
void QFFitResultsEvaluation::resetDefaultFitFix(const QString& id) {
    if (hasFit()) {
        QFRawDataRecord* r=getHighlightedRecord();
        QString en=getEvaluationResultID();
        QString pid=getFitParamFixID(id);
        if (r->resultsExists(en, pid)) r->resultsRemove(en, pid);
    }
}

/*! \brief reset the all parameters to the initial/global/default value in current files */
void QFFitResultsEvaluation::resetAllFitValueCurrent() {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return ;
    if (hasFit()) {
        QFRawDataRecord* r=getHighlightedRecord();
        QString en=getEvaluationResultID();
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            QString pid=getFitParamID(id);
            if (r->resultsExists(en, pid)) r->resultsRemove(en, pid);
        }
    }
}

/*! \brief reset the all parameters to the initial/global/default fix in current files */
void QFFitResultsEvaluation::resetAllFitFixCurrent() {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return ;
    if (hasFit()) {
        QFRawDataRecord* r=getHighlightedRecord();
        QString en=getEvaluationResultID();
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            QString pid=getFitParamFixID(id);
            if (r->resultsExists(en, pid)) r->resultsRemove(en, pid);
        }
    }
}


/*! \brief reset the all fit results to the initial/global/default value in the current file and current run */
void QFFitResultsEvaluation::resetAllFitResultsCurrent() {
    QFRawDataRecord* re=getHighlightedRecord();
    if (!re) return;
    re->resultsClear(getEvaluationResultID());
}



void QFFitResultsEvaluation::setInitFitValue(const QString& id, double value, double error) {
    if (getHighlightedRecord()!=NULL) {
        QString dsid=getParameterStoreID(id);
        QFFitFunction* f=getFitFunction();
        if (f) {
            QFFitFunction::ParameterDescription d=f->getDescription(id);
            if (d.userEditable) {
                parameterStore[getParameterStoreID(id)].value=value;
                parameterStore[getParameterStoreID(id)].valueSet=true;
                if ((error!=0)&&(d.displayError==QFFitFunction::EditError)) {
                    parameterStore[getParameterStoreID(id)].error=error;
                    parameterStore[getParameterStoreID(id)].errorSet=true;
                } else {
                    parameterStore[getParameterStoreID(id)].error=0;
                    parameterStore[getParameterStoreID(id)].errorSet=false;
                }
                emit propertiesChanged();
            }
        }
    }
}

void QFFitResultsEvaluation::setInitFitFix(const QString& id, bool fix) {
    if (getHighlightedRecord()!=NULL) {
        QString dsid=getParameterStoreID(id);
        QFFitFunction* f=getFitFunction();
        if (f) {
            QFFitFunction::ParameterDescription d=f->getDescription(id);
            if (d.userEditable) {
                parameterStore[getParameterStoreID(id)].fix=fix;
                parameterStore[getParameterStoreID(id)].fixSet=true;
                emit propertiesChanged();
            }
        }
    }
}


void QFFitResultsEvaluation::setFitRange(const QString& id, double min, double max) {
    if (getHighlightedRecord()!=NULL) {
        QString dsid=getParameterStoreID(id);
        QFFitFunction* f=getFitFunction();
        if (f) {
            QFFitFunction::ParameterDescription d=f->getDescription(id);
            if (d.userEditable && d.userRangeEditable) {
                parameterStore[getParameterStoreID(id)].min=min;
                parameterStore[getParameterStoreID(id)].minSet=true;
                parameterStore[getParameterStoreID(id)].max=max;
                parameterStore[getParameterStoreID(id)].maxSet=true;
                emit propertiesChanged();
            }
        }
    }
}

void QFFitResultsEvaluation::setFitMin(const QString& id, double min) {
    if (getHighlightedRecord()!=NULL) {
        QString dsid=getParameterStoreID(id);
        QFFitFunction* f=getFitFunction();
        if (f) {
            QFFitFunction::ParameterDescription d=f->getDescription(id);
            if (d.userEditable && d.userRangeEditable) {
                parameterStore[getParameterStoreID(id)].min=min;
                parameterStore[getParameterStoreID(id)].minSet=true;
                emit propertiesChanged();
            }
        }
    }
}

void QFFitResultsEvaluation::setFitMax(const QString& id, double max) {
    if (getHighlightedRecord()!=NULL) {
        QString dsid=getParameterStoreID(id);
        QFFitFunction* f=getFitFunction();
        if (f) {
            QFFitFunction::ParameterDescription d=f->getDescription(id);
            if (d.userEditable && d.userRangeEditable) {
                parameterStore[getParameterStoreID(id)].max=max;
                parameterStore[getParameterStoreID(id)].maxSet=true;
                emit propertiesChanged();
            }
        }
    }
}

double QFFitResultsEvaluation::getFitMin(const QString& id)  {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) {
        //std::cout<<"getFitMin("<<id.toStdString()<<") = 0 [getFitFunction()==NULL]\n";
        return 0;
    }
    double res=f->getDescription(id).minValue;
    res=fitParamSettings->value(QString(m_fitFunction+"/"+id+"_min"), res).toDouble();
    QString psID=getParameterStoreID(id);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].minSet) {
            res=parameterStore[psID].min;
        }
    }
    //std::cout<<"getFitMin("<<id.toStdString()<<") = "<<res<<"\n";
    return res;
}

double QFFitResultsEvaluation::getFitMax(const QString& id) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) {
        //std::cout<<"getFitMax("<<id.toStdString()<<") = 0 [getFitFunction()==NULL]\n";
        return 0;
    }
    double res=f->getDescription(id).maxValue;
    res=fitParamSettings->value(QString(m_fitFunction+"/"+id+"_max"), res).toDouble();
    QString psID=getParameterStoreID(id);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].maxSet) {
            res=parameterStore[psID].max;
        }
    }
    //std::cout<<"getFitMax("<<id.toStdString()<<") = "<<res<<"\n";
    return res;
}
