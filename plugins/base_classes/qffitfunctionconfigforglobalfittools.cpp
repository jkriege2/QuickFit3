#include "qffitfunctionconfigforglobalfittools.h"
#include <QtCore>
#include <QtXml>


bool openGlobalFitConfig(const QString &filename, QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig &config)
{
    QDomDocument doc("mydocument");
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    if (!doc.setContent(&file)) {
        file.close();
        return false;
    }
    file.close();

    QDomElement docElem = doc.documentElement();

    if (docElem.tagName()=="qf3_globalfitconfig") {
        config.menuEntryLabel=docElem.attribute("menu");
        config.groupLabel=docElem.attribute("group");
        config.models.clear();
        config.globalParams.clear();
        QDomElement me=docElem.firstChildElement("model");
        while (!me.isNull()) {
            config.models.append(me.text());
            me=me.nextSiblingElement("model");
        }
        QDomElement gpe=docElem.firstChildElement("globalParam");
        while (!gpe.isNull()) {
            QList<QStringList> formodels;
            QDomElement gpme=gpe.firstChildElement("model");
            while (!gpme.isNull()) {
                QStringList params;
                QDomElement pe=gpme.firstChildElement("parameter");
                while (!pe.isNull()) {
                    params<<pe.text();
                    pe=pe.nextSiblingElement("parameter");
                }
                formodels<<params;
                gpme=gpme.nextSiblingElement("model");
            }
            config.globalParams<<formodels;
            gpe=gpe.nextSiblingElement("globalParam");
        }
    } else {
        return false;
    }
    return true;
}


void storeGlobalFitConfig(const QString &filename, const QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig &config)
{
    QFile output(filename);
    if (!output.open(QFile::WriteOnly|QFile::Text)) return;

    QXmlStreamWriter stream(&output);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("qf3_globalfitconfig");
    stream.writeAttribute("menu", config.menuEntryLabel);
    stream.writeAttribute("group", config.groupLabel);

    for (int i=0; i<config.models.size(); i++) {
        stream.writeTextElement("model", config.models[i]);
    }
    for (int i=0; i<config.globalParams.size(); i++) {
        stream.writeStartElement("globalParam");
        for (int j=0; j<config.globalParams[i].size(); j++) {
            stream.writeStartElement("model");
            for (int k=0; k<config.globalParams[i].at(j).size(); k++) {
                stream.writeTextElement("parameter", config.globalParams[i].at(j).at(k));
            }
            stream.writeEndElement();
        }
        stream.writeEndElement();
    }

    stream.writeEndElement();
    stream.writeEndDocument();

    output.close();
}
