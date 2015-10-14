/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
#include <QReadLocker>
#include <QWriteLocker>
#include <QReadWriteLock>
#include "qfmathtools.h"
#include "qfrdrcurvesinterface.h"


class QDomElement; // forward
class QFRawDataRecordPrivate; // forward
struct qfmpResult; // forward



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
class QFLIB_EXPORT QFRawDataRecord : public QObject, public QFProperties, public QFRDRCurvesInterface {
        Q_OBJECT
        Q_INTERFACES(QFRDRCurvesInterface)
    public:
        /** \brief class constructor, reads from QDomElement */
        QFRawDataRecord(QFProject* parent);
        /** \brief Default destructor */
        virtual ~QFRawDataRecord();

        /** \brief initialize the object with the given data */
        void init(const QString& name=QString(""), QStringList inputFiles=QStringList(), QStringList inputFilesTypes=QStringList(), QStringList inputFileDescriptions=QStringList());
        /** \brief initialize from QDomElement, if \a loadAsDummy is \c true this method won't load the data in the record, but only the common descrption of the record.
         *
         * The property \a loadAsSummy is used by QFProject::readXMLDummy() in order to build a dummy project tree. For more information \see QFProject::readXMLDummy()
         *
         * \note The record ID is read from the XML dat!
         */
        void init(QDomElement& e, bool loadAsDummy=false);
        /** \brief initialize from QDomElement
         *
         * \note This method retrieves a new record ID from the project, the ID in the XML-record is ignored!
         */
        void initNewID(QDomElement& e);


        /** \brief return the next sibling rawdata record in the project */
        inline QFRawDataRecord* getNext() { return project->getNextRawData(this); }
        /** \brief return the next sibling rawdata record in the project */
        inline QFRawDataRecord* getPrevious() { return project->getPreviousRawData(this); }
        /** \brief return the next sibling rawdata record in the project, which has the same type as this record */
        inline QFRawDataRecord* getNextOfSameType() { return project->getNextRawDataOfSameType(this); }
        /** \brief return the next sibling rawdata record in the project, which has the same type as this record */
        inline QFRawDataRecord* getPreviousOfSameType() { return project->getPreviousRawDataOfSameType(this); }

        /** \brief return the ID */
         int getID() const;
        /** \brief return the name */
         QString getName() const;
        /** \brief return the role */
         QString getRole() const;
        /** \brief return the group */
         int getGroup() const;
        /** \brief returns \c true if this RDR has a group */
        bool hasGroup() const;
        /** \brief returns the name of the group of the current record, or an empty string if no group is given  */
        QString getGroupName() const;
        /** \brief returns all members (including this record) of the records group */
        QList<QFRawDataRecord*> getGroupMembers() const;
        /** \brief get all records with a given role from the group of the current record */
        QList<QFRawDataRecord*> getRecordsWithRoleFromGroup(const QString& role) const;
        /** \brief return the description  */
         QString getDescription() const;
        /** \brief return the list of linked files */
         QStringList getFiles() const;
        /** \brief return the list of linked files */
         QStringList getFilesTypes() const;
        /** \brief return the list of linked files */
         QStringList getFilesDescriptions() const;
        /** \brief return the filename associated with a given type, returns an empty string if no file was found */
        QString getFileForType(const QString& type);
        /** \brief return a list of filenames associated with a given type, returns an empty list if no files were found */
        QStringList getFilesForType(const QString& type) const;
        /** \brief return a pointer to the project that contains this QRawDatarecord */
        inline QFProject* getProject() const { return project; }
        /** \brief return the folder */
         QString getFolder() const;


        /** \brief returns \c true if an error occured */
         bool error() const;
        /** \brief returns the description of the last error */
         QString errorDescription() const;

        /** \brief return the path to i-th file */
        QString getFileName(int i) const;
        /** \brief return type of i-th file */
        QString getFileType(int i) const;
        /** \brief return description of i-th file */
        QString getFileDescription(int i) const;
        /** \brief return number of files */
        int getFilesCount() const;
        /** \brief set name of i-th file
         *
         *  \warning Use this function with great care, as it may break the record ... changes might take effect only after saving and reloading the project.
         */
        void setFileName(int i, const QString& file);
        /** \brief set type of i-th file
         *
         *  \warning Use this function with great care, as it may break the record ... changes might take effect only after saving and reloading the project.
         *  \note Only use this function, when you overwrote isFilesListEditable() to return FilesEditable flag. and try to call reloadFromFiles() after editing the files list.
         *        This function does not internally call reloadFromFiles(), so you can do more changes before reloading!
         */
        void setFileType(int i, const QString& type);
        /** \brief set description of i-th file
         *
         *  \warning Use this function with great care, as it may break the record ... changes might take effect only after saving and reloading the project.
         *  \note Only use this function, when you overwrote isFilesListEditable() to return FilesEditable flag. and try to call reloadFromFiles() after editing the files list.
         *        This function does not internally call reloadFromFiles(), so you can do more changes before reloading!
         */
        void setFileDecsription(int i, const QString& description);
        /** \brief add a new file
         *
         *  \warning Use this function with great care, as it may break the record ... changes might take effect only after saving and reloading the project.
         *  \note Only use this function, when you overwrote isFilesListEditable() to return FilesEditable flag. and try to call reloadFromFiles() after editing the files list.
         *        This function does not internally call reloadFromFiles(), so you can do more changes before reloading!
         */
        void addFile(const QString& file, const QString& type, const QString& description);
        /** \brief delete the i-th file
         *
         *  \warning Use this function with great care, as it may break the record ... changes might take effect only after saving and reloading the project.
         *  \note Only use this function, when you overwrote isFilesListEditable() to return FilesEditable flag. and try to call reloadFromFiles() after editing the files list.
         *        This function does not internally call reloadFromFiles(), so you can do more changes before reloading!
         */
        void deleteFile(int i);

        /** \brief move the i-th file one position up in the list
         *
         *  \warning Use this function with great care, as it may break the record ... changes might take effect only after saving and reloading the project.
         *  \note Only use this function, when you overwrote isFilesListEditable() to return FilesEditable flag. and try to call reloadFromFiles() after editing the files list.
         *        This function does not internally call reloadFromFiles(), so you can do more changes before reloading!
         */
        void moveFileUp(int i);
        /** \brief move the i-th file one position down in the list
         *
         *  \warning Use this function with great care, as it may break the record ... changes might take effect only after saving and reloading the project.
         *  \note Only use this function, when you overwrote isFilesListEditable() to return FilesEditable flag. and try to call reloadFromFiles() after editing the files list.
         *        This function does not internally call reloadFromFiles(), so you can do more changes before reloading!
         */
        void moveFileDown(int i);
        /** \brief move the specified files one position up in the list
         *
         *  \warning Use this function with great care, as it may break the record ... changes might take effect only after saving and reloading the project.
         *  \note Only use this function, when you overwrote isFilesListEditable() to return FilesEditable flag. and try to call reloadFromFiles() after editing the files list.
         *        This function does not internally call reloadFromFiles(), so you can do more changes before reloading!
         */
        void moveFilesUp(const QList<int>& list);
        /** \brief move the specified files one position down in the list
         *
         *  \warning Use this function with great care, as it may break the record ... changes might take effect only after saving and reloading the project.
         *  \note Only use this function, when you overwrote isFilesListEditable() to return FilesEditable flag. and try to call reloadFromFiles() after editing the files list.
         *        This function does not internally call reloadFromFiles(), so you can do more changes before reloading!
         */
        void moveFilesDown(const QList<int>& list);




    public slots:
        /** \brief set the name */
        void setName(const QString& n);
        /** \brief set the folder */
        void setFolder(const QString& n);
        /** \brief set the RDRs role */
        void setRole(const QString& n);
        /** \brief set the RDRs role */
        void setGroup(int g);

        /** \brief set the description  */
        void setDescription(const QString& d);
        /** \brief returns a model which may be used to access and edit the properties in this object  */
        QFRDRPropertyModel* getPropertyModel();

        void log_text(const QString& message) const;
        void log_warning(const QString& message) const;
        void log_error(const QString& message) const;

    signals:
        /** \brief emitted whenever at least one of the properties changes */
        void propertiesChanged(QString property, bool visible);
        /** \brief basic properties changed (i.e. name, description and folder) */
        void basicPropertiesChanged();
        /*! \brief emitted whenever at least one of the results changes

            The function arguments specify what changed:
            <table>
              <tr><th>\a evalName</th><th>\a resultName</th><th>meaning</th></tr>
              <tr><th>set</th><th>set</th><td>the property resultName in the evaluation evalName changed</td></tr>
              <tr><th>set</th><th>unset</th><td>something changed in the evaluation evalName</td></tr>
              <tr><th>unset</th><th>unset</th><td>any result changed</td></tr>
            </table>

         */
        void resultsChanged(const QString& evalName, const QString& resultName, bool removed);
        void compoundsChanged(const QString& evalName, const QString& compoundName, bool removed);
        /** \brief emitted whenever the data in this object changes */
        void rawDataChanged();
        /** \brief emitted whenever the folder changes */
        void folderChanged();
    public:
        /** \copydoc QFProperties::emitPropertiesChanged() */
        virtual void emitPropertiesChanged(const QString& property=QString(""), bool visible=true);
        /** \brief this function emits a resultsChanged() signal. */
        virtual void emitResultsChanged(const QString& evalName=QString(""), const QString& resultName=QString(""), bool removed=false);
        /** \brief this function emits a compoundsChanged() signal. */
        virtual void emitCompoundsChanged(const QString& evalName=QString(""), const QString& compoundName=QString(""), bool removed=false);
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

        /** \brief returns a list of the allowed RDR roles, default: an emplty list */
        virtual QStringList getAvailableRoles() const;
        /** \brief if this returns \c true, the user may change the RDRs role in the QFRawDataPropertyEditor, default: \c false */
        virtual bool isRoleUserEditable() const;

        /** \brief indicates, whether the next of same role/previous of same role buttons should be displayed */
        virtual bool showNextPreviousOfSameRoleButton() const;



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
        /*! \brief the role of the current record (basically a string that may be interpreted any way
                   by a QFRawDataRecord implementation to distinguish betwen different applications of
                   the same RDR, e.g. an FCS-record may contain autocorrelations or crosscorrelations

            The role string is not free, each class may implement getAvailableRoles() which returns a list of the allowed roles.
            Implementing areRolesUserChangeable() to return \c true will allow the user to change a files role in the QFRawDataPropertyEditor
        */
        QString role;
        /** \brief list of files connected with this record */
        QStringList files;
        /** \brief list of the types of the files connected with this record (see files) */
        QStringList files_types;
        /** \brief list of the descriptions for the files connected with this record (see files) */
        QStringList files_desciptions;
        /** \brief indicates whether an error has occured */
        bool errorOcc;
        /** \brief contains the description of the last error */
        QString errorDesc;
        /** \brief pointer to the parent project object */
        QFProject* project;

        /** \brief group ID (or -1 of none) */
        int group;

        /** \brief read object contents from QDomElement
         *
         * If the property \a loadAsDummy is \c true this method will only read the basic properties from XML, i.e. name, description,
         * id, ... but not the results and not the actual data. This function can be used to build a dummy project tree.
         *  \see QFProject::readXMLDummy() for more information on the property \a loadAsDummy
         */
        void readXML(QDomElement& e, bool loadAsDummy=false, bool readID=true);
        /** \brief write data contents to QXmlStreamWriter (data tag) <b>IMPLEMENT IN CHILD CLASSES!</b> */
        virtual void intWriteData(QXmlStreamWriter& w) const {Q_UNUSED(w);}
        /** \brief read in external data files <b>and</b> data stored in the project file <b>IMPLEMENT IN CHILD CLASSES!</b>
         *
         * If \a e is \c NULL then this method should only read the datafiles already saved in the files property.
         */
        virtual void intReadData(QDomElement* e=NULL) {Q_UNUSED(e);}
        /** \brief set the internal error flag and description */
        void setError(const QString& description) { errorOcc=true; errorDesc=description; }


    public:
        /** \brief this enum contains possible types of evaluation results */
        enum evaluationResultType {
            qfrdreNumber,                   /*!< a single number (float)  */
            qfrdreNumberError,              /*!< a single number + its error (float)  */
            qfrdreNumberVector,             /*!< an arbitrarily sized vector of numbers (float)  */
            qfrdreNumberMatrix,             /*!< an arbitrarily sized matrix of numbers (float)  */
            qfrdreNumberErrorVector,        /*!< an arbitrarily sized vector of numbers+errors (float)  */
            qfrdreNumberErrorMatrix,        /*!< an arbitrarily sized matrix of numbers+errors (float)  */
            qfrdreInteger,                  /*!< a single integer number  */
            qfrdreIntegerVector,            /*!< an arbitrarily sized vector of integer numbers  */
            qfrdreIntegerMatrix,            /*!< an arbitrarily sized matrix of integer numbers  */
            qfrdreString,                   /*!< a single string  */
            qfrdreStringVector,             /*!< an arbitrarily sized vector of strings  */
            qfrdreStringMatrix,             /*!< an arbitrarily sized matrix of strings  */
            qfrdreBoolean,                  /*!< a single boolean value (true/false)  */
            qfrdreBooleanVector,            /*!< an arbitrarily sized vector of boolean values  */
            qfrdreBooleanMatrix,            /*!< an arbitrarily sized matrix of boolean values  */
            qfrdreInvalid                   /*!< an invalid result (none of the above, i.e. no data contained in this result)  */
        };

        static QString evaluationResultType2String(evaluationResultType type);
        static bool evaluationResultTypeIsNumeric(evaluationResultType type);
        static bool evaluationResultTypeIsBoolean(evaluationResultType type);
        static bool evaluationResultTypeIsString(evaluationResultType type);
        static bool evaluationResultTypeIsVector(evaluationResultType type);
        static bool evaluationResultTypeHasError(evaluationResultType type);
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
        struct QFLIB_EXPORT evaluationResult {
            inline evaluationResult() {
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

            QFLIB_EXPORT int getVectorMatrixItems() const;
            QFLIB_EXPORT bool isNumberType() const;
            QFLIB_EXPORT bool isVectorMatrixType() const;
            QFLIB_EXPORT double getAsDouble() const;
            QFLIB_EXPORT QVector<double> getAsDoubleVector() const;
            QFLIB_EXPORT qfmpResult getAsMathParserResult() const;
            QFLIB_EXPORT QVariant getAsVariant() const;
            QFLIB_EXPORT void setFromMathParserResult(const qfmpResult& result);

            inline void resultsSetNumber(double value,  const QString& unit=QString(""),  const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                this->type=qfrdreNumber;
                this->dvalue=value;
                this->unit=unit;
                this->group=group;
                this->label=label;
                this->label_rich=label_rich;
                this->sortPriority=sortPriority;
            }

            inline void resultsSetNumberError(double value, double error,  const QString& unit=QString(""),  const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                this->type=qfrdreNumberError;
                this->dvalue=value;
                this->derror=error;
                this->unit=unit;
                this->group=group;
                this->label=label;
                this->label_rich=label_rich;
                this->sortPriority=sortPriority;
            }
            inline void resultsSetInteger(qlonglong value,  const QString& unit=QString(""),  const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                this->type=qfrdreInteger;
                this->ivalue=value;
                this->unit=unit;
                this->group=group;
                this->label=label;
                this->label_rich=label_rich;
                this->sortPriority=sortPriority;
            }

            inline void resultsSetBoolean(bool value,  const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                this->type=qfrdreBoolean;
                this->bvalue=value;
                this->unit="";
                this->group=group;
                this->label=label;
                this->label_rich=label_rich;
                this->sortPriority=sortPriority;
            }
            inline void resultsSetString(const QString& value, const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                this->type=qfrdreString;
                this->svalue=value;
                this->unit="";
                this->group=group;
                this->label=label;
                this->label_rich=label_rich;
                this->sortPriority=sortPriority;
            }





            inline void resultsSetNumberVector(const QVector<double>& value,  const QString& unit=QString(""),  const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                this->type=qfrdreNumberVector;
                this->dvec=value;
                this->unit=unit;
                this->group=group;
                this->label=label;
                this->label_rich=label_rich;
                this->sortPriority=sortPriority;
            }

            inline void resultsSetNumberErrorVector(const QVector<double>& value, const QVector<double>& error,  const QString& unit=QString(""),  const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                this->type=qfrdreNumberError;
                this->dvec=value;
                this->evec=error;
                this->unit=unit;
                this->group=group;
                this->label=label;
                this->label_rich=label_rich;
                this->sortPriority=sortPriority;
            }
            inline void resultsSetIntegerVector(const QVector<qlonglong> value,  const QString& unit=QString(""),  const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                this->type=qfrdreInteger;
                this->ivec=value;
                this->unit=unit;
                this->group=group;
                this->label=label;
                this->label_rich=label_rich;
                this->sortPriority=sortPriority;
            }

            inline void resultsSetBooleanVector(const QVector<bool> value,  const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                this->type=qfrdreBoolean;
                this->bvec=value;
                this->unit="";
                this->group=group;
                this->label=label;
                this->label_rich=label_rich;
                this->sortPriority=sortPriority;
            }
            inline void resultsSetStringVector(const QStringList& value, const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                this->type=qfrdreString;
                this->svec=value;
                this->unit="";
                this->group=group;
                this->label=label;
                this->label_rich=label_rich;
                this->sortPriority=sortPriority;
            }





            inline void resultsSetLabel(const QString& label, const QString& label_rich=QString("")) {
                this->label=label;
                this->label_rich=label_rich;
            }

            inline void resultsSetGroup(const QString& group) {
                this->group=group;
            }

            inline void resultsSetGroupAndLabels(const QString& group, const QString& label, const QString& label_rich=QString("")) {
                this->group=group;
                this->label=label;
                this->label_rich=label_rich;
            }

            inline void resultsSetGroupLabelsAndSortPriority(const QString& group, const QString& label, const QString& label_rich, bool pr)  {
                this->group=group;
                this->label=label;
                this->label_rich=label_rich;
                this->sortPriority=pr;
            }
            inline void resultsSetSortPriority(bool pr) {
                this->sortPriority=pr;
            }

        };

        enum evaluationCompoundTypes {
            qfrdrctGraph1D,
            qfrdrctInvalid
        };

        /*! \brief this struct allows to combine evaluationResult records in the result set into compounds, such as graphs

            These compound types are available:
              - evaluationCompoundTypes::qfrdrctGraph1D: a 1D-graph, which combines two vector-of-number (optionally + error) results. Metadata may be:
                  - \c logX , \c logY (boolean) for log-axis
                  - \c labelX , \c labelY (string) axis labels
                .
            .

            For compounds of type qfrdrctGraph1D, the evaluationCompoundResult::referencedResults has to contain at least two entries for x- and y-data.
            In addition, two more entries can be given for x- and y-errors, then in the order \c (x,y,xerror,yerror).
         */
        struct QFLIB_EXPORT evaluationCompoundResult {
                inline evaluationCompoundResult() {
                    type=qfrdrctInvalid;
                    label="";
                    metadata.clear();
                    referencedResults.clear();
                }

                /** \brief type of the compond */
                evaluationCompoundTypes type;
                /** \brief list of results, referenced by this compound type, e.g. the x-axis and y-axis data of a graph */
                QStringList referencedResults;
                /** \brief an arbitrary set of metadata for the compound (e.g. axis labels, axis properties, ... */
                QMap<QString, QVariant> metadata;
                /** \brief label for the compond */
                QString label;
        };

        struct QFLIB_EXPORT QFFitFitResultsStore {
                const QFRawDataRecord* rdr;
                int rdrRecID;
                int index;
                QString evalID;
                QMap<QString, QFRawDataRecord::evaluationResult> fitresults;
                QString evalgroup;
                QString egrouplabel;
                int64_t egroupindex;
                QString egroupdescription;


                inline QFFitFitResultsStore() {
                    rdr=NULL;
                    rdrRecID=-1;
                    index=-1;
                    evalID="";
                    fitresults.clear();
                    evalgroup="";
                    egrouplabel="";
                    egroupindex=0;
                    egroupdescription="";
                }

                inline QFFitFitResultsStore(const QFRawDataRecord* rdr, QString evalID) {
                    setRDR(rdr);
                    index=-1;
                    this->evalID=evalID;
                    fitresults.clear();
                    evalgroup="";
                    egrouplabel="";
                    egroupindex=0;
                    egroupdescription="";
                }

                inline QFFitFitResultsStore(const QFRawDataRecord* rdr) {
                    setRDR(rdr);
                    index=-1;
                    this->evalID="";
                    fitresults.clear();
                    evalgroup="";
                    egrouplabel="";
                    egroupindex=0;
                    egroupdescription="";
                }

                inline QFRawDataRecord* getRDR(const QFProject* p) const {
                    return p->getRawDataByID(rdrRecID);
                }
                inline void setRDR(const QFRawDataRecord* rdr) {
                    this->rdr=rdr;
                    this->rdrRecID=-1;
                    if (rdr) this->rdrRecID=rdr->getID();
                }
                inline void resultsSetEvaluationGroup(const QString& group){
                    this->evalgroup=group;
                }
                inline void resultsSetEvaluationGroupIndex(int64_t groupIndex) {
                    this->egroupindex=groupIndex;
                }
                inline void resultsSetEvaluationDescription(const QString& description) {
                    this->egroupdescription=description;
                }



                inline void setNumber(const QString& resultID, double value, const QString& unit=QString(""), const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                    fitresults[resultID].resultsSetNumber(  value,  unit,  group,  label, label_rich, sortPriority);
                }
                inline void setNumberError(const QString& resultID, double value, double error, const QString& unit=QString(""), const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                    fitresults[resultID].resultsSetNumberError(  value,  error, unit,  group,  label, label_rich, sortPriority);
                }
                inline void setInteger(const QString& resultID, qlonglong value, const QString& unit=QString(""), const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                    fitresults[resultID].resultsSetInteger(  value,  unit,  group,  label, label_rich, sortPriority);
                }
                inline void setBool(const QString& resultID, bool value,  const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                    fitresults[resultID].resultsSetBoolean(  value,    group,  label, label_rich, sortPriority);
                }
                inline void setString(const QString& resultID, const QString& value, const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                    fitresults[resultID].resultsSetString(  value,   group,  label, label_rich, sortPriority);
                }
                inline void setNumberVector(const QString& resultID, const QVector<double>& value, const QString& unit=QString(""), const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                    fitresults[resultID].resultsSetNumberVector(  value,  unit,  group,  label, label_rich, sortPriority);
                }

                inline void setNumberErrorVector(const QString& resultID, const QVector<double>& value, const QVector<double>& error, const QString& unit=QString(""), const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                    fitresults[resultID].resultsSetNumberErrorVector(  value,  error, unit,  group,  label, label_rich, sortPriority);
                }
                inline void setIntegerVector(const QString& resultID, const QVector<qlonglong> value, const QString& unit=QString(""), const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                    fitresults[resultID].resultsSetIntegerVector( value,  unit,  group,  label, label_rich, sortPriority);
                }
                inline void setBoolVector(const QString& resultID, const QVector<bool> value,  const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                    fitresults[resultID].resultsSetBooleanVector(  value,   group,  label, label_rich, sortPriority);
                }
                inline void setStringVector(const QString& resultID, const QStringList& value, const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                    fitresults[resultID].resultsSetStringVector(  value,   group,  label, label_rich, sortPriority);
                }







                inline  void resultsSetNumber(const QString& resultID, double value, const QString& unit=QString(""), const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                    fitresults[resultID].resultsSetNumber(  value,  unit,  group,  label, label_rich, sortPriority);
                }
                inline  void resultsSetNumberError(const QString& resultID, double value, double error, const QString& unit=QString(""), const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                    fitresults[resultID].resultsSetNumberError(  value,  error, unit,  group,  label, label_rich, sortPriority);
                }
                inline  void resultsSetInteger(const QString& resultID, qlonglong value, const QString& unit=QString(""), const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                    fitresults[resultID].resultsSetInteger(  value,  unit,  group,  label, label_rich, sortPriority);
                }
                inline  void resultsSetBoolean(const QString& resultID, bool value,  const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                    fitresults[resultID].resultsSetBoolean(  value,    group,  label, label_rich, sortPriority);
                }
                inline  void resultsSetString(const QString& resultID, const QString& value, const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                    fitresults[resultID].resultsSetString(  value,   group,  label, label_rich, sortPriority);
                }
                inline  void resultsSetNumberVector(const QString& resultID, const QVector<double>& value, const QString& unit=QString(""), const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                    fitresults[resultID].resultsSetNumberVector(  value,  unit,  group,  label, label_rich, sortPriority);
                }

                inline  void resultsSetNumberErrorVector(const QString& resultID, const QVector<double>& value, const QVector<double>& error, const QString& unit=QString(""), const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                    fitresults[resultID].resultsSetNumberErrorVector(  value,  error, unit,  group,  label, label_rich, sortPriority);
                }
                inline  void resultsSetIntegerVector(const QString& resultID, const QVector<qlonglong> value, const QString& unit=QString(""), const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                    fitresults[resultID].resultsSetIntegerVector( value,  unit,  group,  label, label_rich, sortPriority);
                }
                inline  void resultsSetBooleanVector(const QString& resultID, const QVector<bool> value,  const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                    fitresults[resultID].resultsSetBooleanVector(  value,   group,  label, label_rich, sortPriority);
                }
                inline void resultsSetStringVector(const QString& resultID, const QStringList& value, const QString& group=QString(""), const QString& label=QString(""), const QString& label_rich=QString(""),bool sortPriority=false) {
                    fitresults[resultID].resultsSetStringVector(  value,   group,  label, label_rich, sortPriority);
                }





                inline void resultsSetLabel(const QString& resultID, const QString& label, const QString& label_rich=QString("")) {
                    fitresults[resultID].resultsSetLabel(label, label_rich);
                }

                inline void resultsSetGroup(const QString& resultID, const QString& group) {
                    fitresults[resultID].resultsSetGroup(group);
                }

                inline void resultsSetGroupAndLabels(const QString& resultID, const QString& group, const QString& label, const QString& label_rich=QString("")) {
                    fitresults[resultID].resultsSetGroupAndLabels(group, label, label_rich);
                }
                inline void resultsSetGroupAndLabels(const QString& resultID, const QString& group, const QString& label, const QString& label_rich, bool pr) {
                    fitresults[resultID].resultsSetGroupLabelsAndSortPriority(group, label, label_rich, pr);
                }

                inline void resultsSetGroupLabelsAndSortPriority(const QString& resultID, const QString& group, const QString& label, const QString& label_rich, bool pr)  {
                    fitresults[resultID].resultsSetGroupLabelsAndSortPriority(group, label, label_rich, pr);
                }
                inline void resultsSetSortPriority(const QString& resultID, bool pr) {
                    fitresults[resultID].resultsSetSortPriority(pr);
                }

                inline void resultsSetNumberErrorAndBool(const QString& resultID, double value, double error, const QString& unit, const QString& resultBoolName, bool boolValue=true, bool writeBoolValue=true) {
                    fitresults[resultID].resultsSetNumberError(value, error, unit);
                    if (writeBoolValue) fitresults[resultBoolName].resultsSetBoolean(boolValue);
                }

                inline void resultsSetNumberAndBool(const QString& resultID, double value, const QString& unit, const QString& resultBoolName, bool boolValue=true, bool writeBoolValue=true) {
                    fitresults[resultID].resultsSetNumber(value,  unit);
                    if (writeBoolValue) fitresults[resultBoolName].resultsSetBoolean(boolValue);
                }
                inline void resultsSetIntegerAndBool(const QString& resultID, qlonglong value, const QString& unit, const QString& resultBoolName, bool boolValue=true, bool writeBoolValue=true) {
                    fitresults[resultID].resultsSetInteger(value,  unit);
                    if (writeBoolValue) fitresults[resultBoolName].resultsSetBoolean(boolValue);
                }
                inline void resultsSetStringAndBool(const QString& resultID, const QString& value, const QString& unit, const QString& resultBoolName, bool boolValue=true, bool writeBoolValue=true) {
                    fitresults[resultID].resultsSetString(value);
                    if (writeBoolValue) fitresults[resultBoolName].resultsSetBoolean(boolValue);
                    Q_UNUSED(unit);
                }
                inline void resultsSetBooleanAndBool(const QString& resultID, bool value, const QString& unit, const QString& resultBoolName, bool boolValue=true, bool writeBoolValue=true) {
                    fitresults[resultID].resultsSetBoolean(value);
                    if (writeBoolValue) fitresults[resultBoolName].resultsSetBoolean(boolValue);
                    Q_UNUSED(unit);
                }


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


        void readLock() const;
        void writeLock() const;
        bool writeTryLock(int timeout_ms=-1) const;
        bool writeTryLock(int timeout_ms, int repeats) const;
        void readUnLock() const;
        void writeUnLock() const;

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
        inline void resultsSetNumberList(const QString& evaluationName, const QString& resultName, const QVector<int>& value, const QString& unit=QString("")) {
            QVector<double> dat;
            dat.resize(value.size());
            for (int i=0; i<value.size(); i++) dat[i]=value[i];
            resultsSetNumberList(evaluationName, resultName, dat, unit);
        }
        /** \brief set a result of type number vector */
        inline void resultsSetNumberList(const QString& evaluationName, const QString& resultName, const QList<int>& value, const QString& unit=QString("")) {
            QVector<double> dat;
            dat.resize(value.size());
            for (int i=0; i<value.size(); i++) dat[i]=value[i];
            resultsSetNumberList(evaluationName, resultName, dat, unit);
        }
        /** \brief set a result of type number vector */
        inline void resultsSetNumberList(const QString& evaluationName, const QString& resultName, const QList<double>& value, const QString& unit=QString("")) {
            QVector<double> dat;
            dat.resize(value.size());
            for (int i=0; i<value.size(); i++) dat[i]=value[i];
            resultsSetNumberList(evaluationName, resultName, dat, unit);
        }

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


        /** \brief set a result element of type number vector. If more elements are needed in the vector, as available, these are initialized with 0. This function also sets an entry in an additional boolean list at the same position. */
        void resultsSetInNumberErrorListAndBool(const QString& evaluationName, const QString& resultName, int position, double value, double error, const QString& unit, const QString& resultBoolName, bool boolValue=true);
        /** \brief set a result element of type number vector. If more elements are needed in the vector, as available, these are initialized with 0. This function also sets an entry in an additional boolean list at the same position. */
        void resultsSetInNumberListAndBool(const QString& evaluationName, const QString& resultName, int position, double value, const QString& unit, const QString& resultBoolName, bool boolValue=true);
        /** \brief set a result element of type integer vector. If more elements are needed in the vector, as available, these are initialized with 0. This function also sets an entry in an additional boolean list at the same position. */
        void resultsSetInIntegerListAndBool(const QString& evaluationName, const QString& resultName, int position, qlonglong value, const QString& unit, const QString& resultBoolName, bool boolValue=true);
        /** \brief set a result element of type string vector. If more elements are needed in the vector, as available, these are initialized with 0. This function also sets an entry in an additional boolean list at the same position. */
        void resultsSetInStringListAndBool(const QString& evaluationName, const QString& resultName, int position, const QString& value, const QString& unit, const QString& resultBoolName, bool boolValue=true);
        /** \brief set a result element of type boolean vector. If more elements are needed in the vector, as available, these are initialized with 0. This function also sets an entry in an additional boolean list at the same position. */
        void resultsSetInBooleanListAndBool(const QString& evaluationName, const QString& resultName, int position, bool value, const QString& unit, const QString& resultBoolName, bool boolValue=true);






        /** \brief returns an element from the given list, or a defaultValue, if the element does not exist */
        double resultsGetInNumberList(const QString& evaluationName, const QString& resultName, int position, double defaultValue=0) const ;
        /** \brief returns an element from the given matrix, or a defaultValue, if the element does not exist */
        double resultsGetInNumberMatrix(const QString& evaluationName, const QString& resultName,  int row, int column, double defaultValue=0) const;
        /** \brief returns an element from the given list, or a defaultValue, if the element does not exist */
        double resultsGetErrorInNumberErrorList(const QString& evaluationName, const QString& resultName, int position, double defaultValue=0) const;
        /** \brief returns an element from the given matrix, or a defaultValue, if the element does not exist */
        double resultsGetErrorInNumberErrorMatrix(const QString& evaluationName, const QString& resultName,  int row, int column, double defaultValue=0) const;
        /** \brief returns an element from the given list, or a defaultValue, if the element does not exist */
        qlonglong resultsGetInIntegerList(const QString& evaluationName, const QString& resultName, int position, qlonglong defaultValue=0) const;
        /** \brief returns an element from the given matrix, or a defaultValue, if the element does not exist */
        qlonglong resultsGetInIntegerMatrix(const QString& evaluationName, const QString& resultName,  int row, int column, qlonglong defaultValue=0) const;
        /** \brief returns an element from the given list, or a defaultValue, if the element does not exist */
        bool resultsGetInBooleanList(const QString& evaluationName, const QString& resultName, int position, bool defaultValue=false) const;
        /** \brief returns an element from the given matrix, or a defaultValue, if the element does not exist */
        bool resultsGetInBooleanMatrix(const QString& evaluationName, const QString& resultName,  int row, int column, bool defaultValue=false) const;
        /** \brief returns an element from the given list, or a defaultValue, if the element does not exist */
        QString resultsGetInStringList(const QString& evaluationName, const QString& resultName, int position, const QString& defaultValue=QString("")) const;
        /** \brief returns an element from the given matrix, or a defaultValue, if the element does not exist */
        QString resultsGetInStringMatrix(const QString& evaluationName, const QString& resultName,  int row, int column, const QString& defaultValue=QString("")) const;






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
        /** \brief set a result of type string vector */
        inline void resultsSetStringList(const QString& evaluationName, const QString& resultName, const QStringList& value, const QString& unit=QString("")) {
            QVector<QString> s;
            s.resize(value.size());
            for (int i=0; i<value.size(); i++) s[i]=value[i];
            resultsSetStringList(evaluationName, resultName, s, unit);
        }

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
        /** \brief set/add the results in \a results (key is the result ID) with the given \a evaluationName */
        void resultsSet(const QString& evaluationName, const QMap<QString, evaluationResult>& results);
        /** \brief set/add the results in \a results, ignoring the given rdr, assuming that all results shall be given in this RDR! */
        void resultsSet(const QFFitFitResultsStore& results, bool setGroupProps=true);


        /** \brief store the given QFFitSTatistics as results */
        void resultsSetBasicFitStatistics(const QFBasicFitStatistics& result, const QString &evalID, const QString &prefix=QString("fitstat_"), const QString &group=QString("fit statistics"));
        /** \brief returns a list of resultIDs and labels for the fit statistics parameters */
        static void getStatisticsParams(QStringList* ids, QStringList* labels, QStringList* labelsHTML, const QString& prefix=QString("fitstat_"));


        /** \brief return a specified result  */
        evaluationResult resultsGet(const QString& evalName, const QString& resultName) const;

        /** \brief set/add the results in \a results, ignoring the given rdr, assuming that all results shall be given in this RDR! */
        bool resultsHasError(const QString& evalName, const QString& resultName) const;
        /** \brief return a specified result as a QFMathParser result (qfmpResult) */
        qfmpResult resultsGetForMathParser(const QString& evalName, const QString& resultName) const;
        /** \brief set a specified result from a QFMathParser result (qfmpResult) */
        void resultsSetFromMathParser(const QString& evalName, const QString& resultName, const qfmpResult& result) ;

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
        /*! \brief return a specified result as a QVariant, if the result has an associated error, only the values is returned

            The resulting QVariant conatins either a boolean (qfrdreBoolean), a QString (qfrdreString), an integer (qfrdreInteger),
            a double (qfrdreNumber) or a QList<QVariant> (qfrdreNumberVector).
        */
        QVariant resultsGetAsQVariantNoError(const QString& evalName, const QString& resultName) const;
        /*! \brief return the error associated to a specified result as a QVariant

            The resulting QVariant conatins either a boolean (qfrdreBoolean), a QString (qfrdreString), an integer (qfrdreInteger),
            a double (qfrdreNumber) or a QList<QVariant> (qfrdreNumberVector).
        */
        QVariant resultsGetAsQVariantOnlyError(const QString& evalName, const QString& resultName) const;
        /*! \brief return a specified result as a QVariant

            The resulting QVariant conatins either a boolean (qfrdreBoolean), a QString (qfrdreString), an integer (qfrdreInteger),
            a double (qfrdreNumber), QPointF (qfrdreNumberError) or a QList<QVariant> (qfrdreNumberVector).
        */
        QVariantList resultsGetAsQVariantList(const QString& evalName, const QString& resultName) const;
        /*! \brief return a specified result as a QVariant, if the result has an associated error, only the values is returned

            The resulting QVariant conatins either a boolean (qfrdreBoolean), a QString (qfrdreString), an integer (qfrdreInteger),
            a double (qfrdreNumber) or a QList<QVariant> (qfrdreNumberVector).
        */
        QVariantList resultsGetAsQVariantListNoError(const QString& evalName, const QString& resultName) const;
        /*! \brief return the error associated to a specified result as a QVariant

            The resulting QVariant conatins either a boolean (qfrdreBoolean), a QString (qfrdreString), an integer (qfrdreInteger),
            a double (qfrdreNumber) or a QList<QVariant> (qfrdreNumberVector).
        */
        QVariantList resultsGetAsQVariantListOnlyError(const QString& evalName, const QString& resultName) const;
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
        /** \brief return a specified result as double vector (or 0 if not possible!). If \a ok is supplied it will contain \c true if the conversion was possible and \c false otherwise.  */
        QStringList resultsGetAsStringList(const QString& evalName, const QString& resultName, bool* ok=NULL) const;

        /** \brief return the number of entries in a result vector (of any vector/matrix type)  */
        qlonglong resultsGetVectorMatrixItems(const QString& evalName, const QString& resultName) const;
        /** \brief return the number of entries in a result vector (of any vector/matrix type)  */
        bool resultsIsVectorMatrixType(const QString& evalName, const QString& resultName) const;




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

        /** \brief set the group and labels of a result (the result already has to exist!) */
        void resultsSetGroupAndLabels(const QString& evaluationName, const QString& resultName, const QString& group, const QString& label, const QString& label_rich=QString(""));

        /** \brief set the group and labels of a result (the result already has to exist!) */
        void resultsSetGroupLabelsAndSortPriority(const QString& evaluationName, const QString& resultName, const QString& group, const QString& label, const QString& label_rich, bool pr);

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

        /** \brief add/set a compound result */
        void resultsCompoundSet(const QString& evaluationName, const QString& compoundName, evaluationCompoundTypes type, const QStringList& refs, const QString& label=QString(), const QMap<QString, QVariant>& metadata=QMap<QString,QVariant>());
        /** \brief add/set a compound result */
        void resultsCompoundSet(const QString& evaluationName, const QString& compoundName, const evaluationCompoundResult& comp);
        /** \brief return the compound result */
        evaluationCompoundResult resultsCompoundGet(const QString& evaluationName, const QString& compoundName) const;
        /** \brief set a compound type */
        void resultsCompoundSetType(const QString& evaluationName, const QString& compoundName, evaluationCompoundTypes type);
        /** \brief return the compound result type */
        evaluationCompoundTypes resultsCompoundGetType(const QString& evaluationName, const QString& compoundName) const;
        /** \brief set a compound references */
        void resultsCompoundSetReferences(const QString& evaluationName, const QString& compoundName, const QStringList& refs);
        /** \brief return the compound result references */
        QStringList resultsCompoundGetRefs(const QString& evaluationName, const QString& compoundName) const;
        /** \brief set a compound label */
        void resultsCompoundSetLabel(const QString& evaluationName, const QString& compoundName, const QString& label);
        /** \brief return the compound result references */
        QString resultsCompoundGetLabel(const QString& evaluationName, const QString& compoundName) const;
        /** \brief set a compound metadata */
        void resultsCompoundSetMetadata(const QString& evaluationName, const QString& compoundName, const QMap<QString, QVariant>& metadata);
        /** \brief return the compound result references */
        QMap<QString, QVariant> resultsCompoundGetMetadata(const QString& evaluationName, const QString& compoundName) const;
        /** \brief return the compound result references */
        QVariant resultsCompoundGetMetadata(const QString& evaluationName, const QString& compoundName, const QString& meta_id, const QVariant& defaultVal=QVariant()) const;
        /** \brief add a compound metadata */
        void resultsCompoundAddMetadata(const QString& evaluationName, const QString& compoundName, const QString& meta_id, const QVariant& meta_val);


        /** \brief get number of results for a specified evaluation */
        int resultsGetCount(const QString& evalName) const;
        /** \brief get number of compounds for a specified evaluation */
        int resultsCompoundGetCount(const QString& evalName) const;
        /** \brief get number of evaluations in this object */
        int resultsGetEvaluationCount() const;
        /** \brief get the i-th evaluation name */
        QString resultsGetEvaluationName(int i) const;
        /** \brief get the i-th result name */
        QString resultsGetResultName(const QString& evaluationName, int i) const;
        /** \brief get the evaluation names */
        QStringList resultsGetEvaluationNames() const;
        /** \brief get the result names */
        QStringList resultsGetResultNames(const QString& evaluationName) const;

        /*! \brief save a copy of all results with the given \a oldEvalName to the given \a newEvalName
            \param oldEvalName name of the evaluation results section to copy
            \param newEvalName name of the target evaluation results section
            \note if \a newEvalName already exists, the additional results will be added to \a newEvalName
         */
        void resultsCopy(const QString& oldEvalName, const QString& newEvalName) ;

        /*! \brief save the results to a CSV file

            saves a table of the results as Comma-Separated File \a filename. Not all result types may be exported,
            as this format does not support every dataformat. Any data that may be save as a single item is stored directly
            (qfrdreNumber, qfrdreInteger, qfrdreString, qfrdreBoolean). Numbers with errors (qfrdreNumberError) are also stored,
            If numbers in a column have errors then a second column is added for the errors.

            \param filename the file to create
            \param vectorsToAvg if \c false vector/matrix results are expanded into several columns. Otherwise only the avg/stddev are stored in 2 columns
            \param separator column separator in the file
            \param decinmalPoint which character to sue as decimal point
            \param stringDelimiter strings are surrounded by this character (one in front,one behind)
            \return \c true on success
        */
        bool resultsSaveToCSV(const QString& filename, bool vectorsToAvg=false, const QString& separator=", ", QChar decimalPoint='.', QChar stringDelimiter='"') const;

        /*! \brief save the results to a CSV file

            saves a table of the results as Comma-Separated File \a filename. Not all result types may be exported,
            as this format does not support every dataformat. Any data that may be save as a single item is stored directly
            (qfrdreNumber, qfrdreInteger, qfrdreString, qfrdreBoolean). Numbers with errors (qfrdreNumberError) are also stored,
            If numbers in a column have errors then a second column is added for the errors.

            \param vectorsToAvg if \c false vector/matrix results are expanded into several columns. Otherwise only the avg/stddev are stored in 2 columns
            \param colNames column names (output)
            \param rownames row names (output)
            \return a table of data
        */
        QList<QList<QVariant> > resultsGetTable(bool vectorsToAvg=false, QStringList* colNames=NULL, QStringList* rownames=NULL) const;

        /*! \brief save the results to a SYLK file

            saves a table of the results as Comma-Separated File \a filename. Not all result types may be exported,
            as this format does not support every dataformat. Any data that may be save as a single item is stored directly
            (qfrdreNumber, qfrdreInteger, qfrdreString, qfrdreBoolean). Numbers with errors (qfrdreNumberError) are also stored,
            If numbers in a column have errors then a second column is added for the errors.

            \param filename the file to create
            \param vectorsToAvg if \c false vector/matrix results are expanded into several columns. Otherwise only the avg/stddev are stored in 2 columns
            \param flipTable if set \c true, the output will exchange X and Y coordinates of the cells, effectively flipping the result table
            \return \c true on success
        */
        bool resultsSaveToSYLK(const QString& filename, bool vectorsToAvg=false, bool flipTable=false) const;

        /*! \brief save the results to a SYLK file

            saves a table of the results as Comma-Separated File \a filename. Not all result types may be exported,
            as this format does not support every dataformat. Any data that may be save as a single item is stored directly
            (qfrdreNumber, qfrdreInteger, qfrdreString, qfrdreBoolean). Numbers with errors (qfrdreNumberError) are also stored,
            If numbers in a column have errors then a second column is added for the errors.

            \param filename the file to create
            \param vectorsToAvg if \c false vector/matrix results are expanded into several columns. Otherwise only the avg/stddev are stored in 2 columns
            \param flipTable if set \c true, the output will exchange X and Y coordinates of the cells, effectively flipping the result table
            \return \c true on success
        */
        bool resultsSave(const QString& filename, int format, bool vectorsToAvg=false, bool flipTable=false) const;

        /** \brief return a table model which may be used to display the results */
        QFRDRResultsModel* resultsGetModel() const;

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

        /*! \brief return a list of all result labels (first field) for this raw data record, together ith the parameter IDs (second field) and the according evaluation ID (third field)

            If \a evalName is non-empty, this restricts the list to results from the given evaluation \a evalName.
            If \a group is non-empty the resultant list is filtered by result groups.
            If \a evalgroup is non-empty the list is restricted to evaluations from the given evalGroup
         */
        QList<QTriple<QString, QString, QString> > resultsCalcNamesEvalIDsAndLabels(const QString& evalName=QString(""), const QString& group=QString(""), const QString& evalgroup=QString("")) const;

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
        virtual QString getType() const;
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const;
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const;
        /** \brief return type description */
        virtual QString getTypeDescription() const;
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const;
        /** \brief returns the number of additional editor panes for this record */
        virtual int getEditorCount();
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName(int i);
        /** \brief create an object for the i-th editor pane */
        virtual QFRawDataEditor* createEditor(QFPluginServices* services,  QFRawDataPropertyEditor *propEditor, int i=0, QWidget* parent=NULL);
        /** \brief write object contents into XML file
         *
         *  \param w QXmlStreamWriter class for the output
         *  \param projectfilename filename of the output project file
         *  \param copyFilesToSubfolder if this is activated (\c true), the function copies any file, which is not yet in a subfolder of the project,
         *                              into a subfolder inside subfoldername. The copy operation is not executed immediately, but the files to copy
         *                              are appended to filecopylist. The references in the written XML file already point to the new, anticipated file
         *                              locations.
         *  \param subfoldername name of the subfolder, which assembles the files in the project
         *  \param filecopylist list of required file copy operations.
         *
         *  \note If \c copyFilesToSubfolder==true then this function writes only thise files into the XML fragment, for which doCopyFileForExport() returns \c true .
         *        If a file is copied/written in this mode, its location is changed in the following way:
         *           # files, that are within a subfolder of the \a projectfilename are written with a relative path.
         *           # if the files are not in a subfolder, they are to be copied into a subfolder within \a subfoldername below the directory of projectfilename
         *           # if doCopyFileForExport() returns a \c newFilename, that filename is used for the new file.
         *        .
         *        This function does not copy/move any file, it simply adds the files (old/new location) to \a filecopylist, if it is supplied.
         */
        void writeXML(QXmlStreamWriter& w, const QString& projectfilename, bool copyFilesToSubfolder=false, const QString &subfoldername=QString("raw_data_files/"), QList<QFProject::FileCopyList> *filecopylist=NULL, int writeMode=QFProject::wsmAll) const;

        /** \brief this function is called by writeXML() in order to determine, whether a linked file should be included in an export operation
         *
         *  The default implementation returns \c true.
         *
         *  \param filename input filename
         *  \param fileType file type string of the file \a filename
         *  \param[out] newFilename you can optionally provide a new (relative) file path/name for this file.
         *  \param filecopylist can be used to check, whether a file already exists.
         *  \param subfoldername default subfolder for copied files
         *
         *  \note You will HAVE to use filecopylist to ensure, that no two distinct files are written to the SAME output filename.
         *        You can use QFProject::ensureUniqueFilename() to ensure a unique filename in filecopylist.
         */
        virtual bool doCopyFileForExport(const QString& filename, const QString& fileType, QString& newFilename, const QList<QFProject::FileCopyList> *filecopylist=NULL, const QString &subfoldername=QString("rdr_data_files/")) const;


        /** \brief returns a list of filetypes which correspond to the filetypes returned by getExportDialogFiletypes() */
        virtual QStringList getExportFiletypes() const;
        /** \brief returns the title of the Export file dialog */
        virtual QString getExportDialogTitle() const;
        /** \brief returns the filetype of the Export file dialog */
        virtual QString getExportDialogFiletypes() const;
        /** \brief export the raw data into the specified format */
        virtual void exportData(const QString& format, const QString& filename)const ;




        enum FileListEditOption {
                 FilesNotEditable = 0x0,
                 FilesEditable = 0x1,
                 CustomFilesAddFunction = 0x2
         };
         Q_DECLARE_FLAGS(FileListEditOptions, FileListEditOption)


        /** \brief indicates whether the files list may be edited by the user. In that case the file list editing functions (protected) have to be implemented! */
        virtual FileListEditOptions isFilesListEditable() const;

        /** \brief this should present the user with a dialog to select a bunch of files to add to the files list. If the user selects "Cancel", return \c false, otherwise
         *         return \c true. In the second case the files in \a files will be added to the files list of this RDR and reloadFromFiles() will be called immediately afterwards!
         *
         *  \note you only have to implement this, when you overwrote isFilesListEditable() to return the CustomFilesAddFunction and the FilesEditable flags.
         */
        virtual bool selectNewFiles(QStringList& files, QStringList& types, QStringList& descriptions) const;
        /** \brief use this if not all files may be deleted by the user. It is called before files are removed. Returns \c true by default.
         *
         *  \note you only have to implement this, when you overwrote isFilesListEditable() to return FilesEditable flag.
         */
        virtual bool mayDeleteFiles(QStringList& files, QStringList& types, QStringList& descriptions) const;
        /** \brief reload the RDR contents.
         *
         *  \return returns \c true on success and \c false if an error occured.
         *  \note you only have to implement this, when you overwrote isFilesListEditable() to return FilesEditable flag.
         */
        virtual bool reloadFromFiles();
        /** \brief returns a list of possible files types
         *
         *  \return returns \c true on success and \c false if an error occured.
         *  \note you only have to implement this, when you overwrote isFilesListEditable() to return FilesEditable flag.
         */
        virtual QStringList getPossibleFilesTypes() const;


    public:
        virtual int curvesGetCount() const;
        virtual QString curvesGetName(int index) const;
        virtual QVector<double> curvesGetX(int index) const;
        virtual QVector<double> curvesGetXError(int index) const;
        virtual QVector<double> curvesGetY(int index) const;
        virtual QVector<double> curvesGetYError(int index) const;
        virtual bool curvesGetLogX(int index) const;
        virtual bool curvesGetLogY(int index) const;
        virtual CurveType curvesGetType(int index) const;
        virtual QString curvesGetXLabel(int index) const;
        virtual QString curvesGetYLabel(int index) const;

};

DEFINE_READWRITE_LOCKERS(QFRawDataRecord, readLock, writeLock, readUnLock, writeUnLock)


Q_DECLARE_OPERATORS_FOR_FLAGS(QFRawDataRecord::FileListEditOptions)


#endif // QFRAWDATARECORD_H

