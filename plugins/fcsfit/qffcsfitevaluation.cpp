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

    // get list of applicable fit functions
    m_fitFunctions=parent->getServices()->getFitFunctionManager()->getModels("fcs_", this);
    if (m_fitFunctions.size()>0) m_fitFunction=m_fitFunctions.keys().at(0);

    // get list of available fit algorithms
    QStringList fita=parent->getServices()->getFitAlgorithmManager()->getIDList();
    for (int i=0; i<fita.size(); i++) {
        m_fitAlgorithms[fita[i]]=parent->getServices()->getFitAlgorithmManager()->createAlgorithm(fita[i], this);
    }

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


void QFFCSFitEvaluation::intWriteXML(QXmlStreamWriter& w) {
    w.writeStartElement("algorithms");
    w.writeAttribute("current", m_fitAlgorithm);
    w.writeEndElement();
    w.writeStartElement("functions");
    w.writeAttribute("current", m_fitFunction);
    QMapIterator<QString, FitParameter> i(parameterStore);
    while (i.hasNext()) {
        i.next();
        w.writeStartElement("parameter");
        w.writeAttribute("id", i.key());
        w.writeAttribute("value", QString::number(i.value().value, 'g', 12));
        w.writeAttribute("min", QString::number(i.value().min, 'g', 12));
        w.writeAttribute("max", QString::number(i.value().max, 'g', 12));
        w.writeAttribute("fix", QString((i.value().fix)?QString("1"):QString("0")));
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
        if (elt.hasAttribute("id") && elt.hasAttribute("value") && elt.hasAttribute("fix") && elt.hasAttribute("min") && elt.hasAttribute("max")) {
            QString a=elt.attribute("id");
            FitParameter p;
            p.fix=(elt.attribute("fix").toLower()!="0");
            p.value=elt.attribute("value").toDouble();
            p.min=elt.attribute("min").toDouble();
            p.max=elt.attribute("max").toDouble();
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

bool QFFCSFitEvaluation::hasFit() {
    if (getFitFunction()==NULL) return false;
    QFRawDataRecord* r=getHighlightedRecord();
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

void QFFCSFitEvaluation::setFitValue(QString id, double value) {
    if (getHighlightedRecord()!=NULL) {
        QString dsid=getParameterStoreID(id);
        if (hasFit()) {
            setFitResultValue(id, value);
        } else {
            parameterStore[getParameterStoreID(id)].value=value;
            emit propertiesChanged();
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

double QFFCSFitEvaluation::getFitValue(QString id) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return 0;
    int pid=f->getParameterNum(id);
    double res=0;
    if (pid>-1) res=f->getDescription(pid).initialValue;
    res=fitParamSettings->value(m_fitFunction+"/"+id, res).toDouble();
    QString psID=getParameterStoreID(id);
    if (parameterStore.contains(psID)) res=parameterStore[psID].value;
    if (hasFit()) {
        QFRawDataRecord* r=getHighlightedRecord();
        QString en=getEvaluationResultID();
        if (r->resultsExists(en, id)) res=r->resultsGetAsDouble(en, id);
    }
    return res;
}

double QFFCSFitEvaluation::getFitError(QString id)  {
    if (!hasFit()) return 0;
    QFRawDataRecord* r=getHighlightedRecord();
    if (r!=NULL) {
        return r->resultsGetErrorAsDouble(getEvaluationResultID(), id);
    }
    return 0.0;
}

void QFFCSFitEvaluation::setFitFix(QString id, bool fix) {
    if (getHighlightedRecord()!=NULL) {
        QString dsid=getParameterStoreID(id);
        if (hasFit()) {
            setFitResultFix(id, fix);
        } else {
            parameterStore[getParameterStoreID(id)].fix=fix;
            emit propertiesChanged();
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
    if (parameterStore.contains(psID)) res=parameterStore[psID].fix;
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
        parameterStore[getParameterStoreID(id)].min=min;
        parameterStore[getParameterStoreID(id)].max=max;
        emit propertiesChanged();
    }
}

void QFFCSFitEvaluation::setFitMin(QString id, double min) {
    if (getHighlightedRecord()!=NULL) {
        QString dsid=getParameterStoreID(id);
        parameterStore[getParameterStoreID(id)].min=min;
        emit propertiesChanged();
    }
}

void QFFCSFitEvaluation::setFitMax(QString id, double max) {
    if (getHighlightedRecord()!=NULL) {
        QString dsid=getParameterStoreID(id);
        parameterStore[getParameterStoreID(id)].max=max;
        emit propertiesChanged();
    }
}

double QFFCSFitEvaluation::getFitMin(QString id)  {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return 0;
    double res=0;
    res=fitParamSettings->value(m_fitFunction+"/"+id+"_min", res).toDouble();
    QString psID=getParameterStoreID(id);
    if (parameterStore.contains(psID)) res=parameterStore[psID].min;
    return res;
}

double QFFCSFitEvaluation::getFitMax(QString id) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return 0;
    double res=0;
    res=fitParamSettings->value(m_fitFunction+"/"+id+"_max", res).toDouble();
    QString psID=getParameterStoreID(id);
    if (parameterStore.contains(psID)) res=parameterStore[psID].max;
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

void QFFCSFitEvaluation::setCurrentRun(int run) {
    QFRawDataRecord* r=getHighlightedRecord();
    QFRDRFCSDataInterface* fcs=dynamic_cast<QFRDRFCSDataInterface*>(r);
    if ((r!=NULL)) {
        if (run<-1) m_currentRun=-1;
        if (run<fcs->getCorrelationRuns()) m_currentRun=run;
        r->setQFProperty(getType()+QString::number(getID())+"_last_run", m_currentRun);
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
    m_fitFunction=fitFunction;
    emit propertiesChanged();
}


QFFitFunction* QFFCSFitEvaluation::getFitFunction() {
    return getFitFunction(m_fitFunction);
}
