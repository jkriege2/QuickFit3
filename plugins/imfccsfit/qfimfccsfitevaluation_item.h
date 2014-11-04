/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFIMFCCSFITEVALUATIONITEM_H
#define QFIMFCCSFITEVALUATIONITEM_H

#include <QMap>
#include <QSettings>

#include "qfevaluationitem.h"
#include "qfevaluationitemfactory.h"
#include "qffitresultsbyindexasvectorevaluation.h"
#include "qfrdrfcsdatainterface.h"
#include "qfrdrsimplecountrateinterface.h"
#include "qfrdrcountratesinterface.h"
#include "qffitresultsbyindexevaluationfittools.h"
#include "qffcsweightingtools.h"
#include "qfevaluationpropertyeditor.h"
#include "qfimfccsparameterinputtable.h"
#include "qfmatchrdrfunctor.h"
#include "qffitfunction.h"
#include "qfglobalfittool.h"
#include "qfimfccsfilesetsmodel.h"

class QFImFCCSMatchRDRFunctor: public QFMatchRDRFunctor {
    public:
        explicit QFImFCCSMatchRDRFunctor();
        /** \brief reimplement this function to check whether a QFRawDataRecord* is contained in a QFSelectRDRDialog. the default implementation always returns \c true . */
        virtual bool matches(const QFRawDataRecord* record) const ;
};

/*! \brief evaluation item class 
    \ingroup qf3rdrdp_imfccsfit


    
    
*/
class QFImFCCSFitEvaluationItem : public QFFitResultsByIndexAsVectorEvaluation, public QFFitResultsByIndexMultiRDREvaluationFitTools, public QFFCSWeightingTools {
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

        /** \brief calculates fit statistics for the given fit function and dataset. */
        QFFitStatistics calcFitStatistics(bool storeAsResults, QFFitFunction* ffunc, long N, double* tauvals, double* corrdata, double* weights, int datacut_min, int datacut_max, double* fullParams, double* errorsVector, bool* paramsFix, int runAvgWidth, int residualHistogramBins, QFRawDataRecord* record=NULL, int run=-1);


        QFEvaluationRawDataModelProxy* getRawDataProxyModel() const;

        virtual QFRawDataRecord* getFitFile(int num) const;
        virtual QFFitFunction* getFitFunction(int num) const;
        QStringList getFitFunctionIDs() const;
        virtual QFFitFunction* getFitFunctionForID(QString id) const;
        virtual QString getFitFunctionID(int num) const;
        virtual QString getFitFunctionID() const;
        virtual QList<QFRawDataRecord*> getFitFiles() const;

        virtual QFFitFunction* getFitFunction(QFRawDataRecord *rdr) const;
        virtual QString getFitFunctionID( QFRawDataRecord* rdr) const;

        /** \brief if this returns a non-empty string (default: empty string), the result is added to the getParameterStoreID() to make the store specific to the given \a rdr */
        virtual QString rdrPointerToParameterStoreID(QFRawDataRecord* rdr) const;

        QFImFCCSParameterInputTable* getParameterInputTableModel() const;

        int getFitFileCount() const;
        QFMatchRDRFunctor* getMatchFunctor() const;
        /** \brief set the current index to use */
        virtual int getCurrentIndex() const;


        /** \brief returns the number of currently used link parameters
         *
         *  \note This function is reltively costy, as it iterates over all elements in globalParams, to find the maximum value!
         */
        int getLinkParameterCount() const;
        /** \brief returns the global_param of the given file and fit parameter, or -1 if none */
        int getLinkParameter(int file, QString parameter) const;
        /** \brief get a list if items a given parameter is linked to */
        QList<QPair<int, QString> > getLinkedParameterList(int file, QString parameter) const;

        /** \brief returns a list of file sets (lists of QFRawDataRecord s) that have already been fitted */
        const QList<QList<QFRawDataRecord* > >& getFittedFiles() const;
        /** \brief returns a guessed list of other file sets  */
        const QList<QList<QFRawDataRecord* > >& getGuessedFiles() const;
        void removeFittedFileSet(const QList<QFRawDataRecord* >& fileset);
        QFImFCCSFileSetsModel* getFileSetsModel() const;
        void clearFittedFileSets();

        //void loadParametersFromFile(const QString& filename);
        //void save ParametersToFile(const QString& filename);

    public slots:
        void guessFileSets(const QList<QFRawDataRecord* >& fileset, bool emitChangedSignal=true);
        void addFittedFileSet(const QList<QFRawDataRecord* >& fileset);
        void setFitFiles(const QList<QFRawDataRecord*>& records);
        void setFitFileSet(int idx);
        void setFitFile(int num, QFRawDataRecord *record);
        virtual void setFitFunction(int num, QString fitFunction);
        virtual void setFitFunction(QString fitFunction);
        virtual int addFitFile();
        virtual void removeFitFile();
        virtual void removeFitFile(int i);
        /** \brief set the current index to use */
        virtual void setCurrentIndex(int index);
        /** \brief set the global parameter of the given file and fit parameter \a parameter therein */
        virtual void setLinkParameter(int file, QString parameter, int global_param);
        /** \brief set the global parameter of the given file and fit parameter \a parameter therein */
        virtual void unsetLinkParameter(int file, QString parameter);
        /** \brief clear all linked parameters in the given file or all linked parameters, if \c file<0 */
        virtual void clearLinkParameters(int file=-1);
    protected slots:

        /** \brief check all fit file sets, whether they contain non-existing RDRs ... if so, remove them! */
        void checkAndCleanFitFileSets();


    signals:
        void fileChanged(int num, QFRawDataRecord* record);
        void filesetsChanged();
    protected:

        struct doFitData {
            double* params;
            double* initialparams;
            double* errors;
            double* errorsI;
            double* paramsMin;
            double* paramsMax;
            bool* paramsFix;

            QFFitFunction* ffunc;
            int cut_low;
            int cut_up;
            int cut_N;

            double* weights;
            double* taudata;
            double* corrdata;
            long N;
            bool weightsOK;

            bool emitSignals;
        };

        void freeAndClearDoFitDataList(QList<doFitData>& fitData) const;


        QFImFCCSMatchRDRFunctor* matchFunctor;
        /** \brief proxy model to filter rdrModel */
        QFEvaluationRawDataModelProxy* rdrProxy;

        QFImFCCSParameterInputTable* paramTable;

        QFImFCCSFileSetsModel* fileSetsModel;
        /** \brief a list of all file sets (list of QFRawDataRecord s) that have already been fited */
        QList<QList<QFRawDataRecord* > > fittedFileSets, guessedFileSets;
        QFImFCCSFileSetsModel* fittedFileSetsModel;


        /** \brief this list stores the several fit functions selected in the editor */
        QMap<int, QString> m_multiFitFunctions;
        /** \brief files for a fit */
        QList<QPointer<QFRawDataRecord> > fitFilesList;


        /*! \brief list of linked parameters

            The first key is the file number, the second is the parameter in this file. The value finally
            describes the number/ID of the global parameter the given parameter is linked to:

            \c <file, fit_param; global_param>
         */
        QMap<int, QMap<QString, int> > globalParams;
        mutable bool globalParamsChanged;
        mutable int lastGlobalParamsCount;

        /** \brief thread used for fitting */
        QFGlobalThreadedFit* doFitThread;


        /** \brief makes sure that at least one fit file is in the list */
        void ensureFitFiles();


        /** \brief write object contents into XML file
        *
        *  this function saves the id of the current fit function and algorithm, as well as the contents of parameterStore to
        *  the given XML file.
        */
        virtual void intWriteData(QXmlStreamWriter& w) const;
        /** \brief read back the data stored by intWriteXML() */
        virtual void intReadData(QDomElement* e);


        /** \brief determines whether this evaluation is applicable to a given raw data record. This method is used to generate the
         *         list of raw data records presented to the user */
        virtual bool isApplicable(const QFRawDataRecord* record);

        /*! \copydoc QFFitResultsEvaluation::intWriteDataAlgorithm()      */
        virtual void intWriteDataAlgorithm(QXmlStreamWriter& w) const;

        /*! \copydoc QFFitResultsEvaluation::intReadDataAlgorithm()      */
        virtual void intReadDataAlgorithm(QDomElement& e);

        virtual bool hasSpecial(const QFRawDataRecord* r, const QString& id, const QString& paramid, double& value, double& error) const;
        virtual bool hasSpecial(const QFRawDataRecord* r, int index, const QString& paramid, double& value, double& error) const ;


        virtual bool overrideFitFunctionPreset(QFRawDataRecord* r, QString paramName, double &value) const ;
        virtual bool overrideFitFunctionPresetError(QFRawDataRecord* r, QString paramName, double &value) const ;
        virtual bool overrideFitFunctionPresetFix(QFRawDataRecord* r, QString paramName, bool &value) const ;

    public:
        /*! \brief perform a fit for the given \a records and \a run

            The parameters \a defaultMinDatarange and \a defaultMaxDatarange set the range of data points taken for the fit.
            If both are -1, the full range is used

            The object \a dlgFitProgress (if supplied) is used to report the progress and to check whether the user clicked "Cancel".

            \note this method is intended to perform the fits one after the other or single fits. It will internally starts the fit algorithm
                  in its own thread. If you want to control the multithreading by yourself, use doFitForMultithread() instead !!!
          */
        virtual void doFit(const QList<QFRawDataRecord*>& records, int run, int defaultMinDatarange=-1, int defaultMaxDatarange=-1, QFFitAlgorithmReporter* dlgFitProgress=NULL, bool doLog=false);

        /*! \brief perform a fit for the given \a record and \a run

            The parameters \a defaultMinDatarange and \a defaultMaxDatarange set the range of data points taken for the fit.
            If both are -1, the full range is used

            The object \a dlgFitProgress (if supplied) is used to report the progress and to check whether the user clicked "Cancel".
          */
        virtual void doFitForMultithread(const QList<QFRawDataRecord*>& records, int run, int defaultMinDatarange=-1, int defaultMaxDatarange=-1, QFPluginLogService *logservice=NULL) const;

        /** \brief initialize a QFGlobalFitTool and several arrays with values needed to perform a global fit for a given run and list of files */
        void setupGlobalFitTool(QFGlobalFitTool& tool, QList<doFitData>* fitDataOut, QString& iparams, QList<double *> &paramsVector, QList<double *> &initialParamsVector, QList<double*>& errorsVector, QList<double*>& errorsVectorI, const QList<QFRawDataRecord *> &records, int run, int rangeMinDatarange, int rangeMaxDatarange, bool doLog) const;

        /*! \brief fill a 2D array with a chi^2 landscape for the given fit, this is called like doFit() + some output arguments!

            This method evaluates the chi2 landscape for a range of values of two parameters identified by the file/model of the parameter and it's ID therein. The output image
            has to have size paramXValues.size()*paramYvalues.size().

            Global parameters are resolved internally!
         */
        virtual void calcChi2Landscape(double* chi2Landscape, int paramXFile, int paramXID, const QVector<double>& paramXValues, int paramYFile, int paramYID, const QVector<double>& paramYvalues, const QList<QFRawDataRecord*>& records, int run, int defaultMinDatarange=-1, int defaultMaxDatarange=-1);


    protected:

        /* explicitly make some functions visible again, as the C++ compiler hides function definitions
           from a base class that are also declared in the derived class, but with different parameter sets!

           see http://www.parashift.com/c++-faq-lite/strange-inheritance.html#faq-23.9
         */


        using QFFitResultsByIndexAsVectorEvaluation::getFitFunction;

};

#endif // QFIMFCCSFITEVALUATIONITEM_H
