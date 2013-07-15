#ifndef QFFITRESULTSBYINDEXEVALUATIONFITTOOLS_H
#define QFFITRESULTSBYINDEXEVALUATIONFITTOOLS_H
#include <QMutex>
#include "qfrawdatarecord.h"
#include "qffitalgorithm.h"
#include "qfpluginservices.h"

class QFFitResultsByIndexEvaluationFitToolsBase {
    public:
        QFFitResultsByIndexEvaluationFitToolsBase();

        virtual ~QFFitResultsByIndexEvaluationFitToolsBase();

        /** \brief calculates fit statistics for the given fit function and dataset. */
        virtual QFFitStatistics calcFitStatistics(bool storeAsResults, QFFitFunction* ffunc, long N, double* tauvals, double* corrdata, double* weights, int datacut_min, int datacut_max, double* fullParams, double* errors, bool* paramsFix, int runAvgWidth, int residualHistogramBins, QFRawDataRecord* record=NULL, int run=-1);

    protected:

        mutable QMutex* mutexThreadedFit;

};

class QFFitResultsByIndexEvaluationFitTools: public QFFitResultsByIndexEvaluationFitToolsBase
{
    public:


        /*! \brief perform a fit for the given \a record and \a run

            The parameters \a defaultMinDatarange and \a defaultMaxDatarange set the range of data points taken for the fit.
            If both are -1, the full range is used

            The object \a dlgFitProgress (if supplied) is used to report the progress and to check whether the user clicked "Cancel".
          */
        virtual void doFit(QFRawDataRecord* record, int run, int defaultMinDatarange=-1, int defaultMaxDatarange=-1, QFFitAlgorithmReporter* dlgFitProgress=NULL, bool doLog=false)=0;
        /*! \brief perform a fit for the given \a record and \a run

            The parameters \a defaultMinDatarange and \a defaultMaxDatarange set the range of data points taken for the fit.
            If both are -1, the full range is used

            The object \a dlgFitProgress (if supplied) is used to report the progress and to check whether the user clicked "Cancel".
          */
        virtual void doFitForMultithread(QFRawDataRecord* record, int run, int defaultMinDatarange=-1, int defaultMaxDatarange=-1, QFPluginLogService *logservice=NULL) const =0;




};



class QFFitResultsByIndexMultiRDREvaluationFitTools: public QFFitResultsByIndexEvaluationFitToolsBase
{
    public:



        /*! \brief perform a fit for the given \a records and \a run

            The parameters \a defaultMinDatarange and \a defaultMaxDatarange set the range of data points taken for the fit.
            If both are -1, the full range is used

            The object \a dlgFitProgress (if supplied) is used to report the progress and to check whether the user clicked "Cancel".
          */
        virtual void doFit(const QList<QFRawDataRecord*>& records, int run, int defaultMinDatarange=-1, int defaultMaxDatarange=-1, QFFitAlgorithmReporter* dlgFitProgress=NULL, bool doLog=false)=0;
        /*! \brief perform a fit for the given \a records and \a run

            The parameters \a defaultMinDatarange and \a defaultMaxDatarange set the range of data points taken for the fit.
            If both are -1, the full range is used

            The object \a dlgFitProgress (if supplied) is used to report the progress and to check whether the user clicked "Cancel".
          */
        virtual void doFitForMultithread(const QList<QFRawDataRecord*>& records, int run, int defaultMinDatarange=-1, int defaultMaxDatarange=-1, QFPluginLogService *logservice=NULL) const=0;

};
#endif // QFFITRESULTSBYINDEXEVALUATIONFITTOOLS_H
