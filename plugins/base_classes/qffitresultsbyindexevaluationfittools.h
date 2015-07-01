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
        virtual QFFitStatistics calcFitStatistics(bool storeAsResults, QFFitFunction* ffunc, long N, const double* tauvals, const double* corrdata, const double* weights, int datacut_min, int datacut_max, const double* fullParams, const double* errors, const bool* paramsFix, int runAvgWidth, int residualHistogramBins, QFRawDataRecord* record=NULL, int run=-1, const QString &prefix=QString("fitstat_"), const QString &pgroup=QString("fit statistics"), const QVector<double>& COV=QVector<double>(), double paramrange_size=200, bool storeCOV=false);

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

            \note THIS FUNCTION NEEDS TO BE THREAD SAFE!!!
          */
        virtual void doFitForMultithread(QFFitAlgorithm* falg, QFFitFunction* ffunc, QFRawDataRecord* record, int run, int defaultMinDatarange=-1, int defaultMaxDatarange=-1, QFPluginLogService *logservice=NULL) const =0;


        /*! \brief perform a fit for the given \a record and \a run

            The parameters \a defaultMinDatarange and \a defaultMaxDatarange set the range of data points taken for the fit.
            If both are -1, the full range is used

            The object \a dlgFitProgress (if supplied) is used to report the progress and to check whether the user clicked "Cancel".

            \note THIS FUNCTION NEEDS TO BE THREAD SAFE!!!
          */
        virtual void doFitForMultithreadReturn(QFRawDataRecord::QFFitFitResultsStore& result, const QFRawDataRecord* record, int run, int defaultMinDatarange=-1, int defaultMaxDatarange=-1, QFPluginLogService *logservice=NULL) const =0;


        /*! \brief This function should return a usable QFFitAlgoruthm and QFFitFunction as freshly created objects for the given record and run
         *
         * \note This does not have to be thread safe!!!
         */
        virtual void createFitFunctionAndAlgorithm(QFFitAlgorithm*& falg, QFFitFunction*& ffunc, const QFRawDataRecord* record, int run)=0;

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

        /*! \brief perform a fit for the given \a record and \a run

            The parameters \a defaultMinDatarange and \a defaultMaxDatarange set the range of data points taken for the fit.
            If both are -1, the full range is used

            The object \a dlgFitProgress (if supplied) is used to report the progress and to check whether the user clicked "Cancel".
          */
        virtual void doFitForMultithreadReturn(QList<QFRawDataRecord::QFFitFitResultsStore>& result, const QList<const QFRawDataRecord*>& records, const QStringList& fitfunctionIDs, int run, int defaultMinDatarange=-1, int defaultMaxDatarange=-1, QFPluginLogService *logservice=NULL) const=0;

};
#endif // QFFITRESULTSBYINDEXEVALUATIONFITTOOLS_H
