/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfproject.h"
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "qfprojectrawdatamodel.h"
#include "qfprojecttreemodel.h"
#include "qfevaluationitemfactory.h"
#include "qfrawdatarecordfactory.h"
#include "qfpluginservices.h"
#include "qfversion.h"
#include "datatools.h"
#include "qflistprogressdialog.h"
#include <quagzipfile.h>
#include <quazip.h>
#include <quazipfile.h>
#include <JlCompress.h>

#include <QTemporaryFile>
#include <QElapsedTimer>
//#define DEBUG_TIMING
#undef DEBUG_TIMING
#define PROJECT_COPY_CHUNKSIZE 1024*1024*32

QFProject::QFProject(QFEvaluationItemFactory* evalFactory, QFRawDataRecordFactory* rdrFactory, QFPluginServices* services, QObject* parent):
    QObject(parent), QFProperties()
{
    dataChange=false;
    propertiesChange=false;
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
    connect(rec, SIGNAL(propertiesChanged(QString,bool)), this, SLOT(setPropertiesChanged()));
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
    connect(rec, SIGNAL(propertiesChanged(QString,bool)), this, SLOT(setPropertiesChanged()));
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

void QFProject::duplicateRawData(int ID)
{
    if (rawDataIDExists(ID)) {
        QFRawDataRecord* rec=rawData[ID];
        QString xml;
        {
            QXmlStreamWriter w(&xml);
            w.writeStartDocument();
            rec->writeXML(w, file);
            w.writeEndDocument();
        }
        QFRawDataRecord* newrec=getRawDataRecordFactory()->createRecord(rec->getType(), this);
        {
            QDomDocument doc;
            if (doc.setContent(xml)) {
                QDomElement e=doc.firstChildElement("rawdataelement");
                newrec->initNewID(e);
                QString n=tr("%1").arg(rec->getName());
                bool found=false;
                int cnt=1;
                do {
                    if (found) {
                        cnt++;
                        n=tr("%1 %2").arg(rec->getName()).arg(cnt);
                    }
                    found=false;
                    for (int i=0; i<getRawDataCount(); i++) {
                        if (getRawDataByNum(i) && getRawDataByNum(i)->getName()==n) {
                            found=true;
                            break;
                        }
                    }
                } while (found && cnt<1000);
                newrec->setName(n);
            } else {
                delete newrec;
            }
        }
        emitStructureChanged();
    }
}

void QFProject::duplicateEvaluation(int ID)
{
    if (evaluationIDExists(ID)) {
        QFEvaluationItem* rec=evaluations[ID];
        QString xml;
        {
            QXmlStreamWriter w(&xml);
            w.writeStartDocument();
            rec->writeXML(w, file);
            w.writeEndDocument();
        }
        QFEvaluationItem* newrec=getEvaluationItemFactory()->createRecord(rec->getType(), services, this);
        {
            QDomDocument doc;
            if (doc.setContent(xml)) {
                QDomElement e=doc.firstChildElement("evaluationelement");
                newrec->initNewID(e);
                QString n=tr("%1").arg(rec->getName());
                bool found=false;
                int cnt=1;
                do {
                    if (found) {
                        cnt++;
                        n=tr("%1 %2").arg(rec->getName()).arg(cnt);
                    }
                    found=false;
                    for (int i=0; i<getEvaluationCount(); i++) {
                        if (getEvaluationByNum(i) && getEvaluationByNum(i)->getName()==n) {
                            found=true;
                            break;
                        }
                    }
                } while (found && cnt<1000);
                newrec->setName(n);
            } else {
                delete newrec;
            }
        }
        emitStructureChanged();
    }



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



void QFProject::writeXML(QIODevice *file, bool resetDataChanged, const QString& filename_in)
{
    QString filename=filename_in;
    QFile* ffile=qobject_cast<QFile*>(file);
    if (filename.isEmpty() && ffile) {
        filename=ffile->fileName();
    }
    if (reading) return;

    bool namechanged=(filename!=this->file);
    if (!filename.isEmpty()) this->file=filename;


    QString tmpfn=qfGetTempFilename();
    {
        QFile f(tmpfn);
        f.open(QFile::WriteOnly);
        //qDebug()<<"writing "<<tmpfn;
        /*QTemporaryFile f(tmppath+"XXXXXX.tmp");
    QString tmpfn=f.fileName();*/
        //qDebug()<<tmpfn<<"\n"<<"  -> opened: "<<f.open(QFile::WriteOnly)<<"  fn="<<f.fileName();


        internalWriteXML(&f, resetDataChanged, namechanged, filename);
        if (f.error()!=QFile::NoError) {
            setError(tr("Error while writing project to file '%1'!\n Error description: %2.").arg(filename).arg(f.errorString()));
        }

        f.close();

        if (!filename.isEmpty()) {
            QFile f4(filename+".backup_old");
            if (f4.exists()) f4.remove();

            QFile f3(filename+".backup");
            f3.rename(filename+".backup_old");

            QFile f2(filename+".backup");
            if (f2.exists()) f2.remove();

            QFile f1(file);
            f1.rename(filename+".backup");
        }
    }

    QFile fcpy(tmpfn);
    if (fcpy.open(QFile::ReadOnly)) {
        size_t chunksize=PROJECT_COPY_CHUNKSIZE;
        char* chunk=qfMallocT<char>(PROJECT_COPY_CHUNKSIZE);
        while (!chunk && chunksize>1024) {
            chunksize=chunksize/2;
            chunk=qfMallocT<char>(PROJECT_COPY_CHUNKSIZE);
        }
        if (chunk) {
            while (!fcpy.atEnd() && !fcpy.error()) {
                qint64 bytesRead=fcpy.read(chunk, chunksize);
                qint64 written=file->write(chunk, bytesRead);
                if (written!=bytesRead) {
                    setError(tr("Error while writing project to file '%1'!\n Error description: could not write all data / %2.").arg(filename).arg(file->errorString()));
                    break;
                }
                if (fcpy.error()) {
                    setError(tr("Error while writing project to file '%1'!\n Error description: could not read from temporary project file / %2.").arg(filename).arg(fcpy.errorString()));
                    break;
                }
                //qDebug()<<"writing chunk of size "<<bytesRead;
            }

            qfFree(chunk);
        } else {
            setError(tr("Error while writing file: Could not allocate enough memory!"));
        }
        fcpy.close();
    } else {
        setError(tr("could not write temporary project file ..."));
    }
    if (QFile::exists(tmpfn)) {
        fcpy.remove(tmpfn);
        QFile fdel(tmpfn);
        fdel.remove(tmpfn);
    }
}

void QFProject::writeXMLSubset(QIODevice *file, const QSet<int> &rdrSelected, const QSet<int> &evalSelected, bool writeRecordsOnly, int writeMode, const QString &filename_in) const
{
    QString filename=filename_in;
    QFile* ffile=qobject_cast<QFile*>(file);
    if (filename.isEmpty() && ffile) {
        filename=ffile->fileName();
    }
    if (reading) return;

    //bool namechanged=(filename!=this->file);
    QString oldFile=this->file;
    if (!filename.isEmpty()) this->file=filename;


    QString tmpfn=qfGetTempFilename();
    {
        QFile f(tmpfn);
        f.open(QFile::WriteOnly);


        internalWriteXMLConst(&f, filename, false, false, false, NULL, rdrSelected, evalSelected, writeRecordsOnly);
        //internalWriteXML(&f, resetDataChanged, namechanged, filename);
        f.close();
    }

    QFile fcpy(tmpfn);
    if (fcpy.open(QFile::ReadOnly)) {
        size_t chunksize=PROJECT_COPY_CHUNKSIZE;
        char* chunk=qfMallocT<char>(PROJECT_COPY_CHUNKSIZE);
        while (!chunk && chunksize>1024) {
            chunksize=chunksize/2;
            chunk=qfMallocT<char>(PROJECT_COPY_CHUNKSIZE);
        }
        if (chunk) {
            while (!fcpy.atEnd()) {
                qint64 bytesRead=fcpy.read(chunk, chunksize);
                file->write(chunk, bytesRead);
                //qDebug()<<"writing chunk of size "<<bytesRead;
            }

            qfFree(chunk);
        } else {
            setError(tr("Error while writing file: Could not allocate enough memory!"));
        }
        fcpy.close();
    } else {
        setError(tr("could not write temporary project file ..."));
    }
    if (QFile::exists(tmpfn)) {
        fcpy.remove(tmpfn);
        QFile fdel(tmpfn);
        fdel.remove(tmpfn);
    }
    this->file=oldFile;
}


void QFProject::internalWriteXMLConst(QIODevice *file, const QString &projectFileName, bool copyFilesToSubfolder, const QString &rdrsubfoldername, const QString &evalsubfoldername, QList<QFProject::FileCopyList> *filecopylist, const QSet<int> &rdrSelected, const QSet<int> &evalSelected, bool writeRecordsOnly, int writeMode) const
{
    QXmlStreamWriter w(file);

    w.setAutoFormatting(false);
    w.writeStartDocument();
    if (writeRecordsOnly) {
        w.writeStartElement("quickfitprojectsubset");
        w.writeAttribute("quickfit_version", qfInfoVersionFull());
        w.writeAttribute("quickfit_svn", qfInfoSVNVersion());
        w.writeAttribute("quickfit_compiledate", qfInfoCompileDate());
    } else {
        w.writeStartElement("quickfitproject");
        w.writeAttribute("quickfit_version", qfInfoVersionFull());
        w.writeAttribute("quickfit_svn", qfInfoSVNVersion());
        w.writeAttribute("quickfit_compiledate", qfInfoCompileDate());
        w.writeAttribute("name", name);
        w.writeAttribute("creator", creator);
        w.writeAttribute("highest_id", QString::number(highestID));
        w.writeStartElement("rdr_groups");
        for (int i=0; i<rdrgroups.size(); i++) {
            w.writeStartElement("rdr_group");
            w.writeCDATA(rdrgroups[i]);
            w.writeEndElement();
        }
        w.writeEndElement();
        w.writeStartElement("description");
        w.writeCDATA(description);
        w.writeEndElement();
        w.writeStartElement("properties");
        storeProperties(w);
        w.writeEndElement();
    }

    if ((writeMode & wsmRDRs) == wsmRDRs) {
        w.writeStartElement("rawdata");
        for (int i=0; i<rawDataOrder.size(); i++) {
            QFRawDataRecord* rec=getRawDataByID(rawDataOrder[i]);
            if (rec && ((rdrSelected.size()==0 && evalSelected.size()==0) || rdrSelected.contains(rec->getID()))) {
                rec->writeXML(w, projectFileName, copyFilesToSubfolder, rdrsubfoldername, filecopylist, writeMode);
            }
        }
        w.writeEndElement();
    }
    if ((writeMode & wsmEvals) == wsmEvals) {
        w.writeStartElement("evaluations");

        for (int i=0; i<evaluationsOrder.size(); i++) {
            QFEvaluationItem* rec=getEvaluationByID(evaluationsOrder[i]);
            if (rec && ((rdrSelected.size()==0 && evalSelected.size()==0) || evalSelected.contains(rec->getID()))) {
                rec->writeXML(w, projectFileName, copyFilesToSubfolder, evalsubfoldername, filecopylist, writeMode);
            }
        }
        w.writeEndElement();
    }

    w.writeEndElement();
    w.writeEndDocument();
}


void QFProject::internalWriteXML(QIODevice *file, bool resetDataChanged, bool namechanged, const QString& projectFileName, bool copyFilesToSubfolder, const QString &rdrsubfoldername, const QString &evalsubfoldername, QList<FileCopyList> *filecopylist)
{

    internalWriteXMLConst(file, projectFileName, copyFilesToSubfolder, rdrsubfoldername, evalsubfoldername, filecopylist);
    if (resetDataChanged) {
        if (!errorOcc) {
            emitStructureChanged();
            if (namechanged) emitPropertiesChanged();
        }
    }

    /*if (w.hasError() || file->errorString().size()>0) {
        setError(tr("Could no write project file!\n Error description: %2.").arg(file->errorString()));
    }*/
}

void QFProject::writeXML(const QString& file, bool resetDataChanged) {

    if (file.toLower().contains(".qfpz") || file.toLower().contains(".qfp.gz")) {
        QuaGzipFile gzf(file);
        if (gzf.open(QIODevice::WriteOnly)) {
            writeXML(&gzf, resetDataChanged, file);
            gzf.close();
        } else {
            setError(tr("Could not open GZip-file for output (file: '%1', error: '%2')").arg(file).arg(gzf.errorString())+"\n");
        }
    } else {

        if (reading) return;
        bool namechanged=(file!=this->file);
        this->file=file;

        QTemporaryFile f(QFileInfo(file).absolutePath()+"/XXXXXX.tmp");
        f.setAutoRemove(false);
        f.open();
        f.setAutoRemove(false);

        internalWriteXML(&f, resetDataChanged, namechanged, file);

        if (f.error()==QFile::NoError) {

            QFile f4(file+".backup_old");
            if (f4.exists()) f4.remove();

            QFile f3(file+".backup");
            f3.rename(file+".backup_old");

            QFile f2(file+".backup");
            if (f2.exists()) f2.remove();

            QFile f1(file);
            if (f1.exists()) f1.rename(file+".backup");

        } else {
            setError(tr("Error while writing project to file '%1'!\n Error description: %2.").arg(file).arg(f.errorString()));
            return;
        }


        f.setAutoRemove(false);
        if (!f.rename(file)) {
            setError(tr("Could no open file '%1' for output (temp filename: %3)!\n Error description: %2.").arg(file).arg(f.errorString()).arg(f.fileName()));
        }
    }
}

QString QFProject_QuaZIPError(int error) {
    if (error==UNZ_OK) return  QObject::tr("NO ERROR");
    if (error==UNZ_END_OF_LIST_OF_FILE) return  QObject::tr("UNZ_END_OF_LIST_OF_FILE");
    if (error==UNZ_ERRNO) return  QObject::tr("UNZ_ERRNO");
    if (error==UNZ_EOF) return  QObject::tr("UNZ_EOF");
    if (error==UNZ_PARAMERROR) return  QObject::tr("UNZ_PARAMERROR");
    if (error==UNZ_BADZIPFILE) return  QObject::tr("UNZ_BADZIPFILE");
    if (error==UNZ_INTERNALERROR) return  QObject::tr("UNZ_INTERNALERROR");
    if (error==UNZ_CRCERROR) return  QObject::tr("UNZ_CRCERROR");
    return QObject::tr("ERROR No. %1").arg(error);
}

bool QFProject_compressFile(QuaZip* zip, QString fileName, QString fileDest, QString& error, QFProgressMinorProgress* pdlg) {
    error="";
    QFile inFile;
    inFile.setFileName(fileName);
    if(!inFile.open(QIODevice::ReadOnly)) {
        error=QObject::tr("error opening file %1: %2").arg(fileName).arg(inFile.errorString());
        return false;
    }

    QuaZipFile outFile(zip);
    if(!outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(fileDest, inFile.fileName()))) {
        error=QObject::tr("error opening file %1 in ZIP: %2").arg(fileDest).arg(outFile.errorString());
        return false;
    }

    if (!qfCopyData(&inFile, &outFile, pdlg) || outFile.getZipError()!=UNZ_OK) {
        if (outFile.getZipError()!=UNZ_OK) error=QObject::tr("error writing data into ZIP: %1").arg(QFProject_QuaZIPError(outFile.getZipError()));
        else error=QObject::tr("error writing data into ZIP");
        return false;
    }

    outFile.close();
    if (outFile.getZipError()!=UNZ_OK) {
        error=QObject::tr("error closing file in ZIP: %1").arg(QFProject_QuaZIPError(outFile.getZipError()));
        return false;
    }
    inFile.close();
    return true;
}


void QFProject::exportProjectToZIP(const QString &file, QFListProgressDialog* pdlg)
{
    QString currentProjectLocation=this->file;
    errorOcc=false;
    bool copyFilesToSubfolder=true;
    QString rdrsubfoldername="rdr_data_files/";
    QString evalsubfoldername="eval_data_files/";
    QList<FileCopyList> filecopylist;
    QString tmpfn=qfGetTempFilename();
    QString pfn=replaceFileExt(file, ".qfp");
    {
        QFile f(tmpfn);
        if (f.open(QFile::WriteOnly|QFile::Text)) {
            //qDebug()<<f.fileName();
            internalWriteXMLConst(&f, currentProjectLocation, copyFilesToSubfolder, rdrsubfoldername, evalsubfoldername, &filecopylist);
            f.close();
        }
    }


    if (QFile::exists(tmpfn)) {
        if (QFile::exists(file)) QFile::remove(file);
        if (pdlg) {
            pdlg->setRange(0, filecopylist.size()+2);
            pdlg->setProgress(0);
            pdlg->setMainMessage(tr("compressing project into ZIP-file ..."));
            pdlg->addMessage(tr("creating ZIP-file '%1'...").arg(file));
        }
        QuaZip zip(file);
        if (zip.open(QuaZip::mdCreate)) {
            if (pdlg) {
                pdlg->incProgress();
                pdlg->addMessage(tr("adding project file '%1'...").arg(QFileInfo(pfn).fileName()));
            }
            //qDebug()<<tr("adding project file '%1'...").arg(QFileInfo(pfn).fileName());
            QString errorStr;
            bool ok=QFProject_compressFile(&zip, tmpfn, QFileInfo(pfn).fileName(), errorStr, pdlg);
            //qDebug()<<"\n     "<<ok<<errorStr;
            if (ok && (!pdlg || (pdlg && !pdlg->wasCanceled()))) {
                if (filecopylist.size()>0) {
                    /*QString remstart="";
                    for (int j=0; j<10; j++) {
                        QString start="";
                        for (int i=0; i<filecopylist.size(); i++) {
                            QStringList sl=filecopylist[i].outFile.split("/");
                            if (j+1<sl.size()) {
                                start=sl[j];
                                break;
                            }
                        }
                        bool allStartEqual=true;
                        if (!start.isEmpty()) {
                            for (int i=0; i<filecopylist.size(); i++) {
                                QStringList sl=filecopylist[i].outFile.split("/");
                                if (!(sl.size()>j+1 && start==sl[j])) {
                                    allStartEqual=false;
                                    break;
                                }
                            }
                        }
                        if (allStartEqual && !start.isEmpty()) {
                            remstart=remstart+start+"/";
                        } else{
                            break;
                        }
                    }*/
                    //pdlg->addMessage(tr("removing from folder '%1'...").arg(remstart));
                    for (int i=0; i<filecopylist.size(); i++) {
                        QString outf=filecopylist[i].outFile;
                        /*if (remstart.size()>0 && outf.indexOf(remstart)==0) {
                            outf=outf.right(outf.size()-remstart.size());
                        }*/
                        if (pdlg) {
                            pdlg->incProgress();
                            pdlg->addMessage(tr("adding data file '%1'...").arg(filecopylist[i].outFile));
                        }
                        pdlg->setMinorProgressEnabled(true);
                        pdlg->setMinorPrgressLabel("compressing:   ");
                        ok=QFProject_compressFile(&zip, filecopylist[i].inFile, outf, errorStr, pdlg);
                        pdlg->setMinorProgressEnabled(false);
                        //qDebug()<<tr("adding data file %1 -> %2...").arg(filecopylist[i].inFile).arg(filecopylist[i].outFile)<<"\n     "<<ok<<errorStr;
                        if (!ok) {
                            setError(tr("Could not add file '%1' to ZIP-file '%2', error: %3").arg(filecopylist[i].outFile).arg(file).arg(errorStr));
                            break;
                        }
                        if (pdlg && pdlg->wasCanceled()) {
                            setError(tr("Compressing project to ZIP-file '%1' was canceled by the user").arg(file));
                            break;
                        }
                    }
                }
            } else {
                setError(tr("Could not add project file '%1' to ZIP-file '%2', error: %3").arg(pfn).arg(file).arg(errorStr));
            }
            zip.close();
        } else {
            setError(tr("Could not create ZIP-file '%1', error code: %2").arg(file).arg(QFProject_QuaZIPError(zip.getZipError())));
        }
        QFile::remove(tmpfn);
    }
    if (pdlg) {
        pdlg->setProgresComplete();
        if (errorOcc) pdlg->setMessage(tr("FINISHED WITH ERRORS:\n   %1\n   ZIP-filesize: %2 MBytes").arg(errorDesc).arg(double(QFileInfo(file).size())/1024.0/1024.0));
        else pdlg->setMessage(tr("FINISHED SUCCESSFULLY!\n   ZIP-filesize: %1 MBytes").arg(double(QFileInfo(file).size())/1024.0/1024.0));
        pdlg->addMessage("");
        if (errorOcc) {
            pdlg->addMessage(tr("FINISHED WITH ERRORS!"));
            pdlg->addMessage("  "+errorDesc);
        } else {
            pdlg->addMessage(tr("FINISHED SUCCESSFULLY!"));
        }
        pdlg->addMessage(tr("  ZIP-filesize: %1 MBytes").arg(double(QFileInfo(file).size())/1024.0/1024.0));
        pdlg->setUserClose();
    }
}

void QFProject::readXML(const QString &file)
{
    m_dummy=false;
    m_subset=false;
    subsetRDR.clear();
    subsetEval.clear();

    internalReadXML(file);
}



void QFProject::readXML(QIODevice *file, const QString& filename)
{
    m_dummy=false;
    m_subset=false;
    subsetRDR.clear();
    subsetEval.clear();
    internalReadXML(file, filename);
}


void QFProject::readXMLSubSet(QIODevice *file, const QString& filename, const QSet<int> &rdrSelected, const QSet<int> &evalSelected)
{
    m_dummy=false;
    m_subset=true;
    subsetRDR=rdrSelected;
    subsetEval=evalSelected;
    internalReadXML(file, filename);

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
    m_subset=false;
    subsetRDR.clear();
    subsetEval.clear();
}

void QFProject::readXMLSubSet(const QString &file, const QSet<int> &rdrSelected, const QSet<int> &evalSelected)
{
    if (file.toLower().contains(".qfpz") || file.toLower().contains(".qfp.gz")) {
        QuaGzipFile f(file);
        if (!f.open(QIODevice::ReadOnly)) {
            setError(tr("Could no open GZipped project file '%1' for input!\n Error description: %2.").arg(file).arg(f.errorString()));
            return;
        }
        readXMLSubSet(&f, file, rdrSelected, evalSelected);
    } else{
        QFile f(file);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            setError(tr("Could no open file '%1' for input!\n Error description: %2.").arg(file).arg(f.errorString()));
            return;
        }
        readXMLSubSet(&f, file, rdrSelected, evalSelected);
    }
}

void QFProject::readXMLDummy(const QString &file)
{
    m_dummy=true;
    m_subset=false;
    subsetRDR.clear();
    subsetEval.clear();
    internalReadXML(file);
}

void QFProject::readXMLDummy(QIODevice *file, const QString &filename)
{
    m_dummy=true;
    m_subset=false;
    subsetRDR.clear();
    subsetEval.clear();
    internalReadXML(file, filename);
}




void QFProject::internalReadXML(QIODevice *f, const QString &filename_in)
{
    QString filename=filename_in;
    QFile* ffile=qobject_cast<QFile*>(f);
    if (filename.isEmpty() && ffile) {
        filename=ffile->fileName();
    }

    bool oldreading=this->reading;
    this->reading=true;
    this->file=filename;
    QDomDocument d("quickfitproject");
    QString errorm="";
    if (d.setContent(f, &errorm)) {
        QDomElement e=d.documentElement();
#ifdef DEBUG_TIMING
        qDebug()<<"parsed XML      "<<time.elapsed()<<"ms"; time.start();
#endif
        if (e.tagName()=="quickfitproject") {
            QString v=e.attribute("quickfit_version");
            if (v.toDouble()>3.0) {
                setError(tr("Error while parsing project file '%1'!\n Error description: The project file was written by a newer version of QuickFit than this one (writer version: %1).").arg(file).arg(v));
                this->reading=oldreading;
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

                QDomElement rdg=e.firstChildElement("rdr_groups");
                rdrgroups.clear();
                if (!rdg.isNull()) {
                    rdg=rdg.firstChildElement("rdr_group");
                    while (!rdg.isNull()) {
                        rdrgroups<<rdg.text();
                        rdg=rdg.nextSiblingElement("rdr_group");
                    }
                }


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
                                if (e) e->init(rd, m_dummy);
                                else {
                                    setError(tr("Error while creating raw data element: no raw data record plugin of type '%2' was found in your QF3 version!").arg(t));
                                }

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
                                if (e) e->init(rd, m_dummy);
                                else {
                                    setError(tr("Error while creating evaluation element: no evaluation plugin of type '%2' was found in your QF3 version!").arg(t));
                                }
                            } catch(std::exception& E) {
                                setError(tr("Error while initializing evaluation element: %2").arg(E.what()));
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

                //qDebug()<<"highestID [pre]="<<highestID;
                if (e.hasAttribute("highest_id"))  highestID=qMax(highestID+1, e.attribute("highest_id", "-1").toInt());
                //qDebug()<<"highestID [post]="<<highestID;

            }
        } else {
            setError(tr("Error while parsing project file '%1'!\n Error description: This is not a QuickFit 3.0 project file.").arg(file));
            this->reading=oldreading;
            return;
        }
    } else {
        setError(tr("Error while parsing project file '%1'!\n Error description: %2.").arg(file).arg(errorm));
        this->reading=oldreading;
        return;
    }

    this->reading=oldreading;
}






void QFProject::internalReadXML(const QString& file) {
    this->reading=true;
    bool namechanged=(file!=this->file);
    this->file=file;

#ifdef DEBUG_TIMING
    QElapsedTimer time;
    time.start();
#endif

    QIODevice* f=NULL;
    QuaGzipFile fz(file);
    QFile ff(file);
    if (file.toLower().contains(".qfpz") || file.toLower().contains(".qfp.gz")) {
        f=&fz;
        if (!fz.open(QIODevice::ReadOnly)) {
            setError(tr("Could no open GZipped project file '%1' for input!\n Error description: %2.").arg(file).arg(fz.errorString()));
            this->reading=false;
            return;
        }
    } else {
        f=&ff;
        if (!ff.open(QIODevice::ReadOnly | QIODevice::Text)) {
            setError(tr("Could no open file '%1' for input!\n Error description: %2.").arg(file).arg(ff.errorString()));
            this->reading=false;
            return;
        }
    }




#ifdef DEBUG_TIMING
        qDebug()<<"opened file     "<<time.elapsed()<<"ms"; time.start();
#endif


    internalReadXML(f);

    f->close();



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



QStringList QFProject::getAllPropertyNames(bool visible_only) {
    QStringList sl;
    for (int i=0; i<getRawDataCount(); i++) {
        if (!visible_only) {
            sl.append(getRawDataByNum(i)->getPropertyNames());
        } else {
            sl.append(getRawDataByNum(i)->getVisibleProperties());
        }
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

bool QFProject::rdrResultsSaveToCSV(const QString& evalFilter, QString filename, bool vectorsToAvg, QChar separator, QChar decimalPoint, QChar stringDelimiter, const QStringList& filteredFitParamIDs, const QList<QPair<QPointer<QFRawDataRecord>, QString> >& filtereRecords) {
    /*QString sdel=stringDelimiter;
    QString dp=decimalPoint;
    QList<QPair<QString,QString> > colnames=rdrCalcMatchingResultsNamesAndLabels(evalFilter);
    QList<QPair<QPointer<QFRawDataRecord>, QString> > records=rdrCalcMatchingResults(evalFilter);

    if (filteredFitParamIDs.size()>0) {
        for (int j=colnames.size()-1; j>=0; j--) {
            bool found=false;
            for (int i=0; i<filteredFitParamIDs.size(); i++) {
                if (filteredFitParamIDs[i]==colnames[j].second) {
                    found=true;
                    break;
                }
            }
            //qDebug()<<"check colname"<<colnames[j].second<<found;
            if (!found) colnames.removeAt(j);
        }
    }

    if (filtereRecords.size()>0) {
        for (int j=records.size()-1; j>=0; j--) {
            bool found=false;
            for (int i=0; i<filtereRecords.size(); i++) {
                if (filtereRecords[i]==records[j] && records[j].first) {
                    found=true;
                    break;
                }
            }
            //qDebug()<<"check record"<<records[j].second<<found;
            if (!found) records.removeAt(j);
        }
    }

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
    return true;*/
    QStringList colname, rownames;
    QList<QList<QVariant> > data=rdrResultsGetTable(&colname, &rownames, evalFilter, vectorsToAvg, filteredFitParamIDs, filtereRecords);
    /*if (flipTable) {
        data=dataRotate(data);
        qSwap(colname, rownames);
    }*/
    QString dat=toCSV(data, colname, rownames, decimalPoint, separator, true, stringDelimiter);
    QFile of(filename);
    if (of.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&of);
        QTextCodec* c=QTextCodec::codecForName("ISO-8859-1");
        if (c==NULL) c=QTextCodec::codecForCStrings();
        if (c==NULL) c=QTextCodec::codecForLocale();
        out.setCodec(c);
        out<<dat;
        of.close();
        return true;
    } else {
        return false;
    }
}

QList<QList<QVariant> > QFProject::rdrResultsGetTable(QStringList* colNames, QStringList* rowNames, const QString &evalFilter, bool vectorsToAvg, const QStringList &filteredFitParamIDs, const QList<QPair<QPointer<QFRawDataRecord>, QString> > &filtereRecords)
{
    QList<QList<QVariant> > data;
    QStringList colnames, rownames;


    QList<QPair<QString,QString> > colNamesRaw=rdrCalcMatchingResultsNamesAndLabels(evalFilter);
    QList<QPair<QPointer<QFRawDataRecord>, QString> > records=rdrCalcMatchingResults(evalFilter);


    if (filteredFitParamIDs.size()>0) {
        for (int j=colNamesRaw.size()-1; j>=0; j--) {
            bool found=false;
            for (int i=0; i<filteredFitParamIDs.size(); i++) {
                if (filteredFitParamIDs[i]==colNamesRaw[j].second) {
                    found=true;
                    break;
                }
            }
            if (!found) colNamesRaw.removeAt(j);
        }
    }

    if (filtereRecords.size()>0) {
        for (int j=records.size()-1; j>=0; j--) {
            bool found=false;
            for (int i=0; i<filtereRecords.size(); i++) {
                if (filtereRecords[i]==records[j] && records[j].first) {
                    found=true;
                    break;
                }
            }
            if (!found) records.removeAt(j);
        }
    }


    QMap<int, int> subcolumns;
    for (int c=0; c<colNamesRaw.size(); c++) {
        subcolumns[c]=1;
        if (!vectorsToAvg) {
            for (int r=0; r<records.size(); r++) {
                QFRawDataRecord* record=records[r].first;
                QString evalname=records[r].second;
                QString dat="";
                if (record) {
                    if (record->resultsExists(evalname, colNamesRaw[c].second)) {
                        switch(record->resultsGet(evalname, colNamesRaw[c].second).type) {
                            case QFRawDataRecord::qfrdreNumberError:
                            case QFRawDataRecord::qfrdreNumber:
                            case QFRawDataRecord::qfrdreInteger:
                            case QFRawDataRecord::qfrdreBoolean:
                            case QFRawDataRecord::qfrdreString: subcolumns[c]=qMax(subcolumns[c], 1); break;
                            case QFRawDataRecord::qfrdreNumberErrorVector:
                            case QFRawDataRecord::qfrdreNumberErrorMatrix:
                            case QFRawDataRecord::qfrdreNumberVector:
                            case QFRawDataRecord::qfrdreNumberMatrix:  subcolumns[c]=qMax(subcolumns[c], record->resultsGet(evalname, colNamesRaw[c].second).dvec.size()); break;
                            case QFRawDataRecord::qfrdreIntegerVector:
                            case QFRawDataRecord::qfrdreIntegerMatrix:  subcolumns[c]=qMax(subcolumns[c], record->resultsGet(evalname, colNamesRaw[c].second).ivec.size()); break;
                            case QFRawDataRecord::qfrdreBooleanVector:
                            case QFRawDataRecord::qfrdreBooleanMatrix: subcolumns[c]=qMax(subcolumns[c], record->resultsGet(evalname, colNamesRaw[c].second).bvec.size()); break;
                            case QFRawDataRecord::qfrdreStringVector:
                            case QFRawDataRecord::qfrdreStringMatrix: subcolumns[c]=qMax(subcolumns[c], record->resultsGet(evalname, colNamesRaw[c].second).svec.size()); break;
                            default: subcolumns[c]=qMax(subcolumns[c], 1); break;
                        }
                    }
                }
            }
        }
    }


    rownames.append("");
    QList<QVariant> dempty; // empty column of data
    dempty.append(QVariant());
    for (int i=0; i<records.size(); i++) {
        rownames.append(records[i].first->getName()+": "+records[i].second);
        dempty.append(QVariant());
    }
    int col=0;
    for (int c=0; c<colNamesRaw.size(); c++) {
        colnames.append(colNamesRaw[c].first);
        data.append(dempty);
        data.last().operator[](0)=QString("value");
        for (int i=1; i<subcolumns[c]; i++) {
            colnames.append("");
            data.append(dempty);
        }
        bool hasError=false;
        //qDebug()<<col<<data.size()<<colnames.size()<<subcolumns[c]<<data.last().size();
        for (int r=0; r<records.size(); r++) {
            QFRawDataRecord* record=records[r].first;
            QString evalname=records[r].second;
            QStringList dat;
            int colcnt=1;
            if (record) {
                if (record->resultsExists(evalname, colNamesRaw[c].second)) {
                    switch(record->resultsGet(evalname, colNamesRaw[c].second).type) {
                        case QFRawDataRecord::qfrdreNumber: data[col].operator[](r+1)=(record->resultsGetAsDouble(evalname, colNamesRaw[c].second)); break;
                        case QFRawDataRecord::qfrdreNumberVector:
                        case QFRawDataRecord::qfrdreNumberMatrix: {
                            QVector<double> d=record->resultsGetAsDoubleList(evalname, colNamesRaw[c].second);
                            if (!vectorsToAvg) {
                                for (int it=0; it<d.size(); it++) {
                                    data[col+it].operator[](r+1)=(d[it]);
                                }
                                colcnt=d.size();
                            } else {
                                data[col].operator[](r+1)=(qfstatisticsAverage(d));
                            }
                        } break;
                        case QFRawDataRecord::qfrdreNumberErrorVector:
                        case QFRawDataRecord::qfrdreNumberErrorMatrix: {
                            QVector<double> d=record->resultsGetAsDoubleList(evalname, colNamesRaw[c].second);
                            if (!vectorsToAvg) {
                                for (int it=0; it<d.size(); it++) {
                                    data[col+it].operator[](r+1)=(d[it]);
                                }
                                colcnt=d.size();
                            } else {
                                data[col].operator[](r+1)=(qfstatisticsAverage(d));
                            }
                            hasError=true;
                        } break;
                        case QFRawDataRecord::qfrdreNumberError: data[col].operator[](r+1)=(record->resultsGetAsDouble(evalname, colNamesRaw[c].second)); hasError=true; break;
                        case QFRawDataRecord::qfrdreInteger: data[col].operator[](r+1)=((qlonglong)record->resultsGetAsInteger(evalname, colNamesRaw[c].second)); break;
                        case QFRawDataRecord::qfrdreIntegerVector:
                        case QFRawDataRecord::qfrdreIntegerMatrix:
                        case QFRawDataRecord::qfrdreBooleanVector:
                        case QFRawDataRecord::qfrdreBooleanMatrix: {
                            QVector<qlonglong> d=record->resultsGetAsIntegerList(evalname, colNamesRaw[c].second);
                            if (!vectorsToAvg) {
                                for (int it=0; it<d.size(); it++) {
                                    data[col+it].operator[](r+1)=(d[it]);
                                }
                                colcnt=d.size();
                            } else {
                                data[col].operator[](r+1)=(qfstatisticsAverage(d));
                            }
                        } break;
                        case QFRawDataRecord::qfrdreBoolean: data[col].operator[](r+1)=record->resultsGetAsBoolean(evalname, colNamesRaw[c].second); break;
                        case QFRawDataRecord::qfrdreString: data[col].operator[](r+1)=record->resultsGetAsString(evalname, colNamesRaw[c].second); break;
                        case QFRawDataRecord::qfrdreStringVector:
                        case QFRawDataRecord::qfrdreStringMatrix: {
                            QStringList d=record->resultsGetAsStringList(evalname, colNamesRaw[c].second);
                            if (!vectorsToAvg) {
                                for (int it=0; it<d.size(); it++) {
                                    data[col+it].operator[](r+1)=d[it];
                                }
                                colcnt=d.size();
                            }
                        } break;
                        default: break;
                    }
                }
            }
        }
        col+=qMax(1,subcolumns[c]);
        if (hasError) {
            colnames.append(colNamesRaw[c].first);
            data.append(dempty);
            data.last().operator[](0)=QString("error");
            for (int i=1; i<subcolumns[c]; i++) {
                colnames.append("");
                data.append(dempty);
            }
            for (int r=0; r<records.size(); r++) {
                QFRawDataRecord* record=records[r].first;
                QString evalname=records[r].second;
                QStringList dat;
                int colcnt=1;
                if (record) {
                    if (record->resultsExists(evalname, colNamesRaw[c].second)) {
                        switch (record->resultsGet(evalname, colNamesRaw[c].second).type) {
                            case QFRawDataRecord::qfrdreNumberError:
                                data[col].operator[](r+1)=(record->resultsGetErrorAsDouble(evalname, colNamesRaw[c].second));
                                break;
                            case QFRawDataRecord::qfrdreNumberErrorVector:
                            case QFRawDataRecord::qfrdreNumberErrorMatrix: {
                                if (!vectorsToAvg) {
                                    QVector<double> d=record->resultsGetErrorAsDoubleList(evalname, colNamesRaw[c].second);
                                    for (int it=0; it<d.size(); it++) {
                                        data[col+it].operator[](r+1)=(d[it]);
                                    }
                                    colcnt=d.size();
                                } else {
                                    QVector<double> d=record->resultsGetAsDoubleList(evalname, colNamesRaw[c].second);
                                    data[col].operator[](r+1)=(qfstatisticsStd(d));
                                }
                                } break;
                            case QFRawDataRecord::qfrdreNumberVector:
                            case QFRawDataRecord::qfrdreNumberMatrix: {
                                if (vectorsToAvg) {
                                    QVector<double> d=record->resultsGetAsDoubleList(evalname, colNamesRaw[c].second);
                                    data[col].operator[](r+1)=(qfstatisticsStd(d));
                                }
                            } break;
                            default: break;
                        }
                    }
                }
            }
            col+=qMax(1,subcolumns[c]);
        }
    }


    if (colNames) *colNames=colnames;
    if (rowNames) *rowNames=rownames;
    return data;
}

bool QFProject::rdrResultsSaveToSYLK(const QString &evalFilter, QString filename, bool vectorsToAvg, bool flipTable, const QStringList &filteredFitParamIDs, const QList<QPair<QPointer<QFRawDataRecord>, QString> > &filtereRecords) {
    /*QFile of(filename);
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

        if (filteredFitParamIDs.size()>0) {
            for (int j=colnames.size()-1; j>=0; j--) {
                bool found=false;
                for (int i=0; i<filteredFitParamIDs.size(); i++) {
                    if (filteredFitParamIDs[i]==colnames[j].second) {
                        found=true;
                        break;
                    }
                }
                if (!found) colnames.removeAt(j);
            }
        }

        if (filtereRecords.size()>0) {
            for (int j=records.size()-1; j>=0; j--) {
                bool found=false;
                for (int i=0; i<filtereRecords.size(); i++) {
                    if (filtereRecords[i]==records[j] && records[j].first) {
                        found=true;
                        break;
                    }
                }
                if (!found) records.removeAt(j);
            }
        }


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
    return true;*/


    QStringList colname, rownames;
    QList<QList<QVariant> > data=rdrResultsGetTable(&colname, &rownames, evalFilter, vectorsToAvg, filteredFitParamIDs, filtereRecords);
    if (flipTable) {
        data=dataRotate(data);
        qSwap(colname, rownames);
    }
    QString dat=toSYLK(data, colname, rownames);
    QFile of(filename);
    if (of.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&of);
        QTextCodec* c=QTextCodec::codecForName("ISO-8859-1");
        if (c==NULL) c=QTextCodec::codecForCStrings();
        if (c==NULL) c=QTextCodec::codecForLocale();
        out.setCodec(c);
        out<<dat;
        of.close();
        return true;
    } else {
        return false;
    }


}

bool QFProject::rdrResultsSave(const QString &evalFilter, QString filename, int format, bool vectorsToAvg, bool flipTable, const QStringList &filteredFitParamIDs, const QList<QPair<QPointer<QFRawDataRecord>, QString> > &filtereRecords)
{
    QStringList colname, rownames;
    QList<QList<QVariant> > data=rdrResultsGetTable(&colname, &rownames, evalFilter, vectorsToAvg, filteredFitParamIDs, filtereRecords);
    if (flipTable) {
        data=dataRotate(data);
        qSwap(colname, rownames);
    }

    QFDataExportHandler::save(data, format, filename, colname, rownames);
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

void QFProject::setPropertiesChanged()
{
    if (propertiesChange) return;
    propertiesChange=true;
    //qDebug()<<"QFProject emit wasChanged(dataChange="<<dataChange<<")";
    if (m_signalsEnabled) emit propertiesChanged();
    setDataChanged();
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
}

QString QFProject::ensureUniqueFilename(const QString &inFilename, const QString &newFilename, QList<FileCopyList> *filecopylist, bool addToList)
{
    //qDebug()<<inFilename;
    for (int i=0; i<filecopylist->size(); i++) {
        if ((*filecopylist)[i].inFile==inFilename) {
            //qDebug()<<"  => "<<(*filecopylist)[i].outFile;
            return (*filecopylist)[i].outFile;
        }
    }

    int num=0;
    QString newFN=newFilename;
    QFileInfo fi(newFilename);
    while (fileCopyListConatins(inFilename, newFN, filecopylist)>=0 && num<2000)  {
        num++;
        newFN=fi.filePath()+"/"+fi.baseName()+QString("_cnt%1.").arg(num, 4,10,QChar('0'))+fi.completeSuffix();
        //qDebug()<<"  newFN("<<num<<") "<<newFN;
    }
    if (addToList) filecopylist->append(FileCopyList(inFilename, newFN));
    //qDebug()<<"  => "<<newFN;
    return newFN;
}

int QFProject::fileCopyListConatins(const QString &inFilename, const QString &outFile, const QList<FileCopyList> *filecopylist)
{
    for (int i=0; i<filecopylist->size(); i++) {
        if ((*filecopylist)[i].outFile==outFile) {
            return i;
        }
    }
    return -1;
}


void QFProject::setError(QString description) {
    errorOcc=true;
    errorDesc=description;
    emit errorOccured(description);
}

void QFProject::setError(QString description) const {
    errorOcc=true;
    errorDesc=description;
    //emit errorOccured(description);
}

void QFProject::resetError() const
{
    errorOcc=false;
    errorDesc="";
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
}

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

QString QFProject::getRDRGroupName(int group) const
{
    return rdrgroups.value(group, "");
}

QList<QFRawDataRecord *> QFProject::getRDRGroupMembers(int group) const
{
    QList<QFRawDataRecord *> l;
    //if (group>=0 && group<rdrgroups.size()) {
    for (int i=0; i<getRawDataCount(); i++) {
        QFRawDataRecord* rdr=getRawDataByNum(i);
        if (rdr && rdr->getGroup()==group) l<<rdr;
    }
    /*} else {
        for (int i=0; i<getRawDataCount(); i++) {
            l<<getRawDataByNum(i);
        }
    }*/
    return l;
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

QFRawDataRecord* QFProject::getRawDataByID(int ID) const {
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

QList<QFRawDataRecord *> QFProject::getRDRsInFolder(const QString &folder, bool alsoSubfolders)
{
    QList<QFRawDataRecord *> lst;
    for (int i=0; i<getRawDataCount(); i++) {
        QFRawDataRecord * r=getRawDataByNum(i);
        if (r->getFolder()==folder) lst<<r;
        else if (alsoSubfolders&&r->getFolder().startsWith(folder)) lst<<r;
    }
    return lst;
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

int QFProject::addRDRGroup(const QString &name)
{
    rdrgroups<<name;
    emitStructureChanged();
    return rdrgroups.size()-1;
}

void QFProject::setRDRGroupName(int group, const QString &name)
{
    if (group>=0 && group<rdrgroups.size()) {
        rdrgroups[group]=name;
        emitStructureChanged();
    }
}

int QFProject::getRDRGroupCount() const
{
    return rdrgroups.size();
}

QStringList QFProject::getRDRGroupNames() const
{
    return rdrgroups;
}

int QFProject::addOrFindRDRGroup(const QString &name)
{
    int i=findRDRGroup(name);
    if (i<0) {
        i=addRDRGroup(name);
    }
    return i;
}

int QFProject::findRDRGroup(const QString &name)
{
    return rdrgroups.indexOf(name);
}


