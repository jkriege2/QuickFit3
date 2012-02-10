#include "qfrawdatarecord.h"
#include "qftools.h"
#include <QtXml>
#include "qtriple.h"


class QFRawDataRecordPrivate {
    public:
        QFRawDataRecordPrivate() {

        }
        ~QFRawDataRecordPrivate() {

        }

        typedef QHash<QString, QFRawDataRecord::evaluationResult> ResultsResultsType;
        typedef QHashIterator<QString, QFRawDataRecord::evaluationResult> ResultsResultsIterator;


        /*! \brief this struct holds the metadata and also the data about an evaluationID */
        struct evaluationIDMetadata {
            evaluationIDMetadata(int initsize);
            QString group; /**< group this evaluationID belongs to \b (optional), translated to a human-readable version, using evalGroupLabels */
            int64_t groupIndex; /**< index of the results inside the evaluationID group set by \a group \b (optional) */
            QString description; /**< description of the metadata (human-readable version of the actual ID, \b optional )  */
            ResultsResultsType results; /**< the real results */
        };

        typedef QMap<QString, evaluationIDMetadata* > ResultsType;
        typedef QMapIterator<QString, evaluationIDMetadata* > ResultsIterator;

        /** \brief evaluation results are stored in this QHash which maps an evaluation name to
         *         to a list of evaluation results (which is indexed by result names! */
        ResultsType results;

        typedef QMap<QString, QString > GroupLabelsType;
        typedef QMapIterator<QString, QString > GroupLabelsIterator;


        /** \brief maps evaluationIDMetadata.group to a human-readable version */
        GroupLabelsType evalGroupLabels;

};

QFRawDataRecordPrivate::evaluationIDMetadata::evaluationIDMetadata(int initsize) {
    results.reserve(initsize);
}

QFRawDataRecord::QFRawDataRecord(QFProject* parent):
    QObject(parent), QFProperties()
{
    project=parent;
    errorOcc=false;
    errorDesc="";
    name="";
    folder="";
    description="";
    resultsmodel=NULL;
    propModel=NULL;
    doEmitResultsChanged=true;
    evaluationIDMetadataInitSize=100;
    dstore=new QFRawDataRecordPrivate();
    setResultsInitSize(100);
}

void QFRawDataRecord::setResultsInitSize(int initSize) {
    //if (initSize>dstore->results.capacity()) dstore->results.reserve(initSize);
}

void QFRawDataRecord::setEvaluationIDMetadataInitSize(int initSize) {
    evaluationIDMetadataInitSize=initSize;
}


QFRDRPropertyModel* QFRawDataRecord::getPropertyModel() {
    if (propModel==NULL) {
        propModel=new QFRDRPropertyModel();
        propModel->init(this);
    }
    return propModel;
}

QFRDRResultsModel* QFRawDataRecord::resultsGetModel() {
    if (resultsmodel==NULL) {
        resultsmodel=new QFRDRResultsModel();
        resultsmodel->init(this);
    }
    return resultsmodel;
};

void QFRawDataRecord::init(QString name, QStringList inputFiles, QStringList inputFilesTypes) {
    this->ID=project->getNewID();
    this->name=name;
    description="";
    files=inputFiles;
    files_types=inputFilesTypes;
    intReadData();
    //std::cout<<"after intReadData() in init ...\n";
    project->registerRawDataRecord(this);
    //std::cout<<"init ended ...\n";
}


void QFRawDataRecord::init(QDomElement& e) {
    //std::cout<<"creating QFRawDataRecord\n";
    name="";;
    description="";
    //std::cout<<"  reading XML\n";
    readXML(e);
    //std::cout<<"  registering record\n";
    project->registerRawDataRecord(this);
    //std::cout<<"created QFRawDataRecord\n";
}

QFRawDataRecord::~QFRawDataRecord() {
    delete dstore;
    //std::cout<<"deleting QFRawDataRecord\n";
    //std::cout<<"deleting QFRawDataRecord ... OK\n";
}

void QFRawDataRecord::setName(const QString n) {
    name=n;
    emitPropertiesChanged();
}
/** \brief set the folder */
void QFRawDataRecord::setFolder(const QString n) {
    folder=n;
    emitPropertiesChanged();
    if (project) project->setStructureChanged();
}
/** \brief set the description  */
void QFRawDataRecord::setDescription(const QString& d) {
    description=d;
    emitPropertiesChanged();
};

void QFRawDataRecord::readXML(QDomElement& e) {
    bool ok=true;
    name=e.attribute("name", "rawdatarecord");
    ID=e.attribute("id", "-1").toInt(&ok);
    if (ID==-1) { setError(tr("invalid ID in <rawdatarecord name=\"%1\" ...>!").arg(name)); return; }
    if (!project->checkID(ID)) {
        setError(tr("ID %1 in <rawdatarecord name=\"%2\" ...> already in use in the project!").arg(ID).arg(name));
        return;
    }
    folder=e.attribute("folder", "");
    QDomElement te=e.firstChildElement("description");
    if (te.isNull()) { description=""; } else { description=te.text(); }
    //std::cout<<"    reading XML: properties\n";
    props.clear();
    te=e.firstChildElement("properties");
    readProperties(te);

    resultsClearAll();
    te=e.firstChildElement("results");
    if (!te.isNull()) {
        te=te.firstChildElement("evaluation");
        while (!te.isNull()) {
            QString en=te.attribute("name");
            QString group=te.attribute("group");
            int groupIndex=te.attribute("groupindex", "0").toInt();
            QString description=te.attribute("description");
            QDomElement re=te.firstChildElement("result");
            dstore->results[en]=new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
            dstore->results[en]->group=group;
            dstore->results[en]->groupIndex=groupIndex;
            dstore->results[en]->description=description;
            QLocale loc=QLocale::c();
            loc.setNumberOptions(QLocale::OmitGroupSeparator);
            while (!re.isNull()) {
                QString n=re.attribute("name", "");
                QString t=re.attribute("type", "invalid");
                evaluationResult r;
                r.group=re.attribute("group", "");
                r.label=re.attribute("label", "");
                r.label_rich=re.attribute("labelrich", "");
                r.sortPriority=QVariant(re.attribute("sortprior", "false")).toBool();
                //r.name=n;
                r.type=qfrdreInvalid;
                if (t=="boolean"  || t=="bool") {
                    r.type=qfrdreBoolean;
                    r.bvalue=QVariant(re.attribute("value", "false")).toBool();
                } else if (t=="integer" || t=="int") {
                    r.type=qfrdreInteger;
                    r.ivalue=loc.toInt(re.attribute("value", "0"));
                    r.unit=re.attribute("unit", "");
                } else if (t=="string" || t=="str") {
                    r.type=qfrdreString;
                    r.svalue=re.attribute("value", "");
                } else if (t=="number" || t=="num") {
                    r.type=qfrdreNumber;
                    r.dvalue=loc.toDouble(re.attribute("value", "0.0"));
                    r.unit=re.attribute("unit", "");
                } else if (t=="numberlist" || t=="numlst") {
                    r.type=qfrdreNumberVector;
                    r.dvec.clear();
                    QStringList s=re.attribute("value", "0.0").split(";");
                    for (int i=0; i<s.size(); i++) {
                        bool ok=false;
                        double d=loc.toDouble(s[i], &ok);
                        if (ok) { r.dvec.append(d); }
                        else { r.dvec.append(0); }
                    }
                    r.unit=re.attribute("unit", "");
                } else if (t=="numbermatrix" || t=="nummtrx") {
                    r.type=qfrdreNumberVector;
                    r.dvec.clear();
                    QStringList s=re.attribute("value", "0.0").split(";");
                    for (int i=0; i<s.size(); i++) {
                        bool ok=false;
                        double d=loc.toDouble(s[i], &ok);
                        if (ok) { r.dvec.append(d); }
                        else { r.dvec.append(0); }
                    }
                    r.unit=re.attribute("unit", "");
                    r.columns=loc.toInt(re.attribute("columns", "0"));
                } else if (t=="numbererror" || t=="numerr") {
                    r.type=qfrdreNumberError;
                    r.dvalue=loc.toDouble(re.attribute("value", "0.0"));
                    r.derror=loc.toDouble(re.attribute("error", "0.0"));
                    r.unit=re.attribute("unit", "");
                }
                if (!n.isEmpty() && !en.isEmpty()) dstore->results[en]->results.insert(n, r);
                re=re.nextSiblingElement("result");
            }

            te = te.nextSiblingElement("evaluation");
        }
    }

    //std::cout<<"    reading XML: files\n";
    te=e.firstChildElement("evalgrouplabels");
    dstore->evalGroupLabels.clear();
    if (!te.isNull()) {
        QDomElement fe=te.firstChildElement("group");
        while (!fe.isNull()) {
            dstore->evalGroupLabels[fe.attribute("id")]=fe.attribute("label");
            fe=fe.nextSiblingElement("group");
        }
    }


    te=e.firstChildElement("files");
    files.clear();
    if (!te.isNull()) {
        QDomElement fe=te.firstChildElement("file");
        while (!fe.isNull()) {
            QString filexml=fe.text();
            QString typexml=fe.attribute("type", "");
            QFileInfo fi(project->getFile());
            //std::cout<<"file = "<<filexml.toStdString()<<"\n";
            //std::cout<<"  project-absolute path = "<<fi.absoluteDir().absolutePath().toStdString()<<"\n";
            //std::cout<<"  file-absolute path = "<<fi.absoluteDir().absoluteFilePath(filexml).toStdString()<<"\n";

            files.push_back(fi.absoluteDir().absoluteFilePath(filexml));
            files_types.append(typexml);
            fe=fe.nextSiblingElement("file");
        }
    }
    //std::cout<<"    reading XML: data\n";
    te=e.firstChildElement("data");
    intReadData(&te);
    //std::cout<<"reading XML: done!\n";
    if (!errorOcc) {
        emit propertiesChanged();
        emit rawDataChanged();
    }
    if (resultsmodel) resultsmodel->init(this);
}


void QFRawDataRecord::writeXML(QXmlStreamWriter& w) {
    w.writeStartElement("rawdataelement");
    w.writeAttribute("type", getType());
    w.writeAttribute("id", QString::number(ID));
    w.writeAttribute("name", name);
    w.writeAttribute("folder", folder);
    w.writeStartElement("description");
    w.writeCDATA(description);
    w.writeEndElement();
    w.writeStartElement("properties");
    storeProperties(w);
    w.writeEndElement();
    w.writeStartElement("results");
    QFRawDataRecordPrivate::ResultsIterator i(dstore->results);
    while (i.hasNext()) {
    //for (int i=0; i<results.keys().size(); i++) {
        i.next();
        w.writeStartElement("evaluation");
        QString n=i.key();
        w.writeAttribute("name", n);
        w.writeAttribute("group", i.value()->group);
        w.writeAttribute("groupindex", QString::number(i.value()->groupIndex));
        w.writeAttribute("description", i.value()->description);
        QFRawDataRecordPrivate::ResultsResultsIterator  j(i.value()->results);
        //for (int j=0; j<i.value().size(); j++) {
        while (j.hasNext()) {
            j.next();
            w.writeStartElement("result");
            QString rn=j.key();
            w.writeAttribute("name", rn);            
            const evaluationResult& r=j.value();
            if (!r.label.isEmpty()) w.writeAttribute("label", r.label);
            if (!r.group.isEmpty()) w.writeAttribute("group", r.group);
            if (!r.label_rich.isEmpty()) w.writeAttribute("labelrich", r.label_rich);
            if (r.sortPriority) w.writeAttribute("sortprior", (r.sortPriority)?QString("true"):QString("false"));
            QLocale loc=QLocale::c();
            loc.setNumberOptions(QLocale::OmitGroupSeparator);
            switch(r.type) {
                case qfrdreInvalid:
                    w.writeAttribute("type", "invalid");
                    break;
                case qfrdreBoolean:
                    w.writeAttribute("type", "boolean");
                    w.writeAttribute("value", (r.bvalue)?QString("true"):QString("false"));
                    break;
                case qfrdreInteger:
                    w.writeAttribute("type", "integer");
                  #ifdef Q_OS_WIN32
                    w.writeAttribute("value", loc.toString(r.ivalue));
                  #else
                    w.writeAttribute("value", loc.toString(r.ivalue));
                  #endif
                    w.writeAttribute("unit", r.unit);
                    break;
                case qfrdreString:
                    w.writeAttribute("type", "string");
                    w.writeAttribute("value", r.svalue);
                    break;
                case qfrdreNumber:
                    w.writeAttribute("type", "number");
                    w.writeAttribute("value", loc.toString(r.dvalue, 'g', 10));
                    w.writeAttribute("unit", r.unit);
                    break;
                case qfrdreNumberVector: {
                    w.writeAttribute("type", "numberlist");
                    QString s="";
                    for (int i=0; i<r.dvec.size(); i++) {
                        if (i>0) s=s+";";
                        s=s+loc.toString(r.dvec[i], 'g', 10);
                    }
                    w.writeAttribute("value", s);
                    w.writeAttribute("unit", r.unit);
                    } break;
                case qfrdreNumberMatrix: {
                    w.writeAttribute("type", "numbermatrix");
                    QString s="";
                    for (int i=0; i<r.dvec.size(); i++) {
                        if (i>0) s=s+";";
                        s=s+loc.toString(r.dvec[i], 'g', 10);
                    }
                    w.writeAttribute("value", s);
                    w.writeAttribute("unit", r.unit);
                    w.writeAttribute("columns", loc.toString(r.columns));
                    } break;
                case qfrdreNumberError:
                    w.writeAttribute("type", "numbererror");
                    w.writeAttribute("value", loc.toString(r.dvalue, 'g', 10));
                    w.writeAttribute("error", loc.toString(r.derror, 'g', 10));
                    w.writeAttribute("unit", r.unit);
                    break;

            }
            w.writeEndElement();
        }
        w.writeEndElement();
    }
    w.writeEndElement();

    if (dstore->evalGroupLabels.size()>0) {
        w.writeStartElement("evalgrouplabels");
        QFRawDataRecordPrivate::GroupLabelsIterator i(dstore->evalGroupLabels);
        while (i.hasNext()) {
            i.next();
            w.writeStartElement("group");
            w.writeAttribute("id", i.key());
            w.writeAttribute("label", i.value());
            w.writeEndElement();
        }
        w.writeEndElement();
    }
    if (files.size()>0) {
        w.writeStartElement("files");
        for (int i=0; i< files.size(); i++) {
            QString file=files[i];
            QFileInfo fi(project->getFile());
            file=fi.absoluteDir().relativeFilePath(files[i]);
            w.writeStartElement("file");
            if (i<files_types.size()) {
                if (!files_types[i].isEmpty()) {
                    w.writeAttribute("type", files_types[i]);
                }
            }
            w.writeCharacters(file);
            w.writeEndElement();
        }
        w.writeEndElement();
    }
    w.writeStartElement("data");
    intWriteData(w);
    w.writeEndElement();
    w.writeEndElement();
}



void QFRawDataRecord::resultsClearAll() {
    QFRawDataRecordPrivate::ResultsIterator j(dstore->results);
    while (j.hasNext()) {
        j.next();
        delete j.value();
    }
    dstore->results.clear();
    emitResultsChanged();
}

void QFRawDataRecord::resultsClear(QString name) {
    if (dstore->results.contains(name)) {
        dstore->results[name]->results.clear();
        delete dstore->results[name];
        dstore->results.remove(name);
        emitResultsChanged();
    }
};

int QFRawDataRecord::resultsGetCount(QString evalName) const {
    if (dstore->results.contains(evalName)) return dstore->results[evalName]->results.size();
    return 0;
};

int QFRawDataRecord::resultsGetEvaluationCount() const {
    return dstore->results.size();
};

QString QFRawDataRecord::resultsGetEvaluationName(int i) const {
    if ((long)i<dstore->results.size()) return dstore->results.keys().at(i);
    return QString("");
};

void QFRawDataRecord::resultsSetNumber(QString evaluationName, QString resultName, double value, QString unit) {
    evaluationResult r;
    r.type=qfrdreNumber;
    r.dvalue=value;
    r.unit=unit;
    if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
    dstore->results[evaluationName]->results.insert(resultName, r);
    emitResultsChanged();
};

void QFRawDataRecord::resultsRemove(QString evalName, QString resultName, bool emitChangedSignal) {
    if (dstore->results.contains(evalName)) {
        bool changed=false;
        if (dstore->results[evalName]->results.remove(resultName)>0) changed=true;
        if (dstore->results[evalName]->results.isEmpty()) {
            resultsClear(evalName);
            changed=true;
        }
        if (changed && emitChangedSignal && doEmitResultsChanged) emit resultsChanged();
    }
}

void QFRawDataRecord::resultsClear(QString name, QString postfix) {
    if (dstore->results.contains(name)) {
        QFRawDataRecordPrivate::ResultsResultsIterator i(dstore->results[name]->results);
        while (i.hasNext()) {
            i.next();
            //cout << i.key() << ": " << i.value() << endl;
            if (i.key().endsWith(postfix)) dstore->results[name]->results.remove(i.key());
        }
        emitResultsChanged();
    }
}

bool QFRawDataRecord::resultsExists(QString evalName, QString resultName) const {
    if (dstore->results.contains(evalName)) {
        return dstore->results[evalName]->results.contains(resultName);
    }
    return false;
}

bool QFRawDataRecord::resultsExistsFromEvaluation(QString evalName) const {
    return dstore->results.contains(evalName);
};

void QFRawDataRecord::resultsSetNumberList(QString evaluationName, QString resultName, QVector<double>& value, QString unit) {
    evaluationResult r;
    r.type=qfrdreNumberVector;
    r.dvec=value;
    r.unit=unit;
    if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
    dstore->results[evaluationName]->results.insert(resultName, r);
    emitResultsChanged();
};

void QFRawDataRecord::resultsSetNumberMatrix(QString evaluationName, QString resultName, QVector<double>& value, int columns, QString unit) {
    evaluationResult r;
    r.type=qfrdreNumberMatrix;
    r.dvec=value;
    r.unit=unit;
    r.columns=columns;
    if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
    dstore->results[evaluationName]->results.insert(resultName, r);
    emitResultsChanged();
};

void QFRawDataRecord::resultsSetNumberList(QString evaluationName, QString resultName, double* value, int items, QString unit) {
    QVector<double> data;
    for (int i=0; i<items; i++) {
        data.append(value[i]);
    }
    resultsSetNumberList(evaluationName, resultName, data, unit);
}

void QFRawDataRecord::resultsSetNumberMatrix(QString evaluationName, QString resultName, double* value, int columns, int rows, QString unit) {
    QVector<double> data;
    for (int i=0; i<columns*rows; i++) {
        data.append(value[i]);
    }
    resultsSetNumberMatrix(evaluationName, resultName, data, columns, unit);
}



void QFRawDataRecord::resultsSetNumberError(QString evaluationName, QString resultName, double value, double error, QString unit)  {
    evaluationResult r;
    r.type=qfrdreNumberError;
    r.dvalue=value;
    r.derror=error;
    r.unit=unit;
    if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
    dstore->results[evaluationName]->results.insert(resultName, r);
    emitResultsChanged();
};

void QFRawDataRecord::resultsSetInteger(QString evaluationName, QString resultName, int64_t value, QString unit) {
    evaluationResult r;
    r.type=qfrdreInteger;
    r.ivalue=value;
    r.unit=unit;
    if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
    dstore->results[evaluationName]->results.insert(resultName, r);
    emitResultsChanged();
};

void QFRawDataRecord::resultsSetString(QString evaluationName, QString resultName, QString value) {
    evaluationResult r;
    r.type=qfrdreString;
    r.svalue=value;
    if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
    dstore->results[evaluationName]->results.insert(resultName, r);
    emitResultsChanged();
};

void QFRawDataRecord::resultsSetLabel(QString evaluationName, QString resultName, QString label, QString label_rich) {
    if (!resultsExists(evaluationName, resultName)) return;
    evaluationResult r=resultsGet(evaluationName, resultName);
    r.label=label;
    r.label_rich=label_rich;
    if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
    dstore->results[evaluationName]->results[resultName]=r;
    emitResultsChanged();
}

void QFRawDataRecord::resultsSetGroup(QString evaluationName, QString resultName, QString group) {
    if (!resultsExists(evaluationName, resultName)) return;
    evaluationResult r=resultsGet(evaluationName, resultName);
    r.group=group;
    if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
    dstore->results[evaluationName]->results[resultName]=r;
    emitResultsChanged();
}

void QFRawDataRecord::resultsSetSortPriority(QString evaluationName, QString resultName, bool pr) {
    if (!resultsExists(evaluationName, resultName)) return;
    evaluationResult r=resultsGet(evaluationName, resultName);
    r.sortPriority=pr;
    if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
    dstore->results[evaluationName]->results[resultName]=r;
    emitResultsChanged();
}

bool QFRawDataRecord::resultsGetSortPriority(QString evaluationName, QString resultName) const {
    const evaluationResult& r=resultsGet(evaluationName, resultName);
    return r.sortPriority;
}

void QFRawDataRecord::resultsSetEvaluationGroupIndex(QString evaluationName, int64_t groupIndex) {
    if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
    dstore->results[evaluationName]->groupIndex=groupIndex;
}

int64_t QFRawDataRecord::resultsGetEvaluationGroupIndex(QString evaluationName) const {
    if (dstore->results.contains(evaluationName)) {
        return dstore->results[evaluationName]->groupIndex;
    }
    return -1;
}

void QFRawDataRecord::resultsSetEvaluationGroupLabel(QString evalGroup, QString label) {
    dstore->evalGroupLabels[evalGroup]=label;
}

QString QFRawDataRecord::resultsGetLabelForEvaluationGroup(QString evalGroup) const {
    if (dstore->evalGroupLabels.contains(evalGroup)) {
        return dstore->evalGroupLabels[evalGroup];
    } else {
        return evalGroup;
    }
}

void QFRawDataRecord::resultsSetEvaluationGroup(QString evaluationName, QString group) {
    if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
    dstore->results[evaluationName]->group=group;
}

QString QFRawDataRecord::resultsGetEvaluationGroup(QString evaluationName) const {
    if (dstore->results.contains(evaluationName)) {
        return dstore->results[evaluationName]->group;
    }
    return "";
}

QString QFRawDataRecord::resultsGetEvaluationGroupLabel(QString evaluationName) const {
    if (dstore->results.contains(evaluationName)) {
        return resultsGetLabelForEvaluationGroup(dstore->results[evaluationName]->group);
    }
    return "";
}

void QFRawDataRecord::resultsSetEvaluationDescription(QString evaluationName, QString description) {
    if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
    dstore->results[evaluationName]->description=description;
}

QString QFRawDataRecord::resultsGetEvaluationDescription(QString evaluationName) const {
    if (dstore->results.contains(evaluationName)) {
        QString g=dstore->results[evaluationName]->description;
        if (g.isEmpty()) return evaluationName;
        else return g;
    }
    return "";
}

void QFRawDataRecord::resultsSetBoolean(QString evaluationName, QString resultName, bool value) {
    evaluationResult r;
    r.type=qfrdreBoolean;
    r.bvalue=value;
    if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
    dstore->results[evaluationName]->results.insert(resultName, r);
    emitResultsChanged();
};

void QFRawDataRecord::resultsSet(QString evaluationName, QString resultName, const evaluationResult& result) {
    if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
    dstore->results[evaluationName]->results.insert(resultName, result);
    emitResultsChanged();
}

QFRawDataRecord::evaluationResult QFRawDataRecord::resultsGet(QString evalName, QString resultName) const
{
    evaluationResult r;
    if (resultsExists(evalName, resultName)) {
        return dstore->results[evalName]->results.value(resultName);
    }
    return r;
}

QFRawDataRecord::evaluationResultType QFRawDataRecord::resultsGetType(QString evalName, QString resultName) const {
    if (resultsExists(evalName, resultName)) {
        return dstore->results[evalName]->results.value(resultName).type;
    }
    return qfrdreInvalid;
}

QVariant QFRawDataRecord::resultsGetAsQVariant(QString evalName, QString resultName) const {
    QVariant result;
    const evaluationResult& r=resultsGet(evalName, resultName);
    switch(r.type) {
        case qfrdreBoolean: result=r.bvalue; break;
        case qfrdreInteger: result=(qlonglong)r.ivalue; break;
        case qfrdreNumberError: result=QPointF(r.dvalue, r.derror); break;
        case qfrdreNumber: result=r.dvalue; break;
        case qfrdreNumberMatrix:
        case qfrdreNumberVector: {
            QList<QVariant> data;
            for (int i=0; i<r.dvec.size(); i++) {
                data.append(r.dvec[i]);
            }
            result=data;
            break;
        }
        case qfrdreString: result=r.svalue; break;
        default: result=QVariant(); break;
    }
    return result;
}

QString QFRawDataRecord::resultsGetAsString(QString evalName, QString resultName) const {
    const evaluationResult& r=resultsGet(evalName, resultName);
    switch(r.type) {
        case qfrdreBoolean: if (r.bvalue) return tr("true"); else return tr("false");
        case qfrdreInteger: return tr("%1 %2").arg(r.ivalue).arg(r.unit);
        case qfrdreNumber: return tr("%1 %2").arg(r.dvalue).arg(r.unit);
        case qfrdreNumberVector: {
            QString s="(";
            for (int i=0; i<r.dvec.size(); i++) {
                if (i>0) s=s+"; ";
                s=s+QString::number(r.dvec[i]);
            }
            return s+") "+r.unit;
        }
        case qfrdreNumberMatrix: {
            QString s="(";
            for (int i=0; i<r.dvec.size(); i++) {
                if (i>0) {
                    if (i%r.columns==0) s=s+";; ";
                    else s=s+"; ";
                }
                s=s+QString::number(r.dvec[i]);
            }
            return s+") "+r.unit;
        }
        case qfrdreNumberError: return tr("(%1 +/- %2) %3").arg(r.dvalue).arg(r.derror).arg(r.unit);
        case qfrdreString: return r.svalue;
        default: return ("");
    }
    return QString("");
}

QVariant QFRawDataRecord::resultsGetAsStringVariant(QString evalName, QString resultName) const {
    const evaluationResult& r=resultsGet(evalName, resultName);
    switch(r.type) {
        case qfrdreBoolean: if (r.bvalue) return QVariant(tr("true")); else return QVariant(tr("false"));
        case qfrdreInteger: return QVariant(QString("%1 %2").arg(r.ivalue).arg(r.unit));
        case qfrdreNumber: return QVariant(QString("%1 %2").arg(r.dvalue).arg(r.unit));
        case qfrdreNumberVector: {
            QString s="(";
            for (int i=0; i<r.dvec.size(); i++) {
                if (i>0) s=s+"; ";
                s=s+QString::number(r.dvec[i]);
            }
            return QVariant(s+") "+r.unit);
        }
        case qfrdreNumberMatrix: {
            QString s="(";
            for (int i=0; i<r.dvec.size(); i++) {
                if (i>0) {
                    if (i%r.columns==0) s=s+";; ";
                    else s=s+"; ";
                }
                s=s+QString::number(r.dvec[i]);
            }
            return QVariant(s+") "+r.unit);
        }
        case qfrdreNumberError: return QVariant(QString("(%1 +/- %2) %3").arg(r.dvalue).arg(r.derror).arg(r.unit));
        case qfrdreString: return QVariant(r.svalue);
        default: return QVariant();
    }
    return QVariant();
}

double QFRawDataRecord::resultsGetAsDouble(QString evalName, QString resultName, bool* ok) const {
    const evaluationResult& r=resultsGet(evalName, resultName);
    if (ok) *ok=true;
    switch(r.type) {
        case qfrdreBoolean: if (r.bvalue) return 1; else return 0;
        case qfrdreInteger: return r.ivalue;
        case qfrdreNumber: case qfrdreNumberError: return r.dvalue;
        case qfrdreString: return r.svalue.toDouble(ok);
        default: if (ok) *ok=false;
                 return 0.0;

    }
    if (ok) *ok=false;
    return 0.0;
}

int64_t QFRawDataRecord::resultsGetAsInteger(QString evalName, QString resultName, bool* ok) const {
    const evaluationResult& r=resultsGet(evalName, resultName);
    if (ok) *ok=true;
     switch(r.type) {
        case qfrdreBoolean: if (r.bvalue) return 1; else return 0;
        case qfrdreInteger: return r.ivalue;
        case qfrdreNumber: case qfrdreNumberError: return r.dvalue;
        case qfrdreString: return r.svalue.toInt(ok);
        default: if (ok) *ok=false;
                 return 0.0;
    }
    if (ok) *ok=false;
    return 0.0;
}

bool QFRawDataRecord::resultsGetAsBoolean(QString evalName, QString resultName, bool* ok) const {
    const evaluationResult& r=resultsGet(evalName, resultName);
    if (ok) *ok=true;
     switch(r.type) {
        case qfrdreBoolean: return r.bvalue;
        case qfrdreInteger: return r.ivalue!=0;
        case qfrdreNumber: case qfrdreNumberError: return r.dvalue!=0.0;
        case qfrdreString: return QStringToBool(r.svalue);
        default: if (ok) *ok=false;
                 return false;
    }
    if (ok) *ok=false;
    return false;
}

QVector<double> QFRawDataRecord::resultsGetAsDoubleList(QString evalName, QString resultName, bool* ok) const {
    const evaluationResult& r=resultsGet(evalName, resultName);
    if (ok) *ok=true;
    switch(r.type) {
        case qfrdreNumberVector:
        case qfrdreNumberMatrix: return r.dvec;
        default: if (ok) *ok=false;
                 return QVector<double>();
    }
    if (ok) *ok=false;
    return QVector<double>();
}

double QFRawDataRecord::resultsGetErrorAsDouble(QString evalName, QString resultName, bool* ok) const {
    const evaluationResult& r=resultsGet(evalName, resultName);
    if (ok) *ok=true;
    switch(r.type) {
        case qfrdreNumber: case qfrdreNumberError: return r.derror;
        default: if (ok) *ok=false;
                 return 0.0;
    }
    if (ok) *ok=false;
    return 0.0;
};



QString QFRawDataRecord::resultsGetLabel(QString evaluationName, QString resultName) const {
    const evaluationResult& r=resultsGet(evaluationName, resultName);
    if (r.label.isEmpty()) return resultName;
    else return r.label;
}


QString QFRawDataRecord::resultsGetGroup(QString evaluationName, QString resultName) const {
    const evaluationResult& r=resultsGet(evaluationName, resultName);
    return r.group;
}

QString QFRawDataRecord::resultsGetLabelRichtext(QString evaluationName, QString resultName) const {
    const evaluationResult& r=resultsGet(evaluationName, resultName);
    if (r.label_rich.isEmpty()) return r.label;
    else if (r.label_rich.isEmpty()) return resultName;
    else return r.label_rich;
}


QString QFRawDataRecord::resultsGetResultName(QString evaluationName, int i) const {
    if (dstore->results.contains(evaluationName)) {
        QList<QString> r=dstore->results[evaluationName]->results.keys();
        if (i<r.size()) {
            return r.at(i);
        }
    }
    return QString("");
}

void QFRawDataRecord::resultsCopy(QString oldEvalName, QString newEvalName) {
    if (resultsExistsFromEvaluation(oldEvalName)) {
        QFRawDataRecordPrivate::ResultsResultsIterator i(dstore->results[oldEvalName]->results);
        while (i.hasNext()) {
            i.next();
            if (!dstore->results.contains(newEvalName)) dstore->results[newEvalName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
            dstore->results[newEvalName]->results.insert(i.key(), i.value());
        }
        emitResultsChanged();
    }
}

bool QFRawDataRecord::resultsSaveToCSV(QString filename, QString separator, QChar decimalPoint, QChar stringDelimiter) {
    QString sdel=stringDelimiter;
    QString dp=decimalPoint;
    QList<QPair<QString,QString> > rownames=resultsCalcNamesAndLabels();
    QStringList header, data;
    header.append(sdel+tr("datafield")+sdel);
    header.append("");
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    for (int i=0; i<rownames.size(); i++) data.append(sdel+rownames[i].first+sdel);
    for (int c=0; c<resultsGetEvaluationCount(); c++) {
        QString evalname=resultsGetEvaluationName(c);
        header[0] += separator+sdel+evalname+sdel;
        header[1] += separator+sdel+tr("value")+sdel;
        bool hasError=false;
        for (int r=0; r<rownames.size(); r++) {
            QString dat="";
            if (resultsExists(evalname, rownames[r].second)) {
                switch(resultsGet(evalname, rownames[r].second).type) {
                    case qfrdreNumber: dat=doubleToQString(resultsGetAsDouble(evalname, rownames[r].second), 15, 'g', decimalPoint); break;
                    case qfrdreNumberError: dat=doubleToQString(resultsGetAsDouble(evalname, rownames[r].second), 15, 'g', decimalPoint); hasError=true; break;
                    case qfrdreInteger: dat=loc.toString((qlonglong)resultsGetAsInteger(evalname, rownames[r].second)); break;
                    case qfrdreBoolean: dat=(resultsGetAsBoolean(evalname, rownames[r].second))?QString("1"):QString("0"); break;
                    case qfrdreString: dat=stringDelimiter+resultsGetAsString(evalname, rownames[r].second).replace(stringDelimiter, "\\"+QString(stringDelimiter)).replace('\n', "\\n").replace('\r', "\\r")+stringDelimiter; break;
                    case qfrdreNumberVector:
                    case qfrdreNumberMatrix: dat=stringDelimiter+resultsGetAsString(evalname, rownames[r].second).replace(stringDelimiter, "\\"+QString(stringDelimiter)).replace('\n', "\\n").replace('\r', "\\r")+stringDelimiter; break;
                    default: break;
                }
            }
            data[r]+=separator+dat;
        }
        if (hasError) {
            header[0] += separator;
            header[1] += separator+sdel+tr("error")+sdel;
            for (int r=0; r<rownames.size(); r++) {
                QString dat="";
                if (resultsExists(evalname, rownames[r].second)) {
                    if (resultsGet(evalname, rownames[r].second).type==qfrdreNumberError) {
                        dat=doubleToQString(resultsGetErrorAsDouble(evalname, rownames[r].second), 15, 'g', decimalPoint);
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


bool QFRawDataRecord::resultsSaveToSYLK(QString filename) {
    // try output SYLK file
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("ISO-8859-1"));
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    out.setLocale(loc);


    // write SYLK header
    out<<"ID;P\n";

    QChar stringDelimiter='"';
    QList<QPair<QString,QString> > rownames=resultsCalcNamesAndLabels();
    // write column headers
    out<<QString("C;Y1;X1;K\"%1\"\n").arg(tr("datafield"));
    for (int r=0; r<rownames.size(); r++) {
        out<<QString("C;Y%2;X1;K\"%1\"\n").arg(rownames[r].first).arg(r+3);
        //out<<QString("F;Y%2;X1;SDB\n").arg(r+2);
    }

    int col=2;
    for (int c=0; c<resultsGetEvaluationCount(); c++) {
        QString evalname=resultsGetEvaluationName(c);
        out<<QString("C;Y1;X%2;K\"%1\"\n").arg(evalname).arg(col);
        //out<<QString("F;Y1;X%2;SDB\n").arg(col);
        out<<QString("C;Y2;X%2;K\"%1\"\n").arg(tr("value")).arg(col);
        //out<<QString("F;Y2;X%2;SDB\n").arg(col);
        bool hasError=false;
        for (int r=0; r<rownames.size(); r++) {
            QString dat="";
            if (resultsExists(evalname, rownames[r].second)) {
                switch(resultsGet(evalname, rownames[r].second).type) {
                    case qfrdreNumber: dat=CDoubleToQString(resultsGetAsDouble(evalname, rownames[r].second)); break;
                    case qfrdreNumberError: dat=CDoubleToQString(resultsGetAsDouble(evalname, rownames[r].second)); hasError=true; break;
                    case qfrdreInteger: dat=loc.toString((qlonglong)resultsGetAsInteger(evalname, rownames[r].second)); break;
                    case qfrdreBoolean: dat=(resultsGetAsBoolean(evalname, rownames[r].second))?QString("1"):QString("0"); break;
                    case qfrdreString: dat=stringDelimiter+resultsGetAsString(evalname, rownames[r].second).replace(stringDelimiter, "\\"+QString(stringDelimiter)).replace('\n', "\\n").replace('\r', "\\r").replace(';', ",").replace('\"', "_")+stringDelimiter; break;
                    case qfrdreNumberVector:
                    case qfrdreNumberMatrix: dat=stringDelimiter+resultsGetAsString(evalname, rownames[r].second).replace(stringDelimiter, "\\"+QString(stringDelimiter)).replace('\n', "\\n").replace('\r', "\\r")+stringDelimiter; break;
                    default: break;
                }
            }
            if (!dat.isEmpty()) out<<QString("C;X%1;Y%2;N;K%3\n").arg(col).arg(r+3).arg(dat);
        }
        if (hasError) {
            col++;
            out<<QString("C;Y2;X%2;K\"%1\"\n").arg(tr("error")).arg(col);
            //out<<QString("F;Y2;X%2;SDB\n").arg(col);
            for (int r=0; r<rownames.size(); r++) {
                QString dat="";
                if (resultsExists(evalname, rownames[r].second)) {
                    if (resultsGet(evalname, rownames[r].second).type==qfrdreNumberError) {
                        dat=CDoubleToQString(resultsGetErrorAsDouble(evalname, rownames[r].second));;
                    }
                }
                if (!dat.isEmpty()) out<<QString("C;X%1;Y%2;N;K%3\n").arg(col).arg(r+3).arg(dat);
            }
        }
        col++;
    }
    return true;
}


bool QFRawDataRecord_StringPairCaseInsensitiveCompare(const QPair<QString, QString> &s1, const QPair<QString, QString> &s2) {
    return s1.first.toLower() < s2.first.toLower();
}

bool QFRawDataRecord_StringTripleCaseInsensitiveCompare(const QTriple<QString, QString, QString> &s1, const QTriple<QString, QString, QString> &s2) {
    return s1.first.toLower() < s2.first.toLower();
}

bool QFRawDataRecord_StringPairCaseInsensitiveCompareSecond(const QPair<QString, QString> &s1, const QPair<QString, QString> &s2) {
    return s1.second.toLower() < s2.second.toLower();
}

bool QFRawDataRecord_StringTripleCaseInsensitiveCompareSecond(const QTriple<QString, QString, QString> &s1, const QTriple<QString, QString, QString> &s2) {
    return s1.second.toLower() < s2.second.toLower();
}

bool QFRawDataRecord_StringTripleCaseInsensitiveCompareThird(const QTriple<QString, QString, QString> &s1, const QTriple<QString, QString, QString> &s2) {
    return s1.third.toLower() < s2.third.toLower();
}








QList<QString> QFRawDataRecord::resultsCalcNames(const QString& evalName, const QString& group, const QString& evalgroup) const {
    QStringList l, lp;

    QFRawDataRecordPrivate::ResultsIterator i(dstore->results);
    while (i.hasNext()) {
        i.next();
        QString en=i.key();

        QString egrp=i.value()->group;
        if ((evalName.isEmpty() || (en==evalName)) && (evalgroup.isEmpty() || (egrp==evalgroup))) {
            QFRawDataRecordPrivate::ResultsResultsIterator j(i.value()->results);
            while (j.hasNext()) {
                j.next();
                QString rn=j.key();
                if ((group.isEmpty() || (group==j.value().group)) && (!l.contains(rn))) {
                    if (j.value().sortPriority) lp.append(rn);
                    else l.append(rn);
                }
            }
        }
    }
    if (l.size()>0) l.sort();
    if (lp.size()>0) lp.sort();
    lp.append(l);
    return lp;
}


QList<QString> QFRawDataRecord::resultsCalcGroups(const QString& evalName) const {
    QStringList l;
    QFRawDataRecordPrivate::ResultsIterator i(dstore->results);
    while (i.hasNext()) {
        i.next();
        QString en=i.key();
        if (evalName.isEmpty() || (en==evalName)) {
            QFRawDataRecordPrivate::ResultsResultsIterator j(i.value()->results);
            while (j.hasNext()) {
                j.next();
                QString rn=j.key();
                QString g=j.value().group;
                if (!l.contains(g)) {
                    l.append(g);
                }
            }
        }
    }
    if (l.size()>0) l.sort();
    return l;
}


QList<QPair<QString, QString> > QFRawDataRecord::resultsCalcNamesAndLabels(const QString& evalName, const QString& group, const QString& evalgroup) const {
    QStringList l;
    QList<QPair<QString, QString> > list, listp;
    QFRawDataRecordPrivate::ResultsIterator i(dstore->results);
    while (i.hasNext()) {
        i.next();
        QString en=i.key();
        QString egrp=i.value()->group;
        if ((evalName.isEmpty() || (en==evalName)) && (evalgroup.isEmpty() || (egrp==evalgroup))) {
            QFRawDataRecordPrivate::ResultsResultsIterator j(i.value()->results);
            while (j.hasNext()) {
                j.next();
                QString rn=j.key();
                QString g=j.value().group;
                QString lab=resultsGetLabel(en, rn);
                if ((group.isEmpty() || (group==g)) && (!l.contains(lab))) {
                    l.append(lab);
                    if (resultsGetSortPriority(en, rn)) {
                        if (g.isEmpty()) listp.append(qMakePair(lab, rn));
                        else listp.append(qMakePair(g+QString(": ")+lab, rn));
                    } else {
                        if (g.isEmpty()) list.append(qMakePair(lab, rn));
                        else list.append(qMakePair(g+QString(": ")+lab, rn));
                    }
                }
            }
        }
    }
    if (list.size()>0) {
        qSort(list.begin(), list.end(), QFRawDataRecord_StringPairCaseInsensitiveCompareSecond);
    }
    if (listp.size()>0) {
        qSort(listp.begin(), listp.end(), QFRawDataRecord_StringPairCaseInsensitiveCompareSecond);
    }

    listp.append(list);

    return listp;
}


QList<QPair<QString, QString> > QFRawDataRecord::resultsCalcNamesAndLabelsRichtext(const QString& evalName, const QString& group, const QString& evalgroup) const {
    QStringList l;
    /*
       This function first creates a list of the triple <label, richtext_label, result_id> that matches the given filters. Then this list
       ist filtered according to the label (it's difficult to sort a rich-texted string!!!). Finally the list is shrunken down to the
       pair <richtext_label, result_id>!
      */
    QList<QTriple<QString, QString, QString> > list ,listp;
    QList<QPair<QString, QString> > list1;
    QFRawDataRecordPrivate::ResultsIterator i(dstore->results);
    while (i.hasNext()) {
        i.next();
        QString en=i.key();
        QString egrp=i.value()->group;
        if ((evalName.isEmpty() || (en==evalName)) && (evalgroup.isEmpty() || (egrp==evalgroup))) {
            QFRawDataRecordPrivate::ResultsResultsIterator j(i.value()->results);
            while (j.hasNext()) {
                j.next();
                QString rn=j.key();
                QString lab=resultsGetLabel(en, rn);
                QString labrt=resultsGetLabelRichtext(en, rn);
                if ((group.isEmpty() || (group==resultsGetGroup(en, rn))) && (!l.contains(lab))) {
                    l.append(lab);
                    if (resultsGetSortPriority(en, rn)) listp.append(qMakeTriple(lab, labrt, rn));
                    else list.append(qMakeTriple(lab, labrt, rn));
                }
            }
        }
    }
    if (list.size()>0) {
        qSort(list.begin(), list.end(), QFRawDataRecord_StringTripleCaseInsensitiveCompareThird);
    }
    if (listp.size()>0) {
        qSort(listp.begin(), listp.end(), QFRawDataRecord_StringTripleCaseInsensitiveCompareThird);
    }

    listp.append(list);

    for (int i=0; i<listp.size(); i++) {
        list1.append(qMakePair(listp[i].second, listp[i].third));
    }
    return list1;
}


QList<QString> QFRawDataRecord::resultsCalcEvaluationsInGroup(const QString& evalGroup) const {
    QStringList l;


    QFRawDataRecordPrivate::ResultsIterator i(dstore->results);
    while (i.hasNext()) {
        i.next();
        QString en=i.key();
        if (i.value()->group==evalGroup) l.append(en);
    }


    return l;
}


QList<QString> QFRawDataRecord::resultsCalcEvalGroups(const QString& paramgroup) const {
    QStringList l;

    QFRawDataRecordPrivate::ResultsIterator i(dstore->results);
    while (i.hasNext()) {
        i.next();
        QString en=i.key();
        QString grp=i.value()->group;
        if (!l.contains(grp)) {
            if (paramgroup.isEmpty()) {
                l.append(grp);
            } else {
                QList<QString> gl=resultsCalcGroups(en);
                if (gl.contains(paramgroup)) l.append(grp);
            }
        }
    }

    return l;
}


void QFRawDataRecord::emitPropertiesChanged() {
    if (doEmitPropertiesChanged) {
        //qDebug()<<"QFRawDataRecord ("<<name<<") emits propertiesChanged()";
        emit propertiesChanged();

    }
}

void QFRawDataRecord::emitResultsChanged() {
    if (doEmitResultsChanged) {
        //qDebug()<<"QFRawDataRecord ("<<name<<") emits resultsChanged()";
        emit resultsChanged();
    }
}

void QFRawDataRecord::emitRawDataChanged() {
    //qDebug()<<"QFRawDataRecord ("<<name<<") emits rawDataChanged()";
    emit rawDataChanged();
}

void QFRawDataRecord::disableEmitResultsChanged() {
    doEmitResultsChanged=false;
}

void QFRawDataRecord::enableEmitResultsChanged(bool emitnow) {
    doEmitResultsChanged=true;
    if (emitnow) {
        //qDebug()<<"QFRawDataRecord ("<<name<<") emits resultsChanged()";
        emit resultsChanged();
    }
}

void QFRawDataRecord::disableEmitPropertiesChanged() {
    doEmitPropertiesChanged=false;
}

void QFRawDataRecord::enableEmitPropertiesChanged(bool emitnow) {
    doEmitPropertiesChanged=true;
    if (emitnow) {
        //qDebug()<<"QFRawDataRecord ("<<name<<") emits propertiesChanged()";
        emit  propertiesChanged();
    }
}
