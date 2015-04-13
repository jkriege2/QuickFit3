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

#ifndef QFUSESRESULTSEVALUATION_H
#define QFUSESRESULTSEVALUATION_H

#include <QHash>
#include <QMap>
#include <QSettings>

#include "qfevaluationitem.h"
#include "qfevaluationitemfactory.h"
#include "qffitfunction.h"
#include "qffitalgorithm.h"
#include "qffitparameterbasicinterface.h"
#include "qffitalgorithmthreaddedfit.h"
#include "qfsimplefitparameterinterface.h"

/*! \brief evaluation item base class which implements some tool functions to more easily store fit results
    \ingroup qf3evaluationplugins

    Features of this class are:
      - the initial values for fit parameters are read from a special function (getFitParamDefaultValue()). As a second option this
      class opens the file \code <configDir>/fitparams_<type>.ini\endcode and reads the parameters from it in the form \code
[fitFunctionID]
param=<initial value>
param_min=<initial_range_min>
param_max=<initial_range_max>
param_fix=<initial_fix>
      \endcode
    .

*/
class QFUsesResultsEvaluation : public QFEvaluationItem, public QFSimpleFitParameterInterface
{
        Q_OBJECT
        Q_INTERFACES(QFSimpleFitParameterInterface)
    public:
        explicit QFUsesResultsEvaluation(QFProject *parent = 0, bool showRDRList=true, bool useSelection=false);
        virtual ~QFUsesResultsEvaluation();

        /** \copydoc QFEvaluationItem::setPresetProperty() */
        virtual void setPresetProperty(const QString& id, const QVariant& data, bool usereditable=true, bool visible=true);



        /** \brief return \c true if an evaluation has been performed for the given record \a r1 */
        virtual bool hasResults(const QFRawDataRecord* r1) const;
        /** \brief return \c true if an evaluation has been performed for the current record */
        virtual bool hasResults() const;

        /** \brief return \c true if an evaluation has been performed for the given record \a r1 */
        virtual bool hasResults(const QFRawDataRecord* r1, QString resultID) const;


        /** \brief create an ID to reference results that belong to this evaluation \b object (includes the evaluation id) and the
         *         current fit function for a given fitFunction ID */
        virtual QString getEvaluationResultID() const;






        virtual void setFitResultSortPriority(QFRawDataRecord *r, const QString& resultID, const QString &parameterID, bool sortPriority);

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


        /** \brief reset all fit results for the given \a resultID in the given record \a r */
        void resetAllFitResults(QFRawDataRecord* r, const QString& resultID);




        /*! \brief set a fit parameter for the current record and resultID

            The value may be stored at different positions:
              - If no item is highlighted the parameter is not stored at all
              - If a current raw data record is highlighted and no fit has been carried out so far for this record, the value
                is stored in the preset value store of this QFEvaluationItem
              - If a current raw data record is highlighted and a fit has already been carried out for this record, the value
                is stored as a result in the highlighted QFRawDataRecord
            .
            This function also checks whether the value is to be stored (i.e. it's userEditable property is set)

            \param parameterID set the value of the parameter with this id (see QFFitFunction)
            \param value value to be stored
            \param unit unit to be stored

         */
        virtual void setFitValue(const QString& parameterID, double value);
        /*! \brief set a fit parameter for the current record and resultID

            The value may be stored at different positions:
              - If no item is highlighted the parameter is not stored at all
              - If a current raw data record is highlighted and no fit has been carried out so far for this record, the value
                is stored in the preset value store of this QFEvaluationItem
              - If a current raw data record is highlighted and a fit has already been carried out for this record, the value
                is stored as a result in the highlighted QFRawDataRecord
            .
            This function also checks whether the value is to be stored (i.e. it's userEditable property is set)

            \param parameterID set the value of the parameter with this id (see QFFitFunction)
            \param value value to be stored
            \param unit unit to be stored

         */
        virtual void setFitValue(const QString& parameterID, double value, const QString& unit);
        /*! \brief return the value of a given parameter
            \param parameterID the parameter id
            \return the value of the given parameter \a id

            This function looks for the value at several positions:
              -# if a fit has already been performed for the current record, the value is read from raw data record
              -# if no fit has been performed, the value is taken from this evaluation (i.e. the preset value)
              -# if also locally no value is available, the value is looked up in the application INI
              -# if this also fails, the value is taken from the initial value stored in the fitFunction
            .
        */
        virtual double getFitValue(const QString& parameterID) const;
        /*! \brief return the fit error of a given parameter
            \param parameterID the parameter id
            \return the error associated with the given parameter.
        */
        virtual double getFitError(const QString& parameterID) const;

        /*! \brief set the error of a given parameter
            \param parameterID set the value of the parameter with this id (see QFFitFunction)
            \param error error to be set
        */
        virtual void setFitError(const QString& parameterID, double error);

        /*! \brief set the fix property of a fit parameter of the current fit function (see m_fitFunction)

            For a description of when data is stored, see setFitValue()

            \param parameterID set the value of the parameter with this id (see QFFitFunction)
            \param fix fix to be stored

         */
        virtual void setFitFix(const QString& parameterID, bool fix);

        /*! \brief return the fix property of a given parameter

            \param parameterID the parameter id
            For a detailed description of where the value is searched, see getFitValue()
        */
        virtual bool getFitFix(const QString& parameterID) const;



















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
        virtual void setFitValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, const QString& unit);

        /*! \brief set the initial fit parameter value

            \param parameterID set the value of the parameter with this id (see QFFitFunction)
            \param value value to be stored
         */
        virtual void setInitFitValue(const QString& parameterID, double value);

        /*! \brief set the initial fit parameter value

            \param parameterID set the value of the parameter with this id (see QFFitFunction)
            \param value value to be stored
            \param error error to be stored
         */
        virtual void setInitFitValue(const QString& parameterID, double value, double error);

        /*! \brief set the initial fit parameter value

            \param parameterID set the value of the parameter with this id (see QFFitFunction)
            \param value value to be stored
            \param error error to be stored
            \param fix fix to be stored
         */
        virtual void setInitFitValue(const QString& parameterID, double value, double error, bool fix);

        /*! \brief set the initial fit parameter value

            \param parameterID set the value of the parameter with this id (see QFFitFunction)
            \param value value to be stored
            \param fix fix to be stored
         */
        virtual void setInitFitValue(const QString& parameterID, double value, bool fix);

        /*! \brief set the initial fit parameter error

            \param parameterID set the value of the parameter with this id (see QFFitFunction)
            \param error error to be stored
         */
        virtual void setInitFitValueError(const QString& parameterID, double error);

        /*! \brief set the initial fit parameter fix

            \param parameterID set the value of the parameter with this id (see QFFitFunction)
            \param fix fix to be stored
         */
        virtual void setInitFitFix(const QString& parameterID, bool fix);

        /*! \brief set the initial fit parameter min value

            \param parameterID set the value of the parameter with this id (see QFFitFunction)
            \param min minimum value to be stored
         */
        virtual void setInitFitValueMin(const QString& parameterID, double min);

        /*! \brief set the initial fit parameter max value

            \param parameterID set the value of the parameter with this id (see QFFitFunction)
            \param max maximum value to be stored
         */
        virtual void setInitFitValueMax(const QString& parameterID, double max);

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
         *  \param unit unit associated with \a value
         */
        virtual void setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, const QString& unit);

        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param resultID the result ID which to access in the raw data records result store
         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         */
        virtual void setFitResultValueString(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, const QString& value);

        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param resultID the result ID which to access in the raw data records result store
         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         *  \param unit unit associated with \a value
         */
        virtual void setFitResultValueInt(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, int64_t value, const QString& unit=QString());

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
        virtual void setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, double error, const QString& unit);

        /** \brief stores the given error as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param resultID the result ID which to access in the raw data records result store
         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
         *  \param error error to be stored
         */
        virtual void setFitResultError(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double error);

        /** \brief stores the given value and error as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param resultID the result ID which to access in the raw data records result store
         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
         *  \param value value array to be stored
         *  \param error error array associated with \a value
         *  \param N number of items in \a value and \a error
         *  \param unit the unit name
         */
        virtual void setFitResultValueNumberArray(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double* value, double* error, uint32_t N, const QString& unit=QString());

        /** \brief stores the given value and error as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param resultID the result ID which to access in the raw data records result store
         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
         *  \param value value array to be stored
         *  \param N number of items in \a value
         *  \param unit the unit name
         */
        virtual void setFitResultValueNumberArray(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double* value, uint32_t N, const QString& unit=QString());

        /*! \brief return the value array of a given parameter
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
        virtual QVector<double> getFitValueNumberArray(const QFRawDataRecord* r, const QString& resultID, const QString& parameterID) const;
        /*! \brief return the fit error array of a given parameter
            \param r the record to adress
            \param resultID the result ID which to access in the raw data records result store
            \param parameterID the parameter id
            \return the error associated with the given parameter.
        */
        virtual QVector<double> getFitValueErrorArray(const QFRawDataRecord* r, const QString& resultID, const QString& parameterID) const;



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
        virtual double getFitValue(const QFRawDataRecord* r, const QString& resultID, const QString& parameterID) const;
        /*! \brief returns true, if the given fit results exists as a result (i.e. not in the default store)
            \param r the record to adress
            \param resultID the result ID which to access in the raw data records result store
            \param parameterID the parameter id
        */
        virtual bool fitValueExists(QFRawDataRecord* r, const QString& resultID, const QString& parameterID) const;
        /*! \brief return the fit error of a given parameter
            \param r the record to adress
            \param resultID the result ID which to access in the raw data records result store
            \param parameterID the parameter id
            \return the error associated with the given parameter.
        */
        virtual double getFitError(const QFRawDataRecord* r, const QString& resultID, const QString& parameterID) const;

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
        virtual bool getFitFix(const QFRawDataRecord* r, const QString& resultID, const QString& parameterID) const;

        /** \brief return the name for the given parameter in the given model in either HTML richtext or plain text */
        virtual QString getParameterName(int model, int id, bool html=false) const;
        /** \brief return the unit for the given parameter in the given model in either HTML richtext or plain text */
        virtual QString getParameterUnit(int model, int id, bool html) const;
        /** \brief return the number of parameters in the given model */
        virtual int getParameterCount(int model) const;
        /** \brief return the ID for the given parameter in the given model  */
    virtual QString getParameterID(int model, int param) const;

    virtual void setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double value, double error);

        virtual void setFitResultFitStatistics(QFRawDataRecord *record, const QString &index, const QFFitStatistics &result, const QString &prefix, const QString &group);

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

        /** \brief settings object to access global fit parameters */
        QSettings* fitParamGlobalSettings;
        /** \brief settings object to access fit parameters */
        QSettings* fitParamSettings;


        /** \brief thread used for fitting */
        QFFitAlgorithmThreadedFit* doFitThread;



        /** \brief return a valid ID to access parameterStore for the given parameter (id) */
        virtual QString getParameterStoreID(QString parameter) const;

        /** \brief return a valid ID to access parameterStore for the given parameter (id) */
        virtual QString getParameterStoreIDInQSettings(QString parameter) const;

        virtual void intReadData(QDomElement *e);
        virtual void intWriteData(QXmlStreamWriter &w) const;


        /** \brief may be used to transform a resultID before it is used in any of the \c QFRawDataRecord.results... functions */
        virtual QString transformResultID(const QString& resultID) const;

        /** \brief returns an ID for a given fit parameter (i.e. prepends \c fitparam_ and if \a fix is \c true, also appends \c _fix ) */
        virtual QString getFitParamID(QString fitparam) const;

        /** \brief returns an ID for a given fit parameter fix (i.e. prepends \c fitparam_ and appends \c _fix ) */
        virtual QString getFitParamFixID(QString fitparam) const;

        /*! \brief allows to draw certain fit parameters from other sources (e.g. copy a property of the underlying record

            \param r record this appplies to
            \param resultID the result ID which to access in the raw data records result store
            \param paramid the parameter we are looking up
            \param[out] value in this parameter the value is returned, if the function returns \c true
            \param[out] error in this parameter the value' error is returned, if the function returns \c true
            \return \c true if a special value is found. In that case it returns the value in \a value and \a error

         */
        virtual bool hasSpecial(const QFRawDataRecord* r, const QString& resultID, const QString& paramid, double& value, double& error) const;
        /*! \brief struct used to describe fit parameter default values */
        struct FitParameterDefault {
            double value;
            double error;
            bool fix;
            double min;
            double max;
            QVector<double> valueVector;
            QVector<double> errorVector;
            FitParameterDefault();
        };

        /** \brief returns default values for a parameter */
        virtual bool getParameterDefault(const QFRawDataRecord* r, const QString& resultID, const QString& parameterID, FitParameterDefault& defaultValue) const;


};

#endif // QFUSESRESULTSEVALUATION_H
