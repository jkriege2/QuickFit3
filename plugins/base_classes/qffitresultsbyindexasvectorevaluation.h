#ifndef QFFITRESULTSBYINDEXASVECTOREVALUATION_H
#define QFFITRESULTSBYINDEXASVECTOREVALUATION_H

#include "qffitresultsbyindexevaluation.h"
#include "qffitresultsevaluation.h"

#include <QHash>
#include <QMap>
#include <QSettings>

#include "qfevaluationitem.h"
#include "qfevaluationitemfactory.h"
#include "qffitfunction.h"
#include "qffitalgorithm.h"
#include "qffitparameterbasicinterface.h"

/*! \brief evaluation item base class that changes the methods in QFFitResultsByIndexEvaluation so they store fit parameters as vectors, not as single values
    \ingroup qf3evaluationplugins

    The changed storage only applies for index values >=0. So the average run is still stored in the standard form.
    The parameters are stored as lists of values indexed by the given run/index. To distinguish between local and global parameters, a second
    list of booleans result is used for every parameter, indicating whether it is local or global. the name of this second list is created
    from the original parameterID using getParamNameLocalStore().

    To implement this behaviour this class reimplements all functions from its ancestors that write directly to \c QFRawDaataRecord::results... functions.
*/
class QFFitResultsByIndexAsVectorEvaluation : public QFFitResultsByIndexEvaluation {
        Q_OBJECT
    public:
        QFFitResultsByIndexAsVectorEvaluation(const QString& fitFunctionPrefix, QFProject* parent);
        virtual ~QFFitResultsByIndexAsVectorEvaluation();


        virtual QString transformResultID(const QString& resultID) const;

        virtual void resetDefaultFitValue(const QString &id);
        virtual void resetDefaultFitFix(const QString &id);
        virtual void resetAllFitResultsCurrent();
        virtual void resetAllFitResults(QFRawDataRecord* r, const QString &resultID);
        virtual bool hasFit(QFRawDataRecord *r1, const QString &evalID);



        void setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double value);
        void setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double value, QString unit);
        void setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double value, double error);
        void setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double value, double error, QString unit);
        void setFitResultError(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double error);
        void setFitResultValueString(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, QString value);
        void setFitResultValueInt(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, int64_t value, QString unit);
        void setFitResultValueInt(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, int64_t value);
        void setFitResultValueBool(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, bool value);
        void setFitResultValues(QFRawDataRecord *r, const QString &resultID, double *values, double *errors);
        void setFitResultValuesVisibleWithGroupAndLabel(QFRawDataRecord *r, const QString &resultID, double *values, double *errors, const QString &group, bool *fix, const QString &fixGroup, bool sortPriority);
        void setFitResultValuesVisible(QFRawDataRecord *r, const QString &resultID, double *values, double *errors);
        void setFitResultFix(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, bool fix);
        void setFitResultGroup(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, const QString &group);
        void setFitResultLabel(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, const QString &label, const QString &label_richtext);
        double getFitError(QFRawDataRecord *r, const QString &resultID, const QString &parameterID);
        double getFitValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID);
        bool getFitFix(QFRawDataRecord *r, const QString &resultID, const QString &parameterID);
protected:
        virtual QString getParamNameLocalStore(const QString& paramID) const;
        virtual bool isParamNameLocalStore(const QString& paramID) const;
};

#endif // QFFITRESULTSBYINDEXASVECTOREVALUATION_H



/*
  TODO: overwrite function implementations in QFFitResultsByIndexEvaluation that use ->results... from QFRawDataRecord
        ... reenable the imfcsfit plugin!

 */



/*

    bool doEmit=r->isEmitResultsChangedEnabled();
    r->disableEmitResultsChanged();
    if (doEmit) r->enableEmitResultsChanged(true);

  */
