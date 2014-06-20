#include "qfrawdatarecord.h"
#include "qftools.h"
#include <QtXml>
#include "qtriple.h"
#include "binarydatatools.h"
#include "qfmathparser.h"
#include "datatools.h"
//#define DEBUG_THREAN

class QFRawDataRecordPrivate {
    public:
        QFRawDataRecordPrivate() {

        }

        typedef QMap<QString, QFRawDataRecord::evaluationResult> ResultsResultsType;
        typedef QMapIterator<QString, QFRawDataRecord::evaluationResult> ResultsResultsIterator;


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

        ~QFRawDataRecordPrivate() {
            ResultsIterator it(results);
            while (it.hasNext()) {
                it.next();
                delete it.value();
            }
            results.clear();
            evalGroupLabels.clear();
        }

};

QFRawDataRecordPrivate::evaluationIDMetadata::evaluationIDMetadata(int initsize) {
    //results.reserve(initsize);
}

QFRawDataRecord::QFRawDataRecord(QFProject* parent):
    QObject(parent), QFProperties()
{
    lock=new QReadWriteLock(QReadWriteLock::Recursive);
    project=parent;
    group=-1;
    errorOcc=false;
    errorDesc="";
    name="";
    folder="";
    description="";
    role="";
    resultsmodel=NULL;
    propModel=NULL;
    doEmitResultsChanged=true;
    doEmitPropertiesChanged=true;
    evaluationIDMetadataInitSize=100;
    dstore=new QFRawDataRecordPrivate();
    setResultsInitSize(100);
    resultsmodel=new QFRDRResultsModel();
    resultsmodel->init(this);

}


QFRawDataRecord::~QFRawDataRecord() {
    delete dstore;
    delete lock;
    //std::cout<<"deleting QFRawDataRecord\n";
    //std::cout<<"deleting QFRawDataRecord ... OK\n";
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

void QFRawDataRecord::log_text(const QString &message) const
{
    if (project) project->log_text(message);
}

void QFRawDataRecord::log_warning(const QString &message) const
{
    if (project) project->log_warning(message);
}

void QFRawDataRecord::log_error(const QString &message) const
{
    if (project) project->log_error(message);
}

QFRDRResultsModel* QFRawDataRecord::resultsGetModel() const {
    return resultsmodel;
};

void QFRawDataRecord::init(const QString& name, QStringList inputFiles, QStringList inputFilesTypes, QStringList inputFileDescriptions) {
    this->ID=project->getNewID();
    this->name=name;
    description="";
    files=inputFiles;
    files_types=inputFilesTypes;
    files_desciptions=inputFileDescriptions;
    group=-1;
    intReadData();
    //std::cout<<"after intReadData() in init ...\n";
    project->registerRawDataRecord(this);
    //std::cout<<"init ended ...\n";
}


void QFRawDataRecord::init(QDomElement& e, bool loadAsDummy) {
    //std::cout<<"creating QFRawDataRecord\n";
    name="";;
    description="";
    group=-1;
    //std::cout<<"  reading XML\n";
    readXML(e);
    //std::cout<<"  registering record\n";
    project->registerRawDataRecord(this);
    //std::cout<<"created QFRawDataRecord\n";
}

bool QFRawDataRecord::hasGroup() const
{
    return group>=0;
}

QString QFRawDataRecord::getGroupName() const
{
    return project->getRDRGroupName(group);
}

QList<QFRawDataRecord *> QFRawDataRecord::getGroupMembers() const
{
    return project->getRDRGroupMembers(group);
}

QList<QFRawDataRecord *> QFRawDataRecord::getRecordsWithRoleFromGroup(const QString &role) const
{
    QList<QFRawDataRecord *> res=getGroupMembers(), out;

    for (int i=0; i<res.size(); i++) {
        if (res[i] && res[i]->getRole().toLower().trimmed()==role.toLower().trimmed()) {
            out<<res[i];
        }
    }
    return out;
}

QString QFRawDataRecord::getFileForType(const QString &type) {
    QStringList sl=getFilesForType(type);
    if (sl.size()>0) return sl[0];
    return "";
}

QStringList QFRawDataRecord::getFilesForType(const QString &type) {
    QStringList result;
    for (int i=0; i<qMin(files_types.size(), files.size()); i++) {
        if (files_types[i].toLower()==type) result.append(files[i]);
    }
    return result;
}

QString QFRawDataRecord::getFileName(int i) const
{
    return files.value(i, "");
}

QString QFRawDataRecord::getFileType(int i) const
{
    return files_types.value(i, "");
}

QString QFRawDataRecord::getFileDescription(int i) const
{
    return files_desciptions.value(i, "");
}

int QFRawDataRecord::getFilesCount() const
{
    return files.size();
}

void QFRawDataRecord::setFileName(int i, const QString file)
{
    if (i>=0 && i<files.size()) {
        #ifdef DEBUG_THREAN
        qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
        #endif
         QWriteLocker locker(lock);
        #ifdef DEBUG_THREAN
         qDebug()<<Q_FUNC_INFO<<"  locked";
        #endif
        files[i]=file;
    }
    emit basicPropertiesChanged();
}

void QFRawDataRecord::setFileType(int i, const QString type)
{
    if (i>=0 && i<files.size()) {
        #ifdef DEBUG_THREAN
        qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
        #endif
         QWriteLocker locker(lock);
        #ifdef DEBUG_THREAN
         qDebug()<<Q_FUNC_INFO<<"  locked";
        #endif
        while (i>=files_types.size()) files_types.append("");
        files_types[i]=type;
    }
    emit basicPropertiesChanged();
}

void QFRawDataRecord::setFileDecsription(int i, const QString description)
{
    if (i>=0 && i<files.size()) {
        #ifdef DEBUG_THREAN
        qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
        #endif
         QWriteLocker locker(lock);
        #ifdef DEBUG_THREAN
         qDebug()<<Q_FUNC_INFO<<"  locked";
        #endif
        while (i>=files_desciptions.size()) files_desciptions.append("");
        files_desciptions[i]=description;
    }
    emit basicPropertiesChanged();

}

void QFRawDataRecord::addFile(const QString file, const QString type, const QString description)
{
    {
        #ifdef DEBUG_THREAN
        qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
        #endif
         QWriteLocker locker(lock);
        #ifdef DEBUG_THREAN
         qDebug()<<Q_FUNC_INFO<<"  locked";
        #endif
        files.append(file);
        setFileType(files.size()-1, type);
        setFileDecsription(files.size()-1, description);
    }
    emit basicPropertiesChanged();
}

void QFRawDataRecord::deleteFile(int i)
{
    if (i>=0 && i<files.size()) {
        #ifdef DEBUG_THREAN
        qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
        #endif
         QWriteLocker locker(lock);
        #ifdef DEBUG_THREAN
         qDebug()<<Q_FUNC_INFO<<"  locked";
        #endif
        files.removeAt(i);
        if (i<files_types.size()) files_types.removeAt(i);
        if (i<files_desciptions.size()) files_desciptions.removeAt(i);
    }
    emit basicPropertiesChanged();
}

void QFRawDataRecord::moveFileUp(int i) {
    QList<int> list;
    list<<i;
    moveFilesUp(list);
}

void QFRawDataRecord::moveFileDown(int i) {
    QList<int> list;
    list<<i;
    moveFilesDown(list);
}

void QFRawDataRecord::moveFilesUp(const QList<int> &list) {
    QList<int> l=list;
    qSort(l);
    if (l.size()>0) {

        #ifdef DEBUG_THREAN
        qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
        #endif
         QWriteLocker locker(lock);
        #ifdef DEBUG_THREAN
         qDebug()<<Q_FUNC_INFO<<"  locked";
        #endif

        int firstPos=l[0];
        QStringList f, t, d;
        for (int i=l.size()-1; i>=0; i--) {
            f.prepend(files.value(l[i], ""));
            t.prepend(files_types.value(l[i], ""));
            d.prepend(files_desciptions.value(l[i], ""));
            if (l[i]<files.size()) files.removeAt(l[i]);
            if (l[i]<files_types.size()) files_types.removeAt(l[i]);
            if (l[i]<files_desciptions.size()) files_desciptions.removeAt(l[i]);
        }
        int newPos=qMax(0, firstPos-1);
        for (int i=l.size()-1; i>=0; i--) {
            files.insert(newPos, f[i]);
            files_types.insert(newPos, t[i]);
            files_desciptions.insert(newPos, d[i]);
        }
    }
    emit basicPropertiesChanged();
}

void QFRawDataRecord::moveFilesDown(const QList<int> &list) {
    QList<int> l=list;
    qSort(l);
    if (l.size()>0) {

        #ifdef DEBUG_THREAN
        qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
        #endif
         QWriteLocker locker(lock);
        #ifdef DEBUG_THREAN
         qDebug()<<Q_FUNC_INFO<<"  locked";
        #endif

        int firstPos=l[0];
        QStringList f, t, d;
        for (int i=l.size()-1; i>=0; i--) {
            f.prepend(files.value(l[i], ""));
            t.prepend(files_types.value(l[i], ""));
            d.prepend(files_desciptions.value(l[i], ""));
            if (l[i]<files.size()) files.removeAt(l[i]);
            if (l[i]<files_types.size()) files_types.removeAt(l[i]);
            if (l[i]<files_desciptions.size()) files_desciptions.removeAt(l[i]);
        }
        int newPos=qMin(files.size(), firstPos+1);
        //qDebug()<<firstPos<<newPos<<files.size();
        if (newPos>=files.size()) {
            for (int i=0; i<l.size(); i++) {
                files.append(f[i]);
                files_types.append(t[i]);
                files_desciptions.append(d[i]);
            }
        } else {
            for (int i=l.size()-1; i>=0; i--) {
                files.insert(newPos, f[i]);
                files_types.insert(newPos, t[i]);
                files_desciptions.insert(newPos, d[i]);
            }
        }
    }
    emit basicPropertiesChanged();
}


void QFRawDataRecord::setName(const QString &n) {
    bool em=false;
    {
        
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        if (n!=name) {
            name=n;
            em=true;
        }
    }
    if (em)  emit basicPropertiesChanged();

}
/** \brief set the folder */
void QFRawDataRecord::setFolder(const QString& n) {
    bool em=false;
    {
        
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        if (folder!=n) {
            folder=n;
            em=true;
        }
    }
    if (em) emit basicPropertiesChanged();
}

void QFRawDataRecord::setRole(const QString &n)
{
    bool em=false;
    {

#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        if (role!=n) {
            role=n;
            em=true;
        }
    }
    if (em) emit basicPropertiesChanged();
}

void QFRawDataRecord::setGroup(int g)
{
    bool em=false;
    {

#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        if (group!=g) {
            group=g;
            em=true;
        }
    }
    if (em) emit basicPropertiesChanged();
}

void QFRawDataRecord::setDescription(const QString& d) {
    bool em=false;
    {
        
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        if (description!=d) {
            description=d;
            em=true;
        }
    }
    if (em) emit basicPropertiesChanged();
};

void QFRawDataRecord::readXML(QDomElement& e, bool loadAsDummy) {
    {
        
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        bool ok=true;
        name=e.attribute("name", "rawdatarecord");
        role=e.attribute("role", "");
        ID=e.attribute("id", "-1").toInt(&ok);
        if (ID==-1) { setError(tr("invalid ID in <rawdatarecord name=\"%1\" ...>!").arg(name)); return; }
        bool gOK=false;
        group=e.attribute("group", "-1").toInt(&gOK);
        if (!gOK) group=-1;
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
        #ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"unlock";
#endif
 locker.unlock();
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  unlocked";
#endif

        resultsClearAll();
        #ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"relock";
#endif
 locker.relock();
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  relocked";
#endif
        te=e.firstChildElement("results");
        if (!loadAsDummy && !te.isNull()) {
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
                        r.evec.clear();
                        QString storage=re.attribute("storage", "ascii").toLower().trimmed();
                        QString data="0.0";
                        if (re.hasAttribute("value")) data=re.attribute("value", "0.0");
                        else if (!re.text().isEmpty()) data=re.text();
                        if (storage=="base64") {
                            r.dvec=stringToDoubleArray_base64(data);
                        } else if (storage=="hex") {
                            r.dvec=stringToDoubleArray_hex(data);
                        } else {
                            QStringList s=data.split(";");
                            for (int i=0; i<s.size(); i++) {
                                bool ok=false;
                                double d=loc.toDouble(s[i], &ok);
                                if (ok) { r.dvec.append(d); }
                                else { r.dvec.append(0); }
                            }
                        }
                        r.unit=re.attribute("unit", "");
                    } else if (t=="numbermatrix" || t=="nummtrx") {
                        r.type=qfrdreNumberMatrix;
                        r.dvec.clear();
                        r.evec.clear();
                        QString storage=re.attribute("storage", "ascii").toLower().trimmed();
                        QString data="0.0";
                        if (re.hasAttribute("value")) data=re.attribute("value", "0.0");
                        else if (!re.text().isEmpty()) data=re.text();
                        if (storage=="base64") {
                            r.dvec=stringToDoubleArray_base64(data);
                        } else if (storage=="hex") {
                            r.dvec=stringToDoubleArray_hex(data);
                        } else {
                            QStringList s=data.split(";");
                            for (int i=0; i<s.size(); i++) {
                                bool ok=false;
                                double d=loc.toDouble(s[i], &ok);
                                if (ok) { r.dvec.append(d); }
                                else { r.dvec.append(0); }
                            }
                        }
                        r.unit=re.attribute("unit", "");
                        r.columns=loc.toInt(re.attribute("columns", "0"));


                    } else if (t=="stringlist" || t=="strlst") {
                        r.type=qfrdreStringVector;
                        QString data="";
                        if (re.hasAttribute("value")) data=re.attribute("value", "");
                        else if (!re.text().isEmpty()) data=re.text();
                        r.svec=deescapifyList(data);
                        r.unit=re.attribute("unit", "");
                    } else if (t=="stringmatrix" || t=="strmtrx") {
                        r.type=qfrdreStringMatrix;
                        QString data="";
                        if (re.hasAttribute("value")) data=re.attribute("value", "");
                        else if (!re.text().isEmpty()) data=re.text();
                        r.svec=deescapifyList(data);
                        r.unit=re.attribute("unit", "");
                        r.columns=loc.toInt(re.attribute("columns", "0"));


                    } else if (t=="numbererrorlist" || t=="numerrlst") {
                        r.type=qfrdreNumberErrorVector;
                        r.dvec.clear();
                        QString storage=re.attribute("storage", "ascii").toLower().trimmed();
                        QString data="0.0";
                        if (re.hasAttribute("value")) data=re.attribute("value", "0.0");
                        else if (!re.text().isEmpty()) data=re.text();
                        QString error=re.attribute("error", "");
                        if (storage=="base64") {
                            r.dvec=stringToDoubleArray_base64(data);
                            r.evec=stringToDoubleArray_base64(error);
                        } else if (storage=="hex") {
                            r.dvec=stringToDoubleArray_hex(data);
                            r.evec=stringToDoubleArray_hex(error);
                        } else {
                            QStringList s=data.split(";");
                            QStringList s1=error.split(";");
                            for (int i=0; i<s.size(); i++) {
                                bool ok=false;
                                double d=loc.toDouble(s[i], &ok);
                                if (ok) { r.dvec.append(d); }
                                else { r.dvec.append(0); }
                                //double e;
                                //if (i<s1.size()) e=loc.toDouble(s1[i], &ok);
                                if (ok) { r.evec.append(d); }
                                else { r.evec.append(0); }
                            }
                        }
                        r.unit=re.attribute("unit", "");
                    } else if (t=="numbererrormatrix" || t=="numerrmtrx") {
                        r.type=qfrdreNumberErrorMatrix;
                        r.dvec.clear();
                        QString storage=re.attribute("storage", "ascii").toLower().trimmed();
                        QString error=re.attribute("error", "");
                        QString data="0.0";
                        if (re.hasAttribute("value")) data=re.attribute("value", "0.0");
                        else if (!re.text().isEmpty()) data=re.text();
                        if (storage=="base64") {
                            r.dvec=stringToDoubleArray_base64(data);
                            r.evec=stringToDoubleArray_base64(error);
                        } else if (storage=="hex") {
                            r.dvec=stringToDoubleArray_hex(data);
                            r.evec=stringToDoubleArray_hex(error);
                        } else {
                            QStringList s=data.split(";");
                            QStringList s1=error.split(";");
                            for (int i=0; i<s.size(); i++) {
                                bool ok=false;
                                double d=loc.toDouble(s[i], &ok);
                                if (ok) { r.dvec.append(d); }
                                else { r.dvec.append(0); }
                                double e;
                                if (i<s1.size()) e=loc.toDouble(s1[i], &ok);
                                if (ok) { r.evec.append(d); }
                                else { r.evec.append(0); }
                            }
                        }
                        r.unit=re.attribute("unit", "");
                        r.columns=loc.toInt(re.attribute("columns", "0"));
                    } else if (t=="integerlist" || t=="intlst") {
                        r.type=qfrdreIntegerVector;
                        r.ivec.clear();
                        QString storage=re.attribute("storage", "ascii").toLower().trimmed();
                        QString data="0";
                        if (re.hasAttribute("value")) data=re.attribute("value", "0");
                        else if (!re.text().isEmpty()) data=re.text();
                        if (storage=="base64") {
                            r.ivec=stringToQlonglongArray_base64(data);
                        } else if (storage=="hex") {
                            r.ivec=stringToQlonglongArray_hex(data);
                        } else {
                            QStringList s=data.split(";");
                            for (int i=0; i<s.size(); i++) {
                                bool ok=false;
                                qlonglong d=loc.toLongLong(s[i], &ok);
                                if (ok) { r.ivec.append(d); }
                                else { r.ivec.append(0); }
                            }
                        }
                        r.unit=re.attribute("unit", "");
                    } else if (t=="integermatrix" || t=="intmtrx") {
                        r.type=qfrdreIntegerMatrix;
                        r.ivec.clear();
                        QString storage=re.attribute("storage", "ascii").toLower().trimmed();
                        QString data="0";
                        if (re.hasAttribute("value")) data=re.attribute("value", "0");
                        else if (!re.text().isEmpty()) data=re.text();
                        if (storage=="base64") {
                            r.ivec=stringToQlonglongArray_base64(data);
                        } else if (storage=="hex") {
                            r.ivec=stringToQlonglongArray_hex(data);
                        } else {
                            QStringList s=data.split(";");
                            for (int i=0; i<s.size(); i++) {
                                bool ok=false;
                                qlonglong d=loc.toLongLong(s[i], &ok);
                                if (ok) { r.ivec.append(d); }
                                else { r.ivec.append(0); }
                            }
                        }
                        r.unit=re.attribute("unit", "");
                        r.columns=loc.toInt(re.attribute("columns", "0"));
                    } else if (t=="booleanlist" || t=="boollst") {
                        r.type=qfrdreBooleanVector;
                        r.bvec.clear();
                        QString storage=re.attribute("storage", "ascii").toLower().trimmed();
                        QString data="0";
                        if (re.hasAttribute("value")) data=re.attribute("value", "0");
                        else if (!re.text().isEmpty()) data=re.text();
                        if (storage=="hex") {
                            r.bvec=stringToBoolArray(data);
                        } else {
                            QStringList s=data.split(";");
                            for (int i=0; i<s.size(); i++) {
                                if (s[i]!="0") { r.bvec.append(true); }
                                else { r.bvec.append(false); }
                            }
                        }
                        r.unit=re.attribute("unit", "");
                    } else if (t=="booleanmatrix" || t=="boolmtrx") {
                        r.type=qfrdreBooleanMatrix;
                        r.bvec.clear();
                        QString storage=re.attribute("storage", "ascii").toLower().trimmed();
                        QString data="0";
                        if (re.hasAttribute("value")) data=re.attribute("value", "0");
                        else if (!re.text().isEmpty()) data=re.text();
                        if (storage=="hex") {
                            r.bvec=stringToBoolArray(data);
                        } else {
                            QStringList s=data.split(";");
                            for (int i=0; i<s.size(); i++) {
                                if (s[i]!="0") { r.bvec.append(true); }
                                else { r.bvec.append(false); }
                            }
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
        if (!loadAsDummy && !te.isNull()) {
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
                QString descxml=fe.attribute("description", "");
                QFileInfo fi(project->getFile());
                //std::cout<<"file = "<<filexml.toStdString()<<"\n";
                //std::cout<<"  project-absolute path = "<<fi.absoluteDir().absolutePath().toStdString()<<"\n";
                //std::cout<<"  file-absolute path = "<<fi.absoluteDir().absoluteFilePath(filexml).toStdString()<<"\n";

                files.push_back(fi.absoluteDir().absoluteFilePath(filexml));
                files_types.append(typexml);
                files_desciptions.append(descxml);
                fe=fe.nextSiblingElement("file");
            }
        }
        //std::cout<<"    reading XML: data\n";
        te=e.firstChildElement("data");
        if (!loadAsDummy) intReadData(&te);

    }
    //std::cout<<"reading XML: done!\n";
    if (!errorOcc) {
        emit propertiesChanged("", true);
        emit basicPropertiesChanged();
        emit rawDataChanged();
    }
    if (resultsmodel) resultsmodel->init(this);
}

QString QFRawDataRecord::evaluationResultType2String(QFRawDataRecord::evaluationResultType type) {
    switch(type) {
        case qfrdreNumber: return tr("number");
        case qfrdreNumberError: return tr("number+error");
        case qfrdreNumberVector: return tr("number vector");
        case qfrdreNumberMatrix: return tr("number matrix");
        case qfrdreNumberErrorVector: return tr("number+error vector");
        case qfrdreNumberErrorMatrix: return tr("number+error matrix");
        case qfrdreInteger: return tr("integer");
        case qfrdreIntegerVector: return tr("integer vector");
        case qfrdreIntegerMatrix: return tr("integer matrix");
        case qfrdreString: return tr("string");
        case qfrdreStringVector: return tr("string vector");
        case qfrdreStringMatrix: return tr("string matrix");
        case qfrdreBoolean: return tr("boolean");
        case qfrdreBooleanVector: return tr("boolean vector");
        case qfrdreBooleanMatrix: return tr("boolean matrix");
        case qfrdreInvalid: return tr("invalid");
    }

    return tr("invalid");
}


void QFRawDataRecord::writeXML(QXmlStreamWriter& w) {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    w.writeStartElement("rawdataelement");
    w.writeAttribute("type", getType());
    w.writeAttribute("id", QString::number(ID));
    w.writeAttribute("group", QString::number(group));
    w.writeAttribute("name", name);
    if (!folder.isEmpty()) w.writeAttribute("folder", folder);
    if (!role.isEmpty()) w.writeAttribute("role", role);
    if (!description.isEmpty()) {
        w.writeStartElement("description");
        w.writeCDATA(description);
        w.writeEndElement();
    }
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
        if (!i.value()->group.isEmpty()) w.writeAttribute("group", i.value()->group);
        w.writeAttribute("groupindex", QString::number(i.value()->groupIndex));
        if (!i.value()->description.isEmpty()) w.writeAttribute("description", i.value()->description);
        QFRawDataRecordPrivate::ResultsResultsIterator  j(i.value()->results);
        //for (int j=0; j<i.value().size(); j++) {
        while (j.hasNext()) {
            j.next();
            w.writeStartElement("result");
            QString rn=j.key();

            QElapsedTimer saveTimer;
            saveTimer.start();
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
                    w.writeAttribute("storage", "base64");
                    w.writeAttribute("unit", r.unit);
                    w.writeAttribute("value", doubleArrayToString_base64(r.dvec));
                    } break;
                case qfrdreNumberMatrix: {
                    w.writeAttribute("type", "numbermatrix");
                    w.writeAttribute("columns", loc.toString(r.columns));
                    w.writeAttribute("storage", "base64");
                    w.writeAttribute("unit", r.unit);
                    w.writeAttribute("value", doubleArrayToString_base64(r.dvec));
                    } break;
                case qfrdreNumberErrorVector: {
                    w.writeAttribute("type", "numbererrorlist");
                    w.writeAttribute("storage", "base64");
                    w.writeAttribute("unit", r.unit);
                    w.writeAttribute("value", doubleArrayToString_base64(r.dvec));
                    w.writeAttribute("error", doubleArrayToString_base64(r.evec));
                    } break;
                case qfrdreNumberErrorMatrix: {
                    w.writeAttribute("type", "numbererrormatrix");
                    w.writeAttribute("columns", loc.toString(r.columns));
                    w.writeAttribute("storage", "base64");
                    w.writeAttribute("unit", r.unit);
                    w.writeAttribute("value", doubleArrayToString_base64(r.dvec));
                    w.writeAttribute("error", doubleArrayToString_base64(r.evec));
                    } break;
                case qfrdreNumberError:
                    w.writeAttribute("type", "numbererror");
                    w.writeAttribute("value", loc.toString(r.dvalue, 'g', 10));
                    w.writeAttribute("error", loc.toString(r.derror, 'g', 10));
                    w.writeAttribute("unit", r.unit);
                    break;
                case qfrdreIntegerMatrix: {
                    w.writeAttribute("type", "integermatrix");
                    w.writeAttribute("columns", loc.toString(r.columns));
                    w.writeAttribute("storage", "base64");
                    w.writeAttribute("unit", r.unit);
                    w.writeAttribute("value", qlonglongArrayToString_base64(r.ivec));
                    } break;
                case qfrdreIntegerVector: {
                    w.writeAttribute("type", "integerlist");
                    w.writeAttribute("storage", "hex");
                    w.writeAttribute("unit", r.unit);
                    w.writeAttribute("value", qlonglongArrayToString_base64(r.ivec));
                    } break;
                case qfrdreBooleanMatrix: {
                    w.writeAttribute("type", "booleanmatrix");
                    w.writeAttribute("columns", loc.toString(r.columns));
                    w.writeAttribute("storage", "base64");
                    w.writeAttribute("unit", r.unit);
                    w.writeAttribute("value", boolArrayToString(r.bvec));
                    } break;
                case qfrdreBooleanVector: {
                    w.writeAttribute("type", "booleanlist");
                    w.writeAttribute("storage", "hex");
                    w.writeAttribute("unit", r.unit);
                    w.writeAttribute("value", boolArrayToString(r.bvec));
                    } break;
                case qfrdreStringVector: {
                    w.writeAttribute("type", "stringlist");
                    w.writeAttribute("unit", r.unit);
                    w.writeAttribute("value", escapify(r.svec));
                    } break;
                case qfrdreStringMatrix: {
                    w.writeAttribute("type", "stringmatrix");
                    w.writeAttribute("columns", loc.toString(r.columns));
                    w.writeAttribute("unit", r.unit);
                    w.writeAttribute("value", escapify(r.svec));
                    } break;

            }
            //qDebug()<<"   writing property "<<rn<<"   in "<<saveTimer.elapsed()<<" ms";
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
            //file=fi.absoluteDir().relativeFilePath(files[i]);
            file=QDir(fi.canonicalPath()).relativeFilePath(QFileInfo(files[i]).canonicalFilePath());
            w.writeStartElement("file");
            if (i<files_types.size()) {
                if (!files_types[i].isEmpty()) {
                    if (i<files_types.size()) w.writeAttribute("type", files_types[i]);
                    if (i<files_desciptions.size()) w.writeAttribute("description", files_desciptions[i]);
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


QString QFRawDataRecord::getType() const {
    return tr("unknown");
}

QString QFRawDataRecord::getTypeName() const {
    return tr("unknown");
}

QIcon QFRawDataRecord::getSmallIcon() const {
    return QIcon(":/lib/projecttree_emptyrdr.png");
}

QString QFRawDataRecord::getTypeDescription() const {
    return tr("unknown");
}

QIcon QFRawDataRecord::getLargeIcon() const {
    return QIcon(":/lib/projecttree_emptyrdr.png");
}

int QFRawDataRecord::getEditorCount() {
    return 0;
}

QString QFRawDataRecord::getEditorName(int i) {
    return QString("");
}

QFRawDataEditor* QFRawDataRecord::createEditor(QFPluginServices* services,  QFRawDataPropertyEditor *propEditor, int i, QWidget* parent) {
    return NULL;
}

void QFRawDataRecord::exportData(const QString& format, const QString& filename) const {

}

QStringList QFRawDataRecord::getExportFiletypes() {
    return QStringList();
}

QString QFRawDataRecord::getExportDialogTitle() {
    return tr("");
}

QString QFRawDataRecord::getExportDialogFiletypes() {
    return tr("");
}


QFRawDataRecord::FileListEditOptions QFRawDataRecord::isFilesListEditable() const
{
    return QFRawDataRecord::FilesNotEditable;
}

bool QFRawDataRecord::selectNewFiles(QStringList &files, QStringList &types, QStringList &descriptions) const
{
    return false;
}

bool QFRawDataRecord::mayDeleteFiles(QStringList &files, QStringList &types, QStringList &descriptions) const
{
    return true;
}

bool QFRawDataRecord::reloadFromFiles()
{
    return true;
}

QStringList QFRawDataRecord::getPossibleFilesTypes() const
{
    return QStringList();
}



void QFRawDataRecord::resultsClearAll() {
    {
        
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        QFRawDataRecordPrivate::ResultsIterator j(dstore->results);
        while (j.hasNext()) {
            j.next();
            delete j.value();
        }
        dstore->results.clear();
    }
    emitResultsChanged();
}

void QFRawDataRecord::resultsClear(const QString& name) {
    bool em=false;
    {
        
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif

        if (dstore->results.contains(name)) {
            //qDebug()<<"resultsClear("<<name<<")";
            dstore->results[name]->results.clear();
            delete dstore->results[name];
            dstore->results.remove(name);
            em=true;

        }
    }
    if (em) emitResultsChanged(name, "", true);
}

int QFRawDataRecord::resultsGetCount(const QString& evalName) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if (dstore->results.contains(evalName)) return dstore->results[evalName]->results.size();
    return 0;
};

int QFRawDataRecord::resultsGetEvaluationCount() const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    return dstore->results.size();
};

QString QFRawDataRecord::resultsGetEvaluationName(int i) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if ((long)i<dstore->results.size()) return dstore->results.keys().at(i);
    return QString("");
};

void QFRawDataRecord::resultsSetNumber(const QString& evaluationName, const QString& resultName, double value, const QString& unit) {
    {    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        evaluationResult r;
        r.type=qfrdreNumber;
        r.dvalue=value;
        r.unit=unit;
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        dstore->results[evaluationName]->results.insert(resultName, r);
    }
    emitResultsChanged(evaluationName, resultName, false);
};

void QFRawDataRecord::resultsRemove(const QString& evalName, const QString& resultName, bool emitChangedSignal) {
    bool changed=false;
    {    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        if (dstore->results.contains(evalName)) {
            if (dstore->results[evalName]->results.remove(resultName)>0) changed=true;
            if (dstore->results[evalName]->results.isEmpty()) {
                #ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"unlock";
#endif
 locker.unlock();
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  unlocked";
#endif
                resultsClear(evalName);
                changed=true;
            }
        }
    }
    if (changed && emitChangedSignal && doEmitResultsChanged) emit resultsChanged(evalName, resultName, true);
}

void QFRawDataRecord::resultsClear(const QString& name, const QString& postfix) {
    bool em=false;
    {
        
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        if (dstore->results.contains(name)) {
            QFRawDataRecordPrivate::ResultsResultsIterator i(dstore->results[name]->results);
            while (i.hasNext()) {
                i.next();
                //cout << i.key() << ": " << i.value() << endl;
                if (i.key().endsWith(postfix)) dstore->results[name]->results.remove(i.key());
            }
            if (dstore->results[name]->results.isEmpty()) dstore->results.remove(name);
            em=true;

        }
    }
    if (em) emitResultsChanged(name, "", true);
}

bool QFRawDataRecord::resultsExists(const QString& evalName, const QString& resultName) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if (dstore->results.contains(evalName)) {
        return dstore->results[evalName]->results.contains(resultName);
    }
    return false;
}

bool QFRawDataRecord::resultsExistsFromEvaluation(const QString& evalName) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    return dstore->results.contains(evalName);
}

void QFRawDataRecord::resultsSetNumberList(const QString& evaluationName, const QString& resultName, const QVector<double>& value, const QString& unit) {
    {    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        evaluationResult r;
        r.type=qfrdreNumberVector;
        r.dvec=value;
        r.unit=unit;
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        dstore->results[evaluationName]->results.insert(resultName, r);
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetNumberMatrix(const QString& evaluationName, const QString& resultName, const QVector<double>& value, int columns, const QString& unit) {
    {
        
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif

        evaluationResult r;
        r.type=qfrdreNumberMatrix;
        r.dvec=value;
        r.unit=unit;
        r.columns=columns;
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        dstore->results[evaluationName]->results.insert(resultName, r);
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetNumberList(const QString& evaluationName, const QString& resultName, double* value, int items, const QString& unit) {
    QVector<double> data;
    for (int i=0; i<items; i++) {
        data.append(value[i]);
    }
    resultsSetNumberList(evaluationName, resultName, data, unit);
}

void QFRawDataRecord::resultsSetNumberMatrix(const QString& evaluationName, const QString& resultName, double* value, int columns, int rows, const QString& unit) {
    QVector<double> data;
    for (int i=0; i<columns*rows; i++) {
        data.append(value[i]);
    }
    resultsSetNumberMatrix(evaluationName, resultName, data, columns, unit);
}

void QFRawDataRecord::resultsSetNumberErrorList(const QString& evaluationName, const QString& resultName, const QVector<double> &value, const QVector<double> &error, const QString& unit) {
    {
        
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        evaluationResult r;
        r.type=qfrdreNumberErrorVector;
        r.dvec=value;
        r.evec=error;
        r.unit=unit;
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        dstore->results[evaluationName]->results.insert(resultName, r);
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetNumberErrorMatrix(const QString& evaluationName, const QString& resultName, const QVector<double> &value, const QVector<double> &error, int columns, const QString& unit) {
    {
        
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif

        evaluationResult r;
        r.type=qfrdreNumberErrorMatrix;
        r.dvec=value;
        r.evec=error;
        r.unit=unit;
        r.columns=columns;
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        dstore->results[evaluationName]->results.insert(resultName, r);
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetNumberErrorList(const QString& evaluationName, const QString& resultName, double *value, double *error, int items, const QString& unit) {
    QVector<double> data, datae;
    for (int i=0; i<items; i++) {
        data.append(value[i]);
        datae.append(error[i]);
    }
    resultsSetNumberErrorList(evaluationName, resultName, data, datae, unit);
}

void QFRawDataRecord::resultsSetNumberErrorMatrix(const QString& evaluationName, const QString& resultName, double *value, double *error, int columns, int rows, const QString& unit) {
    QVector<double> data, datae;
    for (int i=0; i<columns*rows; i++) {
        data.append(value[i]);
        datae.append(error[i]);
    }
    resultsSetNumberErrorMatrix(evaluationName, resultName, data, datae, columns, unit);
}


void QFRawDataRecord::resultsSetInNumberList(const QString &evaluationName, const QString &resultName, int position, double value, const QString &unit) {
    {
        
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        if (!dstore->results[evaluationName]->results.contains(resultName))  dstore->results[evaluationName]->results.insert(resultName, evaluationResult());

        evaluationResult& r=dstore->results[evaluationName]->results[resultName];
        r.type=qfrdreNumberVector;
        r.unit=unit;

        if (position>=r.dvec.size()) {
            for (int i=r.dvec.size(); i<=position; i++) r.dvec.append(0.0);
        }
        r.dvec[position]=value;
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetInNumberMatrix(const QString &evaluationName, const QString &resultName, int row, int column, double value, const QString &unit) {
    {    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif

        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        if (!dstore->results[evaluationName]->results.contains(resultName))  dstore->results[evaluationName]->results.insert(resultName, evaluationResult());

        evaluationResult& r=dstore->results[evaluationName]->results[resultName];
        r.type=qfrdreNumberMatrix;
        r.unit=unit;

        int position=column+row*r.columns;
        if (position>=r.dvec.size()) {
            for (int i=r.dvec.size(); i<=position; i++) r.dvec.append(0.0);
        }
        r.dvec[position]=value;
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetInNumberErrorList(const QString &evaluationName, const QString &resultName, int position, double value, double error, const QString &unit) {
    {    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif

        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        if (!dstore->results[evaluationName]->results.contains(resultName))  dstore->results[evaluationName]->results.insert(resultName, evaluationResult());

        evaluationResult& r=dstore->results[evaluationName]->results[resultName];
        r.type=qfrdreNumberErrorVector;
        r.unit=unit;

        if (position>=r.dvec.size()) {
            for (int i=r.dvec.size(); i<=position; i++) r.dvec.append(0.0);
        }
        if (position>=r.evec.size()) {
            for (int i=r.evec.size(); i<=position; i++) r.evec.append(0.0);
        }
        r.dvec[position]=value;
        r.evec[position]=error;
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetInNumberErrorListAndBool(const QString &evaluationName, const QString &resultName, int position, double value, double error, const QString &unit, const QString &resultBoolName, bool boolValue)
{
    {
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif

        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results[evaluationName];
        if (!evr->results.contains(resultName))  evr->results.insert(resultName, evaluationResult());
        if (!evr->results.contains(resultBoolName))  evr->results.insert(resultBoolName, evaluationResult());

        evaluationResult& r=evr->results[resultName];
        r.type=qfrdreNumberErrorVector;
        r.unit=unit;

        if (position>=r.dvec.size()) {
            for (int i=r.dvec.size(); i<=position; i++) r.dvec.append(0.0);
        }
        if (position>=r.evec.size()) {
            for (int i=r.evec.size(); i<=position; i++) r.evec.append(0.0);
        }
        r.dvec[position]=value;
        r.evec[position]=error;

        evaluationResult& r1=evr->results[resultBoolName];
        r1.type=qfrdreBooleanVector;

        if (position>=r1.bvec.size()) {
            for (int i=r1.bvec.size(); i<=position; i++) r1.bvec.append(false);
        }
        r1.bvec[position]=boolValue;
    }
    emitResultsChanged(evaluationName, resultName, false);
    emitResultsChanged(evaluationName, resultBoolName, false);
}

void QFRawDataRecord::resultsSetInNumberListAndBool(const QString &evaluationName, const QString &resultName, int position, double value, const QString &unit, const QString &resultBoolName, bool boolValue)
{
    {
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif

        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results[evaluationName];
        if (!evr->results.contains(resultName))  evr->results.insert(resultName, evaluationResult());
        if (!evr->results.contains(resultBoolName))  evr->results.insert(resultBoolName, evaluationResult());

        evaluationResult& r=evr->results[resultName];
        r.type=qfrdreNumberVector;
        r.unit=unit;

        if (position>=r.dvec.size()) {
            for (int i=r.dvec.size(); i<=position; i++) r.dvec.append(0.0);
        }
        r.dvec[position]=value;

        evaluationResult& r1=evr->results[resultBoolName];
        r1.type=qfrdreBooleanVector;

        if (position>=r1.bvec.size()) {
            for (int i=r1.bvec.size(); i<=position; i++) r1.bvec.append(false);
        }
        r1.bvec[position]=boolValue;
    }
    emitResultsChanged(evaluationName, resultName, false);
    emitResultsChanged(evaluationName, resultBoolName, false);
}

void QFRawDataRecord::resultsSetInIntegerListAndBool(const QString &evaluationName, const QString &resultName, int position, qlonglong value, const QString &unit, const QString &resultBoolName, bool boolValue)
{
    {
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif

        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results[evaluationName];
        if (!evr->results.contains(resultName))  evr->results.insert(resultName, evaluationResult());
        if (!evr->results.contains(resultBoolName))  evr->results.insert(resultBoolName, evaluationResult());

        evaluationResult& r=evr->results[resultName];
        r.type=qfrdreIntegerVector;
        r.unit=unit;

        if (position>=r.ivec.size()) {
            for (int i=r.ivec.size(); i<=position; i++) r.ivec.append(0.0);
        }
        r.ivec[position]=value;

        evaluationResult& r1=evr->results[resultBoolName];
        r1.type=qfrdreBooleanVector;

        if (position>=r1.bvec.size()) {
            for (int i=r1.bvec.size(); i<=position; i++) r1.bvec.append(false);
        }
        r1.bvec[position]=boolValue;
    }
    emitResultsChanged(evaluationName, resultName, false);
    emitResultsChanged(evaluationName, resultBoolName, false);

}

void QFRawDataRecord::resultsSetInStringListAndBool(const QString &evaluationName, const QString &resultName, int position, const QString &value, const QString &unit, const QString &resultBoolName, bool boolValue)
{
    {
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif

        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results[evaluationName];
        if (!evr->results.contains(resultName))  evr->results.insert(resultName, evaluationResult());
        if (!evr->results.contains(resultBoolName))  evr->results.insert(resultBoolName, evaluationResult());

        evaluationResult& r=evr->results[resultName];
        r.type=qfrdreStringVector;
        r.unit=unit;

        if (position>=r.svec.size()) {
            for (int i=r.svec.size(); i<=position; i++) r.svec.append("");
        }
        r.svec[position]=value;

        evaluationResult& r1=evr->results[resultBoolName];
        r1.type=qfrdreBooleanVector;

        if (position>=r1.bvec.size()) {
            for (int i=r1.bvec.size(); i<=position; i++) r1.bvec.append(false);
        }
        r1.bvec[position]=boolValue;
    }
    emitResultsChanged(evaluationName, resultName, false);
    emitResultsChanged(evaluationName, resultBoolName, false);

}

void QFRawDataRecord::resultsSetInBooleanListAndBool(const QString &evaluationName, const QString &resultName, int position, bool value, const QString &unit, const QString &resultBoolName, bool boolValue)
{
    {
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif

        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results[evaluationName];
        if (!evr->results.contains(resultName))  evr->results.insert(resultName, evaluationResult());
        if (!evr->results.contains(resultBoolName))  evr->results.insert(resultBoolName, evaluationResult());

        evaluationResult& r=evr->results[resultName];
        r.type=qfrdreBooleanVector;
        r.unit=unit;

        if (position>=r.bvec.size()) {
            for (int i=r.bvec.size(); i<=position; i++) r.bvec.append(false);
        }
        r.bvec[position]=value;

        evaluationResult& r1=evr->results[resultBoolName];
        r1.type=qfrdreBooleanVector;

        if (position>=r1.bvec.size()) {
            for (int i=r1.bvec.size(); i<=position; i++) r1.bvec.append(false);
        }
        r1.bvec[position]=boolValue;
    }
    emitResultsChanged(evaluationName, resultName, false);
    emitResultsChanged(evaluationName, resultBoolName, false);

}


void QFRawDataRecord::resultsSetErrorInNumberErrorList(const QString &evaluationName, const QString &resultName, int position, double error) {
     {    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
     if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
     QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results[evaluationName];
        if (!evr->results.contains(resultName))  evr->results.insert(resultName, evaluationResult());

        evaluationResult& r=evr->results[resultName];
        r.type=qfrdreNumberErrorVector;

        if (position>=r.evec.size()) {
            for (int i=r.evec.size(); i<=position; i++) r.evec.append(0.0);
        }
        r.evec[position]=error;
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetInNumberErrorMatrix(const QString &evaluationName, const QString &resultName, int row, int column, double value, double error, const QString &unit) {
    {   
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif

        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results[evaluationName];
        if (!evr->results.contains(resultName))  evr->results.insert(resultName, evaluationResult());

        evaluationResult& r=evr->results[resultName];
        r.type=qfrdreNumberErrorMatrix;
        r.unit=unit;

        int position=column+row*r.columns;
        if (position>=r.dvec.size()) {
            for (int i=r.dvec.size(); i<=position; i++) r.dvec.append(0.0);
        }
        if (position>=r.evec.size()) {
            for (int i=r.evec.size(); i<=position; i++) r.evec.append(0.0);
        }
        r.dvec[position]=value;
        r.evec[position]=error;
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetInIntegerList(const QString &evaluationName, const QString &resultName, int position, qlonglong value, const QString &unit) {
    {    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif

        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results[evaluationName];
        if (!evr->results.contains(resultName))  evr->results.insert(resultName, evaluationResult());

        evaluationResult& r=evr->results[resultName];
        r.type=qfrdreIntegerVector;
        r.unit=unit;

        if (position>=r.ivec.size()) {
            for (int i=r.ivec.size(); i<=position; i++) r.ivec.append(0);
        }
        r.ivec[position]=value;
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetInIntegerMatrix(const QString &evaluationName, const QString &resultName, int row, int column, qlonglong value, const QString &unit) {
    {     
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif

        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results[evaluationName];
        if (!evr->results.contains(resultName))  evr->results.insert(resultName, evaluationResult());

        evaluationResult& r=evr->results[resultName];
        r.type=qfrdreNumberMatrix;
        r.unit=unit;

        int position=column+row*r.columns;
        if (position>=r.ivec.size()) {
            for (int i=r.ivec.size(); i<=position; i++) r.ivec.append(0);
        }
        r.ivec[position]=value;
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetInBooleanList(const QString &evaluationName, const QString &resultName, int position, bool value, const QString &unit) {
    {     
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results[evaluationName];
        if (!evr->results.contains(resultName))  evr->results.insert(resultName, evaluationResult());

        evaluationResult& r=evr->results[resultName];
        r.type=qfrdreBooleanVector;
        r.unit=unit;

        if (position>=r.bvec.size()) {
            for (int i=r.bvec.size(); i<=position; i++) r.bvec.append(0);
        }
        r.bvec[position]=value;
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetInBooleanMatrix(const QString &evaluationName, const QString &resultName, int row, int column, bool value, const QString &unit) {
    {
        
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results[evaluationName];
        if (!evr->results.contains(resultName))  evr->results.insert(resultName, evaluationResult());

        evaluationResult& r=evr->results[resultName];
        r.type=qfrdreBooleanMatrix;
        r.unit=unit;

        int position=column+row*r.columns;
        if (position>=r.bvec.size()) {
            for (int i=r.bvec.size(); i<=position; i++) r.bvec.append(0);
        }
        r.bvec[position]=value;
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetInStringList(const QString &evaluationName, const QString &resultName, int position, const QString &value, const QString &unit) {
    {     
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif

        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results[evaluationName];
        if (!evr->results.contains(resultName))  evr->results.insert(resultName, evaluationResult());

        evaluationResult& r=evr->results[resultName];
        r.type=qfrdreStringVector;
        r.unit=unit;

        if (position>=r.svec.size()) {
            for (int i=r.svec.size(); i<=position; i++) r.svec.append("");
        }
        r.svec[position]=value;
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetInStringMatrix(const QString &evaluationName, const QString &resultName, int row, int column, const QString &value, const QString &unit) {
    {     
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results[evaluationName];
        if (!evr->results.contains(resultName))  evr->results.insert(resultName, evaluationResult());

        evaluationResult& r=evr->results[resultName];
        r.type=qfrdreStringMatrix;
        r.unit=unit;

        int position=column+row*r.columns;
        if (position>=r.svec.size()) {
            for (int i=r.svec.size(); i<=position; i++) r.svec.append(0);
        }
        r.svec[position]=value;
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsResetInMatrix(const QString &evaluationName, const QString &resultName, int row, int column) {
    {   
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        if (!dstore->results.contains(evaluationName)) return;
        QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results[evaluationName];
        if (!evr->results.contains(resultName)) return;

        evaluationResult& r=evr->results[resultName];
        #ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"unlock";
#endif
 locker.unlock();
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  unlocked";
#endif
        resultsResetInList(evaluationName, resultName, column+row*r.columns);
    }
}

void QFRawDataRecord::resultsResetInList(const QString &evaluationName, const QString &resultName, int position) {
    {    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif

        if (!dstore->results.contains(evaluationName)) return;
        QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results[evaluationName];
        if (!evr->results.contains(resultName)) return;

        evaluationResult& r=evr->results[resultName];

        switch(r.type) {
            case qfrdreStringVector:
            case qfrdreStringMatrix:
                if (position<r.svec.size()) {
                    r.svec[position]="";
                }
                break;
            case qfrdreIntegerVector:
            case qfrdreIntegerMatrix:
                if (position<r.ivec.size()) {
                    r.ivec[position]=0;
                }
                break;
            case qfrdreBooleanVector:
            case qfrdreBooleanMatrix:
                if (position<r.bvec.size()) {
                    r.bvec[position]=false;
                }
                break;
            case qfrdreNumberVector:
            case qfrdreNumberMatrix:
                if (position<r.dvec.size()) {
                    r.dvec[position]=0;
                }
                break;
            case qfrdreNumberErrorVector:
            case qfrdreNumberErrorMatrix:
                if (position<r.dvec.size()) {
                    r.dvec[position]=0;
                }
                if (position<r.evec.size()) {
                    r.evec[position]=0;
                }
                break;
            default: break;
        }
    }
    emitResultsChanged(evaluationName, resultName, false);
}


double QFRawDataRecord::resultsGetInNumberList(const QString &evaluationName, const QString &resultName, int position, double defaultValue) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif

    if (!dstore->results.contains(evaluationName)) return defaultValue;
    const QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results.value(evaluationName);
    if (!evr->results.contains(resultName)) return defaultValue;

    const evaluationResult& r=evr->results.value(resultName);
    if (position<r.dvec.size()) return r.dvec[position];

    return defaultValue;
}

void QFRawDataRecord::resultsSetIntegerList(const QString &evaluationName, const QString &resultName, const QVector<qlonglong> &value, const QString &unit) {
    {
        
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        evaluationResult r;
        r.type=qfrdreIntegerVector;
        r.ivec=value;
        r.unit=unit;
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        dstore->results[evaluationName]->results.insert(resultName, r);
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetIntegerMatrix(const QString &evaluationName, const QString &resultName, const QVector<qlonglong> &value, int columns, const QString &unit) {
    { 
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        evaluationResult r;
        r.type=qfrdreIntegerMatrix;
        r.ivec=value;
        r.unit=unit;
        r.columns=columns;
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        dstore->results[evaluationName]->results.insert(resultName, r);
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetIntegerList(const QString &evaluationName, const QString &resultName, qlonglong *value, int items, const QString &unit) {
    QVector<qlonglong> data;
    for (int i=0; i<items; i++) {
        data.append(value[i]);
    }
    resultsSetIntegerList(evaluationName, resultName, data, unit);
}

void QFRawDataRecord::resultsSetIntegerMatrix(const QString &evaluationName, const QString &resultName, qlonglong *value, int columns, int rows, const QString &unit) {
    QVector<qlonglong> data;
    for (int i=0; i<columns*rows; i++) {
        data.append(value[i]);
    }
    resultsSetIntegerMatrix(evaluationName, resultName, data, columns, unit);
}

void QFRawDataRecord::resultsSetBooleanList(const QString &evaluationName, const QString &resultName, const QVector<bool> &value, const QString &unit) {
    { 
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        evaluationResult r;
        r.type=qfrdreBooleanVector;
        r.bvec=value;
        r.unit=unit;
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        dstore->results[evaluationName]->results.insert(resultName, r);
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetBooleanMatrix(const QString &evaluationName, const QString &resultName, const QVector<bool> &value, int columns, const QString &unit) {
    { 
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        evaluationResult r;
        r.type=qfrdreBooleanMatrix;
        r.bvec=value;
        r.unit=unit;
        r.columns=columns;
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        dstore->results[evaluationName]->results.insert(resultName, r);
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetBooleanList(const QString &evaluationName, const QString &resultName, bool *value, int items, const QString &unit) {
    QVector<bool> data;
    for (int i=0; i<items; i++) {
        data.append(value[i]);
    }
    resultsSetBooleanList(evaluationName, resultName, data, unit);
}

void QFRawDataRecord::resultsSetBooleanMatrix(const QString &evaluationName, const QString &resultName, bool *value, int columns, int rows, const QString &unit) {
    QVector<bool> data;
    for (int i=0; i<columns*rows; i++) {
        data.append(value[i]);
    }
    resultsSetBooleanMatrix(evaluationName, resultName, data, columns, unit);
}

void QFRawDataRecord::resultsSetStringList(const QString &evaluationName, const QString &resultName, const QVector<QString> &value, const QString &unit) {
    { 
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        evaluationResult r;
        r.type=qfrdreStringVector;
        r.svec=value.toList();
        r.unit=unit;
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        dstore->results[evaluationName]->results.insert(resultName, r);
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetStringMatrix(const QString &evaluationName, const QString &resultName, const QVector<QString> &value, int columns, const QString &unit) {
    { 
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        evaluationResult r;
        r.type=qfrdreStringMatrix;
        r.svec=value.toList();
        r.unit=unit;
        r.columns=columns;
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        dstore->results[evaluationName]->results.insert(resultName, r);
    }
    emitResultsChanged(evaluationName, resultName, false);
}



void QFRawDataRecord::resultsSetNumberError(const QString& evaluationName, const QString& resultName, double value, double error, const QString& unit)  {
    { 
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        evaluationResult r;
        r.type=qfrdreNumberError;
        r.dvalue=value;
        r.derror=error;
        r.unit=unit;
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        dstore->results[evaluationName]->results.insert(resultName, r);
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetNumberErrorError(const QString &evaluationName, const QString &resultName, double error) {
    { 
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        evaluationResult r;
        r.type=qfrdreNumberError;
        r.derror=error;
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        dstore->results[evaluationName]->results.insert(resultName, r);
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetInteger(const QString& evaluationName, const QString& resultName, int64_t value, const QString& unit) {
    { 
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        evaluationResult r;
        r.type=qfrdreInteger;
        r.ivalue=value;
        r.unit=unit;
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        dstore->results[evaluationName]->results.insert(resultName, r);
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetString(const QString& evaluationName, const QString& resultName, const QString& value) {
    { 
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        evaluationResult r;
        r.type=qfrdreString;
        r.svalue=value;
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        dstore->results[evaluationName]->results.insert(resultName, r);
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetLabel(const QString& evaluationName, const QString& resultName, const QString& label, const QString& label_rich) {
    bool ok=true;
    #ifdef DEBUG_THREAN
    qDebug()<<Q_FUNC_INFO<<"QReadLocker";
    #endif
     //QReadLocker locker(lock);
    lock->lockForRead();
    //qDebug()<<"resultsSetGroup("<<evaluationName<<", "<<resultName<<", "<<group<<"): lock read";
    #ifdef DEBUG_THREAN
     qDebug()<<Q_FUNC_INFO<<"  locked";
    #endif
     evaluationResult r;
     if (dstore->results.contains(evaluationName)) {
         if (dstore->results.value(evaluationName)->results.contains(resultName)) {
             r=dstore->results.value(evaluationName)->results.value(resultName);
             ok=((r.label!=label) || (r.label_rich!=label_rich));
         }
     }
     //qDebug()<<"resultsSetGroup("<<evaluationName<<", "<<resultName<<", "<<group<<"): ok="<<ok<<"   unlock read";
     lock->unlock();


    if (ok) {
        
        #ifdef DEBUG_THREAN
        qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
        #endif
         QWriteLocker locker(lock);
        #ifdef DEBUG_THREAN
         qDebug()<<Q_FUNC_INFO<<"  locked";
        #endif
         if (dstore->results.contains(evaluationName)) {
             QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results[evaluationName];
             if (evr->results.contains(resultName)) {
                 r=evr->results[resultName];
             }
         }
         r.label=label;
         r.label_rich=label_rich;
         if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
         dstore->results[evaluationName]->results[resultName]=r;
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetGroup(const QString& evaluationName, const QString& resultName, const QString& group) {
/*        if (!resultsExists(evaluationName, resultName)) return;
        evaluationResult r=resultsGet(evaluationName, resultName);*/
    bool ok=true;
    #ifdef DEBUG_THREAN
    qDebug()<<Q_FUNC_INFO<<"QReadLocker";
    #endif
     //QReadLocker locker(lock);
    lock->lockForRead();
    //qDebug()<<"resultsSetGroup("<<evaluationName<<", "<<resultName<<", "<<group<<"): lock read";
    #ifdef DEBUG_THREAN
     qDebug()<<Q_FUNC_INFO<<"  locked";
    #endif
     if (dstore->results.contains(evaluationName)) {
         if (dstore->results.value(evaluationName)->results.contains(resultName)) {
             ok=(dstore->results.value(evaluationName)->results.value(resultName).group!=group);
         }
     }
     //qDebug()<<"resultsSetGroup("<<evaluationName<<", "<<resultName<<", "<<group<<"): ok="<<ok<<"   unlock read";
     lock->unlock();


    if (ok) {
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 //QWriteLocker locker(lock);
 lock->lockForWrite();
 //qDebug()<<"resultsSetGroup("<<evaluationName<<", "<<resultName<<", "<<group<<"): lock write";
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
         evaluationResult r;
         if (dstore->results.contains(evaluationName)) {
             if (dstore->results.value(evaluationName)->results.contains(resultName)) {
                 r=dstore->results.value(evaluationName)->results.value(resultName);
             }
         }
         r.group=group;
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        dstore->results[evaluationName]->results[resultName]=r;
        //qDebug()<<"resultsSetGroup("<<evaluationName<<", "<<resultName<<", "<<group<<"): unlock write";
        lock->unlock();
        emitResultsChanged(evaluationName, resultName, false);
    }

}

void QFRawDataRecord::resultsSetSortPriority(const QString& evaluationName, const QString& resultName, bool pr) {
    bool ok=true;
    #ifdef DEBUG_THREAN
    qDebug()<<Q_FUNC_INFO<<"QReadLocker";
    #endif
     //QReadLocker locker(lock);
    lock->lockForRead();
    //qDebug()<<"resultsSetGroup("<<evaluationName<<", "<<resultName<<", "<<group<<"): lock read";
    #ifdef DEBUG_THREAN
     qDebug()<<Q_FUNC_INFO<<"  locked";
    #endif
     if (dstore->results.contains(evaluationName)) {
         if (dstore->results.value(evaluationName)->results.contains(resultName)) {
             ok=(dstore->results.value(evaluationName)->results.value(resultName).sortPriority!=pr);
         }
     }
     //qDebug()<<"resultsSetGroup("<<evaluationName<<", "<<resultName<<", "<<group<<"): ok="<<ok<<"   unlock read";
     lock->unlock();


    if (ok)  {

        
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        evaluationResult r;
        if (dstore->results.contains(evaluationName)) {
            QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results[evaluationName];
            if (evr->results.contains(resultName)) {
                r=evr->results[resultName];
            }
        }

        r.sortPriority=pr;
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        dstore->results[evaluationName]->results[resultName]=r;
    }
    emitResultsChanged(evaluationName, resultName, false);
}

bool QFRawDataRecord::resultsGetSortPriority(const QString& evaluationName, const QString& resultName) const {
    const evaluationResult r=resultsGet(evaluationName, resultName);
    return r.sortPriority;
}

void QFRawDataRecord::resultsSetEvaluationGroupIndex(const QString& evaluationName, int64_t groupIndex) {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
    dstore->results[evaluationName]->groupIndex=groupIndex;
}

int64_t QFRawDataRecord::resultsGetEvaluationGroupIndex(const QString& evaluationName) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if (dstore->results.contains(evaluationName)) {
        return dstore->results[evaluationName]->groupIndex;
    }
    return -1;
}

void QFRawDataRecord::resultsSetEvaluationGroupLabel(const QString& evalGroup, const QString& label) {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    dstore->evalGroupLabels[evalGroup]=label;
}

QString QFRawDataRecord::resultsGetLabelForEvaluationGroup(const QString& evalGroup) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if (dstore->evalGroupLabels.contains(evalGroup)) {
        return dstore->evalGroupLabels[evalGroup];
    } else {
        return evalGroup;
    }
}

void QFRawDataRecord::resultsSetEvaluationGroup(const QString& evaluationName, const QString& group) {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
    dstore->results[evaluationName]->group=group;
}

QString QFRawDataRecord::resultsGetEvaluationGroup(const QString& evaluationName) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if (dstore->results.contains(evaluationName)) {
        return dstore->results[evaluationName]->group;
    }
    return "";
}

QString QFRawDataRecord::resultsGetEvaluationGroupLabel(const QString& evaluationName) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if (dstore->results.contains(evaluationName)) {
        QString g=dstore->results[evaluationName]->group;
        #ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"unlock";
#endif
 locker.unlock();
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  unlocked";
#endif
        return resultsGetLabelForEvaluationGroup(g);
    }
    return "";
}

void QFRawDataRecord::resultsSetEvaluationDescription(const QString& evaluationName, const QString& description) {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
    dstore->results[evaluationName]->description=description;
}

QString QFRawDataRecord::resultsGetEvaluationDescription(const QString& evaluationName) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if (dstore->results.contains(evaluationName)) {
        QString g=dstore->results[evaluationName]->description;
        if (g.isEmpty()) return evaluationName;
        else return g;
    }
    return "";
}

void QFRawDataRecord::resultsSetBoolean(const QString& evaluationName, const QString& resultName, bool value) {
    { 
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        evaluationResult r;
        r.type=qfrdreBoolean;
        r.bvalue=value;
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        dstore->results[evaluationName]->results.insert(resultName, r);
    }
    emitResultsChanged(evaluationName, resultName, false);
};

void QFRawDataRecord::resultsSet(const QString& evaluationName, const QString& resultName, const evaluationResult& result) {
    { 
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        dstore->results[evaluationName]->results.insert(resultName, result);
    }
    emitResultsChanged(evaluationName, resultName, false);
}

QFRawDataRecord::evaluationResult QFRawDataRecord::resultsGet(const QString& evalName, const QString& resultName) const
{
    #ifdef DEBUG_THREAN
    qDebug()<<Q_FUNC_INFO<<"QReadLocker";
    #endif
     QReadLocker locker(lock);
    #ifdef DEBUG_THREAN
     qDebug()<<Q_FUNC_INFO<<"  locked";
    #endif
    evaluationResult r;
    if (dstore->results.contains(evalName)) {
        if (dstore->results[evalName]->results.contains(resultName)) {
            r=dstore->results[evalName]->results[resultName];
        }
    }
    //return false;

/*    if (resultsExists(evalName, resultName)) {
        #ifdef DEBUG_THREAN
        qDebug()<<Q_FUNC_INFO<<"QReadLocker";
        #endif
         QReadLocker locker(lock);
        #ifdef DEBUG_THREAN
         qDebug()<<Q_FUNC_INFO<<"  locked";
        #endif
        return dstore->results[evalName]->results.value(resultName);
    }*/
    return r;
}

qfmpResult QFRawDataRecord::resultsGetForMathParser(const QString &evalName, const QString &resultName) const
{
    return resultsGet(evalName, resultName).getAsMathParserResult();
}

void QFRawDataRecord::resultsSetFromMathParser(const QString &evaluationName, const QString &resultName, const qfmpResult &result)
{
    {
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        evaluationResult r;
        r.setFromMathParserResult(result);
        if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
        dstore->results[evaluationName]->results.insert(resultName, r);
    }
    emitResultsChanged(evaluationName, resultName, false);
}

QFRawDataRecord::evaluationResultType QFRawDataRecord::resultsGetType(const QString& evalName, const QString& resultName) const {
    if (resultsExists(evalName, resultName)) {
        
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
        return dstore->results[evalName]->results.value(resultName).type;
    }
    return qfrdreInvalid;
}

QVariant QFRawDataRecord::resultsGetAsQVariant(const QString& evalName, const QString& resultName) const {
    QVariant result;
    const evaluationResult r=resultsGet(evalName, resultName);
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
        case qfrdreStringMatrix:
        case qfrdreStringVector: {
            QList<QVariant> data;
            for (int i=0; i<r.svec.size(); i++) {
                data.append(r.svec[i]);
            }
            result=data;
            break;
        }
        case qfrdreNumberErrorMatrix:
        case qfrdreNumberErrorVector: {
            QList<QVariant> data;
            for (int i=0; i<qMin(r.dvec.size(), r.evec.size()); i++) {
                data.append(QPointF(r.dvec[i], r.evec[i]));
            }
            result=data;
            break;
        }
        case qfrdreIntegerMatrix:
        case qfrdreIntegerVector: {
            QList<QVariant> data;
            for (int i=0; i<r.ivec.size(); i++) {
                data.append(r.ivec[i]);
            }
            result=data;
            break;
        }
        case qfrdreBooleanMatrix:
        case qfrdreBooleanVector: {
            QList<QVariant> data;
            for (int i=0; i<r.bvec.size(); i++) {
                data.append(r.bvec[i]);
            }
            result=data;
            break;
        }
        case qfrdreString: result=r.svalue; break;
        default: result=QVariant(); break;
    }
    return result;
}

QString QFRawDataRecord::resultsGetAsString(const QString& evalName, const QString& resultName) const {
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
        case qfrdreStringVector: {
            QString s="(";
            for (int i=0; i<r.svec.size(); i++) {
                if (i>0) s=s+"; ";
                s=s+"\""+r.svec[i]+"\"";
            }
            return s+") "+r.unit;
        }
        case qfrdreStringMatrix: {
            QString s="(";
            for (int i=0; i<r.svec.size(); i++) {
                if (i>0) {
                    if (i%r.columns==0) s=s+";; ";
                    else s=s+"; ";
                }
                s=s+"\""+r.svec[i]+"\"";
            }
            return s+") "+r.unit;
        }
        case qfrdreNumberErrorVector: {
            QString s="[";
            for (int i=0; i<qMin(r.dvec.size(), r.evec.size()); i++) {
                if (i>0) s=s+"; ";
                s=s+QString("(%1 +/- %2)").arg(r.dvec[i]).arg(r.evec[i]);
            }
            return s+"] "+r.unit;
        }
        case qfrdreNumberErrorMatrix: {
            QString s="[";
            for (int i=0; i<qMin(r.dvec.size(), r.evec.size()); i++) {
                if (i>0) {
                    if (i%r.columns==0) s=s+";; ";
                    else s=s+"; ";
                }
                s=s+QString("(%1 +/- %2)").arg(r.dvec[i]).arg(r.evec[i]);
            }
            return s+"] "+r.unit;
        }
        case qfrdreIntegerVector: {
            QString s="(";
            for (int i=0; i<r.ivec.size(); i++) {
                if (i>0) s=s+"; ";
                s=s+QString::number(r.ivec[i]);
            }
            return s+") "+r.unit;
        }
        case qfrdreIntegerMatrix: {
            QString s="(";
            for (int i=0; i<r.ivec.size(); i++) {
                if (i>0) {
                    if (i%r.columns==0) s=s+";; ";
                    else s=s+"; ";
                }
                s=s+QString::number(r.ivec[i]);
            }
            return s+") "+r.unit;
        }
        case qfrdreBooleanVector: {
            QString s="(";
            for (int i=0; i<r.bvec.size(); i++) {
                if (i>0) s=s+"; ";
                s=s+((r.bvec[i])?tr("t"):tr("f"));
            }
            return s+") "+r.unit;
        }
        case qfrdreBooleanMatrix: {
            QString s="(";
            for (int i=0; i<r.bvec.size(); i++) {
                if (i>0) {
                    if (i%r.columns==0) s=s+";; ";
                    else s=s+"; ";
                }
                s=s+((r.bvec[i])?tr("t"):tr("f"));
            }
            return s+") "+r.unit;
        }
        case qfrdreNumberError: return tr("(%1 +/- %2) %3").arg(r.dvalue).arg(r.derror).arg(r.unit);
        case qfrdreString: return r.svalue;
        default: return ("");
    }
    return QString("");
}

QVariant QFRawDataRecord::resultsGetAsStringVariant(const QString& evalName, const QString& resultName) const {
    return QVariant(resultsGetAsString(evalName, resultName));
}

double QFRawDataRecord::resultsGetAsDouble(const QString& evalName, const QString& resultName, bool* ok) const {
    const evaluationResult r=resultsGet(evalName, resultName);
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

int64_t QFRawDataRecord::resultsGetAsInteger(const QString& evalName, const QString& resultName, bool* ok) const {
    const evaluationResult r=resultsGet(evalName, resultName);
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

bool QFRawDataRecord::resultsGetAsBoolean(const QString& evalName, const QString& resultName, bool* ok) const {
    const evaluationResult r=resultsGet(evalName, resultName);
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

QVector<double> QFRawDataRecord::resultsGetAsDoubleList(const QString& evalName, const QString& resultName, bool* ok) const {
    const evaluationResult r=resultsGet(evalName, resultName);
    if (ok) *ok=true;
    switch(r.type) {
        case qfrdreNumber:
        case qfrdreNumberError: {
                QVector<double> v;
                v.append(r.dvalue);
                return v;
            }
            break;
        case qfrdreInteger: {
                QVector<double> v;
                v.append(r.ivalue);
                return v;
            }
            break;
        case qfrdreBoolean: {
                QVector<double> v;
                v.append(r.bvalue?1:0);
                return v;
            }
            break;

        case qfrdreNumberVector:
        case qfrdreNumberMatrix:
        case qfrdreNumberErrorVector:
        case qfrdreNumberErrorMatrix:
            return r.dvec;
        case qfrdreIntegerVector:
        case qfrdreIntegerMatrix: {
                QVector<double> result;
                result.resize(r.ivec.size());
                for (int i=0; i<r.ivec.size(); i++) {
                    result[i]=r.ivec[i];
                }
            }
            break;
        case qfrdreBooleanVector:
        case qfrdreBooleanMatrix: {
                QVector<double> result;
                result.resize(r.bvec.size());
                for (int i=0; i<r.bvec.size(); i++) {
                    result[i]=r.bvec[i];
                }
            }
            break;

        default: if (ok) *ok=false;
                 return QVector<double>();
    }
    if (ok) *ok=false;
    return QVector<double>();
}

QVector<qlonglong> QFRawDataRecord::resultsGetAsIntegerList(const QString& evalName, const QString& resultName, bool* ok) const {
    const evaluationResult r=resultsGet(evalName, resultName);
    if (ok) *ok=true;
    switch(r.type) {
        case qfrdreInteger: {
                QVector<qlonglong> v;
                v.append(r.ivalue);
                return v;
            }
            break;
        case qfrdreBoolean: {
                QVector<qlonglong> v;
                v.append(r.bvalue?1:0);
                return v;
            }
            break;
        case qfrdreIntegerVector:
        case qfrdreIntegerMatrix:
            return r.ivec;
        case qfrdreBooleanVector:
        case qfrdreBooleanMatrix: {
                QVector<qlonglong> result;
                result.resize(r.bvec.size());
                for (int i=0; i<r.bvec.size(); i++) {
                    result[i]=r.bvec[i];
                }
            }
            break;
        default: if (ok) *ok=false;
                 return QVector<qlonglong>();
    }
    if (ok) *ok=false;
    return QVector<qlonglong>();
}

QVector<bool> QFRawDataRecord::resultsGetAsBooleanList(const QString& evalName, const QString& resultName, bool* ok) const {
    const evaluationResult r=resultsGet(evalName, resultName);
    if (ok) *ok=true;
    switch(r.type) {
        case qfrdreBooleanVector:
        case qfrdreBooleanMatrix: return r.bvec;
        default: if (ok) *ok=false;
                 return QVector<bool>();
    }
    if (ok) *ok=false;
    return QVector<bool>();
}

QStringList QFRawDataRecord::resultsGetAsStringList(const QString &evalName, const QString &resultName, bool* ok) const {
    const evaluationResult r=resultsGet(evalName, resultName);
    if (ok) *ok=true;
    switch(r.type) {
        case qfrdreStringVector:
        case qfrdreStringMatrix: return r.svec;
        default: if (ok) *ok=false;
                 return QStringList();
    }
    if (ok) *ok=false;
    return QStringList();
}

QString QFRawDataRecord::resultsGetAsString(const QString &evalName, const QString &resultName, int position, bool alsoGetNonVec) const {
    const evaluationResult r=resultsGet(evalName, resultName);
    switch(r.type) {
        case qfrdreNumberMatrix:
        case qfrdreNumberVector: {
            if (position>= r.dvec.size()) return "0 "+r.unit;
            return QString::number(r.dvec[position])+" "+r.unit;
        }
        case qfrdreNumber: {
            if (alsoGetNonVec) return QString::number(r.dvalue)+" "+r.unit;
            else return "";
        }
        case qfrdreNumberErrorMatrix:
        case qfrdreNumberErrorVector: {
            if (position>= r.dvec.size()) return "(0 +/- 0) "+r.unit;
            if (position>= r.evec.size()) return QString("(%1 +/- 0)").arg(r.dvec[position])+" "+r.unit;
            else return QString("(%1 +/- %2)").arg(r.dvec[position]).arg(r.evec[position])+" "+r.unit;
        }

        case qfrdreNumberError: {
            if (alsoGetNonVec) return "("+QString::number(r.dvalue)+" +/- "+QString::number(r.derror)+") "+r.unit;
            else return "";
        }
        case qfrdreStringVector:
        case qfrdreStringMatrix: {
                if (position>= r.svec.size()) return r.unit;
                return r.svec[position]+" "+r.unit;
        }
        case qfrdreString: {
            if (alsoGetNonVec) return r.svalue;
            else return "";
        }


        case qfrdreIntegerVector:
        case qfrdreIntegerMatrix: {
                if (position>= r.ivec.size()) return "0 "+r.unit;
                return QString::number(r.ivec[position])+" "+r.unit;
        }
        case qfrdreInteger: {
            if (alsoGetNonVec) return QString::number(r.ivalue)+" "+r.unit;
            else return "";
        }
        case qfrdreBooleanVector:
        case qfrdreBooleanMatrix: {
                if (position>= r.bvec.size()) return "false";
                if (r.bvec[position]) return "true";
                else return "false";
        }
        case qfrdreBoolean: {
            if (alsoGetNonVec) {
                if (r.bvalue) return "true";
                else return "false";
            }
            else return "";
        }
        default: return ("");
    }
    return QString("");
}

QVariant QFRawDataRecord::resultsGetAsStringVariant(const QString &evalName, const QString &resultName, int position, bool alsoGetNonVec) const {
    return QVariant(resultsGetAsString(evalName, resultName, position));
}

QVariant QFRawDataRecord::resultsGetAsQVariant(const QString &evalName, const QString &resultName, int position, bool alsoGetNonVec) const {
    const evaluationResult r=resultsGet(evalName, resultName);
    switch(r.type) {
        case qfrdreInteger:
            if (alsoGetNonVec) return QVariant(r.ivalue);
            else return QVariant();
        case qfrdreNumber:
        case qfrdreNumberError:
            if (alsoGetNonVec) return QVariant(r.dvalue);
            else return QVariant();
        case qfrdreBoolean:
            if (alsoGetNonVec) return QVariant(r.bvalue);
            else return QVariant();
        case qfrdreString:
            if (alsoGetNonVec) return QVariant(r.svalue);
            else return QVariant();

        case qfrdreNumberMatrix:
        case qfrdreNumberVector: {
            if (position>= r.dvec.size()) return QVariant();
            return QVariant(r.dvec[position]);
        }
        case qfrdreNumberErrorMatrix:
        case qfrdreNumberErrorVector: {
            if (position>= r.dvec.size()) return QVariant();
            if (position>= r.evec.size()) return QVariant(QPointF(r.dvec[position], 0.0));
            else return QVariant(QPointF(r.dvec[position], r.evec[position]));
        }

        case qfrdreStringVector:
        case qfrdreStringMatrix: {
                if (position>= r.svec.size()) return QVariant();
                return QVariant(r.svec[position]);
        }

        case qfrdreIntegerVector:
        case qfrdreIntegerMatrix: {
                if (position>= r.ivec.size()) return QVariant();
                return QVariant(r.ivec[position]);
        }
        case qfrdreBooleanVector:
        case qfrdreBooleanMatrix: {
                if (position>= r.bvec.size()) return QVariant();
                return QVariant(r.bvec[position]);
        }
        default: return QVariant();
    }
    return QVariant();
}

double QFRawDataRecord::resultsGetAsDouble(const QString &evalName, const QString &resultName, int position, bool *ok, bool alsoGetNonVec) const {
    const evaluationResult r=resultsGet(evalName, resultName);
    switch(r.type) {
        case qfrdreInteger:
            if (alsoGetNonVec) return r.ivalue;
            else return 0.0;
        case qfrdreNumber:
            if (alsoGetNonVec) return r.dvalue;
            else return 0.0;
        case qfrdreBoolean:
            if (alsoGetNonVec) return r.bvalue?1:0;
            else return 0.0;

        case qfrdreNumberMatrix:
        case qfrdreNumberVector:
        case qfrdreNumberErrorMatrix:
        case qfrdreNumberErrorVector: {
            if (position>= r.dvec.size()) return 0.0;
            return r.dvec[position];
        }

        case qfrdreIntegerVector:
        case qfrdreIntegerMatrix: {
                if (position>= r.ivec.size()) return 0.0;
                return r.ivec[position];
        }
        case qfrdreBooleanVector:
        case qfrdreBooleanMatrix: {
                if (position>= r.bvec.size()) return 0.0;
                return (r.bvec[position])?1.0:0.0;
        }
        default: return 0.0;
    }
    return 0.0;
}

double QFRawDataRecord::resultsGetErrorAsDouble(const QString &evalName, const QString &resultName, int position, bool *ok, bool alsoGetNonVec) const {
    const evaluationResult r=resultsGet(evalName, resultName);
    switch(r.type) {
        case qfrdreNumberErrorMatrix:
        case qfrdreNumberErrorVector: {
            if (position>= r.evec.size()) return 0.0;
            return r.evec[position];
        }
        case qfrdreNumberError:
            if (alsoGetNonVec) return r.derror;
            else return 0;

        default: return 0.0;
    }
    return 0.0;
}

bool QFRawDataRecord::resultsGetAsBoolean(const QString &evalName, const QString &resultName, int position, bool *ok, bool alsoGetNonVec) const {
    const evaluationResult r=resultsGet(evalName, resultName);
    switch(r.type) {
        case qfrdreBooleanVector:
        case qfrdreBooleanMatrix: {
                if (position>= r.bvec.size()) return false;
                return r.bvec[position];
        }
        case qfrdreBoolean:
            if (alsoGetNonVec) return r.bvalue;
            else return 0;
        default: return false;
    }
    return false;
}

int64_t QFRawDataRecord::resultsGetAsInteger(const QString &evalName, const QString &resultName, int position, bool *ok, bool alsoGetNonVec) const {
    const evaluationResult r=resultsGet(evalName, resultName);
    switch(r.type) {
        case qfrdreInteger:
            if (alsoGetNonVec) return r.ivalue;
            else return 0;
        case qfrdreBoolean:
            if (alsoGetNonVec) return r.bvalue?1:0;
            else return 0;
        case qfrdreIntegerVector:
        case qfrdreIntegerMatrix: {
                if (position>= r.ivec.size()) return 0;
                return r.ivec[position];
        }
        case qfrdreBooleanVector:
        case qfrdreBooleanMatrix: {
                if (position>= r.bvec.size()) return 0;
                return (r.bvec[position])?1:0;
        }
        default: return 0;
    }
    return 0;
}

double QFRawDataRecord::resultsGetErrorAsDouble(const QString& evalName, const QString& resultName, bool* ok) const {
    const evaluationResult r=resultsGet(evalName, resultName);
    if (ok) *ok=true;
    switch(r.type) {
        case qfrdreNumber: case qfrdreNumberError: return r.derror;
        default: if (ok) *ok=false;
                 return 0.0;
    }
    if (ok) *ok=false;
    return 0.0;
}

QVector<double> QFRawDataRecord::resultsGetErrorAsDoubleList(const QString &evalName, const QString &resultName, bool *ok) const {
    const evaluationResult r=resultsGet(evalName, resultName);
    if (ok) *ok=true;
    switch(r.type) {
        case qfrdreNumberErrorVector:
        case qfrdreNumberErrorMatrix: {
                if (r.evec.size()<r.dvec.size()) return QVector<double>(r.dvec.size(), 0.0);
                return r.evec;
            }
        default: if (ok) *ok=false;
                 return QVector<double>(r.dvec.size(), 0.0);
    }
    if (ok) *ok=false;
    return QVector<double>(r.dvec.size(), 0.0);
}



QString QFRawDataRecord::resultsGetLabel(const QString& evaluationName, const QString& resultName) const {
    const evaluationResult r=resultsGet(evaluationName, resultName);
    if (r.label.isEmpty()) return resultName;
    else return r.label;
}


QString QFRawDataRecord::resultsGetGroup(const QString& evaluationName, const QString& resultName) const {
    const evaluationResult r=resultsGet(evaluationName, resultName);
    return r.group;
}

void QFRawDataRecord::resultsSetGroupAndLabels(const QString &evaluationName, const QString &resultName, const QString &group, const QString &label, const QString &label_rich)
{
    {
        #ifdef DEBUG_THREAN
        qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
        #endif
         QWriteLocker locker(lock);
        #ifdef DEBUG_THREAN
         qDebug()<<Q_FUNC_INFO<<"  locked";
        #endif
         evaluationResult r;
         if (dstore->results.contains(evaluationName)) {
             QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results[evaluationName];
             if (evr->results.contains(resultName)) {
                 r=evr->results[resultName];
             }
         }
         r.label=label;
         r.label_rich=label_rich;
         r.group=group;
         if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
         dstore->results[evaluationName]->results[resultName]=r;
    }
    emitResultsChanged(evaluationName, resultName, false);
}

void QFRawDataRecord::resultsSetGroupLabelsAndSortPriority(const QString &evaluationName, const QString &resultName, const QString &group, const QString &label, const QString &label_rich, bool pr)
{
    {
        #ifdef DEBUG_THREAN
        qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
        #endif
         QWriteLocker locker(lock);
        #ifdef DEBUG_THREAN
         qDebug()<<Q_FUNC_INFO<<"  locked";
        #endif
         evaluationResult r;
         if (dstore->results.contains(evaluationName)) {
             QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results[evaluationName];
             if (evr->results.contains(resultName)) {
                 r=evr->results[resultName];
             }
         }
         r.label=label;
         r.label_rich=label_rich;
         r.group=group;
         r.sortPriority=pr;
         if (!dstore->results.contains(evaluationName)) dstore->results[evaluationName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
         dstore->results[evaluationName]->results[resultName]=r;
    }
    emitResultsChanged(evaluationName, resultName, false);
}

QString QFRawDataRecord::resultsGetLabelRichtext(const QString& evaluationName, const QString& resultName) const {
    const evaluationResult r=resultsGet(evaluationName, resultName);
    if (r.label_rich.isEmpty()) return r.label;
    else if (r.label_rich.isEmpty()) return resultName;
    else return r.label_rich;
}


QString QFRawDataRecord::resultsGetResultName(const QString& evaluationName, int i) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if (dstore->results.contains(evaluationName)) {
        QList<QString> r=dstore->results[evaluationName]->results.keys();
        if (i<r.size()) {
            return r.at(i);
        }
    }
    return QString("");
}

void QFRawDataRecord::resultsCopy(const QString& oldEvalName, const QString& newEvalName)  {
    if (resultsExistsFromEvaluation(oldEvalName)) {
        {     
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QWriteLocker";
#endif
 QWriteLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif

            QFRawDataRecordPrivate::ResultsResultsIterator i(dstore->results[oldEvalName]->results);
            while (i.hasNext()) {
                i.next();
                if (!dstore->results.contains(newEvalName)) dstore->results[newEvalName] = new QFRawDataRecordPrivate::evaluationIDMetadata(evaluationIDMetadataInitSize);
                dstore->results[newEvalName]->results.insert(i.key(), i.value());
            }
        }
        emitResultsChanged(newEvalName, "", false);
    }
}

bool QFRawDataRecord::resultsSaveToCSV(const QString& filename, bool vectorsToAvg, const QString& separator, QChar decimalPoint, QChar stringDelimiter) const {
    /*QString sdel=stringDelimiter;
    QString dp=decimalPoint;
    QList<QPair<QString,QString> > rownames=resultsCalcNamesAndLabels();
    QStringList header, data;
    header.append(sdel+tr("datafield")+sdel);
    header.append("");
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    for (int i=0; i<rownames.size(); i++) data.append(sdel+rownames[i].first+sdel);

    QMap<int, int> colCount;
    for (int c=0; c<resultsGetEvaluationCount(); c++) {
        QString evalname=resultsGetEvaluationName(c);
        colCount[c]=1;
        if (!vectorsToAvg) {
            for (int r=0; r<rownames.size(); r++) {
                QString dat="";
                if (resultsExists(evalname, rownames[r].second)) {
                    QFRawDataRecord::evaluationResult res=resultsGet(evalname, rownames[r].second);
                    switch(res.type) {
                        case qfrdreInteger:
                        case qfrdreBoolean:
                        case qfrdreString:
                        case qfrdreNumber:  break;
                        case qfrdreNumberError:  break;
                        case qfrdreStringVector:
                        case qfrdreStringMatrix: colCount[c]=qMax(colCount[c], res.svec.size()); break;
                        case qfrdreNumberVector:
                        case qfrdreNumberMatrix:
                        case qfrdreNumberErrorVector:
                        case qfrdreNumberErrorMatrix: colCount[c]=qMax(colCount[c], res.dvec.size()); break;
                        case qfrdreIntegerVector:
                        case qfrdreIntegerMatrix: colCount[c]=qMax(colCount[c], res.ivec.size()); break;
                        case qfrdreBooleanVector:
                        case qfrdreBooleanMatrix:  colCount[c]=qMax(colCount[c], res.bvec.size()); break;
                        default: break;
                    }
                }
            }
        }
    }






    for (int c=0; c<resultsGetEvaluationCount(); c++) {
        QString evalname=resultsGetEvaluationName(c);
        header[0] += separator+sdel+evalname+sdel;
        header[1] += separator+sdel+tr("value")+sdel;
        for (int i=1; i<colCount[c]; i++) {
            header[0]+=separator;
            header[1]+=separator;
        }
        bool hasError=false;
        for (int r=0; r<rownames.size(); r++) {
            QStringList dat;
            if (resultsExists(evalname, rownames[r].second)) {
                switch(resultsGet(evalname, rownames[r].second).type) {
                    case qfrdreNumber: dat<<doubleToQString(resultsGetAsDouble(evalname, rownames[r].second), 15, 'g', decimalPoint); break;
                    case qfrdreNumberError: dat<<doubleToQString(resultsGetAsDouble(evalname, rownames[r].second), 15, 'g', decimalPoint); hasError=true; break;
                    case qfrdreInteger: dat<<loc.toString((qlonglong)resultsGetAsInteger(evalname, rownames[r].second)); break;
                    case qfrdreBoolean: if (resultsGetAsBoolean(evalname, rownames[r].second)) dat<<QString("1"); else dat<<QString("0"); break;
                    case qfrdreString: dat<<(stringDelimiter+resultsGetAsString(evalname, rownames[r].second).replace(stringDelimiter, "\\"+QString(stringDelimiter)).replace('\n', "\\n").replace('\r', "\\r")+stringDelimiter); break;
                    case qfrdreStringVector:
                    case qfrdreStringMatrix: {
                            QStringList sl=resultsGetAsStringList(evalname, rownames[r].second);
                            if (!vectorsToAvg) {
                                for (int i=0; i<sl.size(); i++) {
                                    dat<<(stringDelimiter+sl[i].replace(stringDelimiter, "\\"+QString(stringDelimiter)).replace('\n', "\\n").replace('\r', "\\r")+stringDelimiter);
                                }
                            }
                        } break;
                    case qfrdreNumberVector:
                    case qfrdreNumberMatrix:
                    case qfrdreNumberErrorVector:
                    case qfrdreNumberErrorMatrix: {
                            QVector<double> sl=resultsGetAsDoubleList(evalname, rownames[r].second);
                            if (!vectorsToAvg) {
                                for (int i=0; i<sl.size(); i++) {
                                    dat<<doubleToQString(sl[i], 15, 'g', decimalPoint);
                                }
                            } else {
                                dat<<doubleToQString(qfstatisticsAverage(sl), 15, 'g', decimalPoint);
                            }
                        }break;
                    case qfrdreIntegerVector:
                    case qfrdreIntegerMatrix:
                    case qfrdreBooleanVector:
                    case qfrdreBooleanMatrix:  {
                            QVector<qlonglong> sl=resultsGetAsIntegerList(evalname, rownames[r].second);
                            if (!vectorsToAvg) {
                                for (int i=0; i<sl.size(); i++) {
                                    dat<<loc.toString(sl[i]);
                                }
                            } else {
                                dat<<doubleToQString(qfstatisticsAverage(sl), 15, 'g', decimalPoint);
                            }
                        } break;
                    default: break;
                }
            }
            for (int i=0; i<dat.size(); i++) {
                data[r]+=separator+dat[i];
            }
            for (int i=dat.size(); i<colCount[c]; i++) {
                data[r]+=separator;
            }
        }
        if (hasError) {
            header[0] += separator;
            header[1] += separator+sdel+tr("error")+sdel;
            for (int i=1; i<colCount[c]; i++) {
                header[0]+=separator;
                header[1]+=separator;
            }
            for (int r=0; r<rownames.size(); r++) {
                QStringList dat;
                if (resultsExists(evalname, rownames[r].second)) {
                    switch(resultsGet(evalname, rownames[r].second).type) {
                        case qfrdreNumberError: dat<<doubleToQString(resultsGetErrorAsDouble(evalname, rownames[r].second), 15, 'g', decimalPoint); hasError=true; break;
                        case qfrdreNumberErrorVector:
                        case qfrdreNumberErrorMatrix: {
                                QVector<double> sl=resultsGetErrorAsDoubleList(evalname, rownames[r].second);
                                if (!vectorsToAvg) {
                                    for (int i=0; i<sl.size(); i++) {
                                        dat<<doubleToQString(sl[i], 15, 'g', decimalPoint);
                                    }
                                } else {
                                    dat<<doubleToQString(qfstatisticsAverage(sl), 15, 'g', decimalPoint);
                                }
                            } break;
                        default: break;
                    }
                }
                for (int i=0; i<dat.size(); i++) {
                    data[r]+=separator+dat[i];
                }
                for (int i=dat.size(); i<colCount[c]; i++) {
                    data[r]+=separator;
                }
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
     return true;*/
    QStringList colname, rownames;
    QList<QList<QVariant> > data=resultsGetTable(vectorsToAvg, &colname, &rownames);
    //QFDataExportHandler::save(data, colname, rownames, )
    QString dat=toCSV(data, colname, rownames, decimalPoint, separator, true, stringDelimiter);
    QFile of(filename);
    if (of.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&of);
        QTextCodec* c=QTextCodec::codecForName("ISO-8859-1");
        if (c==NULL) c=QTextCodec::codecForCStrings();
        if (c==NULL) c=QTextCodec::codecForLocale();
        out.setCodec(c);
        out<<dat;
        of.close();
        return true;
    } else {
        return false;
    }

}

QList<QList<QVariant> > QFRawDataRecord::resultsGetTable(bool vectorsToAvg, QStringList *colNames, QStringList *rowNames) const
{
    QList<QPair<QString,QString> > rnames=resultsCalcNamesAndLabels();
    QList<QList<QVariant> > data; // initially a list of rows of data
    QList<QVariant> dempty; // empty column of data
    QStringList colnames, rownames;

    //data.append(dempty);
    rownames.append("");
    dempty.append(QVariant());
    for (int i=0; i<rnames.size(); i++) {
        rownames.append(rnames[i].first);
        //data.append(dempty);
        dempty.append(QVariant());
    }

    QMap<int, int> colCount;
    for (int c=0; c<resultsGetEvaluationCount(); c++) {
        QString evalname=resultsGetEvaluationName(c);
        colCount[c]=1;
        if (!vectorsToAvg) {
            for (int r=0; r<rnames.size(); r++) {
                if (resultsExists(evalname, rnames[r].second)) {
                    QFRawDataRecord::evaluationResult res=resultsGet(evalname, rnames[r].second);
                    switch(res.type) {
                        case qfrdreInteger:
                        case qfrdreBoolean:
                        case qfrdreString:
                        case qfrdreNumber:  break;
                        case qfrdreNumberError:  break;
                        case qfrdreStringVector:
                        case qfrdreStringMatrix: colCount[c]=qMax(colCount[c], res.svec.size()); break;
                        case qfrdreNumberVector:
                        case qfrdreNumberMatrix:
                        case qfrdreNumberErrorVector:
                        case qfrdreNumberErrorMatrix: colCount[c]=qMax(colCount[c], res.dvec.size()); break;
                        case qfrdreIntegerVector:
                        case qfrdreIntegerMatrix: colCount[c]=qMax(colCount[c], res.ivec.size()); break;
                        case qfrdreBooleanVector:
                        case qfrdreBooleanMatrix:  colCount[c]=qMax(colCount[c], res.bvec.size()); break;
                        default: break;
                    }
                }
            }
        }
    }





    int col=0;
    for (int c=0; c<resultsGetEvaluationCount(); c++) {
        QString evalname=resultsGetEvaluationName(c);
        colnames.append(evalname);
        data.append(dempty);
        data.last().operator[](0)=QString("value");
        //colnames[1] += separator+sdel+tr("value")+sdel;
        for (int i=1; i<colCount[c]; i++) {
            colnames.append("");
            data.append(dempty);
        }
        bool hasError=false;
        for (int r=0; r<rnames.size(); r++) {
            if (resultsExists(evalname, rnames[r].second)) {
                switch(resultsGet(evalname, rnames[r].second).type) {
                    case qfrdreNumber: data[col].operator[](r+1)=resultsGetAsDouble(evalname, rnames[r].second); break;
                    case qfrdreNumberError: data[col].operator[](r+1)=resultsGetAsDouble(evalname, rnames[r].second); hasError=true; break;
                    case qfrdreInteger: data[col].operator[](r+1)=(qlonglong)resultsGetAsInteger(evalname, rnames[r].second); break;
                    case qfrdreBoolean: data[col].operator[](r+1)=resultsGetAsBoolean(evalname, rnames[r].second); break;
                    case qfrdreString: data[col].operator[](r+1)=resultsGetAsString(evalname, rnames[r].second); break;
                    case qfrdreStringVector:
                    case qfrdreStringMatrix: {
                            QStringList sl=resultsGetAsStringList(evalname, rnames[r].second);
                            if (!vectorsToAvg) {
                                for (int i=0; i<sl.size(); i++) {
                                    data[col+i].operator[](r+1)=sl[i];
                                }
                            } else {
                                data[col].operator[](r+1)=QVariant();
                            }
                        } break;
                    case qfrdreNumberVector:
                    case qfrdreNumberMatrix:
                    case qfrdreNumberErrorVector:
                    case qfrdreNumberErrorMatrix: {
                            QVector<double> sl=resultsGetAsDoubleList(evalname, rnames[r].second);
                            if (!vectorsToAvg) {
                                for (int i=0; i<sl.size(); i++) {
                                    data[col+i].operator[](r+1)=sl[i];
                                }
                            } else {
                                data[col].operator[](r+1)=qfstatisticsAverage(sl);
                                hasError=true;
                            }
                        }break;
                    case qfrdreIntegerVector:
                    case qfrdreIntegerMatrix:
                    case qfrdreBooleanVector:
                    case qfrdreBooleanMatrix:  {
                            QVector<qlonglong> sl=resultsGetAsIntegerList(evalname, rnames[r].second);
                            if (!vectorsToAvg) {
                                for (int i=0; i<sl.size(); i++) {
                                    data[col+i].operator[](r+1)=sl[i];
                                }
                            } else {
                                data[col].operator[](r+1)=qfstatisticsAverage(sl);
                                hasError=true;
                            }
                        } break;
                    default:  break;
                }
            }

        }
        col=col+qMax(colCount[c],1);
        if (hasError) {
            colnames.append(evalname);
            data.append(dempty);
            data.last().operator [](0)=QString("error");
            for (int i=1; i<colCount[c]; i++) {
                colnames.append("");
                data.append(dempty);
            }
            for (int r=0; r<rnames.size(); r++) {

                if (resultsExists(evalname, rnames[r].second)) {
                    switch(resultsGet(evalname, rnames[r].second).type) {
                        case qfrdreNumberError: data[col].operator[](r+1)=resultsGetErrorAsDouble(evalname, rnames[r].second); break;
                        case qfrdreNumberErrorVector:
                        case qfrdreNumberErrorMatrix: {
                                if (!vectorsToAvg) {
                                    QVector<double> sl=resultsGetErrorAsDoubleList(evalname, rnames[r].second);
                                    for (int i=0; i<sl.size(); i++) {
                                        data[col+i].operator[](r+1)=sl[i];
                                    }
                                } else {
                                    QVector<double> sl=resultsGetAsDoubleList(evalname, rnames[r].second);
                                    data[col].operator[](r+1)=qfstatisticsStd(sl);
                                }
                            } break;
                        default: break;
                        case qfrdreNumberVector:
                        case qfrdreNumberMatrix:  {
                                QVector<double> sl=resultsGetAsDoubleList(evalname, rnames[r].second);
                                if (vectorsToAvg) {
                                    data[col].operator[](r+1)=qfstatisticsStd(sl);
                                    hasError=true;
                                }
                            }break;
                        case qfrdreIntegerVector:
                        case qfrdreIntegerMatrix:
                        case qfrdreBooleanVector:
                        case qfrdreBooleanMatrix:  {
                                QVector<qlonglong> sl=resultsGetAsIntegerList(evalname, rnames[r].second);
                                if (vectorsToAvg) {
                                    data[col].operator[](r+1)=qfstatisticsStd(sl);
                                    hasError=true;
                                }
                            } break;

                    }
                }
            }
            col=col+qMax(colCount[c],1);
        }
    }


     if (colNames) *colNames=colnames;
     if (rowNames) *rowNames=rownames;
     return data;

}


bool QFRawDataRecord::resultsSaveToSYLK(const QString& filename, bool vectorsToAvg, bool flipTable) const {
    QStringList colname, rownames;
    QList<QList<QVariant> > data=resultsGetTable(vectorsToAvg, &colname, &rownames);
    if (flipTable) {
        data=dataRotate(data);
        qSwap(colname, rownames);
    }
    //QFDataExportHandler::save(data, colname, rownames, )
    QString dat=toSYLK(data, colname, rownames);
    QFile of(filename);
    if (of.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&of);
        QTextCodec* c=QTextCodec::codecForName("ISO-8859-1");
        if (c==NULL) c=QTextCodec::codecForCStrings();
        if (c==NULL) c=QTextCodec::codecForLocale();
        out.setCodec(c);
        out<<dat;
        of.close();
        return true;
    } else {
        return false;
    }





    /*// try output SYLK file
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


    QMap<int, int> colCount;
    for (int c=0; c<resultsGetEvaluationCount(); c++) {
        QString evalname=resultsGetEvaluationName(c);
        colCount[c]=1;
        if (!vectorsToAvg) {
            for (int r=0; r<rownames.size(); r++) {
                if (resultsExists(evalname, rownames[r].second)) {
                    QFRawDataRecord::evaluationResult res=resultsGet(evalname, rownames[r].second);
                    switch(res.type) {
                        case qfrdreInteger:
                        case qfrdreBoolean:
                        case qfrdreString:
                        case qfrdreNumber:  break;
                        case qfrdreNumberError:  break;
                        case qfrdreStringVector:
                        case qfrdreStringMatrix: colCount[c]=qMax(colCount[c], res.svec.size()); break;
                        case qfrdreNumberVector:
                        case qfrdreNumberMatrix:
                        case qfrdreNumberErrorVector:
                        case qfrdreNumberErrorMatrix: colCount[c]=qMax(colCount[c], res.dvec.size()); break;
                        case qfrdreIntegerVector:
                        case qfrdreIntegerMatrix: colCount[c]=qMax(colCount[c], res.ivec.size()); break;
                        case qfrdreBooleanVector:
                        case qfrdreBooleanMatrix:  colCount[c]=qMax(colCount[c], res.bvec.size()); break;
                        default: break;
                    }
                }
            }
        }
    }


    // write column headers
    out<<QString("F;Y1;X1;SDS\nF;Y2;X1;SDS\nC;Y1;X1;K\"%1\"\n").arg(tr("datafield"));

    for (int r=0; r<rownames.size(); r++) {
        if (flipTable) {
            out<<QString("F;X%2;Y1;SDS\nC;X%2;Y1;K\"%1\"\n").arg(rownames[r].first).arg(r+3);
        } else {
            out<<QString("F;Y%2;X1;SDS\nC;Y%2;X1;K\"%1\"\n").arg(rownames[r].first).arg(r+3);
        }
    }

    int col=2;
    for (int c=0; c<resultsGetEvaluationCount(); c++) {
        QString evalname=resultsGetEvaluationName(c);
        if (flipTable) {
            out<<QString("F;SDS;X1;Y%2\nC;X1;Y%2;K\"%1\"\n").arg(evalname.replace(stringDelimiter, "\\"+QString(stringDelimiter)).replace('\n', "\\n").replace('\r', "\\r").replace(';', ",").replace(stringDelimiter, "_")).arg(col);
            out<<QString("F;SDIS;X2;Y%2\nC;X2;Y%2;K\"%1\"\n").arg(tr("value")).arg(col);
        } else {
            out<<QString("F;SDS;Y1;X%2\nC;Y1;X%2;K\"%1\"\n").arg(evalname.replace(stringDelimiter, "\\"+QString(stringDelimiter)).replace('\n', "\\n").replace('\r', "\\r").replace(';', ",").replace(stringDelimiter, "_")).arg(col);
            out<<QString("F;SDIS;Y2;X%2\nC;Y2;X%2;K\"%1\"\n").arg(tr("value")).arg(col);
        }
        bool hasError=false;
        int addCol=1;
        for (int r=0; r<rownames.size(); r++) {


            QStringList dat;
            if (resultsExists(evalname, rownames[r].second)) {
                switch(resultsGet(evalname, rownames[r].second).type) {
                    case qfrdreNumber: dat<<CDoubleToQString(resultsGetAsDouble(evalname, rownames[r].second)); break;
                    case qfrdreNumberError: dat<<CDoubleToQString(resultsGetAsDouble(evalname, rownames[r].second)); hasError=true; break;
                    case qfrdreInteger: dat<<loc.toString((qlonglong)resultsGetAsInteger(evalname, rownames[r].second)); break;
                    case qfrdreBoolean: if (resultsGetAsBoolean(evalname, rownames[r].second)) dat<<QString("1"); else dat<<QString("0"); break;
                    case qfrdreString: dat<<(stringDelimiter+resultsGetAsString(evalname, rownames[r].second).replace(stringDelimiter, "\\"+QString(stringDelimiter)).replace('\n', "\\n").replace('\r', "\\r").replace(';', ",").replace(stringDelimiter, "_")+stringDelimiter); break;
                    case qfrdreStringVector:
                    case qfrdreStringMatrix: {
                            QStringList sl=resultsGetAsStringList(evalname, rownames[r].second);
                            if (!vectorsToAvg) for (int i=0; i<sl.size(); i++) {
                                dat<<(stringDelimiter+sl[i].replace(stringDelimiter, "\\"+QString(stringDelimiter)).replace('\n', "\\n").replace('\r', "\\r").replace(';', ",").replace(stringDelimiter, "_")+stringDelimiter);
                            }
                        } break;
                    case qfrdreNumberVector:
                    case qfrdreNumberMatrix:
                    case qfrdreNumberErrorVector:
                    case qfrdreNumberErrorMatrix: {
                            QVector<double> sl=resultsGetAsDoubleList(evalname, rownames[r].second);
                            if (!vectorsToAvg) {
                                for (int i=0; i<sl.size(); i++) {
                                    dat<<CDoubleToQString(sl[i]);
                                }
                            } else dat<<CDoubleToQString(qfstatisticsAverage(sl));
                        } break;
                    case qfrdreIntegerVector:
                    case qfrdreIntegerMatrix:
                    case qfrdreBooleanVector:
                    case qfrdreBooleanMatrix:  {
                            QVector<qlonglong> sl=resultsGetAsIntegerList(evalname, rownames[r].second);
                            if (!vectorsToAvg) {
                                for (int i=0; i<sl.size(); i++) {
                                    dat<<loc.toString(sl[i]);
                                }
                            } else dat<<CDoubleToQString(qfstatisticsAverage(sl));
                        } break;
                    default: break;
                }
            }
            for (int i=0; i<dat.size(); i++) {
                if (flipTable) {
                    out<<QString("C;X%2;Y%1;K%3\n").arg(col+i).arg(r+3).arg(dat[i]);
                } else {
                    out<<QString("C;X%1;Y%2;K%3\n").arg(col+i).arg(r+3).arg(dat[i]);
                }
            }
            addCol=qMax(addCol, dat.size());
        }
        col+=addCol;
        if (hasError) {
            if (flipTable) {
                out<<QString("F;SDIS;Y%2;X2\nC;Y%2;X2;K\"%1\"\n").arg(tr("error")).arg(col);
            } else {
                out<<QString("F;SDIS;Y2;X%2\nC;Y2;X%2;K\"%1\"\n").arg(tr("error")).arg(col);
            }
            for (int r=0; r<rownames.size(); r++) {
                QStringList dat;
                if (resultsExists(evalname, rownames[r].second)) {
                    switch(resultsGet(evalname, rownames[r].second).type) {
                        case qfrdreNumberError: dat<<CDoubleToQString(resultsGetErrorAsDouble(evalname, rownames[r].second)); hasError=true; break;
                        case qfrdreNumberErrorVector:
                        case qfrdreNumberErrorMatrix: {
                                QVector<double> sl=resultsGetErrorAsDoubleList(evalname, rownames[r].second);
                                if (!vectorsToAvg) {
                                    for (int i=0; i<sl.size(); i++) {
                                        dat<<CDoubleToQString(sl[i]);
                                    }
                                } else dat<<CDoubleToQString(qfstatisticsAverage(sl));
                            } break;
                        default: break;
                    }
                }
                for (int i=0; i<dat.size(); i++) {
                    if (flipTable) {
                        out<<QString("C;X%2;Y%1;K%3\n").arg(col+i).arg(r+3).arg(dat[i]);
                    } else {
                        out<<QString("C;X%1;Y%2;K%3\n").arg(col+i).arg(r+3).arg(dat[i]);
                    }
                }
                addCol=qMax(addCol, dat.size());
            }
            col+=addCol;
        }
    }
    if (flipTable) {
        out<<"F;R1;SDSB\n";
        out<<"F;C1;SDS\n";
        out<<"F;C2;SDSR\n";
    } else {
        out<<"F;C1;SDSR\n";
        out<<"F;R1;SDS\n";
        out<<"F;R2;SDSB\n";
    }
    out<<"E\n";
    return true;*/
}

bool QFRawDataRecord::resultsSave(const QString &filename, int format, bool vectorsToAvg, bool flipTable) const
{
    QStringList colname, rownames;
    QList<QList<QVariant> > data=resultsGetTable(vectorsToAvg, &colname, &rownames);
    if (flipTable) {
        data=dataRotate(data);
        qSwap(colname, rownames);
    }

    QFDataExportHandler::save(data, format, filename, colname, rownames);
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
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
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
                    if (!lp.contains(rn)) {
                        if (j.value().sortPriority ) lp.append(rn);
                        else l.append(rn);
                    }
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
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
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
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
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
                QString lab=j.value().label;
                if (lab.isEmpty()) lab=rn;
                //resultsGetLabel(en, rn);
                if ((group.isEmpty() || (group==g)) && (!l.contains(lab))) {
                    l.append(lab);
                    if (j.value().sortPriority) {
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

QList<QTriple<QString, QString, QString> > QFRawDataRecord::resultsCalcNamesEvalIDsAndLabels(const QString &evalName, const QString &group, const QString &evalgroup) const
{

#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    QStringList l;
    QList<QTriple<QString, QString, QString> > list, listp;
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
                QString lab=j.value().label;
                if (lab.isEmpty()) lab=rn;
                //resultsGetLabel(en, rn);
                if ((group.isEmpty() || (group==g)) && (!l.contains(lab))) {
                    l.append(lab);
                    if (j.value().sortPriority) {
                        if (g.isEmpty()) listp.append(qMakeTriple(lab, rn, en));
                        else listp.append(qMakeTriple(g+QString(": ")+lab, rn, en));
                    } else {
                        if (g.isEmpty()) list.append(qMakeTriple(lab, rn, en));
                        else list.append(qMakeTriple(g+QString(": ")+lab, rn, en));
                    }
                }
            }
        }
    }
    if (list.size()>0) {
        qSort(list.begin(), list.end(), QFRawDataRecord_StringTripleCaseInsensitiveCompareSecond);
    }
    if (listp.size()>0) {
        qSort(listp.begin(), listp.end(), QFRawDataRecord_StringTripleCaseInsensitiveCompareSecond);
    }

    listp.append(list);

    return listp;
}


QList<QPair<QString, QString> > QFRawDataRecord::resultsCalcNamesAndLabelsRichtext(const QString& evalName, const QString& group, const QString& evalgroup) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
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
                QString lab=j.value().label;
                if (lab.isEmpty()) lab=rn; //resultsGetLabel(en, rn);
                QString labrt=j.value().label;
                if (labrt.isEmpty()) labrt=lab; //resultsGetLabelRichtext(en, rn);
                if ((group.isEmpty() || (group==j.value().group)) && (!l.contains(lab))) {
                    l.append(lab);
                    if (j.value().sortPriority) listp.append(qMakeTriple(lab, labrt, rn));
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
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
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
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
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


void QFRawDataRecord::emitPropertiesChanged(const QString &property, bool visible) {
    //qDebug()<<"emitPropertiesChanged( prop="<<property<<"   vis="<<visible<<")";
    if (doEmitPropertiesChanged) {
        //qDebug()<<"QFRawDataRecord ("<<name<<") emits propertiesChanged()";
        emit propertiesChanged(property, visible);
        //qDebug()<<"   ... emitted";

    }
}

void QFRawDataRecord::emitResultsChanged(const QString& evalName, const QString& resultName, bool removed) {
    if (doEmitResultsChanged) {
        //qDebug()<<"QFRawDataRecord ("<<name<<") emits resultsChanged()";
        emit resultsChanged(evalName, resultName, removed);
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
        emit resultsChanged("", "", false);
    }
}

bool QFRawDataRecord::isEmitResultsChangedEnabled() const {
    return doEmitResultsChanged;
}

void QFRawDataRecord::disableEmitPropertiesChanged() {
    doEmitPropertiesChanged=false;
}

void QFRawDataRecord::enableEmitPropertiesChanged(bool emitnow) {
    doEmitPropertiesChanged=true;
    if (emitnow) {
        //qDebug()<<"QFRawDataRecord ("<<name<<") emits propertiesChanged()";
        emit  propertiesChanged("", true);
    }
}

bool QFRawDataRecord::isEmitPropertiesChangedEnabled() const {
    return doEmitPropertiesChanged;
}

QStringList QFRawDataRecord::getAvailableRoles() const
{
    return QStringList();
}

bool QFRawDataRecord::isRoleUserEditable() const
{
    return false;
}

QString QFRawDataRecord::resultsGetInStringMatrix(const QString &evaluationName, const QString &resultName, int row, int column, const QString &defaultValue) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if (!dstore->results.contains(evaluationName)) return defaultValue;
    const QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results.value(evaluationName);
    if (!evr->results.contains(resultName)) return defaultValue;

    const evaluationResult& r=evr->results.value(resultName);
    #ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"unlock";
#endif
 locker.unlock();
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  unlocked";
#endif
    return resultsGetInStringList(evaluationName, resultName, r.columns*row+column, defaultValue);
}

bool QFRawDataRecord::resultsGetInBooleanMatrix(const QString &evaluationName, const QString &resultName, int row, int column, bool defaultValue) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if (!dstore->results.contains(evaluationName)) return defaultValue;
    const QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results.value(evaluationName);
    if (!evr->results.contains(resultName)) return defaultValue;

    const evaluationResult& r=evr->results.value(resultName);
    #ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"unlock";
#endif
 locker.unlock();
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  unlocked";
#endif
    return resultsGetInBooleanList(evaluationName, resultName, r.columns*row+column, defaultValue);
}

QString QFRawDataRecord::resultsGetInStringList(const QString &evaluationName, const QString &resultName, int position, const QString &defaultValue) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if (!dstore->results.contains(evaluationName)) return defaultValue;
    const QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results.value(evaluationName);
    if (!evr->results.contains(resultName)) return defaultValue;

    const evaluationResult& r=evr->results.value(resultName);
    if (position<r.svec.size()) return r.svec[position];

    return defaultValue;
}

qlonglong QFRawDataRecord::resultsGetInIntegerMatrix(const QString &evaluationName, const QString &resultName, int row, int column, qlonglong defaultValue) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if (!dstore->results.contains(evaluationName)) return defaultValue;
    const QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results.value(evaluationName);
    if (!evr->results.contains(resultName)) return defaultValue;

    const evaluationResult& r=evr->results.value(resultName);
    #ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"unlock";
#endif
 locker.unlock();
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  unlocked";
#endif
    return resultsGetInIntegerList(evaluationName, resultName, r.columns*row+column, defaultValue);
}

bool QFRawDataRecord::resultsGetInBooleanList(const QString &evaluationName, const QString &resultName, int position, bool defaultValue) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if (!dstore->results.contains(evaluationName)) return defaultValue;
    //const QFRawDataRecordPrivate::evaluationIDMetadata* ev=dstore->results[evaluationName];
    const QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results.value(evaluationName);
    if (!evr->results.contains(resultName)) return defaultValue;

    evaluationResult r=evr->results.value(resultName);
    if (position<r.bvec.size()) return r.bvec[position];

    return defaultValue;
}

double QFRawDataRecord::resultsGetErrorInNumberErrorMatrix(const QString &evaluationName, const QString &resultName, int row, int column, double defaultValue) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if (!dstore->results.contains(evaluationName)) return defaultValue;
    const QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results.value(evaluationName);
    if (!evr->results.contains(resultName)) return defaultValue;

    const evaluationResult& r=evr->results.value(resultName);
    #ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"unlock";
#endif
 locker.unlock();
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  unlocked";
#endif
    return resultsGetErrorInNumberErrorList(evaluationName, resultName, r.columns*row+column, defaultValue);
}

qlonglong QFRawDataRecord::resultsGetInIntegerList(const QString &evaluationName, const QString &resultName, int position, qlonglong defaultValue) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if (!dstore->results.contains(evaluationName)) return defaultValue;
    const QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results.value(evaluationName);
    if (!evr->results.contains(resultName)) return defaultValue;

    const evaluationResult& r=evr->results.value(resultName);
    if (position<r.ivec.size()) return r.ivec[position];

    return defaultValue;
}

double QFRawDataRecord::resultsGetInNumberMatrix(const QString &evaluationName, const QString &resultName, int row, int column, double defaultValue) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if (!dstore->results.contains(evaluationName)) return defaultValue;
    const QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results.value(evaluationName);
    if (!evr->results.contains(resultName)) return defaultValue;

    const evaluationResult& r=evr->results.value(resultName);
    #ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"unlock";
#endif
 locker.unlock();
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  unlocked";
#endif
    return resultsGetInNumberList(evaluationName, resultName, r.columns*row+column, defaultValue);
}

double QFRawDataRecord::resultsGetErrorInNumberErrorList(const QString &evaluationName, const QString &resultName, int position, double defaultValue) const {
    
#ifdef DEBUG_THREAN
qDebug()<<Q_FUNC_INFO<<"QReadLocker";
#endif
 QReadLocker locker(lock);
#ifdef DEBUG_THREAN
 qDebug()<<Q_FUNC_INFO<<"  locked";
#endif
    if (!dstore->results.contains(evaluationName)) return defaultValue;
    const QFRawDataRecordPrivate::evaluationIDMetadata* evr=dstore->results.value(evaluationName);
    if (!evr->results.contains(resultName)) return defaultValue;

    const evaluationResult& r=evr->results.value(resultName);
    if (position<r.evec.size()) return r.evec[position];

    return defaultValue;
}

int QFRawDataRecord::evaluationResult::getVectorMatrixItems() const {
    switch (type) {
        case QFRawDataRecord::qfrdreNumberVector:
        case QFRawDataRecord::qfrdreNumberMatrix:
            return dvec.size();
        case QFRawDataRecord::qfrdreNumberErrorVector:
        case QFRawDataRecord::qfrdreNumberErrorMatrix:
            return qMin(dvec.size(), evec.size());
        case QFRawDataRecord::qfrdreIntegerVector:
        case QFRawDataRecord::qfrdreIntegerMatrix:
            return ivec.size();
        case QFRawDataRecord::qfrdreBooleanVector:
        case QFRawDataRecord::qfrdreBooleanMatrix:
            return bvec.size();
        case QFRawDataRecord::qfrdreStringVector:
        case QFRawDataRecord::qfrdreStringMatrix:
            return svec.size();
        default:
            return 0;
    }
    return 0;
}

bool QFRawDataRecord::evaluationResult::isNumberType() const
{
    switch (type) {
        case QFRawDataRecord::qfrdreNumber:
        case QFRawDataRecord::qfrdreNumberError:
        case QFRawDataRecord::qfrdreInteger:
        case QFRawDataRecord::qfrdreNumberVector:
        case QFRawDataRecord::qfrdreNumberMatrix:
        case QFRawDataRecord::qfrdreNumberErrorVector:
        case QFRawDataRecord::qfrdreNumberErrorMatrix:
        case QFRawDataRecord::qfrdreIntegerVector:
        case QFRawDataRecord::qfrdreIntegerMatrix:
            return true;
        default:
            return false;
    }
    return false;
}

double QFRawDataRecord::evaluationResult::getAsDouble() const
{
    switch (type) {
        case QFRawDataRecord::qfrdreNumber:
            return dvalue;
        case QFRawDataRecord::qfrdreInteger:
            return ivalue;
        default:
            return 0;
    }
    return 0;
}

QVector<double> QFRawDataRecord::evaluationResult::getAsDoubleVector() const
{
    QVector<double> res;
    switch (type) {
        case QFRawDataRecord::qfrdreNumber:
        case QFRawDataRecord::qfrdreNumberError:
            res<<dvalue;
            return res;
        case QFRawDataRecord::qfrdreInteger:
            res<<ivalue;
            return res;
        case QFRawDataRecord::qfrdreNumberVector:
        case QFRawDataRecord::qfrdreNumberMatrix:
        case QFRawDataRecord::qfrdreNumberErrorVector:
        case QFRawDataRecord::qfrdreNumberErrorMatrix:
            return dvec;
        case QFRawDataRecord::qfrdreIntegerVector:
        case QFRawDataRecord::qfrdreIntegerMatrix:
            {
                for(int i=0; i<ivec.size(); i++) res<<ivec[i];
                return res;
            }
        default:
            return res;
    }
    return res;
}

qfmpResult QFRawDataRecord::evaluationResult::getAsMathParserResult() const
{
    switch (type) {
        case QFRawDataRecord::qfrdreNumber:
        case QFRawDataRecord::qfrdreNumberError:
            return qfmpResult(dvalue);
        case QFRawDataRecord::qfrdreInteger:
            return qfmpResult((double)ivalue);
        case QFRawDataRecord::qfrdreNumberVector:
        case QFRawDataRecord::qfrdreNumberMatrix:
        case QFRawDataRecord::qfrdreNumberErrorVector:
        case QFRawDataRecord::qfrdreNumberErrorMatrix:
            return qfmpResult(dvec);
        case QFRawDataRecord::qfrdreIntegerVector:
        case QFRawDataRecord::qfrdreIntegerMatrix:
            {
                QVector<double> dv;
                for(int i=0; i<ivec.size(); i++) dv<<ivec[i];
                return qfmpResult(dv);
            }
        case QFRawDataRecord::qfrdreString:
            return qfmpResult(svalue);
        case QFRawDataRecord::qfrdreStringVector:
        case QFRawDataRecord::qfrdreStringMatrix:
            if (!svec.isEmpty()) return qfmpResult(svec);
            break;
        case QFRawDataRecord::qfrdreBoolean:
            return qfmpResult(bvalue);
        case QFRawDataRecord::qfrdreBooleanVector:
        case QFRawDataRecord::qfrdreBooleanMatrix:
            if (!bvec.isEmpty()) return qfmpResult(bvec);
            break;
        case QFRawDataRecord::qfrdreInvalid:
        default:
            return qfmpResult::invalidResult();
    }
    return qfmpResult::invalidResult();
}

QVariant QFRawDataRecord::evaluationResult::getAsVariant() const
{
    switch (type) {
        case QFRawDataRecord::qfrdreNumber:
        case QFRawDataRecord::qfrdreNumberError:
            return QVariant(dvalue);
        case QFRawDataRecord::qfrdreInteger:
            return QVariant((double)ivalue);
        case QFRawDataRecord::qfrdreNumberVector:
        case QFRawDataRecord::qfrdreNumberMatrix:
        case QFRawDataRecord::qfrdreNumberErrorVector:
        case QFRawDataRecord::qfrdreNumberErrorMatrix:{
                QList<QVariant> dv;
                for(int i=0; i<dvec.size(); i++) dv<<dvec[i];
                return QVariant(dv);
            } break;
        case QFRawDataRecord::qfrdreIntegerVector:
        case QFRawDataRecord::qfrdreIntegerMatrix:
            {
                QList<QVariant> dv;
                for(int i=0; i<ivec.size(); i++) dv<<ivec[i];
                return QVariant(dv);
            } break;
        case QFRawDataRecord::qfrdreString:
            return QVariant(svalue);
        case QFRawDataRecord::qfrdreStringVector:
        case QFRawDataRecord::qfrdreStringMatrix:
            if (!svec.isEmpty()) return QVariant(svec);
            break;
        case QFRawDataRecord::qfrdreBoolean:
            return QVariant(bvalue);
        case QFRawDataRecord::qfrdreBooleanVector:
        case QFRawDataRecord::qfrdreBooleanMatrix:{
            QList<QVariant> dv;
                for(int i=0; i<bvec.size(); i++) dv<<bvec[i];
                return QVariant(dv);
            }
            break;
        case QFRawDataRecord::qfrdreInvalid:
        default:
            return QVariant();
    }
    return QVariant();
}

void QFRawDataRecord::evaluationResult::setFromMathParserResult(const qfmpResult &result)
{
    if (result.isValid) {
        switch(result.type) {
            case qfmpDouble:
                type=QFRawDataRecord::qfrdreNumber;
                dvalue=result.num;
                break;
            case qfmpString:
                type=QFRawDataRecord::qfrdreString;
                svalue=result.str;
                break;
            case qfmpBool:
                type=QFRawDataRecord::qfrdreBoolean;
                bvalue=result.boolean;
                break;
            case qfmpDoubleVector:
                type=QFRawDataRecord::qfrdreNumberVector;
                dvec=result.numVec;
                break;
            case qfmpBoolVector:
                type=QFRawDataRecord::qfrdreBooleanVector;
                bvec=result.boolVec;
                break;
            case qfmpStringVector:
                type=QFRawDataRecord::qfrdreStringVector;
                svec=result.strVec;
                break;
            default:
            case qfmpVoid:
                type=QFRawDataRecord::qfrdreInvalid;
                QVariantList vl=result.asVariantList();
                if (vl.size()>1) {
                    switch (vl.first().type()) {
                        case QVariant::Double:
                        case QVariant::Int:
                        case QVariant::LongLong:
                        case QVariant::UInt:
                        case QVariant::ULongLong:
                            type=QFRawDataRecord::qfrdreNumberVector;
                            dvec.clear();
                            for (int i=0; i<vl.size(); i++) dvec.append(vl[i].toDouble());
                            break;
                        case QVariant::Bool:
                            type=QFRawDataRecord::qfrdreBooleanVector;
                            bvec.clear();
                            for (int i=0; i<vl.size(); i++) bvec.append(vl[i].toBool());
                            break;
                        case QVariant::String:
                            type=QFRawDataRecord::qfrdreStringVector;
                            svec.clear();
                            for (int i=0; i<vl.size(); i++) svec.append(vl[i].toString());
                            break;
                        default: break;
                    }
                } else if (vl.size()==1) {
                    switch (vl.first().type()) {
                        case QVariant::Double:
                        case QVariant::Int:
                        case QVariant::LongLong:
                        case QVariant::UInt:
                        case QVariant::ULongLong:
                            type=QFRawDataRecord::qfrdreNumber;
                            dvalue=vl.first().toDouble();
                            break;
                        case QVariant::Bool:
                            type=QFRawDataRecord::qfrdreBoolean;
                            bvalue=vl.first().toBool();
                            break;
                        case QVariant::String:
                            type=QFRawDataRecord::qfrdreString;
                            svalue=vl.first().toString();
                            break;
                        default: break;
                    }
                }
                break;
        }
    } else {
        type=QFRawDataRecord::qfrdreInvalid;
    }
}


