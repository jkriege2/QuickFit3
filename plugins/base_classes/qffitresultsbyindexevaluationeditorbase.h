#ifndef QFFITRESULTSBYINDEXEVALUATIONEDITORBASE_H
#define QFFITRESULTSBYINDEXEVALUATIONEDITORBASE_H

#include "qffitresultsevaluationeditorbase.h"
#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include "qffitresultsbyindexevaluation.h"
#include "qffitresultsevaluation.h"

/*! \brief evaluation item editor base class for data fits using QFFitAlgorithm and QFFitFunction where the
           QFRawDataRecord is a dataset with different runs (we can fit to each run). The QFEvaluationItem for
           this editor mus cast to QFFitResultsByIndexEvaluation.
    \ingroup qf3evaluationplugins


*/
class QFFitResultsByIndexEvaluationEditorBase : public QFFitResultsEvaluationEditorBase {
        Q_OBJECT
    public:
        /** \brief class constructor */
        explicit QFFitResultsByIndexEvaluationEditorBase(QString iniPrefix, QFPluginServices* services, QWidget *parent = 0);
        /** \brief get the lower datacut for the current record */
        virtual int getUserMin(int defaultMin=0);
        /** \brief get the upper datacut for the current record */
        virtual int getUserMax(int defaultMax=1);

        /** \brief get the lower datacut for the current record */
        virtual int getUserMin(QFRawDataRecord* rec, int index, int defaultMin=0);
        /** \brief get the upper datacut for the current record */
        virtual int getUserMax(QFRawDataRecord* rec, int index, int defaultMax=1);
    signals:

    public slots:
        /** \brief copy min cut to all files and all runs therein (except the current run in the current file) */
        void copyUserMinToAll(int userMin);

        /** \brief copy max cut to all files and all runs therein (except the current run in the current file) */
        void copyUserMaxToAll(int userMax);

        /** \brief copy min and max cut to all files and all runs therein (except the current run in the current file) */
        void copyUserMinMaxToAll(int userMin, int userMax);

        /** \brief copy min cut to all runs (except the current run) */
        void copyUserMinToAllRuns(int userMin);

        /** \brief copy min and max cut to all runs (except the current run) */
        void copyUserMinMaxToAllRuns(int userMin, int userMax);

        /** \brief copy max cut to all runs (except the current run) */
        void copyUserMaxToAllRuns(int userMax);

        /** \brief set the lower datacut for the current record */
        virtual void setUserMin(int userMin);

        /** \brief set the upper datacut for the current record */
        virtual void setUserMax(int userMax);

        /** \brief set the upper and lower datacut for the current record */
        virtual void setUserMinMax(int userMin, int userMax);
};

#endif // QFFITRESULTSBYINDEXEVALUATIONEDITORBASE_H
