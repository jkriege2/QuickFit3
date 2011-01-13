#ifndef QFRAWDATARECORD_H
#define QFRAWDATARECORD_H
#include <QSettings>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QtXml>
#include <QStringList>
#include <QMap>
#include <QIcon>
#include <QAbstractTableModel>
#include "qfproject.h"
#include "qfrawdataeditor.h"
#include "qfrdrresultsmodel.h"
#include "qfrdrpropertymodel.h"
#include "qfproperties.h"


/*! \brief this class manages one raw data record in the project
    \ingroup qf3lib_project


    This class manages the basic properties of every raw data item in QuickFit 3.0.


    \section QFRawDataRecord_Error Error Reporting
    Errors are reported by the methods error() which returns \c true if an error has occured and the method
    errorDescription() which returns a textual description of the error. The protected method setError() may be
    used to indicate that an error has occured to the class.
 */
class QFRawDataRecord : public QObject, public QFProperties {
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
            emit propertiesChanged();
        }
        /** \brief set the description  */
        inline void setDescription(const QString& d) {
            description=d;
            //emit dataChanged(index(project->getRawDataIndex(this), 0), index(project->getRawDataIndex(this), 2));
            emit propertiesChanged();
        };
        /** \brief returns a model which may be used to access and edit the properties in this object  */
        inline QFRDRPropertyModel* getPropertyModel() { return propModel; }
    signals:
        /** \brief emitted whenever at least one of the properties changes */
        void propertiesChanged();
        /** \brief emitted whenever at least one of the results changes */
        void resultsChanged();
        /** \brief emitted whenever the data in this object changes */
        void rawDataChanged();
    protected:
        /** \copydoc QFProperties::emitPropertiesChanged() */
        virtual void emitPropertiesChanged() { emit propertiesChanged(); };
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
        /** \brief read in external data files <u>and</u> data stored in the project file <b>IMPLEMENT IN CHILD CLASSES!</b>
         *
         * If \a e is \c NULL then this method should only read the datafiles already saved in the files property.
         */
        virtual void intReadData(QDomElement* e=NULL) { /*std::cout<<"QFRawDataRecord::intReadData()\n";*/ };
        /** \brief set the internal error flag and description */
        void setError(QString description) { errorOcc=true; errorDesc=description; }


    public:
        /** \brief this enum contains possible types of evaluation results */
        enum evaluationResultType {
            qfrdreNumber,
            qfrdreNumberError,
            qfrdreNumberList,
            qfrdreInteger,
            qfrdreString,
            qfrdreBoolean,
            qfrdreInvalid
        };
        /** \brief this struct is used to store evaluation results associated with this data record */
        struct evaluationResult {
            evaluationResult() {
                type=qfrdreInvalid;
            }
            evaluationResultType type;
            double dvalue;
            double derror;
            int64_t ivalue;
            bool bvalue;
            QString svalue;
            QString unit;
            QVector<double> dvec;
        };
    protected:
        /** \brief evaluation results are stored in this QMap which maps an evaluation name to
         *         to a list of evaluation results (which is indexed by result names! */
        QMap<QString, QMap<QString, evaluationResult> > results;
        /** \brief table used to display the results */
        QFRDRResultsModel* resultsmodel;
    public:
        /** \brief clear all evaluation results */
        inline void resultsClearAll() {
            results.clear();
            emit resultsChanged();
        };
        /** \brief clear all evaluation results of a specific evaluation name */
        inline void resultsClear(QString name) {
            if (results.contains(name)) {
                results[name].clear();
                results.remove(name);
                emit resultsChanged();
            }
        };
        /** \brief check whether a result exists */
        inline bool resultsExists(QString evalName, QString resultName) {
            if (results.contains(evalName)) {
                return results[evalName].contains(resultName);
            }
            return false;
        };
        /** \brief check whether there are any results from a given evauation */
        inline bool resultsExistsFromEvaluation(QString evalName) {
            return results.contains(evalName);
        };
        /** \brief set a result of type number */
        void resultsSetNumber(QString evaluationName, QString resultName, double value, QString unit=QString(""));
        /** \brief set a result of type number */
        void resultsSetNumberList(QString evaluationName, QString resultName, QVector<double>& value, QString unit=QString(""));
        /** \brief set a result of type number+error */
        void resultsSetNumberError(QString evaluationName, QString resultName, double value, double error, QString unit=QString(""));
        /** \brief set a result of type integer */
        void resultsSetInteger(QString evaluationName, QString resultName, int64_t value, QString unit=QString(""));
        /** \brief set a result of type string */
        void resultsSetString(QString evaluationName, QString resultName, QString value);
        /** \brief set a result of type boolean */
        void resultsSetBoolean(QString evaluationName, QString resultName, bool value);
        /** \brief return a specified result as variant */
        inline evaluationResult resultsGet(QString evalName, QString resultName) {
            evaluationResult r;
            if (resultsExists(evalName, resultName)) {
                r=results[evalName].value(resultName);
            }
            return r;
        };
        /** \brief return a specified result as string */
        QString  resultsGetAsString(QString evalName, QString resultName);
        /** \brief return a specified result as double (or 0 if not possible!) */
        double resultsGetAsDouble(QString evalName, QString resultName);
        /** \brief return a specified result as double (or 0 if not possible!) */
        QVector<double> resultsGetAsDoubleList(QString evalName, QString resultName);
        /** \brief return a specified result's error as double (or 0 if not possible!) */
        double resultsGetErrorAsDouble(QString evalName, QString resultName);
        /** \brief get number of results for a specified evaluation */
        inline unsigned int resultsGetCount(QString evalName) {
            if (results.contains(evalName)) return results[evalName].size();
            return 0;
        };
        /** \brief get number of evaluations in this object */
        inline unsigned int resultsGetEvaluationCount() {
            return results.size();
        };
        /** \brief get the i-th evaluation name */
        inline QString resultsGetEvaluationName(unsigned int i) {
            if ((long)i<results.size()) return results.keys().at(i);
            return QString("");
        };
        /** \brief get the i-th result name */
        QString resultsGetResultName(QString evaluationName, unsigned int i);

        /** \brief return a table model which may be used to display the results */
        QFRDRResultsModel* resultsGetModel() { return resultsmodel; };

    public:
        /** \brief return type (short type string) */
        virtual QString getType() const { return tr("unknown"); };
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("unknown"); };
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/projecttree_emptyfile.png"); };
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr("unknown"); };
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/projecttree_emptyfile.png"); };
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
