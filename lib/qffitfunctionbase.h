/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-11-25 17:03:47 +0100 (Di, 25 Nov 2014) $  (revision $Rev: 3644 $)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFFITFUNCTIONBASE_H
#define QFFITFUNCTIONBASE_H
#include <stdint.h>
#include <QString>
#include <QList>
#include <QStringList>
#include <cfloat>
#include <cmath>
#include "qfmathtools.h"
#include "lib_imexport.h"
#include <QObject>

/*! \brief base class for fit functions
    \ingroup qf3lib_fitting

*/
class QFLIB_EXPORT QFFitFunctionBase
{
    public:
        /** \brief different types of input widgets for parameters */
        enum ParameterWidgetType {
            FloatNumber=0, /**< a floating point number edit widget is used for the parameter*/
            LogFloatNumber=3, /**< a floating point number edit widget is used for the parameter, which uses a logarithmic stepping scheme*/
            IntNumber=1,   /**< an integer number edit widget is used for the parameter*/
            IntCombo=2,    /**< a combobox widget is used for the parameter*/
            Invalid=100    /**< invalid widget */
        };

        /** \brief parameter classes, which may be used by the fit algorithms. These could e.g. introduce parameter transform to quilibrate the ranges better. */
        enum ParameterType {
            StandardParameter=0, /**< a simple, linearly scaling parameter */
            LogParameter=1, /**< a parameter, which may span several orders of magnitude and is positive */
            PosNegOrderOfMagnitudeParameter=2, /**< a parameter, which may span several orders of magnitude and is positive */
            CustomTramsformParameter=100, /**< The functions cutsomTransform() and cutsomBackTransform() have been implemented for this parameter and implement the transforms */
        };

        /** \brief error display mode */
        enum ErrorDisplayMode {
            NoError=0,      /**< do not display an error widget */
            DisplayError=1, /**< display a label for the error */
            EditError=2     /**< display a widget to edit the error */

        };
        /** \brief this struct is used to describe the fitting parameters */
        struct QFLIB_EXPORT ParameterDescription {
            /** \brief type of the parameter widget */
            ParameterWidgetType widgetType;
            /** \brief type of the parameter */
            ParameterType parameterType;
            /** \brief short unique id for the parameter, e.g. "n" */
            QString id;
            /** \brief description of the parameter, e.g. "particle number N" */
            QString name;
            /** \brief a label for the parameter, e.g. "<i>N</i>". You may use HTML markup for this */
            QString label;
            /** \brief unit of the parameter, e.g. "usec", "g"  */
            QString unit;
            /** \brief unit label of the parameter, e.g. "&mu;s", "g". You may use HTML markup for this  */
            QString unitLabel;
            /** \brief is this a fitting parameter, if \c false the user may supply a value, but the value will never be changed by the fitting
             *         algorithm. Use this e.g. to configure a model (number of components, ...) */
            bool fit;
            /** \brief determine whether, or not the user is allowed to change the value of a parameter. You could use a parameter with
             *         \c fit=userEditable=false to output values that were calculated from the fitting parameters after the fit. */
            bool userEditable;
            /** \brief determine whether, or not an error value is displayed for this parameter */
            ErrorDisplayMode displayError;
            /** \brief determine whether the user should be allowed to edit the parameter range */
            bool userRangeEditable;
            /** \brief an initial value for the parameter */
            double initialValue;
            /** \brief initial fix state of parameter */
            bool initialFix;
            /** \brief minimum value of the parameter range (if supported by algorithm) */
            double minValue;
            /** \brief maximum value of the parameter range (if supported by algorithm) */
            double maxValue;
            /** \brief value increment for the widget associated with the parameter */
            double inc;
            /** \brief absolute minimum value of the parameter range (if supported by algorithm), or \c -DBL_MAX if disabled */
            double absMinValue;
            /** \brief absolute maximum value of the parameter range (if supported by algorithm), or \c DBL_MAX if disabled */
            double absMaxValue;
            /** \brief names for the items in the combobox (if type==IntCombo) */
            QStringList comboItems;

            inline explicit ParameterDescription() {
                widgetType=Invalid;
                id="";
                name="";
                label="";
                unit="";
                fit=false;
                initialFix=false;
                userEditable=false;
                displayError=NoError;
                userRangeEditable=false;
                initialValue=0;
                minValue=0;
                maxValue=0;
                inc=1;
                absMinValue=-DBL_MAX;
                absMaxValue=DBL_MAX;
                comboItems=QStringList();
                parameterType=StandardParameter;
            }
        };
        inline virtual ~QFFitFunctionBase() {}

        /** \brief returns the number of parameters */
        inline int paramCount() const {
            return m_parameters.size();
        }
        /** \brief return a name for the model */
        virtual QString name() const=0;
        /** \brief return a short name for the model (max. around 10-20 characters) */
        virtual QString shortName() const;
        /** \brief this returns the category of the fit function, which allows to group fit functions by groups */
        virtual QString category() const;
        /** \brief return a short unique model ID string */
        virtual QString id() const=0;


        /** \brief indicates, that this fit function should not be used any longer, i.e. is DEPRECATED */
        virtual bool isDeprecated() const;

        /** \brief retrun \c if this fit function implements analytic derivatives in evaluateDerivatives() */
        virtual bool get_implementsDerivatives() const ;


        /*! \brief calculate non-fit parameters and their errors (if \a error is supplied), i.e. \c fit=userEditable=false
            \param[in,out] parameterValues the parameter values on which to base the calculation. This is also
                           used to write back the calculated values.
            \param[in,out] error if \c !=NULL this contains the errors of the parameters in parameterValues. the calculated
                           errors are also stored in this array.
            \see sortParameter()
        */
        virtual void calcParameter(double* parameterValues, double* error=NULL) const ;
        inline  void calcParameter(QVector<double>& parameterValues, QVector<double>& error) const {
            calcParameter(parameterValues.data(), error.data());
        }
        inline  void calcParameter(QVector<double>& parameterValues) const {
            calcParameter(parameterValues.data(), NULL);
        }



        /*! \brief Sometimes it is desireable to sort the fit parameters in some given order. This function may be uused to do so.
                   <b>DO NOT SORT IN calcParameter()</b>
            \param[in,out] parameterValues the parameter values which to sort
            \param[in,out] error if \c !=NULL this contains the errors to be sorted
            \see calcParameter()
        */
        virtual void sortParameter(double* parameterValues, double* error=NULL, bool* fix=NULL) const ;
        inline  void sortParameter(QVector<double>& parameterValues, QVector<double>& error, QVector<bool>& fix) const {
            sortParameter(parameterValues.data(), error.data(), fix.data());
        }
        inline  void sortParameter(QVector<double>& parameterValues, QVector<bool>& fix) const {
            sortParameter(parameterValues.data(), NULL, fix.data());
        }
        inline  void sortParameter(QVector<double>& parameterValues, QVector<double>& error) const {
            sortParameter(parameterValues.data(), error.data(), NULL);
        }
        inline  void sortParameter(QVector<double>& parameterValues) const {
            sortParameter(parameterValues.data(), NULL, NULL);
        }

        /*! \brief returns \c true if the given parameter is currently visible (which could e.g. depend on the setting of the other parameters)
            \param parameter which parameter to test
            \param parameterValues the parameter values on which to base the decission
            \return \c true if parameter is visibled (based on the parameter values in \a parameterValues )
        */
        virtual bool isParameterVisible(int parameter, const double* parameterValues) const ;
        inline  bool isParameterVisible(int parameter, const QVector<double>& parameterValues) const {
            return isParameterVisible(parameter, parameterValues.data());
        }


        /*! \brief return the number of graphs that should be plotted additional to the function grph itself
            \param params The decision may be based on this parameter set.
            \return number of additional plots, or 0 if none
        */
        virtual unsigned int getAdditionalPlotCount(const double* params) const;
        inline  unsigned int getAdditionalPlotCount(const QVector<double>& params) const {
            return getAdditionalPlotCount(params.constData());
        }

        /*! \brief transform the given parameter vector so tht it my be used to plot the \a plot -th additional graph
            \param plot the number of the plot to plot
            \param[in,out] params parameter vector. This is assumed to be filled with the full parameters when the function is called
            \return label/name for the graph
        */
        virtual QString transformParametersForAdditionalPlot(int plot, double* params) const ;
        inline  QString transformParametersForAdditionalPlot(int plot, QVector<double>& params) const {
            params.resize(m_parameters.size());
            return transformParametersForAdditionalPlot(plot, params.data());
        }

        /** \brief returns the description of the i-th parameter */
        inline ParameterDescription getDescription(int i) const  {
            return m_parameters[i];
        }

        /** \brief returns the description of the parameter id */
        inline ParameterDescription getDescription(QString id) const  {
            int i=getParameterNum(id);
            if (i>-1) return m_parameters[i];
            else return ParameterDescription();
        }

        /** \brief get num of the given parameter or -1 */
        inline int getParameterNum(QString param) const {
            for (int i=0; i<m_parameters.size(); i++) {
                if (m_parameters[i].id==param) return i;
            }
            return -1;
        }

        /** \brief return \c true if a parameter wit the given id exists */
        inline bool hasParameter(QString id) const {
            return (getParameterNum(id)>=0);
        }

        /** \brief get id of the given parameter or an empty string */
        inline QString getParameterID(int i) const {
            if ((i>=0) && (i<m_parameters.size())) return m_parameters[i].id;
            return QString("");
        }

        /** \brief return a list with the ids of all parameters of this model */
        inline QStringList getParameterIDs() const {
            QStringList sl;
            for (int i=0; i<m_parameters.size(); i++) sl.append(m_parameters[i].id);
            return sl;
        }

        /** \brief custom transform function for parameter i */
        virtual double customTransform(int parameter, double value) const;
        /** \brief custom transform function for parameter i */
        virtual double customBackTransform(int parameter, double value) const;



        /** \brief returns the vector of initial fit parameter values */
        QVector<double> getInitialParamValues() const;
        /** \brief returns the vector of minimum fit parameter values */
        QVector<double> getInitialParamMins() const;
        /** \brief returns the vector of maximum fit parameter values */
        QVector<double> getInitialParamMaxs() const;
        /** \brief returns the vector of minimum fit parameter values */
        QVector<double> getParamAbsMins() const;
        /** \brief returns the vector of maximum fit parameter values */
        QVector<double> getParamAbsMaxs() const;
        /** \brief returns the vector of initial fit parameter fixes */
        QVector<bool> getInitialParamFixes() const;

    protected:
        /*! \brief add a parameter description

            used in the constructor to define the model parameters
            \param description parameter description to be aded
            \return the id of the parameter
         */
        inline int addParameter(const ParameterDescription& description) {
            m_parameters.append(description);
            return m_parameters.size()-1;
        }
        /*! \brief add a parameter description

            used in the constructor to define the model parameters
            \return the id of the parameter
         */
        inline int addParameter(ParameterType paramType, ParameterWidgetType widgetType, const QString& id, const QString& name, const QString& label, const QString& unit, const QString& unitLabel, bool fit, bool userEditable, bool userRangeEditable, ErrorDisplayMode displayError, bool initialFix, double initialValue, double minValue=-DBL_MAX, double maxValue=DBL_MAX, double inc=1, double absMinValue=-DBL_MAX, double absMaxValue=DBL_MAX, const QStringList& comboItems=QStringList()) {
            ParameterDescription d;
            d.widgetType=widgetType;
            d.parameterType=paramType;
            d.id=id;
            d.name=name;
            d.label=label;
            d.unit=unit;
            d.unitLabel=unitLabel;
            d.fit=fit;
            d.userEditable=userEditable;
            d.initialValue=initialValue;
            d.minValue=minValue;
            d.maxValue=maxValue;
            d.inc=inc;
            d.displayError=displayError;
            d.userRangeEditable=userRangeEditable;
            d.absMaxValue=absMaxValue;
            d.absMinValue=absMinValue;
            d.comboItems=comboItems;
            d.initialFix=initialFix;

            return addParameter(d);
        }
        /*! \brief add a parameter description for a standard parameter

            used in the constructor to define the model parameters
            \return the id of the parameter
         */
        inline int addParameter(ParameterWidgetType widgetType, const QString& id, const QString& name, const QString& label, const QString& unit, const QString& unitLabel, bool fit, bool userEditable, bool userRangeEditable, ErrorDisplayMode displayError, bool initialFix, double initialValue, double minValue=-DBL_MAX, double maxValue=DBL_MAX, double inc=1, double absMinValue=-DBL_MAX, double absMaxValue=DBL_MAX, const QStringList& comboItems=QStringList()) {
            return addParameter(StandardParameter, widgetType,  id,  name,  label,  unit,  unitLabel,  fit,  userEditable,  userRangeEditable,  displayError,  initialFix,  initialValue,  minValue,  maxValue,  inc,  absMinValue,  absMaxValue,  comboItems);
        }
        /*! \brief add a parameter description for a logarithmic parameter

            used in the constructor to define the model parameters
            \return the id of the parameter
         */
        inline int addLogParameter(ParameterWidgetType widgetType, const QString& id, const QString& name, const QString& label, const QString& unit, const QString& unitLabel, bool fit, bool userEditable, bool userRangeEditable, ErrorDisplayMode displayError, bool initialFix, double initialValue, double minValue=-DBL_MAX, double maxValue=DBL_MAX, double inc=1, double absMinValue=-DBL_MAX, double absMaxValue=DBL_MAX, const QStringList& comboItems=QStringList()) {
            return addParameter(LogParameter, widgetType,  id,  name,  label,  unit,  unitLabel,  fit,  userEditable,  userRangeEditable,  displayError,  initialFix,  initialValue,  minValue,  maxValue,  inc,  absMinValue,  absMaxValue,  comboItems);
        }

        QList<ParameterDescription> m_parameters;
};

#endif // QFFITFUNCTIONBASE_H
