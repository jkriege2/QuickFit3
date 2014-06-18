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
        explicit QFFitResultsByIndexEvaluationEditorBase(QString iniPrefix, QFEvaluationPropertyEditor* propEditor, QFPluginServices* services, QWidget *parent = 0);


        /** \brief get the lower datacut for the current record, reimplement this by calling getUserMin(QFRawDataRecord*,int,int) with a viable choice for \a defaultMin */
        virtual int getUserMin(QFRawDataRecord* rec, int index)=0;
        /** \brief get the upper datacut for the current record, reimplement this by calling getUserMin(QFRawDataRecord*,int,int) with a viable choice for \a defaultMax */
        virtual int getUserMax(QFRawDataRecord* rec, int index)=0;
        /** \brief get the lower datacut for the current record, the \a defaultMin should be read from editors current data cut widget */
        virtual int getUserMin(QFRawDataRecord* rec, int index, int defaultMin);
        /** \brief get the upper datacut for the current record, the \a defaultMax should be read from editors current data cut widget */
        virtual int getUserMax(QFRawDataRecord* rec, int index, int defaultMax);

        /** \brief get the lower datacut for the current record, reimplement this by calling getUserMin(QFRawDataRecord*,int) with a viable choice for \a defaultMin */
        virtual int getUserMin(QFRawDataRecord* rec);
        /** \brief get the upper datacut for the current record, reimplement this by calling getUserMin(QFRawDataRecord*,int) with a viable choice for \a defaultMax */
        virtual int getUserMax(QFRawDataRecord* rec);
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



        /** \brief reset current */
        virtual void resetCurrent();
        /** \brief reset all files */
        virtual void resetAll();
        /** \brief copy to all files, all runs */
        virtual void copyToAll();
        /** \brief reset all runs in current file */
        virtual void resetAllRuns();
        /** \brief copy to all runs in current file */
        virtual void copyToAllRuns();
        /** \brief copy to all files, but only current run */
        virtual void copyToAllCurrentRun();

        /** \brief store a parameter image (user setable) as fit results */
        virtual void setParamAllIndexes(QFRawDataRecord* rec, const QString& parameter, const QVector<double>& datar, const QVector<double> &dataerror=QVector<double>());
        /** \brief store a parameter image (user setable) as fit results */
        virtual void setParamAllIndexesInCurrent(const QString& parameter, const QVector<double>& data);


        void log_text(QString message);
        void log_warning(QString message);
        void log_error(QString message);


    public:

        /* explicitly make some functions visible again, as the C++ compiler hides function definitions
           from a base class that are also declared in the derived class, but with different parameter sets!

           see http://www.parashift.com/c++-faq-lite/strange-inheritance.html#faq-23.9
         */

        using QFFitResultsEvaluationEditorBase::getUserMin;
        using QFFitResultsEvaluationEditorBase::getUserMax;

};

#endif // QFFITRESULTSBYINDEXEVALUATIONEDITORBASE_H
