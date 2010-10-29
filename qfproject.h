#ifndef QFPROJECT_H
#define QFPROJECT_H

#include <QObject>
#include <QMap>
#include <QSet>
#include <QAbstractItemModel>
#include <QString>
#include <iostream>
#include <QProgressBar>
#include <QStringList>
#include "qt/qtlogfile.h"

// forward declaration
class QFRawDataRecord;
class QFEvaluationItem;
class QFProjectRawDataModel;
class QFProjectTreeModel;


typedef QMap<QString, QVariant> qfp_param_type;

/*! \brief this class manages one project

    The  project may be thought of as the central data vault. All files,
    evaluations, ... are represented by items. These items are implemented
    as classes which are derived of a small set of special base classes.

    Several Qt data models may be gotten from this object to access the data in the project:
      - getRawDataModel() returns a data model which may be used to display a list of all
        contained raw data items. This model is drag-enabled!
      - getTreeModel() returns a data model representing the items in the model as a tree.
        This model is used to display the project tree
    .

    You may associate free properties to the project as well as to each item
    in the project that supports this (rawData record and evaluationItems do).
    The project properties may be used to store global settings and are generally
    NOT user editable!

*/
class QFProject : public QObject {
        Q_OBJECT
    protected:
        /** \brief contains the currently highest ID */
        int highestID;
        /** \brief stores all raw data records that belong to the project */
        QMap<int, QFRawDataRecord*> rawData;
        /** \brief stores all evaluation records that belong to the project */
        QMap<int, QFEvaluationItem*> evaluations;
        /** \brief stores all raw data records that belong to the project */
        QSet<int> IDs;
        /** \brief name of the project */
        QString name;
        /** \brief description of the project */
        QString description;
        /** \brief creator of the project */
        QString creator;
        /** \brief file associated with the project project (empty if none)*/
        QString file;
        /** \brief indicates whether an error has occured */
        bool errorOcc;
        /** \brief contains the description of the last error */
        QString errorDesc;
        /** \brief indicates whether the data has changed  */
        bool dataChange;
        /** \brief data model which may be used to display a list of all
         *         contained raw data items. This model is dragf-enabled! */
        QFProjectRawDataModel* rdModel;
        /** \brief data model representing the items in the model as a tree.
         * This model is used to display the project tree */
        QFProjectTreeModel* treeModel;
        /** \brief struct to store a property */
        struct propertyItem {
            QVariant data;
            bool usereditable;
            bool visible;
        };
        /** \brief internal store for the objectproperties */
        QMap<QString, propertyItem> props;

        /** \brief pointer to a logfile object that may be used to output logging information */
        QtLogFile* logF;
        /** \brief pointer to a progressbar control */
        QProgressBar* prg;
    public:
        /** Default constructor */
        QFProject(QObject* parent=NULL, QtLogFile* lf=NULL, QProgressBar* p=NULL);
        /** Default constructor which loads a project */
        QFProject(QString& filename, QObject* parent=NULL, QtLogFile* lf=NULL, QProgressBar* p=NULL);
        /** Default destructor */
        virtual ~QFProject();

        /** \brief set the LogFile object pointer */
        inline void setLogFile(QtLogFile* l) { logF=l; };
        /** \brief returns a pointer to a logfile object */
        inline QtLogFile* getLogFile() { return logF; };

        /** \brief set the LogFile object pointer */
        inline void setProgressBar(QProgressBar* l) { prg=l; };
        /** \brief returns a pointer to a logfile object */
        inline QProgressBar* getProgressBar() { return prg; };

        /** \brief return a new unused ID (stores the returned ID as currently highestID, so the next call will return a higher ID!) */
        int getNewID();
        /** \brief tell the project that a a raw data record belongs to the project. The records ID will be internally registered to access the record!.
         *
         * \return \c false if the ID is already in use.
         * If ID is unused \a rec will be registered as belonging to the project.
         */
        bool registerRawDataRecord(QFRawDataRecord* rec);
        /** \brief tell the project that an evaluation record belongs to the project.
         *         The records ID will be internally registered to access the record!.
         *
         * \return \c false if the ID is already in use.
         * If ID is unused \a rec will be registered as belonging to the project.
         */
        bool registerEvaluation(QFEvaluationItem* rec);
        /** \brief return \c true if ID is unused */
        inline bool checkID(int ID)const  { return !IDs.contains(ID); }
        /** \brief return the name */
        inline QString getName()const  { return name; }
        /** \brief set name of project */
        inline void setName(const QString& n) {
            if (name!=n) {
                name=n;
                emit propertiesChanged();
            }
        }
        /** \brief return the description  */
        inline QString getDescription()const  { return description; };
        /** \brief set description of project */
        inline void setDescription(const QString& d) {
            if (description!=d) {
                description=d;
                emit propertiesChanged();
            }
        }
        /** \brief return the creator  */
        inline QString getCreator()const  { return creator; };
        /** \brief set creator of project */
        inline void setCreator(const QString& c) {
            if (creator!=c) {
                creator=c;
                emit propertiesChanged();
            }
        }
        /** \brief return the file name  */
        inline QString getFile()const  { return file; };

        /** \brief return the number of raw data records in the project */
        inline int getRawDataCount()const  { return rawData.size(); }

        /** \brief return the number of raw data records in the project */
        inline int getEvaluationCount()const  { return evaluations.size(); }
        /** \brief returns \c true if a raw data record for the specified ID exists */
        inline bool rawDataIDExists(int ID)const  { return rawData.contains(ID); }
        /** \brief returns \c true if a raw data record for the specified ID exists */
        inline bool evaluationIDExists(int ID)const  { return evaluations.contains(ID); }
        /** \brief returns \c true if a raw data record for the specified ID exists */
        inline bool rawDataExists(QFRawDataRecord* rec) const  { return rawData.values().contains(rec); }
        /** \brief returns \c true if a raw data record for the specified ID exists */
        inline bool evaluationExists(QFEvaluationItem* rec) const  { return evaluations.values().contains(rec); }
        /** \brief returns \c true if a raw data record for the specified ID exists */
        inline int getRawDataIndex(QFRawDataRecord* rec) const  { return rawData.values().indexOf(rec); }
        /** \brief returns \c true if a raw data record for the specified ID exists */
        inline bool getEvaluationIndex(QFEvaluationItem* rec) const  { return evaluations.values().indexOf(rec); }
        /** \brief return the raw data record specified by the ID, or \c NULL */
        inline QFRawDataRecord* getRawDataByID(int ID) {
            if (rawDataIDExists(ID)) return rawData[ID];
            return NULL;
        }
        /** \brief return the i-th raw data record, or \c NULL */
        inline QFRawDataRecord* getRawDataByNum(int i) const {
            if (i>=rawData.size() || i<0) return NULL;
            int ID=rawData.keys().at(i);
            return rawData[ID];
        }
        /** \brief return the next sibling rawdata record, or NULL if none */
        QFRawDataRecord* getNextRawData(QFRawDataRecord* current);
        /** \brief return the previous sibling rawdata record, or NULL if none */
        QFRawDataRecord* getPreviousRawData(QFRawDataRecord* current);
        /** \brief return the next sibling rawdata record, or NULL if none, which has the same type as current */
        QFRawDataRecord* getNextRawDataOfSameType(QFRawDataRecord* current);
        /** \brief return the previous sibling rawdata record, or NULL if none, which has the same type as current */
        QFRawDataRecord* getPreviousRawDataOfSameType(QFRawDataRecord* current);
        /** \brief return the raw data record specified by the ID, or \c NULL */
        inline QFEvaluationItem* getEvaluationByID(int ID) {
            if (evaluationIDExists(ID)) return evaluations[ID];
            return NULL;
        }
        /** \brief return the i-th raw data record, or \c NULL */
        inline QFEvaluationItem* getEvaluationByNum(int i) const {
            if (i>=evaluations.size() || i<0) return NULL;
            int ID=evaluations.keys().at(i);
            return evaluations[ID];
        }
        /** \brief return the next sibling rawdata record, or NULL if none */
        QFEvaluationItem* getNextEvaluation(QFEvaluationItem* current);
        /** \brief return the previous sibling rawdata record, or NULL if none */
        QFEvaluationItem* getPreviousEvaluation(QFEvaluationItem* current);
        /** \brief return the next sibling rawdata record, or NULL if none */
        QFEvaluationItem* getNextEvaluationOfSameType(QFEvaluationItem* current);
        /** \brief return the previous sibling rawdata record, or NULL if none */
        QFEvaluationItem* getPreviousEvaluationOfSameType(QFEvaluationItem* current);

        /** brief returns a QStringList with all property names currently used in the project */
        QStringList getAllPropertyNames();

        /** \brief returns a QList with all the used IDs for rawData records*/
        inline QList<int> getRawDataIDList() { return rawData.keys(); }
        /** \brief returns a QList with all the used IDs for evaluations */
        inline QList<int> getEvaluationIDList() { return evaluations.keys(); }
        /** \brief returns a QList with all the raw data records */
        inline QList<QFRawDataRecord*> getRawDataList() { return rawData.values(); }
        /** \brief returns a QList with all the evaluation records */
        inline QList<QFEvaluationItem*> getEvaluationList() { return evaluations.values(); }
        /** \brief delete the specified raw data record */
        void deleteRawData(int ID);
        /** \brief delete the specified evaluation record */
        void deleteEvaluation(int ID);
        /** \brief add new raw data record
         *
         * \param type type of the record to insert ("table", "unknown", ...)
         * \param name name of the new record
         * \param inputFiles files to import into the project
         * \param initParams parameters to add to the record parameters BEFORE importing the files
         * \param initParamsReadonly a list of the parameters that shall be readonly, i.e. not usereditable
         * \return a pointer to the newly created record
         */
        QFRawDataRecord* addRawData(QString type, QString name=QString(""), QStringList inputFiles=QStringList(), qfp_param_type initParams=qfp_param_type(), QStringList initParamsReadonly=QStringList());

        /** \brief add new evaluation record
         *
         * \param type type of the record to insert ("table", "unknown", ...)
         * \param name name of the new record
         * \return a pointer to the newly created record
         */
        QFEvaluationItem* addEvaluation(QString type, QString name=QString(""));

        /** \brief returns \c true if an error occured */
        inline bool error() const { return errorOcc; };
        /** \brief returns the description of the last error */
        inline QString errorDescription() const { return errorDesc; };
        /** \brief write project to XML file, sets error and errorDescription, as well as \c dataChange=false */
        void writeXML(const QString& file);
        /** \brief open XML project file, sets error and errorDescription, as well as \c dataChange=false */
        void readXML(const QString& file);
        /** \brief returns \c true when the project has changed */
        inline bool hasChanged() { return dataChange; };
        /** \brief returns a data model which may be used to display a list of all
         *         contained raw data items. This model is dragf-enabled! */
        inline QFProjectRawDataModel* getRawDataModel() const { return rdModel; };
        /** \brief data model representing the items in the model as a tree.
         * This model is used to display the project tree */
        inline QFProjectTreeModel* getTreeModel() const { return treeModel; };
        /** \brief clear all properties */
        inline void clearProperties() { props.clear(); emit propertiesChanged(); }
        /** \brief return the value of the specified property */
        inline QVariant getProperty( const QString& p) { return props[p].data; };
        /** \brief return the value of the specified property or the supplied default value */
        inline QVariant getProperty(const QString& p, const QVariant& defaultValue) {
            if (props.contains(p)) return props.value(p).data;
            return defaultValue;
        };
        /** \brief return the number of properties in the object */
        inline unsigned int getPropertyCount() { return props.size(); };
        /** \brief return the number of visible properties in the object */
        unsigned int getVisiblePropertyCount();
        /** \brief get the name of the i-th visible property */
        QString getVisibleProperty(unsigned int i);
        /** \brief returns a QStringList which contains the names of all properties */
        inline QStringList getPropertyNames() { return props.keys(); };
        /** \brief returns the name of the i-th property */
        inline QString getPropertyName(int i) { return props.keys().at(i); };
        /** \brief returns whether the given property is visible */
        inline bool isPropertyVisible(QString property) {
            if (!props.contains(property)) return false;
            return props[property].visible;
        };
        /** \brief returns whether the given property is user editable */
        inline bool isPropertyUserEditable(QString property) {
            if (!props.contains(property)) return false;
            return props[property].usereditable;
        };
        /** \brief delete the given property */
        inline void deleteProperty(const QString& n) { props.remove(n); emit propertiesChanged(); };

        /** \brief returns true if the specified property exists */
        inline bool propertyExists(const QString& p) { return props.contains(p); };
    signals:
        /** \brief emitted when the data changed state of the project is modified */
        void wasChanged(bool changed);
        /** \brief emitted when a record is about to be deleted */
        void recordAboutToBeDeleted(QFRawDataRecord* r);
        /** \brief emitted when a record is about to be deleted */
        void evaluationAboutToBeDeleted(QFEvaluationItem* r);
        /** \brief emitted when the project properties (name, description, ...) changed */
        void propertiesChanged();
        /** \brief emitted when an error occured (may be used to display the error) */
        void errorOccured(QString errorDescription);
    public slots:
        /** \brief tell the project that the data contained in it has changed ... and it needs to be saved */
        void setDataChanged() {
            dataChange=true;
            emit wasChanged(dataChange);
        }
        /** \brief set property to the specified value */
        inline void setProperty(const QString& p, QVariant value, bool usereditable=false, bool visible=false) {
            propertyItem i;
            i.data=value;
            i.usereditable=usereditable;
            i.visible=visible;
            props[p]=i;
            emit propertiesChanged();
        }
        /** \brief set property to the specified value */
        inline void setDoubleProperty(const QString& p, double value, bool usereditable=false, bool visible=false) {
            setProperty(p, QVariant(value), usereditable, visible);
        }
        /** \brief set property to the specified value */
        inline void setBoolProperty(const QString& p, bool value, bool usereditable=false, bool visible=false) {
            setProperty(p, QVariant(value), usereditable, visible);
        }
        /** \brief set property to the specified value */
        inline void setIntProperty(const QString& p, qlonglong value, bool usereditable=false, bool visible=false) {
            setProperty(p, QVariant(value), usereditable, visible);
        }
        /** \brief set property to the specified value */
        inline void setStringProperty(const QString& p, QString value,bool usereditable=false, bool visible=false) {
            setProperty(p, QVariant(value), usereditable, visible);
        }
    protected:
        /** \brief set the internal error flag and description */
        inline void setError(QString description) {
            errorOcc=true;
            errorDesc=description;
            emit errorOccured(description);
        }

    protected slots:
        void projectChanged();

};


#endif // QFPROJECT_H
