#include "qffcsfitevaluation.h"
#include "qffcsfitevaluationeditor.h"
#include "../interfaces/qfrdrfcsdatainterface.h"



QFFCSFitEvaluation::QFFCSFitEvaluation(QFProject* parent):
    QFFitResultsEvaluation("fcs_", parent)
    //QFEvaluationItem(parent, true, false)
{
    QFileInfo inif(parent->getServices()->getOptions()->getIniFilename());
    QString inifn=inif.absolutePath()+"/fitparams.ini";
    fitParamSettings=new QSettings(inifn, QSettings::IniFormat);

    m_weighting=EqualWeighting;
    m_currentRun=-1;

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

bool QFFCSFitEvaluation::hasFit(QFRawDataRecord* r1, int run) {
    QFRawDataRecord* r=r1;
    if (getFitFunction()==NULL) return false;
    if (r==NULL) r=getHighlightedRecord();
    if (r==NULL) return false;
    QString rsid=getEvaluationResultID(run);
    return r->resultsExistsFromEvaluation(rsid);
}




void QFFCSFitEvaluation::setFitResultValue(QFRawDataRecord* r, int run, const QString& id, double value) {
    setFitResultValue(r, getEvaluationResultID(run), id, value);
}
void QFFCSFitEvaluation::setFitResultValue(QFRawDataRecord* r, int run, const QString& id, double value, QString unit) {
    setFitResultValue(r, getEvaluationResultID(run), id, value, unit);
}

void QFFCSFitEvaluation::setFitResultValueString(QFRawDataRecord* r, int run, const QString& id, QString value) {
    setFitResultValueString(r, getEvaluationResultID(run), id, value);
}

void QFFCSFitEvaluation::setFitResultValueBool(QFRawDataRecord* r, int run, const QString& id, bool value) {
    setFitResultValueBool(r, getEvaluationResultID(run), id, value);
}

void QFFCSFitEvaluation::setFitResultValueInt(QFRawDataRecord* r, int run, const QString& id, int64_t value) {
    setFitResultValueInt(r, getEvaluationResultID(run), id, value);
}

void QFFCSFitEvaluation::setFitResultValueInt(QFRawDataRecord* r, int run, const QString& id, int64_t value, QString unit) {
    setFitResultValueInt(r, getEvaluationResultID(run), id, value, unit);
}

void QFFCSFitEvaluation::setFitValue(QFRawDataRecord* r, int run, const QString& id, double value) {
    setFitValue(r, getEvaluationResultID(run), id, value);
}

void QFFCSFitEvaluation::setFitError(QFRawDataRecord* r, int run, const QString& id, double error) {
    setFitError(r, getEvaluationResultID(run), id, error);
}

void QFFCSFitEvaluation::setFitResultValue(QFRawDataRecord* r, int run, const QString& id, double value, double error) {
    setFitResultValue(r, getEvaluationResultID(run), id, value, error);
}

void QFFCSFitEvaluation::setFitResultError(QFRawDataRecord* r, int run, const QString& id, double error) {
    setFitResultError(r, getEvaluationResultID(run), id, error);
}

void QFFCSFitEvaluation::setFitResultValues(QFRawDataRecord* r, int run, double* values, double* errors) {
    setFitResultValues(r, getEvaluationResultID(run), values, errors);
}

void QFFCSFitEvaluation::setFitResultValuesVisible(QFRawDataRecord* r, int run, double* values, double* errors) {
    setFitResultValuesVisible(r, getEvaluationResultID(run),  values, errors);
}
double QFFCSFitEvaluation::getFitValue(QFRawDataRecord* r, int run, const QString& id) {
    /*QMap<QString, double> specials;
    QFRDRCountRatesInterface* crintf=qobject_cast<QFRDRCountRatesInterface*>(r);
    if (crintf && (id=="count_rate")) {
        specials["count_rate"]=crintf->getRateMean(run)*1000;
    }*/
    return getFitValue(r, getEvaluationResultID(run), id/*, specials*/);
}

double QFFCSFitEvaluation::getFitError(QFRawDataRecord* r, int run, const QString& id)  {
    /*QMap<QString, double> specials;
    QFRDRCountRatesInterface* crintf=qobject_cast<QFRDRCountRatesInterface*>(r);
    if (crintf && (id=="count_rate")) {
        specials["count_rate"]=crintf->getRateStdDev(run)*1000;
    }*/
    return getFitError(r, getEvaluationResultID(run), id/*, specials*/);
}

bool QFFCSFitEvaluation::hasSpecial(QFRawDataRecord* r, const QString& id, const QString& paramid, double& value, double& error) {
    int run=getRunFromEvaluationResultID(paramid);
    QFRDRCountRatesInterface* crintf=qobject_cast<QFRDRCountRatesInterface*>(r);
    if (crintf && (id=="count_rate")) {
        error=crintf->getRateStdDev(run)*1000;
        value=crintf->getRateMean(run)*1000;
        return true;
    }
    return false;
}

int QFFCSFitEvaluation::getRunFromEvaluationResultID(const QString& resultID) {
    if (resultID.endsWith("avg")) return -1;
    int l=0;
    while (resultID[resultID.size()-1-l].isDigit()) {
        l++;
    }
    if (l>0) return resultID.right(l).toInt();
    return -1;
}

double QFFCSFitEvaluation::getFitValue(const QString& id) {
    return getFitValue(getHighlightedRecord(), getEvaluationResultID(), id);
}

double QFFCSFitEvaluation::getFitError(const QString& id)  {
    return getFitError(getHighlightedRecord(), getEvaluationResultID(), id);
}

void QFFCSFitEvaluation::setFitFix(QFRawDataRecord* r, int run, const QString& id, bool fix) {
    setFitFix(r, getEvaluationResultID(run), id, fix);
}

void QFFCSFitEvaluation::setFitResultFix(QFRawDataRecord* r, int run, const QString& id, bool fix) {
    setFitResultFix(r, getEvaluationResultID(run), id, fix);
}

bool QFFCSFitEvaluation::getFitFix(QFRawDataRecord* r, int run, const QString& id) {
    return getFitFix(r, getEvaluationResultID(run), id);
}

void QFFCSFitEvaluation::fillParameters(QFRawDataRecord* r, int run, double* param) {
    fillParameters(r, getEvaluationResultID(run), param);
}

void QFFCSFitEvaluation::fillParameterErrors(QFRawDataRecord* r, int run, double* param) {
    fillParameterErrors(r, getEvaluationResultID(run), param);
}


void QFFCSFitEvaluation::fillFix(QFRawDataRecord* r, int run, bool* param) {
    fillFix(r, getEvaluationResultID(run), param);
}


double* QFFCSFitEvaluation::allocFillParameters(QFRawDataRecord* r, int run) {
    return allocFillParameters(r, getEvaluationResultID(run));
}

double* QFFCSFitEvaluation::allocFillParameterErrors(QFRawDataRecord* r, int run) {
    return allocFillParameterErrors(r, getEvaluationResultID(run));
}

bool* QFFCSFitEvaluation::allocFillFix(QFRawDataRecord* r, int run) {
    return allocFillFix(r, getEvaluationResultID(run));
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


/*! \brief reset the all fit results to the initial/global/default value in the current file, but all runs */
void QFFCSFitEvaluation::resetAllFitResultsCurrentFileAllRuns() {
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




/*! \brief reset all fit results to the initial/global/default value in all files and all runs */
void QFFCSFitEvaluation::resetAllFitResultsAllFilesAllRuns() {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return ;
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


