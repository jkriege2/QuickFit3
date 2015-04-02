/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfevaluationitem.h"
#include "qfevaluationpropertyeditor.h"
#include <QMutex>
#include <QReadLocker>
#include <QMutexLocker>
#include <QWriteLocker>
#include <QReadWriteLock>

typedef QMutexLocker QFEvalReadLocker;
typedef QMutexLocker QFEvalWriteLocker;

class QFEvaluationItemPrivate {
    public:
        explicit QFEvaluationItemPrivate(QFEvaluationItem* dd) {
            d=dd;
            lock=new QMutex(QMutex::Recursive);
        }
        ~QFEvaluationItemPrivate() {
            delete lock;
        }


        QFEvaluationItem* d;
        mutable QMutex* lock;
};

QFEvaluationItem::QFEvaluationItem(QFProject* parent, bool showRDRList, bool useSelection):
    QObject(parent), QFProperties(), p(new QFEvaluationItemPrivate(this))
{
    this->useSelection=useSelection;
    this->showRDRList=showRDRList;
    doEmitPropertiesChanged=true;
    doEmitResultsChanged=true;
    highlightedRecord=NULL;
    project=parent;
    errorOcc=false;
    errorDesc="";
    name="";
    description="";
    nameFilter=QRegExp();
    nameNotFilter=QRegExp();
    if (project) connect(project, SIGNAL(recordAboutToBeDeleted(QFRawDataRecord*)), this, SLOT(recordAboutToBeDeleted(QFRawDataRecord*)));
}

void QFEvaluationItem::init(QString name) {
    this->ID=project->getNewID();
    this->name=name;
    description="";
    //intReadData();
    project->registerEvaluation(this);
}


void QFEvaluationItem::init(QDomElement& e, bool loadAsDummy) {
    //std::cout<<"creating QFEvaluationItem\n";
    name="";;
    description="";
    //std::cout<<"  reading XML\n";
    readXML(e, loadAsDummy, true);
    //std::cout<<"  registering record\n";
    project->registerEvaluation(this);
    //std::cout<<"created QFEvaluationItem\n";
}

void QFEvaluationItem::initNewID(QDomElement &e)
{
    //std::cout<<"creating QFEvaluationItem\n";
    name="";;
    description="";
    //std::cout<<"  reading XML\n";
    readXML(e, false, false);
    this->ID=project->getNewID();
    //std::cout<<"  registering record\n";
    project->registerEvaluation(this);
    //std::cout<<"created QFEvaluationItem\n";


}

bool QFEvaluationItem::isFilteredAndApplicable(QFRawDataRecord *record) const
{
    QFEvalReadLocker locker(p->lock);
    if (!record) return false;
    return isApplicable(record)
            && (nameFilter.pattern().isEmpty() || record->getName().indexOf(nameFilter)>=0)
            && (nameNotFilter.pattern().isEmpty() || record->getName().indexOf(nameNotFilter)<0);

}

void QFEvaluationItem::setPresetProperty(const QString &id, const QVariant &data, bool usereditable, bool visible)
{
    QString d=id;
    if (!id.startsWith("PRESET_")) d=QString("PRESET_")+id;
    setQFProperty(d, data, usereditable, visible);
}

QFEvaluationItem *QFEvaluationItem::getNext()
{
    QFEvalReadLocker locker(p->lock);
    return project->getNextEvaluation(this);
}

QFEvaluationItem *QFEvaluationItem::getPrevious() {
    QFEvalReadLocker locker(p->lock);
    return project->getPreviousEvaluation(this); }

QFEvaluationItem *QFEvaluationItem::getNextOfSameType() {
    QFEvalReadLocker locker(p->lock);
    return project->getNextEvaluationOfSameType(this); }

QFEvaluationItem *QFEvaluationItem::getPreviousOfSameType() {
    QFEvalReadLocker locker(p->lock);
    return project->getPreviousEvaluationOfSameType(this); }

int QFEvaluationItem::getID() const {
    QFEvalReadLocker locker(p->lock);
    return ID; }

QString QFEvaluationItem::getName() const {
    QFEvalReadLocker locker(p->lock);
    return name; }

QString QFEvaluationItem::getDescription() const {
    QFEvalReadLocker locker(p->lock);
    return description; }

bool QFEvaluationItem::error() const {
    QFEvalReadLocker locker(p->lock);
    return errorOcc; }

QString QFEvaluationItem::errorDescription() const {
    QFEvalReadLocker locker(p->lock);
    return errorDesc; }

bool QFEvaluationItem::getUseSelection() const {
    QFEvalReadLocker locker(p->lock);
    return useSelection;
}

bool QFEvaluationItem::getShowRDRList() const {
    QFEvalReadLocker locker(p->lock);
    return showRDRList;
}

QList<QPointer<QFRawDataRecord> > QFEvaluationItem::getSelectedRecords() const {
    QFEvalReadLocker locker(p->lock);
    return selectedRecords;
}

QFEvaluationItem::~QFEvaluationItem() {
    //std::cout<<"deleting QFEvaluationItem\n";
    //std::cout<<"deleting QFEvaluationItem ... OK\n";
}

void QFEvaluationItem::readXML(QDomElement& e, bool loadAsDummy, bool readID) {
    bool ok=true;
    name=e.attribute("name", "evaluation");
    if (readID) {
        ID=e.attribute("id", "-1").toInt(&ok);
        if (ID==-1) { setError(tr("invalid ID in <evaluation name=\"%1\" ...>!").arg(name)); return; }
        if (!project->checkID(ID)) {
            setError(tr("ID %1 in <evaluation name=\"%2\" ...> already in use in the project!").arg(ID).arg(name));
            return;
        }
    }
    QDomElement te=e.firstChildElement("description");
    if (te.isNull()) { description=""; } else { description=te.text(); }

    te=e.firstChildElement("properties");
    readProperties(te);
    nameFilter.setPattern(getProperty("RECORD_FILTER", "").toString());
    nameNotFilter.setPattern(getProperty("RECORD_FILTERNOT", "").toString());
    nameFilter.setPatternSyntax(getProperty("RECORD_FILTER_REGEXP", false).toBool()?QRegExp::RegExp:QRegExp::Wildcard);
    nameNotFilter.setPatternSyntax(getProperty("RECORD_FILTERNOT_REGEXP", false).toBool()?QRegExp::RegExp:QRegExp::Wildcard);

    //std::cout<<"    reading XML: data\n";

    // read list of selected items
    if (useSelection) {
        QList<QPointer<QFRawDataRecord> > selectedRecords;
        te=e.firstChildElement("selection");
        te=te.firstChildElement("item");
        while (!te.isNull()) {
            QFRawDataRecord* rec=project->getRawDataByID(te.attribute("id").toInt());
            if (isApplicable(rec)) selectedRecords.append(rec);
            te=te.nextSiblingElement("item");
        }
        setHighlightedRecord(NULL);
        setSelectedRecords(selectedRecords);
        if (selectedRecords.size()>0) setHighlightedRecord(selectedRecords[0]);
    }

    te=e.firstChildElement("data");
    if (!loadAsDummy) intReadData(&te);
    //std::cout<<"reading XML: done!\n";
    if (!errorOcc) {
        emitPropertiesChanged("", true);
        emitResultsChanged();
    }
}


void QFEvaluationItem::writeXML(QXmlStreamWriter& w, const QString &/*projectfilename*/, bool /*copyFilesToSubfolder*/, const QString &/*subfoldername*/, QList<QFProject::FileCopyList> */*filecopylist*/, int /*writeMode*/) {
    QFEvalReadLocker locker(p->lock);
    w.writeStartElement("evaluationelement");
    w.writeAttribute("type", getType());
    w.writeAttribute("name", name);
    w.writeAttribute("id", QString::number(ID));
    w.writeStartElement("description");
    w.writeCDATA(description);
    w.writeEndElement();
    w.writeStartElement("properties");
    storeProperties(w);
    w.writeEndElement();


    // write list of selected items
    if ( (selectedRecords.size()>0) && useSelection) {
        w.writeStartElement("selection");
        for (int i=0; i<selectedRecords.size(); i++) {
            w.writeStartElement("item");
            w.writeAttribute("id", QString("%1").arg(selectedRecords[i]->getID()));
            w.writeEndElement();
        }
        w.writeEndElement();
    }
    w.writeStartElement("data");
    intWriteData(w);
    w.writeEndElement();
    w.writeEndElement();
}

bool QFEvaluationItem::doCopyFileForExport(const QString &/*filename*/, const QString &/*fileType*/, QString &/*newFilename*/, const QList<QFProject::FileCopyList> */*filecopylist*/, const QString &/*subfoldername*/) const
{
    return true;
}

void QFEvaluationItem::setHighlightedRecord(QFRawDataRecord* record) {
    QFEvalWriteLocker locker(p->lock);
    if (isFilteredAndApplicable(record)) {
        QFRawDataRecord* old=highlightedRecord;
        //disconnect(old, NULL, this, NULL);
        highlightedRecord=record;
        //qDebug()<<"QFEvaluationItem ("<<name<<") emits highlightingChanged(old="<<old<<", record="<<record<<")";

        emit highlightingChanged(old, record);
    }
}

void QFEvaluationItem::setSelectedRecords(QList<QPointer<QFRawDataRecord> > records) {
    QFEvalWriteLocker locker(p->lock);
    for (int i=records.size()-1; i>=0; i--) {
        if (!isFilteredAndApplicable(records[i])) records.removeAt(i);
        //disconnect(selectedRecords[i], NULL, this, NULL);
    }
    selectedRecords=records;
    //qDebug()<<"QFEvaluationItem ("<<name<<") emits selectionChanged("<<records.size()<<")";
    emit selectionChanged(records);
    project->setDataChanged();
}

void QFEvaluationItem::selectRecord(QFRawDataRecord* record) {
    QFEvalWriteLocker locker(p->lock);
    if ((record!=NULL) && isFilteredAndApplicable(record)) {
        if (!selectedRecords.contains(record)) {
            selectedRecords.append(record);
            //qDebug()<<"QFEvaluationItem ("<<name<<") emits selectionChanged("<<selectedRecords.size()<<")";
            emit selectionChanged(selectedRecords);
            project->setDataChanged();
        }
    }
}

void QFEvaluationItem::deselectRecord(QFRawDataRecord* record) {
    QFEvalWriteLocker locker(p->lock);
    if (selectedRecords.contains(record)) {
        selectedRecords.removeAll(record);
        //qDebug()<<"QFEvaluationItem ("<<name<<") emits selectionChanged("<<selectedRecords.size()<<")";
        emit selectionChanged(selectedRecords);
        project->setDataChanged();
    }
}

void QFEvaluationItem::deselectRecord(int i) {
    QFEvalWriteLocker locker(p->lock);
    if ((i>=0) && (i<selectedRecords.size())) {
        selectedRecords.removeAt(i);
        //qDebug()<<"QFEvaluationItem ("<<name<<") emits selectionChanged("<<selectedRecords.size()<<")";
        emit selectionChanged(selectedRecords);
        project->setDataChanged();
    }
}

int QFEvaluationItem::getSelectedRecordCount() {
    QFEvalReadLocker locker(p->lock);

    return selectedRecords.size();
}

QPointer<QFRawDataRecord> QFEvaluationItem::getSelectedRecord(int i) const {
    QFEvalReadLocker locker(p->lock);
    if ((i>=0) && (i<selectedRecords.size())) {
        return selectedRecords[i];
    }
    return NULL;
}

void QFEvaluationItem::clearSelectedRecords() {
    QFEvalWriteLocker locker(p->lock);
    if (selectedRecords.size()>0) {
        selectedRecords.clear();
        //qDebug()<<"QFEvaluationItem ("<<name<<") emits selectionChanged("<<selectedRecords.size()<<")";
        emit selectionChanged(selectedRecords);
        project->setDataChanged();
    }
}

bool QFEvaluationItem::isSelected(QFRawDataRecord *record) const {     QFEvalReadLocker locker(p->lock); return selectedRecords.contains(record); }

QFRawDataRecord *QFEvaluationItem::getHighlightedRecord() const {     QFEvalReadLocker locker(p->lock); return highlightedRecord; }

void QFEvaluationItem::selectAllAplicableRecords() {
    QFEvalWriteLocker locker(p->lock);
    if (project!=NULL) {
        bool added=false;
        for (int i=0; i<project->getRawDataCount(); i++) {
            QFRawDataRecord* rec=project->getRawDataByNum(i);
            if (isFilteredAndApplicable(rec) && (!selectedRecords.contains(rec))) {
                added=true;
                selectedRecords.append(rec);
            }
        }
        if (added) {
            //qDebug()<<"QFEvaluationItem ("<<name<<") emits selectionChanged("<<selectedRecords.size()<<")";
            emit selectionChanged(selectedRecords);
            project->setDataChanged();
        }
    }
}

QList<QPointer<QFRawDataRecord> > QFEvaluationItem::getApplicableRecords() const {
    QFEvalReadLocker locker(p->lock);
    QList<QPointer<QFRawDataRecord> > recs;
    for (int i=0; i<project->getRawDataCount(); i++) {
        QPointer<QFRawDataRecord> rec=project->getRawDataByNum(i);
        if (isFilteredAndApplicable(rec)) recs.append(rec);
    }
    return recs;
}

QString QFEvaluationItem::getResultsDisplayFilter() const {
    return getType()+"_"+QString::number(getID())+"*";
}

bool QFEvaluationItem::get_doEmitResultsChanged() const {
    QFEvalReadLocker locker(p->lock);
    return doEmitResultsChanged;
}

void QFEvaluationItem::set_doEmitResultsChanged(bool enable) {
    QFEvalReadLocker locker(p->lock);
    doEmitResultsChanged=enable;
}

bool QFEvaluationItem::get_doEmitPropertiesChanged() const {
    QFEvalReadLocker locker(p->lock);
    return doEmitPropertiesChanged;
}

void QFEvaluationItem::set_doEmitPropertiesChanged(bool enable)
{
    QFEvalReadLocker locker(p->lock);
    doEmitPropertiesChanged=enable;
}

void QFEvaluationItem::recordAboutToBeDeleted(QFRawDataRecord* r) {
    QFEvalWriteLocker locker(p->lock);
    bool wasCurrent=false;
    if (highlightedRecord==r) {
        highlightedRecord=r->getNextOfSameType();
        wasCurrent=true;
    }

    if (selectedRecords.contains(r)) {
        selectedRecords.removeAll(r);
    }
    selectedRecords.removeAll(NULL);
    //qDebug()<<"QFEvaluationItem ("<<name<<") emits selectionChanged("<<selectedRecords.size()<<")";
    emit selectionChanged(selectedRecords);
    if (wasCurrent) {
        //qDebug()<<"QFEvaluationItem ("<<name<<") emits highlightingChanged(r="<<r<<", highlighted="<<highlightedRecord<<")";
        emit highlightingChanged(r, highlightedRecord);
    }
    project->setDataChanged();
}

void QFEvaluationItem::setName(const QString n) {
    QFEvalWriteLocker locker(p->lock);
    if (name!=n) {
        name=n;
        emit basicPropertiesChanged();
    }
}

void QFEvaluationItem::setDescription(const QString& d) {
    QFEvalWriteLocker locker(p->lock);
    if (description!=d) {
        description=d;
        emit basicPropertiesChanged();
    }
};

void QFEvaluationItem::emitResultsChanged(QFRawDataRecord* record, const QString& evaluationName, const QString& resultName)  {
    QFEvalReadLocker locker(p->lock);
    if (doEmitResultsChanged) {
        //qDebug()<<"QFEvaluationItem ("<<name<<") emits resultsChanged()";
        emit resultsChanged(record, evaluationName, resultName);
    }
}

void QFEvaluationItem::emitPropertiesChanged(const QString& property, bool visible)  {
    QFEvalReadLocker locker(p->lock);
    if (doEmitPropertiesChanged) {
        //qDebug()<<"QFEvaluationItem ("<<name<<") emits propertiesChanged()";
        emit propertiesChanged(property, visible);
    }
}

void QFEvaluationItem::setNameFilter(QString filter, bool regexp)
{
    QFEvalWriteLocker locker(p->lock);
    nameFilter.setPattern(filter);
    nameFilter.setCaseSensitivity(Qt::CaseInsensitive);
    if (regexp) nameFilter.setPatternSyntax(QRegExp::RegExp);
    else nameFilter.setPatternSyntax(QRegExp::Wildcard);
    setQFProperty("RECORD_FILTER", filter, false, false);
    setQFProperty("RECORD_FILTER_REGEXP", regexp, false, false);
}

void QFEvaluationItem::setNameNotFilter(QString filter, bool regexp)
{
    QFEvalWriteLocker locker(p->lock);
    nameNotFilter.setPattern(filter);
    nameNotFilter.setCaseSensitivity(Qt::CaseInsensitive);
    if (regexp) nameNotFilter.setPatternSyntax(QRegExp::RegExp);
    else nameNotFilter.setPatternSyntax(QRegExp::Wildcard);
    setQFProperty("RECORD_FILTERNOT", filter, false, false);
    setQFProperty("RECORD_FILTERNOT_REGEXP", regexp, false, false);
}

void QFEvaluationItem::setNameNameNotFilter(QString filter, QString filterNot, bool regexp, bool regexpNot)
{
    QFEvalWriteLocker locker(p->lock);
    nameFilter.setPattern(filter);
    nameFilter.setCaseSensitivity(Qt::CaseInsensitive);
    if (regexp) nameFilter.setPatternSyntax(QRegExp::RegExp);
    else nameFilter.setPatternSyntax(QRegExp::Wildcard);
    nameNotFilter.setPattern(filterNot);
    nameNotFilter.setCaseSensitivity(Qt::CaseInsensitive);
    if (regexpNot) nameNotFilter.setPatternSyntax(QRegExp::RegExp);
    else nameNotFilter.setPatternSyntax(QRegExp::Wildcard);
    setQFProperty("RECORD_FILTER", filter, false, false);
    setQFProperty("RECORD_FILTERNOT", filterNot, false, false);
    setQFProperty("RECORD_FILTER_REGEXP", regexp, false, false);
    setQFProperty("RECORD_FILTERNOT_REGEXP", regexpNot, false, false);

}


QString QFEvaluationItem::getNameFilter() const
{
    QFEvalReadLocker locker(p->lock);
    return nameFilter.pattern();
}

bool QFEvaluationItem::getNameFilterRegExp() const
{
    QFEvalReadLocker locker(p->lock);
    return nameFilter.patternSyntax()==QRegExp::RegExp;
}

QString QFEvaluationItem::getNameNotFilter() const
{
    QFEvalReadLocker locker(p->lock);
    return nameNotFilter.pattern();
}

bool QFEvaluationItem::getNameNotFilterRegExp() const
{
    QFEvalReadLocker locker(p->lock);
    return nameNotFilter.patternSyntax()==QRegExp::RegExp;
}

int QFEvaluationItem::getIndexFromEvaluationResultID(const QString &resultID) const
{
    QFEvalReadLocker locker(p->lock);
    if (resultID.size()<=0) return -1;
    if (resultID.endsWith("runavg")) return -1;
    if (resultID.endsWith("avg")) return -1;

    //QRegExp rxNum("[+-]\\d+");

    int l=0;
    const QString digits="0123456789+-";
    QChar c;
    while (l<resultID.size() && (digits.contains(c=resultID[resultID.size()-1-l]))) {
        l++;
        if (c=='+' || c=='-') break;
    }


    if (l>0) {
        return resultID.right(l).toInt();
    }
    return -1;
}

void QFEvaluationItem::readLock() const
{
    p->lock->lock();
}

void QFEvaluationItem::writeLock() const
{
    p->lock->lock();
}

void QFEvaluationItem::readUnLock() const
{
    p->lock->unlock();
}

void QFEvaluationItem::writeUnLock() const
{
    p->lock->unlock();
}

void QFEvaluationItem::setDataChanged()
{
    QFEvalReadLocker locker(p->lock);
    if (project) project->setDataChanged();
}
