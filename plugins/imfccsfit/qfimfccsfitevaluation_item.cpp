#include "qfimfccsfitevaluation_item.h"
#include "qfimfccsfitevaluation_editor.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include "qffitfunction.h"
#include "qftools.h"
#include "qffcstools.h"
#include "qfprojectrawdatamodel.h"


QFImFCCSFitEvaluationItem::QFImFCCSFitEvaluationItem(QFProject* parent):
    QFFitResultsByIndexAsVectorEvaluation("fcs_,dls_,fccs_", parent, false, false)
{
    matchFunctor=new QFImFCCSMatchRDRFunctor();
    m_weighting=EqualWeighting;
    m_currentIndex=-1;
    m_multiFitFunctions.clear();
    //mutexThreadedFit=new QMutex(QMutex::Recursive);

    if (m_fitFunctions.contains("fcs_spim_diffc")) {
        m_fitFunction="fcs_spim_diff";
    }/* else if (m_fitFunctions.contains("fcs_diff1")) {
        m_fitFunction="fcs_diff1";
    }*/

    if (m_fitAlgorithms.contains("fit_lmfit")) {
        m_fitAlgorithm="fit_lmfit";
    }

    m_multiFitFunctions[0]=m_fitFunction;
    QList<QPointer<QFRawDataRecord> > recs=getApplicableRecords();
    if (recs.size()>0) fitFilesList.append(recs[0]);

    rdrProxy=new QFEvaluationRawDataModelProxy(this);
    rdrProxy->setSourceModel(project->getRawDataModel());
    rdrProxy->setEvaluation(this);

    paramTable=new QFImFCCSParameterInputTable(this);
    connect(this, SIGNAL(parameterStructureChanged()), paramTable, SLOT(rebuildModel()));
    ensureFitFiles();
}

QFImFCCSFitEvaluationItem::~QFImFCCSFitEvaluationItem() {
    delete matchFunctor;
}



QFEvaluationEditor* QFImFCCSFitEvaluationItem::createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent) {
    return new QFImFCCSFitEvaluationEditor(services, propEditor, parent);
}

int QFImFCCSFitEvaluationItem::getIndexMin(QFRawDataRecord *r) const
{
    return -1;
}

int QFImFCCSFitEvaluationItem::getIndexMax(QFRawDataRecord *r) const
{
    if (!r) return -1;
    QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(r);
    if (fcs->getCorrelationRuns()<=0) return -1;
    else return fcs->getCorrelationRuns()-1;
}

void QFImFCCSFitEvaluationItem::doFit(QFRawDataRecord *record, int run, int defaultMinDatarange, int defaultMaxDatarange, QFFitAlgorithmReporter *dlgFitProgress, bool doLog)
{
}

void QFImFCCSFitEvaluationItem::doFitForMultithread(QFRawDataRecord *record, int run, int defaultMinDatarange, int defaultMaxDatarange, QFPluginLogService *logservice) const
{
}

QFFitStatistics QFImFCCSFitEvaluationItem::calcFitStatistics(bool storeAsResults, QFFitFunction *ffunc, long N, double *tauvals, double *corrdata, double *weights, int datacut_min, int datacut_max, double *fullParams, double *errors, bool *paramsFix, int runAvgWidth, int residualHistogramBins, QFRawDataRecord *record, int run)
{
    return QFFitResultsByIndexEvaluationFitTools::calcFitStatistics(storeAsResults, ffunc,  N, tauvals, corrdata, weights,  datacut_min,  datacut_max, fullParams, errors, paramsFix,  runAvgWidth,  residualHistogramBins, record,  run);
}

QFEvaluationRawDataModelProxy *QFImFCCSFitEvaluationItem::getRawDataProxyModel() const
{
    return rdrProxy;
}

QFRawDataRecord *QFImFCCSFitEvaluationItem::getFitFile(int num) const
{
    return fitFilesList.value(num, NULL);
}

QString QFImFCCSFitEvaluationItem::getFitFunctionID(int num) const
{
    return m_multiFitFunctions.value(num, m_fitFunction);
}

QFFitFunction* QFImFCCSFitEvaluationItem::getFitFunction(int num) const
{
    return getFitFunction(getFitFunctionID(num));
}

QFFitFunction *QFImFCCSFitEvaluationItem::getFitFunctionForID(QString id) const
{
    return getFitFunction(id);
}

void QFImFCCSFitEvaluationItem::setFitFunction(int num, QString fitFunction)
{
    m_multiFitFunctions[num]=fitFunction;
    emit parameterStructureChanged();
    setDataChanged();
}

void QFImFCCSFitEvaluationItem::setFitFunction(QString fitFunction)
{
    setFitFunction(0,fitFunction);
}

int QFImFCCSFitEvaluationItem::addFitFile()
{
    QList<QPointer<QFRawDataRecord> > recs=getApplicableRecords();
    if (fitFilesList.size()<=0)  {
        if (recs.size()>0) fitFilesList.append(recs[0]);
    } else {
        QFRawDataRecord* next=fitFilesList.last()->getNextOfSameType();
        if (!next) next=fitFilesList[0];
        if (next) fitFilesList.append(next);
    }
    setDataChanged();
    paramTable->rebuildModel();
    return fitFilesList.size()-1;
}

void QFImFCCSFitEvaluationItem::removeFitFile()
{
    if (fitFilesList.size()>1) {
        fitFilesList.removeLast();
        paramTable->rebuildModel();
        setDataChanged();
    }

}

void QFImFCCSFitEvaluationItem::setCurrentIndex(int index)
{
    m_currentIndex=index;
    QFRawDataRecord* r=getFitFile(0);
    if (r) {
        if (index<=getIndexMin(r)) m_currentIndex=getIndexMin(r);
        if (index<=getIndexMax(r)) m_currentIndex=index;
        if (index>getIndexMax(r)) m_currentIndex=getIndexMax(r);
        r->setQFProperty(QString(getType()+QString::number(getID())+"_last_index"), m_currentIndex, false, false);
    }
}

void QFImFCCSFitEvaluationItem::ensureFitFiles()
{
    if (fitFilesList.size()<=0)  {
        QList<QPointer<QFRawDataRecord> > recs=getApplicableRecords();
        if (recs.size()>0) {
            fitFilesList.append(recs[0]);
            paramTable->rebuildModel();
            setDataChanged();
        }
    }
}

void QFImFCCSFitEvaluationItem::setFitFile(int num, QFRawDataRecord *record)
{
    if (num<0) return;
    else if (num>fitFilesList.size()) fitFilesList.append(record);
    else fitFilesList[num]=record;
    emit parameterStructureChanged();
    setDataChanged();
    emit fileChanged(num, record);

}


void QFImFCCSFitEvaluationItem::intWriteData(QXmlStreamWriter &w)
{
    QFFitResultsByIndexAsVectorEvaluation::intWriteData(w);

    w.writeStartElement("multifunctions");
    QMapIterator<int,QString> i(m_multiFitFunctions);
    while (i.hasNext()) {
        i.next();
        w.writeStartElement("multifunction");
        w.writeAttribute("num", QString::number(i.key()));
        w.writeAttribute("id", i.value());
        w.writeEndElement();
    }
    w.writeEndElement();
    w.writeStartElement("fitfiles");
    for (int ii=0; ii<fitFilesList.size(); ii++) {
        w.writeStartElement("fitfile");
        w.writeAttribute("num", QString::number(ii));
        if (fitFilesList[ii]) {
            w.writeAttribute("id", QString::number(fitFilesList[ii]->getID()));
        }
        w.writeEndElement();
    }
    w.writeEndElement();
}

void QFImFCCSFitEvaluationItem::intReadData(QDomElement *e)
{
    QFFitResultsByIndexAsVectorEvaluation::intReadData(e);
    m_multiFitFunctions.clear();
    QDomElement e1=e->firstChildElement("multifunctions").firstChildElement("multifunction");
    while (!e1.isNull()) {
        bool ok=false;
        int num=e1.attribute("num", "-1").toInt(&ok);
        QString id=e1.attribute("id", "");

        if (num>=0 && ok && m_fitFunctions.contains(id)) {
            m_multiFitFunctions[num]=id;
        }

        e1=e1.nextSiblingElement("multifunction");
    }
    fitFilesList.clear();
    e1=e->firstChildElement("fitfiles").firstChildElement("fitfile");
    while (!e1.isNull()) {
        bool ok=false, ok1=false;
        int num=e1.attribute("num", "-1").toInt(&ok);
        int id=e1.attribute("id", "-1").toInt(&ok1);
        QFRawDataRecord* rdr=project->getRawDataByID(id);
        if (num>=0) {
            fitFilesList.append(rdr);
        }

        e1=e1.nextSiblingElement("fitfile");
    }
    ensureFitFiles();
    paramTable->rebuildModel();

}




bool QFImFCCSFitEvaluationItem::isApplicable(QFRawDataRecord *record)
{
    //return record->inherits("QFRDRFCSDataInterface") && record->inherits("QFRDRImageToRunInterface");
    return matchFunctor->matches(record);
}

void QFImFCCSFitEvaluationItem::intWriteDataAlgorithm(QXmlStreamWriter &w) const
{
    w.writeAttribute("weighting", dataWeightToString(getFitDataWeighting()));
}

void QFImFCCSFitEvaluationItem::intReadDataAlgorithm(QDomElement &e)
{
    if (e.hasAttribute("weighting")) {
        QString a=e.attribute("weighting").toLower();
        setFitDataWeighting(stringToDataWeight(a));
    }
}

bool QFImFCCSFitEvaluationItem::hasSpecial(const QFRawDataRecord *r, const QString &id, const QString &paramid, double &value, double &error) const
{
    int run=getIndexFromEvaluationResultID(id);
    return qfFCSHasSpecial(r, run, paramid, value, error);
}

bool QFImFCCSFitEvaluationItem::hasSpecial(const QFRawDataRecord *r, int index, const QString &paramid, double &value, double &error) const
{
    return qfFCSHasSpecial(r, index, paramid, value, error);
}

bool QFImFCCSFitEvaluationItem::overrideFitFunctionPreset(QFRawDataRecord* r, QString paramName, double &value) const
{
    if (qfFCSOverrideFitFunctionPreset(this, r, paramName, value)) {
        return true;
    }
    return QFFitResultsByIndexAsVectorEvaluation::overrideFitFunctionPreset(r, paramName, value);
}

bool QFImFCCSFitEvaluationItem::overrideFitFunctionPresetError(QFRawDataRecord* r, QString paramName, double &value) const
{
    if (qfFCSOverrideFitFunctionPresetError(this, r, paramName, value)) {
        return true;
    }
    return QFFitResultsByIndexAsVectorEvaluation::overrideFitFunctionPresetError(r, paramName, value);
}

bool QFImFCCSFitEvaluationItem::overrideFitFunctionPresetFix(QFRawDataRecord* r, QString paramName, bool &value) const
{
    if (qfFCSOverrideFitFunctionPresetFix(this, r, paramName, value)) {
        return true;
    }
    return QFFitResultsByIndexAsVectorEvaluation::overrideFitFunctionPresetFix(r, paramName, value);
}

QString QFImFCCSFitEvaluationItem::getFitFunctionID() const
{
    return getFitFunctionID(0);
}

QFFitFunction *QFImFCCSFitEvaluationItem::getFitFunction(QFRawDataRecord *rdr) const
{
    int idx=fitFilesList.indexOf(rdr);
    if (rdr && idx>=0) {
        return getFitFunction(idx);
    }
    return NULL;
}

QString QFImFCCSFitEvaluationItem::getFitFunctionID(QFRawDataRecord *rdr) const
{
    int idx=fitFilesList.indexOf(rdr);
    if (rdr && idx>=0) {
        return getFitFunctionID(idx);
    }
    return NULL;
}

QString QFImFCCSFitEvaluationItem::rdrPointerToParameterStoreID(QFRawDataRecord *rdr) const
{
    int idx=fitFilesList.indexOf(rdr);
    if (rdr && idx>=0) {
        return QString::number(idx);
    }
    return QString("");
}

QFImFCCSParameterInputTable *QFImFCCSFitEvaluationItem::getParameterInputTableModel() const
{
    return paramTable;
}

int QFImFCCSFitEvaluationItem::getNumberOfFitFiles() const
{
    return fitFilesList.size();
}

QFMatchRDRFunctor *QFImFCCSFitEvaluationItem::getMatchFunctor() const
{
    return matchFunctor;
}

int QFImFCCSFitEvaluationItem::getCurrentIndex() const
{
    QFRawDataRecord* r=getFitFile(0);
    int index=m_currentIndex;
    if (r!=NULL) {
        /*m_currentIndex=*/index=r->getProperty(QString(getType()+QString::number(getID())+"_last_index"), index).toInt();
    }
    if (index<getIndexMin(r)) {
        /*m_currentIndex=*/index=getIndexMin(r);
    }
    if (index>getIndexMax(r)) {
        /*m_currentIndex=*/index=getIndexMax(r);
    }
    return index;
}


QFImFCCSMatchRDRFunctor::QFImFCCSMatchRDRFunctor()
{
}

bool QFImFCCSMatchRDRFunctor::matches(const QFRawDataRecord *record) const
{
    return record->inherits("QFRDRFCSDataInterface") && record->inherits("QFRDRImageToRunInterface");
}
