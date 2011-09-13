#ifndef QFFITRESULTSEVALUATION_H
#define QFFITRESULTSEVALUATION_H

#include <QHash>
#include <QMap>
#include <QSettings>

#include "qfevaluationitem.h"
#include "qfevaluationitemfactory.h"
#include "qffitfunction.h"
#include "qffitalgorithm.h"
#include "qffitparameterbasicinterface.h"

/*! \brief evaluation item base class for data fits using QFFitAlgorithm and QFFitFunction
    \ingroup qf3evaluationplugins

    Features of this class are:
      - it allows to easily implement evaluation items which store fit results to the raw data records.
      - The fit algorithm and model/function may be selected globally.
      - parameter value ranges and fix-indicators are set globally for all records
      - fit parameter values and errors are stored locally in the raw data record after the first fit (hasFit() returns \c true ).
        Before the first fit they are stored globally. So the user first sees the global parameters which are valid as starting
        point for every fit. Then he  runs a fit and ends up woth the local parameters. This way it is always possible to
        go back to the initial/global values to reset the fit and set the initial values for a whole bunch of files at once.
      - The \c getFit...() and \c setFit...() functions already care for the structure described in the last topic. They are defined in different flavors:
          -# The most basic versions (which also really contain the implementation) have a QFRawDataRecord, the \c resultID and a \a parameterID
             as parameters. This triplet allows to access a value from the result store in a QFRawDataRecord and also the internal
             datastore, if no fit has been carried out yet. The rusultID is created by functions called getEvaluationResultID() with
             different sets of parameters. The resultID (a short string) should be built to include all important settings that
             distinguish different parameter sets, i.e. the ID of the fit function and maybe something like the
             selected run. Here is an example:
             \code QString("%1_%2_%3_r%4").arg(getType()).arg(getID()).arg(fitFunctionID).arg(currentRun)\endcode
             Usually you won't use
          -# In addition there are also functions to access the values of the currently selected/highlighted record. These rely on
             an implementtaion of getEvaluationResultID() with no parameters, which means to take the currently selected
             fit function, fit algorithm, ...
        .
      - the global fit parameters are stored internally in the protected member parameterStore. They are also saved to
        the project file (see intWriteData() and intReadData() ). If you want to save additional data either implement these
        functions in your class and call the functions from this class explicitly (using e.g. <code>QFFitResultsEvaluation::intWriteData()</code>),
        or implement the functions intReadDataFitParam() and intWriteDataFitParam() which allow to save additional data for every
        fit parameter.
      - the initial values for fit parameters are read from the initial values given in the QFFitFunction. As a second option this
      class opens the file \code <configDir>/fitparams_<type>.ini\endcode and reads the parameters from it in the form \code
[fitFunctionID]
param=<initial value>
param_min=<initial_range_min>
param_max=<initial_range_max>
param_fix=<initial_fix>
      \endcode
    .

*/
class QFFitResultsEvaluation : public QFEvaluationItem, public QFFitParameterBasicInterface {
    Q_OBJECT
public:
    /*! \brief class conrtuctor

        \param fitFunctionPrefix only fit functions with this prefix will be used by this object, leave empty to use all
        \param parent project this object belongs to
     */
    QFFitResultsEvaluation(const QString& fitFunctionPrefix, QFProject* parent);
    virtual ~QFFitResultsEvaluation();

    /** \brief returns a list of all FCS fitting functions available for this evaluation
     *  \note This class uses only those fit functions where the ID starts with \c "fcs_"
     */
    virtual QStringList getAvailableFitFunctions() const;
    /** \brief returns the given fit function instance (DO NEVER DELETE THIS INSTANCE!) */
    virtual QFFitFunction* getFitFunction(const QString& parameterID) const;

    /** \brief set the current fitting algorithm by ID */
    virtual void setFitAlgorithm(QString fitAlgorithm);

    /** \brief get the current fitting algorithm */
    virtual QFFitAlgorithm* getFitAlgorithm() const;

    /** \brief returns a list of all available fit algorithms */
    virtual QStringList getAvailableFitAlgorithms() const;

    /** \brief return the instance of a given fit algorithm for this evaluation (DO NEVER DELETE THIS INSTANCE!)  */
    virtual QFFitAlgorithm* getFitAlgorithm(const QString& parameterID) const;

    /*! \brief set the current fitting algorithm

        This function also tries to transform the parameters between different models. The transformation is based on the parameter
        name. If the new model has some parameters with the same name as the old model, the new model parameters are set to the value
        of the old parameter.
    */
    virtual void setFitFunction(QString fitFunction);

    /** \brief get the current fitting algorithm */
    virtual QFFitFunction* getFitFunction();









    /** \brief sets the group of the given fit result
     *  \param r the record to adress
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
     *  \param group group to be stored
     */
    virtual void setFitResultGroup(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, const QString& group);

    /** \brief sets the label of the given fit result
     *  \param r the record to adress
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
     *  \param label label to be stored
     *  \param label_richtext richtext-ed label to be stored
     */
    virtual void setFitResultLabel(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, const QString& label, const QString& label_richtext=QString(""));

    /** \brief sets the group of the given evaluation result ID \a resultID
     *  \param r the record to adress
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param group group to be stored
     */
    virtual void setFitResultEvaluationGroup(QFRawDataRecord* r, const QString& resultID, const QString& group);

    /** \brief sets the description of the given evaluation result ID \a resultID
     *  \param r the record to adress
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param description description to be stored
     */
    virtual void setFitResultEvaluationDescription(QFRawDataRecord* r, const QString& resultID, const QString& description);

    /** \brief sets the group of the current fit result
     *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
     *  \param group group to be stored
     */
    virtual void setFitResultGroup(const QString& parameterID, const QString& group);

    /** \brief sets the label of the current fit result
     *  \param r the record to adress
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
     *  \param label label to be stored
     *  \param label_richtext richtext-ed label to be stored
     */
    virtual void setFitResultLabel(const QString& parameterID, const QString& label, const QString& label_richtext=QString(""));

    /** \brief sets the group of the current evaluation result ID \a resultID
     *  \param r the record to adress
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param group group to be stored
     */
    virtual void setFitResultEvaluationGroup(const QString& group);

    /** \brief sets the description of the current evaluation result ID \a resultID
     *  \param r the record to adress
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param description description to be stored
     */
    virtual void setFitResultEvaluationDescription(const QString& description);






    /*! \brief set a fit parameter of the current fit function (see m_fitFunction) to the specified value

        The value may be stored at different positions:
          - If no item is highlighted the parameter is not stored at all
          - If a current raw data record is highlighted and no fit has been carried out so far for this record, the value
            is stored in the preset value store of this QFEvaluationItem
          - If a current raw data record is highlighted and a fit has already been carried out for this record, the value
            is stored as a result in the highlighted QFRawDataRecord
        .
        This function also checks whether the value is to be stored (i.e. it's userEditable property is set)

        \param r the record to adress
        \param resultID the result ID which to access in the raw data records result store
        \param parameterID set the value of the parameter with this id (see QFFitFunction)
        \param value value to be stored

     */
    virtual void setFitValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value);

    /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
     *  \param r the record to adress
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
     *  \param value value to be stored
     */
    virtual void setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value);

    /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
     *  \param r the record to adress
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
     *  \param value value to be stored
     *  \param unit unit associated with \a value
     */
    virtual void setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, QString unit);

    /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
     *  \param r the record to adress
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
     *  \param value value to be stored
     */
    virtual void setFitResultValueString(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, QString value);

    /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
     *  \param r the record to adress
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
     *  \param value value to be stored
     */
    virtual void setFitResultValueInt(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, int64_t value);

    /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
     *  \param r the record to adress
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
     *  \param value value to be stored
     *  \param unit unit associated with \a value
     */
    virtual void setFitResultValueInt(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, int64_t value, QString unit);

    /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
     *  \param r the record to adress
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
     *  \param value value to be stored
     */
    virtual void setFitResultValueBool(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, bool value);

    /** \brief stores the given value and error as a fit result, i.e. into the currently highlighted QFRawDataRecord
     *  \param r the record to adress
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
     *  \param value value to be stored
     *  \param error error associated with \a value
     */
    virtual void setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, double error);

    /** \brief stores the given error as a fit result, i.e. into the currently highlighted QFRawDataRecord
     *  \param r the record to adress
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
     *  \param error error to be stored
     */
    virtual void setFitResultError(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double error);

    /** \brief stores the given values and errors as a fit result, i.e. into the currently highlighted QFRawDataRecord
     *  \param r the record to adress
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param values values to be stored
     *  \param errors errors to be stored
     */
    virtual void setFitResultValues(QFRawDataRecord* r, const QString& resultID, double* values, double* errors);

    /** \brief stores the given values and errors as a fit result if they are visible, i.e. into the currently highlighted QFRawDataRecord
     *  \param r the record to adress
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param values values to be stored
     *  \param errors errors to be stored
     */
    virtual void setFitResultValuesVisible(QFRawDataRecord* r, const QString& resultID, double* values, double* errors);

    /** \brief stores the given values and errors as a fit result if they are visible, i.e. into the currently highlighted QFRawDataRecord.
     *          Also imports the fit parameter description from the QFFitFunction and sets the parameter group to the given value
     *  \param r the record to adress
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param values values to be stored
     *  \param errors errors to be stored
     *  \param group the fit parameter group to assign
     *  \param fix indicates whether a parameter was fixed
     */
    virtual void setFitResultValuesVisibleWithGroupAndLabel(QFRawDataRecord* r, const QString& resultID, double* values, double* errors, const QString& group, bool* fix=NULL);



    /*! \brief return the value of a given parameter
        \param r the record to adress
        \param resultID the result ID which to access in the raw data records result store
        \param parameterID the parameter id
        \return the value of the given parameter \a id

        This function looks for the value at several positions:
          -# if a fit has already been performed for the current record, the value is read from raw data record
          -# if no fit has been performed, the value is taken from this evaluation (i.e. the preset value)
          -# if also locally no value is available, the value is looked up in the application INI
          -# if this also fails, the value is taken from the initial value stored in the fitFunction
        .
    */
    virtual double getFitValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID);
    /*! \brief return the fit error of a given parameter
        \param r the record to adress
        \param resultID the result ID which to access in the raw data records result store
        \param parameterID the parameter id
        \return the error associated with the given parameter.
    */
    virtual double getFitError(QFRawDataRecord* r, const QString& resultID, const QString& parameterID);

    /*! \brief set the error of a given parameter

        \param r the record to adress
        \param resultID the result ID which to access in the raw data records result store
        \param parameterID set the value of the parameter with this id (see QFFitFunction)
        \param error error to be set
    */
    virtual void setFitError(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double error);

    /*! \brief set the fix property of a fit parameter of the current fit function (see m_fitFunction)

        For a description of when data is stored, see setFitValue()

        \param r the record to adress
        \param resultID the result ID which to access in the raw data records result store
        \param parameterID set the value of the parameter with this id (see QFFitFunction)
        \param fix fix to be stored

     */
    virtual void setFitFix(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, bool fix);

    /** \brief stores the given fix property as a fit result, i.e. into the currently highlighted QFRawDataRecord
     *  \param r the record to adress
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
     *  \param fix fix to be stored
     */
    virtual void setFitResultFix(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, bool fix);

    /*! \brief return the fix property of a given parameter

        \param r the record to adress
        \param resultID the result ID which to access in the raw data records result store
        \param parameterID the parameter id
        For a detailed description of where the value is searched, see getFitValue()
    */
    virtual bool getFitFix(QFRawDataRecord* r, const QString& resultID, const QString& parameterID);


    /*! \brief set a fit parameter of the current fit function (see m_fitFunction) to the specified value

        The value may be stored at different positions:
          - If no item is highlighted the parameter is not stored at all
          - If a current raw data record is highlighted and no fit has been carried out so far for this record, the value
            is stored in the preset value store of this QFEvaluationItem
          - If a current raw data record is highlighted and a fit has already been carried out for this record, the value
            is stored as a result in the highlighted QFRawDataRecord
        .
        This function also checks whether the value is to be stored (i.e. it's userEditable property is set)

        \param id set the value of the parameter with this id (see QFFitFunction)
        \param value value to be stored

     */
    virtual void setFitValue(const QString& id, double value);

    /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
     *  \param id set the value of the parameter with this id (see QFFitFunction)
     *  \param value value to be stored
     */
    virtual void setFitResultValue(const QString& id, double value);

    /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
     *  \param id set the value of the parameter with this id (see QFFitFunction)
     *  \param value value to be stored
     *  \param unit unit associated with \a value
     */
    virtual void setFitResultValue(const QString& id, double value, QString unit);

    /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
     *  \param id set the value of the parameter with this id (see QFFitFunction)
     *  \param value value to be stored
     */
    virtual void setFitResultValueString(const QString& id, QString value);

    /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
     *  \param id set the value of the parameter with this id (see QFFitFunction)
     *  \param value value to be stored
     */
    virtual void setFitResultValueInt(const QString& id, int64_t value);

    /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
     *  \param id set the value of the parameter with this id (see QFFitFunction)
     *  \param value value to be stored
     *  \param unit unit associated with \a value
     */
    virtual void setFitResultValueInt(const QString& id, int64_t value, QString unit);

    /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
     *  \param id set the value of the parameter with this id (see QFFitFunction)
     *  \param value value to be stored
     */
    virtual void setFitResultValueBool(const QString& id, bool value);

    /** \brief stores the given value and error as a fit result, i.e. into the currently highlighted QFRawDataRecord
     *  \param id set the value of the parameter with this id (see QFFitFunction)
     *  \param value value to be stored
     *  \param error error associated with \a value
     */
    virtual void setFitResultValue(const QString& id, double value, double error);

    /** \brief stores the given error as a fit result, i.e. into the currently highlighted QFRawDataRecord
     *  \param id set the value of the parameter with this id (see QFFitFunction)
     *  \param error error to be stored
     */
    virtual void setFitResultError(const QString& id, double error);

    /** \brief stores the given values and errors as a fit result, i.e. into the currently highlighted QFRawDataRecord
     *  \param values values to be stored
     *  \param errors errors to be stored
     */
    virtual void setFitResultValues(double* values, double* errors);

    /** \brief stores the given values and errors as a fit result if they are visible, i.e. into the currently highlighted QFRawDataRecord
     *  \param values values to be stored
     *  \param errors errors to be stored
     */
    virtual void setFitResultValuesVisible(double* values, double* errors);

    /*! \brief return the value of a given parameter
        \param id the parameter id
        \return the value of the given parameter \a id

        This function looks for the value at several positions:
          -# if a fit has already been performed for the current record, the value is read from raw data record
          -# if no fit has been performed, the value is taken from this evaluation (i.e. the preset value)
          -# if also locally no value is available, the value is looked up in the application INI
          -# if this also fails, the value is taken from the initial value stored in the fitFunction
        .
    */
    virtual double getFitValue(const QString& id);

    /*! \brief return the fit error of a given parameter

        \param id the parameter id
        \return the error associated with the given parameter.
    */
    virtual double getFitError(const QString& id);

    /*! \brief set the error of a given parameter

        \param id set the value of the parameter with this id (see QFFitFunction)
        \param error error to be set
    */
    virtual void setFitError(const QString& id, double error);

    /*! \brief set the fix property of a fit parameter of the current fit function (see m_fitFunction)

        For a description of when data is stored, see setFitValue()

        \param id set the value of the parameter with this id (see QFFitFunction)
        \param fix fix to be stored

     */
    virtual void setFitFix(const QString& id, bool fix);

    /** \brief stores the given fix property as a fit result, i.e. into the currently highlighted QFRawDataRecord
     *  \param id set the value of the parameter with this id (see QFFitFunction)
     *  \param fix fix to be stored
     */
    virtual void setFitResultFix(const QString& id, bool fix);

    /*! \brief return the fix property of a given parameter

        \param id the parameter id
        For a detailed description of where the value is searched, see getFitValue()
    */
    virtual bool getFitFix(const QString& id);


    /*! \brief set the value range of a fit parameter of the current fit function (see m_fitFunction)

        This range is stored locally in the evaluation, never in the raw data record

        \param id set the value of the parameter with this id (see QFFitFunction)
        \param min lower bound for the value
        \param max upper bound for the value

     */
    virtual void setFitRange(const QString& id, double min, double max);

    /*! \brief set the value range of a fit parameter of the current fit function (see m_fitFunction)

        This range is stored locally in the evaluation, never in the raw data record

        \param id set the value of the parameter with this id (see QFFitFunction)
        \param min lower bound for the value

     */
    virtual void setFitMin(const QString& id, double min);

    /*! \brief set the value range of a fit parameter of the current fit function (see m_fitFunction)

        This range is stored locally in the evaluation, never in the raw data record

        \param id set the value of the parameter with this id (see QFFitFunction)
        \param max upper bound for the value

     */
    virtual void setFitMax(const QString& id, double max);
    /*! \brief return the lower value bound of a given parameter

        \param id set the range min of the parameter with this id (see QFFitFunction)
        For a detailed description of where the value is searched, see getFitValue()
    */
    virtual double getFitMin(const QString& id);


    /*! \brief return the upper value bound of a given parameter

        \param id set the range max of the parameter with this id (see QFFitFunction)
        For a detailed description of where the value is searched, see getFitValue()
    */
    virtual double getFitMax(const QString& id);



    /** \brief determine whether a fit has been carried out for the given record and resultID
     *
     *  \param r the given record, if \c NULL the currently highlighted record is checked
     *  \param resultID the result ID which to access in the raw data records result store
     *  \return \c true if fit results exist for the current evaluation and fit function in the highlighted QFRawDataRecord result store.
     *          If no record is highlighted or any other circumstances prevent the determination of a proper result, the result will be \c false!
     */
    virtual bool hasFit(QFRawDataRecord* r, const QString& resultID);

    /** \brief determine whether a fit has been carried out for the given record and the currently selected resultID
     *
     *  \param resultID the result ID which to access in the raw data records result store
     *  \return \c true if fit results exist for the current evaluation and fit function in the highlighted QFRawDataRecord result store.
     *          If no record is highlighted or any other circumstances prevent the determination of a proper result, the result will be \c false!
     */
    virtual bool hasFit(QFRawDataRecord* r);

    /** \brief determine whether a fit has been carried out for the currently highlighted record and the currently (implicitly) selected resultID
     *
     *  \return \c true if fit results exist for the current evaluation and fit function in the highlighted QFRawDataRecord result store.
     *          If no record is highlighted or any other circumstances prevent the determination of a proper result, the result will be \c false!
     */
    virtual  bool hasFit();




    /** \brief fill the given array of doubles with the current parameter values, as appropriate to use together with QFFitFunction
     *  \param param parameter vector to fill
     */
    virtual void fillParameters(double* param);
    /** \brief fill the given array of doubles with the current parameter errors, as appropriate to use together with QFFitFunction
     *  \param param parameter error vector to fill
    */
    virtual void fillParameterErrors(double* param);
    /** \brief fill the given array of doubles with the current parameter lower bounds, as appropriate to use together with QFFitFunction
     *  \param param parameter minimums vector to fill
    */
    virtual void fillParametersMin(double* param);
    /** \brief fill the given array of doubles with the current parameter upper bounds, as appropriate to use together with QFFitFunction
     *  \param param parameter maximums vector to fill
    */
    virtual void fillParametersMax(double* param);
    /** \brief fill the given array of bools with the current parameter fix values, as appropriate to use together with QFFitFunction
     *  \param param parameter fix vector to fill
    */
    virtual void fillFix(bool* param);

    /*! \brief fill a newly allocated array of doubles with the current parameter values, as appropriate to use together with QFFitFunction
        \return parameter vector
        \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
    */
    virtual double* allocFillParameters();
    /*! \brief fill a newly allocated array of doubles with the current parameter errors, as appropriate to use together with QFFitFunction
        \return parameter error vector
        \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
    */
    virtual double* allocFillParameterErrors();
    /*! \brief fill a newly allocated array of doubles with the current parameter lower bounds, as appropriate to use together with QFFitFunction
        \return parameter minimums vector
        \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
    */
    virtual double* allocFillParametersMin();
    /*! \brief fill a newly allocated array of doubles with the current parameter upper bounds, as appropriate to use together with QFFitFunction
        \return parameter maximums vector
        \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
    */
    virtual double* allocFillParametersMax();
    /*! \brief fill a newly allocated array of bools with the current parameter fix values, as appropriate to use together with QFFitFunction
        \return parameter fix vector
        \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
    */
    virtual bool* allocFillFix();





    /** \brief fill the given array of doubles with the current parameter values, as appropriate to use together with QFFitFunction
     *  \param r record this appplies to
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param param parameter vector to fill
     */
    virtual void fillParameters(QFRawDataRecord* r,  const QString& resultID, double* param);
    /** \brief fill the given array of doubles with the current parameter errors, as appropriate to use together with QFFitFunction
     *  \param r record this appplies to
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param param parameter error vector to fill
    */
    virtual void fillParameterErrors(QFRawDataRecord* r,  const QString& resultID, double* param);
    /** \brief fill the given array of bools with the current parameter fix values, as appropriate to use together with QFFitFunction
     *  \param r record this appplies to
     *  \param resultID the result ID which to access in the raw data records result store
     *  \param param parameter fix vector to fill
    */
    virtual void fillFix(QFRawDataRecord* r,  const QString& resultID, bool* param);

    /*! \brief fill a newly allocated array of doubles with the current parameter values, as appropriate to use together with QFFitFunction
        \param r record this appplies to
        \param resultID the result ID which to access in the raw data records result store
        \return parameter vector
        \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
    */
    virtual double* allocFillParameters(QFRawDataRecord* r,  const QString& resultID);
    /*! \brief fill a newly allocated array of doubles with the current parameter errors, as appropriate to use together with QFFitFunction
        \param r record this appplies to
        \param resultID the result ID which to access in the raw data records result store
        \return parameter error vector
        \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
    */
    virtual double* allocFillParameterErrors(QFRawDataRecord* r,  const QString& resultID);
    /*! \brief fill a newly allocated array of bools with the current parameter fix values, as appropriate to use together with QFFitFunction
        \param r record this appplies to
        \param resultID the result ID which to access in the raw data records result store
        \return parameter fix vector
        \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
    */
    virtual bool* allocFillFix(QFRawDataRecord* r,  const QString& resultID);


    /*! \brief reset the given parameter \a id to the initial/global/default value */
    virtual void resetDefaultFitValue(const QString& id);
    /*! \brief reset the given parameter \a id to the initial/global/default fix */
    virtual void resetDefaultFitFix(const QString& id);
    /*! \brief return the default/initial/global value of a given parameter        */
    virtual double getDefaultFitValue(const QString& id);
    /*! \brief return the default/initial/global fix of a given parameter        */
    virtual bool getDefaultFitFix(const QString& id);
    /*! \brief reset the all fit results to the initial/global/default value in the currently displayed curve/data */
    virtual void resetAllFitResultsCurrent();
    /*! \brief reset all parameters to the initial/global/default value in current file and resultID */
    virtual void resetAllFitValueCurrent();
    /*! \brief reset all parameters to the initial/global/default fix in current file and resultID */
    virtual void resetAllFitFixCurrent();
    /*! \brief set a fit parameter of the current fit function (see m_fitFunction) to the specified value in the initial parameters
        \param id set the value of the parameter with this id (see QFFitFunction)
        \param value value to be stored
        \param error error assigned to \a value

     */
    virtual void setInitFitValue(const QString& id, double value, double error=0.0);
    /*! \brief set the fix property of a fit parameter in the initial parameters
        \param id set the value of the parameter with this id (see QFFitFunction)
        \param fix value to be stored
     */
    virtual void setInitFitFix(const QString& id, bool fix);




    /*! \brief save all parameters of a given QFFitAlgorithm to the internal fit algorithm parameter store algorithm_parameterstore
    */
    virtual void storeQFFitAlgorithmParameters(QFFitAlgorithm* algorithm);
    /*! \brief load all parameters of a given QFFitAlgorithm from the internal fit algorithm parameter store algorithm_parameterstore
    */
    virtual void restoreQFFitAlgorithmParameters(QFFitAlgorithm* algorithm);



    /*! \brief return an resultID for storage of fit results in a QFRawDataRecord results store. The ID is valid for the currently highlo9ghted record and all current settings */
    virtual QString getEvaluationResultID()=0;
signals:

public slots:

protected:
    /*! \brief struct used to locally store fit parameter properties

        For each parameter that is stored in parameterStore this saves also whether it is really set, so each
        parameter is represented by two datamemeber: \c value and \c valueSet which is \c true only if the user
        has explicitly set the \c value. This information is used in the \c getFitXYZ() functions and set in the
        \c setFitXYZ() functions.
    */
    struct FitParameter {
        double value;
        double error;
        bool fix;
        double min;
        double max;
        bool valueSet;
        bool fixSet;
        bool minSet;
        bool maxSet;
        bool errorSet;
        FitParameter() {
            value=0;
            error=0;
            fix=false;
            min=0;
            max=0;
            valueSet=false;
            fixSet=false;
            minSet=false;
            maxSet=false;
            errorSet=false;
        }
    };

    /*! \brief datastore for fit parameter values

        Internally a uniquie key for every fit parameter is used to adress the contents. The key is calculated
        from the \code fit_fuction_id+"___"+parameter_id \endcode
     */
    QHash<QString, FitParameter> parameterStore;

    /*! \brief parameter store for fit algorithm parameters

    This map effectively has two strings as key. the first is the fit algorithm ID and the second the parameter name
    The value stored for each key is a QVariant.
    */
    QHash<QString, QHash<QString, QVariant> > algorithm_parameterstore;


    /** \brief the current fitting function */
    QString m_fitFunction;
    /** \brief the current fitting algorithm */
    QString m_fitAlgorithm;
    /** \brief map with all available fit functions */
    QMap<QString, QFFitFunction*> m_fitFunctions;
    /** \brief map with all available fit algorithms */
    QMap<QString, QFFitAlgorithm*> m_fitAlgorithms;

    /** \brief settings object to access global fit parameters */
    QSettings* fitParamGlobalSettings;
    /** \brief settings object to access fit parameters */
    QSettings* fitParamSettings;



    /** \brief return a valid ID to access parameterStore for the given parameter (id) in the current fit function (m_fitFunction) */
    inline QString getParameterStoreID(QString parameter);

    /** \brief return a valid ID to access parameterStore for the given parameter (id) in the current fit function (m_fitFunction) */
    inline QString getParameterStoreID(QString fitfunction, QString parameter);

    /** \brief write object contents into XML file
    *
    *  this function saves the id of the current fit function and algorithm, as well as the contents of parameterStore to
    *  the given XML file.
    */
    virtual void intWriteData(QXmlStreamWriter& w);
    /** \brief read back the data stored by intWriteXML() */
    virtual void intReadData(QDomElement* e);

    /*! \brief allows to read additional information from a fit parameter node in the project file

     \param parameterID id of the current fit parameter
     \param fitParam for your information the so far save information
     \param e QDomElement to read from

    */
    virtual void intReadDataFitParam(const QString& parameterID, const FitParameter& fitParam, QDomElement& e) {};
    /*! \brief allows to write additional information to a fit parameter node in the project file

     \param parameterID id of the current fit parameter
     \param w XML writer, currently in the parameter node (i.e. write subnode with w.writeStartElement() and attributes with w.writeAttribute() )
    */
    virtual void intWriteDataFitParam(const QString& parameterID, QXmlStreamWriter& w) const {};
    /*! \brief allows to write additional information to a fit algorithm node in the project file

     \param w XML writer, currently in the parameter node (i.e. write subnode with w.writeStartElement() and attributes with w.writeAttribute() )
    */
    virtual void intWriteDataAlgorithm(QXmlStreamWriter& w) const {};

    /*! \brief allows to read additional information from a fit algorithm node in the project file

     \param e QDomElement to read from

    */
    virtual void intReadDataAlgorithm(QDomElement& e) {};



    /** \brief returns an ID for a given fit parameter (i.e. prepends \c fitparam_ and if \a fix is \c true, also appends \c _fix ) */
    inline QString getFitParamID(QString fitparam) {
        return QString("fitparam_%1").arg(fitparam);
    }

    /** \brief returns an ID for a given fit parameter fix (i.e. prepends \c fitparam_ and appends \c _fix ) */
    inline QString getFitParamFixID(QString fitparam) {
        return QString("fitparam_%1_fix").arg(fitparam);
    }
    /*! \brief allows to draw certain fit parameters from other sources (e.g. copy a property of the underlying record

        \param r record this appplies to
        \param resultID the result ID which to access in the raw data records result store
        \param paramid the parameter we are looking up
        \param[out] value in this parameter the value is returned, if the function returns \c true
        \param[out] error in this parameter the value' error is returned, if the function returns \c true
        \return \c true if a special value is found. In that case it returns the value in \a value and \a error

     */
    virtual bool hasSpecial(QFRawDataRecord* r, const QString& resultID, const QString& paramid, double& value, double& error);
};

#endif // QFFITRESULTSEVALUATION_H
