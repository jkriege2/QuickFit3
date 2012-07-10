#ifndef QFPROJECT_H
#define QFPROJECT_H

#include "lib_imexport.h"
#include <map>
#include <QObject>
#include <QMap>
#include <QSet>
#include <QAbstractItemModel>
#include <QString>
#include <iostream>
#include <QProgressBar>
#include <QStringList>
#include <QPair>
#include <QList>
#include <QPointer>
#include "qfproperties.h"
#include "qtriple.h"

// forward declaration
class QFRawDataRecord;
class QFEvaluationItem;
class QFProjectRawDataModel;
class QFProjectTreeModel;
class QFEvaluationItemFactory;
class QFRawDataRecordFactory;
class QFPluginServices;

typedef QMap<QString, QVariant> qfp_param_type;

/*! \brief this class manages one project
    \ingroup qf3lib_project

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
class QFLIB_EXPORT QFProject : public QObject, public QFProperties {
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


        QFEvaluationItemFactory* evalFactory;
        QFRawDataRecordFactory* rdrFactory;
        QFPluginServices* services;

    public:
        /** Default constructor */
        QFProject(QFEvaluationItemFactory* evalFactory, QFRawDataRecordFactory* rdrFactory, QFPluginServices* services, QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFProject();

        QFPluginServices* getServices() { return services; };

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
        bool checkID(int ID)const;
        /** \brief return the name */
        QString getName()const;
        /** \brief set name of project */
        void setName(const QString& n);
        /** \brief return the description  */
        QString getDescription()const;
        /** \brief set description of project */
        void setDescription(const QString& d);
        /** \brief return the creator  */
        QString getCreator()const;
        /** \brief set creator of project */
        void setCreator(const QString& c);
        /** \brief return the file name  */
        QString getFile()const;

        /** \brief return the number of raw data records in the project */
        int getRawDataCount()const;

        /** \brief return the number of raw data records in the project */
        int getEvaluationCount()const;
        /** \brief returns \c true if a raw data record for the specified ID exists */
        bool rawDataIDExists(int ID)const;
        /** \brief returns \c true if a raw data record for the specified ID exists */
        bool evaluationIDExists(int ID)const;
        /** \brief returns \c true if a raw data record for the specified ID exists */
        bool rawDataExists(QFRawDataRecord* rec) const;
        /** \brief returns \c true if a raw data record for the specified ID exists */
        bool evaluationExists(QFEvaluationItem* rec) const;
        /** \brief returns \c true if a raw data record for the specified ID exists */
        int getRawDataIndex(QFRawDataRecord* rec) const;
        /** \brief returns \c true if a raw data record for the specified ID exists */
        bool getEvaluationIndex(QFEvaluationItem* rec) const;
        /** \brief return the raw data record specified by the ID, or \c NULL */
        QFRawDataRecord* getRawDataByID(int ID);
        /** \brief return the i-th raw data record, or \c NULL */
        QFRawDataRecord* getRawDataByNum(int i) const;
        /** \brief return the next sibling rawdata record, or NULL if none */
        QFRawDataRecord* getNextRawData(QFRawDataRecord* current);
        /** \brief return the previous sibling rawdata record, or NULL if none */
        QFRawDataRecord* getPreviousRawData(QFRawDataRecord* current);
        /** \brief return the next sibling rawdata record, or NULL if none, which has the same type as current */
        QFRawDataRecord* getNextRawDataOfSameType(QFRawDataRecord* current);
        /** \brief return the previous sibling rawdata record, or NULL if none, which has the same type as current */
        QFRawDataRecord* getPreviousRawDataOfSameType(QFRawDataRecord* current);
        /** \brief return the raw data record specified by the ID, or \c NULL */
        QFEvaluationItem* getEvaluationByID(int ID) const;
        /** \brief return the i-th raw data record, or \c NULL */
        QFEvaluationItem* getEvaluationByNum(int i) const;
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
         * \param inputFilesTypes types of the files listed in \a inputFiles
         * \return a pointer to the newly created record
         */
        QFRawDataRecord* addRawData(QString type, QString name=QString(""), QStringList inputFiles=QStringList(), qfp_param_type initParams=qfp_param_type(), QStringList initParamsReadonly=QStringList(), QStringList inputFilesTypes=QStringList(), QStringList inputFilesDescriptions=QStringList());

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
        /*! \brief write project to XML file, sets error and errorDescription, as well as \c dataChange=false
            \param file the file to store the project to
            \param resetDataChanged if \c true (default) the property dataChange is reset to \c false.
                   Set this to \c false, if you want to save the project, without influence on the contained data (e.g. for autosaves).
        */
        void writeXML(const QString& file, bool resetDataChanged=true);
        /** \brief open XML project file, sets error and errorDescription, as well as \c dataChange=false */
        void readXML(const QString& file);
        /** \brief returns \c true when the project has changed */
        inline bool hasChanged() { return dataChange; };
        /** \brief returns a data model which may be used to display a list of all
         *         contained raw data items. This model is dragf-enabled! */
        QFProjectRawDataModel* getRawDataModel();
        /** \brief data model representing the items in the model as a tree.
         * This model is used to display the project tree */
        QFProjectTreeModel* getTreeModel();

        /*! \brief return a list of all result names for a given evaluation name for all raw data records in this project

            \param evalFilter a wildcard filter for the evaluation name. Use \c "*" to select all evaluations.
            \param groupFilter a wildcard filter for the result group. Use \c "*" to select all evaluations.
        */
        QStringList rdrCalcMatchingResultsNames(const QString& evalFilter, const QString& groupFilter=QString("*")) const;

        /*! \brief return a list of all result names (and labels) for a given evaluation name for all raw data records in this project

            \param evalFilter a wildcard filter for the evaluation name. Use \c "*" to select all evaluations.
            \param groupFilter a wildcard filter for the result group. Use \c "*" to select all evaluations.
        */
        QList<QPair<QString, QString> > rdrCalcMatchingResultsNamesAndLabels(const QString& evalFilter, const QString& groupFilter=QString("*")) const;

        /*! \brief return a list of all QFRawDataRecord object that contain at least one evaluation which matched the given \a evalFilter
                   together with a list of all the matching evaluation results therein.

            \param evalFilter a wildcard filter for the evaluation name. Use \c "*" to select all evaluations.
        */
        QList<QPair<QPointer<QFRawDataRecord>, QString> > rdrCalcMatchingResults(const QString& evalFilter) const;

        /*! \brief save the raw data record results stored in this project for a given evaluation to a CSV file



            Saves a table of the results as Comma-Separated File \a filename. Not all result types may be exported,
            as this format does not support every dataformat. Any data that may be save as a single item is stored directly
            (qfrdreNumber, qfrdreInteger, qfrdreString, qfrdreBoolean). Numbers with errors (qfrdreNumberError) are also stored,
            If numbers in a column have errors then a second column is added for the errors.

            \param evalFilter a wildcard filter for the evaluation name. Use \c "*" to select all evaluations.
            \param filename the file to create
            \param vectorsToAvg if \c false vector/matrix results are expanded into several columns. Otherwise only the avg/stddev are stored in 2 columns
            \param separator column separator in the file
            \param decinmalPoint which character to sue as decimal point
            \param stringDelimiter strings are surrounded by this character (one in front,one behind)
            \return \c true on success
        */
        bool rdrResultsSaveToCSV(const QString& evalFilter, QString filename, bool vectorsToAvg=false, QChar separator=',', QChar decimalPoint='.', QChar stringDelimiter='"');

        /*! \brief save the  raw data record results stored in this project for a given evaluation to a SYLK file

            saves a table of the results as Comma-Separated File \a filename. Not all result types may be exported,
            as this format does not support every dataformat. Any data that may be save as a single item is stored directly
            (qfrdreNumber, qfrdreInteger, qfrdreString, qfrdreBoolean). Numbers with errors (qfrdreNumberError) are also stored,
            If numbers in a column have errors then a second column is added for the errors.

            \param evalFilter a wildcard filter for the evaluation name. Use \c "*" to select all evaluations.
            \param filename the file to create
            \param vectorsToAvg if \c false vector/matrix results are expanded into several columns. Otherwise only the avg/stddev are stored in 2 columns
            \return \c true on success
        */
        bool rdrResultsSaveToSYLK(const QString& evalFilter, QString filename, bool vectorsToAvg=false);


        QFRawDataRecordFactory* getRawDataRecordFactory() { return rdrFactory; };
        QFEvaluationItemFactory* getEvaluationItemFactory() { return evalFactory; };
    signals:
        /** \brief emitted when the data changed state of the project is modified */
        void wasChanged(bool changed);
        /** \brief emitted when a record is about to be deleted */
        void recordAboutToBeDeleted(QFRawDataRecord* r);
        /** \brief emitted when a record is about to be deleted */
        void evaluationAboutToBeDeleted(QFEvaluationItem* r);
        /** \brief emitted when properties changed */
        void propertiesChanged();
        /** \brief emitted when an error occured (may be used to display the error) */
        void errorOccured(QString errorDescription);
        /** \bief emitted when the project structure changes (i.e. a record is added, or renamed) */
        void structureChanged();

        /** \brief emitted when the data in a record changes */
        //void rdDataChanged(QFRawDataRecord* record);
    public slots:
        /** \brief tell the project that the data contained in it has changed ... and it needs to be saved */
        void setDataChanged();

        virtual void setStructureChanged();

    protected:
        /** \brief set the internal error flag and description */
        void setError(QString description);
        /** \copydoc QFProperties::emitPropertiesChanged() */
        virtual void emitPropertiesChanged();

        virtual void emitStructureChanged();
        /** \copybrief QFProperties::setPropertiesError() */
        virtual void setPropertiesError(QString message);


};


#endif // QFPROJECT_H
