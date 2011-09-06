#ifndef QFRAWDATARECORD_H
#define QFRAWDATARECORD_H
#include <QSettings>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QStringList>
#include <QMap>
#include <QIcon>
#include <QAbstractTableModel>
#include "qfproject.h"
#include "qfrawdataeditor.h"
#include "qfrdrresultsmodel.h"
#include "qfrdrpropertymodel.h"
#include "qfproperties.h"
#include "lib_imexport.h"


class QDomElement; // forward

/*! \brief this class manages one raw data record in the project
    \ingroup qf3lib_project


    This class manages the basic properties of every raw data item in QuickFit 3.0.


    \section QFRawDataRecord_Error Error Reporting
    Errors are reported by the methods error() which returns \c true if an error has occured and the method
    errorDescription() which returns a textual description of the error. The protected method setError() may be
    used to indicate that an error has occured to the class.


    \section QFRawDataRecord_Help Online-Help
    The user may supply an Online-Help for the QFRawDataRecord and QFRawDataEditor. The Help has to be written
    as HTML page (or a set of these) and has to be stored as \verbatim plugins/help/<plugin_id>/<plugin_id>.html \endverbatim.

 */
class QFLIB_EXPORT QFRawDataRecord : public QObject, public QFProperties {
        Q_OBJECT
    public:
        /** \brief class constructor, reads from QDomElement */
        QFRawDataRecord(QFProject* parent);
        /** \brief Default destructor */
        virtual ~QFRawDataRecord();

        /** \brief initialize the object with the given data */
        void init(QString name=QString(""), QStringList inputFiles=QStringList());
        /** \brief initialize from QDomElement */
        void init(QDomElement& e);


        /** \brief return the next sibling rawdata record in the project */
        inline QFRawDataRecord* getNext() { return project->getNextRawData(this); };
        /** \brief return the next sibling rawdata record in the project */
        inline QFRawDataRecord* getPrevious() { return project->getPreviousRawData(this); };
        /** \brief return the next sibling rawdata record in the project, which has the same type as this record */
        inline QFRawDataRecord* getNextOfSameType() { return project->getNextRawDataOfSameType(this); };
        /** \brief return the next sibling rawdata record in the project, which has the same type as this record */
        inline QFRawDataRecord* getPreviousOfSameType() { return project->getPreviousRawDataOfSameType(this); };

        /** \brief return the ID */
        inline int getID() { return ID; };
        /** \brief return the name */
        inline QString getName() { return name; }
        /** \brief return the description  */
        inline QString getDescription() { return description; };
        /** \brief return the list of linked files */
        inline QStringList getFiles() { return files; }
        /** \brief return a pointer to the project that contains this QRawDatarecord */
        inline QFProject* getProject() { return project; }


        /** \brief returns \c true if an error occured */
        inline bool error() { return errorOcc; }
        /** \brief returns the description of the last error */
        inline QString errorDescription() { return errorDesc; }



    public slots:
        /** \brief set the name */
        inline void setName(const QString n) {
            name=n;
            //emit dataChanged(propModel->index(project->getRawDataIndex(this), 0), index(project->getRawDataIndex(this), 2));
            emitPropertiesChanged();
        }
        /** \brief set the description  */
        inline void setDescription(const QString& d) {
            description=d;
            //emit dataChanged(index(project->getRawDataIndex(this), 0), index(project->getRawDataIndex(this), 2));
            emitPropertiesChanged();
        };
        /** \brief returns a model which may be used to access and edit the properties in this object  */
        QFRDRPropertyModel* getPropertyModel();
    signals:
        /** \brief emitted whenever at least one of the properties changes */
        void propertiesChanged();
        /** \brief emitted whenever at least one of the results changes */
        void resultsChanged();
        /** \brief emitted whenever the data in this object changes */
        void rawDataChanged();
    public:
        /** \copydoc QFProperties::emitPropertiesChanged() */
        virtual void emitPropertiesChanged() { emit propertiesChanged(); };
        /** \brief this function emits a resultsChanged() signal. */
        virtual void emitResultsChanged() { if (doEmitResultsChanged) emit resultsChanged(); };
        /** \brief this function emits a rawDataChanged() signal. */
        virtual void emitRawDataChanged() { emit rawDataChanged(); };
        /** \brief disable emitting of rawDataChanged() signal*/
        void disableEmitResultsChanged() { doEmitResultsChanged=false; }
        /** \brief enable emitting of rawDataChanged() signal and emit one signal */
        void enableEmitResultsChanged() { doEmitResultsChanged=true; emit resultsChanged(); }
    protected:
        /** \copybrief QFProperties::setPropertiesError() */
        virtual void setPropertiesError(QString message) { setError(message); };

        /** \brief a model used to access the properties */
        QFRDRPropertyModel* propModel;
        /** \brief ID of the raw data record */
        int ID;
        /** \brief name of the raw data record */
        QString name;
        /** \brief description of the raw data record */
        QString description;
        /** \brief list of files connected with this record */
        QStringList files;
        /** \brief indicates whether an error has occured */
        bool errorOcc;
        /** \brief contains the description of the last error */
        QString errorDesc;
        /** \brief pointer to the parent project object */
        QFProject* project;

        /** \brief read object contents from QDomElement */
        void readXML(QDomElement& e);
        /** \brief write data contents to QXmlStreamWriter (data tag) <b>IMPLEMENT IN CHILD CLASSES!</b> */
        virtual void intWriteData(QXmlStreamWriter& w) {};
        /** \brief read in external data files <b>and</b> data stored in the project file <b>IMPLEMENT IN CHILD CLASSES!</b>
         *
         * If \a e is \c NULL then this method should only read the datafiles already saved in the files property.
         */
        virtual void intReadData(QDomElement* e=NULL) {};
        /** \brief set the internal error flag and description */
        void setError(QString description) { errorOcc=true; errorDesc=description; }


    public:
        /** \brief this enum contains possible types of evaluation results */
        enum evaluationResultType {
            qfrdreNumber,
            qfrdreNumberError,
            qfrdreNumberVector,
            qfrdreNumberMatrix,
            qfrdreInteger,
            qfrdreString,
            qfrdreBoolean,
            qfrdreInvalid
        };
        /*! \brief this struct is used to store evaluation results associated with this data record

            Data is stored in different fields, according to the data type:
              - evaluationResultType::qfrdreNumber: data stored in dvalue [double]
              - evaluationResultType::qfrdreNumberError: data stored in dvalue, error on derror [double]
              - evaluationResultType::qfrdreNumberVector: data stored in dvec [double]
              - evaluationResultType::qfrdreInteger: data stored in ivalue [int64_t]
              - evaluationResultType::qfrdreString: data stored in dvec [double]
              - evaluationResultType::qfrdreBoolean: data stored in bvalue [boolean]
              - evaluationResultType::qfrdreInvalid: no data stored (invalid result)
              - evaluationResultType::qfrdreNumberMatrix: data stored in dvec [double], in row-major order, number of columns is stored in columns
            .
         */
        struct evaluationResult {
            evaluationResult() {
                type=qfrdreInvalid;
                columns=0;
            }
            evaluationResultType type;  /**< type of data */
            double dvalue;  /**< double data value */
            double derror;  /**< error of double data */
            /*int64_t*/qlonglong ivalue;  /**< value as 64-bit signed integer */
            bool bvalue;  /**< value as boolean */
            QString svalue;  /**< value as string */
            QString unit; /**< unit name of the stored values */
            QVector<double> dvec;  /**< data as double vector */
            int columns;  /**< number of columns if matrix is stored in dvec (\c type \c == evaluationResultType::qfrdreNumberMatrix) */
            QString label;  /**< a label describing the result (no richtext markup!) */
            QString label_rich; /**< a label describing the result (with richtext markup!) */
            QString group; /**< this result belongs to a group with the given name */
        };
    protected:
        /*! \brief this struct holds the metadata and also the data about an evaluationID */
        struct evaluationIDMetadata {
            QString group; /**< group this evaluationID belongs to \b (optional) */
            int64_t groupIndex; /**< index of the results inside the evaluationID group set by \a group \b (optional) */
            QString description; /**< description of the metadata (human-readable version of the actual ID, \b optional )  */
            QMap<QString, evaluationResult> results; /**< the real results */
        };

        /** \brief evaluation results are stored in this QMap which maps an evaluation name to
         *         to a list of evaluation results (which is indexed by result names! */
        QMap<QString, evaluationIDMetadata > results;
        /** \brief table used to display the results */
        QFRDRResultsModel* resultsmodel;

        /** \brief is this is \c false, the signal resultsChanged() is NOT emited */
        bool doEmitResultsChanged;
    public:
        /** \brief clear all evaluation results */
        inline void resultsClearAll() {
            results.clear();
            if (doEmitResultsChanged) emit resultsChanged();
        };
        /** \brief clear all evaluation results of a specific evaluation name */
        inline void resultsClear(QString name) {
            if (results.contains(name)) {
                results[name].results.clear();
                results.remove(name);
                if (doEmitResultsChanged) emit resultsChanged();
            }
        };
        /** \brief clear all evaluation results of a specific evaluation name which contain a given \a postfix */
        virtual void resultsClear(QString name, QString postfix);
        /** \brief check whether a result exists */
        inline bool resultsExists(QString evalName, QString resultName) const {
            if (results.contains(evalName)) {
                return results[evalName].results.contains(resultName);
            }
            return false;
        };
        /** \brief check whether there are any results from a given evauation */
        inline bool resultsExistsFromEvaluation(QString evalName) const {
            return results.contains(evalName);
        };
        /** \brief set a result of type number */
        void resultsSetNumber(QString evaluationName, QString resultName, double value, QString unit=QString(""));
        /** \brief set a result of type number vector */
        void resultsSetNumberList(QString evaluationName, QString resultName, QVector<double>& value, QString unit=QString(""));
        /** \brief set a result of type number matrix */
        void resultsSetNumberMatrix(QString evaluationName, QString resultName, QVector<double>& value, int columns, QString unit=QString(""));
        /** \brief set a result of type number vector */
        void resultsSetNumberList(QString evaluationName, QString resultName, double* value, int items, QString unit=QString(""));
        /** \brief set a result of type number matrix */
        void resultsSetNumberMatrix(QString evaluationName, QString resultName, double* value, int columns, int rows, QString unit=QString(""));


        /** \brief set a result of type number+error */
        void resultsSetNumberError(QString evaluationName, QString resultName, double value, double error, QString unit=QString(""));
        /** \brief set a result of type integer */
        void resultsSetInteger(QString evaluationName, QString resultName, int64_t value, QString unit=QString(""));
        /** \brief set a result of type string */
        void resultsSetString(QString evaluationName, QString resultName, QString value);
        /** \brief set a result of type boolean */
        void resultsSetBoolean(QString evaluationName, QString resultName, bool value);
        /** \brief set result from evaluationResult */
        void resultsSet(QString evaluationName, QString resultName, const evaluationResult& result);
        /** \brief return a specified result as variant */
        inline evaluationResult resultsGet(QString evalName, QString resultName) const {
            evaluationResult r;
            if (resultsExists(evalName, resultName)) {
                r=results[evalName].results.value(resultName);
            }
            return r;
        };
        /** \brief return a specified result as string */
        QString  resultsGetAsString(QString evalName, QString resultName) const;
        /** \brief remove the value stored in the given position
         *
         *  If the results for the given \a evalName are empty after the delete, the entry for the
         *  \a evalName is also removed.
         *
         *  \param evalName the evaluation result set in which to search for the result to be deleted
         *  \param resultName the name of the result to be deleted in the evaluation result set \a evalName
         *  \param emitChangedSignal if set \c false (default is \c true ) this function will NOT emit a resultsChanged() signal.
         */
        void resultsRemove(QString evalName, QString resultName, bool emitChangedSignal=true);
        /*! \brief return a specified result as a QVariant

            The resulting QVariant conatins either a boolean (qfrdreBoolean), a QString (qfrdreString), an integer (qfrdreInteger),
            a double (qfrdreNumber), QPointF (qfrdreNumberError) or a QList<QVariant> (qfrdreNumberVector).
        */
        QVariant resultsGetAsQVariant(QString evalName, QString resultName) const;
        /** \brief return a specified result as double (or 0 if not possible!). If \a ok is supplied it will contain \c true if the conversion was possible and \c false otherwise. */
        double resultsGetAsDouble(QString evalName, QString resultName, bool* ok=NULL) const;
        /** \brief return a specified result as boolean (or \c false if not possible!). If \a ok is supplied it will contain \c true if the conversion was possible and \c false otherwise. */
        bool resultsGetAsBoolean(QString evalName, QString resultName, bool* ok=NULL) const;
        /** \brief return a specified result as integer (or 0 if not possible!). If \a ok is supplied it will contain \c true if the conversion was possible and \c false otherwise.  */
        int64_t resultsGetAsInteger(QString evalName, QString resultName, bool* ok=NULL) const;
        /** \brief return a specified result as double (or 0 if not possible!). If \a ok is supplied it will contain \c true if the conversion was possible and \c false otherwise.  */
        QVector<double> resultsGetAsDoubleList(QString evalName, QString resultName, bool* ok=NULL) const;
        /** \brief return a specified result's error as double (or 0 if not possible!). If \a ok is supplied it will contain \c true if the conversion was possible and \c false otherwise.  */
        double resultsGetErrorAsDouble(QString evalName, QString resultName, bool* ok=NULL) const;

        /** \brief set the label of a result (the result already has to exist!) */
        void resultsSetLabel(QString evaluationName, QString resultName, QString label, QString label_rich=QString(""));
        /** \brief return the label of a result (the non-richtext version!) */
        QString resultsGetLabel(QString evaluationName, QString resultName) const;
        /** \brief return the label of a result, potentially with richtext markup! */
        QString resultsGetLabelRichtext(QString evaluationName, QString resultName) const;

        /** \brief set the group of a result (the result already has to exist!) */
        void resultsSetGroup(QString evaluationName, QString resultName, QString group);
        /** \brief return the group of a result  */
        QString resultsGetGroup(QString evaluationName, QString resultName) const;

        /** \brief set the group of an evaluation ID */
        void resultsSetEvaluationGroup(QString evaluationName, QString group);
        /** \brief return the group of an evaluation ID, returns an empty string, if the evaluationName does not exist  */
        QString resultsGetEvaluationGroup(QString evaluationName) const;
        /** \brief set the group index of an evaluation ID */
        void resultsSetEvaluationGroupIndex(QString evaluationName, int64_t groupIndex);
        /** \brief return the group index of an evaluation ID, returns \c -1, if the evaluationName does not exist  */
        int64_t resultsGetEvaluationGroupIndex(QString evaluationName) const;
        /** \brief set the description of an evaluation ID */
        void resultsSetEvaluationDescription(QString evaluationName, QString description);
        /** \brief return the description of an evaluation ID, returns evaluationName, if no description is set, if the evaluationName does not exist  */
        QString resultsGetEvaluationDescription(QString evaluationName) const;


        /** \brief get number of results for a specified evaluation */
        inline int resultsGetCount(QString evalName) const {
            if (results.contains(evalName)) return results[evalName].results.size();
            return 0;
        };
        /** \brief get number of evaluations in this object */
        inline int resultsGetEvaluationCount() const {
            return results.size();
        };
        /** \brief get the i-th evaluation name */
        inline QString resultsGetEvaluationName(int i) const {
            if ((long)i<results.size()) return results.keys().at(i);
            return QString("");
        };
        /** \brief get the i-th result name */
        QString resultsGetResultName(QString evaluationName, int i) const;

        /*! \brief save a copy of all results with the given \a oldEvalName to the given \a newEvalName
            \param oldEvalName name of the evaluation results section to copy
            \param newEvalName name of the target evaluation results section
            \note if \a newEvalName already exists, the additional results will be added to \a newEvalName
         */
        void resultsCopy(QString oldEvalName, QString newEvalName);

        /*! \brief save the results to a CSV file

            saves a table of the results as Comma-Separated File \a filename. Not all result types may be exported,
            as this format does not support every dataformat. Any data that may be save as a single item is stored directly
            (qfrdreNumber, qfrdreInteger, qfrdreString, qfrdreBoolean). Numbers with errors (qfrdreNumberError) are also stored,
            If numbers in a column have errors then a second column is added for the errors.

            \param filename the file to create
            \param separator column separator in the file
            \param decinmalPoint which character to sue as decimal point
            \param stringDelimiter strings are surrounded by this character (one in front,one behind)
            \return \c true on success
        */
        bool resultsSaveToCSV(QString filename, QString separator=", ", QChar decimalPoint='.', QChar stringDelimiter='"');

        /*! \brief save the results to a SYLK file

            saves a table of the results as Comma-Separated File \a filename. Not all result types may be exported,
            as this format does not support every dataformat. Any data that may be save as a single item is stored directly
            (qfrdreNumber, qfrdreInteger, qfrdreString, qfrdreBoolean). Numbers with errors (qfrdreNumberError) are also stored,
            If numbers in a column have errors then a second column is added for the errors.

            \param filename the file to create
            \return \c true on success
        */
        bool resultsSaveToSYLK(QString filename);

        /** \brief return a table model which may be used to display the results */
        QFRDRResultsModel* resultsGetModel();

        /*! \brief return a list of all result names for this raw data record

            If \a evalName is non-empty, this restricts the list to results from the given evaluation \a evalName.
            If \a group is non-empty the resultant list is filtered by result groups.
            If \a evalgroup is non-empty the list is restricted to evaluations from the given evalGroup
         */
        QList<QString> resultsCalcNames(const QString& evalName=QString(""), const QString& group=QString(""), const QString& evalgroup=QString("")) const;

        /*! \brief return a list of all result labels (first field) for this raw data record, together ith the parameter IDs (second field)

            If \a evalName is non-empty, this restricts the list to results from the given evaluation \a evalName.
            If \a group is non-empty the resultant list is filtered by result groups.
            If \a evalgroup is non-empty the list is restricted to evaluations from the given evalGroup
         */
        QList<QPair<QString, QString> > resultsCalcNamesAndLabels(const QString& evalName=QString(""), const QString& group=QString(""), const QString& evalgroup=QString("")) const;

        /*! \brief return a list of all richtexted result labels (first field) for this raw data record, together ith the parameter IDs (second field)

            If \a evalName is non-empty, this restricts the list to results from the given evaluation \a evalName.
            If \a group is non-empty the resultant list is filtered by result groups.
            If \a evalgroup is non-empty the list is restricted to evaluations from the given evalGroup
         */
        QList<QPair<QString, QString> > resultsCalcNamesAndLabelsRichtext(const QString& evalName=QString(""), const QString& group=QString(""), const QString& evalgroup=QString("")) const;

        /*! \brief return a list of all result Groups, restricted to a given evaluation

            If \a evalName is non-empty, this restricts the list to results from the given evaluation \a evalName.
            If \a group is non-empty the resultant list is filtered by result groups.
         */
        QList<QString> resultsCalcGroups(const QString& evalName=QString("")) const;

        /*! \brief returns a list of evaluationIDs in a given evaluation group */
        QList<QString> resultsCalcEvaluationsInGroup(const QString& evalGroup) const;

        /*! \brief return a list of all evaluation groups in this object

            If \a paramgroup is non-empty the resultant list contains only thos groups that have parameters in the given paramgroup
         */
        QList<QString> resultsCalcEvalGroups(const QString& paramgroup=QString("")) const;

    public:
        /** \brief return type (short type string) */
        virtual QString getType() const { return tr("unknown"); };
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("unknown"); };
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/lib/projecttree_emptyrdr.png"); };
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr("unknown"); };
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/lib/projecttree_emptyrdr.png"); };
        /** \brief returns the number of additional editor panes for this record */
        virtual int getEditorCount() { return 0; };
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName(int i) { return QString(""); };
        /** \brief create an object for the i-th editor pane */
        virtual QFRawDataEditor* createEditor(QFPluginServices* services, int i=0, QWidget* parent=NULL) { return NULL; };
        /** \brief export the raw data into the specified format */
        virtual void exportData(const QString& format, const QString& filename) const {};
        /** \brief write object contents into XML file */
        void writeXML(QXmlStreamWriter& w);
        /** \brief returns a list of filetypes which correspond to the filetypes returned by getExportDialogFiletypes() */
        virtual QStringList getExportFiletypes() { return QStringList(); };
        /** \brief returns the title of the Export file dialog */
        virtual QString getExportDialogTitle() { return tr(""); };
        /** \brief returns the filetype of the Export file dialog */
        virtual QString getExportDialogFiletypes() { return tr(""); };

};



#endif // QFRAWDATARECORD_H
