#include "qffitresultsevaluation.h"

QFFitResultsEvaluation::QFFitResultsEvaluation(QFProject* parent) :
    QFEvaluationItem(parent, true, false)
{
    QFileInfo inif(parent->getServices()->getOptions()->getIniFilename());
    QString inifn=inif.absolutePath()+QString("/fitparams_%1.ini").arg(getType());
    fitParamSettings=new QSettings(inifn, QSettings::IniFormat);

    m_fitAlgorithm="";
    m_fitFunction="";


    // get list of applicable fit functions
    m_fitFunctions=parent->getServices()->getFitFunctionManager()->getModels("fcs_", this);
    if (m_fitFunctions.size()>0) m_fitFunction=m_fitFunctions.keys().at(0);

    // get list of available fit algorithms and store their parameters in the internal algorithm_parameterstore
    algorithm_parameterstore.clear();
    QStringList fita=parent->getServices()->getFitAlgorithmManager()->getIDList();
    for (int i=0; i<fita.size(); i++) {
        QFFitAlgorithm* falg=parent->getServices()->getFitAlgorithmManager()->createAlgorithm(fita[i], this);
        m_fitAlgorithms[fita[i]]=falg;
        storeQFFitAlgorithmParameters(falg);
    }
    if (m_fitAlgorithms.size()>0) m_fitAlgorithm=m_fitAlgorithms.keys().at(0);

    parameterStore.clear();

}

QFFitResultsEvaluation::~QFFitResultsEvaluation() {
    // the fit algorithm and fit function instances are private to this class, so we have to delete the objects!
    QMapIterator<QString, QFFitFunction*> i(m_fitFunctions);
    while (i.hasNext()) {
        i.next();
        delete i.value();
    }
    m_fitFunctions.clear();
    QMapIterator<QString, QFFitAlgorithm*> j(m_fitAlgorithms);
    while (j.hasNext()) {
        j.next();
        delete j.value();
    }
    m_fitAlgorithms.clear();
}



QStringList QFFitResultsEvaluation::getAvailableFitAlgorithms() const {
    return m_fitAlgorithms.keys();
}

QFFitAlgorithm* QFFitResultsEvaluation::getFitAlgorithm(const QString& id) const {
    if (m_fitAlgorithms.contains(id)) return m_fitAlgorithms[id];
    return NULL;
}

QStringList QFFitResultsEvaluation::getAvailableFitFunctions() const {
    return m_fitFunctions.keys();
}

QFFitFunction* QFFitResultsEvaluation::getFitFunction(const QString& id) const {
    if (m_fitFunctions.contains(id)) return m_fitFunctions[id];
    else return NULL;
}

QFFitAlgorithm* QFFitResultsEvaluation::getFitAlgorithm() const {
    return getFitAlgorithm(m_fitAlgorithm);
}

void QFFitResultsEvaluation::setFitAlgorithm(QString fitAlgorithm) {
    m_fitAlgorithm=fitAlgorithm;
}


QFFitFunction* QFFitResultsEvaluation::getFitFunction() {
    return getFitFunction(m_fitFunction);
}

void QFFitResultsEvaluation::setFitFunction(QString fitFunction) {
    if (!getAvailableFitFunctions().contains(fitFunction)) return;
    if (m_fitFunction!=fitFunction) {
        QString oldff=m_fitFunction;
        QFFitFunction* ffold=getFitFunction(m_fitFunction);
        QMap<QString, FitParameter> params;
        if (ffold) {
            for (int i=0; i<ffold->paramCount(); i++) {
                QString pid=ffold->getParameterID(i);
                params[pid].value=getFitValue(pid);
                params[pid].min=getFitMin(pid);
                params[pid].max=getFitMax(pid);
                params[pid].fix=getFitFix(pid);
            }
        }
        m_fitFunction=fitFunction;
        QFFitFunction* ff=getFitFunction(fitFunction);
        for (int i=0; i<ff->paramCount(); i++) {
            QString pid=ff->getParameterID(i);
            QString newid=getParameterStoreID(fitFunction, ff->getParameterID(i));
            if (params.contains(pid)) {
                if (params[pid].value!=getFitValue(pid)) setFitValue(pid, params[pid].value);
                if (params[pid].fix!=getFitFix(pid)) setFitFix(pid, params[pid].fix);
                if (params[pid].min!=getFitMin(pid)) setFitMin(pid, params[pid].min);
                if (params[pid].max!=getFitMax(pid)) setFitMax(pid, params[pid].max);
            }
        }
        emit propertiesChanged();
    }
}

void QFFitResultsEvaluation::storeQFFitAlgorithmParameters(QFFitAlgorithm* algorithm) {
    QString aid=algorithm->id();
    QStringList params=algorithm->getParameterIDs();
    for (int i=0; i<params.size(); i++) {
        QString pid=params[i];
        algorithm_parameterstore[aid].insert(pid, algorithm->getParameter(pid));
    }
    emit propertiesChanged();
}

void QFFitResultsEvaluation::restoreQFFitAlgorithmParameters(QFFitAlgorithm* algorithm) {
    QString aid=algorithm->id();
    QStringList params=algorithm->getParameterIDs();
    for (int i=0; i<params.size(); i++) {
        QString pid=params[i];
        algorithm->setParameter(pid, algorithm_parameterstore[aid].value(pid, algorithm->getParameter(pid)));
    }
}


void QFFitResultsEvaluation::intWriteData(QXmlStreamWriter& w) {
    w.writeStartElement("algorithms");
    w.writeAttribute("current", m_fitAlgorithm);
    /*if (m_weighting==EqualWeighting) w.writeAttribute("weighting", "equal");
    if (m_weighting==StdDevWeighting) w.writeAttribute("weighting", "stddev");
    if (m_weighting==RunErrorWeighting) w.writeAttribute("weighting", "runerror");*/

    QMapIterator<QString, QMap<QString, QVariant> > fapit(algorithm_parameterstore);
    while (fapit.hasNext()) {
        fapit.next();
        w.writeStartElement("algorithm");
        w.writeAttribute("id", fapit.key());
        QMapIterator<QString, QVariant> mi(fapit.value());
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

    w.writeEndElement();
    w.writeStartElement("functions");
    w.writeAttribute("current", m_fitFunction);
    QMapIterator<QString, FitParameter> i(parameterStore);
    while (i.hasNext()) {
        i.next();
        w.writeStartElement("parameter");
        w.writeAttribute("id", i.key());
        if (i.value().valueSet) w.writeAttribute("value", QString::number(i.value().value, 'g', 12));
        if (i.value().errorSet) w.writeAttribute("error", QString::number(i.value().error, 'g', 12));
        if (i.value().minSet) w.writeAttribute("min", QString::number(i.value().min, 'g', 12));
        if (i.value().maxSet) w.writeAttribute("max", QString::number(i.value().max, 'g', 12));
        if (i.value().fixSet) w.writeAttribute("fix", QString((i.value().fix)?QString("1"):QString("0")));
        intWriteDataFitParam(i.key(), w);
        w.writeEndElement();
    }
    w.writeEndElement();
}

void QFFitResultsEvaluation::intReadData(QDomElement* e) {
    /*
        <algorithms current="..." weighting="equal|stddev">
          <algorithm id="name1">
            <parameter id="pname1" type="ptype" data="pdata" />
            <parameter id="pname2" type="ptype" data="pdata" />
          </algorithm
          <algorithm id="name2">
            <parameter id="pname1" type="ptype" data="pdata" />
            <parameter id="pname2" type="ptype" data="pdata" />
          </algorithm
        </algorithms>
    */

    QDomElement e1=e->firstChildElement("algorithms");
    if (e1.hasAttribute("current")) {
        QString a=e1.attribute("current");
        if (getAvailableFitAlgorithms().contains(a)) {
            setFitAlgorithm(a);
        } else {
            // STOP if unsupported fit algorithm was found
            setError(tr("found unsupported fitting algorithm with ID '%1', maybe you are missing a plugin").arg(a));
        }
    }
    /*if (e1.hasAttribute("weighting")) {
        QString a=e1.attribute("weighting").toLower();
        m_weighting=EqualWeighting;
        if (a=="stddev") m_weighting=StdDevWeighting;
        if (a=="runerror") m_weighting=RunErrorWeighting;
    }*/
    QDomElement elt=e1.firstChildElement("algorithm");
    algorithm_parameterstore.clear();
    for (; !elt.isNull(); elt = elt.nextSiblingElement("algorithm")) {
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

            if (m_fitAlgorithms.contains(id)) restoreQFFitAlgorithmParameters(m_fitAlgorithms[id]);
        }
    }




    e1=e->firstChildElement("functions");
    if (e1.hasAttribute("current")) {
        QString a=e1.attribute("current");
        if (getAvailableFitFunctions().contains(a)) {
            setFitFunction(a);
        } else {
            // STOP if unsupported fit algorithm was found
            setError(tr("found unsupported fitting function with ID '%1', maybe you are missing a plugin").arg(a));
        }
    }

    // read <parameter> tag
    elt=e1.firstChildElement("parameter");
    parameterStore.clear();
    for (; !elt.isNull(); elt = elt.nextSiblingElement("parameter")) {
        if (elt.hasAttribute("id") && ( elt.hasAttribute("value") || elt.hasAttribute("fix") || elt.hasAttribute("min") || elt.hasAttribute("max") ) ) {
            QString a=elt.attribute("id");
            FitParameter p;
            if (elt.hasAttribute("fix")) {
                p.fix=(elt.attribute("fix").toLower()!="0");
                p.fixSet=true;
            }
            if (elt.hasAttribute("value")) {
                p.value=elt.attribute("value").toDouble();
                p.valueSet=true;
            }
            if (elt.hasAttribute("error")) {
                p.error=elt.attribute("error").toDouble();
                p.errorSet=true;
            }
            if (elt.hasAttribute("min")) {
                p.min=elt.attribute("min").toDouble();
                p.minSet=true;
            }
            if (elt.hasAttribute("max")) {
                p.max=elt.attribute("max").toDouble();
                p.maxSet=true;
            }
            parameterStore[a]=p;
            intReadDataFitParam(a, p, elt);
        }
    }
}

QString QFFitResultsEvaluation::getParameterStoreID(QString parameter) {
    QString ff="";
    if (getFitFunction()!=NULL) {
        ff=getFitFunction()->id();
    }
    return getParameterStoreID(ff, parameter);
};


QString QFFitResultsEvaluation::getParameterStoreID(QString fitfunction, QString parameter) {
    return QString(fitfunction+"___"+parameter).trimmed().toLower();
};
