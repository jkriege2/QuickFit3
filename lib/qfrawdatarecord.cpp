#include "qfrawdatarecord.h"

QFRawDataRecord::QFRawDataRecord(QFProject* parent):
    QObject(parent)
{
    project=parent;
    errorOcc=false;
    errorDesc="";
    name="";
    description="";
    resultsmodel=new QFRDRResultsModel();
    resultsmodel->init(this);
    propModel=new QFRDRPropertyModel();
    propModel->init(this);
}



void QFRawDataRecord::init(QString name, QStringList inputFiles) {
    this->ID=project->getNewID();
    this->name=name;
    description="";
    files=inputFiles;
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
    //std::cout<<"deleting QFRawDataRecord\n";
    //std::cout<<"deleting QFRawDataRecord ... OK\n";
}

void QFRawDataRecord::readXML(QDomElement& e) {
    bool ok=true;
    name=e.attribute("name", "rawdatarecord");
    ID=e.attribute("id", "-1").toInt(&ok);
    if (ID==-1) { setError(tr("invalid ID in <rawdatarecord name=\"%1\" ...>!").arg(name)); return; }
    if (!project->checkID(ID)) {
        setError(tr("ID %1 in <rawdatarecord name=\"%2\" ...> already in use in the project!").arg(ID).arg(name));
        return;
    }
    QDomElement te=e.firstChildElement("description");
    if (te.isNull()) { description=""; } else { description=te.text(); }
    //std::cout<<"    reading XML: properties\n";
    props.clear();
    te=e.firstChildElement("properties");
    readProperties(te);

    results.clear();
    te=e.firstChildElement("results");
    if (!te.isNull()) {
        te=te.firstChildElement("evaluation");
        while (!te.isNull()) {
            QString en=te.attribute("name");
            QDomElement re=te.firstChildElement("result");
            while (!re.isNull()) {
                QString n=re.attribute("name", "");
                QString t=re.attribute("type", "invalid");
                evaluationResult r;
                //r.name=n;
                r.type=qfrdreInvalid;
                if (t=="boolean") {
                    r.type=qfrdreBoolean;
                    r.bvalue=QVariant(re.attribute("value", "false")).toBool();
                } else if (t=="integer") {
                    r.type=qfrdreInteger;
                    r.ivalue=re.attribute("value", "0").toInt();
                    r.unit=re.attribute("unit", "");
                } else if (t=="string") {
                    r.type=qfrdreString;
                    r.svalue=re.attribute("value", "");
                } else if (t=="number") {
                    r.type=qfrdreNumber;
                    r.dvalue=re.attribute("value", "0.0").toDouble();
                    r.unit=re.attribute("unit", "");
                } else if (t=="numberlist") {
                    r.type=qfrdreNumberList;
                    r.dvec.clear();
                    QStringList s=re.attribute("value", "0.0").split(";");
                    for (int i=0; i<s.size(); i++) {
                        bool ok=false;
                        double d=s[i].toDouble(&ok);
                        if (ok) { r.dvec.append(d); }
                        else { r.dvec.append(0); }
                    }
                    r.unit=re.attribute("unit", "");
                } else if (t=="numbererror") {
                    r.type=qfrdreNumberError;
                    r.dvalue=re.attribute("value", "0.0").toDouble();
                    r.derror=re.attribute("error", "0.0").toDouble();
                    r.unit=re.attribute("unit", "");
                }
                if (!n.isEmpty() && !en.isEmpty()) results[en].insert(n, r);
                re=re.nextSiblingElement("result");
            }
            te = te.nextSiblingElement("evaluation");
        }
    }

    //std::cout<<"    reading XML: files\n";
    te=e.firstChildElement("files");
    files.clear();
    if (!te.isNull()) {
        QDomElement fe=te.firstChildElement("file");
        while (!fe.isNull()) {
            files.push_back(fe.text());
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
}


void QFRawDataRecord::writeXML(QXmlStreamWriter& w) {
    w.writeStartElement("rawdataelement");
    w.writeAttribute("type", getType());
    w.writeAttribute("name", name);
    w.writeAttribute("id", QString::number(ID));
    w.writeStartElement("description");
    w.writeCDATA(description);
    w.writeEndElement();
    w.writeStartElement("properties");
    storeProperties(w);
    w.writeEndElement();
    w.writeStartElement("results");
    QMapIterator<QString, QMap<QString, evaluationResult> > i(results);
    while (i.hasNext()) {
    //for (int i=0; i<results.keys().size(); i++) {
        i.next();
        w.writeStartElement("evaluation");
        QString n=i.key();
        w.writeAttribute("name", n);
        QMapIterator<QString, evaluationResult> j(i.value());
        //for (int j=0; j<i.value().size(); j++) {
        while (j.hasNext()) {
            j.next();
            w.writeStartElement("result");
            QString rn=j.key();
            w.writeAttribute("name", rn);
            evaluationResult r=j.value();
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
                    w.writeAttribute("value", QString::number(r.ivalue));
                    w.writeAttribute("unit", r.unit);
                    break;
                case qfrdreString:
                    w.writeAttribute("type", "string");
                    w.writeAttribute("value", r.svalue);
                    break;
                case qfrdreNumber:
                    w.writeAttribute("type", "number");
                    w.writeAttribute("value", QString::number(r.dvalue));
                    w.writeAttribute("unit", r.unit);
                    break;
                case qfrdreNumberList: {
                    w.writeAttribute("type", "numberlist");
                    QString s="";
                    for (int i=0; i<r.dvec.size(); i++) {
                        if (i>0) s=s+";";
                        s=s+QString::number(r.dvec[i]);
                    }
                    w.writeAttribute("value", s);
                    w.writeAttribute("unit", r.unit);
                    } break;
                case qfrdreNumberError:
                    w.writeAttribute("type", "numbererror");
                    w.writeAttribute("value", QString::number(r.dvalue));
                    w.writeAttribute("error", QString::number(r.derror));
                    w.writeAttribute("unit", r.unit);
                    break;

            }
            w.writeEndElement();
        }
        w.writeEndElement();
    }
    w.writeEndElement();
    if (files.size()>0) {
        w.writeStartElement("files");
        for (int i=0; i< files.size(); i++) {
            QString file=files[i];
            QFileInfo fi(project->getFile());
            file=fi.absoluteDir().relativeFilePath(files[i]);
            w.writeTextElement("file", file);
        }
        w.writeEndElement();
    }
    w.writeStartElement("data");
    intWriteData(w);
    w.writeEndElement();
    w.writeEndElement();
}





void QFRawDataRecord::resultsSetNumber(QString evaluationName, QString resultName, double value, QString unit) {
    evaluationResult r;
    r.type=qfrdreNumber;
    r.dvalue=value;
    r.unit=unit;
    results[evaluationName].insert(resultName, r);
    emit resultsChanged();
};


void QFRawDataRecord::resultsSetNumberList(QString evaluationName, QString resultName, QVector<double>& value, QString unit) {
    evaluationResult r;
    r.type=qfrdreNumberList;
    r.dvec=value;
    r.unit=unit;
    results[evaluationName].insert(resultName, r);
    emit resultsChanged();
};


void QFRawDataRecord::resultsSetNumberError(QString evaluationName, QString resultName, double value, double error, QString unit)  {
    evaluationResult r;
    r.type=qfrdreNumberError;
    r.dvalue=value;
    r.derror=error;
    r.unit=unit;
    results[evaluationName].insert(resultName, r);
    emit resultsChanged();
};

void QFRawDataRecord::resultsSetInteger(QString evaluationName, QString resultName, int64_t value, QString unit) {
    evaluationResult r;
    r.type=qfrdreInteger;
    r.ivalue=value;
    r.unit=unit;
    results[evaluationName].insert(resultName, r);
    emit resultsChanged();
};

void QFRawDataRecord::resultsSetString(QString evaluationName, QString resultName, QString value) {
    evaluationResult r;
    r.type=qfrdreString;
    r.svalue=value;
    results[evaluationName].insert(resultName, r);
    emit resultsChanged();
};

void QFRawDataRecord::resultsSetBoolean(QString evaluationName, QString resultName, bool value) {
    evaluationResult r;
    r.type=qfrdreBoolean;
    r.bvalue=value;
    results[evaluationName].insert(resultName, r);
    emit resultsChanged();
};

QString QFRawDataRecord::resultsGetAsString(QString evalName, QString resultName) {
    evaluationResult r=resultsGet(evalName, resultName);
    switch(r.type) {
        case qfrdreBoolean: if (r.bvalue) return tr("true"); else return tr("false");
        case qfrdreInteger: return tr("%1 %2").arg(r.ivalue).arg(r.unit);
        case qfrdreNumber: return tr("%1 %2").arg(r.dvalue).arg(r.unit);
        case qfrdreNumberList: {
            QString s="(";
            for (int i=0; i<r.dvec.size(); i++) {
                if (i>0) s=s+";";
                s=s+QString::number(r.dvec[i]);
            }
            return s+") "+r.unit;
        }
        case qfrdreNumberError: return tr("(%1 +/- %2) %3").arg(r.dvalue).arg(r.derror).arg(r.unit);
        case qfrdreString: return r.svalue;
    }
    return QString("");
}

double QFRawDataRecord::resultsGetAsDouble(QString evalName, QString resultName) {
    evaluationResult r=resultsGet(evalName, resultName);
    switch(r.type) {
        case qfrdreBoolean: if (r.bvalue) return 1; else return 0;
        case qfrdreInteger: return r.ivalue;
        case qfrdreNumber: case qfrdreNumberError: return r.dvalue;
        case qfrdreString: return r.svalue.toDouble();
    }
    return 0.0;
}

int64_t QFRawDataRecord::resultsGetAsInteger(QString evalName, QString resultName) {
    evaluationResult r=resultsGet(evalName, resultName);
    switch(r.type) {
        case qfrdreBoolean: if (r.bvalue) return 1; else return 0;
        case qfrdreInteger: return r.ivalue;
        case qfrdreNumber: case qfrdreNumberError: return r.dvalue;
        case qfrdreString: return r.svalue.toInt();
    }
    return 0.0;
}

QVector<double> QFRawDataRecord::resultsGetAsDoubleList(QString evalName, QString resultName) {
    evaluationResult r=resultsGet(evalName, resultName);
    switch(r.type) {
        case qfrdreNumberList: return r.dvec;
    }
    return QVector<double>();
}

double QFRawDataRecord::resultsGetErrorAsDouble(QString evalName, QString resultName) {
    evaluationResult r=resultsGet(evalName, resultName);
    switch(r.type) {
        case qfrdreNumber: case qfrdreNumberError: return r.derror;
    }
    return 0.0;
};

QString QFRawDataRecord::resultsGetResultName(QString evaluationName, unsigned int i) {
    if (results.contains(evaluationName)) {
        if (i<results[evaluationName].size()) {
            return results[evaluationName].keys().at(i);
        }
    }
    return QString("");
}

