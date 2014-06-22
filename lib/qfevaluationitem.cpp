/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#include "qfevaluationitem.h"
#include "qfevaluationpropertyeditor.h"



QFEvaluationItem::QFEvaluationItem(QFProject* parent, bool showRDRList, bool useSelection):
    QObject(parent), QFProperties()
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
    readXML(e);
    //std::cout<<"  registering record\n";
    project->registerEvaluation(this);
    //std::cout<<"created QFEvaluationItem\n";
}

bool QFEvaluationItem::isFilteredAndApplicable(QFRawDataRecord *record)
{
    if (!record) return false;
    return isApplicable(record)
            && (nameFilter.pattern().isEmpty() || record->getName().indexOf(nameFilter)>=0)
            && (nameNotFilter.pattern().isEmpty() || record->getName().indexOf(nameNotFilter)<0);

}

QFEvaluationItem::~QFEvaluationItem() {
    //std::cout<<"deleting QFEvaluationItem\n";
    //std::cout<<"deleting QFEvaluationItem ... OK\n";
}

void QFEvaluationItem::readXML(QDomElement& e, bool loadAsDummy) {
    bool ok=true;
    name=e.attribute("name", "evaluation");
    ID=e.attribute("id", "-1").toInt(&ok);
    if (ID==-1) { setError(tr("invalid ID in <evaluation name=\"%1\" ...>!").arg(name)); return; }
    if (!project->checkID(ID)) {
        setError(tr("ID %1 in <evaluation name=\"%2\" ...> already in use in the project!").arg(ID).arg(name));
        return;
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


void QFEvaluationItem::writeXML(QXmlStreamWriter& w) {
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

void QFEvaluationItem::setHighlightedRecord(QFRawDataRecord* record) {
    if (isFilteredAndApplicable(record)) {
        QFRawDataRecord* old=highlightedRecord;
        //disconnect(old, NULL, this, NULL);
        highlightedRecord=record;
        //qDebug()<<"QFEvaluationItem ("<<name<<") emits highlightingChanged(old="<<old<<", record="<<record<<")";

        emit highlightingChanged(old, record);
    }
}

void QFEvaluationItem::setSelectedRecords(QList<QPointer<QFRawDataRecord> > records) {
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
    if (selectedRecords.contains(record)) {
        selectedRecords.removeAll(record);
        //qDebug()<<"QFEvaluationItem ("<<name<<") emits selectionChanged("<<selectedRecords.size()<<")";
        emit selectionChanged(selectedRecords);
        project->setDataChanged();
    }
}

void QFEvaluationItem::deselectRecord(int i) {
    if ((i>=0) && (i<selectedRecords.size())) {
        selectedRecords.removeAt(i);
        //qDebug()<<"QFEvaluationItem ("<<name<<") emits selectionChanged("<<selectedRecords.size()<<")";
        emit selectionChanged(selectedRecords);
        project->setDataChanged();
    }
}

QPointer<QFRawDataRecord> QFEvaluationItem::getSelectedRecord(int i) {
    if ((i>=0) && (i<selectedRecords.size())) {
        return selectedRecords[i];
    }
    return NULL;
}

void QFEvaluationItem::clearSelectedRecords() {
    if (selectedRecords.size()>0) {
        selectedRecords.clear();
        //qDebug()<<"QFEvaluationItem ("<<name<<") emits selectionChanged("<<selectedRecords.size()<<")";
        emit selectionChanged(selectedRecords);
        project->setDataChanged();
    }
}

void QFEvaluationItem::selectAllAplicableRecords() {
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

QList<QPointer<QFRawDataRecord> > QFEvaluationItem::getApplicableRecords() {
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

void QFEvaluationItem::recordAboutToBeDeleted(QFRawDataRecord* r) {
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
    if (name!=n) {
        name=n;
        emit basicPropertiesChanged();
    }
}

void QFEvaluationItem::setDescription(const QString& d) {
    if (description!=d) {
        description=d;
        emit basicPropertiesChanged();
    }
};

void QFEvaluationItem::emitResultsChanged(QFRawDataRecord* record, const QString& evaluationName, const QString& resultName)  {
    if (doEmitResultsChanged) {
        //qDebug()<<"QFEvaluationItem ("<<name<<") emits resultsChanged()";
        emit resultsChanged(record, evaluationName, resultName);
    }
}

void QFEvaluationItem::emitPropertiesChanged(const QString& property, bool visible)  {
    if (doEmitPropertiesChanged) {
        //qDebug()<<"QFEvaluationItem ("<<name<<") emits propertiesChanged()";
        emit propertiesChanged(property, visible);
    }
}

void QFEvaluationItem::setNameFilter(QString filter, bool regexp)
{
    nameFilter.setPattern(filter);
    nameFilter.setCaseSensitivity(Qt::CaseInsensitive);
    if (regexp) nameFilter.setPatternSyntax(QRegExp::RegExp);
    else nameFilter.setPatternSyntax(QRegExp::Wildcard);
    setQFProperty("RECORD_FILTER", filter, false, false);
    setQFProperty("RECORD_FILTER_REGEXP", regexp, false, false);
}

void QFEvaluationItem::setNameNotFilter(QString filter, bool regexp)
{
    nameNotFilter.setPattern(filter);
    nameNotFilter.setCaseSensitivity(Qt::CaseInsensitive);
    if (regexp) nameNotFilter.setPatternSyntax(QRegExp::RegExp);
    else nameNotFilter.setPatternSyntax(QRegExp::Wildcard);
    setQFProperty("RECORD_FILTERNOT", filter, false, false);
    setQFProperty("RECORD_FILTERNOT_REGEXP", regexp, false, false);
}

void QFEvaluationItem::setNameNameNotFilter(QString filter, QString filterNot, bool regexp, bool regexpNot)
{
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
    return nameFilter.pattern();
}

bool QFEvaluationItem::getNameFilterRegExp() const
{
    return nameFilter.patternSyntax()==QRegExp::RegExp;
}

QString QFEvaluationItem::getNameNotFilter() const
{
    return nameNotFilter.pattern();
}

bool QFEvaluationItem::getNameNotFilterRegExp() const
{
    return nameNotFilter.patternSyntax()==QRegExp::RegExp;
}

void QFEvaluationItem::setDataChanged()
{
    if (project) project->setDataChanged();
}
