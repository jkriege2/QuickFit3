#ifndef QFIMFCCSFITEVALUATIONITEM_H
#define QFIMFCCSFITEVALUATIONITEM_H

#include <QMap>
#include <QSettings>

#include "qfevaluationitem.h"
#include "qfevaluationitemfactory.h"
#include "qffitresultsbyindexasvectorevaluation.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include "../interfaces/qfrdrsimplecountrateinterface.h"
#include "../interfaces/qfrdrcountratesinterface.h"
#include "qffitresultsbyindexevaluationfittools.h"
#include "qffcsweightingtools.h"
#include "qfevaluationpropertyeditor.h"
#include "qfimfccsparameterinputtable.h"
#include "qfmatchrdrfunctor.h"

class QFImFCCSMatchRDRFunctor: public QFMatchRDRFunctor {
    public:
        explicit QFImFCCSMatchRDRFunctor();
        /** \brief reimplement this function to check whether a QFRawDataRecord* is contained in a QFSelectRDRDialog. the default implementation always returns \c true . */
        virtual bool matches(const QFRawDataRecord* record) const ;
};

/*! \brief evaluation item class 
    \ingroup qf3rdrdp_imfccsfit


    
    
*/
class QFImFCCSFitEvaluationItem : public QFFitResultsByIndexAsVectorEvaluation, public QFFitResultsByIndexEvaluationFitTools, public QFFCSWeightingTools {
        Q_OBJECT
    signals:
        void parameterStructureChanged();
    public:
        /** \brief which data weighting should be applied */
        /** Default constructor */
        QFImFCCSFitEvaluationItem(QFProject* parent);
        /** Default destructor */
        virtual ~QFImFCCSFitEvaluationItem();

        /** \brief return type (short type string) */
        virtual QString getType() const { return QString("imfccs_fit"); };
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("imaging FCCS Fit"); };
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/imfccs_fit.png"); };
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr("Least squares fitting for imaging fluorescence crosscorrelation spectroscopy (imaging FCCS) data"); };
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/imfccs_fit_logo.png"); };
        /** \brief returns the number of additional editor panes for this record */
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName() { return QString("Fit"); };
        /** \brief create an object for the i-th editor pane */
        virtual QFEvaluationEditor* createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent=NULL);

        


        /** \brief return the smallest available index */
        virtual int getIndexMin(QFRawDataRecord* r) const;
        /** \brief return the largest available index */
        virtual int getIndexMax(QFRawDataRecord* r) const;

        /*! \brief perform a fit for the given \a record and \a run

            The parameters \a defaultMinDatarange and \a defaultMaxDatarange set the range of data points taken for the fit.
            If both are -1, the full range is used

            The object \a dlgFitProgress (if supplied) is used to report the progress and to check whether the user clicked "Cancel".

            \note this method is intended to perform the fits one after the other or single fits. It will internally starts the fit algorithm
                  in its own thread. If you want to control the multithreading by yourself, use doFitForMultithread() instead !!!
          */
        virtual void doFit(QFRawDataRecord* record, int run, int defaultMinDatarange=-1, int defaultMaxDatarange=-1, QFFitAlgorithmReporter* dlgFitProgress=NULL, bool doLog=false);

        /*! \brief perform a fit for the given \a record and \a run

            The parameters \a defaultMinDatarange and \a defaultMaxDatarange set the range of data points taken for the fit.
            If both are -1, the full range is used

            The object \a dlgFitProgress (if supplied) is used to report the progress and to check whether the user clicked "Cancel".
          */
        virtual void doFitForMultithread(QFRawDataRecord* record, int run, int defaultMinDatarange=-1, int defaultMaxDatarange=-1, QFPluginLogService *logservice=NULL) const;

        /** \brief calculates fit statistics for the given fit function and dataset. */
        QFFitStatistics calcFitStatistics(bool storeAsResults, QFFitFunction* ffunc, long N, double* tauvals, double* corrdata, double* weights, int datacut_min, int datacut_max, double* fullParams, double* errors, bool* paramsFix, int runAvgWidth, int residualHistogramBins, QFRawDataRecord* record=NULL, int run=-1);


        QFEvaluationRawDataModelProxy* getRawDataProxyModel() const;

        virtual QFRawDataRecord* getFitFile(int num) const;
        virtual QFFitFunction* getFitFunction(int num) const;
        virtual QFFitFunction* getFitFunctionForID(QString id) const;
        virtual QString getFitFunctionID(int num) const;
        virtual QString getFitFunctionID() const;

        virtual QFFitFunction* getFitFunction(QFRawDataRecord *rdr) const;
        virtual QString getFitFunctionID( QFRawDataRecord* rdr) const;

        /** \brief if this returns a non-empty string (default: empty string), the result is added to the getParameterStoreID() to make the store specific to the given \a rdr */
        virtual QString rdrPointerToParameterStoreID(QFRawDataRecord* rdr) const;

        QFImFCCSParameterInputTable* getParameterInputTableModel() const;

        int getNumberOfFitFiles() const;
        QFMatchRDRFunctor* getMatchFunctor() const;
        /** \brief set the current index to use */
        virtual int getCurrentIndex() const;
    public slots:
        void setFitFile(int num, QFRawDataRecord *record);
        virtual void setFitFunction(int num, QString fitFunction);
        virtual void setFitFunction(QString fitFunction);
        virtual int addFitFile();
        virtual void removeFitFile();
        /** \brief set the current index to use */
        virtual void setCurrentIndex(int index);

    signals:
        void fileChanged(int num, QFRawDataRecord* record);
    protected:

        QFImFCCSMatchRDRFunctor* matchFunctor;
        /** \brief proxy model to filter rdrModel */
        QFEvaluationRawDataModelProxy* rdrProxy;

        QFImFCCSParameterInputTable* paramTable;


        /** \brief this list stores the several fit functions selected in the editor */
        QMap<int, QString> m_multiFitFunctions;
        /** \brief files for a fit */
        QList<QPointer<QFRawDataRecord> > fitFilesList;


        /** \brief makes sure that at least one fit file is in the list */
        void ensureFitFiles();


        /** \brief write object contents into XML file
        *
        *  this function saves the id of the current fit function and algorithm, as well as the contents of parameterStore to
        *  the given XML file.
        */
        virtual void intWriteData(QXmlStreamWriter& w);
        /** \brief read back the data stored by intWriteXML() */
        virtual void intReadData(QDomElement* e);



        /** \brief determines whether this evaluation is applicable to a given raw data record. This method is used to generate the
         *         list of raw data records presented to the user */
        virtual bool isApplicable(QFRawDataRecord* record);

        /*! \copydoc QFFitResultsEvaluation::intWriteDataAlgorithm()      */
        virtual void intWriteDataAlgorithm(QXmlStreamWriter& w) const;

        /*! \copydoc QFFitResultsEvaluation::intReadDataAlgorithm()      */
        virtual void intReadDataAlgorithm(QDomElement& e);

        virtual bool hasSpecial(const QFRawDataRecord* r, const QString& id, const QString& paramid, double& value, double& error) const;
        virtual bool hasSpecial(const QFRawDataRecord* r, int index, const QString& paramid, double& value, double& error) const ;


        virtual bool overrideFitFunctionPreset(QFRawDataRecord* r, QString paramName, double &value) const ;
        virtual bool overrideFitFunctionPresetError(QFRawDataRecord* r, QString paramName, double &value) const ;
        virtual bool overrideFitFunctionPresetFix(QFRawDataRecord* r, QString paramName, bool &value) const ;
    protected:

        /* explicitly make some functions visible again, as the C++ compiler hides function definitions
           from a base class that are also declared in the derived class, but with different parameter sets!

           see http://www.parashift.com/c++-faq-lite/strange-inheritance.html#faq-23.9
         */


        using QFFitResultsByIndexAsVectorEvaluation::getFitFunction;

};

#endif // QFIMFCCSFITEVALUATIONITEM_H
