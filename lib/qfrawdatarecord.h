#ifndef QFRAWDATARECORD_H
#define QFRAWDATARECORD_H
#include <QSettings>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QStringList>
#include <QHash>
#include <QIcon>
#include <QAbstractTableModel>
#include "qfproject.h"
#include "qfrawdataeditor.h"
#include "qfrdrresultsmodel.h"
#include "qfrdrpropertymodel.h"
#include "qfproperties.h"
#include "lib_imexport.h"
#include <stdint.h>


class QDomElement; // forward
class QFRawDataRecordPrivate; // forward

/*! \brief this class manages one raw data record in the project
    \ingroup qf3lib_project


    This class manages the basic properties of every raw data item in QuickFit 3.0.

    \section QFRawDataRecor_Signals Signals & Slots
    This class has some signals that can be emitted when fit properties or fit results change. When setting a whole
    bunch of results where each set() operation could emit a signal it may be wise to surround this portion
    of code by function calls that stop the emitting of these signals:
\code
    rdr->disableEmitResultsChanged();

    // ... YOUR CODE HERE ...

    rdr->enableEmitResultsChanged();
\endcode
    Of course you also have to make sure to use the methods emitResultsChanged() functions,
    instead of emiting the signals directly.

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
        void init(const QString& name=QString(""), QStringList inputFiles=QStringList(), QStringList inputFilesTypes=QStringList());
        /** \brief initialize from QDomElement */
        void init(QDomElement& e);


        /** \brief return the next sibling rawdata record in the project */
        inline QFRawDataRecord* getNext() { return project->getNextRawData(this); }
        /** \brief return the next sibling rawdata record in the project */
        inline QFRawDataRecord* getPrevious() { return project->getPreviousRawData(this); }
        /** \brief return the next sibling rawdata record in the project, which has the same type as this record */
        inline QFRawDataRecord* getNextOfSameType() { return project->getNextRawDataOfSameType(this); }
        /** \brief return the next sibling rawdata record in the project, which has the same type as this record */
        inline QFRawDataRecord* getPreviousOfSameType() { return project->getPreviousRawDataOfSameType(this); }

        /** \brief return the ID */
        inline int getID() const { return ID; }
        /** \brief return the name */
        inline QString getName() const { return name; }
        /** \brief return the description  */
        inline QString getDescription() const { return description; }
        /** \brief return the list of linked files */
        inline QStringList getFiles() const { return files; }
        /** \brief return the list of linked files */
        inline QStringList getFilesTypes() const { return files_types; }
        /** \brief return the filename associated with a given type, returns an empty string if no file was found */
        QString getFileForType(const QString& type);
        /** \brief return a list of filenames associated with a given type, returns an empty list if no files were found */
        QStringList getFilesForType(const QString& type);
        /** \brief return a pointer to the project that contains this QRawDatarecord */
        inline QFProject* getProject() const { return project; }
        /** \brief return the folder */
        inline QString getFolder() const { return folder; }


        /** \brief returns \c true if an error occured */
        inline bool error() { return errorOcc; }
        /** \brief returns the description of the last error */
        inline QString errorDescription() { return errorDesc; }



    public slots:
        /** \brief set the name */
        void setName(const QString& n);
        /** \brief set the folder */
        void setFolder(const QString& n);
        /** \brief set the description  */
        void setDescription(const QString& d);
        /** \brief returns a model which may be used to access and edit the properties in this object  */
        QFRDRPropertyModel* getPropertyModel();
    signals:
        /** \brief emitted whenever at least one of the properties changes */
        void propertiesChanged();
        /** \brief emitted whenever at least one of the results changes */
        void resultsChanged();
        /** \brief emitted whenever the data in this object changes */
        void rawDataChanged();
        /** \brief emitted whenever the folder changes */
        void folderChanged();
    public:
        /** \copydoc QFProperties::emitPropertiesChanged() */
        virtual void emitPropertiesChanged();
        /** \brief this function emits a resultsChanged() signal. */
        virtual void emitResultsChanged();
        /** \brief this function emits a rawDataChanged() signal. */
        virtual void emitRawDataChanged();
        /** \brief disable emitting of rawDataChanged() signal*/
        void disableEmitResultsChanged();
        /** \brief enable emitting of rawDataChanged() signal and emit one signal */
        void enableEmitResultsChanged(bool emitnow=true);
        /** \brief returns whether resultsChanged() signals are currently to be emitted */
        bool isEmitResultsChangedEnabled() const;
        /** \brief disable emitting of propertiesChanged() signal*/
        void disableEmitPropertiesChanged();
        /** \brief enable emitting of propertiesChanged() signal and emit one signal */
        void enableEmitPropertiesChanged(bool emitnow=true);
        /** \brief returns whether resultsChanged() signals are currently to be emitted */
        bool isEmitPropertiesChangedEnabled() const;
    protected:
        /** \copybrief QFProperties::setPropertiesError() */
        virtual void setPropertiesError(const QString& message) { setError(message); }

        /** \brief a model used to access the properties */
        QFRDRPropertyModel* propModel;
        /** \brief ID of the raw data record */
        int ID;
        /** \brief name of the raw data record */
        QString name;
        /** \brief description of the raw data record */
        QString description;
        /** \brief folder for grouing in prject tree */
        QString folder;
        /** \brief list of files connected with this record */
        QStringList files;
        /** \brief list of the types of the files connected with this record (see files) */
        QStringList files_types;
        /** \brief indicates whether an error has occured */
        bool errorOcc;
        /** \brief contains the description of the last error */
        QString errorDesc;
        /** \brief pointer to the parent project object */
        QFProject* project;

        /** \brief read object contents from QDomElement
         *  \see
         */
        void readXML(QDomElement& e);
        /** \brief write data contents to QXmlStreamWriter (data tag) <b>IMPLEMENT IN CHILD CLASSES!</b> */
        virtual void intWriteData(QXmlStreamWriter& w) {}
        /** \brief read in external data files <b>and</b> data stored in the project file <b>IMPLEMENT IN CHILD CLASSES!</b>
         *
         * If \a e is \c NULL then this method should only read the datafiles already saved in the files property.
         */
        virtual void intReadData(QDomElement* e=NULL) {}
        /** \brief set the internal error flag and description */
        void setError(const QString& description) { errorOcc=true; errorDesc=description; }


    public:
        /** \brief this enum contains possible types of evaluation results */
        enum evaluationResultType {
            qfrdreNumber,
            qfrdreNumberError,
            qfrdreNumberVector,
            qfrdreNumberMatrix,
            qfrdreNumberErrorVector,
            qfrdreNumberErrorMatrix,
            qfrdreInteger,
            qfrdreIntegerVector,
            qfrdreIntegerMatrix,
            qfrdreString,
            qfrdreStringVector,
            qfrdreStringMatrix,
            qfrdreBoolean,
            qfrdreBooleanVector,
            qfrdreBooleanMatrix,
            qfrdreInvalid
        };

        static QString evaluationResultType2String(evaluationResultType type);
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
                sortPriority=false;
                group="";
                label="";
                label_rich="";
                unit="";
                dvalue=0;
                derror=0;
                ivalue=0;
                bvalue=false;
                svalue="";
            }
            evaluationResultType type;  /**< type of data */
            double dvalue;  /**< double data value */
            double derror;  /**< error of double data */
            qlonglong ivalue;  /**< value as 64-bit signed integer */
            bool bvalue;  /**< value as boolean */
            QString svalue;  /**< value as string */
            QString unit; /**< unit name of the stored values */
            QVector<double> dvec;  /**< data as a double vector */
            QVector<double> evec;  /**< errors as a double vector */
            QVector<bool> bvec;  /**< data as boolean vector */
            QVector<qlonglong> ivec;  /**< data as an integer vector */
            QStringList svec; /**< data as a vector of strings */
            int columns;  /**< number of columns if matrix is stored in dvec (\c type \c == evaluationResultType::qfrdreNumberMatrix) */
            QString label;  /**< a label describing the result (no richtext markup!) */
            QString label_rich; /**< a label describing the result (with richtext markup!) */
            QString group; /**< this result belongs to a group with the given name */
            bool sortPriority; /**< this result should appear towards the start of tables */
        };

        /*! \brief initial size for results hash

            You can increase this if you expect that there will be very many items in your evaluation
         */
        void setResultsInitSize(int initSize);

        /*! \brief initial size for results hash in evaluationIDMetadata

            You can increase this if you expect that there will be very many items in your evaluation
         */
        void setEvaluationIDMetadataInitSize(int initSize);
    protected:
        /*! \brief initial size for results hash in evaluationIDMetadata

            You can increase this if you expect that there will be very many items in your evaluation
         */
        int evaluationIDMetadataInitSize;




        QFRawDataRecordPrivate* dstore;

        /** \brief table used to display the results */
        QFRDRResultsModel* resultsmodel;

        /** \brief is this is \c false, the signal resultsChanged() is NOT emited */
        bool doEmitResultsChanged;
        bool doEmitPropertiesChanged;
    public:
        /** \brief clear all evaluation results */
        void resultsClearAll();
        /** \brief clear all evaluation results of a specific evaluation name */
        void resultsClear(const QString& name);
        /** \brief clear all evaluation results of a specific evaluation name which contain a given \a postfix */
        void resultsClear(const QString& name, const QString& postfix);
        /** \brief check whether a result exists */
        bool resultsExists(const QString& evalName, const QString& resultName) const;
        /** \brief check whether there are any results from a given evauation */
        bool resultsExistsFromEvaluation(const QString& evalName) const;
        /** \brief set a result of type number */
        void resultsSetNumber(const QString& evaluationName, const QString& resultName, double value, const QString& unit=QString(""));

        /** \brief set a result of type number vector */
        void resultsSetNumberList(const QString& evaluationName, const QString& resultName, const QVector<double>& value, const QString& unit=QString(""));
        /** \brief set a result of type number matrix */
        void resultsSetNumberMatrix(const QString& evaluationName, const QString& resultName, const QVector<double>& value, int columns, const QString& unit=QString(""));
        /** \brief set a result of type number vector */
        void resultsSetNumberList(const QString& evaluationName, const QString& resultName, double* value, int items, const QString& unit=QString(""));
        /** \brief set a result of type number matrix */
        void resultsSetNumberMatrix(const QString& evaluationName, const QString& resultName, double* value, int columns, int rows, const QString& unit=QString(""));

        /** \brief set a result of type number vector */
        void resultsSetNumberErrorList(const QString& evaluationName, const QString& resultName, const QVector<double>& value, const QVector<double>& error, const QString& unit=QString(""));
        /** \brief set a result of type number matrix */
        void resultsSetNumberErrorMatrix(const QString& evaluationName, const QString& resultName, const QVector<double>& value, const QVector<double>& error, int columns, const QString& unit=QString(""));
        /** \brief set a result of type number vector */
        void resultsSetNumberErrorList(const QString& evaluationName, const QString& resultName, double* value, double* error, int items, const QString& unit=QString(""));
        /** \brief set a result of type number matrix */
        void resultsSetNumberErrorMatrix(const QString& evaluationName, const QString& resultName, double* value, double* error, int columns, int rows, const QString& unit=QString(""));

        /** \brief set a result element of type number vector. If more elements are needed in the vector, as available, these are initialized with 0 */
        void resultsSetInNumberList(const QString& evaluationName, const QString& resultName, int position, double value, const QString& unit=QString(""));
        /** \brief set a result element of type number matrix. If more elements are needed in the vector, as available, these are initialized with 0 */
        void resultsSetInNumberMatrix(const QString& evaluationName, const QString& resultName,  int row, int column, double value, const QString& unit=QString(""));
        /** \brief set a result element of type number vector. If more elements are needed in the vector, as available, these are initialized with 0 */
        void resultsSetInNumberErrorList(const QString& evaluationName, const QString& resultName, int position, double value, double error, const QString& unit=QString(""));
        /** \brief set a result error element of type number vector. If more elements are needed in the vector, as available, these are initialized with 0 */
        void resultsSetErrorInNumberErrorList(const QString& evaluationName, const QString& resultName, int position, double error);
        /** \brief set a result element of type number matrix. If more elements are needed in the vector, as available, these are initialized with 0 */
        void resultsSetInNumberErrorMatrix(const QString& evaluationName, const QString& resultName,  int row, int column, double value, double error, const QString& unit=QString(""));
        /** \brief set a result element of type integer vector. If more elements are needed in the vector, as available, these are initialized with 0 */
        void resultsSetInIntegerList(const QString& evaluationName, const QString& resultName, int position, qlonglong value, const QString& unit=QString(""));
        /** \brief set a result element of type integer matrix. If more elements are needed in the vector, as available, these are initialized with 0 */
        void resultsSetInIntegerMatrix(const QString& evaluationName, const QString& resultName,  int row, int column, qlonglong value, const QString& unit=QString(""));
        /** \brief set a result element of type boolean vector. If more elements are needed in the vector, as available, these are initialized with 0 */
        void resultsSetInBooleanList(const QString& evaluationName, const QString& resultName, int position, bool value, const QString& unit=QString(""));
        /** \brief set a result element of type boolean matrix. If more elements are needed in the vector, as available, these are initialized with 0 */
        void resultsSetInBooleanMatrix(const QString& evaluationName, const QString& resultName,  int row, int column, bool value, const QString& unit=QString(""));
        /** \brief set a result element of type string vector. If more elements are needed in the vector, as available, these are initialized with 0 */
        void resultsSetInStringList(const QString& evaluationName, const QString& resultName, int position, const QString& value, const QString& unit=QString(""));
        /** \brief set a result element of type string matrix. If more elements are needed in the vector, as available, these are initialized with 0 */
        void resultsSetInStringMatrix(const QString& evaluationName, const QString& resultName,  int row, int column, const QString& value, const QString& unit=QString(""));
        /** \brief reset an element any matrix type to a default value */
        void resultsResetInMatrix(const QString& evaluationName, const QString& resultName,  int row, int column);
        /** \brief reset an element any list type to a default value */
        void resultsResetInList(const QString& evaluationName, const QString& resultName,  int position);






        /** \brief returns an element from the given list, or a defaultValue, if the element does not exist */
        double resultsGetInNumberList(const QString& evaluationName, const QString& resultName, int position, double defaultValue=0);
        /** \brief returns an element from the given matrix, or a defaultValue, if the element does not exist */
        double resultsGetInNumberMatrix(const QString& evaluationName, const QString& resultName,  int row, int column, double defaultValue=0);
        /** \brief returns an element from the given list, or a defaultValue, if the element does not exist */
        double resultsGetErrorInNumberErrorList(const QString& evaluationName, const QString& resultName, int position, double defaultValue=0);
        /** \brief returns an element from the given matrix, or a defaultValue, if the element does not exist */
        double resultsGetErrorInNumberErrorMatrix(const QString& evaluationName, const QString& resultName,  int row, int column, double defaultValue=0);
        /** \brief returns an element from the given list, or a defaultValue, if the element does not exist */
        qlonglong resultsGetInIntegerList(const QString& evaluationName, const QString& resultName, int position, qlonglong defaultValue=0);
        /** \brief returns an element from the given matrix, or a defaultValue, if the element does not exist */
        qlonglong resultsGetInIntegerMatrix(const QString& evaluationName, const QString& resultName,  int row, int column, qlonglong defaultValue=0);
        /** \brief returns an element from the given list, or a defaultValue, if the element does not exist */
        bool resultsGetInBooleanList(const QString& evaluationName, const QString& resultName, int position, bool defaultValue=false);
        /** \brief returns an element from the given matrix, or a defaultValue, if the element does not exist */
        bool resultsGetInBooleanMatrix(const QString& evaluationName, const QString& resultName,  int row, int column, bool defaultValue=false);
        /** \brief returns an element from the given list, or a defaultValue, if the element does not exist */
        QString resultsGetInStringList(const QString& evaluationName, const QString& resultName, int position, const QString& defaultValue=QString(""));
        /** \brief returns an element from the given matrix, or a defaultValue, if the element does not exist */
        QString resultsGetInStringMatrix(const QString& evaluationName, const QString& resultName,  int row, int column, const QString& defaultValue=QString(""));






        /** \brief set a result of type integer vector */
        void resultsSetIntegerList(const QString& evaluationName, const QString& resultName, const QVector<qlonglong>& value, const QString& unit=QString(""));
        /** \brief set a result of type integer matrix */
        void resultsSetIntegerMatrix(const QString& evaluationName, const QString& resultName, const QVector<qlonglong>& value, int columns, const QString& unit=QString(""));
        /** \brief set a result of type integer vector */
        void resultsSetIntegerList(const QString& evaluationName, const QString& resultName, qlonglong* value, int items, const QString& unit=QString(""));
        /** \brief set a result of type integer matrix */
        void resultsSetIntegerMatrix(const QString& evaluationName, const QString& resultName, qlonglong* value, int columns, int rows, const QString& unit=QString(""));

        /** \brief set a result of type bool vector */
        void resultsSetBooleanList(const QString& evaluationName, const QString& resultName, const QVector<bool>& value, const QString& unit=QString(""));
        /** \brief set a result of type bool matrix */
        void resultsSetBooleanMatrix(const QString& evaluationName, const QString& resultName, const QVector<bool>& value, int columns, const QString& unit=QString(""));
        /** \brief set a result of type bool vector */
        void resultsSetBooleanList(const QString& evaluationName, const QString& resultName, bool* value, int items, const QString& unit=QString(""));
        /** \brief set a result of type bool matrix */
        void resultsSetBooleanMatrix(const QString& evaluationName, const QString& resultName, bool* value, int columns, int rows, const QString& unit=QString(""));

        /** \brief set a result of type string vector */
        void resultsSetStringList(const QString& evaluationName, const QString& resultName, const QVector<QString>& value, const QString& unit=QString(""));
        /** \brief set a result of type string matrix */
        void resultsSetStringMatrix(const QString& evaluationName, const QString& resultName, const QVector<QString>& value, int columns, const QString& unit=QString(""));


        /** \brief set a result of type number+error */
        void resultsSetNumberError(const QString& evaluationName, const QString& resultName, double value, double error, const QString& unit=QString(""));
        /** \brief set error of a result of type number+error */
        void resultsSetNumberErrorError(const QString& evaluationName, const QString& resultName, double error);
        /** \brief set a result of type integer */
        void resultsSetInteger(const QString& evaluationName, const QString& resultName, int64_t value, const QString& unit=QString(""));
        /** \brief set a result of type string */
        void resultsSetString(const QString& evaluationName, const QString& resultName, const QString& value);
        /** \brief set a result of type boolean */
        void resultsSetBoolean(const QString& evaluationName, const QString& resultName, bool value);
        /** \brief set result from evaluationResult */
        void resultsSet(const QString& evaluationName, const QString& resultName, const evaluationResult& result);
        /** \brief return a specified result as variant */
        evaluationResult resultsGet(const QString& evalName, const QString& resultName) const;
        /** \brief return the type of a specified result, or qfrdreInvalid if an error occured (eval does not exist ...) */
        evaluationResultType resultsGetType(const QString& evalName, const QString& resultName) const;
        /** \brief remove the value stored in the given position
         *
         *  If the results for the given \a evalName are empty after the delete, the entry for the
         *  \a evalName is also removed.
         *
         *  \param evalName the evaluation result set in which to search for the result to be deleted
         *  \param resultName the name of the result to be deleted in the evaluation result set \a evalName
         *  \param emitChangedSignal if set \c false (default is \c true ) this function will NOT emit a resultsChanged() signal.
         */
        void resultsRemove(const QString& evalName, const QString& resultName, bool emitChangedSignal=true);
        /** \brief return a specified result as string */
        QString  resultsGetAsString(const QString& evalName, const QString& resultName) const;
        /** \brief return a specified result as string, but returns as a QVariant. If the result is invalid, a QVariant() will be returned. */
        QVariant  resultsGetAsStringVariant(const QString& evalName, const QString& resultName) const;
        /*! \brief return a specified result as a QVariant

            The resulting QVariant conatins either a boolean (qfrdreBoolean), a QString (qfrdreString), an integer (qfrdreInteger),
            a double (qfrdreNumber), QPointF (qfrdreNumberError) or a QList<QVariant> (qfrdreNumberVector).
        */
        QVariant resultsGetAsQVariant(const QString& evalName, const QString& resultName) const;
        /** \brief return a specified result as double (or 0 if not possible!). If \a ok is supplied it will contain \c true if the conversion was possible and \c false otherwise. */
        double resultsGetAsDouble(const QString& evalName, const QString& resultName, bool* ok=NULL) const;
        /** \brief return a specified result's error as double vector (or 0 if not possible!). If \a ok is supplied it will contain \c true if the conversion was possible and \c false otherwise.  */
        double resultsGetErrorAsDouble(const QString& evalName, const QString& resultName, bool* ok=NULL) const;
        /** \brief return a specified result as boolean (or \c false if not possible!). If \a ok is supplied it will contain \c true if the conversion was possible and \c false otherwise. */
        bool resultsGetAsBoolean(const QString& evalName, const QString& resultName, bool* ok=NULL) const;
        /** \brief return a specified result as integer (or 0 if not possible!). If \a ok is supplied it will contain \c true if the conversion was possible and \c false otherwise.  */
        int64_t resultsGetAsInteger(const QString& evalName, const QString& resultName, bool* ok=NULL) const;
        /** \brief return a specified result as double vector (or 0 if not possible!). If \a ok is supplied it will contain \c true if the conversion was possible and \c false otherwise.  */
        QVector<double> resultsGetAsDoubleList(const QString& evalName, const QString& resultName, bool* ok=NULL) const;
        /** \brief return a specified result's error as double (or 0 if not possible!). If \a ok is supplied it will contain \c true if the conversion was possible and \c false otherwise.  */
        QVector<double> resultsGetErrorAsDoubleList(const QString& evalName, const QString& resultName, bool* ok=NULL) const;
        /** \brief return a specified result as integer vector (or 0 if not possible!). If \a ok is supplied it will contain \c true if the conversion was possible and \c false otherwise.  */
        QVector<qlonglong> resultsGetAsIntegerList(const QString& evalName, const QString& resultName, bool* ok=NULL) const;
        /** \brief return a specified result as bool vector (or 0 if not possible!). If \a ok is supplied it will contain \c true if the conversion was possible and \c false otherwise.  */
        QVector<bool> resultsGetAsBooleanList(const QString& evalName, const QString& resultName, bool* ok=NULL) const;





        /** \brief return a specified result as string, given a certain position in a vector/matrix datatype
         *
         *  If the parameter \a alsoGetNonVec is \c true , the function will return the value of the single string result if the result is no
         *  vector or matrix otherwise an empty result is returned.
         */
        QString  resultsGetAsString(const QString& evalName, const QString& resultName, int position, bool alsoGetNonVec=false) const;
        /** \brief return a specified result as string, but returns as a QVariant, given a certain position in a vector/matrix datatype. If the result is invalid, a QVariant() will be returned.
         *
         *  If the parameter \a alsoGetNonVec is \c true , the function will return the value of the single string result if the result is no
         *  vector or matrix otherwise an empty result is returned.
         */
        QVariant  resultsGetAsStringVariant(const QString& evalName, const QString& resultName, int position, bool alsoGetNonVec=false) const;
        /*! \brief return a specified result as a QVariant, given a certain position in a vector/matrix datatype

            The resulting QVariant conatins either a boolean (qfrdreBoolean), a QString (qfrdreString), an integer (qfrdreInteger),
            a double (qfrdreNumber), QPointF (qfrdreNumberError and qfrdreNumberErrorVector) or a QList<QVariant> (for all list/matrix data).


            If the parameter \a alsoGetNonVec is \c true , the function will return the value of the single result if the result is no
            vector or matrix otherwise an invalid result is returned.

        */
        QVariant resultsGetAsQVariant(const QString& evalName, const QString& resultName, int position, bool alsoGetNonVec=false) const;
        /** \brief return a specified result as double (or 0 if not possible!). If \a ok is supplied it will contain \c true if the conversion was possible and \c false otherwise.
         *
         *  If the parameter \a alsoGetNonVec is \c true , the function will return the value of the single double result if the result is no
         *  vector or matrix otherwise a 0 result is returned.
         */
        double resultsGetAsDouble(const QString& evalName, const QString& resultName, int position, bool* ok=NULL, bool alsoGetNonVec=false) const;
        /** \brief return a specified result's error as double vector (or 0 if not possible!). If \a ok is supplied it will contain \c true if the conversion was possible and \c false otherwise.
         *
         *  If the parameter \a alsoGetNonVec is \c true , the function will return the value of the single error result if the result is no
         *  vector or matrix otherwise a 0 result is returned.
         */
        double resultsGetErrorAsDouble(const QString& evalName, const QString& resultName, int position, bool* ok=NULL, bool alsoGetNonVec=false) const;
        /** \brief return a specified result as boolean (or \c false if not possible!). If \a ok is supplied it will contain \c true if the conversion was possible and \c false otherwise.
         *
         *  If the parameter \a alsoGetNonVec is \c true , the function will return the value of the single boolean result if the result is no
         *  vector or matrix otherwise a false result is returned.
         */
        bool resultsGetAsBoolean(const QString& evalName, const QString& resultName, int position, bool* ok=NULL, bool alsoGetNonVec=false) const;
        /** \brief return a specified result as integer (or 0 if not possible!). If \a ok is supplied it will contain \c true if the conversion was possible and \c false otherwise.
         *
         *  If the parameter \a alsoGetNonVec is \c true , the function will return the value of the single integer result if the result is no
         *  vector or matrix otherwise a 0 result is returned.
         */
        int64_t resultsGetAsInteger(const QString& evalName, const QString& resultName, int position, bool* ok=NULL, bool alsoGetNonVec=false) const;




        /** \brief set the label of a result (the result already has to exist!) */
        void resultsSetLabel(const QString& evaluationName, const QString& resultName, const QString& label, const QString& label_rich=QString(""));
        /** \brief return the label of a result (the non-richtext version!) */
        QString resultsGetLabel(const QString& evaluationName, const QString& resultName) const;
        /** \brief return the label of a result, potentially with richtext markup! */
        QString resultsGetLabelRichtext(const QString& evaluationName, const QString& resultName) const;

        /** \brief set the group of a result (the result already has to exist!) */
        void resultsSetGroup(const QString& evaluationName, const QString& resultName, const QString& group);
        /** \brief return the group of a result  */
        QString resultsGetGroup(const QString& evaluationName, const QString& resultName) const;

        /** \brief set the sort priority of a result (the result already has to exist!) */
        void resultsSetSortPriority(const QString& evaluationName, const QString& resultName, bool pr);
        /** \brief return the sort priority of a result  */
        bool resultsGetSortPriority(const QString& evaluationName, const QString& resultName) const;

        /** \brief set the human-readable label for a group name used as evalGroup  */
        void resultsSetEvaluationGroupLabel(const QString& evalGroup, const QString& label);
        /** \brief return the human-readable label of an evaluation group
         *
         *  This function returns the argument \a evalGroup if a label is not saved!
         */
        QString resultsGetLabelForEvaluationGroup(const QString& evalGroup) const;
        /** \brief return the group of an evaluation ID, returns an empty string, if the evaluationName does not exist  */
        QString resultsGetEvaluationGroup(const QString& evaluationName) const;
        /** \brief return the group of an evaluation ID as human-readable label
         *
         *   returns an empty string, if the evaluationName does not exist and the group itself, if no label exists.
         */
        QString resultsGetEvaluationGroupLabel(const QString& evaluationName) const;
        /** \brief set the group of an evaluation ID */
        void resultsSetEvaluationGroup(const QString& evaluationName, const QString& group);
        /** \brief set the group index of an evaluation ID */
        void resultsSetEvaluationGroupIndex(const QString& evaluationName, int64_t groupIndex);
        /** \brief return the group index of an evaluation ID, returns \c -1, if the evaluationName does not exist  */
        int64_t resultsGetEvaluationGroupIndex(const QString& evaluationName) const;
        /** \brief set the description of an evaluation ID */
        void resultsSetEvaluationDescription(const QString& evaluationName, const QString& description);
        /** \brief return the description of an evaluation ID, returns evaluationName, if no description is set, if the evaluationName does not exist  */
        QString resultsGetEvaluationDescription(const QString& evaluationName) const;


        /** \brief get number of results for a specified evaluation */
        int resultsGetCount(const QString& evalName) const;
        /** \brief get number of evaluations in this object */
        int resultsGetEvaluationCount() const;
        /** \brief get the i-th evaluation name */
        QString resultsGetEvaluationName(int i) const;
        /** \brief get the i-th result name */
        QString resultsGetResultName(const QString& evaluationName, int i) const;

        /*! \brief save a copy of all results with the given \a oldEvalName to the given \a newEvalName
            \param oldEvalName name of the evaluation results section to copy
            \param newEvalName name of the target evaluation results section
            \note if \a newEvalName already exists, the additional results will be added to \a newEvalName
         */
        void resultsCopy(const QString& oldEvalName, const QString& newEvalName);

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
        bool resultsSaveToCSV(const QString& filename, const QString& separator=", ", QChar decimalPoint='.', QChar stringDelimiter='"');

        /*! \brief save the results to a SYLK file

            saves a table of the results as Comma-Separated File \a filename. Not all result types may be exported,
            as this format does not support every dataformat. Any data that may be save as a single item is stored directly
            (qfrdreNumber, qfrdreInteger, qfrdreString, qfrdreBoolean). Numbers with errors (qfrdreNumberError) are also stored,
            If numbers in a column have errors then a second column is added for the errors.

            \param filename the file to create
            \return \c true on success
        */
        bool resultsSaveToSYLK(const QString& filename);

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
