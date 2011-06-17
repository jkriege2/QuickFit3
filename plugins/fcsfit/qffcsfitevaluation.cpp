#include "qffcsfitevaluation.h"
#include "qffcsfitevaluationeditor.h"
#include "../interfaces/qfrdrfcsdatainterface.h"



QFFCSFitEvaluation::QFFCSFitEvaluation(QFProject* parent):
    QFEvaluationItem(parent, true, false)
{
    QFileInfo inif(parent->getServices()->getOptions()->getIniFilename());
    QString inifn=inif.absolutePath()+"/fitparams.ini";
    fitParamSettings=new QSettings(inifn, QSettings::IniFormat);

    m_fitAlgorithm="";
    m_fitFunction="";
    m_weighting=EqualWeighting;
    m_currentRun=-1;


    // get list of applicable fit functions
    m_fitFunctions=parent->getServices()->getFitFunctionManager()->getModels("fcs_", this);
    if (m_fitFunctions.size()>0) m_fitFunction=m_fitFunctions.keys().at(0);

    // get list of available fit algorithms and store their parameters in the internal algorithm_parameterstore
    algorithm_parameterstore.clear();
    QStringList fita=parent->getServices()->getFitAlgorithmManager()->getIDList();
    for (int i=0; i<fita.size(); i++) {
        QFFitAlgorithm* falg=parent->getServices()->getFitAlgorithmManager()->createAlgorithm(fita[i], this);
        m_fitAlgorithms[fita[i]]=falg;
        storeQFFitAlgorithmParameters(falg);
    }
    if (m_fitAlgorithms.size()>0) m_fitAlgorithm=m_fitAlgorithms.keys().at(0);


    parameterStore.clear();
}

QFFCSFitEvaluation::~QFFCSFitEvaluation() {
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


void QFFCSFitEvaluation::intWriteData(QXmlStreamWriter& w) {
    w.writeStartElement("algorithms");
    w.writeAttribute("current", m_fitAlgorithm);
    if (m_weighting==EqualWeighting) w.writeAttribute("weighting", "equal");
    if (m_weighting==StdDevWeighting) w.writeAttribute("weighting", "stddev");

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
        w.writeEndElement();
    }
    w.writeEndElement();
}

void QFFCSFitEvaluation::intReadData(QDomElement* e) {
    /*
        <algorithms current="..." weighting="equal|stddev">
          <algorithm id="name1">
            <parameter id="pname1" type="ptype" data="pdata" />
            <parameter id="pname2" type="ptype" data="pdata" />
          </algorithm
          <algorithm id="name2">
            <parameter id="pname1" type="ptype" data="pdata" />
            <parameter id="pname2" type="ptype" data="pdata" />
          </algorithm
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
    if (e1.hasAttribute("weighting")) {
        QString a=e1.attribute("weighting").toLower();
        m_weighting=EqualWeighting;
        if (a=="stddev") m_weighting=StdDevWeighting;
    }
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
        }
    }
}


QFEvaluationEditor* QFFCSFitEvaluation::createEditor(QFPluginServices* services, QWidget* parent) {
    return new QFFCSFitEvaluationEditor(services, parent);
};

bool QFFCSFitEvaluation::isApplicable(QFRawDataRecord* record) {
    return record->inherits("QFRDRFCSDataInterface");
}

bool QFFCSFitEvaluation::hasFit(QFRawDataRecord* r1) {
    QFRawDataRecord* r=r1;
    if (getFitFunction()==NULL) return false;
    if (r==NULL) r=getHighlightedRecord();
    if (r==NULL) return false;
    QString rsid=getEvaluationResultID();
    return r->resultsExistsFromEvaluation(rsid);
}

bool QFFCSFitEvaluation::hasFit(QFRawDataRecord* r1, int run) {
    QFRawDataRecord* r=r1;
    if (getFitFunction()==NULL) return false;
    if (r==NULL) r=getHighlightedRecord();
    if (r==NULL) return false;
    QString rsid=getEvaluationResultID(run);
    return r->resultsExistsFromEvaluation(rsid);
}




void QFFCSFitEvaluation::setFitResultValue(QFRawDataRecord* r, int run, const QString& id, double value) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(id);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetNumber(getEvaluationResultID(run), getFitParamID(id), value, unit);
        emit resultsChanged();
    }
}
void QFFCSFitEvaluation::setFitResultValue(QFRawDataRecord* r, int run, const QString& id, double value, QString unit) {
    if (r!=NULL) {
        //QFFitFunction* f=getFitFunction();
        r->resultsSetNumber(getEvaluationResultID(run), getFitParamID(id), value, unit);
        emit resultsChanged();
    }
}

void QFFCSFitEvaluation::setFitResultValueString(QFRawDataRecord* r, int run, const QString& id, QString value) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(id);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetString(getEvaluationResultID(run), getFitParamID(id), value);
        emit resultsChanged();
    }
}

void QFFCSFitEvaluation::setFitResultValueBool(QFRawDataRecord* r, int run, const QString& id, bool value) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(id);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetBoolean(getEvaluationResultID(run), getFitParamID(id), value);
        emit resultsChanged();
    }
}

void QFFCSFitEvaluation::setFitResultValueInt(QFRawDataRecord* r, int run, const QString& id, int64_t value) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(id);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetInteger(getEvaluationResultID(run), getFitParamID(id), value, unit);
        emit resultsChanged();
    }
}

void QFFCSFitEvaluation::setFitResultValueInt(QFRawDataRecord* r, int run, const QString& id, int64_t value, QString unit) {
    if (r!=NULL) {
        //QFFitFunction* f=getFitFunction();
        r->resultsSetInteger(getEvaluationResultID(run), getFitParamID(id), value, unit);
        emit resultsChanged();
    }
}

void QFFCSFitEvaluation::setFitValue(QFRawDataRecord* r, int run, const QString& id, double value) {
    if (r!=NULL) {
        QString dsid=getParameterStoreID(id);
        if (hasFit(r, run)) {
            setFitResultValue(r, run, id, value, getFitError(id));
        } else {
            QFFitFunction* f=getFitFunction();
            if (f) {
                QFFitFunction::ParameterDescription d=f->getDescription(id);
                if (d.userEditable) {
                    parameterStore[getParameterStoreID(id)].value=value;
                    parameterStore[getParameterStoreID(id)].valueSet=true;
                    emit propertiesChanged();
                }
            }
        }

    }
}

void QFFCSFitEvaluation::setFitError(QFRawDataRecord* r, int run, const QString& id, double error) {
    if (r!=NULL) {
        QString dsid=getParameterStoreID(id);
        if (hasFit(r, run)) {
            setFitResultError(r, run, id, error);
        } else {
            QFFitFunction* f=getFitFunction();
            if (f) {
                QFFitFunction::ParameterDescription d=f->getDescription(id);
                if (d.userEditable) {
                    parameterStore[getParameterStoreID(id)].error=error;
                    parameterStore[getParameterStoreID(id)].errorSet=true;
                    emit propertiesChanged();
                }
            }
        }

    }
}

void QFFCSFitEvaluation::setFitResultValue(QFRawDataRecord* r, int run, const QString& id, double value, double error) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(id);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetNumberError(getEvaluationResultID(run), getFitParamID(id), value, error, unit);
        emit resultsChanged();
    }
}

void QFFCSFitEvaluation::setFitResultError(QFRawDataRecord* r, int run, const QString& id, double error) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(id);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetNumberError(getEvaluationResultID(run), getFitParamID(id), getFitValue(id), error, unit);
        emit resultsChanged();
    }
}

void QFFCSFitEvaluation::setFitResultValues(QFRawDataRecord* r, int run, double* values, double* errors) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        if (f) {
            for (int i=0; i<f->paramCount(); i++) {
                QString pid=f->getParameterID(i);
                //setFitResultValue(pid, values[i], errors[i]);
                QString unit=f->getDescription(pid).unit;
                r->resultsSetNumberError(getEvaluationResultID(run), getFitParamID(pid), values[i], errors[i], unit);
            }
            emit propertiesChanged();
            emit resultsChanged();
        }

    }
}

void QFFCSFitEvaluation::setFitResultValuesVisible(QFRawDataRecord* r, int run, double* values, double* errors) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        if (f) {
            for (int i=0; i<f->paramCount(); i++) {
                if (f->isParameterVisible(i, values)) {
                    QString pid=f->getParameterID(i);
                    //setFitResultValue(pid, values[i], errors[i]);
                    QString unit=f->getDescription(pid).unit;
                    r->resultsSetNumberError(getEvaluationResultID(run), getFitParamID(pid), values[i], errors[i], unit);
                }
            }
            emit propertiesChanged();
            emit resultsChanged();
        }

    }
}
double QFFCSFitEvaluation::getFitValue(QFRawDataRecord* r, int run, const QString& id) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) {
        return 0;
    }
    if (!f->hasParameter(id)) return 0;
    int pid=f->getParameterNum(id);
    double res=0;
    if (pid>-1) res=f->getDescription(pid).initialValue;
    res=fitParamSettings->value(QString(m_fitFunction+"/"+id), res).toDouble();
    QFRDRCountRatesInterface* crintf=qobject_cast<QFRDRCountRatesInterface*>(r);
    if (r && (id=="count_rate")) {
        res=crintf->getRateMean(run)*1000;
    }
    QString psID=getParameterStoreID(id);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].valueSet) {
            res=parameterStore[psID].value;
        }
    }
    if (hasFit(r, run)) {
        QString en=getEvaluationResultID(run);
        QString pid=getFitParamID(id);
        if (r->resultsExists(en, pid)) res=r->resultsGetAsDouble(en, pid);
    }
    return res;
}

double QFFCSFitEvaluation::getFitError(QFRawDataRecord* r, int run, const QString& id)  {
    if (hasFit(r, run)) {
        if (r!=NULL) {
            return r->resultsGetErrorAsDouble(getEvaluationResultID(run), getFitParamID(id));
        }
    }
    QFRDRCountRatesInterface* crintf=qobject_cast<QFRDRCountRatesInterface*>(r);
    QString psID=getParameterStoreID(id);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].errorSet) {
            return parameterStore[psID].error;
        }
    }
    if (r && (id=="count_rate")) {
        return crintf->getRateStdDev(run)*1000;
    }
    return 0.0;
}

void QFFCSFitEvaluation::setFitFix(QFRawDataRecord* r, int run, const QString& id, bool fix) {
    if (r!=NULL) {
        QString dsid=getParameterStoreID(id);
        if (hasFit(r, run)) {
            setFitResultFix(r, run, id, fix);
        } else {
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
}

void QFFCSFitEvaluation::setFitResultFix(QFRawDataRecord* r, int run, const QString& id, bool fix) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(id);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetBoolean(getEvaluationResultID(run), getFitParamFixID(id), fix);
        emit resultsChanged();
    }
}

bool QFFCSFitEvaluation::getFitFix(QFRawDataRecord* r, int run, const QString& id) {
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
    if (hasFit(r, run)) {
        QString en=getEvaluationResultID(run);
        QString pid=getFitParamFixID(id);
        if (r->resultsExists(en, pid)) res=r->resultsGetAsDouble(en, pid);
    }
    return res;
}

















void QFFCSFitEvaluation::setFitResultValue(const QString& id, double value) {
    setFitResultValue(getHighlightedRecord(), getCurrentRun(), id, value);
}


void QFFCSFitEvaluation::setFitResultValue(const QString& id, double value, QString unit) {
    setFitResultValue(getHighlightedRecord(), getCurrentRun(), id, value, unit);
}

void QFFCSFitEvaluation::setFitResultValueString(const QString& id, QString value) {
    setFitResultValueString(getHighlightedRecord(), getCurrentRun(), id, value);
}

void QFFCSFitEvaluation::setFitResultValueBool(const QString& id, bool value) {
    setFitResultValueBool(getHighlightedRecord(), getCurrentRun(), id, value);
}

void QFFCSFitEvaluation::setFitResultValueInt(const QString& id, int64_t value) {
    setFitResultValueInt(getHighlightedRecord(), getCurrentRun(), id, value);
}

void QFFCSFitEvaluation::setFitResultValueInt(const QString& id, int64_t value, QString unit) {
    setFitResultValueInt(getHighlightedRecord(), getCurrentRun(), id, value, unit);
}

void QFFCSFitEvaluation::setFitValue(const QString& id, double value) {
    setFitValue(getHighlightedRecord(), getCurrentRun(), id, value);
}

void QFFCSFitEvaluation::setFitError(const QString& id, double error) {
    setFitError(getHighlightedRecord(), getCurrentRun(), id, error);
}

void QFFCSFitEvaluation::setFitResultValue(const QString& id, double value, double error) {
    setFitResultValue(getHighlightedRecord(), getCurrentRun(), id, value, error);
}

void QFFCSFitEvaluation::setFitResultError(const QString& id, double error) {
    setFitResultError(getHighlightedRecord(), getCurrentRun(), id, error);
}

void QFFCSFitEvaluation::setFitResultValues(double* values, double* errors) {
    setFitResultValues(getHighlightedRecord(), getCurrentRun(), values, errors);
}

void QFFCSFitEvaluation::setFitResultValuesVisible(double* values, double* errors) {
    setFitResultValuesVisible(getHighlightedRecord(), getCurrentRun(), values, errors);
}

double QFFCSFitEvaluation::getFitValue(const QString& id) {
    return getFitValue(getHighlightedRecord(), getCurrentRun(), id);
}

double QFFCSFitEvaluation::getFitError(const QString& id)  {
    return getFitError(getHighlightedRecord(), getCurrentRun(), id);
}

void QFFCSFitEvaluation::setFitFix(const QString& id, bool fix) {
    setFitFix(getHighlightedRecord(), getCurrentRun(), id, fix);
}

void QFFCSFitEvaluation::setFitResultFix(const QString& id, bool fix) {
    setFitResultFix(getHighlightedRecord(), getCurrentRun(), id, fix);
}

bool QFFCSFitEvaluation::getFitFix(const QString& id) {
    return getFitFix(getHighlightedRecord(), getCurrentRun(), id);
}











void QFFCSFitEvaluation::setFitRange(const QString& id, double min, double max) {
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

void QFFCSFitEvaluation::setFitMin(const QString& id, double min) {
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

void QFFCSFitEvaluation::setFitMax(const QString& id, double max) {
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

double QFFCSFitEvaluation::getFitMin(const QString& id)  {
    QString key=QString(' ',1024);
    key=m_fitFunction;
    key+='/';
    key+=id;
    key+="_min"; //QString("%1/%2_min").arg(m_fitFunction).arg(id);
    QFFitFunction* f=getFitFunction();
    if (f==NULL) {
        //std::cout<<"getFitMin("<<id.toStdString()<<") = 0 [getFitFunction()==NULL]\n";
        return 0;
    }
    double res=f->getDescription(id).minValue;
    res=fitParamSettings->value(key, res).toDouble();
    QString psID=getParameterStoreID(id);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].minSet) {
            res=parameterStore[psID].min;
        }
    }
    //std::cout<<"getFitMin("<<id.toStdString()<<") = "<<res<<"\n";
    return res;
}

double QFFCSFitEvaluation::getFitMax(const QString& id) {
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

double* QFFCSFitEvaluation::allocFillParametersMin() {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        double* res=(double*)calloc(f->paramCount(), sizeof(double));
        fillParametersMin(res);
        return res;
    }
    return NULL;
}

double* QFFCSFitEvaluation::allocFillParametersMax() {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        double* res=(double*)calloc(f->paramCount(), sizeof(double));
        fillParametersMax(res);
        return res;
    }
    return NULL;
}

void QFFCSFitEvaluation::fillParametersMin(double* param) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitMin(id);
        }
    }
}

void QFFCSFitEvaluation::fillParametersMax(double* param) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitMax(id);
        }
    }
}















void QFFCSFitEvaluation::fillParameters(double* param) {
    fillParameters(getHighlightedRecord(), getCurrentRun(), param);
}

void QFFCSFitEvaluation::fillParameterErrors(double* param) {
    fillParameterErrors(getHighlightedRecord(), getCurrentRun(), param);
}

void QFFCSFitEvaluation::fillFix(bool* param) {
    fillFix(getHighlightedRecord(), getCurrentRun(), param);
}


double* QFFCSFitEvaluation::allocFillParameters() {
    return allocFillParameters(getHighlightedRecord(), getCurrentRun());

}

double* QFFCSFitEvaluation::allocFillParameterErrors() {
    return allocFillParameterErrors(getHighlightedRecord(), getCurrentRun());

}

bool* QFFCSFitEvaluation::allocFillFix() {
    return allocFillFix(getHighlightedRecord(), getCurrentRun());
}















void QFFCSFitEvaluation::fillParameters(QFRawDataRecord* r, int run, double* param) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitValue(r, run, id);
        }
    }
}

void QFFCSFitEvaluation::fillParameterErrors(QFRawDataRecord* r, int run, double* param) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitError(r, run, id);
        }
    }
}


void QFFCSFitEvaluation::fillFix(QFRawDataRecord* r, int run, bool* param) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitFix(r, run, id);
        }
    }
}


double* QFFCSFitEvaluation::allocFillParameters(QFRawDataRecord* r, int run) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        double* res=(double*)calloc(f->paramCount(), sizeof(double));
        fillParameters(r, run, res);
        return res;
    }
    return NULL;
}

double* QFFCSFitEvaluation::allocFillParameterErrors(QFRawDataRecord* r, int run) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        double* res=(double*)calloc(f->paramCount(), sizeof(double));
        fillParameterErrors(r, run, res);
        return res;
    }
    return NULL;
}




bool* QFFCSFitEvaluation::allocFillFix(QFRawDataRecord* r, int run) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        bool* res=(bool*)calloc(f->paramCount(), sizeof(bool));
        fillFix(r, run, res);
        return res;
    }
    return NULL;
}










void QFFCSFitEvaluation::setCurrentRun(int run) {
    QFRawDataRecord* r=getHighlightedRecord();
    QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(r);
    if ((r!=NULL)) {
        if (run<=-1) m_currentRun=-1;
        if (run<fcs->getCorrelationRuns()) m_currentRun=run;
        if (fcs->getCorrelationRuns()<=1) m_currentRun=-1;
        r->setQFProperty(QString(getType()+QString::number(getID())+"_last_run"), m_currentRun, false, false);
        //std::cout<<"m_currentRun = "<<m_currentRun<<std::endl;
    }
}

int QFFCSFitEvaluation::getCurrentRun() {
    QFRawDataRecord* r=getHighlightedRecord();
    QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(r);
    //std::cout<<"getCurrentRun()\n";
    //std::cout<<"   r="<<r<<"\n";
    int run=m_currentRun;
    if ((r!=NULL)&&(fcs!=NULL)) {
        m_currentRun=run=r->getProperty(QString(getType()+QString::number(getID())+"_last_run"), run).toInt();
        //std::cout<<"   m_currentRun="<<m_currentRun<<"   corrRuns="<<fcs->getCorrelationRuns()<<"\n";
    }
    if (run<-1) {
        m_currentRun=run=-1;
        //setCurrentRun(run);
    }
    if (fcs) {
        if (run>=(int)fcs->getCorrelationRuns()) {
            m_currentRun=run=-1;
            //setCurrentRun(run);
        }
        if (fcs->getCorrelationRuns()<=1) {
            m_currentRun=run=-1;
            //setCurrentRun(run);
        }
    }
    return run;
}


void QFFCSFitEvaluation::setFitFunction(QString fitFunction) {
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


QFFitFunction* QFFCSFitEvaluation::getFitFunction() {
    return getFitFunction(m_fitFunction);
}

/*! \brief return the default/initial/global value of a given parameter        */
double QFFCSFitEvaluation::getDefaultFitValue(const QString& id) {
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
bool QFFCSFitEvaluation::getDefaultFitFix(const QString& id) {
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
void QFFCSFitEvaluation::resetDefaultFitValue(const QString& id) {
    if (hasFit()) {
        QFRawDataRecord* r=getHighlightedRecord();
        QString en=getEvaluationResultID();
        QString pid=getFitParamID(id);
        if (r->resultsExists(en, pid)) r->resultsRemove(en, pid);
    }
}

/*! \brief reset the given parameter \a id to the initial/global/default fix */
void QFFCSFitEvaluation::resetDefaultFitFix(const QString& id) {
    if (hasFit()) {
        QFRawDataRecord* r=getHighlightedRecord();
        QString en=getEvaluationResultID();
        QString pid=getFitParamFixID(id);
        if (r->resultsExists(en, pid)) r->resultsRemove(en, pid);
    }
}

/*! \brief reset the all parameters to the initial/global/default value in current files */
void QFFCSFitEvaluation::resetAllFitValueCurrent() {
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
void QFFCSFitEvaluation::resetAllFitFixCurrent() {
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

/*! \brief reset the all fit results to the initial/global/default value in the current file, but all runs */
void QFFCSFitEvaluation::resetAllFitResultsCurrent() {
    QFRawDataRecord* re=getHighlightedRecord();
    if (!re) return;

    QFRDRFCSDataInterface* rec=qobject_cast<QFRDRFCSDataInterface*>(re);
    QString en=getEvaluationResultID(-1);
    re->resultsClear(en);
    for(int r=0; r<(int)rec->getCorrelationRuns(); r++) {
        en=getEvaluationResultID(r);
        re->resultsClear(en);
    }
}

/*! \brief reset the all fit results to the initial/global/default value in the current file and current run */
void QFFCSFitEvaluation::resetAllFitResultsCurrentCurrentRun() {
    QFRawDataRecord* re=getHighlightedRecord();
    if (!re) return;

    //QFRDRFCSDataInterface* rec=qobject_cast<QFRDRFCSDataInterface*>(re);
    QString en=getEvaluationResultID(-1);
    re->resultsClear(en);
    int r=getCurrentRun();
    en=getEvaluationResultID(r);
    re->resultsClear(en);
}


/*! \brief reset all fit results to the initial/global/default value in all files and all runs */
void QFFCSFitEvaluation::resetAllFitResults() {
    QList<QFRawDataRecord*> recs=getApplicableRecords();
    for (int i=0; i<recs.size(); i++) {
        QFRDRFCSDataInterface* rec=qobject_cast<QFRDRFCSDataInterface*>(recs[i]);
        QString en=getEvaluationResultID(-1);
        recs[i]->resultsClear(en);
        for(int r=0; r<(int)rec->getCorrelationRuns(); r++) {
            en=getEvaluationResultID(r);
            recs[i]->resultsClear(en);
        }
    }
}

/*! \brief reset all parameters to the initial/global/default value in all files and all runs */
void QFFCSFitEvaluation::resetAllFitValue()  {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return ;
    QList<QFRawDataRecord*> recs=getApplicableRecords();
    for (int i=0; i<recs.size(); i++) {
        QFRDRFCSDataInterface* rec=qobject_cast<QFRDRFCSDataInterface*>(recs[i]);
        QString en=getEvaluationResultID(-1);
        for (int j=0; j<f->paramCount(); j++) {
            QString id=f->getParameterID(j);
            QString pid=getFitParamID(id);
            if (recs[i]->resultsExists(en, pid)) recs[i]->resultsRemove(en, pid);
        }
        for(int r=0; r<(int)rec->getCorrelationRuns(); r++) {
            en=getEvaluationResultID(r);
            for (int j=0; j<f->paramCount(); j++) {
                QString id=f->getParameterID(j);
                QString pid=getFitParamID(id);
                if (recs[i]->resultsExists(en, pid)) recs[i]->resultsRemove(en, pid);
            }
        }
    }
}

/*! \brief reset all parameters to the initial/global/default fix in all files and all runs */
void QFFCSFitEvaluation::resetAllFitFix() {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return ;
    QList<QFRawDataRecord*> recs=getApplicableRecords();
    for (int i=0; i<recs.size(); i++) {
        QFRDRFCSDataInterface* rec=qobject_cast<QFRDRFCSDataInterface*>(recs[i]);
        QString en=getEvaluationResultID(-1);
        for (int j=0; j<f->paramCount(); j++) {
            QString id=f->getParameterID(j);
            QString pid=getFitParamFixID(id);
            if (recs[i]->resultsExists(en, pid)) recs[i]->resultsRemove(en, pid);
        }
        for(int r=0; r<(int)rec->getCorrelationRuns(); r++) {
            en=getEvaluationResultID(r);
            for (int j=0; j<f->paramCount(); j++) {
                QString id=f->getParameterID(j);
                QString pid=getFitParamFixID(id);
                if (recs[i]->resultsExists(en, pid)) recs[i]->resultsRemove(en, pid);
            }
        }
    }
}

/*! \brief set the given parameter \a id to the given value (and error) in all files, hasFit for the file is \c true */
void QFFCSFitEvaluation::setAllFitValues(const QString& id, double value, double error) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return ;
    QList<QFRawDataRecord*> recs=getApplicableRecords();
    QString en=getEvaluationResultID();
    int pid=f->getParameterNum(id);
    QString unit="";
    if (pid>-1) unit=f->getDescription(pid).unit;
    for (int i=0; i<recs.size(); i++) {
        QFRDRFCSDataInterface* rec=qobject_cast<QFRDRFCSDataInterface*>(recs[i]);
        QString en=getEvaluationResultID(-1);
        if (hasFit(recs[i], -1)) recs[i]->resultsSetNumberError(en, getFitParamID(id), value, error, unit);
        for(int r=0; r<(int)rec->getCorrelationRuns(); r++) {
            en=getEvaluationResultID(r);
            if (hasFit(recs[i], r)) recs[i]->resultsSetNumberError(en, getFitParamID(id), value, error, unit);
        }
    }
    emit resultsChanged();
}

/*! \brief set the given parameter \a id to the given fix value */
void QFFCSFitEvaluation::setAllFitFixes(const QString& id, bool fix) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return ;
    QList<QFRawDataRecord*> recs=getApplicableRecords();
    QString en=getEvaluationResultID();
    int pid=f->getParameterNum(id);
    QString unit="";
    if (pid>-1) unit=f->getDescription(pid).unit;
    for (int i=0; i<recs.size(); i++) {
        QFRDRFCSDataInterface* rec=qobject_cast<QFRDRFCSDataInterface*>(recs[i]);
        QString en=getEvaluationResultID(-1);
        if (hasFit(recs[i], -1)) recs[i]->resultsSetBoolean(en, getFitParamFixID(id), fix);
        for(int r=0; r<(int)rec->getCorrelationRuns(); r++) {
            en=getEvaluationResultID(r);
            if (hasFit(recs[i], r)) recs[i]->resultsSetBoolean(en, getFitParamFixID(id), fix);
        }
    }
    emit resultsChanged();
}

void QFFCSFitEvaluation::setInitFitValue(const QString& id, double value, double error) {
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

void QFFCSFitEvaluation::setInitFitFix(const QString& id, bool fix) {
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


void QFFCSFitEvaluation::storeQFFitAlgorithmParameters(QFFitAlgorithm* algorithm) {
    QString aid=algorithm->id();
    QStringList params=algorithm->getParameterIDs();
    for (int i=0; i<params.size(); i++) {
        QString pid=params[i];
        algorithm_parameterstore[aid].insert(pid, algorithm->getParameter(pid));
    }
    emit propertiesChanged();
}

void QFFCSFitEvaluation::restoreQFFitAlgorithmParameters(QFFitAlgorithm* algorithm) {
    QString aid=algorithm->id();
    QStringList params=algorithm->getParameterIDs();
    for (int i=0; i<params.size(); i++) {
        QString pid=params[i];
        algorithm->setParameter(pid, algorithm_parameterstore[aid].value(pid, algorithm->getParameter(pid)));
    }
}

