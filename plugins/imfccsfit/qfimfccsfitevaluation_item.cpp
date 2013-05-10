#include "qfimfccsfitevaluation_item.h"
#include "qfimfccsfitevaluation_editor.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include "qffitfunction.h"
#include "qftools.h"
#include "qffcstools.h"
#include "qfprojectrawdatamodel.h"
#include "qfglobalfittool.h"

QFImFCCSMatchRDRFunctor::QFImFCCSMatchRDRFunctor()
{
}

bool QFImFCCSMatchRDRFunctor::matches(const QFRawDataRecord *record) const
{
    return record->inherits("QFRDRFCSDataInterface") && record->inherits("QFRDRImageToRunInterface");
}

QFImFCCSFitEvaluationItem::QFImFCCSFitEvaluationItem(QFProject* parent):
    QFFitResultsByIndexAsVectorEvaluation("fcs_,dls_,fccs_", parent, false, false),
    QFFCSWeightingTools(),
    QFFitResultsByIndexMultiRDREvaluationFitTools()
{
    getEvaluationResultIDUsesFitFunction=true;
    matchFunctor=new QFImFCCSMatchRDRFunctor();
    m_weighting=EqualWeighting;
    m_currentIndex=-1;
    m_multiFitFunctions.clear();
    globalParamsChanged=true;
    lastGlobalParamsCount=0;
    doFitThread=new QFGlobalThreadedFit(this);
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
    connect(this, SIGNAL(currentIndexChanged(int)), paramTable, SLOT(rebuildModel()));
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

QFFitStatistics QFImFCCSFitEvaluationItem::calcFitStatistics(bool storeAsResults, QFFitFunction *ffunc, long N, double *tauvals, double *corrdata, double *weights, int datacut_min, int datacut_max, double *fullParams, double *errors, bool *paramsFix, int runAvgWidth, int residualHistogramBins, QFRawDataRecord *record, int run)
{
    return QFFitResultsByIndexEvaluationFitToolsBase::calcFitStatistics(storeAsResults, ffunc,  N, tauvals, corrdata, weights,  datacut_min,  datacut_max, fullParams, errors, paramsFix,  runAvgWidth,  residualHistogramBins, record,  run);
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

QStringList QFImFCCSFitEvaluationItem::getFitFunctionIDs() const
{
    QStringList res;
    for (int i=0; i<getFitFileCount(); i++) {
        res.append(getFitFunctionID(i));
    }
    return res;
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
    if (fitFilesList.size()>0) emit fileChanged(fitFilesList.size()-1, fitFilesList.last());
    return fitFilesList.size()-1;
}

void QFImFCCSFitEvaluationItem::removeFitFile()
{
    if (fitFilesList.size()>1) {
        fitFilesList.removeLast();
        if (fitFilesList.size()>0) emit fileChanged(0, fitFilesList.first());
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
        emit currentIndexChanged(m_currentIndex);
    }
}

void QFImFCCSFitEvaluationItem::setLinkParameter(int file, QString parameter, int global_param)
{
    if (global_param>=0) {
        int old=globalParams[file].value(parameter, -1);
        globalParams[file].insert(parameter, global_param);
        if (old!=global_param) setDataChanged();
    } else {
        bool change=globalParams[file].contains(parameter);
        globalParams[file].remove(parameter);
        if (change) setDataChanged();
    }
    globalParamsChanged=true;
}

void QFImFCCSFitEvaluationItem::unsetLinkParameter(int file, QString parameter)
{
    setLinkParameter(file, parameter, -1);
}

void QFImFCCSFitEvaluationItem::clearLinkParameters(int file)
{
    if (file<0) {
        globalParams.clear();
    } else {
        globalParams.remove(file);
    }
    globalParamsChanged=true;
    setDataChanged();
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

    w.writeStartElement("globalparams");
    QMapIterator<int, QMap<QString, int> > it(globalParams);
    while (it.hasNext()) {
        it.next();
        QMapIterator<QString, int> it2(it.value());
        while (it2.hasNext()) {
            it2.next();
            w.writeStartElement("globalparam");
            w.writeAttribute("file", QString::number(it.key()));
            w.writeAttribute("parameter", it2.key());
            w.writeAttribute("global", QString::number(it2.value()));
            w.writeEndElement();
        }
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

    globalParams.clear();
    e1=e->firstChildElement("globalparams").firstChildElement("globalparam");
    while (!e1.isNull()) {
        bool ok=false, ok1=false;
        int file=e1.attribute("file", "-1").toInt(&ok);
        int global=e1.attribute("global", "-1").toInt(&ok1);
        QString param=e1.attribute("parameter", "");
        if (file>=0 && global>=0 && ok && ok1 && (!param.isEmpty())) {
            globalParams[file].insert(param, global);
        }

        e1=e1.nextSiblingElement("globalparam");
    }
    globalParamsChanged=true;
    lastGlobalParamsCount=0;

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

QList<QFRawDataRecord *> QFImFCCSFitEvaluationItem::getFitFiles() const
{
    QList<QFRawDataRecord *> l;
    for (int i=0; i<fitFilesList.size(); i++) l<<fitFilesList[i];
    return l;
}

QFFitFunction *QFImFCCSFitEvaluationItem::getFitFunction( QFRawDataRecord *rdr) const
{
    //if (rdr) qDebug()<<"QFImFCCSFitEvaluationItem::getFitFunction("<<rdr->getName()<<")";
    //else qDebug()<<"QFImFCCSFitEvaluationItem::getFitFunction("<<rdr<<")";
    return getFitFunctionForID(getFitFunctionID(rdr));
}

QString QFImFCCSFitEvaluationItem::getFitFunctionID(QFRawDataRecord *rdr) const
{
    //if (rdr) qDebug()<<"QFImFCCSFitEvaluationItem::getFitFunctionID("<<rdr->getName()<<")";
    //else qDebug()<<"QFImFCCSFitEvaluationItem::getFitFunctionID("<<rdr<<")";
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

int QFImFCCSFitEvaluationItem::getFitFileCount() const
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

int QFImFCCSFitEvaluationItem::getLinkParameterCount() const
{
    if (!globalParamsChanged) return qMax(0,lastGlobalParamsCount);
    int count=0;
    QMapIterator<int, QMap<QString, int> > it(globalParams);
    while (it.hasNext()) {
        it.next();
        QMapIterator<QString, int> it2(it.value());
        while (it2.hasNext()) {
            it2.next();
            if (it2.value()>count) count=it2.value();
        }
    }
    globalParamsChanged=false;
    lastGlobalParamsCount=count+1;
    return count+1;
}

int QFImFCCSFitEvaluationItem::getLinkParameter(int file, QString parameter) const
{
    if (globalParams.contains(file)) {
        return globalParams[file].value(parameter, -1);
    }
    return -1;
}

QList<QPair<int, QString> > QFImFCCSFitEvaluationItem::getLinkedParameterList(int file, QString parameter) const
{
    QList<QPair<int, QString> > l;
    int link=getLinkParameter(file, parameter);
    if (link<0) return l;

    QMapIterator<int, QMap<QString, int> > it(globalParams);
    while (it.hasNext()) {
        it.next();
        QMapIterator<QString, int> it2(it.value());
        while (it2.hasNext()) {
            it2.next();
            if (it2.value()==link && (it.key()!=file || it2.key()!=parameter)) l<<qMakePair(it.key(), it2.key());
        }
    }
    return l;
}









void QFImFCCSFitEvaluationItem::doFit(const QList<QFRawDataRecord *> &records, int run, int defaultMinDatarange, int defaultMaxDatarange, QFFitAlgorithmReporter *dlgFitProgress, bool doLog)
{
    QString iparams="";
    QString oparams="";
    QString orparams="";


    QFFitAlgorithm* falg=getFitAlgorithm();
    if ((!falg)||records.size()<=0) return;
    QFRDRFCSDataInterface* data0=qobject_cast<QFRDRFCSDataInterface*>(records.first());
    if (!data0) return;
    QFGlobalFitTool tool(falg);
    tool.clear();

    if (dlgFitProgress) {
        dlgFitProgress->reportStatus(tr("setting up ..."));
        dlgFitProgress->setProgressMax(100);
        dlgFitProgress->setProgress(0);
    }

    QString runname=tr("average");
    if (run>=0) runname=QString::number(run);
    int rangeMinDatarange=0;
    int rangeMaxDatarange=data0->getCorrelationN();
    if (defaultMinDatarange>=0) rangeMinDatarange=defaultMinDatarange;
    if (defaultMaxDatarange>=0) rangeMaxDatarange=defaultMaxDatarange;

    restoreQFFitAlgorithmParameters(falg);
    falg->setReporter(dlgFitProgress);

    if (doLog) QFPluginLogTools::log_text(tr("setting up global fit for run %1 ... \n").arg(runname));

    bool epc=get_doEmitPropertiesChanged();
    bool erc=get_doEmitResultsChanged();
    QList<doFitData> fitData;
    set_doEmitPropertiesChanged(false);
    set_doEmitResultsChanged(false);

    QString egroup=QString("%1%2__%3").arg(getType()).arg(getID()).arg(falg->id());//.arg(dfd.ffunc->id());
    QString egrouplabel=QString("#%3 \"%1\": %2").arg(getName()).arg(falg->shortName()).arg(getID());//.arg(dfd.ffunc->shortName());

    for (int r=0; r<records.size(); r++) {
        QFRawDataRecord* record=records[r];
        QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
        if (data) {
            doFitData dfd;
            dfd.emitSignals=record->isEmitResultsChangedEnabled();
            record->disableEmitResultsChanged();

            QFFitFunction* ffunc=getFitFunction(record);
            egroup+=QString("__"+ffunc->id());
            egrouplabel+=QString(", "+ffunc->shortName());
            if (doLog) QFPluginLogTools::log_text(tr("   - adding RDR '%1', model '%2'' ... \n").arg(record->getName()).arg(ffunc->name()));
            dfd.N=data->getCorrelationN();
            dfd.weights=NULL;
            dfd.taudata=data->getCorrelationT();
            dfd.corrdata=NULL;
            if (run<0) {
                dfd.corrdata=data->getCorrelationMean();
            } else {
                if (run<(int)data->getCorrelationRuns()) {
                    dfd.corrdata=data->getCorrelationRun(run);
                } else {
                    dfd.corrdata=data->getCorrelationMean();
                }
            }
            // we also have to care for the data cutting
            int cut_low=rangeMinDatarange;
            int cut_up=rangeMaxDatarange;
            if (cut_low<0) cut_low=0;
            if (cut_up>=dfd.N) cut_up=dfd.N-1;
            int cut_N=cut_up-cut_low+1;
            if (cut_N<0) {
                cut_low=0;
                cut_up=ffunc->paramCount()-1;
                if (cut_up>=dfd.N) cut_up=dfd.N-1;
                cut_N=cut_up+1;
            }
            if (doLog) QFPluginLogTools::log_text(tr("      - fit data range: %1...%2 (%3 datapoints)\n").arg(cut_low).arg(cut_up).arg(cut_N));
            dfd.weightsOK=false;
            dfd.weights=allocWeights(&(dfd.weightsOK), record, run, cut_low, cut_up);
            if (!dfd.weightsOK && doLog) QFPluginLogTools::log_warning(tr("      - weights have invalid values => setting all weights to 1\n"));
            // retrieve fit parameters and errors. run calcParameters to fill in calculated parameters and make sure
            // we are working with a complete set of parameters
            dfd.params=allocFillParameters(record, run);
            dfd.initialparams=allocFillParameters(record, run);
            dfd.errors=allocFillParameterErrors(record, run);
            dfd.errorsI=allocFillParameterErrors(record, run);
            dfd.paramsMin=allocFillParametersMin();
            dfd.paramsMax=allocFillParametersMax();
            dfd.paramsFix=allocFillFix(record, run);
            dfd.ffunc=ffunc;
            dfd.cut_low=cut_low;
            dfd.cut_up=cut_up;
            dfd.cut_N=cut_N;

            qDebug()<<"adding "<<r<<": ("<<record->getName()<<")   ffunc="<<ffunc->name();
            qDebug()<<"  params="<<arrayToString(dfd.params, ffunc->paramCount());
            qDebug()<<"  initialparams="<<arrayToString(dfd.initialparams, ffunc->paramCount());
            qDebug()<<"  errors="<<arrayToString(dfd.errors, ffunc->paramCount());
            qDebug()<<"  errorsI="<<arrayToString(dfd.errorsI, ffunc->paramCount());

            paramsVector.append(dfd.params);
            initialParamsVector.append(dfd.initialparams);
            errorsVector.append(dfd.errors);

            ffunc->calcParameter(dfd.params, dfd.errors);
            ffunc->calcParameter(dfd.initialparams, dfd.errors);

            int fitparamcount=0;
            //qDebug()<<"record "<<r<<"  ("<<record->getName()<<"):";
            for (int i=0; i<ffunc->paramCount(); i++) {
                if (ffunc->isParameterVisible(i, dfd.params) && (!dfd.paramsFix[i]) && ffunc->getDescription(i).fit) {
                    if (!iparams.isEmpty()) iparams=iparams+";  ";
                    fitparamcount++;
                    iparams=iparams+QString("%1 = %2").arg(ffunc->getDescription(i).id).arg(dfd.params[i]);
                }
                //qDebug("  before_fit: %s = %lf +/m %lf", ffunc->getDescription(i).id.toStdString().c_str(), dfd.params[i], dfd.errors[i]);
            }
            //qDebug()<<"\n\n";
            if (doLog) QFPluginLogTools::log_text(tr("      - initial params         (%1)\n").arg(iparams));


            fitData.append(dfd);
            tool.addTerm(ffunc, dfd.params, dfd.paramsFix, dfd.taudata, dfd.corrdata, dfd.weights, dfd.N);
        }
    }

    if (doLog) QFPluginLogTools::log_text(tr("   - adding global links ... \n"));
    int cnt=0;
    tool.setGlobalParamCount(getLinkParameterCount());
    //QList<QList<QPair<int, int> > > links(getLinkParameterCount());
    for (int r=0; r<records.size(); r++) {
        QFRawDataRecord* record=records[r];
        QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
        if (data) {
            QStringList paramIDs=fitData[cnt].ffunc->getParameterIDs();
            for (int p=0; p<paramIDs.size(); p++) {
                int l=getLinkParameter(r, paramIDs[p]);
                if (l>=0) {
                    tool.addToGlobalParam(l, r, p);
                }
            }
            cnt++;
        }
    }

    bool OK=true;
    try {
        if (!dlgFitProgress->isCanceled()) {

            if (dlgFitProgress) {
                dlgFitProgress->reportStatus(tr("fitting ..."));
                dlgFitProgress->setProgressMax(100);
                dlgFitProgress->setProgress(0);
            }
            doFitThread->init(&tool, paramsVector, errorsVector, initialParamsVector, errorsVector);
            //doFitThread->init(falg, params, errors, &taudata[cut_low], &corrdata[cut_low], &weights[cut_low], cut_N, ffunc, initialparams, paramsFix, paramsMin, paramsMax);
            doFitThread->start(QThread::HighPriority);
            QTime t;
            t.start();
            while (!doFitThread->isFinished()) {
                if (t.elapsed()>10) {
                    QApplication::processEvents(QEventLoop::AllEvents, 50);
                    if (dlgFitProgress && dlgFitProgress->isCanceled()) {
                      doFitThread->terminate();
                      break;
                    }
                    t.start();
                }
            }
            if (dlgFitProgress) {
                dlgFitProgress->setProgressFull();
                dlgFitProgress->reportStatus(tr("calculating parameters, errors and storing data ..."));
                OK=!dlgFitProgress->isCanceled();
            } else {
                OK=true;
            }
        }

        if (OK) {
            QFFitAlgorithm::FitResult result=doFitThread->getResults();
            cnt=0;
            for (int r=0; r<records.size(); r++) {
                QFRawDataRecord* record=records[r];
                QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
                if (data) {
                    doFitData dfd=fitData[cnt];
                    double* errors=errorsVector[cnt];
                    double* params=paramsVector[cnt];
                    for (int i=0; i<dfd.ffunc->paramCount(); i++) {
                        if (!(dfd.ffunc->isParameterVisible(i, params) && (!dfd.paramsFix[i]) && dfd.ffunc->getDescription(i).fit)) {
                            errors[i]=dfd.errorsI[i];
                        }
                    }

                    dfd.ffunc->calcParameter(params, errors);
                    dfd.ffunc->sortParameter(params, errors, dfd.paramsFix);
                    dfd.ffunc->calcParameter(params, errors);

                    //qDebug()<<"record "<<r<<"  ("<<record->getName()<<"):  errors="<<errors<<"  params="<<params;
                    for (int i=0; i<dfd.ffunc->paramCount(); i++) {
                        if (dfd.ffunc->isParameterVisible(i, params) && (!dfd.paramsFix[i]) && dfd.ffunc->getDescription(i).fit) {
                            if (!oparams.isEmpty()) oparams=oparams+";  ";

                            oparams=oparams+QString("%1 = %2+/-%3").arg(dfd.ffunc->getDescription(i).id).arg(params[i]).arg(errors[i]);
                        }
                        //qDebug("  after_fit: %s = %lf +/- %lf", dfd.ffunc->getDescription(i).id.toStdString().c_str(), dfd.params[i], dfd.errors[i]);
                    }
                    //qDebug()<<"\n\n";

                    // round errors and values
                    for (int i=0; i<dfd.ffunc->paramCount(); i++) {
                        errors[i]=roundError(errors[i], 2);
                        params[i]=roundWithError(params[i], errors[i], 2);
                    }

                    //qDebug()<<"record "<<r<<"  ("<<record->getName()<<"):";
                    for (int i=0; i<dfd.ffunc->paramCount(); i++) {
                        if (dfd.ffunc->isParameterVisible(i, params) && (!dfd.paramsFix[i]) && dfd.ffunc->getDescription(i).fit) {
                            if (!orparams.isEmpty()) orparams=orparams+";  ";
                            orparams=orparams+QString("%1 = %2+/-%3").arg(dfd.ffunc->getDescription(i).id).arg(params[i]).arg(errors[i]);
                        }
                        //qDebug("  rounded_after_fit: %s = %lf +/- %lf", dfd.ffunc->getDescription(i).id.toStdString().c_str(), params[i], errors[i]);
                    }
                    //qDebug()<<"\n\n";
                    setFitResultValuesVisibleWithGroupAndLabel(record, run, params, errors, tr("fit results"), dfd.paramsFix, tr("fit results"), true);

                    //qDebug()<<"record "<<r<<"  ("<<record->getName()<<"):";
                    for (int i=0; i<dfd.ffunc->paramCount(); i++) {
                        //qDebug("  rounded_after_fit_after_save: %s = %lf +/- %lf", dfd.ffunc->getDescription(i).id.toStdString().c_str(), params[i], errors[i]);
                    }
                    //qDebug()<<"\n\n";

                    if (doLog) QFPluginLogTools::log_text(tr("   - fit completed after %1 msecs with result %2\n").arg(doFitThread->getDeltaTime()).arg(result.fitOK?tr("success"):tr("no convergence")));
                    if (doLog) QFPluginLogTools::log_text(tr("   - result-message: %1\n").arg(result.messageSimple));
                    if (doLog) QFPluginLogTools::log_text(tr("   - initial params         (%1)\n").arg(iparams));
                    if (doLog) QFPluginLogTools::log_text(tr("   - output params          (%1)\n").arg(oparams));
                    if (doLog) QFPluginLogTools::log_text(tr("   - output params, rounded (%1)\n").arg(orparams));


                    QString evalID=transformResultID(getEvaluationResultID(run, record));
                    QString param;
                    QString group="fit properties";


                    record->resultsSetEvaluationGroup(evalID, egroup);
                    record->resultsSetEvaluationGroupLabel(egroup, egrouplabel);
                    record->resultsSetEvaluationGroupIndex(evalID, run);
                    record->resultsSetEvaluationDescription(evalID, QString(""));


                    if (run<0) record->resultsSetString(evalID, "fit_model_name", dfd.ffunc->id());
                    else record->resultsSetInStringList(evalID, "fit_model_name", run, dfd.ffunc->id());
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: model"));

                    if (run<0) record->resultsSetString(evalID, "fitalg_name", falg->id());
                    else if (!getProperty("dontSaveFitResultMessage", false).toBool()) record->resultsSetInStringList(evalID, "fitalg_name", run, falg->id());
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: algorithm"));

                    if (run<0) record->resultsSetNumber(evalID, "fitalg_runtime", doFitThread->getDeltaTime(), "msecs");
                    else record->resultsSetInNumberList(evalID, "fitalg_runtime", run, doFitThread->getDeltaTime(), "msecs");
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: runtime"));

                    if (run<0) record->resultsSetBoolean(evalID, "fitalg_success", result.fitOK);
                    else record->resultsSetInBooleanList(evalID, "fitalg_success", run, result.fitOK);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: success"));

                    if (run<0) record->resultsSetString(evalID, "fitalg_message", result.messageSimple);
                    else if (!getProperty("dontSaveFitResultMessage", false).toBool()) record->resultsSetInStringList(evalID, "fitalg_message", run, result.messageSimple);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: message"));

                    if (run<0) record->resultsSetString(evalID, "fitalg_messageHTML", result.message);
                    else if (!getProperty("dontSaveFitResultMessage", false).toBool()) record->resultsSetInStringList(evalID, "fitalg_messageHTML", run, result.message);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: message (markup)"));

                    if (run<0) record->resultsSetInteger(evalID, "fit_datapoints", dfd.cut_N);
                    else record->resultsSetInIntegerList(evalID, "fit_datapoints", run, dfd.cut_N);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: datapoints"));

                    if (run<0) record->resultsSetInteger(evalID, "fit_cut_low", dfd.cut_low);
                    else record->resultsSetInIntegerList(evalID, "fit_cut_low", run, dfd.cut_low);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: first point"));

                    if (run<0) record->resultsSetInteger(evalID, "fit_cut_up", dfd.cut_up);
                    else record->resultsSetInIntegerList(evalID, "fit_cut_up", run, dfd.cut_up);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: last point"));


                    QMapIterator<QString, QFRawDataRecord::evaluationResult> it(result.params);
                    while (it.hasNext()) {
                        it.next();
                        param="";
                        //record->resultsSet(evalID, param=("fitalg_"+it.key()), it.value());
                        switch(it.value().type) {
                            case QFRawDataRecord::qfrdreNumber:
                                if (run<0) record->resultsSetNumber(evalID, param=("fitalg_"+it.key()), it.value().dvalue, it.value().unit);
                                else record->resultsSetInNumberList(evalID, param=("fitalg_"+it.key()), run, it.value().dvalue, it.value().unit);
                                break;
                            case QFRawDataRecord::qfrdreNumberError:
                                if (run<0) record->resultsSetNumberError(evalID, param=("fitalg_"+it.key()), it.value().dvalue, it.value().derror, it.value().unit);
                                else record->resultsSetInNumberErrorList(evalID, param=("fitalg_"+it.key()), run, it.value().dvalue, it.value().derror, it.value().unit);
                                break;
                            case QFRawDataRecord::qfrdreInteger:
                                if (run<0) record->resultsSetInteger(evalID, param=("fitalg_"+it.key()), it.value().ivalue, it.value().unit);
                                else record->resultsSetInIntegerList(evalID, param=("fitalg_"+it.key()), run, it.value().ivalue, it.value().unit);
                                break;
                            case QFRawDataRecord::qfrdreBoolean:
                                if (run<0) record->resultsSetBoolean(evalID, param=("fitalg_"+it.key()), it.value().bvalue);
                                else record->resultsSetInBooleanList(evalID, param=("fitalg_"+it.key()), run, it.value().bvalue, it.value().unit);
                                break;
                            case QFRawDataRecord::qfrdreString:
                                if (run<0) record->resultsSetString(evalID, param=("fitalg_"+it.key()), it.value().svalue);
                                else if (!getProperty("dontSaveFitResultMessage", false).toBool()) record->resultsSetInStringList(evalID, param=("fitalg_"+it.key()), run, it.value().svalue, it.value().unit);
                                break;

                            case QFRawDataRecord::qfrdreBooleanVector:
                            case QFRawDataRecord::qfrdreBooleanMatrix:
                            case QFRawDataRecord::qfrdreNumberVector:
                            case QFRawDataRecord::qfrdreNumberMatrix:
                            case QFRawDataRecord::qfrdreNumberErrorVector:
                            case QFRawDataRecord::qfrdreNumberErrorMatrix:
                            case QFRawDataRecord::qfrdreIntegerVector:
                            case QFRawDataRecord::qfrdreIntegerMatrix:
                                if (run<0) record->resultsSet(evalID, param=("fitalg_"+it.key()), it.value());
                                break;
                            default:
                                break;
                        }

                        if (!param.isEmpty()) {
                            record->resultsSetGroup(evalID, param, group);
                            record->resultsSetLabel(evalID, param, it.value().label, it.value().label_rich);
                        }
                    }


                    {
                        QFFitStatistics fit_stat=calcFitStatistics(true, dfd.ffunc, dfd.N, dfd.taudata, dfd.corrdata, dfd.weights, dfd.cut_low, dfd.cut_up, params, errors, dfd.paramsFix, 11, 25, record, run);
                        fit_stat.free();
                    }
                    cnt++;
                }
            }
        } else {
            if (doLog) QFPluginLogTools::log_warning(tr("   - fit canceled by user!!!\n"));
        }
    } catch(std::exception& E) {
        if (doLog) QFPluginLogTools::log_error(tr("error during fitting, error message: %1\n").arg(E.what()));
    }

    if (dlgFitProgress) dlgFitProgress->reportStatus(tr("clean up ..."));
    if (doLog) QFPluginLogTools::log_text(tr("   - clean up ... \n"));
    if (epc) set_doEmitPropertiesChanged(true);
    if (erc) set_doEmitResultsChanged(true);
    cnt=0;
    for (int r=0; r<records.size(); r++) {
        QFRawDataRecord* record=records[r];
        QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
        if (data) {
            if (fitData[cnt].emitSignals) record->enableEmitResultsChanged(true);
            free(fitData[cnt].weights);
            free(fitData[cnt].params);
            free(fitData[cnt].initialparams);
            free(fitData[cnt].errors);
            free(fitData[cnt].errorsI);
            free(fitData[cnt].paramsFix);
            free(fitData[cnt].paramsMax);
            free(fitData[cnt].paramsMin);

            cnt++;
        }
    }
    fitData.clear();
    falg->setReporter(NULL);

}

void QFImFCCSFitEvaluationItem::doFitForMultithread(const QList<QFRawDataRecord *> &records, int run, int defaultMinDatarange, int defaultMaxDatarange, QFPluginLogService *logservice) const
{
}
