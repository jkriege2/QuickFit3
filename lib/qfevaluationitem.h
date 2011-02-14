#ifndef QEVALUATIONITEM_H
#define QEVALUATIONITEM_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QVariant>
#include <QtXml>
#include <QStringList>
#include <QMap>
#include <QIcon>
#include <QAbstractTableModel>
#include "qfproject.h"
#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include "qfproperties.h"
#include "qfpluginservices.h"
#include "qffitalgorithm.h"
#include "qffitfunction.h"
#include "qftools.h"


// forward declaration
class QFEvaluationPropertyEditor;


/*! \brief this class manages one evaluation record in the project
    \ingroup qf3lib_project

    An evaluation is a very basic node in the project tree. It does not come with a lot of features. Basically
    the notion of an evaluation is to take (some or all of the) data and produce some kind of result from these.

    The QFEvaluationItem may be used to represent a wide variety of different evaluations. In its basic form, no
    additional features are provided and the suer has to implement the selection of the records that apply to the
    evaluation and also the user interface. In addition to that some "special features" are implemented to allow the
    easy implementation of often occuring cases:
      * In many cases an evaluation is only applicable to a raw data set if certain conditions apply, e.g. if the data
        set has a certain type ... This may be represented by overwriting the isApplicable() function which returns
        \c true in its default implementation.
      * Many evaluations apply to a (selectable) set raw data records. To implement this the class allows to select
        and to highlight records. There may be multiple selected records, but only one highlighted record. The function
        getShowRDRList() returns \c true if a list of applicable records is to be displayed and getUseSelection() returns
        \c ture if the user shall be able to select multiple records from the list. The second one only applies if the
        first one is \c true of course. Also if getShowRDRList() returns \c true the user may highlight one record in
        any case, but the evaluation of course does not have to react to this. The selection/highlighting may be accessed
        by these functions:
          * getSelectedRecords()
          * selectRecord()
          * selectAllAplicableRecords()
          * deselectRecord()
          * deselectRecord()
          * getSelectedRecordCount()
          * getSelectedRecord()
          * clearSelectedRecords()
          * isSelected()
          * getHighlightedRecord()
          * setHighlightedRecord()
          * \b signal: highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord)
          * \b signal: selectionChanged(QList<QFRawDataRecord*> selectedRecords)
        .
    .

    Results created by evaluations are either stored in the according QFRawDataRecord objects (there are methods
    to save, read and display results) or in any other place possible (which then has to implemented). One could
    imagine to have an evaluation create a new, or update an existing raw data record.

    Evaluation items also contain properties, but they are not visible for the user and only used as a simple
    interface to load and store them.

    \section QFEvaluationRecord_Error Error Reporting
    Errors are reported by the methods error() which returns \c true if an error has occured and the method
    errorDescription() which returns a textual description of the error. The protected method setError() may be
    used to indicate that an error has occured to the class.
 */
class QFEvaluationItem : public QObject, public QFProperties {
        Q_OBJECT
    public:


        /*! \brief class constructor, reads from QDomElement
            \param parent the project this object belongs to
            \param useSelection indicates whether the user may select a set of records or not.
         */
        QFEvaluationItem(QFProject* parent, bool showRDRList, bool useSelection);
        /** \brief Default destructor */
        virtual ~QFEvaluationItem();

        /** \brief initialize the object with the given data */
        void init(QString name=QString(""));

        /** \brief initialize from QDomElement */
        void init(QDomElement& e);


        /** \brief return the next sibling rawdata record in the project */
        inline QFEvaluationItem* getNext() { return project->getNextEvaluation(this); };
        /** \brief return the next sibling rawdata record in the project */
        inline QFEvaluationItem* getPrevious() { return project->getPreviousEvaluation(this); };
        /** \brief return the next sibling rawdata record in the project, which has the same type as this evaluation */
        inline QFEvaluationItem* getNextOfSameType() { return project->getNextEvaluationOfSameType(this); };
        /** \brief return the next sibling rawdata record in the project, which has the same type as this evaluation */
        inline QFEvaluationItem* getPreviousOfSameType() { return project->getPreviousEvaluationOfSameType(this); };

        /** \brief return the ID */
        inline int getID() const { return ID; };
        /** \brief return the name */
        inline QString getName() const { return name; }
        /** \brief return the description  */
        inline QString getDescription() const { return description; };
        /** \brief return a pointer to the project that contains this QRawDatarecord */
        inline QFProject* getProject() const { return project; }


        /** \brief returns \c true if an error occured */
        inline bool error() const { return errorOcc; }
        /** \brief returns the description of the last error */
        inline QString errorDescription() const { return errorDesc; }




        /** \brief determines whether this evaluation is applicable to a given raw data record. This method is used to generate the
         *         list of raw data records presented to the user */
        virtual bool isApplicable(QFRawDataRecord* record) { return true; };

        /** \brief returns whether the user may select a set of records (\c true ) or not (\c false ). */
        bool getUseSelection() const { return useSelection; };

        /** \brief returns whether a list of applicable records is displayed (\c true ) or not (\c false ). */
        bool getShowRDRList() const { return showRDRList; };

        /** \brief returns a list of currently selected items */
        QList<QFRawDataRecord*> getSelectedRecords() const { return selectedRecords; };

        /** \brief add a record to the set of selected records */
        void selectRecord(QFRawDataRecord* record);

        /** \brief select all records from the project to which the evaluation is applicable */
        void selectAllAplicableRecords();

        /** \brief remove a record from the set of selected records */
        void deselectRecord(QFRawDataRecord* record);

        /** \brief remove the i-th record from the set of selected records */
        void deselectRecord(int i);

        /** \brief return the number of selected records */
        inline int getSelectedRecordCount() { return selectedRecords.size(); }

        /** \brief return the i-th selected record */
        QFRawDataRecord* getSelectedRecord(int i);

        /** \brief empty list of selected records */
        void clearSelectedRecords();

        /** \brief returns \c true, if the given record is selected */
        bool isSelected(QFRawDataRecord* record) const { return selectedRecords.contains(record); };

        /** \brief returns the currently highlughted record */
        QFRawDataRecord* getHighlightedRecord() const { return highlightedRecord; };

        /** \brief this method may be called in order to select a new record or to select none (call with \c NULL as argument) */
        void setHighlightedRecord(QFRawDataRecord* record);

        /** \brief return type (short type string) */
        virtual QString getType() const { return tr("unknown"); };
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("unknown"); };
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/projecttree_emptyevaluation.png"); };
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr("unknown"); };
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/projecttree_emptyevaluatione.png"); };
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName() { return QString(""); };
        /** \brief create an object for the i-th editor pane */
        virtual QFEvaluationEditor* createEditor(QFPluginServices* services, QWidget* parent=NULL) { return NULL; };
        /** \brief write object contents into XML file */
        virtual void writeXML(QXmlStreamWriter& w);

        /** \brief list of the raw data records this evaluation is applicable to */
        QList<QFRawDataRecord*> getApplicableRecords();

    public slots:
        /** \brief set the name */
        inline void setName(const QString n) {
            name=n;
            emit propertiesChanged();
        }
        /** \brief set the description  */
        inline void setDescription(const QString& d) {
            description=d;
            emit propertiesChanged();
        };


    signals:
        /** \brief emitted whenever at least one of the properties changes */
        void propertiesChanged();
        /** \brief emitted whenever the evaluation results change */
        void resultsChanged();
        /** \brief signal emitted when the highlighted record changed */
        void highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord);
        /** \brief signal emitted when the selection changed */
        void selectionChanged(QList<QFRawDataRecord*> selectedRecords);

    protected:
        /** \brief ID of the raw data record */
        int ID;
        /** \brief name of the raw data record */
        QString name;
        /** \brief description of the raw data record */
        QString description;
        /** \brief indicates whether an error has occured */
        bool errorOcc;
        /** \brief contains the description of the last error */
        QString errorDesc;
        /** \brief pointer to the parent project object */
        QFProject* project;

        /** \brief read object contents from QDomElement */
        void readXML(QDomElement& e);
        /** \brief write contents to QXmlStreamWriter (data tag) <b>IMPLEMENT IN CHILD CLASSES!</b>
         *
         * This method may be used to store additional data (like algorithm configuration ...) from the project file
         * \note You will have to explicitly call the intWriteData() metod of the parent class of your newly implemented
         * evaluation class in order to ensure that all data is correctly stored to the project file! Do so by calling
         * \code QFEvaluationItem::intWriteData(e); \endcode at the start of your method !!!
         */
        virtual void intWriteData(QXmlStreamWriter& w) {};
        /** \brief read in data stored in the project file <b>IMPLEMENT IN CHILD CLASSES!</b>
         *
         * This method may be used to read additional data (like algorithm configuration ...) from the project file
         * \note You will have to explicitly call the intReadData() metod of the parent class of your newly implemented
         * evaluation class in order to ensure that all data is correctly read from the project file!  Do so by calling
         * \code QFEvaluationItem::intReadData(e); \endcode at the start of your method !!!
         */
        virtual void intReadData(QDomElement* e) {};
        /** \brief set the internal error flag and description */
        void setError(QString description) { errorOcc=true; errorDesc=description; }

        /** \brief set the list of currently selected items */
        void setSelectedRecords(QList<QFRawDataRecord*> records);

        /** \brief determines whether the user may select a set of records (\c true ) or whether the user may only
         *         highlight a single record  (\c false , default).
         * \note This only applied when showRDRList is \c true. The only way to set this property is by used of the
         *       class constructor, so the implementor of an evaluation is forced to think about this topic! */
        bool useSelection;

        /** \brief determines whether the user may select a set of records (\c true ) or whether the user may only
         *         highlight a single record  (\c false , default).
         * \note The only way to set this property is by used of the class constructor, so the implementor of an
         *       evaluation is forced to think about this topic! */
        bool showRDRList;

        /** \brief list of the currently selected records */
        QList<QFRawDataRecord*> selectedRecords;

        /** \brief points to the currently highlighted record */
        QFRawDataRecord* highlightedRecord;
};

#endif // QEVALUATIONITEM_H

