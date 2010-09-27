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
    if (!te.isNull()) {
        te=te.firstChildElement("property");
        while (!te.isNull()) {
            QString n=te.attribute("name", "");
            QString t=te.attribute("type", "string").toLower();
            QVariant d=te.attribute("data", "");
            bool c=false;
            if (t=="bool") { c=d.convert(QVariant::Bool); }
            else if (t=="char") { c=d.convert(QVariant::Char); }
            else if (t=="date") { c=d.convert(QVariant::Date); }
            else if (t=="datetime") { c=d.convert(QVariant::DateTime); }
            else if (t=="double") { c=d.convert(QVariant::Double); }
            else if (t=="int") { c=d.convert(QVariant::Int); }
            else if (t=="longlong") { c=d.convert(QVariant::LongLong); }
            else if (t=="string") { c=d.convert(QVariant::String); }
            else if (t=="stringlist") { c=d.convert(QVariant::StringList); }
            else if (t=="uint") { c=d.convert(QVariant::UInt); }
            else if (t=="ulonglong") { c=d.convert(QVariant::ULongLong); }
            else if (t=="time") { c=d.convert(QVariant::Time); }
            else if (t=="bytearray") { c=d.convert(QVariant::ByteArray); }
            else if (t=="color") { c=d.convert(QVariant::Color); }
            else {
                setError(tr("Property '%1' has an unsupported type (%2)!\n Value is \"%3\".").arg(n).arg(t).arg(te.attribute("data", "")));
                return;
            }
            if (!c) {
                setError(tr("The value of property '%1' (%2) could not be converted to type %3!").arg(n).arg(te.attribute("data", "")).arg(t));
                return;
            }
            propertyItem pi;
            pi.data=d;
            pi.usereditable=QVariant(te.attribute("usereditable", "true")).toBool();
            props[n]=pi;
            te = te.nextSiblingElement("property");
        }
    }

    results.clear();
    te=e.firstChildElement("results");
    if (!te.isNull()) {
        te=te.firstChildElement("evaluation");
        QString en=te.attribute("name");
        while (!te.isNull()) {
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
    for (int i=0; i<props.keys().size(); i++) {
        w.writeStartElement("property");
        QString n=props.keys().at(i);
        w.writeAttribute("name", n);
        QString t="invalid";
        switch(props[n].data.type()) {
            case QVariant::Bool: t="bool"; break;
            case QVariant::Char: t="char"; break;
            case QVariant::Date: t="date"; break;
            case QVariant::DateTime: t="datetime"; break;
            case QVariant::Double: t="double"; break;
            case QVariant::Int: t="int"; break;
            case QVariant::LongLong: t="longlong"; break;
            case QVariant::String: t="string"; break;
            case QVariant::StringList: t="stringlist"; break;
            case QVariant::UInt: t="uint"; break;
            case QVariant::ULongLong: t="ulonglong"; break;
            case QVariant::Time: t="time"; break;
            case QVariant::Point: t="point"; break;
            case QVariant::PointF: t="pointf"; break;
            case QVariant::Size: t="size"; break;
            case QVariant::SizeF: t="sizef"; break;
            case QVariant::BitArray: t="bitarray"; break;
            case QVariant::ByteArray: t="bytearray"; break;
            case QVariant::Color: t="color"; break;
            case QVariant::Font: t="font"; break;
            case QVariant::Hash: t="hash"; break;
            case QVariant::List: t="list"; break;
            case QVariant::Map: t="map"; break;
            case QVariant::Rect: t="rect"; break;
            case QVariant::RectF: t="rectf"; break;

        }
        w.writeAttribute("type", t);
        w.writeAttribute("data", props[n].data.toString());
        w.writeAttribute("usereditable", (props[n].usereditable)?QString("true"):QString("false"));
        w.writeEndElement();
    }
    w.writeEndElement();
    w.writeStartElement("results");
    for (int i=0; i<results.keys().size(); i++) {
        w.writeStartElement("evaluation");
        QString n=props.keys().at(i);
        w.writeAttribute("name", n);
        for (int j=0; j<results[n].size(); j++) {
            w.writeStartElement("result");
            QString rn=results[n].keys().at(j);
            w.writeAttribute("name", rn);
            evaluationResult r=results[n].value(rn);
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

unsigned int QFRawDataRecord::getVisiblePropertyCount() {
    unsigned int c=0;
    for (int i=0; i<props.keys().size(); i++) {
        QString p=props.keys().at(i);
        if (props[p].visible) c++;
    }
    return c;
}

QString QFRawDataRecord::getVisibleProperty(unsigned int j) {
    unsigned int c=0;
    for (int i=0; i<props.keys().size(); i++) {
        QString p=props.keys().at(i);
        if (props[p].visible) {
            if (c==j) return p;
            c++;
        }
    }
    return QString("");
}


