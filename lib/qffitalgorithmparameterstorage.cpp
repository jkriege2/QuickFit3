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

#include "qffitalgorithmparameterstorage.h"


QFFitAlgorithmParameterStorage::QFFitAlgorithmParameterStorage(QFProject *project)
{
    algparam_project=project;
}

QFFitAlgorithmParameterStorage::~QFFitAlgorithmParameterStorage()
{

}


void QFFitAlgorithmParameterStorage::clearQFFitAlgorithmParameters()
{
    algorithm_parameterstore.clear();
}

void QFFitAlgorithmParameterStorage::configFitAlgorithm(QFFitAlgorithm *algorithm)
{
    if (algorithm) {
        restoreQFFitAlgorithmParameters(algorithm);
        if (algorithm->displayConfig()) {
            storeQFFitAlgorithmParameters(algorithm);
        }
    }
}

void QFFitAlgorithmParameterStorage::configFitAlgorithm(QFFitAlgorithm *algorithm, QFFitAlgorithmParameterStorage *data)
{
    if (algorithm && data) {
        data->restoreQFFitAlgorithmParameters(algorithm);
        if (algorithm->displayConfig()) {
            data->storeQFFitAlgorithmParameters(algorithm);
        }
    }
}


void QFFitAlgorithmParameterStorage::setQFFitAlgorithmParametersProject(QFProject *project)
{
    algparam_project=project;
}

void QFFitAlgorithmParameterStorage::writeQFFitAlgorithmParameters(QXmlStreamWriter &w, const QString &tagName) const
{
    QHashIterator<QString, QHash<QString, QVariant> > fapit(algorithm_parameterstore);
    while (fapit.hasNext()) {
        fapit.next();
        w.writeStartElement(tagName);
        w.writeAttribute("id", fapit.key());
        QHashIterator<QString, QVariant> mi(fapit.value());
        while (mi.hasNext()) {
            mi.next();
            w.writeStartElement("parameter");
            w.writeAttribute("id", mi.key());
            w.writeAttribute("type", getQVariantType(mi.value()));
            w.writeAttribute("data", getQVariantData(mi.value()));
            w.writeEndElement();
        }
        w.writeEndElement();
    }
}

void QFFitAlgorithmParameterStorage::readQFFitAlgorithmParameters(const QDomElement& eltin, const QString& tagName)
{
    if (eltin.isNull()) return;
    QDomElement elt=eltin;
    while ( !elt.isNull() ) {
        if (elt.hasAttribute("id")) {
            QString id=elt.attribute("id");
            //qDebug()<<"  fit algorithm "<<id.toStdString()<<std::endl;
            QDomElement eltt=elt.firstChildElement("parameter");
            for (; !eltt.isNull(); eltt = eltt.nextSiblingElement("parameter")) {
                if (eltt.hasAttribute("id") && eltt.hasAttribute("type") && eltt.hasAttribute("data")) {
                    QString pid=eltt.attribute("id");
                    QString ptype=eltt.attribute("type");
                    QString pdata=eltt.attribute("data");
                    //qDebug()<<"    param "<<pid.toStdString()<<" type="<<ptype.toStdString()<<" data="<<pdata.toStdString()<<std::endl;
                    algorithm_parameterstore[id].insert(pid, getQVariantFromString(ptype, pdata));
                }
            }

            //if (m_fitAlgorithms.contains(id)) restoreQFFitAlgorithmParameters(m_fitAlgorithms[id]);
            elt = elt.nextSiblingElement(tagName);
        }
    }
}

void QFFitAlgorithmParameterStorage::storeQFFitAlgorithmParameters(QFFitAlgorithm* algorithm) {
    QString aid=algorithm->id();
    QStringList params=algorithm->getParameterIDs();
    for (int i=0; i<params.size(); i++) {
        QString pid=params[i];
        algorithm_parameterstore[aid].insert(pid, algorithm->getParameter(pid));
    }
    //emitPropertiesChanged();
    if (algparam_project) algparam_project->setDataChanged();
}

void QFFitAlgorithmParameterStorage::restoreQFFitAlgorithmParameters(QFFitAlgorithm* algorithm) const {
    QString aid=algorithm->id();
    QStringList params=algorithm->getParameterIDs();
    for (int i=0; i<params.size(); i++) {
        QString pid=params[i];
        if (algorithm_parameterstore.contains(aid)) algorithm->setParameter(pid, algorithm_parameterstore[aid].value(pid, algorithm->getParameter(pid)));
    }
}
