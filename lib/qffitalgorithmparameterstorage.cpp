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

void QFFitAlgorithmParameterStorage::readQFFitAlgorithmParameters(QDomElement elt, const QString& tagName)
{
    for (; !elt.isNull(); elt = elt.nextSiblingElement(tagName)) {
        if (elt.hasAttribute("id")) {
            QString id=elt.attribute("id");
            //std::cout<<"  fit algorithm "<<id.toStdString()<<std::endl;
            QDomElement eltt=elt.firstChildElement("parameter");
            for (; !eltt.isNull(); eltt = eltt.nextSiblingElement("parameter")) {
                if (eltt.hasAttribute("id") && eltt.hasAttribute("type") && eltt.hasAttribute("data")) {
                    QString pid=eltt.attribute("id");
                    QString ptype=eltt.attribute("type");
                    QString pdata=eltt.attribute("data");
                    //std::cout<<"    param "<<pid.toStdString()<<" type="<<ptype.toStdString()<<" data="<<pdata.toStdString()<<std::endl;
                    algorithm_parameterstore[id].insert(pid, getQVariantFromString(ptype, pdata));
                }
            }

            //if (m_fitAlgorithms.contains(id)) restoreQFFitAlgorithmParameters(m_fitAlgorithms[id]);
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
        algorithm->setParameter(pid, algorithm_parameterstore[aid].value(pid, algorithm->getParameter(pid)));
    }
}
