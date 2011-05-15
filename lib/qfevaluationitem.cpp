#include "qfevaluationitem.h"
#include "qfevaluationpropertyeditor.h"



QFEvaluationItem::QFEvaluationItem(QFProject* parent, bool showRDRList, bool useSelection):
    QObject(parent), QFProperties()
{
    this->useSelection=useSelection;
    this->showRDRList=showRDRList;
    highlightedRecord=NULL;
    project=parent;
    errorOcc=false;
    errorDesc="";
    name="";
    description="";
}

void QFEvaluationItem::init(QString name) {
    this->ID=project->getNewID();
    this->name=name;
    description="";
    //intReadData();
    project->registerEvaluation(this);
}


void QFEvaluationItem::init(QDomElement& e) {
    //std::cout<<"creating QFEvaluationItem\n";
    name="";;
    description="";
    //std::cout<<"  reading XML\n";
    readXML(e);
    //std::cout<<"  registering record\n";
    project->registerEvaluation(this);
    //std::cout<<"created QFEvaluationItem\n";
}

QFEvaluationItem::~QFEvaluationItem() {
    //std::cout<<"deleting QFEvaluationItem\n";
    //std::cout<<"deleting QFEvaluationItem ... OK\n";
}

void QFEvaluationItem::readXML(QDomElement& e) {
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

    //std::cout<<"    reading XML: data\n";

    // read list of selected items
    if (useSelection) {
        QList<QFRawDataRecord*> selectedRecords;
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
    intReadData(&te);
    //std::cout<<"reading XML: done!\n";
    if (!errorOcc) {
        emit propertiesChanged();
        emit resultsChanged();
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
    if (isApplicable(record)) {
        QFRawDataRecord* old=highlightedRecord;
        //disconnect(old, NULL, this, NULL);
        highlightedRecord=record;
        emit highlightingChanged(old, record);
    }
}

void QFEvaluationItem::setSelectedRecords(QList<QFRawDataRecord*> records) {
    for (int i=records.size()-1; i>=0; i--) {
        if (!isApplicable(records[i])) records.removeAt(i);
        //disconnect(selectedRecords[i], NULL, this, NULL);
    }
    selectedRecords=records;
    emit selectionChanged(records);
    project->setDataChanged();
}

void QFEvaluationItem::selectRecord(QFRawDataRecord* record) {
    if ((record!=NULL) && isApplicable(record)) {
        if (!selectedRecords.contains(record)) {
            selectedRecords.append(record);
            emit selectionChanged(selectedRecords);
            project->setDataChanged();
        }
    }
}

void QFEvaluationItem::deselectRecord(QFRawDataRecord* record) {
    if (selectedRecords.contains(record)) {
        selectedRecords.removeAll(record);
        emit selectionChanged(selectedRecords);
        project->setDataChanged();
    }
}

void QFEvaluationItem::deselectRecord(int i) {
    if ((i>=0) && (i<selectedRecords.size())) {
        selectedRecords.removeAt(i);
        emit selectionChanged(selectedRecords);
        project->setDataChanged();
    }
}

QFRawDataRecord* QFEvaluationItem::getSelectedRecord(int i) {
    if ((i>=0) && (i<selectedRecords.size())) {
        return selectedRecords[i];
    }
    return NULL;
}

void QFEvaluationItem::clearSelectedRecords() {
    if (selectedRecords.size()>0) {
        selectedRecords.clear();
        emit selectionChanged(selectedRecords);
        project->setDataChanged();
    }
}

void QFEvaluationItem::selectAllAplicableRecords() {
    if (project!=NULL) {
        bool added=false;
        for (int i=0; i<project->getRawDataCount(); i++) {
            QFRawDataRecord* rec=project->getRawDataByNum(i);
            if (isApplicable(rec) && (!selectedRecords.contains(rec))) {
                added=true;
                selectedRecords.append(rec);
            }
        }
        if (added) {
            emit selectionChanged(selectedRecords);
            project->setDataChanged();
        }
    }
}

QList<QFRawDataRecord*> QFEvaluationItem::getApplicableRecords() {
    QList<QFRawDataRecord*> recs;
    for (int i=0; i<project->getRawDataCount(); i++) {
        QFRawDataRecord* rec=project->getRawDataByNum(i);
        recs.append(rec);
    }
    return recs;
}

QString QFEvaluationItem::getResultsDisplayFilter() const {
    return getType()+"_"+QString::number(getID())+"*";
}
