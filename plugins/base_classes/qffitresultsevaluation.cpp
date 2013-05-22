#include "qffitresultsevaluation.h"
#include "qftools.h"
#include "qffitfunctionmanager.h"
#include "qffitalgorithmmanager.h"





QFFitResultsEvaluation::QFFitResultsEvaluation(const QString& fitFunctionPrefix, QFProject* parent, bool showRDRList, bool useSelection) :
    QFEvaluationItem(parent, showRDRList, useSelection)
{
    // open ini-file to superseede fit parameter defaults from QFFitFunction
    QString inifn=parent->getServices()->getOptions()->getConfigFileDirectory()+QString("/fitparams_%1.ini").arg(getType());
    fitParamSettings=new QSettings(inifn, QSettings::IniFormat);
    QString inifng=parent->getServices()->getOptions()->getGlobalConfigFileDirectory()+QString("/fitparams_%1.ini").arg(getType());
    fitParamGlobalSettings=new QSettings(inifng, QSettings::IniFormat);

    m_fitAlgorithm="";
    m_fitFunction="";

    doFitThread=new QFFitAlgorithmThreadedFit(this);


    // get list of applicable fit functions
    if (!fitFunctionPrefix.contains(',')) {
        m_fitFunctions=parent->getServices()->getFitFunctionManager()->getModels(fitFunctionPrefix, this);
    } else {
        QStringList sl=fitFunctionPrefix.split(',');
        m_fitFunctions.clear();
        for (int i=0; i<sl.size(); i++) {
            m_fitFunctions=m_fitFunctions.unite(parent->getServices()->getFitFunctionManager()->getModels(sl[i], this));
        }
    }
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

    if (fitParamSettings) delete fitParamSettings;
    if (fitParamGlobalSettings) delete fitParamGlobalSettings;
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


QFFitFunction* QFFitResultsEvaluation::getFitFunction() const {
    return getFitFunction(getHighlightedRecord());
}

QString QFFitResultsEvaluation::getFitFunctionID() const
{
    return getFitFunctionID(getHighlightedRecord());
}

QFFitFunction *QFFitResultsEvaluation::getFitFunction( QFRawDataRecord *rdr) const
{
    return getFitFunction(getFitFunctionID(rdr));
}

QString QFFitResultsEvaluation::getFitFunctionID(QFRawDataRecord *rdr) const
{
    return m_fitFunction;
}

QFFitAlgorithm *QFFitResultsEvaluation::createFitAlgorithm(QObject* parent) const
{
    return QFFitAlgorithmManager::getInstance()->createAlgorithm(m_fitAlgorithm, parent);
}

QFFitFunction *QFFitResultsEvaluation::createFitFunction(QFRawDataRecord *rdr, QObject *parent) const
{
    return QFFitFunctionManager::getInstance()->createFunction(getFitFunctionID(rdr), parent);
}

QString QFFitResultsEvaluation::transformResultID(const QString &resultID) const {
    return resultID;
}

void QFFitResultsEvaluation::setFitFunction(QString fitFunction) {
    if (!getAvailableFitFunctions().contains(fitFunction)) return;
    if (m_fitFunction!=fitFunction) {
        QString oldff=m_fitFunction;
        QFFitFunction* ffold=getFitFunction(m_fitFunction);
        QHash<QString, FitParameter> params;
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
            QString newid=getParameterStoreID(getHighlightedRecord(), fitFunction, ff->getParameterID(i));
            if (params.contains(pid)) {
                if (params[pid].value!=getFitValue(pid)) setFitValue(pid, params[pid].value);
                if (params[pid].fix!=getFitFix(pid)) setFitFix(pid, params[pid].fix);
                if (params[pid].min!=getFitMin(pid)) setFitMin(pid, params[pid].min);
                if (params[pid].max!=getFitMax(pid)) setFitMax(pid, params[pid].max);
            }
        }
        //emitPropertiesChanged();
        project->setDataChanged();
    }
}

void QFFitResultsEvaluation::storeQFFitAlgorithmParameters(QFFitAlgorithm* algorithm) {
    QString aid=algorithm->id();
    QStringList params=algorithm->getParameterIDs();
    for (int i=0; i<params.size(); i++) {
        QString pid=params[i];
        algorithm_parameterstore[aid].insert(pid, algorithm->getParameter(pid));
    }
    //emitPropertiesChanged();
    project->setDataChanged();
}

void QFFitResultsEvaluation::restoreQFFitAlgorithmParameters(QFFitAlgorithm* algorithm) const {
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

    QHashIterator<QString, QHash<QString, QVariant> > fapit(algorithm_parameterstore);
    while (fapit.hasNext()) {
        fapit.next();
        w.writeStartElement("algorithm");
        w.writeAttribute("id", fapit.key());
        QHashIterator<QString, QVariant> mi(fapit.value());
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
            intReadDataFitParam(a, p, elt);
        }
    }
}

QString QFFitResultsEvaluation::getParameterStoreID(QFRawDataRecord *rdr, QString parameter) const {
    QString ff="";
    if (getFitFunction(rdr)!=NULL) {
        ff=getFitFunction(rdr)->id();
    }
    return getParameterStoreID(rdr, ff, parameter);
}


QString QFFitResultsEvaluation::getParameterStoreID(QFRawDataRecord *rdr, QString fitfunction, QString parameter) const {
    QString r=rdrPointerToParameterStoreID(rdr);
    if (!r.isEmpty()) r+="__";
    return QString(r+fitfunction+"___"+parameter).trimmed().toLower();
}

QString QFFitResultsEvaluation::rdrPointerToParameterStoreID(QFRawDataRecord *rdr) const
{
    return "";
}


bool QFFitResultsEvaluation::hasFit(QFRawDataRecord* rin, const QString& evalID) const {
    QFRawDataRecord* r=rin;
    if (!rin) r=getHighlightedRecord();
    if (getFitFunction(r)==NULL) return false;
    if (r==NULL) r=getHighlightedRecord();
    if (r==NULL) return false;
    return r->resultsExistsFromEvaluation(transformResultID(evalID));
}

bool QFFitResultsEvaluation::hasFit(QFRawDataRecord* rin) const {
    QFRawDataRecord* r=rin;
    if (!rin) r=getHighlightedRecord();
    return hasFit(r, getEvaluationResultID(r));
}

bool QFFitResultsEvaluation::hasFit() const {
    return hasFit(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()));
}









void QFFitResultsEvaluation::setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value)  {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction(r);
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetNumber(transformResultID(resultID), getFitParamID(parameterID), value, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}
void QFFitResultsEvaluation::setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, QString unit)  {
    if (r!=NULL) {
        //QFFitFunction* f=getFitFunction(r);
        r->resultsSetNumber(transformResultID(resultID), getFitParamID(parameterID), value, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsEvaluation::setFitResultValueString(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, QString value)  {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction(r);
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetString(transformResultID(resultID), getFitParamID(parameterID), value);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsEvaluation::setFitResultValueBool(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, bool value)  {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction(r);
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetBoolean(transformResultID(resultID), getFitParamID(parameterID), value);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsEvaluation::setFitResultValueInt(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, int64_t value)  {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction(r);
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetInteger(transformResultID(resultID), getFitParamID(parameterID), value, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsEvaluation::setFitResultValueInt(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, int64_t value, QString unit)  {
    if (r!=NULL) {
        //QFFitFunction* f=getFitFunction(r);
        r->resultsSetInteger(transformResultID(resultID), getFitParamID(parameterID), value, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsEvaluation::setFitValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value) {
    if (r!=NULL) {
        if (hasFit(r, resultID)) {
            setFitResultValue(r, resultID, parameterID, value, getFitError(parameterID));
        } else {
            QFFitFunction* f=getFitFunction(r);
            if (f) {
                QString dsid=getParameterStoreID(r, f->id(), parameterID);
                QFFitFunction::ParameterDescription d=f->getDescription(parameterID);
                if (d.userEditable) {
                    parameterStore[dsid].value=value;
                    parameterStore[dsid].valueSet=true;
                    //emitPropertiesChanged();
                    project->setDataChanged();
                }
            }
        }

    }
}

void QFFitResultsEvaluation::setFitError(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double error) {
    if (r!=NULL) {
        if (hasFit(r, resultID)) {
            setFitResultError(r, resultID, parameterID, error);
        } else {
            QFFitFunction* f=getFitFunction(r);
            if (f) {
                QString dsid=getParameterStoreID(r, f->id(), parameterID);
                QFFitFunction::ParameterDescription d=f->getDescription(parameterID);
                if (d.userEditable) {
                    parameterStore[dsid].error=error;
                    parameterStore[dsid].errorSet=true;
                    //emitPropertiesChanged();
                    project->setDataChanged();
                }
            }
        }

    }
}

void QFFitResultsEvaluation::setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, double error)  {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction(r);
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetNumberError(transformResultID(resultID), getFitParamID(parameterID), value, error, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsEvaluation::setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, QVector<double> value, QVector<double> error, QString unit)
{
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction(r);
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetNumberErrorList(transformResultID(resultID), getFitParamID(parameterID), value, error, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsEvaluation::setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, QVector<double> value, QString unit)
{
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction(r);
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetNumberList(transformResultID(resultID), getFitParamID(parameterID), value, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsEvaluation::setFitResultError(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double error)  {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction(r);
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetNumberError(transformResultID(resultID), getFitParamID(parameterID), getFitValue(parameterID), error, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsEvaluation::setFitResultValues(QFRawDataRecord* r, const QString& resultID, double* values, double* errors)  {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction(r);
        if (f) {
            for (int i=0; i<f->paramCount(); i++) {
                QString pid=f->getParameterID(i);
                //setFitResultValue(pid, values[i], errors[i]);
                QString unit=f->getDescription(pid).unit;
                r->resultsSetNumberError(transformResultID(resultID), getFitParamID(pid), values[i], errors[i], unit);
            }
            emitResultsChanged();
        }

    }
}

void QFFitResultsEvaluation::setFitResultValuesVisible(QFRawDataRecord* r, const QString& resultID, double* values, double* errors)  {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction(r);
        if (f) {
            for (int i=0; i<f->paramCount(); i++) {
                if (f->isParameterVisible(i, values)) {
                    QString pid=f->getParameterID(i);
                    //setFitResultValue(pid, values[i], errors[i]);
                    QString unit=f->getDescription(pid).unit;
                    r->resultsSetNumberError(transformResultID(resultID), getFitParamID(pid), values[i], errors[i], unit);
                }
            }
            emitResultsChanged();
        }

    }
}

void QFFitResultsEvaluation::setFitResultValuesVisibleWithGroupAndLabel(QFRawDataRecord* r, const QString& resultID, const double *values, const double *errors, const QString& group, const bool *fix, const QString& fixGroup, bool sortPriority)  {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction(r);
        if (f) {
            for (int i=0; i<f->paramCount(); i++) {
                if (f->isParameterVisible(i, values)) {
                    QString pid=f->getParameterID(i);
                    //setFitResultValue(pid, values[i], errors[i]);
                    QString unit=f->getDescription(pid).unit;
                    QString fpid=getFitParamID(pid);
                    QString ffid= getFitParamFixID(pid);
                    r->resultsSetNumberError(transformResultID(resultID), fpid, values[i], errors[i], unit);
                    r->resultsSetGroup(transformResultID(resultID), fpid, group);
                    r->resultsSetLabel(transformResultID(resultID), fpid, f->getDescription(pid).name, f->getDescription(pid).label);
                    r->resultsSetSortPriority(transformResultID(resultID), fpid, sortPriority);
                    if (fix) {
                        r->resultsSetBoolean(transformResultID(resultID), ffid, fix[i]);
                        if (!fixGroup.isEmpty()) r->resultsSetGroup(transformResultID(resultID),ffid, fixGroup);
                        r->resultsSetLabel(transformResultID(resultID), ffid, f->getDescription(pid).name+tr(", fix"), f->getDescription(pid).label+tr(", fix"));
                        r->resultsSetSortPriority(transformResultID(resultID),  ffid, sortPriority);
                    }
                }
            }
            emitResultsChanged();
        }

    }
}

bool QFFitResultsEvaluation::hasSpecial(const QFRawDataRecord* r, const QString& id, const QString& paramid, double& value, double& error) const {
    return false;
}

double QFFitResultsEvaluation::getFitValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID/*, const QMap<QString, double>& specials*/) const {
    QFFitFunction* f=getFitFunction(r);
    if (f==NULL) {
        return 0;
    }
    if (!f->hasParameter(parameterID)) return 0;
    int pid=f->getParameterNum(parameterID);
    double res=0;
    if (pid>-1) res=f->getDescription(pid).initialValue;
    double value=0;
    if (overrideFitFunctionPreset(r, parameterID, value)) res=value;

    res=fitParamGlobalSettings->value(QString(getFitFunctionID(r)+"/"+parameterID), res).toDouble();
    res=fitParamSettings->value(QString(getFitFunctionID(r)+"/"+parameterID), res).toDouble();

    QString psID=getParameterStoreID(r, parameterID);
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
        QString en=transformResultID(resultID);
        QString pid=getFitParamID(parameterID);
        if (r->resultsExists(en, pid)) res=r->resultsGetAsDouble(en, pid);
    }
    return res;
}

double QFFitResultsEvaluation::getFitError(QFRawDataRecord* r, const QString& resultID, const QString& parameterID/*, const QMap<QString, double>& specials*/) const  {

    //if (specials.contains(parameterID)) return specials[parameterID];
    double sval=0, serr=0;

    double value=0;
    if (overrideFitFunctionPresetError(r, parameterID, value)) serr=value;

    if (hasFit(r, resultID)) {
        if (r!=NULL) {
            return r->resultsGetErrorAsDouble(transformResultID(resultID), getFitParamID(parameterID));
        }
    }
    if (hasSpecial(r, resultID, parameterID, sval, serr)) {
       return serr;
    }
    QString psID=getParameterStoreID(r, parameterID);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].errorSet) {
            return parameterStore[psID].error;
        }
    }

    return serr;
}

void QFFitResultsEvaluation::setFitFix(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, bool fix) {
    if (r!=NULL) {
        QString dsid=getParameterStoreID(r, parameterID);
        if (hasFit(r, resultID)) {
            setFitResultFix(r, resultID, parameterID, fix);
        } else {
            QFFitFunction* f=getFitFunction(r);
            if (f) {
                QFFitFunction::ParameterDescription d=f->getDescription(parameterID);
                if (d.userEditable) {
                    parameterStore[getParameterStoreID(r, parameterID)].fix=fix;
                    parameterStore[getParameterStoreID(r, parameterID)].fixSet=true;
                    //emitPropertiesChanged();
                    project->setDataChanged();
                }
            }
        }

    }
}

void QFFitResultsEvaluation::setFitResultFix(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, bool fix)  {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction(r);
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetBoolean(transformResultID(resultID), getFitParamFixID(parameterID), fix);
        emitResultsChanged(r, resultID, getFitParamFixID(parameterID));
    }
}

bool QFFitResultsEvaluation::getFitFix(QFRawDataRecord* r, const QString& resultID, const QString& parameterID) const {
    QFFitFunction* f=getFitFunction(r);
    if (f==NULL) return 0;
    bool res=false;

    int pid=f->getParameterNum(parameterID);
    if (pid>-1) res=f->getDescription(pid).initialFix;

    bool value=0;
    if (overrideFitFunctionPresetFix(r, parameterID, value)) res=value;

    res=fitParamGlobalSettings->value(QString(getFitFunctionID(r)+"/"+parameterID+"_fix"), res).toBool();
    res=fitParamSettings->value(QString(getFitFunctionID(r)+"/"+parameterID+"_fix"), res).toBool();
    QString psID=getParameterStoreID(r, parameterID);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].fixSet) {
            res=parameterStore[psID].fix;
        }
    }
    if (hasFit(r, resultID)) {
        QString en=transformResultID(resultID);
        QString pid=getFitParamFixID(parameterID);
        if (r->resultsExists(en, pid)) res=r->resultsGetAsDouble(en, pid);
    }
    return res;
}

void QFFitResultsEvaluation::setFitResultGroup(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, const QString& group)  {
    if (r!=NULL) {
        //QFFitFunction* f=getFitFunction(r);
        QString unit="";
        /*if (f) {
            int pid=f->getParameterNum(parameterID);
        }*/
        r->resultsSetGroup(transformResultID(resultID), getFitParamID(parameterID), group);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsEvaluation::setFitResultLabel(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, const QString& label, const QString& label_richtext)  {
    if (r!=NULL) {
        //QFFitFunction* f=getFitFunction(r);
        QString unit="";
        /*if (f) {
            int pid=f->getParameterNum(parameterID);
        }*/
        r->resultsSetLabel(transformResultID(resultID), getFitParamID(parameterID), label, label_richtext);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}


void QFFitResultsEvaluation::setFitResultEvaluationGroup(QFRawDataRecord* r, const QString& resultID, const QString& group)  {
    if (r!=NULL) {
        r->resultsSetEvaluationGroup(transformResultID(resultID), group);
        emitResultsChanged(r, resultID);
    }
}


void QFFitResultsEvaluation::setFitResultEvaluationDescription(QFRawDataRecord* r, const QString& resultID, const QString& description)  {
    if (r!=NULL) {
        r->resultsSetEvaluationDescription(transformResultID(resultID), description);
        emitResultsChanged(r, resultID);
    }
}

void QFFitResultsEvaluation::setFitResultGroup(const QString& parameterID, const QString& group)  {
    setFitResultGroup(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), parameterID, group);
}

void QFFitResultsEvaluation::setFitResultLabel(const QString& parameterID, const QString& label, const QString& label_richtext)  {
    setFitResultLabel(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), parameterID, label, label_richtext);
}

void QFFitResultsEvaluation::setFitResultEvaluationGroup(const QString& group)  {
    setFitResultEvaluationGroup(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), group);
}

void QFFitResultsEvaluation::setFitResultEvaluationDescription(const QString& description)  {
    setFitResultEvaluationDescription(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), description);
}


void QFFitResultsEvaluation::setFitResultValue(const QString& id, double value)  {
    setFitResultValue(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), id, value);
}


void QFFitResultsEvaluation::setFitResultValue(const QString& id, double value, QString unit)  {
    setFitResultValue(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), id, value, unit);
}

void QFFitResultsEvaluation::setFitResultValueString(const QString& id, QString value)  {
    setFitResultValueString(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), id, value);
}

void QFFitResultsEvaluation::setFitResultValueBool(const QString& id, bool value)  {
    setFitResultValueBool(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), id, value);
}

void QFFitResultsEvaluation::setFitResultValueInt(const QString& id, int64_t value)  {
    setFitResultValueInt(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), id, value);
}

void QFFitResultsEvaluation::setFitResultValueInt(const QString& id, int64_t value, QString unit)  {
    setFitResultValueInt(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), id, value, unit);
}

void QFFitResultsEvaluation::setFitValue(const QString& id, double value, QFRawDataRecord *r) {
    if (!r) setFitValue(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), id, value);
    else setFitValue(r, getEvaluationResultID(r), id, value);
}

void QFFitResultsEvaluation::setFitError(const QString& id, double error, QFRawDataRecord *r) {
    if (!r) setFitError(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), id, error);
    else setFitError(r, getEvaluationResultID(r), id, error);
}

void QFFitResultsEvaluation::setFitResultValue(const QString& id, double value, double error)  {
    setFitResultValue(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), id, value, error);
}

void QFFitResultsEvaluation::setFitResultError(const QString& id, double error)  {
    setFitResultError(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), id, error);
}

void QFFitResultsEvaluation::setFitResultValues(double* values, double* errors)  {
    setFitResultValues(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), values, errors);
}

void QFFitResultsEvaluation::setFitResultValuesVisible(double* values, double* errors)  {
    setFitResultValuesVisible(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), values, errors);
}

double QFFitResultsEvaluation::getFitValue(const QString& id, QFRawDataRecord *r) const {
    if (!r) return getFitValue(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), id);
    else return getFitValue(r, getEvaluationResultID(r), id);
}

double QFFitResultsEvaluation::getFitError(const QString& id, QFRawDataRecord *r) const  {
    if (!r) return getFitError(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), id);
    else return getFitError(r, getEvaluationResultID(r), id);
}

void QFFitResultsEvaluation::setFitFix(const QString& id, bool fix, QFRawDataRecord *r) {
    if (!r) setFitFix(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), id, fix);
    else setFitFix(r, getEvaluationResultID(r), id, fix);
}

void QFFitResultsEvaluation::setFitResultFix(const QString& id, bool fix)  {
    setFitResultFix(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), id, fix);
}

bool QFFitResultsEvaluation::getFitFix(const QString& id, QFRawDataRecord *r) const {
    if (!r) return getFitFix(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), id);
    else return getFitFix(r, getEvaluationResultID(r), id);
}

void QFFitResultsEvaluation::fillParameters(double* param, QFFitFunction *function) const {
    fillParameters(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), param, function);
}

void QFFitResultsEvaluation::fillParameterErrors(double* param, QFFitFunction *function) const {
    fillParameterErrors(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), param, function);
}

void QFFitResultsEvaluation::fillFix(bool* param, QFFitFunction* function) const {
    fillFix(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), param, function);
}


double* QFFitResultsEvaluation::allocFillParameters(QFFitFunction *function) const {
    return allocFillParameters(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), function);

}

double* QFFitResultsEvaluation::allocFillParameterErrors(QFFitFunction* function) const {
    return allocFillParameterErrors(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), function);

}

bool* QFFitResultsEvaluation::allocFillFix(QFFitFunction* function) const {
    return allocFillFix(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), function);
}















void QFFitResultsEvaluation::fillParameters(QFRawDataRecord* r, const QString& resultID, double* param, QFFitFunction *function) const {
    QFFitFunction* f=function;
    if (!f) f=getFitFunction(r);
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitValue(r, resultID, id);
        }
    }
}

void QFFitResultsEvaluation::fillParameterErrors(QFRawDataRecord* r, const QString& resultID, double* param, QFFitFunction *function) const {
    QFFitFunction* f=function;
    if (!f) f=getFitFunction(r);
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitError(r, resultID, id);
        }
    }
}


void QFFitResultsEvaluation::fillFix(QFRawDataRecord* r, const QString& resultID, bool* param, QFFitFunction *function) const {
    QFFitFunction* f=function;
    if (!f) f=getFitFunction(r);
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitFix(r, resultID, id);
        }
    }
}


double* QFFitResultsEvaluation::allocFillParameters(QFRawDataRecord* r, const QString& resultID, QFFitFunction *function) const {
    //if (function) qDebug()<<"  QFFitResultsEvaluation::allocFillParameters("<<r->getName()<<", "<<resultID<<", "<<function->id()<<")";
    //else qDebug()<<"  QFFitResultsEvaluation::allocFillParameters("<<r->getName()<<", "<<resultID<<", "<<NULL<<")";
    QFFitFunction* f=function;
    if (!f) f=getFitFunction(r);
    if (f!=NULL) {
        double* res=(double*)calloc(f->paramCount(), sizeof(double));
        //qDebug()<<"    result size: "<<f->paramCount();
        fillParameters(r, resultID, res, f);
        return res;
    }
    return NULL;
}

double* QFFitResultsEvaluation::allocFillParameterErrors(QFRawDataRecord* r, const QString& resultID, QFFitFunction *function) const {
    QFFitFunction* f=function;
    if (!f) f=getFitFunction(r);
    if (f!=NULL) {
        double* res=(double*)calloc(f->paramCount(), sizeof(double));
        fillParameterErrors(r, resultID, res, f);
        return res;
    }
    return NULL;
}




bool* QFFitResultsEvaluation::allocFillFix(QFRawDataRecord* r, const QString& resultID, QFFitFunction *function) const {
    QFFitFunction* f=function;
    if (!f) f=getFitFunction(r);
    if (f!=NULL) {
        bool* res=(bool*)calloc(f->paramCount(), sizeof(bool));
        fillFix(r, resultID, res, f);
        return res;
    }
    return NULL;
}


double* QFFitResultsEvaluation::allocFillParametersMin(QFRawDataRecord* r, QFFitFunction *function) const {
    QFFitFunction* f=function;
    if (!f) f=getFitFunction(getHighlightedRecord());

    if (f!=NULL) {
        double* res=(double*)calloc(f->paramCount(), sizeof(double));
        fillParametersMin(res, r, f);
        return res;
    }
    return NULL;
}

double* QFFitResultsEvaluation::allocFillParametersMax(QFRawDataRecord* r, QFFitFunction *function) const {
    QFFitFunction* f=function;
    if (!f) f=getFitFunction(getHighlightedRecord());

    if (f!=NULL) {
        double* res=(double*)calloc(f->paramCount(), sizeof(double));
        fillParametersMax(res, r, f);
        return res;
    }
    return NULL;
}

void QFFitResultsEvaluation::fillParametersMin(double* param, QFRawDataRecord* r, QFFitFunction* function) const {
    QFFitFunction* f=function;
    if (!r) r=getHighlightedRecord();
    if (!f) f=getFitFunction(r);
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitMin(id, r);
        }
    }
}

void QFFitResultsEvaluation::fillParametersMax(double* param, QFRawDataRecord* r, QFFitFunction* function) const {
    QFFitFunction* f=function;
    if (!r) r=getHighlightedRecord();
    if (!f) f=getFitFunction(r);

    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitMax(id, r);
        }
    }
}

/*! \brief return the default/initial/global value of a given parameter        */
double QFFitResultsEvaluation::getDefaultFitValue(const QString& id, QFRawDataRecord *rin) const {
    QFRawDataRecord* r=rin;
    if (!rin) r=getHighlightedRecord();
    QFFitFunction* f=getFitFunction(r);
    if (f==NULL) {
        //std::cout<<"getFitValue("<<id.toStdString()<<") = "<<0<<" [getFitFunction(r)==NULL]\n";
        return 0;
    }
    if (!f->hasParameter(id)) return 0;
    int pid=f->getParameterNum(id);
    double res=0;
    if (pid>-1) res=f->getDescription(pid).initialValue;
    /*double value=0;
    if (overrideFitFunctionPreset(id, value)) res=value;*/

    res=fitParamGlobalSettings->value(QString(getFitFunctionID(r)+"/"+id), res).toDouble();
    res=fitParamSettings->value(QString(getFitFunctionID(r)+"/"+id), res).toDouble();
    QString psID=getParameterStoreID(r, id);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].valueSet) {
            res=parameterStore[psID].value;
        }
    }
    return res;
}

/*! \brief return the default/initial/global fix of a given parameter        */
bool QFFitResultsEvaluation::getDefaultFitFix(const QString& id, QFRawDataRecord *rin) const {
    QFRawDataRecord* r=rin;
    if (!rin) r=getHighlightedRecord();
    QFFitFunction* f=getFitFunction(r);
    if (f==NULL) return 0;
    bool res=false;
    res=fitParamGlobalSettings->value(QString(getFitFunctionID(r)+"/"+id+"_fix"), res).toBool();
    res=fitParamSettings->value(QString(getFitFunctionID(r)+"/"+id+"_fix"), res).toBool();
    QString psID=getParameterStoreID(r, id);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].fixSet) {
            res=parameterStore[psID].fix;
        }
    }
    return res;
}

void QFFitResultsEvaluation::resetAllFitResults(QFRawDataRecord *rin, const QString &resultID) {
    QFRawDataRecord* r=rin; if (!r) r=getHighlightedRecord();
    QFFitFunction* f=getFitFunction(r);
    if (!f || !r) return ;
    bool doEmit=r->isEmitResultsChangedEnabled();
    r->disableEmitResultsChanged();
    /*for (int i=0; i<f->paramCount(); i++) {
        QString id=f->getParameterID(i);
        resetDefaultFitValue(r, resultID, id);
    }*/
    r->resultsClear(resultID, "");
    if (doEmit) r->enableEmitResultsChanged(true);
}

/*! \brief reset the given parameter \a id to the initial/global/default value */
void QFFitResultsEvaluation::resetDefaultFitValue(const QString& id) {
    /*if (hasFit()) {
        QFRawDataRecord* r=getHighlightedRecord();
        QString en=transformResultID(getEvaluationResultID());
        QString pid=getFitParamID(id);
        if (r->resultsExists(en, pid)) r->resultsRemove(en, pid);
    }*/
    resetDefaultFitValue(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), id);
}

/*! \brief reset the given parameter \a id to the initial/global/default fix */
void QFFitResultsEvaluation::resetDefaultFitFix(const QString& id) {
    /*if (hasFit()) {
        QFRawDataRecord* r=getHighlightedRecord();
        QString en=transformResultID(getEvaluationResultID());
        QString pid=getFitParamFixID(id);
        if (r->resultsExists(en, pid)) r->resultsRemove(en, pid);
    }*/
    resetDefaultFitFix(getHighlightedRecord(), getEvaluationResultID(getHighlightedRecord()), id);
}

/*! \brief reset the given parameter \a id to the initial/global/default value */
void QFFitResultsEvaluation::resetDefaultFitValue(QFRawDataRecord* rin, const QString& resultID, const QString& id) {
    QFRawDataRecord* r=rin;
    if (!rin) r=getHighlightedRecord();
    if (hasFit(r)) {
        QString en=transformResultID(resultID);
        QString pid=getFitParamID(id);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        if (r->resultsExists(en, pid)) r->resultsRemove(en, pid);
        if (doEmit) r->enableEmitResultsChanged(true);
    }
}

/*! \brief reset the given parameter \a id to the initial/global/default fix */
void QFFitResultsEvaluation::resetDefaultFitFix(QFRawDataRecord* rin, const QString& resultID, const QString& id) {
    QFRawDataRecord* r=rin;
    if (!rin) r=getHighlightedRecord();
    if (hasFit(r)) {
        QString en=transformResultID(resultID);
        QString pid=getFitParamFixID(id);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        if (doEmit) r->enableEmitResultsChanged(true);
        if (r->resultsExists(en, pid)) r->resultsRemove(en, pid);
    }
}

void QFFitResultsEvaluation::resetAllFitValueCurrent(QFRawDataRecord* rin) {
    QFRawDataRecord* r=rin; if (!r) r=getHighlightedRecord();
    QFFitFunction* f=getFitFunction(r);
    if (!f || !r) return ;
    bool doEmit=r->isEmitResultsChangedEnabled();
    r->disableEmitResultsChanged();
    for (int i=0; i<f->paramCount(); i++) {
        QString id=f->getParameterID(i);
        resetDefaultFitValue(r, getEvaluationResultID(r), id);
    }
    if (doEmit) r->enableEmitResultsChanged(true);
}

void QFFitResultsEvaluation::resetAllFitFixCurrent(QFRawDataRecord* rin) {
    QFFitFunction* f=getFitFunction();
    QFRawDataRecord* r=rin; if (!r) r=getHighlightedRecord();
    if (!f || !r) return ;
    bool doEmit=r->isEmitResultsChangedEnabled();
    r->disableEmitResultsChanged();
    for (int i=0; i<f->paramCount(); i++) {
        QString id=f->getParameterID(i);
        resetDefaultFitFix(r, getEvaluationResultID(r), id);
    }
    if (doEmit) r->enableEmitResultsChanged(true);
}




/*! \brief reset the all fit results to the initial/global/default value in the current file and current run */
void QFFitResultsEvaluation::resetAllFitResultsCurrent(QFRawDataRecord* r) {
    QFRawDataRecord* re=r; if (!re) re=getHighlightedRecord();
    if (!re) return;
    re->resultsClear(transformResultID(getEvaluationResultID(re)));
}

void QFFitResultsEvaluation::resetAllFitResultsAllFiles() {
    QList<QPointer<QFRawDataRecord> > recs=getApplicableRecords();
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* r=recs[i];
        if (r) {
            r->disableEmitResultsChanged();
            QString en=transformResultID(getEvaluationResultID(r));
            r->resultsClear(en);
            r->enableEmitResultsChanged();
        }
    }
}

void QFFitResultsEvaluation::setInitFitValue(const QString& id, double value, double error, QFRawDataRecord *rin) {
    QFRawDataRecord* r=rin;
    if (!rin) r=getHighlightedRecord();
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction(r);
        if (f) {
            QString dsid=getParameterStoreID(r, f->id(), id);
            QFFitFunction::ParameterDescription d=f->getDescription(id);
            if (d.userEditable) {
                parameterStore[dsid].value=value;
                parameterStore[dsid].valueSet=true;
                if ((error!=0)&&(d.displayError==QFFitFunction::EditError)) {
                    parameterStore[dsid].error=error;
                    parameterStore[dsid].errorSet=true;
                } else {
                    parameterStore[dsid].error=0;
                    parameterStore[dsid].errorSet=false;
                }
                //emitPropertiesChanged();
                project->setDataChanged();
            }
        }
    }
}

void QFFitResultsEvaluation::setInitFitFix(const QString& id, bool fix, QFRawDataRecord* rin) {
    QFRawDataRecord* r=rin;
    if (!rin) r=getHighlightedRecord();
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction(r);
        if (f) {
            QString dsid=getParameterStoreID(r, f->id(), id);
            QFFitFunction::ParameterDescription d=f->getDescription(id);
            if (d.userEditable) {
                parameterStore[dsid].fix=fix;
                parameterStore[dsid].fixSet=true;
                //emitPropertiesChanged();
                project->setDataChanged();
            }
        }
    }
}


void QFFitResultsEvaluation::setFitRange(const QString& id, double min, double max, QFRawDataRecord* rin) {
    QFRawDataRecord* r=rin;
    if (!rin) r=getHighlightedRecord();
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction(r);
        if (f) {
            QString dsid=getParameterStoreID(r, f->id(), id);
            QFFitFunction::ParameterDescription d=f->getDescription(id);
            if (d.userEditable && d.userRangeEditable) {
                parameterStore[dsid].min=min;
                parameterStore[dsid].minSet=true;
                parameterStore[dsid].max=max;
                parameterStore[dsid].maxSet=true;
                //emitPropertiesChanged();
                project->setDataChanged();
            }
        }
    }
}

void QFFitResultsEvaluation::setFitMin(const QString& id, double min, QFRawDataRecord* rin) {
    QFRawDataRecord* r=rin;
    if (!rin) r=getHighlightedRecord();
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction(r);
        if (f) {
            QString dsid=getParameterStoreID(r, f->id(), id);
            QFFitFunction::ParameterDescription d=f->getDescription(id);
            if (d.userEditable && d.userRangeEditable) {
                parameterStore[dsid].min=min;
                parameterStore[dsid].minSet=true;
                //emitPropertiesChanged();
                project->setDataChanged();
            }
        }
    }
}

void QFFitResultsEvaluation::setFitMax(const QString& id, double max, QFRawDataRecord* rin) {
    QFRawDataRecord* r=rin;
    if (!rin) r=getHighlightedRecord();
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction(r);
        if (f) {
            QString dsid=getParameterStoreID(r, f->id(), id);
            QFFitFunction::ParameterDescription d=f->getDescription(id);
            if (d.userEditable && d.userRangeEditable) {
                parameterStore[dsid].max=max;
                parameterStore[dsid].maxSet=true;
                //emitPropertiesChanged();
                project->setDataChanged();
            }
        }
    }
}

double QFFitResultsEvaluation::getFitMin(const QString& id, QFRawDataRecord *rin) const  {
    QFRawDataRecord* r=rin;
    if (!rin) r=getHighlightedRecord();
    QFFitFunction* f=getFitFunction(r);
    if (f==NULL) {
        //std::cout<<"getFitMin("<<id.toStdString()<<") = 0 [getFitFunction()==NULL]\n";
        return 0;
    }
    double res=f->getDescription(id).minValue;
    res=fitParamGlobalSettings->value(QString(getFitFunctionID(r)+"/"+id+"_min"), res).toDouble();
    res=fitParamSettings->value(QString(getFitFunctionID(r)+"/"+id+"_min"), res).toDouble();
    QString psID=getParameterStoreID(r, f->id(), id);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].minSet) {
            res=parameterStore[psID].min;
        }
    }
    //std::cout<<"getFitMin("<<id.toStdString()<<") = "<<res<<"\n";
    return res;
}

double QFFitResultsEvaluation::getFitMax(const QString& id, QFRawDataRecord *rin) const {
    QFRawDataRecord* r=rin;
    if (!rin) r=getHighlightedRecord();
    QFFitFunction* f=getFitFunction(r);
    if (f==NULL) {
        //std::cout<<"getFitMax("<<id.toStdString()<<") = 0 [getFitFunction()==NULL]\n";
        return 0;
    }
    double res=f->getDescription(id).maxValue;
    res=fitParamGlobalSettings->value(QString(getFitFunctionID(r)+"/"+id+"_max"), res).toDouble();
    res=fitParamSettings->value(QString(getFitFunctionID(r)+"/"+id+"_max"), res).toDouble();
    QString psID=getParameterStoreID(r, f->id(), id);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].maxSet) {
            res=parameterStore[psID].max;
        }
    }
    //std::cout<<"getFitMax("<<id.toStdString()<<") = "<<res<<"\n";
    return res;
}

bool QFFitResultsEvaluation::overrideFitFunctionPreset(QFRawDataRecord *r, QString paramName, double &value) const
{
    return false;
}

bool QFFitResultsEvaluation::overrideFitFunctionPresetError(QFRawDataRecord *r, QString paramName, double &value) const
{
    return false;
}

bool QFFitResultsEvaluation::overrideFitFunctionPresetFix(QFRawDataRecord *r, QString paramName, bool &value) const
{
    return false;
}



