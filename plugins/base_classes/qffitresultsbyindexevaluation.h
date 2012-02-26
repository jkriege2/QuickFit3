#ifndef QFFITRESULTSBYINDEXEVALUATION_H
#define QFFITRESULTSBYINDEXEVALUATION_H

#include "qffitresultsevaluation.h"

#include <QHash>
#include <QMap>
#include <QSettings>

#include "qfevaluationitem.h"
#include "qfevaluationitemfactory.h"
#include "qffitfunction.h"
#include "qffitalgorithm.h"
#include "qffitparameterbasicinterface.h"

/*! \brief evaluation item base class that extends QFFitResultsEvaluation with an index-based interface
    \ingroup qf3evaluationplugins

*/
class QFFitResultsByIndexEvaluation : public QFFitResultsEvaluation {
        Q_OBJECT
    public:
        /*! \brief class constructor

          \param fitFunctionPrefix only fit functions with this prefix will be used by this object, leave empty to use all, you may supply a list, separated by commas
          \param parent project this object belongs to
        */
        QFFitResultsByIndexEvaluation(const QString& fitFunctionPrefix, QFProject* parent);

        /** \brief class destructor */
        virtual ~QFFitResultsByIndexEvaluation();




    public:
        /** \brief create an ID to reference results that belong to this evaluation \b object (includes the evaluation id) and the
         *         current fit function for a given fitFunction ID */
        virtual QString getEvaluationResultID(QString fitFunction, int currentIndex);

        /** \brief extract the index number (or -1 for average) from the supplied resultID, which has to have been created with getEvaluationResultID() from this class */
        virtual int getIndexFromEvaluationResultID(const QString& resultID);

        /** \brief return the smallest available index */
        virtual int getIndexMin(QFRawDataRecord* r)=0;
        /** \brief return the largest available index */
        virtual int getIndexMax(QFRawDataRecord* r)=0;

        /** \brief set the current index to use */
        int getCurrentIndex();

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
            \param index the index in which to adress
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param value value to be stored

         */
        virtual void setFitValue(QFRawDataRecord* r, int index, const QString& id, double value);

        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param id set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         */
        virtual void setFitResultValue(QFRawDataRecord* r, int index, const QString& id, double value);

        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param id set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         *  \param unit unit associated with \a value
         */
        virtual void setFitResultValue(QFRawDataRecord* r, int index, const QString& id, double value, QString unit);

        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param id set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         */
        virtual void setFitResultValueString(QFRawDataRecord* r, int index, const QString& id, QString value);

        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param id set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         */
        virtual void setFitResultValueInt(QFRawDataRecord* r, int index, const QString& id, int64_t value);

        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param id set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         *  \param unit unit associated with \a value
         */
        virtual void setFitResultValueInt(QFRawDataRecord* r, int index, const QString& id, int64_t value, QString unit);

        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param id set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         */
        virtual void setFitResultValueBool(QFRawDataRecord* r, int index, const QString& id, bool value);

        /** \brief stores the given value and error as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param id set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         *  \param error error associated with \a value
         */
        virtual void setFitResultValue(QFRawDataRecord* r, int index, const QString& id, double value, double error);

        /** \brief stores the given error as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param id set the value of the parameter with this id (see QFFitFunction)
         *  \param error error to be stored
         */
        virtual void setFitResultError(QFRawDataRecord* r, int index, const QString& id, double error);

        /** \brief stores the given values and errors as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param values values to be stored
         *  \param errors errors to be stored
         */
        virtual void setFitResultValues(QFRawDataRecord* r, int index, double* values, double* errors);

        /** \brief stores the given values and errors as a fit result if they are visible, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param values values to be stored
         *  \param errors errors to be stored
         */
        virtual void setFitResultValuesVisible(QFRawDataRecord* r, int index, double* values, double* errors);

        /** \brief stores the given values and errors as a fit result if they are visible, i.e. into the currently highlighted QFRawDataRecord.
         *          Also imports the fit parameter description from the QFFitFunction and sets the parameter group to the given value
         *  \param r the record to adress
         *  \param resultID the result ID which to access in the raw data records result store
         *  \param values values to be stored
         *  \param errors errors to be stored
         */
        virtual void setFitResultValuesVisibleWithGroupAndLabel(QFRawDataRecord* r, int index, double* values, double* errors, const QString& group, bool* fix=NULL, const QString& fixGroup=QString(""), bool sortPriority=true);

        /*! \brief return the value of a given parameter
            \param r the record to adress
            \param index the index in which to adress
            \param id the parameter id
            \return the value of the given parameter \a id

            This function looks for the value at several positions:
              -# if a fit has already been performed for the current record, the value is read from raw data record
              -# if no fit has been performed, the value is taken from this evaluation (i.e. the preset value)
              -# if also locally no value is available, the value is looked up in the application INI
              -# if this also fails, the value is taken from the initial value stored in the fitFunction
            .
        */
        virtual double getFitValue(QFRawDataRecord* r, int index, const QString& id);

        /*! \brief return the fit error of a given parameter
            \param r the record to adress
            \param index the index in which to adress
            \param id the parameter id
            \return the error associated with the given parameter.
        */
        virtual double getFitError(QFRawDataRecord* r, int index, const QString& id);

        /*! \brief set the error of a given parameter

            \param r the record to adress
            \param index the index in which to adress
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param error error to be set
        */
        virtual void setFitError(QFRawDataRecord* r, int index, const QString& id, double error);

        /*! \brief set the fix property of a fit parameter of the current fit function (see m_fitFunction)

            For a description of when data is stored, see setFitValue()

            \param r the record to adress
            \param index the index in which to adress
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param fix fix to be stored

         */
        virtual void setFitFix(QFRawDataRecord* r, int index, const QString& id, bool fix);

        /** \brief stores the given fix property as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param id set the value of the parameter with this id (see QFFitFunction)
         *  \param fix fix to be stored
         */
        virtual void setFitResultFix(QFRawDataRecord* r, int index, const QString& id, bool fix);

        /*! \brief return the fix property of a given parameter

            \param r the record to adress
            \param index the index in which to adress
            \param id the parameter id
            For a detailed description of where the value is searched, see getFitValue()
        */
        virtual bool getFitFix(QFRawDataRecord* r, int index, const QString& id);

        /** \brief sets the group of the given fit result
         *  \param r the record to adress
            \param index the index in which to adress
         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
         *  \param group group to be stored
         */
        virtual void setFitResultGroup(QFRawDataRecord* r, int index, const QString& parameterID, const QString& group);

        /** \brief sets the label of the given fit result
         *  \param r the record to adress
            \param index the index in which to adress
         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
         *  \param label label to be stored
         *  \param label_richtext richtext-ed label to be stored
         */
        virtual void setFitResultLabel(QFRawDataRecord* r, int index, const QString& parameterID, const QString& label, const QString& label_richtext=QString(""));

        /** \brief sets the group of the given evaluation result ID \a resultID
         *  \param r the record to adress
            \param index the index in which to adress
         *  \param group group to be stored
         */
        virtual void setFitResultEvaluationGroup(QFRawDataRecord* r, int index, const QString& group);

        /** \brief sets the description of the given evaluation result ID \a resultID
         *  \param r the record to adress
            \param index the index in which to adress
         *  \param description description to be stored
         */
        virtual void setFitResultEvaluationDescription(QFRawDataRecord* r, int index, const QString& description);

        /** \brief fill the given array of doubles with the current parameter values, as appropriate to use together with QFFitFunction
         *  \param r record this appplies to
         *  \param index index in \a r this applies to
         *  \param param parameter vector to fill
         */
        virtual void fillParameters(QFRawDataRecord* r, int index, double* param);
        /** \brief fill the given array of doubles with the current parameter errors, as appropriate to use together with QFFitFunction
         *  \param r record this appplies to
         *  \param index index in \a r this applies to
         *  \param param parameter error vector to fill
        */
        virtual void fillParameterErrors(QFRawDataRecord* r, int index, double* param);
        /** \brief fill the given array of bools with the current parameter fix values, as appropriate to use together with QFFitFunction
         *  \param r record this appplies to
         *  \param index index in \a r this applies to
         *  \param param parameter fix vector to fill
        */
        virtual void fillFix(QFRawDataRecord* r, int index, bool* param);

        /*! \brief fill a newly allocated array of doubles with the current parameter values, as appropriate to use together with QFFitFunction
            \param r record this appplies to
            \param index index in \a r this applies to
            \return parameter vector
            \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
        */
        virtual double* allocFillParameters(QFRawDataRecord* r, int index);
        /*! \brief fill a newly allocated array of doubles with the current parameter errors, as appropriate to use together with QFFitFunction
            \param r record this appplies to
            \param index index in \a r this applies to
            \return parameter error vector
            \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
        */
        virtual double* allocFillParameterErrors(QFRawDataRecord* r, int index);
        /*! \brief fill a newly allocated array of bools with the current parameter fix values, as appropriate to use together with QFFitFunction
            \param r record this appplies to
            \param index index in \a r this applies to
            \return parameter fix vector
            \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
        */
        virtual bool* allocFillFix(QFRawDataRecord* r, int index);

        /** \brief determine whether a fit has been carried out for the given record and index
         *
         *  \param r the given record, if \c NULL the currently highlighted record is checked
         *  \param index the index to check
         *  \return \c true if fit results exist for the current evaluation and fit function in the highlighted QFRawDataRecord result store.
         *          If no record is highlighted or any other circumstances prevent the determination of a proper result, the result will be \c false!
         */
        virtual bool hasFit(QFRawDataRecord* r, int index);

        /*! \brief set the given parameter \a id to the given value (and error) in all files and all indexes */
        virtual void setAllFitValues(const QString& id, double value, double error=0.0, bool currentFileOnly=false);
        /*! \brief set the given parameter \a id to the given fix value and all indexes */
        virtual void setAllFitFixes(const QString& id, bool fix, bool currentFileOnly=false);
        /*! \brief reset all parameters to the initial/global/default value in all files and all indexes */
        virtual void resetAllFitValue();
        /*! \brief reset all fit results to the initial/global/default value in all files and all indexes */
        virtual void resetAllFitResults();
        /*! \brief reset all parameters to the initial/global/default fix in all files and all indexes */
        virtual void resetAllFitFix();
        /*! \brief reset all fit results to the initial/global/default value in the current file but all indexes */
        virtual void resetAllFitResultsCurrentFileAllIndices();
        /*! \brief reset all fit results to the initial/global/default value in the current file but all indexes */
        virtual void resetAllFitResultsAllFilesAllIndices();

        /** \brief create an ID to reference results that belong to this evaluation \b object (includes the evaluation id) and the
         *         current fit function */
        virtual QString getEvaluationResultID();

        /** \brief create an ID to reference results that belong to this evaluation \b object (includes the evaluation id) and the
         *         current fit function for a given fitFunction ID */
        virtual QString getEvaluationResultID(QString fitFunction);

        /** \brief create an ID to reference results that belong to this evaluation \b object (includes the evaluation id) and the
         *         current fit function for a given fitFunction ID */
        virtual QString getEvaluationResultID(int currentIndex);


        virtual bool hasSpecial(QFRawDataRecord* r, int index, const QString& paramid, double& value, double& error);


        /*! \brief perform a fit for the given \a record and \a run

            The parameters \a defaultMinDatarange and \a defaultMaxDatarange set the range of data points taken for the fit.
            If both are -1, the full range is used

            The object \a dlgFitProgress (if supplied) is used to report the progress and to check whether the user clicked "Cancel".
          */
        virtual void doFit(QFRawDataRecord* record, int run, int defaultMinDatarange=-1, int defaultMaxDatarange=-1, QFFitAlgorithmReporter* dlgFitProgress=NULL)=0;

    public slots:
        /** \brief set the current index to use */
        virtual void setCurrentIndex(int index);



    protected:
        /** \brief current index to use */
        int m_currentIndex;



    public:

        /* explicitly make some functions visible again, as the C++ compiler hides function definitions
           from a base class that are also declared in the derived class, but with different parameter sets!

           see http://www.parashift.com/c++-faq-lite/strange-inheritance.html#faq-23.9
         */


        using QFFitResultsEvaluation::setFitValue;
        using QFFitResultsEvaluation::setFitResultValue;
        using QFFitResultsEvaluation::setFitResultValueString;
        using QFFitResultsEvaluation::setFitResultValueInt;
        using QFFitResultsEvaluation::setFitResultValueBool;
        using QFFitResultsEvaluation::setFitResultError;
        using QFFitResultsEvaluation::setFitResultValues;
        using QFFitResultsEvaluation::setFitResultValuesVisible;
        using QFFitResultsEvaluation::getFitValue;
        using QFFitResultsEvaluation::getFitError;
        using QFFitResultsEvaluation::setFitFix;
        using QFFitResultsEvaluation::setFitResultFix;
        using QFFitResultsEvaluation::getFitFix;
        using QFFitResultsEvaluation::setFitError;
        using QFFitResultsEvaluation::setFitRange;
        using QFFitResultsEvaluation::setFitMin;
        using QFFitResultsEvaluation::setFitMax;
        using QFFitResultsEvaluation::hasFit;
        using QFFitResultsEvaluation::hasSpecial;
        using QFFitResultsEvaluation::fillParameters;
        using QFFitResultsEvaluation::fillParameterErrors;
        using QFFitResultsEvaluation::fillParametersMin;
        using QFFitResultsEvaluation::fillParametersMax;
        using QFFitResultsEvaluation::fillFix;
        using QFFitResultsEvaluation::allocFillParameters;
        using QFFitResultsEvaluation::allocFillParameterErrors;
        using QFFitResultsEvaluation::allocFillParametersMin;
        using QFFitResultsEvaluation::allocFillParametersMax;
        using QFFitResultsEvaluation::allocFillFix;
        using QFFitResultsEvaluation::resetDefaultFitValue;
        using QFFitResultsEvaluation::resetDefaultFitFix;
        using QFFitResultsEvaluation::getDefaultFitValue;
        using QFFitResultsEvaluation::getDefaultFitFix;
        using QFFitResultsEvaluation::resetAllFitResultsCurrent;
        using QFFitResultsEvaluation::setFitResultGroup;
        using QFFitResultsEvaluation::setFitResultLabel;
        using QFFitResultsEvaluation::setFitResultEvaluationGroup;
        using QFFitResultsEvaluation::setFitResultEvaluationDescription;
        using QFFitResultsEvaluation::setFitResultValuesVisibleWithGroupAndLabel;
        using QFFitResultsEvaluation::getEvaluationResultID;


};

#endif // QFFITRESULTSBYINDEXEVALUATION_H
