/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#include "qffitfunctionconfigforglobalfittools.h"
#include <QtCore>
#include <QtXml>
#include "qftools.h"

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

    config.paramValues.clear();
    config.globalParams.clear();
    config.fixes.clear();
    config.models.clear();
    config.roles.clear();

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
    } else if (docElem.tagName()=="qf3_globalfitconfig_v2") {
        config.menuEntryLabel=docElem.attribute("menu");
        config.groupLabel=docElem.attribute("group");
        config.models.clear();
        config.globalParams.clear();
        QDomElement mse=docElem.firstChildElement("models");
        if (!mse.isNull()) {
            QDomElement me=mse.firstChildElement("model");
            while (!me.isNull()) {
                config.models.append(me.attribute("id"));
                config.roles.append(me.attribute("role"));
                QDomElement fe=me.firstChildElement("fix");
                QStringList f;
                while (!fe.isNull()) {
                    f<<fe.attribute("id");
                    fe=fe.nextSiblingElement("fix");
                }
                config.fixes<<f;
                me=me.nextSiblingElement("model");
            }
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

        QDomElement gppe=docElem.firstChildElement("paramPresets");
        if (!gppe.isNull()) {
            QDomElement gpme=gppe.firstChildElement("model");
            while (!gpme.isNull()) {
                QMap<QString, QFFitFunctionConfigForGlobalFitInterface::GlobalFitParameter> params;
                QDomElement pe=gpme.firstChildElement("parameter");
                while (!pe.isNull()) {
                    params[pe.attribute("id")].value=CQStringToDouble(pe.attribute("value", "0"));
                    params[pe.attribute("id")].error=CQStringToDouble(pe.attribute("error", "0"));
                    params[pe.attribute("id")].rangeMin=CQStringToDouble(pe.attribute("min", "0"));
                    params[pe.attribute("id")].rangeMax=CQStringToDouble(pe.attribute("max", "0"));
                    pe=pe.nextSiblingElement("parameter");
                }
                gpme=gpme.nextSiblingElement("model");
                config.paramValues<<params;
            }
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
    stream.writeStartElement("qf3_globalfitconfig_v2");
    stream.writeAttribute("menu", config.menuEntryLabel);
    stream.writeAttribute("group", config.groupLabel);

    stream.writeStartElement("models");
    for (int i=0; i<config.models.size(); i++) {
        stream.writeStartElement("model");
        stream.writeAttribute("id", config.models[i]);
        if (!config.roles.value(i, "").isEmpty()){
            stream.writeAttribute("role", config.roles.value(i, ""));
        }
        if (config.fixes.value(i).size()>0) {
            for (int j=0; j<config.fixes.value(i).size(); j++) {
                if (!config.fixes[i].value(j).isEmpty()) {
                    stream.writeStartElement("fix");
                    stream.writeAttribute("id", config.fixes[i].value(j));
                    stream.writeEndElement();
                }
            }
        }
        stream.writeEndElement();
    }
    stream.writeEndElement();
    for (int i=0; i<config.globalParams.size(); i++) {
        stream.writeStartElement("globalParam");
        for (int j=0; j<config.globalParams[i].size(); j++) {
            stream.writeStartElement("model");
            //stream.writeAttribute("id", config.models.value(i));
            for (int k=0; k<config.globalParams[i].at(j).size(); k++) {
                stream.writeTextElement("parameter", config.globalParams[i].at(j).at(k));
            }
            stream.writeEndElement();
        }
        stream.writeEndElement();
    }

    if (config.paramValues.size()>0) {
        stream.writeStartElement("paramPresets");
        for (int i=0; i<config.paramValues.size(); i++) {
            stream.writeStartElement("model");
            QMapIterator<QString, QFFitFunctionConfigForGlobalFitInterface::GlobalFitParameter> it(config.paramValues[i]);
            while (it.hasNext()) {
                it.next();
                stream.writeStartElement("parameter");
                stream.writeAttribute("id", it.key());
                stream.writeAttribute("value", CDoubleToQString( it.value().value));
                stream.writeAttribute("error", CDoubleToQString(it.value().error));
                stream.writeAttribute("min", CDoubleToQString(it.value().rangeMin));
                stream.writeAttribute("max", CDoubleToQString(it.value().rangeMax));
                stream.writeEndElement();
            }
            stream.writeEndElement();
        }
        stream.writeEndElement();
    }

    stream.writeEndElement();
    stream.writeEndDocument();

    output.close();
}
