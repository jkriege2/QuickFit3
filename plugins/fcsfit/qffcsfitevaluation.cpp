#include "qffcsfitevaluation.h"
#include "qffcsfitevaluationeditor.h"
#include "../fcs/qfrdrfcsdatainterface.h"



QFFCSFitEvaluation::QFFCSFitEvaluation(QFProject* parent):
    QFEvaluationItem(parent, true, false)
{
    QFileInfo inif(parent->getServices()->getOptions()->getIniFilename());
    QString inifn=inif.absolutePath()+"/fitparams.ini";
    fitParamSettings=new QSettings(inifn, QSettings::IniFormat);

    m_fitAlgorithm="";
    m_fitFunction="";
    m_weighting=EqualWeighting;


    // get list of applicable fit functions
    m_fitFunctions=parent->getServices()->getFitFunctionManager()->getModels("fcs_", this);
    if (m_fitFunctions.size()>0) m_fitFunction=m_fitFunctions.keys().at(0);

    // get list of available fit algorithms
    QStringList fita=parent->getServices()->getFitAlgorithmManager()->getIDList();
    for (int i=0; i<fita.size(); i++) {
        m_fitAlgorithms[fita[i]]=parent->getServices()->getFitAlgorithmManager()->createAlgorithm(fita[i], this);
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

bool QFFCSFitEvaluation::hasFit(QFRawDataRecord* r) {
    if (getFitFunction()==NULL) return false;
    if (r==NULL) r=getHighlightedRecord();
    if (r==NULL) return false;
    QString rsid=getEvaluationResultID();
    return r->resultsExistsFromEvaluation(rsid);
}

void QFFCSFitEvaluation::setFitResultValue(QString id, double value) {
    QFRawDataRecord* r=getHighlightedRecord();
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(id);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetNumber(getEvaluationResultID(), id, value, unit);
    }
}

void QFFCSFitEvaluation::setFitResultValue(QString id, double value, QString unit) {
    QFRawDataRecord* r=getHighlightedRecord();
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        r->resultsSetNumber(getEvaluationResultID(), id, value, unit);
    }
}

void QFFCSFitEvaluation::setFitResultValueString(QString id, QString value) {
    QFRawDataRecord* r=getHighlightedRecord();
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(id);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetString(getEvaluationResultID(), id, value);
    }
}

void QFFCSFitEvaluation::setFitResultValueBool(QString id, bool value) {
    QFRawDataRecord* r=getHighlightedRecord();
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(id);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetBoolean(getEvaluationResultID(), id, value);
    }
}

void QFFCSFitEvaluation::setFitResultValueInt(QString id, int64_t value) {
    QFRawDataRecord* r=getHighlightedRecord();
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(id);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetInteger(getEvaluationResultID(), id, value, unit);
    }
}

void QFFCSFitEvaluation::setFitResultValueInt(QString id, int64_t value, QString unit) {
    QFRawDataRecord* r=getHighlightedRecord();
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        r->resultsSetInteger(getEvaluationResultID(), id, value, unit);
    }
}

void QFFCSFitEvaluation::setFitValue(QString id, double value) {
    //std::cout<<"setFitValue("<<id.toStdString()<<", "<<value<<")\n";
    if (getHighlightedRecord()!=NULL) {
        QString dsid=getParameterStoreID(id);
        if (hasFit()) {
            setFitResultValue(id, value, getFitError(id));
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

void QFFCSFitEvaluation::setFitError(QString id, double error) {
    //std::cout<<"setFitValue("<<id.toStdString()<<", "<<value<<")\n";
    if (getHighlightedRecord()!=NULL) {
        QString dsid=getParameterStoreID(id);
        if (hasFit()) {
            setFitResultError(id, error);
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

void QFFCSFitEvaluation::setFitResultValue(QString id, double value, double error) {
    QFRawDataRecord* r=getHighlightedRecord();
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(id);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetNumberError(getEvaluationResultID(), id, value, error, unit);
    }
}

void QFFCSFitEvaluation::setFitResultError(QString id, double error) {
    QFRawDataRecord* r=getHighlightedRecord();
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(id);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetNumberError(getEvaluationResultID(), id, getFitValue(id), error, unit);
    }
}

void QFFCSFitEvaluation::setFitResultValues(double* values, double* errors) {
    QFRawDataRecord* r=getHighlightedRecord();
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        if (f) {
            for (int i=0; i<f->paramCount(); i++) {
                QString pid=f->getParameterID(i);
                //setFitResultValue(pid, values[i], errors[i]);
                QString unit=f->getDescription(pid).unit;
                r->resultsSetNumberError(getEvaluationResultID(), pid, values[i], errors[i], unit);
            }
            emit propertiesChanged();
        }

    }
}

void QFFCSFitEvaluation::setFitResultValuesVisible(double* values, double* errors) {
    QFRawDataRecord* r=getHighlightedRecord();
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        if (f) {
            for (int i=0; i<f->paramCount(); i++) {
                if (f->isParameterVisible(i, values)) {
                    QString pid=f->getParameterID(i);
                    //setFitResultValue(pid, values[i], errors[i]);
                    QString unit=f->getDescription(pid).unit;
                    r->resultsSetNumberError(getEvaluationResultID(), pid, values[i], errors[i], unit);
                }
            }
            emit propertiesChanged();
        }

    }
}
double QFFCSFitEvaluation::getFitValue(QString id) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) {
        //std::cout<<"getFitValue("<<id.toStdString()<<") = "<<0<<" [getFitFunction()==NULL]\n";
        return 0;
    }
    if (!f->hasParameter(id)) return 0;
    int pid=f->getParameterNum(id);
    double res=0;
    if (pid>-1) res=f->getDescription(pid).initialValue;
    res=fitParamSettings->value(m_fitFunction+"/"+id, res).toDouble();
    QString psID=getParameterStoreID(id);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].valueSet) {
            res=parameterStore[psID].value;
        }
    }
    if (hasFit()) {
        QFRawDataRecord* r=getHighlightedRecord();
        QString en=getEvaluationResultID();
        if (r->resultsExists(en, id)) res=r->resultsGetAsDouble(en, id);
    }
    //std::cout<<"getFitValue("<<id.toStdString()<<") = "<<res<<"\n";
    return res;
}

double QFFCSFitEvaluation::getFitError(QString id)  {
    if (hasFit()) {
        QFRawDataRecord* r=getHighlightedRecord();
        if (r!=NULL) {
            return r->resultsGetErrorAsDouble(getEvaluationResultID(), id);
        }
    }
    QString psID=getParameterStoreID(id);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].errorSet) {
            return parameterStore[psID].error;
        }
    }
    return 0.0;
}

void QFFCSFitEvaluation::setFitFix(QString id, bool fix) {
    if (getHighlightedRecord()!=NULL) {
        QString dsid=getParameterStoreID(id);
        if (hasFit()) {
            setFitResultFix(id, fix);
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

void QFFCSFitEvaluation::setFitResultFix(QString id, bool fix) {
    QFRawDataRecord* r=getHighlightedRecord();
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(id);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetBoolean(getEvaluationResultID(), id+"_fix", fix);
    }
}

bool QFFCSFitEvaluation::getFitFix(QString id) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return 0;
    bool res=false;
    res=fitParamSettings->value(m_fitFunction+"/"+id+"_fix", res).toBool();
    QString psID=getParameterStoreID(id);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].fixSet) {
            res=parameterStore[psID].fix;
        }
    }
    if (hasFit()) {
        QFRawDataRecord* r=getHighlightedRecord();
        QString en=getEvaluationResultID();
        if (r->resultsExists(en, id+"_fix")) res=r->resultsGetAsDouble(en, id+"_fix");
    }
    return res;
}

void QFFCSFitEvaluation::setFitRange(QString id, double min, double max) {
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

void QFFCSFitEvaluation::setFitMin(QString id, double min) {
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

void QFFCSFitEvaluation::setFitMax(QString id, double max) {
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

double QFFCSFitEvaluation::getFitMin(QString id)  {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) {
        //std::cout<<"getFitMin("<<id.toStdString()<<") = 0 [getFitFunction()==NULL]\n";
        return 0;
    }
    double res=f->getDescription(id).minValue;
    res=fitParamSettings->value(m_fitFunction+"/"+id+"_min", res).toDouble();
    QString psID=getParameterStoreID(id);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].minSet) {
            res=parameterStore[psID].min;
        }
    }
    //std::cout<<"getFitMin("<<id.toStdString()<<") = "<<res<<"\n";
    return res;
}

double QFFCSFitEvaluation::getFitMax(QString id) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) {
        //std::cout<<"getFitMax("<<id.toStdString()<<") = 0 [getFitFunction()==NULL]\n";
        return 0;
    }
    double res=f->getDescription(id).maxValue;
    res=fitParamSettings->value(m_fitFunction+"/"+id+"_max", res).toDouble();
    QString psID=getParameterStoreID(id);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].maxSet) {
            res=parameterStore[psID].max;
        }
    }
    //std::cout<<"getFitMax("<<id.toStdString()<<") = "<<res<<"\n";
    return res;
}

void QFFCSFitEvaluation::fillParameters(double* param) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitValue(id);
        }
    }
}

void QFFCSFitEvaluation::fillParameterErrors(double* param) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitError(id);
        }
    }
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

void QFFCSFitEvaluation::fillFix(bool* param) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitFix(id);
        }
    }
}


double* QFFCSFitEvaluation::allocFillParameters() {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        double* res=(double*)calloc(f->paramCount(), sizeof(double));
        fillParameters(res);
        return res;
    }
    return NULL;
}

double* QFFCSFitEvaluation::allocFillParameterErrors() {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        double* res=(double*)calloc(f->paramCount(), sizeof(double));
        fillParameterErrors(res);
        return res;
    }
    return NULL;
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

bool* QFFCSFitEvaluation::allocFillFix() {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        bool* res=(bool*)calloc(f->paramCount(), sizeof(bool));
        fillFix(res);
        return res;
    }
    return NULL;
}


void QFFCSFitEvaluation::setCurrentRun(int run) {
    QFRawDataRecord* r=getHighlightedRecord();
    QFRDRFCSDataInterface* fcs=dynamic_cast<QFRDRFCSDataInterface*>(r);
    if ((r!=NULL)) {
        if (run<=-1) m_currentRun=-1;
        if (run<fcs->getCorrelationRuns()) m_currentRun=run;
        r->setQFProperty(getType()+QString::number(getID())+"_last_run", m_currentRun);
        std::cout<<"m_currentRun = "<<m_currentRun<<std::endl;
    }
}

int QFFCSFitEvaluation::getCurrentRun() {
    QFRawDataRecord* r=getHighlightedRecord();
    QFRDRFCSDataInterface* fcs=dynamic_cast<QFRDRFCSDataInterface*>(r);
    int run=m_currentRun;
    if ((r!=NULL)&&(fcs!=NULL)) {
        run=r->getProperty(getType()+QString::number(getID())+"_last_run", run).toInt();
    }
    if (run<-1) run=-1;
    if (run>=fcs->getCorrelationRuns()) run=-1;
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
double QFFCSFitEvaluation::getDefaultFitValue(QString id) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) {
        //std::cout<<"getFitValue("<<id.toStdString()<<") = "<<0<<" [getFitFunction()==NULL]\n";
        return 0;
    }
    if (!f->hasParameter(id)) return 0;
    int pid=f->getParameterNum(id);
    double res=0;
    if (pid>-1) res=f->getDescription(pid).initialValue;
    res=fitParamSettings->value(m_fitFunction+"/"+id, res).toDouble();
    QString psID=getParameterStoreID(id);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].valueSet) {
            res=parameterStore[psID].value;
        }
    }
    return res;
}

/*! \brief return the default/initial/global fix of a given parameter        */
bool QFFCSFitEvaluation::getDefaultFitFix(QString id) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return 0;
    bool res=false;
    res=fitParamSettings->value(m_fitFunction+"/"+id+"_fix", res).toBool();
    QString psID=getParameterStoreID(id);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].fixSet) {
            res=parameterStore[psID].fix;
        }
    }
    return res;
}

/*! \brief reset the given parameter \a id to the initial/global/default value */
void QFFCSFitEvaluation::resetDefaultFitValue(QString id) {
    if (hasFit()) {
        QFRawDataRecord* r=getHighlightedRecord();
        QString en=getEvaluationResultID();
        if (r->resultsExists(en, id)) r->resultsRemove(en, id);
    }
}

/*! \brief reset the given parameter \a id to the initial/global/default fix */
void QFFCSFitEvaluation::resetDefaultFitFix(QString id) {
    if (hasFit()) {
        QFRawDataRecord* r=getHighlightedRecord();
        QString en=getEvaluationResultID();
        if (r->resultsExists(en, id+"_fix")) r->resultsRemove(en, id+"_fix");
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
            if (r->resultsExists(en, id)) r->resultsRemove(en, id);
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
            if (r->resultsExists(en, id+"_fix")) r->resultsRemove(en, id+"_fix");
        }
    }
}

/*! \brief reset the all fit results to the initial/global/default value in all files */
void QFFCSFitEvaluation::resetAllFitResultsCurrent() {
    QFRawDataRecord* r=getHighlightedRecord();
    if (!r) return;
    QString en=getEvaluationResultID();
    r->resultsClear(en);
}


/*! \brief reset all fit results to the initial/global/default value in all files */
void QFFCSFitEvaluation::resetAllFitResults() {
    QList<QFRawDataRecord*> recs=getApplicableRecords();
    QString en=getEvaluationResultID();
    for (int i=0; i<recs.size(); i++) {
        recs[i]->resultsClear(en);
    }
}

/*! \brief reset all parameters to the initial/global/default value in all files */
void QFFCSFitEvaluation::resetAllFitValue()  {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return ;
    QList<QFRawDataRecord*> recs=getApplicableRecords();
    QString en=getEvaluationResultID();
    for (int i=0; i<recs.size(); i++) {
        for (int j=0; j<f->paramCount(); j++) {
            QString id=f->getParameterID(j);
            if (recs[i]->resultsExists(en, id)) recs[i]->resultsRemove(en, id);
        }
    }
}

/*! \brief reset all parameters to the initial/global/default fix in all files */
void QFFCSFitEvaluation::resetAllFitFix() {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return ;
    QList<QFRawDataRecord*> recs=getApplicableRecords();
    QString en=getEvaluationResultID();
    for (int i=0; i<recs.size(); i++) {
        for (int j=0; j<f->paramCount(); j++) {
            QString id=f->getParameterID(j)+"_fix";
            if (recs[i]->resultsExists(en, id)) recs[i]->resultsRemove(en, id);
        }
    }
}

/*! \brief set the given parameter \a id to the given value (and error) in all files, hasFit for the file is \c true */
void QFFCSFitEvaluation::setAllFitValues(QString id, double value, double error) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return ;
    QList<QFRawDataRecord*> recs=getApplicableRecords();
    QString en=getEvaluationResultID();
    int pid=f->getParameterNum(id);
    QString unit="";
    if (pid>-1) unit=f->getDescription(pid).unit;
    for (int i=0; i<recs.size(); i++) {
        if (hasFit(recs[i])) recs[i]->resultsSetNumberError(getEvaluationResultID(), id, value, error, unit);
    }
}

/*! \brief set the given parameter \a id to the given fix value */
void QFFCSFitEvaluation::setAllFitFixes(QString id, bool fix) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return ;
    QList<QFRawDataRecord*> recs=getApplicableRecords();
    QString en=getEvaluationResultID();
    int pid=f->getParameterNum(id);
    QString unit="";
    if (pid>-1) unit=f->getDescription(pid).unit;
    for (int i=0; i<recs.size(); i++) {
        if (hasFit(recs[i])) recs[i]->resultsSetBoolean(getEvaluationResultID(), id+"_fix", fix);
    }
}

void QFFCSFitEvaluation::setInitFitValue(QString id, double value, double error) {
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

void QFFCSFitEvaluation::setInitFitFix(QString id, bool fix) {
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
