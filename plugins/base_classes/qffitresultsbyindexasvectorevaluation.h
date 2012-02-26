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

    The changed storage only applies for index values >=0. So the average run is still stored in the standard form
*/
class QFFitResultsByIndexAsVectorEvaluation : public QFFitResultsByIndexEvaluation {
        Q_OBJECT
    public:
        QFFitResultsByIndexAsVectorEvaluation(const QString& fitFunctionPrefix, QFProject* parent);
        virtual ~QFFitResultsByIndexAsVectorEvaluation();
/*        virtual void setAllFitFixes(const QString &id, bool fix, bool currentFileOnly);
        virtual void setAllFitValues(const QString &id, double value, double error, bool currentFileOnly);
        void setFitResultValue(QFRawDataRecord *r, int index, const QString &id, double value);
        bool *allocFillFix(QFRawDataRecord *r, int index);
        double *allocFillParameterErrors(QFRawDataRecord *r, int index);
        double *allocFillParameters(QFRawDataRecord *r, int index);
        void fillFix(QFRawDataRecord *r, int index, bool *param);
        void fillParameterErrors(QFRawDataRecord *r, int index, double *param);
        void fillParameters(QFRawDataRecord *r, int index, double *param);
        bool getFitFix(QFRawDataRecord *r, int index, const QString &id);
        void setFitResultFix(QFRawDataRecord *r, int index, const QString &id, bool fix);
        void setFitFix(QFRawDataRecord *r, int index, const QString &id, bool fix);
        void setFitResultEvaluationDescription(QFRawDataRecord *r, int index, const QString &description);
        void setFitResultEvaluationGroup(QFRawDataRecord *r, int index, const QString &group);
        void setFitResultLabel(QFRawDataRecord *r, int index, const QString &parameterID, const QString &label, const QString &label_richtext);
        void setFitResultGroup(QFRawDataRecord *r, int index, const QString &parameterID, const QString &group);
        double getFitError(QFRawDataRecord *r, int index, const QString &id);
        double getFitValue(QFRawDataRecord *r, int index, const QString &id);
        void setFitResultValuesVisibleWithGroupAndLabel(QFRawDataRecord *r, int index, double *values, double *errors, const QString &group, bool *fix, const QString &fixGroup, bool sortPriority);
        void setFitResultValuesVisible(QFRawDataRecord *r, int index, double *values, double *errors);
        void setFitResultValues(QFRawDataRecord *r, int index, double *values, double *errors);
        void setFitResultError(QFRawDataRecord *r, int index, const QString &id, double error);
        void setFitResultValue(QFRawDataRecord *r, int index, const QString &id, double value, double error);
        void setFitError(QFRawDataRecord *r, int index, const QString &id, double error);
        void setFitValue(QFRawDataRecord *r, int index, const QString &id, double value);
        void setFitResultValueInt(QFRawDataRecord *r, int index, const QString &id, int64_t value, QString unit);
        void setFitResultValueInt(QFRawDataRecord *r, int index, const QString &id, int64_t value);
        void setFitResultValueBool(QFRawDataRecord *r, int index, const QString &id, bool value);
        void setFitResultValueString(QFRawDataRecord *r, int index, const QString &id, QString value);
        void setFitResultValue(QFRawDataRecord *r, int index, const QString &id, double value, QString unit);*/





//        /** \brief sets the group of the given fit result
//         *  \param r the record to adress
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
//         *  \param group group to be stored
//         */
//        virtual void setFitResultGroup(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, const QString& group);

//        /** \brief sets the label of the given fit result
//         *  \param r the record to adress
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
//         *  \param label label to be stored
//         *  \param label_richtext richtext-ed label to be stored
//         */
//        virtual void setFitResultLabel(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, const QString& label, const QString& label_richtext=QString(""));

//        /** \brief sets the group of the given evaluation result ID \a resultID
//         *  \param r the record to adress
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param group group to be stored
//         */
//        virtual void setFitResultEvaluationGroup(QFRawDataRecord* r, const QString& resultID, const QString& group);

//        /** \brief sets the description of the given evaluation result ID \a resultID
//         *  \param r the record to adress
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param description description to be stored
//         */
//        virtual void setFitResultEvaluationDescription(QFRawDataRecord* r, const QString& resultID, const QString& description);

//        /** \brief sets the group of the current fit result
//         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
//         *  \param group group to be stored
//         */
//        virtual void setFitResultGroup(const QString& parameterID, const QString& group);

//        /** \brief sets the label of the current fit result
//         *  \param r the record to adress
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
//         *  \param label label to be stored
//         *  \param label_richtext richtext-ed label to be stored
//         */
//        virtual void setFitResultLabel(const QString& parameterID, const QString& label, const QString& label_richtext=QString(""));

//        /** \brief sets the group of the current evaluation result ID \a resultID
//         *  \param r the record to adress
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param group group to be stored
//         */
//        virtual void setFitResultEvaluationGroup(const QString& group);

//        /** \brief sets the description of the current evaluation result ID \a resultID
//         *  \param r the record to adress
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param description description to be stored
//         */
//        virtual void setFitResultEvaluationDescription(const QString& description);






//        /*! \brief set a fit parameter of the current fit function (see m_fitFunction) to the specified value

//            The value may be stored at different positions:
//              - If no item is highlighted the parameter is not stored at all
//              - If a current raw data record is highlighted and no fit has been carried out so far for this record, the value
//                is stored in the preset value store of this QFEvaluationItem
//              - If a current raw data record is highlighted and a fit has already been carried out for this record, the value
//                is stored as a result in the highlighted QFRawDataRecord
//            .
//            This function also checks whether the value is to be stored (i.e. it's userEditable property is set)

//            \param r the record to adress
//            \param resultID the result ID which to access in the raw data records result store
//            \param parameterID set the value of the parameter with this id (see QFFitFunction)
//            \param value value to be stored

//         */
//        virtual void setFitValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value);

//        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         */
//        virtual void setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value);

//        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         *  \param unit unit associated with \a value
//         */
//        virtual void setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, QString unit);

//        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         */
//        virtual void setFitResultValueString(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, QString value);

//        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         */
//        virtual void setFitResultValueInt(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, int64_t value);

//        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         *  \param unit unit associated with \a value
//         */
//        virtual void setFitResultValueInt(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, int64_t value, QString unit);

//        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         */
//        virtual void setFitResultValueBool(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, bool value);

//        /** \brief stores the given value and error as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         *  \param error error associated with \a value
//         */
//        virtual void setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, double error);

//        /** \brief stores the given error as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
//         *  \param error error to be stored
//         */
//        virtual void setFitResultError(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double error);

//        /** \brief stores the given values and errors as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param values values to be stored
//         *  \param errors errors to be stored
//         */
//        virtual void setFitResultValues(QFRawDataRecord* r, const QString& resultID, double* values, double* errors);

//        /** \brief stores the given values and errors as a fit result if they are visible, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param values values to be stored
//         *  \param errors errors to be stored
//         */
//        virtual void setFitResultValuesVisible(QFRawDataRecord* r, const QString& resultID, double* values, double* errors);

//        /** \brief stores the given values and errors as a fit result if they are visible, i.e. into the currently highlighted QFRawDataRecord.
//         *          Also imports the fit parameter description from the QFFitFunction and sets the parameter group to the given value
//         *  \param r the record to adress
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param values values to be stored
//         *  \param errors errors to be stored
//         *  \param group the fit parameter group to assign
//         *  \param fix indicates whether a parameter was fixed
//         *  \param fixGroup the group the fix results should be added to
//         *  \param sortPriority should the results be assigned sort priority?
//         */
//        virtual void setFitResultValuesVisibleWithGroupAndLabel(QFRawDataRecord* r, const QString& resultID, double* values, double* errors, const QString& group, bool* fix=NULL, const QString& fixGroup=QString(""), bool sortPriority=true);



//        /*! \brief return the value of a given parameter
//            \param r the record to adress
//            \param resultID the result ID which to access in the raw data records result store
//            \param parameterID the parameter id
//            \return the value of the given parameter \a id

//            This function looks for the value at several positions:
//              -# if a fit has already been performed for the current record, the value is read from raw data record
//              -# if no fit has been performed, the value is taken from this evaluation (i.e. the preset value)
//              -# if also locally no value is available, the value is looked up in the application INI
//              -# if this also fails, the value is taken from the initial value stored in the fitFunction
//            .
//        */
//        virtual double getFitValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID);
//        /*! \brief return the fit error of a given parameter
//            \param r the record to adress
//            \param resultID the result ID which to access in the raw data records result store
//            \param parameterID the parameter id
//            \return the error associated with the given parameter.
//        */
//        virtual double getFitError(QFRawDataRecord* r, const QString& resultID, const QString& parameterID);

//        /*! \brief set the error of a given parameter

//            \param r the record to adress
//            \param resultID the result ID which to access in the raw data records result store
//            \param parameterID set the value of the parameter with this id (see QFFitFunction)
//            \param error error to be set
//        */
//        virtual void setFitError(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double error);

//        /*! \brief set the fix property of a fit parameter of the current fit function (see m_fitFunction)

//            For a description of when data is stored, see setFitValue()

//            \param r the record to adress
//            \param resultID the result ID which to access in the raw data records result store
//            \param parameterID set the value of the parameter with this id (see QFFitFunction)
//            \param fix fix to be stored

//         */
//        virtual void setFitFix(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, bool fix);

//        /** \brief stores the given fix property as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
//         *  \param fix fix to be stored
//         */
//        virtual void setFitResultFix(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, bool fix);

//        /*! \brief return the fix property of a given parameter

//            \param r the record to adress
//            \param resultID the result ID which to access in the raw data records result store
//            \param parameterID the parameter id
//            For a detailed description of where the value is searched, see getFitValue()
//        */
//        virtual bool getFitFix(QFRawDataRecord* r, const QString& resultID, const QString& parameterID);


//        /*! \brief set a fit parameter of the current fit function (see m_fitFunction) to the specified value

//            The value may be stored at different positions:
//              - If no item is highlighted the parameter is not stored at all
//              - If a current raw data record is highlighted and no fit has been carried out so far for this record, the value
//                is stored in the preset value store of this QFEvaluationItem
//              - If a current raw data record is highlighted and a fit has already been carried out for this record, the value
//                is stored as a result in the highlighted QFRawDataRecord
//            .
//            This function also checks whether the value is to be stored (i.e. it's userEditable property is set)

//            \param id set the value of the parameter with this id (see QFFitFunction)
//            \param value value to be stored

//         */
//        virtual void setFitValue(const QString& id, double value);

//        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param id set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         */
//        virtual void setFitResultValue(const QString& id, double value);

//        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param id set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         *  \param unit unit associated with \a value
//         */
//        virtual void setFitResultValue(const QString& id, double value, QString unit);

//        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param id set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         */
//        virtual void setFitResultValueString(const QString& id, QString value);

//        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param id set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         */
//        virtual void setFitResultValueInt(const QString& id, int64_t value);

//        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param id set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         *  \param unit unit associated with \a value
//         */
//        virtual void setFitResultValueInt(const QString& id, int64_t value, QString unit);

//        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param id set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         */
//        virtual void setFitResultValueBool(const QString& id, bool value);

//        /** \brief stores the given value and error as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param id set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         *  \param error error associated with \a value
//         */
//        virtual void setFitResultValue(const QString& id, double value, double error);

//        /** \brief stores the given error as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param id set the value of the parameter with this id (see QFFitFunction)
//         *  \param error error to be stored
//         */
//        virtual void setFitResultError(const QString& id, double error);

//        /** \brief stores the given values and errors as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param values values to be stored
//         *  \param errors errors to be stored
//         */
//        virtual void setFitResultValues(double* values, double* errors);

//        /** \brief stores the given values and errors as a fit result if they are visible, i.e. into the currently highlighted QFRawDataRecord
//         *  \param values values to be stored
//         *  \param errors errors to be stored
//         */
//        virtual void setFitResultValuesVisible(double* values, double* errors);

//        /*! \brief return the value of a given parameter
//            \param id the parameter id
//            \return the value of the given parameter \a id

//            This function looks for the value at several positions:
//              -# if a fit has already been performed for the current record, the value is read from raw data record
//              -# if no fit has been performed, the value is taken from this evaluation (i.e. the preset value)
//              -# if also locally no value is available, the value is looked up in the application INI
//              -# if this also fails, the value is taken from the initial value stored in the fitFunction
//            .
//        */
//        virtual double getFitValue(const QString& id);

//        /*! \brief return the fit error of a given parameter

//            \param id the parameter id
//            \return the error associated with the given parameter.
//        */
//        virtual double getFitError(const QString& id);

//        /*! \brief set the error of a given parameter

//            \param id set the value of the parameter with this id (see QFFitFunction)
//            \param error error to be set
//        */
//        virtual void setFitError(const QString& id, double error);

//        /*! \brief set the fix property of a fit parameter of the current fit function (see m_fitFunction)

//            For a description of when data is stored, see setFitValue()

//            \param id set the value of the parameter with this id (see QFFitFunction)
//            \param fix fix to be stored

//         */
//        virtual void setFitFix(const QString& id, bool fix);

//        /** \brief stores the given fix property as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param id set the value of the parameter with this id (see QFFitFunction)
//         *  \param fix fix to be stored
//         */
//        virtual void setFitResultFix(const QString& id, bool fix);

//        /*! \brief return the fix property of a given parameter

//            \param id the parameter id
//            For a detailed description of where the value is searched, see getFitValue()
//        */
//        virtual bool getFitFix(const QString& id);


//        /*! \brief set the value range of a fit parameter of the current fit function (see m_fitFunction)

//            This range is stored locally in the evaluation, never in the raw data record

//            \param id set the value of the parameter with this id (see QFFitFunction)
//            \param min lower bound for the value
//            \param max upper bound for the value

//         */
//        virtual void setFitRange(const QString& id, double min, double max);

//        /*! \brief set the value range of a fit parameter of the current fit function (see m_fitFunction)

//            This range is stored locally in the evaluation, never in the raw data record

//            \param id set the value of the parameter with this id (see QFFitFunction)
//            \param min lower bound for the value

//         */
//        virtual void setFitMin(const QString& id, double min);

//        /*! \brief set the value range of a fit parameter of the current fit function (see m_fitFunction)

//            This range is stored locally in the evaluation, never in the raw data record

//            \param id set the value of the parameter with this id (see QFFitFunction)
//            \param max upper bound for the value

//         */
//        virtual void setFitMax(const QString& id, double max);
//        /*! \brief return the lower value bound of a given parameter

//            \param id set the range min of the parameter with this id (see QFFitFunction)
//            For a detailed description of where the value is searched, see getFitValue()
//        */
//        virtual double getFitMin(const QString& id);


//        /*! \brief return the upper value bound of a given parameter

//            \param id set the range max of the parameter with this id (see QFFitFunction)
//            For a detailed description of where the value is searched, see getFitValue()
//        */
//        virtual double getFitMax(const QString& id);



//        /** \brief determine whether a fit has been carried out for the given record and resultID
//         *
//         *  \param r the given record, if \c NULL the currently highlighted record is checked
//         *  \param evalID the evaluation ID which to access in the raw data records result store
//         *  \return \c true if fit results exist for the current evaluation and fit function in the highlighted QFRawDataRecord result store.
//         *          If no record is highlighted or any other circumstances prevent the determination of a proper result, the result will be \c false!
//         */
//        virtual bool hasFit(QFRawDataRecord* r, const QString& evalID);

//        /** \brief determine whether a fit has been carried out for the given record and the currently selected resultID
//         *
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \return \c true if fit results exist for the current evaluation and fit function in the highlighted QFRawDataRecord result store.
//         *          If no record is highlighted or any other circumstances prevent the determination of a proper result, the result will be \c false!
//         */
//        virtual bool hasFit(QFRawDataRecord* r);

//        /** \brief determine whether a fit has been carried out for the currently highlighted record and the currently (implicitly) selected resultID
//         *
//         *  \return \c true if fit results exist for the current evaluation and fit function in the highlighted QFRawDataRecord result store.
//         *          If no record is highlighted or any other circumstances prevent the determination of a proper result, the result will be \c false!
//         */
//        virtual  bool hasFit();




//        /** \brief fill the given array of doubles with the current parameter values, as appropriate to use together with QFFitFunction
//         *  \param param parameter vector to fill
//         */
//        virtual void fillParameters(double* param);
//        /** \brief fill the given array of doubles with the current parameter errors, as appropriate to use together with QFFitFunction
//         *  \param param parameter error vector to fill
//        */
//        virtual void fillParameterErrors(double* param);
//        /** \brief fill the given array of doubles with the current parameter lower bounds, as appropriate to use together with QFFitFunction
//         *  \param param parameter minimums vector to fill
//        */
//        virtual void fillParametersMin(double* param);
//        /** \brief fill the given array of doubles with the current parameter upper bounds, as appropriate to use together with QFFitFunction
//         *  \param param parameter maximums vector to fill
//        */
//        virtual void fillParametersMax(double* param);
//        /** \brief fill the given array of bools with the current parameter fix values, as appropriate to use together with QFFitFunction
//         *  \param param parameter fix vector to fill
//        */
//        virtual void fillFix(bool* param);

//        /*! \brief fill a newly allocated array of doubles with the current parameter values, as appropriate to use together with QFFitFunction
//            \return parameter vector
//            \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
//        */
//        virtual double* allocFillParameters();
//        /*! \brief fill a newly allocated array of doubles with the current parameter errors, as appropriate to use together with QFFitFunction
//            \return parameter error vector
//            \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
//        */
//        virtual double* allocFillParameterErrors();
//        /*! \brief fill a newly allocated array of doubles with the current parameter lower bounds, as appropriate to use together with QFFitFunction
//            \return parameter minimums vector
//            \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
//        */
//        virtual double* allocFillParametersMin();
//        /*! \brief fill a newly allocated array of doubles with the current parameter upper bounds, as appropriate to use together with QFFitFunction
//            \return parameter maximums vector
//            \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
//        */
//        virtual double* allocFillParametersMax();
//        /*! \brief fill a newly allocated array of bools with the current parameter fix values, as appropriate to use together with QFFitFunction
//            \return parameter fix vector
//            \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
//        */
//        virtual bool* allocFillFix();





//        /** \brief fill the given array of doubles with the current parameter values, as appropriate to use together with QFFitFunction
//         *  \param r record this appplies to
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param param parameter vector to fill
//         */
//        virtual void fillParameters(QFRawDataRecord* r,  const QString& resultID, double* param);
//        /** \brief fill the given array of doubles with the current parameter errors, as appropriate to use together with QFFitFunction
//         *  \param r record this appplies to
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param param parameter error vector to fill
//        */
//        virtual void fillParameterErrors(QFRawDataRecord* r,  const QString& resultID, double* param);
//        /** \brief fill the given array of bools with the current parameter fix values, as appropriate to use together with QFFitFunction
//         *  \param r record this appplies to
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param param parameter fix vector to fill
//        */
//        virtual void fillFix(QFRawDataRecord* r,  const QString& resultID, bool* param);

//        /*! \brief fill a newly allocated array of doubles with the current parameter values, as appropriate to use together with QFFitFunction
//            \param r record this appplies to
//            \param resultID the result ID which to access in the raw data records result store
//            \return parameter vector
//            \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
//        */
//        virtual double* allocFillParameters(QFRawDataRecord* r,  const QString& resultID);
//        /*! \brief fill a newly allocated array of doubles with the current parameter errors, as appropriate to use together with QFFitFunction
//            \param r record this appplies to
//            \param resultID the result ID which to access in the raw data records result store
//            \return parameter error vector
//            \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
//        */
//        virtual double* allocFillParameterErrors(QFRawDataRecord* r,  const QString& resultID);
//        /*! \brief fill a newly allocated array of bools with the current parameter fix values, as appropriate to use together with QFFitFunction
//            \param r record this appplies to
//            \param resultID the result ID which to access in the raw data records result store
//            \return parameter fix vector
//            \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
//        */
//        virtual bool* allocFillFix(QFRawDataRecord* r,  const QString& resultID);

//        /*! \brief reset the given parameter \a id to the initial/global/default value */
//        virtual void resetDefaultFitValue(const QString& id);
//        /*! \brief reset the given parameter \a id to the initial/global/default fix */
//        virtual void resetDefaultFitFix(const QString& id);
//        /*! \brief return the default/initial/global value of a given parameter        */
//        virtual double getDefaultFitValue(const QString& id);
//        /*! \brief return the default/initial/global fix of a given parameter        */
//        virtual bool getDefaultFitFix(const QString& id);
//        /*! \brief reset the all fit results to the initial/global/default value in the currently displayed curve/data */
//        virtual void resetAllFitResultsCurrent();
//        /*! \brief reset the all fit results to the initial/global/default value in all raw data arecords */
//        virtual void resetAllFitResultsAllFiles();
//        /*! \brief reset all parameters to the initial/global/default value in current file and resultID */
//        virtual void resetAllFitValueCurrent();
//        /*! \brief reset all parameters to the initial/global/default fix in current file and resultID */
//        virtual void resetAllFitFixCurrent();


//        /*! \brief return an resultID for storage of fit results in a QFRawDataRecord results store. The ID is valid for the currently highlighted record and all current settings */
//        virtual QString getEvaluationResultID()=0;






//        /*! \brief set a fit parameter of the current fit function (see m_fitFunction) to the specified value

//            The value may be stored at different positions:
//              - If no item is highlighted the parameter is not stored at all
//              - If a current raw data record is highlighted and no fit has been carried out so far for this record, the value
//                is stored in the preset value store of this QFEvaluationItem
//              - If a current raw data record is highlighted and a fit has already been carried out for this record, the value
//                is stored as a result in the highlighted QFRawDataRecord
//            .
//            This function also checks whether the value is to be stored (i.e. it's userEditable property is set)

//            \param r the record to adress
//            \param index the index in which to adress
//            \param id set the value of the parameter with this id (see QFFitFunction)
//            \param value value to be stored

//         */
//        virtual void setFitValue(QFRawDataRecord* r, int index, const QString& id, double value);

//        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param index the index in which to adress
//         *  \param id set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         */
//        virtual void setFitResultValue(QFRawDataRecord* r, int index, const QString& id, double value);

//        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param index the index in which to adress
//         *  \param id set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         *  \param unit unit associated with \a value
//         */
//        virtual void setFitResultValue(QFRawDataRecord* r, int index, const QString& id, double value, QString unit);

//        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param index the index in which to adress
//         *  \param id set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         */
//        virtual void setFitResultValueString(QFRawDataRecord* r, int index, const QString& id, QString value);

//        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param index the index in which to adress
//         *  \param id set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         */
//        virtual void setFitResultValueInt(QFRawDataRecord* r, int index, const QString& id, int64_t value);

//        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param index the index in which to adress
//         *  \param id set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         *  \param unit unit associated with \a value
//         */
//        virtual void setFitResultValueInt(QFRawDataRecord* r, int index, const QString& id, int64_t value, QString unit);

//        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param index the index in which to adress
//         *  \param id set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         */
//        virtual void setFitResultValueBool(QFRawDataRecord* r, int index, const QString& id, bool value);

//        /** \brief stores the given value and error as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param index the index in which to adress
//         *  \param id set the value of the parameter with this id (see QFFitFunction)
//         *  \param value value to be stored
//         *  \param error error associated with \a value
//         */
//        virtual void setFitResultValue(QFRawDataRecord* r, int index, const QString& id, double value, double error);

//        /** \brief stores the given error as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param index the index in which to adress
//         *  \param id set the value of the parameter with this id (see QFFitFunction)
//         *  \param error error to be stored
//         */
//        virtual void setFitResultError(QFRawDataRecord* r, int index, const QString& id, double error);

//        /** \brief stores the given values and errors as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param index the index in which to adress
//         *  \param values values to be stored
//         *  \param errors errors to be stored
//         */
//        virtual void setFitResultValues(QFRawDataRecord* r, int index, double* values, double* errors);

//        /** \brief stores the given values and errors as a fit result if they are visible, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param index the index in which to adress
//         *  \param values values to be stored
//         *  \param errors errors to be stored
//         */
//        virtual void setFitResultValuesVisible(QFRawDataRecord* r, int index, double* values, double* errors);

//        /** \brief stores the given values and errors as a fit result if they are visible, i.e. into the currently highlighted QFRawDataRecord.
//         *          Also imports the fit parameter description from the QFFitFunction and sets the parameter group to the given value
//         *  \param r the record to adress
//         *  \param resultID the result ID which to access in the raw data records result store
//         *  \param values values to be stored
//         *  \param errors errors to be stored
//         */
//        virtual void setFitResultValuesVisibleWithGroupAndLabel(QFRawDataRecord* r, int index, double* values, double* errors, const QString& group, bool* fix=NULL, const QString& fixGroup=QString(""), bool sortPriority=true);

//        /*! \brief return the value of a given parameter
//            \param r the record to adress
//            \param index the index in which to adress
//            \param id the parameter id
//            \return the value of the given parameter \a id

//            This function looks for the value at several positions:
//              -# if a fit has already been performed for the current record, the value is read from raw data record
//              -# if no fit has been performed, the value is taken from this evaluation (i.e. the preset value)
//              -# if also locally no value is available, the value is looked up in the application INI
//              -# if this also fails, the value is taken from the initial value stored in the fitFunction
//            .
//        */
//        virtual double getFitValue(QFRawDataRecord* r, int index, const QString& id);

//        /*! \brief return the fit error of a given parameter
//            \param r the record to adress
//            \param index the index in which to adress
//            \param id the parameter id
//            \return the error associated with the given parameter.
//        */
//        virtual double getFitError(QFRawDataRecord* r, int index, const QString& id);

//        /*! \brief set the error of a given parameter

//            \param r the record to adress
//            \param index the index in which to adress
//            \param id set the value of the parameter with this id (see QFFitFunction)
//            \param error error to be set
//        */
//        virtual void setFitError(QFRawDataRecord* r, int index, const QString& id, double error);

//        /*! \brief set the fix property of a fit parameter of the current fit function (see m_fitFunction)

//            For a description of when data is stored, see setFitValue()

//            \param r the record to adress
//            \param index the index in which to adress
//            \param id set the value of the parameter with this id (see QFFitFunction)
//            \param fix fix to be stored

//         */
//        virtual void setFitFix(QFRawDataRecord* r, int index, const QString& id, bool fix);

//        /** \brief stores the given fix property as a fit result, i.e. into the currently highlighted QFRawDataRecord
//         *  \param r the record to adress
//         *  \param index the index in which to adress
//         *  \param id set the value of the parameter with this id (see QFFitFunction)
//         *  \param fix fix to be stored
//         */
//        virtual void setFitResultFix(QFRawDataRecord* r, int index, const QString& id, bool fix);

//        /*! \brief return the fix property of a given parameter

//            \param r the record to adress
//            \param index the index in which to adress
//            \param id the parameter id
//            For a detailed description of where the value is searched, see getFitValue()
//        */
//        virtual bool getFitFix(QFRawDataRecord* r, int index, const QString& id);

//        /** \brief sets the group of the given fit result
//         *  \param r the record to adress
//            \param index the index in which to adress
//         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
//         *  \param group group to be stored
//         */
//        virtual void setFitResultGroup(QFRawDataRecord* r, int index, const QString& parameterID, const QString& group);

//        /** \brief sets the label of the given fit result
//         *  \param r the record to adress
//            \param index the index in which to adress
//         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
//         *  \param label label to be stored
//         *  \param label_richtext richtext-ed label to be stored
//         */
//        virtual void setFitResultLabel(QFRawDataRecord* r, int index, const QString& parameterID, const QString& label, const QString& label_richtext=QString(""));

//        /** \brief sets the group of the given evaluation result ID \a resultID
//         *  \param r the record to adress
//            \param index the index in which to adress
//         *  \param group group to be stored
//         */
//        virtual void setFitResultEvaluationGroup(QFRawDataRecord* r, int index, const QString& group);

//        /** \brief sets the description of the given evaluation result ID \a resultID
//         *  \param r the record to adress
//            \param index the index in which to adress
//         *  \param description description to be stored
//         */
//        virtual void setFitResultEvaluationDescription(QFRawDataRecord* r, int index, const QString& description);

//        /** \brief fill the given array of doubles with the current parameter values, as appropriate to use together with QFFitFunction
//         *  \param r record this appplies to
//         *  \param index index in \a r this applies to
//         *  \param param parameter vector to fill
//         */
//        virtual void fillParameters(QFRawDataRecord* r, int index, double* param);
//        /** \brief fill the given array of doubles with the current parameter errors, as appropriate to use together with QFFitFunction
//         *  \param r record this appplies to
//         *  \param index index in \a r this applies to
//         *  \param param parameter error vector to fill
//        */
//        virtual void fillParameterErrors(QFRawDataRecord* r, int index, double* param);
//        /** \brief fill the given array of bools with the current parameter fix values, as appropriate to use together with QFFitFunction
//         *  \param r record this appplies to
//         *  \param index index in \a r this applies to
//         *  \param param parameter fix vector to fill
//        */
//        virtual void fillFix(QFRawDataRecord* r, int index, bool* param);

//        /*! \brief fill a newly allocated array of doubles with the current parameter values, as appropriate to use together with QFFitFunction
//            \param r record this appplies to
//            \param index index in \a r this applies to
//            \return parameter vector
//            \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
//        */
//        virtual double* allocFillParameters(QFRawDataRecord* r, int index);
//        /*! \brief fill a newly allocated array of doubles with the current parameter errors, as appropriate to use together with QFFitFunction
//            \param r record this appplies to
//            \param index index in \a r this applies to
//            \return parameter error vector
//            \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
//        */
//        virtual double* allocFillParameterErrors(QFRawDataRecord* r, int index);
//        /*! \brief fill a newly allocated array of bools with the current parameter fix values, as appropriate to use together with QFFitFunction
//            \param r record this appplies to
//            \param index index in \a r this applies to
//            \return parameter fix vector
//            \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
//        */
//        virtual bool* allocFillFix(QFRawDataRecord* r, int index);

//        /** \brief determine whether a fit has been carried out for the given record and index
//         *
//         *  \param r the given record, if \c NULL the currently highlighted record is checked
//         *  \param index the index to check
//         *  \return \c true if fit results exist for the current evaluation and fit function in the highlighted QFRawDataRecord result store.
//         *          If no record is highlighted or any other circumstances prevent the determination of a proper result, the result will be \c false!
//         */
//        virtual bool hasFit(QFRawDataRecord* r, int index);

//        /*! \brief set the given parameter \a id to the given value (and error) in all files and all indexes */
//        virtual void setAllFitValues(const QString& id, double value, double error=0.0, bool currentFileOnly=false);
//        /*! \brief set the given parameter \a id to the given fix value and all indexes */
//        virtual void setAllFitFixes(const QString& id, bool fix, bool currentFileOnly=false);
//        /*! \brief reset all parameters to the initial/global/default value in all files and all indexes */
//        virtual void resetAllFitValue();
//        /*! \brief reset all fit results to the initial/global/default value in all files and all indexes */
//        virtual void resetAllFitResults();
//        /*! \brief reset all parameters to the initial/global/default fix in all files and all indexes */
//        virtual void resetAllFitFix();
//        /*! \brief reset all fit results to the initial/global/default value in the current file but all indexes */
//        virtual void resetAllFitResultsCurrentFileAllIndices();
//        /*! \brief reset all fit results to the initial/global/default value in the current file but all indexes */
//        virtual void resetAllFitResultsAllFilesAllIndices();

};

#endif // QFFITRESULTSBYINDEXASVECTOREVALUATION_H
