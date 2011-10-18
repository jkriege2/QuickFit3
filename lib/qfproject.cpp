#include "qfproject.h"
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "qfprojectrawdatamodel.h"
#include "qfprojecttreemodel.h"
#include "qfevaluationitemfactory.h"
#include "qfrawdatarecordfactory.h"
#include "qfpluginservices.h"
#include "../version.h"

#include <QTemporaryFile>


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

    QMapIterator<int, QFEvaluationItem*> j(evaluations);
    while (j.hasNext()) {
        j.next();
        //emit recordAboutToBeDeleted(i.value());
        delete j.value();
    }
    evaluations.clear();
    //std::cout<<"deleting QFProject ... OK\n";
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
    if ( (!current) || (rawData.size()<=0) ) return NULL;
    int i=rawData.values().indexOf(current);
    if ((i+1>0) && (i+1<rawData.size())) return rawData.values().at(i+1);
    if (i==rawData.size()-1) return rawData.values().at(0);
    return NULL;
}

QFRawDataRecord* QFProject::getPreviousRawData(QFRawDataRecord* current) {
    if ( (!current) || (rawData.size()<=0) ) return NULL;
    int i=rawData.values().indexOf(current);
    if ((i-1>=0) && (i-1<rawData.size()) ) return rawData.values().at(i-1);
    if (i==0) return rawData.values().at(rawData.size()-1);
    return NULL;
}

QFRawDataRecord* QFProject::getNextRawDataOfSameType(QFRawDataRecord* current) {
    if (!current || rawData.size()<=0) return NULL;
    int i=rawData.values().indexOf(current);
    QString t=current->getType();
    int cnt=rawData.size();
    for (int k=i+1; k<i+cnt; k++) {
        int l=k%cnt;
        if (rawData.values().at(l)->getType()==t) return rawData.values().at(l);
    }
    return current;
}

QFRawDataRecord* QFProject::getPreviousRawDataOfSameType(QFRawDataRecord* current) {
    if (!current || rawData.size()<=0) return NULL;
    int i=rawData.values().indexOf(current);
    QString t=current->getType();
    int cnt=rawData.size();
    for (int k=i-1+cnt; k>i; k--) {
        int l=k%cnt;
        if (rawData.values().at(l)->getType()==t) return rawData.values().at(l);
    }
    return current;
}

QFEvaluationItem* QFProject::getNextEvaluation(QFEvaluationItem* current) {
    if (!current || evaluations.size()<=0) return NULL;
    int i=evaluations.values().indexOf(current);
    if (i+1>=0 && i+1<evaluations.size()) return evaluations.values().at(i+1);
    if (i==evaluations.size()-1) return evaluations.values().at(0);
    return NULL;
}

QFEvaluationItem* QFProject::getPreviousEvaluation(QFEvaluationItem* current) {
    if (!current || evaluations.size()<=0) return NULL;
    int i=evaluations.values().indexOf(current);
    if (i-1>=0 && i-1<evaluations.size()) return evaluations.values().at(i-1);
    if (i==0) return evaluations.values().at(evaluations.size()-1);
    return NULL;
}

QFEvaluationItem* QFProject::getNextEvaluationOfSameType(QFEvaluationItem* current) {
    if (!current || evaluations.size()<=0) return NULL;
    int i=evaluations.values().indexOf(current);
    QString t=current->getType();
    int cnt=evaluations.size();
    for (int k=i+1; k<i+cnt; k++) {
        int l=k%cnt;
        if (evaluations.values().at(l)->getType()==t) return evaluations.values().at(l);
    }
    return current;
}

QFEvaluationItem* QFProject::getPreviousEvaluationOfSameType(QFEvaluationItem* current) {
    if (!current || evaluations.size()<=0) return NULL;
    int i=evaluations.values().indexOf(current);
    QString t=current->getType();
    int cnt=evaluations.size();
    for (int k=i-1+cnt; k>i; k--) {
        int l=k%cnt;
        if (evaluations.values().at(l)->getType()==t) return evaluations.values().at(l);
    }
    return current;
}

void QFProject::projectChanged() {
    dataChange=true;
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
    IDs.insert(newID);
    dataChange=true;
    connect(rec, SIGNAL(rawDataChanged()), this, SLOT(projectChanged()));
    connect(rec, SIGNAL(propertiesChanged()), this, SLOT(projectChanged()));
    connect(rec, SIGNAL(propertiesChanged()), this, SLOT(setStructureChanged()));
    connect(rec, SIGNAL(resultsChanged()), this, SLOT(projectChanged()));
    connect(rec, SIGNAL(rawDataChanged()), this, SLOT(setDataChanged()));
    connect(rec, SIGNAL(propertiesChanged()), this, SLOT(setDataChanged()));
    connect(rec, SIGNAL(resultsChanged()), this, SLOT(setDataChanged()));
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
    IDs.insert(newID);
    connect(rec, SIGNAL(resultsChanged()), this, SLOT(projectChanged()));
    connect(rec, SIGNAL(propertiesChanged()), this, SLOT(projectChanged()));
    connect(rec, SIGNAL(propertiesChanged()), this, SLOT(setStructureChanged()));
    connect(rec, SIGNAL(resultsChanged()), this, SLOT(setDataChanged()));
    connect(rec, SIGNAL(propertiesChanged()), this, SLOT(setDataChanged()));
    emitStructureChanged();
    return true;
}

void QFProject::deleteRawData(int ID) {
    if (rawDataIDExists(ID)) {
        QFRawDataRecord* rec=rawData[ID];
        emit recordAboutToBeDeleted(rec);
        rec->disconnect();
        rawData.remove(ID);
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
        IDs.remove(ID);
        delete rec;
        emitStructureChanged();
    }
}

void QFProject::writeXML(const QString& file, bool resetDataChanged) {

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
    w.setAutoFormatting(false);
    w.writeStartDocument();
    w.writeStartElement("quickfitproject");
    w.writeAttribute("quickfit_version", VERSION_FULL);
    w.writeAttribute("name", name);
    w.writeAttribute("creator", creator);
    w.writeStartElement("description");
    w.writeCDATA(description);
    w.writeEndElement();
    w.writeStartElement("properties");
    storeProperties(w);
    w.writeEndElement();
    w.writeStartElement("rawdata");
    QMapIterator<int, QFRawDataRecord*> ir(rawData);
    //for (int i=0; i<rawData.keys().size(); i++) {
    while (ir.hasNext()) {
        ir.next();
        //std::cout<<"writing rdr "<<ir.key()<<std::endl;
        //int k=rawData.keys().at(i);
        ir.value()->writeXML(w);
        //std::cout<<"   DONE!"<<std::endl;
    }
    w.writeEndElement();
    w.writeStartElement("evaluations");
    QMapIterator<int, QFEvaluationItem*> i(evaluations);
    //for (int i=0; i<evaluations.keys().size(); i++) {
    while (i.hasNext()) {
        i.next();
        //std::cout<<"writing eval "<<i.key()<<std::endl;
        //int k=evaluations.keys().at(i);
        i.value()->writeXML(w);
        //std::cout<<"   DONE!"<<std::endl;
    }
    w.writeEndElement();

    w.writeEndElement();
    w.writeEndDocument();
    if (resetDataChanged) {
        if (!errorOcc) {
            emitStructureChanged();
            if (namechanged) emit propertiesChanged();
        }
    }

    QFile f2(file+".backup");
    if (f2.exists()) f2.remove();

    QFile f1(file);
    f1.rename(file+".backup");
    f.setAutoRemove(false);
    if (!f.rename(file)) {
        setError(tr("Could no open file '%1' for output!\n Error description: %2.").arg(file).arg(f.errorString()));
    }
    /*if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setError(tr("Could no open file '%1' for output!\n Error description: %2.").arg(file).arg(f.errorString()));
        return;
    }*/
}

void QFProject::readXML(const QString& file) {
    bool namechanged=(file!=this->file);
    this->file=file;
    QFile f(file);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setError(tr("Could no open file '%1' for input!\n Error description: %2.").arg(file).arg(f.errorString()));
        return;
    }
    QDomDocument d("quickfitproject");
    QString errorm="";
    if (d.setContent(&f, &errorm)) {
        QDomElement e=d.documentElement();
        if (e.tagName()=="quickfitproject") {
            QString v=e.attribute("quickfit_version");
            if (v.toDouble()>3.0) {
                setError(tr("Error while parsing project file '%1'!\n Error description: The project file was written by a newer version of QuickFit than this one (writer version: %1).").arg(file).arg(v));
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
                    rd=rd.firstChildElement("rawdataelement");
                    while (!rd.isNull()) {
                        if (services) {
                            services->incProgress();
                            QApplication::processEvents();
                        }
                        QString t=rd.attribute("type", "invalid").toLower();
                        //std::cout<<t.toStdString()<<std::endl;
                        try {
                            QFRawDataRecord* e=getRawDataRecordFactory()->createRecord(t, this);
                            e->init(rd);
                        } catch(std::exception& E) {
                            setError(tr("Error while opening raw data element: %2").arg(E.what()));
                        }
                        rd=rd.nextSiblingElement("rawdataelement");
                    }
                }
                rd=e.firstChildElement("evaluations");
                if (!rd.isNull()) {
                    rd=rd.firstChildElement("evaluationelement");
                    while (!rd.isNull()) {
                        if (services) {
                            services->incProgress();
                            QApplication::processEvents();
                        }
                        QString t=rd.attribute("type", "invalid").toLower();

                        try {
                            QFEvaluationItem* e=getEvaluationItemFactory()->createRecord(t, services, this);
                            e->init(rd);
                        } catch(std::exception& E) {
                            setError(tr("Error while opening raw data element: %2").arg(E.what()));
                        }
                        //std::cout<<t.toStdString()<<std::endl;
                        /*if (t=="plot") {

                        } else if (t=="unknown") {
                            //new QFRawDataRecord(rd, this);
                            QFEvaluationItem* e=new QFEvaluationItem(this, true, true);
                            e->init(rd);
                        }*/
                        rd=rd.nextSiblingElement("evaluationelement");
                    }
                }
                if (services) {
                    services->setProgress(0);
                }
            }
        } else {
            setError(tr("Error while parsing project file '%1'!\n Error description: This is not a QuickFit 3.0 project file.").arg(file));
            return;
        }
    } else {
        setError(tr("Error while parsing project file '%1'!\n Error description: %2.").arg(file).arg(errorm));
        return;
    }
    if (!errorOcc) {
        dataChange=false;
        emit wasChanged(dataChange);
        emit structureChanged();
        if (namechanged) emit propertiesChanged();
    }

}

QFRawDataRecord* QFProject::addRawData(QString type, QString name, QStringList inputFiles, QMap<QString, QVariant> initParams, QStringList initParamsReadonly, QStringList inputFilesTypes) {
    QString t=type.toLower();
    QFRawDataRecord* rde=NULL;
    rde=getRawDataRecordFactory()->createRecord(type, this);
    if (rde) {
        for (int i=0; i<initParams.keys().size(); i++) {
            rde->setQFProperty(initParams.keys().at(i), initParams[initParams.keys().at(i)], !initParamsReadonly.contains(initParams.keys().at(i)));
        }
        rde->init(name, inputFiles, inputFilesTypes);
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

bool QFProject_StringPairCaseInsensitiveCompareSecond(const QPair<QString, QString> &s1, const QPair<QString, QString> &s2) {
    return s1.second.toLower() < s2.second.toLower();
}

bool QFProject_StringPairCaseInsensitiveCompare(const QPair<QString, QString> &s1, const QPair<QString, QString> &s2) {
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
        qSort(list.begin(), list.end(), QFProject_StringPairCaseInsensitiveCompareSecond);
    }
    if (listp.size()>0) {
        qSort(listp.begin(), listp.end(), QFProject_StringPairCaseInsensitiveCompareSecond);
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

bool QFProject::rdrResultsSaveToCSV(const QString& evalFilter, QString filename, QChar separator, QChar decimalPoint, QChar stringDelimiter) {
    QString sdel=stringDelimiter;
    QString dp=decimalPoint;
    QStringList colnames=rdrCalcMatchingResultsNames(evalFilter);
    QList<QPair<QPointer<QFRawDataRecord>, QString> > records=rdrCalcMatchingResults(evalFilter);
    QStringList header, data;
    header.append(sdel+tr("file")+sdel);
    header.append("");
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    for (int i=0; i<records.size(); i++) data.append(sdel+records[i].first->getName()+": "+records[i].second+sdel);

    for (int c=0; c<colnames.size(); c++) {
        header[0] += separator+sdel+colnames[c]+sdel;
        header[1] += separator+sdel+tr("value")+sdel;
        bool hasError=false;
        for (int r=0; r<records.size(); r++) {
            QFRawDataRecord* record=records[r].first;
            QString evalname=records[r].second;
            QString dat="";
            if (record) {
                if (record->resultsExists(evalname, colnames[c])) {
                    switch(record->resultsGet(evalname, colnames[c]).type) {
                        case QFRawDataRecord::qfrdreNumber: dat=doubleToQString(record->resultsGetAsDouble(evalname, colnames[c]), 15, 'g', decimalPoint); break;
                        case QFRawDataRecord::qfrdreNumberError: dat=doubleToQString(record->resultsGetAsDouble(evalname, colnames[c]), 15, 'g', decimalPoint); hasError=true; break;
                        case QFRawDataRecord::qfrdreInteger: dat=loc.toString((qlonglong)record->resultsGetAsInteger(evalname, colnames[c])); break;
                        case QFRawDataRecord::qfrdreBoolean: dat=(record->resultsGetAsBoolean(evalname, colnames[c]))?QString("1"):QString("0"); break;
                        case QFRawDataRecord::qfrdreString: dat=stringDelimiter+record->resultsGetAsString(evalname, colnames[c]).replace(separator, "_").replace('\t', " ").replace('\n', "\\n").replace('\r', "\\r").replace(stringDelimiter, "_")+stringDelimiter; break;
                        default: dat=""; break;
                    }
                }
            }
            data[r]+=separator+dat;
        }
        if (hasError) {
            header[0] += separator;
            header[1] += separator+sdel+tr("error")+sdel;
            for (int r=0; r<records.size(); r++) {
                QFRawDataRecord* record=records[r].first;
                QString evalname=records[r].second;
                QString dat="";
                if (record) {
                    if (record->resultsExists(evalname, colnames[c])) {
                        if (record->resultsGet(evalname, colnames[c]).type==QFRawDataRecord::qfrdreNumberError) {
                            dat=doubleToQString(record->resultsGetErrorAsDouble(evalname, colnames[c]), 15, 'g', decimalPoint);
                        }
                    }
                }
                data[r]+=separator+dat;
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

bool QFProject::rdrResultsSaveToSYLK(const QString& evalFilter, QString filename) {
    QFile of(filename);
    if (of.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&of);
        QTextCodec* c=QTextCodec::codecForName("ISO-8859-1");
        if (c==NULL) c=QTextCodec::codecForCStrings();
        if (c==NULL) c=QTextCodec::codecForLocale();
        out.setCodec(c);
        out<<"ID;P\n";

        QChar stringDelimiter='"';
        QChar decimalPoint='.';
        QStringList colnames=rdrCalcMatchingResultsNames(evalFilter);
        QList<QPair<QPointer<QFRawDataRecord>, QString> > records=rdrCalcMatchingResults(evalFilter);
        QLocale loc=QLocale::c();
        loc.setNumberOptions(QLocale::OmitGroupSeparator);
        for (int i=0; i<records.size(); i++) {
            out<<QString("C;Y%1;X%2;K\"%3\"\n").arg(i+3).arg(1).arg(records[i].first->getName()+": "+records[i].second);
        }
        int col=2;
        for (int c=0; c<colnames.size(); c++) {
            out<<QString("C;Y%1;X%2;K\"%3\"\n").arg(1).arg(col).arg(colnames[c]);
            out<<QString("C;Y%1;X%2;K\"%3\"\n").arg(2).arg(col).arg(tr("value"));
            bool hasError=false;
            for (int r=0; r<records.size(); r++) {
                QFRawDataRecord* record=records[r].first;
                QString evalname=records[r].second;
                QString dat="";
                if (record) {
                    if (record->resultsExists(evalname, colnames[c])) {
                        switch(record->resultsGet(evalname, colnames[c]).type) {
                            case QFRawDataRecord::qfrdreNumber: dat=doubleToQString(record->resultsGetAsDouble(evalname, colnames[c]), 15, 'g', decimalPoint); break;
                            case QFRawDataRecord::qfrdreNumberError: dat=doubleToQString(record->resultsGetAsDouble(evalname, colnames[c]), 15, 'g', decimalPoint); hasError=true; break;
                            case QFRawDataRecord::qfrdreInteger: dat=loc.toString((qlonglong)record->resultsGetAsInteger(evalname, colnames[c])); break;
                            case QFRawDataRecord::qfrdreBoolean: dat=(record->resultsGetAsBoolean(evalname, colnames[c]))?QString("1"):QString("0"); break;
                            case QFRawDataRecord::qfrdreString: dat=stringDelimiter+record->resultsGetAsString(evalname, colnames[c]).replace('\t', " ").replace('\n', "\\n").replace('\r', "\\r").replace(';', ",").replace(stringDelimiter, "_")+stringDelimiter; break;
                            default: dat=""; break;
                        }
                    }
                }
                if (!dat.isEmpty()) out<<QString("C;X%1;Y%2;N;K%3\n").arg(col).arg(r+3).arg(dat);
            }
            if (hasError) {
                col++;
                out<<QString("C;Y%1;X%2;K\"%3\"\n").arg(2).arg(col).arg(tr("error"));
                for (int r=0; r<records.size(); r++) {
                    QFRawDataRecord* record=records[r].first;
                    QString evalname=records[r].second;
                    QString dat="";
                    if (record) {
                        if (record->resultsExists(evalname, colnames[c])) {
                            if (record->resultsGet(evalname, colnames[c]).type==QFRawDataRecord::qfrdreNumberError) {
                                dat=doubleToQString(record->resultsGetErrorAsDouble(evalname, colnames[c]), 15, 'g', decimalPoint);
                            }
                        }
                    }
                    if (!dat.isEmpty()) out<<QString("C;X%1;Y%2;N;K%3\n").arg(col).arg(r+3).arg(dat);
                }
            }
            col++;
        }

    } else { return false; }
    return true;
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

    return l;
}
