#include "qfproject.h"
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "qfprojectrawdatamodel.h"
#include "qfprojecttreemodel.h"
#include "qfevaluationitemfactory.h"
#include "qfrawdatarecordfactory.h"
#include "qfpluginservices.h"
#include "qfversion.h"

#include <QTemporaryFile>
#include <QElapsedTimer>
//#define DEBUG_TIMING
#undef DEBUG_TIMING

QFProject::QFProject(QFEvaluationItemFactory* evalFactory, QFRawDataRecordFactory* rdrFactory, QFPluginServices* services, QObject* parent):
    QObject(parent), QFProperties()
{
    dataChange=false;
    this->services=services;
    this->rdrFactory=rdrFactory;
    this->evalFactory=evalFactory;
    name="unnamed";
    file="";
    description="";
    creator="";
    highestID=-1;
    errorOcc=false;
    errorDesc="";
    rdModel=NULL;
    treeModel=NULL;
    reading=false;
    m_signalsEnabled=true;
    m_dummy=false;
}


QFProject::~QFProject()
{
    //std::cout<<"deleting QFProject\n";
    QMapIterator<int, QFRawDataRecord*> i(rawData);
    while (i.hasNext()) {
        i.next();
        //emit recordAboutToBeDeleted(i.value());
        delete i.value();
    }
    rawData.clear();
    rawDataOrder.clear();

    QMapIterator<int, QFEvaluationItem*> j(evaluations);
    while (j.hasNext()) {
        j.next();
        //emit recordAboutToBeDeleted(i.value());
        delete j.value();
    }
    evaluations.clear();
    evaluationsOrder.clear();
    //std::cout<<"deleting QFProject ... OK\n";
}

void QFProject::setSignalsEnabled(bool enabled, bool emitChange)
{
    m_signalsEnabled=enabled;
    if (emitChange) {
        emit structureChanged();
        emit wasChanged(dataChange);
        emit propertiesChanged();
    }
}

QFProjectRawDataModel* QFProject::getRawDataModel() {
    if (rdModel==NULL) {
        rdModel=new QFProjectRawDataModel(this);
        rdModel->setParent(this);
    }
    return rdModel;
};

QFProjectTreeModel* QFProject::getTreeModel() {
    if (treeModel==NULL) {
        treeModel=new QFProjectTreeModel(this);
        treeModel->init(this);
    }
    return treeModel;
};

QFRawDataRecord* QFProject::getNextRawData(QFRawDataRecord* current) {
    if ( (!current) || (rawData.size()<=0) || rawDataOrder.isEmpty() ) return NULL;
    /*int i=rawData.values().indexOf(current);
    if ((i+1>0) && (i+1<rawData.size())) return rawData.values().at(i+1);
    if (i==rawData.size()-1) return rawData.values().at(0);
    return NULL;*/
    int i=rawDataOrder.indexOf(current->getID());
    int newI=i+1;
    if ((newI>=0) && newI<rawDataOrder.size()) return rawData.value(rawDataOrder[newI], NULL);
    if (newI>=rawDataOrder.size()) return rawData.value(rawDataOrder.first(), NULL);
    return NULL;
}

QFRawDataRecord* QFProject::getPreviousRawData(QFRawDataRecord* current) {
    if ( (!current) || (rawData.size()<=0) || rawDataOrder.isEmpty() ) return NULL;
    /*int i=rawData.values().indexOf(current);
    if ((i-1>=0) && (i-1<rawData.size()) ) return rawData.values().at(i-1);
    if (i==0) return rawData.values().at(rawData.size()-1);
    return NULL;*/
    int i=rawDataOrder.indexOf(current->getID());
    int newI=i-1;
    if ((newI>=0) && newI<rawDataOrder.size()) return rawData.value(rawDataOrder[newI], NULL);
    if (newI<=0) return rawData.value(rawDataOrder.last(), NULL);
    return NULL;

}

QFRawDataRecord* QFProject::getNextRawDataOfSameType(QFRawDataRecord* current) {
    /*if (!current || rawData.size()<=0) return NULL;
    int i=rawData.values().indexOf(current);
    QString t=current->getType();
    int cnt=rawData.size();
    for (int k=i+1; k<i+cnt; k++) {
        int l=k%cnt;
        if (rawData.values().at(l)->getType()==t) return rawData.values().at(l);
    }
    return current;*/
    if ( (!current) || (rawData.size()<=0) || rawDataOrder.isEmpty() ) return NULL;
    int i=rawDataOrder.indexOf(current->getID());
    int newI=i+1;
    QString t=current->getType();
    int cnt=rawDataOrder.size();
    for (int k=newI; k<i+cnt; k++) {
        int l=k%cnt;
        QFRawDataRecord* rec=rawData[rawDataOrder[l]];
        if (rec->getType()==t) return rec;
    }
    return current;
}

QFRawDataRecord* QFProject::getPreviousRawDataOfSameType(QFRawDataRecord* current) {
    /*if (!current || rawData.size()<=0) return NULL;
    int i=rawData.values().indexOf(current);
    QString t=current->getType();
    int cnt=rawData.size();
    for (int k=i-1+cnt; k>i; k--) {
        int l=k%cnt;
        if (rawData.values().at(l)->getType()==t) return rawData.values().at(l);
    }
    return current;*/
    if ( (!current) || (rawData.size()<=0) || rawDataOrder.isEmpty() ) return NULL;
    int i=rawDataOrder.indexOf(current->getID());
    int newI=i-1;
    QString t=current->getType();
    int cnt=rawDataOrder.size();
    for (int k=newI+cnt; k>i; k--) {
        int l=k%cnt;
        QFRawDataRecord* rec=rawData[rawDataOrder[l]];
        if (rec->getType()==t) return rec;
    }
    return current;
}

QFEvaluationItem* QFProject::getNextEvaluation(QFEvaluationItem* current) {
    /*if (!current || evaluations.size()<=0) return NULL;
    int i=evaluations.values().indexOf(current);
    if (i+1>=0 && i+1<evaluations.size()) return evaluations.values().at(i+1);
    if (i==evaluations.size()-1) return evaluations.values().at(0);
    return NULL;*/
    if ( (!current) || (evaluations.size()<=0) || evaluationsOrder.isEmpty() ) return NULL;
    int i=evaluationsOrder.indexOf(current->getID());
    int newI=i+1;
    if ((newI>=0) && newI<evaluationsOrder.size()) return evaluations.value(evaluationsOrder[newI], NULL);
    if (newI>=evaluationsOrder.size()) return evaluations.value(evaluationsOrder.first(), NULL);
    return NULL;

}

QFEvaluationItem* QFProject::getPreviousEvaluation(QFEvaluationItem* current) {
    /*if (!current || evaluations.size()<=0) return NULL;
    int i=evaluations.values().indexOf(current);
    if (i-1>=0 && i-1<evaluations.size()) return evaluations.values().at(i-1);
    if (i==0) return evaluations.values().at(evaluations.size()-1);
    return NULL;*/
    if ( (!current) || (evaluations.size()<=0) || evaluationsOrder.isEmpty() ) return NULL;
    int i=evaluationsOrder.indexOf(current->getID());
    int newI=i-1;
    if ((newI>=0) && newI<evaluationsOrder.size()) return evaluations.value(evaluationsOrder[newI], NULL);
    if (newI<=0) return evaluations.value(evaluationsOrder.last(), NULL);
    return NULL;
}

QFEvaluationItem* QFProject::getNextEvaluationOfSameType(QFEvaluationItem* current) {
    /*if (!current || evaluations.size()<=0) return NULL;
    int i=evaluations.values().indexOf(current);
    QString t=current->getType();
    int cnt=evaluations.size();
    for (int k=i+1; k<i+cnt; k++) {
        int l=k%cnt;
        if (evaluations.values().at(l)->getType()==t) return evaluations.values().at(l);
    }
    return current;*/
    if ( (!current) || (evaluations.size()<=0) || evaluationsOrder.isEmpty() ) return NULL;
    int i=evaluationsOrder.indexOf(current->getID());
    int newI=i+1;
    QString t=current->getType();
    int cnt=evaluationsOrder.size();
    for (int k=newI; k<i+cnt; k++) {
        int l=k%cnt;
        QFEvaluationItem* rec=evaluations[evaluationsOrder[l]];
        if (rec->getType()==t) return rec;
    }
    return current;
}

QFEvaluationItem* QFProject::getPreviousEvaluationOfSameType(QFEvaluationItem* current) {
    /*if (!current || evaluations.size()<=0) return NULL;
    int i=evaluations.values().indexOf(current);
    QString t=current->getType();
    int cnt=evaluations.size();
    for (int k=i-1+cnt; k>i; k--) {
        int l=k%cnt;
        if (evaluations.values().at(l)->getType()==t) return evaluations.values().at(l);
    }
    return current;*/
    if ( (!current) || (evaluations.size()<=0) || evaluationsOrder.isEmpty() ) return NULL;
    int i=evaluationsOrder.indexOf(current->getID());
    int newI=i-1;
    QString t=current->getType();
    int cnt=evaluationsOrder.size();
    for (int k=newI+cnt; k>i; k--) {
        int l=k%cnt;
        QFEvaluationItem* rec=evaluations[evaluationsOrder[l]];
        if (rec->getType()==t) return rec;
    }
    return current;
}


int QFProject::getNewID() {
    highestID++;
    return highestID;
}

bool QFProject::registerRawDataRecord(QFRawDataRecord* rec) {
    if (!rec) return false;
    int newID=rec->getID();
    if (rawData.contains(newID)) return false;
    if (newID>highestID) highestID=newID;
    //rawData[newID]=rec;
    rawData.insert(newID, rec);
    rawDataOrder.append(newID);
    IDs.insert(newID);
    dataChange=true;
    connect(rec, SIGNAL(propertiesChanged(QString,bool)), this, SLOT(setDataChanged()));
    connect(rec, SIGNAL(basicPropertiesChanged()), this, SLOT(setStructureChanged()));
    connect(rec, SIGNAL(rawDataChanged()), this, SLOT(setDataChanged()));
    connect(rec, SIGNAL(resultsChanged(QString,QString,bool)), this, SLOT(setDataChanged()));
    emitStructureChanged();

    return true;
}

bool QFProject::registerEvaluation(QFEvaluationItem* rec) {
    if (!rec) return false;
    int newID=rec->getID();
    if (evaluations.contains(newID)) return false;
    if (newID>highestID) highestID=newID;
    //evaluations[rec->getID()]=rec;
    evaluations.insert(newID, rec);
    evaluationsOrder.append(newID);
    IDs.insert(newID);
    connect(rec, SIGNAL(basicPropertiesChanged()), this, SLOT(setStructureChanged()));
    connect(rec, SIGNAL(resultsChanged(QFRawDataRecord*,QString,QString)), this, SLOT(setDataChanged()));
    connect(rec, SIGNAL(propertiesChanged(QString,bool)), this, SLOT(setDataChanged()));
    emitStructureChanged();
    return true;
}

void QFProject::deleteRawData(int ID) {
    if (rawDataIDExists(ID)) {
        QFRawDataRecord* rec=rawData[ID];
        emit recordAboutToBeDeleted(rec);
        rec->disconnect();
        rawData.remove(ID);
        rawDataOrder.removeAll(ID);
        IDs.remove(ID);
        delete rec;
        emitStructureChanged();
    }
}

void QFProject::deleteEvaluation(int ID) {
    if (evaluationIDExists(ID)) {
        QFEvaluationItem* rec=evaluations[ID];
        emit evaluationAboutToBeDeleted(rec);
        rec->disconnect();
        evaluations.remove(ID);
        evaluationsOrder.removeAll(ID);
        IDs.remove(ID);
        delete rec;
        emitStructureChanged();
    }
}

void QFProject::writeXML(const QString& file, bool resetDataChanged) {

    if (reading) return;
    bool namechanged=(file!=this->file);
    this->file=file;
    /*QFile f(file);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setError(tr("Could no open file '%1' for output!\n Error description: %2.").arg(file).arg(f.errorString()));
        return;
    }*/
    QTemporaryFile f(QFileInfo(file).absolutePath()+"/XXXXXX.tmp");
    f.open();
    QXmlStreamWriter w(&f);
    //QByteArray bytemp;
    //{
//        QBuffer buf(&bytemp);
  //      QXmlStreamWriter w(&buf);
        w.setAutoFormatting(false);
        w.writeStartDocument();
        w.writeStartElement("quickfitproject");
        w.writeAttribute("quickfit_version", qfInfoVersionFull());
        w.writeAttribute("quickfit_svn", qfInfoSVNVersion());
        w.writeAttribute("quickfit_compiledate", qfInfoCompileDate());
        w.writeAttribute("name", name);
        w.writeAttribute("creator", creator);
        w.writeStartElement("description");
        w.writeCDATA(description);
        w.writeEndElement();
        w.writeStartElement("properties");
        storeProperties(w);
        w.writeEndElement();
        w.writeStartElement("rawdata");
        /*QMapIterator<int, QFRawDataRecord*> ir(rawData);
        //for (int i=0; i<rawData.keys().size(); i++) {
        while (ir.hasNext()) {
            ir.next();
            //qDebug()<<"writing rdr "<<ir.key();
            //int k=rawData.keys().at(i);
            ir.value()->writeXML(w);
            //qDebug()<<"   DONE!";
        }*/
        for (int i=0; i<rawDataOrder.size(); i++) {
            QFRawDataRecord* rec=getRawDataByID(rawDataOrder[i]);
            if (rec) rec->writeXML(w);
        }
        w.writeEndElement();
        w.writeStartElement("evaluations");
        /*QMapIterator<int, QFEvaluationItem*> i(evaluations);
        //for (int i=0; i<evaluations.keys().size(); i++) {
        while (i.hasNext()) {
            i.next();
            //qDebug()<<"writing eval "<<i.key();
            //int k=evaluations.keys().at(i);
            i.value()->writeXML(w);
            //qDebug()<<"   DONE!";
        }*/
        for (int i=0; i<evaluationsOrder.size(); i++) {
            QFEvaluationItem* rec=getEvaluationByID(evaluationsOrder[i]);
            if (rec) rec->writeXML(w);
        }
        w.writeEndElement();

        w.writeEndElement();
        w.writeEndDocument();
        if (resetDataChanged) {
            if (!errorOcc) {
                emitStructureChanged();
                if (namechanged) emitPropertiesChanged();
            }
        }
  //  }

    QFile f2(file+".backup");
    if (f2.exists()) f2.remove();

    QFile f1(file);
    f1.rename(file+".backup");

    //QFile f(file);
    //if (f.open(QIODevice::WriteOnly)) {
//        f.write(bytemp);
//    } else {
//        setError(tr("Could no open file '%1' for output!\n Error description: %2.").arg(file).arg(f.errorString()));
//    }
    f.setAutoRemove(false);
    if (!f.rename(file)) {
        setError(tr("Could no open file '%1' for output!\n Error description: %2.").arg(file).arg(f.errorString()));
    }
    /*if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setError(tr("Could no open file '%1' for output!\n Error description: %2.").arg(file).arg(f.errorString()));
        return;
    }*/
}

void QFProject::readXML(const QString &file)
{
    m_dummy=false;
    m_subset=false;
    subsetRDR.clear();
    subsetEval.clear();
    internalReadXML(file);
}

void QFProject::readXMLSubSet(const QString &file, const QSet<int> &rdrSelected, const QSet<int> &evalSelected)
{
    m_dummy=false;
    m_subset=true;
    subsetRDR=rdrSelected;
    subsetEval=evalSelected;
    internalReadXML(file);

    // change filename, so old project does not get overwritten!
    QFileInfo fi=QFileInfo(this->file);
    QDir d=fi.dir();
    QString newFN=fi.completeBaseName()+QString("_subset.")+fi.suffix();
    int i=1;
    while (d.exists(newFN)) {
        newFN=fi.completeBaseName()+QString("_subset%1.").arg(i, 2, 10, QLatin1Char('0'))+fi.suffix();
    }
    this->file=newFN;
    setDataChanged();

    setName(tr("subset of '%1'").arg(getName()));
}

void QFProject::readXMLDummy(const QString &file)
{
    m_dummy=true;
    m_subset=false;
    subsetRDR.clear();
    subsetEval.clear();
    internalReadXML(file);
}


void QFProject::internalReadXML(const QString& file) {
    this->reading=true;
    bool namechanged=(file!=this->file);
    this->file=file;

#ifdef DEBUG_TIMING
    QElapsedTimer time;
    time.start();
#endif

    QFile f(file);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setError(tr("Could no open file '%1' for input!\n Error description: %2.").arg(file).arg(f.errorString()));
        this->reading=false;
        return;
    }
    QDomDocument d("quickfitproject");
    QString errorm="";
#ifdef DEBUG_TIMING
        qDebug()<<"opened file     "<<time.elapsed()<<"ms"; time.start();
#endif
    if (d.setContent(&f, &errorm)) {
        QDomElement e=d.documentElement();
#ifdef DEBUG_TIMING
        qDebug()<<"parsed XML      "<<time.elapsed()<<"ms"; time.start();
#endif
        if (e.tagName()=="quickfitproject") {
            QString v=e.attribute("quickfit_version");
            if (v.toDouble()>3.0) {
                setError(tr("Error while parsing project file '%1'!\n Error description: The project file was written by a newer version of QuickFit than this one (writer version: %1).").arg(file).arg(v));
                this->reading=false;
                return;
            } else {
                name=e.attribute("name");
                creator=e.attribute("creator");
                QDomElement de=e.firstChildElement("description");
                if (!de.isNull()) {
                    description=de.text();
                }
                //std::cout<<"    reading XML: project properties\n";
                QDomElement te=e.firstChildElement("properties");
                readProperties(te);

                if (services) {
                    services->setProgressRange(0, e.firstChildElement("rawdata").elementsByTagName("rawdataelement").size()+e.firstChildElement("evaluations").elementsByTagName("evaluation").size());
                }
                QDomElement rd=e.firstChildElement("rawdata");
                if (!rd.isNull()) {
#ifdef DEBUG_TIMING
        time.start();
#endif

                    rd=rd.firstChildElement("rawdataelement");
                    while (!rd.isNull()) {
                        if (services) {
                            services->incProgress();
                            QApplication::processEvents(QEventLoop::AllEvents, 50);
                        }
                        QString t=rd.attribute("type", "invalid").toLower();
                        bool IDok=false;
                        bool loadRec=true;
                        int ID=rd.attribute("id", "-1").toInt(&IDok);
                        //std::cout<<t.toStdString()<<std::endl;

                        if (m_subset) {
                            loadRec=false;
                            if (IDok && subsetRDR.contains(ID)) loadRec=true;
                        }
                        if (loadRec) {
                            try {
                                QFRawDataRecord* e=getRawDataRecordFactory()->createRecord(t, this);
                                e->init(rd, m_dummy);
                            } catch(std::exception& E) {
                                setError(tr("Error while opening raw data element: %2").arg(E.what()));
                            }
                        }
                        rd=rd.nextSiblingElement("rawdataelement");
#ifdef DEBUG_TIMING
        qDebug()<<"read rdr     "<<time.elapsed()<<"ms"; time.start();
#endif

                    }
                }
                rd=e.firstChildElement("evaluations");
                if (!rd.isNull()) {
                    rd=rd.firstChildElement("evaluationelement");
#ifdef DEBUG_TIMING
        time.start();
#endif
                    while (!rd.isNull()) {
                        if (services) {
                            services->incProgress();
                            QApplication::processEvents(QEventLoop::AllEvents, 50);
                        }
                        QString t=rd.attribute("type", "invalid").toLower();
                        bool IDok=false;
                        bool loadRec=true;
                        int ID=rd.attribute("id", "-1").toInt(&IDok);
                        //std::cout<<t.toStdString()<<std::endl;

                        if (m_subset) {
                            loadRec=false;
                            if (IDok && subsetEval.contains(ID)) loadRec=true;
                        }
                        if (loadRec) {

                            try {
                                QFEvaluationItem* e=getEvaluationItemFactory()->createRecord(t, services, this);
                                e->init(rd, m_dummy);
                            } catch(std::exception& E) {
                                setError(tr("Error while opening raw data element: %2").arg(E.what()));
                            }
                        }

                        rd=rd.nextSiblingElement("evaluationelement");
#ifdef DEBUG_TIMING
        qDebug()<<"read eval     "<<time.elapsed()<<"ms"; time.start();
#endif
                    }
                }
                if (services) {
                    services->setProgress(0);
                }
            }
        } else {
            setError(tr("Error while parsing project file '%1'!\n Error description: This is not a QuickFit 3.0 project file.").arg(file));
            this->reading=false;
            return;
        }
    } else {
        setError(tr("Error while parsing project file '%1'!\n Error description: %2.").arg(file).arg(errorm));
        this->reading=false;
        return;
    }
    if (!errorOcc) {
        dataChange=false;
#ifdef DEBUG_TIMING
        time.start();
#endif
        //qDebug()<<"QFProject emits wasChanged("<<dataChange<<")";
        emit wasChanged(dataChange);
        //qDebug()<<"QFProject emits structureChanged";
        emit structureChanged();
        if (namechanged) emitPropertiesChanged();
#ifdef DEBUG_TIMING
        qDebug()<<"emited changed events     "<<time.elapsed()<<"ms"; time.start();
#endif
    }

    this->reading=false;
}

QFRawDataRecord* QFProject::addRawData(QString type, QString name, QStringList inputFiles, QMap<QString, QVariant> initParams, QStringList initParamsReadonly, QStringList inputFilesTypes, QStringList inputFilesDescriptions) {
    return addRawData(type, name, "", inputFiles, initParams, initParamsReadonly, inputFilesTypes, inputFilesDescriptions);
}

QFRawDataRecord *QFProject::addRawData(QString type, QString name, QString role, QStringList inputFiles, qfp_param_type initParams, QStringList initParamsReadonly, QStringList inputFilesTypes, QStringList inputFilesDescriptions)
{
    QString t=type.toLower();
    QFRawDataRecord* rde=NULL;
    rde=getRawDataRecordFactory()->createRecord(type, this);
    if (rde) {
        for (int i=0; i<initParams.keys().size(); i++) {
            rde->setQFProperty(initParams.keys().at(i), initParams[initParams.keys().at(i)], !initParamsReadonly.contains(initParams.keys().at(i)));
        }
        rde->setRole(role);
        rde->init(name, inputFiles, inputFilesTypes,inputFilesDescriptions);
    } else {
        setError(tr("error while trying to initialize an object for datatype '%1' in the QuickFit Project '%2'").arg(type).arg(getName()));
        return NULL;
    }


    setDataChanged();
    return rde;
}

QFEvaluationItem* QFProject::addEvaluation(QString type, QString name) {
    QString t=type.toLower();
    QFEvaluationItem* rde=NULL;
    rde=getEvaluationItemFactory()->createRecord(type, services, this);
    if (rde) {
        rde->init(name);
    } else {
        setError(tr("error while trying to initialize an object for datatype '%1' in the QuickFit Project '%2'").arg(type).arg(getName()));
        return NULL;
    }
    setDataChanged();
    return rde;
}



QStringList QFProject::getAllPropertyNames() {
    QStringList sl;
    for (int i=0; i<getRawDataCount(); i++) {
        sl.append(getRawDataByNum(i)->getPropertyNames());
    }
    sl.removeDuplicates();
    sl.sort();
    return sl;
}

template <class T1>
bool QFProject_StringPairCaseInsensitiveCompareSecond(const QPair<T1, QString> &s1, const QPair<T1, QString> &s2) {
    return s1.second.toLower() < s2.second.toLower();
}

template <class T1>
bool QFProject_StringPairCaseInsensitiveCompare(const QPair<QString, T1> &s1, const QPair<QString, T1> &s2) {
    return s1.first.toLower() < s2.first.toLower();
}

QList<QPair<QString, QString> > QFProject::rdrCalcMatchingResultsNamesAndLabels(const QString& evalFilter, const QString& groupFilter) const {
    QList<QPair<QString, QString> > list, listp;
    QStringList l;
    QRegExp rx(evalFilter);
    rx.setPatternSyntax(QRegExp::Wildcard);
    QRegExp rx1(groupFilter);
    rx1.setPatternSyntax(QRegExp::Wildcard);
    QMapIterator<int, QFRawDataRecord*> i(rawData);
    // iterate over all raw data records
    while (i.hasNext()) {
        i.next();
        // iterate over all evaluations that have save a result in the current rdr
        for (int e=0; e<i.value()->resultsGetEvaluationCount(); e++) {
            QString evalname=i.value()->resultsGetEvaluationName(e);

            // futher look into an evaluation only if it matches evalFilter
            if (rx.exactMatch(evalname)) {
                for (int r=0; r<i.value()->resultsGetCount(evalname); r++) {
                    QString rname=i.value()->resultsGetResultName(evalname, r);
                    QString group=i.value()->resultsGetGroup(evalname, rname);
                    QString label=i.value()->resultsGetLabel(evalname, rname);
                    if (!l.contains(rname) && rx1.exactMatch(group)) {
                        l.append(rname);
                        if (i.value()->resultsGetSortPriority(evalname, rname)) {
                            if (group.isEmpty()) listp.append(qMakePair(label, rname));
                            else listp.append(qMakePair(group+QString(": ")+label, rname));
                        } else {
                            if (group.isEmpty()) list.append(qMakePair(label, rname));
                            else list.append(qMakePair(group+QString(": ")+label, rname));
                        }
                    }
                }
            }
        }
    }

    if (list.size()>0) {
        qSort(list.begin(), list.end(), QFProject_StringPairCaseInsensitiveCompareSecond<QString>);
    }
    if (listp.size()>0) {
        qSort(listp.begin(), listp.end(), QFProject_StringPairCaseInsensitiveCompareSecond<QString>);
    }
    listp.append(list);
    return listp;
}

QStringList QFProject::rdrCalcMatchingResultsNames(const QString& evalFilter, const QString& groupFilter) const {
    QStringList l, lp;
    QRegExp rx(evalFilter);
    rx.setPatternSyntax(QRegExp::Wildcard);
    QRegExp rx1(groupFilter);
    rx1.setPatternSyntax(QRegExp::Wildcard);
    QMapIterator<int, QFRawDataRecord*> i(rawData);
    // iterate over all raw data records
    while (i.hasNext()) {
        i.next();
        // iterate over all evaluations that have save a result in the current rdr
        for (int e=0; e<i.value()->resultsGetEvaluationCount(); e++) {
            QString evalname=i.value()->resultsGetEvaluationName(e);
            // futher look into an evaluation only if it matches evalFilter
            if (rx.exactMatch(evalname)) {
                for (int r=0; r<i.value()->resultsGetCount(evalname); r++) {
                    QString rname=i.value()->resultsGetResultName(evalname, r);
                    QString group=i.value()->resultsGetGroup(evalname, rname);
                    if (!l.contains(rname) && !lp.contains(rname) && rx1.exactMatch(group)) {
                        if (i.value()->resultsGetSortPriority(evalname, rname)) lp.append(rname);
                        else l.append(rname);
                    }
                }
            }
        }
    }

    if (l.size()>0) l.sort();
    if (lp.size()>0) lp.sort();
    lp.append(l);
    return lp;
}

bool QFProject::rdrResultsSaveToCSV(const QString& evalFilter, QString filename, bool vectorsToAvg, QChar separator, QChar decimalPoint, QChar stringDelimiter) {
    QString sdel=stringDelimiter;
    QString dp=decimalPoint;
    QList<QPair<QString,QString> > colnames=rdrCalcMatchingResultsNamesAndLabels(evalFilter);
    QList<QPair<QPointer<QFRawDataRecord>, QString> > records=rdrCalcMatchingResults(evalFilter);
    QStringList header, data;
    header.append(sdel+tr("file")+sdel);
    header.append("");
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    for (int i=0; i<records.size(); i++) data.append(sdel+records[i].first->getName()+": "+records[i].second+sdel);


    QMap<int, int> subcolumns;
    for (int c=0; c<colnames.size(); c++) {
        subcolumns[c]=1;
        if (!vectorsToAvg) {
            for (int r=0; r<records.size(); r++) {
                QFRawDataRecord* record=records[r].first;
                QString evalname=records[r].second;
                if (record) {
                    if (record->resultsExists(evalname, colnames[c].second)) {
                        switch(record->resultsGet(evalname, colnames[c].second).type) {
                            case QFRawDataRecord::qfrdreNumberError:
                            case QFRawDataRecord::qfrdreNumber:
                            case QFRawDataRecord::qfrdreInteger:
                            case QFRawDataRecord::qfrdreBoolean:
                            case QFRawDataRecord::qfrdreString: subcolumns[c]=qMax(subcolumns[c], 1); break;
                            case QFRawDataRecord::qfrdreNumberErrorVector:
                            case QFRawDataRecord::qfrdreNumberErrorMatrix:
                            case QFRawDataRecord::qfrdreNumberVector:
                            case QFRawDataRecord::qfrdreNumberMatrix:  subcolumns[c]=qMax(subcolumns[c], record->resultsGet(evalname, colnames[c].second).dvec.size()); break;
                            case QFRawDataRecord::qfrdreIntegerVector:
                            case QFRawDataRecord::qfrdreIntegerMatrix:  subcolumns[c]=qMax(subcolumns[c], record->resultsGet(evalname, colnames[c].second).ivec.size()); break;
                            case QFRawDataRecord::qfrdreBooleanVector:
                            case QFRawDataRecord::qfrdreBooleanMatrix: subcolumns[c]=qMax(subcolumns[c], record->resultsGet(evalname, colnames[c].second).bvec.size()); break;
                            case QFRawDataRecord::qfrdreStringVector:
                            case QFRawDataRecord::qfrdreStringMatrix: subcolumns[c]=qMax(subcolumns[c], record->resultsGet(evalname, colnames[c].second).svec.size()); break;
                            default: subcolumns[c]=qMax(subcolumns[c], 1); break;
                        }
                    }
                }
            }
        }
    }


    for (int c=0; c<colnames.size(); c++) {
        header[0] += separator+sdel+colnames[c].first+sdel;
        header[1] += separator+sdel+tr("value")+sdel;
        bool hasError=false;
        int hcols;
        for (int r=0; r<records.size(); r++) {
            QFRawDataRecord* record=records[r].first;
            QString evalname=records[r].second;
            QString dat="";
            int colcnt=0;
            if (record) {
                if (record->resultsExists(evalname, colnames[c].second)) {
                    switch(record->resultsGet(evalname, colnames[c].second).type) {
                        case QFRawDataRecord::qfrdreNumber: dat=doubleToQString(record->resultsGetAsDouble(evalname, colnames[c].second), 15, 'g', decimalPoint); break;
                        case QFRawDataRecord::qfrdreNumberVector:
                        case QFRawDataRecord::qfrdreNumberMatrix: {
                            QVector<double> d=record->resultsGetAsDoubleList(evalname, colnames[c].second);
                            if (!vectorsToAvg) {
                                for (int it=0; it<d.size(); it++) {
                                    if (it>0) dat+=separator;
                                    dat+=doubleToQString(d[it], 15, 'g', decimalPoint);
                                }
                                colcnt=d.size();
                            } else {
                                dat+=doubleToQString(qfstatisticsAverage(d), 15, 'g', decimalPoint);
                            }
                        } break;
                        case QFRawDataRecord::qfrdreNumberErrorVector:
                        case QFRawDataRecord::qfrdreNumberErrorMatrix: {
                            QVector<double> d=record->resultsGetAsDoubleList(evalname, colnames[c].second);
                            if (!vectorsToAvg) {
                                for (int it=0; it<d.size(); it++) {
                                    if (it>0) dat+=separator;
                                    dat+=doubleToQString(d[it], 15, 'g', decimalPoint);
                                }
                                colcnt=d.size();
                            } else {
                                dat+=doubleToQString(qfstatisticsAverage(d), 15, 'g', decimalPoint);
                            }
                            hasError=true;
                        } break;
                        case QFRawDataRecord::qfrdreNumberError: dat=doubleToQString(record->resultsGetAsDouble(evalname, colnames[c].second), 15, 'g', decimalPoint); hasError=true; break;
                        case QFRawDataRecord::qfrdreInteger: dat=loc.toString((qlonglong)record->resultsGetAsInteger(evalname, colnames[c].second)); break;
                        case QFRawDataRecord::qfrdreIntegerVector:
                        case QFRawDataRecord::qfrdreIntegerMatrix:
                        case QFRawDataRecord::qfrdreBooleanVector:
                        case QFRawDataRecord::qfrdreBooleanMatrix: {
                            QVector<qlonglong> d=record->resultsGetAsIntegerList(evalname, colnames[c].second);
                            if (!vectorsToAvg) {
                                for (int it=0; it<d.size(); it++) {
                                    if (it>0) dat+=separator;
                                    dat+=loc.toString(d[it]);
                                }
                                colcnt=d.size();
                            } else {
                                dat+=loc.toString(qfstatisticsAverage(d), 15);
                            }

                        } break;
                        case QFRawDataRecord::qfrdreBoolean: dat=(record->resultsGetAsBoolean(evalname, colnames[c].second))?QString("1"):QString("0"); break;
                        case QFRawDataRecord::qfrdreString: dat=stringDelimiter+record->resultsGetAsString(evalname, colnames[c].second).replace(separator, "_").replace('\t', " ").replace('\n', "\\n").replace('\r', "\\r").replace(stringDelimiter, "_")+stringDelimiter; break;
                        case QFRawDataRecord::qfrdreStringVector:
                        case QFRawDataRecord::qfrdreStringMatrix: {
                            QStringList d=record->resultsGetAsStringList(evalname, colnames[c].second);
                            if (!vectorsToAvg) {
                                for (int it=0; it<d.size(); it++) {
                                    if (it>0) dat+=separator;
                                    dat+=stringDelimiter+d[it].replace(separator, "_").replace('\t', " ").replace('\n', "\\n").replace('\r', "\\r").replace(stringDelimiter, "_")+stringDelimiter;
                                }
                                colcnt=d.size();
                            }
                        } break;
                        default: dat=""; break;
                    }
                }
            }
            data[r]+=separator+dat;
            colcnt=qMax(1,colcnt);
            int addcols=subcolumns[c]-colcnt;
            for (int ac=0; ac<addcols; ac++) {
                data[r]+=separator;
            }
            hcols+=addcols;
        }
        for (int ac=0; ac<subcolumns[c]-1; ac++) {
            header[0]+=separator;
            header[1]+=separator;
        }

        hcols=0;
        if (hasError) {
            header[0] += separator;
            header[1] += separator+sdel+tr("error")+sdel;
            for (int r=0; r<records.size(); r++) {
                QFRawDataRecord* record=records[r].first;
                QString evalname=records[r].second;
                QString dat="";
                int colcnt=1;
                if (record) {
                    if (record->resultsExists(evalname, colnames[c].second)) {
                        switch (record->resultsGet(evalname, colnames[c].second).type) {
                            case QFRawDataRecord::qfrdreNumberError:
                                dat=doubleToQString(record->resultsGetErrorAsDouble(evalname, colnames[c].second), 15, 'g', decimalPoint);
                                break;
                            case QFRawDataRecord::qfrdreNumberErrorVector:
                            case QFRawDataRecord::qfrdreNumberErrorMatrix: {
                                QVector<double> d=record->resultsGetErrorAsDoubleList(evalname, colnames[c].second);
                                if (!vectorsToAvg) {
                                    for (int it=0; it<d.size(); it++) {
                                        if (it>0) dat+=separator;
                                        dat+=doubleToQString(d[it], 15, 'g', decimalPoint);
                                    }
                                    colcnt=d.size();
                                } else {
                                    dat+=doubleToQString(qfstatisticsAverage(d), 15, 'g', decimalPoint);
                                }
                            } break;
                            default: break;
                        }
                    }
                }
                data[r]+=separator+dat;
                colcnt=qMax(1,colcnt);
                int addcols=subcolumns[c]-colcnt;
                for (int ac=0; ac<addcols; ac++) {
                    data[r]+=separator;
                }
                //hcols+=addcols;
            }

            for (int ac=0; ac<subcolumns[c]-1; ac++) {
                header[0]+=separator;
                header[1]+=separator;
            }
        }

    }

    QFile of(filename);
    if (of.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&of);
        QTextCodec* c=QTextCodec::codecForName("ISO-8859-1");
        if (c==NULL) c=QTextCodec::codecForCStrings();
        if (c==NULL) c=QTextCodec::codecForLocale();
        out.setCodec(c);
        for (int i=0; i<header.size(); i++) out<<header[i]<<"\n";
        for (int i=0; i<data.size(); i++) out<<data[i]<<"\n";
    } else { return false; }
    return true;
}

bool QFProject::rdrResultsSaveToSYLK(const QString& evalFilter, QString filename, bool vectorsToAvg, bool flipTable) {
    QFile of(filename);
    if (of.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&of);
        QTextCodec* c=QTextCodec::codecForName("ISO-8859-1");
        if (c==NULL) c=QTextCodec::codecForCStrings();
        if (c==NULL) c=QTextCodec::codecForLocale();
        out.setCodec(c);
        out<<"ID;P\n";



        QChar stringDelimiter='"';
        QList<QPair<QString,QString> > colnames=rdrCalcMatchingResultsNamesAndLabels(evalFilter);
        QList<QPair<QPointer<QFRawDataRecord>, QString> > records=rdrCalcMatchingResults(evalFilter);
        QLocale loc=QLocale::c();
        loc.setNumberOptions(QLocale::OmitGroupSeparator);


        QMap<int, int> subcolumns;
        for (int c=0; c<colnames.size(); c++) {
            subcolumns[c]=1;
            if (!vectorsToAvg) {
                for (int r=0; r<records.size(); r++) {
                    QFRawDataRecord* record=records[r].first;
                    QString evalname=records[r].second;
                    QString dat="";
                    if (record) {
                        if (record->resultsExists(evalname, colnames[c].second)) {
                            switch(record->resultsGet(evalname, colnames[c].second).type) {
                                case QFRawDataRecord::qfrdreNumberError:
                                case QFRawDataRecord::qfrdreNumber:
                                case QFRawDataRecord::qfrdreInteger:
                                case QFRawDataRecord::qfrdreBoolean:
                                case QFRawDataRecord::qfrdreString: subcolumns[c]=qMax(subcolumns[c], 1); break;
                                case QFRawDataRecord::qfrdreNumberErrorVector:
                                case QFRawDataRecord::qfrdreNumberErrorMatrix:
                                case QFRawDataRecord::qfrdreNumberVector:
                                case QFRawDataRecord::qfrdreNumberMatrix:  subcolumns[c]=qMax(subcolumns[c], record->resultsGet(evalname, colnames[c].second).dvec.size()); break;
                                case QFRawDataRecord::qfrdreIntegerVector:
                                case QFRawDataRecord::qfrdreIntegerMatrix:  subcolumns[c]=qMax(subcolumns[c], record->resultsGet(evalname, colnames[c].second).ivec.size()); break;
                                case QFRawDataRecord::qfrdreBooleanVector:
                                case QFRawDataRecord::qfrdreBooleanMatrix: subcolumns[c]=qMax(subcolumns[c], record->resultsGet(evalname, colnames[c].second).bvec.size()); break;
                                case QFRawDataRecord::qfrdreStringVector:
                                case QFRawDataRecord::qfrdreStringMatrix: subcolumns[c]=qMax(subcolumns[c], record->resultsGet(evalname, colnames[c].second).svec.size()); break;
                                default: subcolumns[c]=qMax(subcolumns[c], 1); break;
                            }
                        }
                    }
                }
            }
        }


        loc.setNumberOptions(QLocale::OmitGroupSeparator);
        for (int i=0; i<records.size(); i++) {
            if (flipTable)
                out<<QString("F;SDS;Y%2;X%1\nC;Y%2;X%1;K\"%3\"\n").arg(i+3).arg(1).arg(QString(records[i].first->getName()+": "+records[i].second).replace(stringDelimiter, "\\"+QString(stringDelimiter)).replace('\n', "\\n").replace('\r', "\\r").replace(';', ",").replace(stringDelimiter, "_"));
            else
                out<<QString("F;SDS;Y%1;X%2\nC;Y%1;X%2;K\"%3\"\n").arg(i+3).arg(1).arg(QString(records[i].first->getName()+": "+records[i].second).replace(stringDelimiter, "\\"+QString(stringDelimiter)).replace('\n', "\\n").replace('\r', "\\r").replace(';', ",").replace(stringDelimiter, "_"));
        }
        int col=2;
        for (int c=0; c<colnames.size(); c++) {
            if (flipTable) {
                out<<QString("F;SDS;Y%2;X%1\nC;Y%2;X%1;K\"%3\"\n").arg(1).arg(col).arg(colnames[c].first);
                out<<QString("F;SDIS;Y%2;X%1\nC;Y%2;X%1;K\"%3\"\n").arg(2).arg(col).arg(tr("value"));
            } else {
                out<<QString("F;SDS;Y%1;X%2\nC;Y%1;X%2;K\"%3\"\n").arg(1).arg(col).arg(colnames[c].first);
                out<<QString("F;SDIS;Y%1;X%2\nC;Y%1;X%2;K\"%3\"\n").arg(2).arg(col).arg(tr("value"));
            }
            bool hasError=false;
            for (int r=0; r<records.size(); r++) {
                QFRawDataRecord* record=records[r].first;
                QString evalname=records[r].second;
                QStringList dat;
                int colcnt=1;
                if (record) {
                    if (record->resultsExists(evalname, colnames[c].second)) {
                        switch(record->resultsGet(evalname, colnames[c].second).type) {
                            case QFRawDataRecord::qfrdreNumber: dat<<CDoubleToQString(record->resultsGetAsDouble(evalname, colnames[c].second)); break;
                            case QFRawDataRecord::qfrdreNumberVector:
                            case QFRawDataRecord::qfrdreNumberMatrix: {
                                QVector<double> d=record->resultsGetAsDoubleList(evalname, colnames[c].second);
                                if (!vectorsToAvg) {
                                    for (int it=0; it<d.size(); it++) {
                                        dat<<CDoubleToQString(d[it]);
                                    }
                                    colcnt=d.size();
                                } else {
                                    dat<<CDoubleToQString(qfstatisticsAverage(d));
                                }
                            } break;
                            case QFRawDataRecord::qfrdreNumberErrorVector:
                            case QFRawDataRecord::qfrdreNumberErrorMatrix: {
                                QVector<double> d=record->resultsGetAsDoubleList(evalname, colnames[c].second);
                                if (!vectorsToAvg) {
                                    for (int it=0; it<d.size(); it++) {
                                        dat<<CDoubleToQString(d[it]);
                                    }
                                    colcnt=d.size();
                                } else {
                                    dat<<CDoubleToQString(qfstatisticsAverage(d));
                                }
                                hasError=true;
                            } break;
                            case QFRawDataRecord::qfrdreNumberError: dat<<CDoubleToQString(record->resultsGetAsDouble(evalname, colnames[c].second)); hasError=true; break;
                            case QFRawDataRecord::qfrdreInteger: dat<<loc.toString((qlonglong)record->resultsGetAsInteger(evalname, colnames[c].second)); break;
                            case QFRawDataRecord::qfrdreIntegerVector:
                            case QFRawDataRecord::qfrdreIntegerMatrix:
                            case QFRawDataRecord::qfrdreBooleanVector:
                            case QFRawDataRecord::qfrdreBooleanMatrix: {
                                QVector<qlonglong> d=record->resultsGetAsIntegerList(evalname, colnames[c].second);
                                if (!vectorsToAvg) {
                                    for (int it=0; it<d.size(); it++) {
                                        dat<<loc.toString(d[it]);
                                    }
                                    colcnt=d.size();
                                } else {
                                    dat<<CDoubleToQString(qfstatisticsAverage(d));
                                }
                            } break;
                            case QFRawDataRecord::qfrdreBoolean: if (record->resultsGetAsBoolean(evalname, colnames[c].second)) dat<<QString("1"); else dat<<QString("0"); break;
                            case QFRawDataRecord::qfrdreString: dat<<(stringDelimiter+record->resultsGetAsString(evalname, colnames[c].second).replace('\t', " ").replace('\n', "\\n").replace('\r', "\\r").replace(';', ",").replace(stringDelimiter, "_")+stringDelimiter); break;
                            case QFRawDataRecord::qfrdreStringVector:
                            case QFRawDataRecord::qfrdreStringMatrix: {
                                QStringList d=record->resultsGetAsStringList(evalname, colnames[c].second);
                                if (!vectorsToAvg) {
                                    for (int it=0; it<d.size(); it++) {
                                        dat<<(stringDelimiter+d[it].replace('\t', " ").replace('\n', "\\n").replace('\r', "\\r").replace(';', ",").replace(stringDelimiter, "_")+stringDelimiter);
                                    }
                                    colcnt=d.size();
                                }
                            } break;
                            default: break;
                        }
                    }
                }
                for (int i=0; i<dat.size(); i++) {
                    if (flipTable) {
                        out<<QString("C;X%2;Y%1;K%3\n").arg(col+i).arg(r+3).arg(dat[i]);
                    } else {
                        out<<QString("C;X%1;Y%2;K%3\n").arg(col+i).arg(r+3).arg(dat[i]);
                    }
                    //col++;
                }
                //if ((subcolumns[c]-colcnt)>0) col+=(subcolumns[c]-colcnt);
            }
            col+=subcolumns[c];
            if (hasError) {
                if (flipTable) {
                    out<<QString("F;SDIS;Y%2;X%1\nC;Y%2;X%1;K\"%3\"\n").arg(2).arg(col).arg(tr("error"));
                } else {
                    out<<QString("F;SDIS;Y%1;X%2\nC;Y%1;X%2;K\"%3\"\n").arg(2).arg(col).arg(tr("error"));
                }
                for (int r=0; r<records.size(); r++) {
                    QFRawDataRecord* record=records[r].first;
                    QString evalname=records[r].second;
                    QStringList dat;
                    int colcnt=1;
                    if (record) {
                        if (record->resultsExists(evalname, colnames[c].second)) {
                            switch (record->resultsGet(evalname, colnames[c].second).type) {
                                case QFRawDataRecord::qfrdreNumberError:
                                    dat<<CDoubleToQString(record->resultsGetErrorAsDouble(evalname, colnames[c].second));
                                    break;
                                case QFRawDataRecord::qfrdreNumberErrorVector:
                                case QFRawDataRecord::qfrdreNumberErrorMatrix: {
                                    QVector<double> d=record->resultsGetErrorAsDoubleList(evalname, colnames[c].second);
                                    if (!vectorsToAvg) {
                                        for (int it=0; it<d.size(); it++) {
                                            dat<<CDoubleToQString(d[it]);
                                        }
                                        colcnt=d.size();
                                    } else {
                                        dat<<CDoubleToQString(qfstatisticsAverage(d));
                                    }
                                } break;
                                    default: break;
                            }
                        }
                    }
                    //if (!dat.isEmpty()) out<<QString("C;X%1;Y%2;K%3\n").arg(col).arg(r+3).arg(dat);
                    for (int i=0; i<dat.size(); i++) {
                        if (flipTable) {
                            out<<QString("C;X%2;Y%1;K%3\n").arg(col+i).arg(r+3).arg(dat[i]);
                        } else {
                            out<<QString("C;X%1;Y%2;K%3\n").arg(col+i).arg(r+3).arg(dat[i]);
                        }
                        //col++;
                    }
                    //if ((subcolumns[c]-colcnt)>0) col+=(subcolumns[c]-colcnt);
                }
            }
        }

        if (flipTable) {
            out<<"F;R1;SDSB\n";
            out<<"F;C1;SDS\n";
            out<<"F;C2;SDSR\n";
        } else {
            out<<"F;C1;SDSR\n";
            out<<"F;R1;SDS\n";
            out<<"F;R2;SDSB\n";
        }
        out<<"E\n";

    } else { return false; }
    return true;
}

QFRawDataRecordFactory *QFProject::getRawDataRecordFactory()
{
    return rdrFactory;
}

QFEvaluationItemFactory *QFProject::getEvaluationItemFactory()
{
    return evalFactory;
}



bool rdrCalcMatchingResults_compare(const QPair<QPointer<QFRawDataRecord>, QString> &s1, const QPair<QPointer<QFRawDataRecord>, QString> &s2) {
    QString ss1=s1.first->getName()+": "+s1.second;
    QString ss2=s2.first->getName()+": "+s2.second;
    return ss1.toLower() < ss2.toLower();
}


QList<QPair<QPointer<QFRawDataRecord>, QString> > QFProject::rdrCalcMatchingResults(const QString& evalFilter) const {
    QList<QPair<QPointer<QFRawDataRecord>, QString> > l;

    QRegExp rx(evalFilter);
    rx.setPatternSyntax(QRegExp::Wildcard);
    QMapIterator<int, QFRawDataRecord*> i(rawData);
    // iterate over all raw data records
    while (i.hasNext()) {
        i.next();
        // iterate over all evaluations that have save a result in the current rdr
        for (int e=0; e<i.value()->resultsGetEvaluationCount(); e++) {
            QString evalname=i.value()->resultsGetEvaluationName(e);
            // futher look into an evaluation only if it matches evalFilter
            if (rx.exactMatch(evalname)) {
                QPointer<QFRawDataRecord> po(i.value());
                QPair<QPointer<QFRawDataRecord>, QString> pa=qMakePair(po, evalname);
                l.append(pa);
            }
        }
    }

    if (l.size()>0) {
        //qDebug()<<"rdrCalcMatchingResults: qSort()";
        //for (int i=0; i<l.size(); i++) qDebug()<<"  "<<i<<l[i].second;
        qSort(l.begin(), l.end(), rdrCalcMatchingResults_compare );
        //for (int i=0; i<l.size(); i++) qDebug()<<"  "<<i<<l[i].second;
    }

    return l;
}


void QFProject::setDataChanged() {
    if (dataChange) return;
    dataChange=true;
    //qDebug()<<"QFProject emit wasChanged(dataChange="<<dataChange<<")";
    if (m_signalsEnabled) emit wasChanged(dataChange);
}

void QFProject::setStructureChanged() {
    emitStructureChanged();
}

void QFProject::log_text(const QString &message) const
{
    if (services) services->log_text(message);
}

void QFProject::log_warning(const QString &message) const
{
    if (services) services->log_warning(message);
}

void QFProject::log_error(const QString &message) const
{
    if (services) services->log_error(message);
};


void QFProject::setError(QString description) {
    errorOcc=true;
    errorDesc=description;
    emit errorOccured(description);
}

void QFProject::emitPropertiesChanged() {
    //qDebug()<<"QFProject emit propertiesChanged()";
    if (m_signalsEnabled) emit propertiesChanged();
};

void QFProject::emitStructureChanged() {
    setDataChanged();
    //qDebug()<<"QFProject emit structureChanged()";
    if (m_signalsEnabled) emit structureChanged();
};

void QFProject::setPropertiesError(QString message) {
    setError(message);
};

void QFProject::setName(const QString& n) {
    if (name!=n) {
        name=n;
        emitPropertiesChanged();
    }
}

QString QFProject::getDescription()const  {
    return description;
}

void QFProject::setDescription(const QString& d) {
    if (description!=d) {
        description=d;
        emitPropertiesChanged();
    }
}

QString QFProject::getCreator()const  {
    return creator;
}

void QFProject::setCreator(const QString& c) {
    if (creator!=c) {
        creator=c;
        emitPropertiesChanged();
    }
}

QString QFProject::getFile()const  {
    return file;
}

int QFProject::getRawDataCount()const  {
    return rawData.size();
}


int QFProject::getEvaluationCount()const  {
    return evaluations.size();
}

bool QFProject::rawDataIDExists(int ID)const  {
    return rawData.contains(ID);
}

bool QFProject::evaluationIDExists(int ID)const  {
    return evaluations.contains(ID);
}

bool QFProject::rawDataExists(QFRawDataRecord* rec) const  {
    return rawData.values().contains(rec);
}

bool QFProject::evaluationExists(QFEvaluationItem* rec) const  {
    return evaluations.values().contains(rec);
}

int QFProject::getRawDataIndex(QFRawDataRecord* rec) const  {
    return rawData.values().indexOf(rec);
}

bool QFProject::getEvaluationIndex(QFEvaluationItem* rec) const  {
    return evaluations.values().indexOf(rec);
}

QFRawDataRecord* QFProject::getRawDataByID(int ID) {
    if (rawDataIDExists(ID)) return rawData[ID];
    return NULL;
}

QFRawDataRecord* QFProject::getRawDataByNum(int i) const {
    /*QList<int> keys=rawData.keys();
    if ((i>=keys.size()) || (i<0)) return NULL;
    int ID=keys.at(i);
    if (rawData.contains(ID)) return rawData[ID];
    else return NULL;*/
    if ((i>=rawDataOrder.size()) || (i<0) || (rawDataOrder.size()<=0)) return NULL;
    int ID=rawDataOrder.value(i, -1);
    if (rawData.contains(ID)) return rawData[ID];
    else return NULL;
}


QFEvaluationItem* QFProject::getEvaluationByID(int ID) const {
    if (evaluationIDExists(ID)) return evaluations[ID];
    return NULL;
}

QFEvaluationItem* QFProject::getEvaluationByNum(int i) const {
    /*QList<int> keys=evaluations.keys();
    if ((i>=keys.size()) || (i<0)) return NULL;
    int ID=keys.at(i);
    if (evaluations.contains(ID)) { return evaluations[ID]; }
    else return NULL;*/
    if ((i>=evaluationsOrder.size()) || (i<0) || (evaluationsOrder.size()<=0)) return NULL;
    int ID=evaluationsOrder.value(i, -1);
    if (evaluations.contains(ID)) return evaluations[ID];
    else return NULL;

}

bool QFProject::checkID(int ID)const {
    return !IDs.contains(ID);
}

QString QFProject::getName()const {
    return name;
}

void QFProject::moveRawDataRecordToPosition(QFRawDataRecord *rec, int positionIndex)
{
    if (!rec) return;
    int ID=rec->getID();
    int idx=rawDataOrder.indexOf(ID);
    if (idx<0) return;
    rawDataOrder.move(idx, positionIndex);
    emitStructureChanged();
}

void QFProject::moveRawDataRecord(int recID, int recID2, QFProject::RecordInsertModes moveMode)
{
    moveRawDataRecord(getRawDataByID(recID), getRawDataByID(recID2), moveMode);
}

void QFProject::moveRawDataRecordUp(QFRawDataRecord *rec)
{
    if (!rec) return;
    int ID=rec->getID();
    int pos=rawDataOrder.indexOf(ID);
    if (pos<=0) return;
    moveRawDataRecordToPosition(rec, pos-1);
}

void QFProject::moveRawDataRecordToPosition(int recID, int positionIndex)
{
    moveRawDataRecordToPosition(getRawDataByID(recID), positionIndex);
}

void QFProject::moveRawDataRecord(QFRawDataRecord *rec, QFRawDataRecord *rec2, QFProject::RecordInsertModes moveMode)
{
    if (!rec || !rec2) return;
    int ID=rec->getID();
    int pos=rawDataOrder.indexOf(ID);
    if (pos<=0 || pos>rawDataOrder.size()) return;
    int ID2=rec->getID();
    int pos2=rawDataOrder.indexOf(ID2);
    if (pos2<=0 || pos2>rawDataOrder.size()) return;
    if (moveMode==QFProject::swapRecods) {
        rawDataOrder.swap(pos, pos2);
    } else if (moveMode==QFProject::insertRecordBefore) {
        rawDataOrder.move(pos, pos2);
    } else if (moveMode==QFProject::insertRecordBefore) {
        if (pos2+1<rawDataOrder.size()) rawDataOrder.move(pos, pos2+1);
    }
    emitStructureChanged();
}

void QFProject::moveRawDataRecordUp(int recID)
{
    moveRawDataRecordUp(getRawDataByID(recID));
}

void QFProject::moveRawDataRecordDown(QFRawDataRecord *rec)
{
    if (!rec) return;
    int ID=rec->getID();
    int pos=rawDataOrder.indexOf(ID);
    if (pos>=rawDataOrder.size()-1) return;
    moveRawDataRecordToPosition(rec, pos+1);
}

void QFProject::moveRawDataRecordDown(int recID)
{
    moveRawDataRecordDown(getRawDataByID(recID));
}

void QFProject::moveEvaluationToPosition(QFEvaluationItem *rec, int positionIndex)
{
    if (!rec) return;
    int ID=rec->getID();
    int idx=evaluationsOrder.indexOf(ID);
    if (idx<0) return;
    evaluationsOrder.move(idx, positionIndex);
    emitStructureChanged();
}

void QFProject::moveEvaluationToPosition(int recID, int positionIndex)
{
    moveEvaluationToPosition(getEvaluationByID(recID), positionIndex);
}

void QFProject::moveEvaluation(QFEvaluationItem *rec, QFEvaluationItem *rec2, QFProject::RecordInsertModes moveMode)
{
    if (!rec || !rec2) return;
    int ID=rec->getID();
    int pos=evaluationsOrder.indexOf(ID);
    if (pos<=0 || pos>evaluationsOrder.size()) return;
    int ID2=rec->getID();
    int pos2=evaluationsOrder.indexOf(ID2);
    if (pos2<=0 || pos2>evaluationsOrder.size()) return;
    if (moveMode==QFProject::swapRecods) {
        evaluationsOrder.swap(pos, pos2);
    } else if (moveMode==QFProject::insertRecordBefore) {
        evaluationsOrder.move(pos, pos2);
    } else if (moveMode==QFProject::insertRecordBefore) {
        if (pos2+1<evaluationsOrder.size()) evaluationsOrder.move(pos, pos2+1);
    }
    emitStructureChanged();
}

void QFProject::moveEvaluation(int recID, int recID2, QFProject::RecordInsertModes moveMode)
{
    moveEvaluation(getEvaluationByID(recID), getEvaluationByID(recID2), moveMode);
}

void QFProject::moveEvaluationUp(QFEvaluationItem *rec)
{
    if (!rec) return;
    int ID=rec->getID();
    int pos=evaluationsOrder.indexOf(ID);
    if (pos<=0) return;
    moveEvaluationToPosition(rec, pos-1);
}

void QFProject::moveEvaluationUp(int recID)
{
    moveEvaluationUp(getEvaluationByID(recID));
}

void QFProject::moveEvaluationDown(QFEvaluationItem *rec)
{
    if (!rec) return;
    int ID=rec->getID();
    int pos=evaluationsOrder.indexOf(ID);
    if (pos>=evaluationsOrder.size()-1) return;
    moveEvaluationToPosition(rec, pos+1);
}

void QFProject::moveEvaluationDown(int recID)
{
    moveEvaluationDown(getEvaluationByID(recID));
}

bool QFProject::areSignalsEnabled() const
{
    return m_signalsEnabled;
}

bool QFProject::isDummy() const
{
    return m_dummy;
}


