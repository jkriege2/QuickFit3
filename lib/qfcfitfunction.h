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

#ifndef QFCFITFUNCTION_H
#define QFCFITFUNCTION_H

#  ifndef __WINDOWS__
#    if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#      define __WINDOWS__
#    endif
#  endif

#ifdef __WINDOWS__
#  ifdef LIB_IN_DLL
#    if defined(LIB_EXPORT_LIBRARY)
#      define LIB_EXPORT __declspec(dllexport)
#    else
#      define LIB_EXPORT __declspec(dllimport)
#    endif
#  else
#    define LIB_EXPORT
#  endif
#else
#  define LIB_EXPORT
#endif

#include <cmath>
#include <climits>
#include <cfloat>
#include <cstring>
#include <stdlib.h>

/** \brief different types of input widgets for parameters */
enum QFCFitFunctionParameterType {
    FloatNumber=0, /**< a floating point number edit widget is used for the parameter*/
    LogFloatNumber=3, /**< a floating point number edit widget is used for the parameter, which uses a logarithmic stepping scheme*/
    IntNumber=1,   /**< an integer number edit widget is used for the parameter*/
    IntCombo=2,    /**< a combobox widget is used for the parameter*/
    Invalid=100    /**< invalid widget */
};

/** \brief error display mode */
enum QFCFitFunctionErrorDisplayMode {
    NoError=0,      /**< do not display an error widget */
    DisplayError=1, /**< display a label for the error */
    EditError=2     /**< display a widget to edit the error */

};

/** \brief this struct is used to describe the fitting parameters */
struct  QFCFitFunctionParameterDescription {
    /** \brief type of the parameter */
    QFCFitFunctionParameterType type;
    /** \brief short unique id for the parameter, e.g. "n" */
    char* id;
    /** \brief description of the parameter, e.g. "particle number N" */
    char* name;
    /** \brief a label for the parameter, e.g. "<i>N</i>". You may use HTML markup for this */
    char* label;
    /** \brief unit of the parameter, e.g. "usec", "g"  */
    char* unit;
    /** \brief unit label of the parameter, e.g. "&mu;s", "g". You may use HTML markup for this  */
    char* unitLabel;
    /** \brief is this a fitting parameter, if \c false the user may supply a value, but the value will never be changed by the fitting
     *         algorithm. Use this e.g. to configure a model (number of components, ...) */
    bool fit;
    /** \brief determine whether, or not the user is allowed to change the value of a parameter. You could use a parameter with
     *         \c fit=userEditable=false to output values that were calculated from the fitting parameters after the fit. */
    bool userEditable;
    /** \brief determine whether, or not an error value is displayed for this parameter */
    QFCFitFunctionErrorDisplayMode displayError;
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
    /** \brief names for the items in the combobox (if type==IntCombo), separated by ";;" */
    char* comboItems;

    QFCFitFunctionParameterDescription* next;
};




class LIB_EXPORT QFCFitFunction {
    public:
        /** \brief different types of input widgets for parameters */
        enum ParameterType {
            FloatNumber=0, /**< a floating point number edit widget is used for the parameter*/
            LogFloatNumber=3, /**< a floating point number edit widget is used for the parameter, which uses a logarithmic stepping scheme*/
            IntNumber=1,   /**< an integer number edit widget is used for the parameter*/
            IntCombo=2,    /**< a combobox widget is used for the parameter*/
            Invalid=100    /**< invalid widget */
        };

        /** \brief error display mode */
        enum ErrorDisplayMode {
            NoError=0,      /**< do not display an error widget */
            DisplayError=1, /**< display a label for the error */
            EditError=2     /**< display a widget to edit the error */

        };
        /** \brief this struct is used to describe the fitting parameters */
        struct LIB_EXPORT ParameterDescription {
            /** \brief type of the parameter */
            ParameterType type;
            /** \brief short unique id for the parameter, e.g. "n" */
            std::string id;
            /** \brief description of the parameter, e.g. "particle number N" */
            std::string name;
            /** \brief a label for the parameter, e.g. "<i>N</i>". You may use HTML markup for this */
            std::string label;
            /** \brief unit of the parameter, e.g. "usec", "g"  */
            std::string unit;
            /** \brief unit label of the parameter, e.g. "&mu;s", "g". You may use HTML markup for this  */
            std::string unitLabel;
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
            std::vector<std::string> comboItems;

            ParameterDescription() {
                type=Invalid;
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
                comboItems=std::vector<std::string>();
            }
        };

        virtual ~QFCFitFunction() {}

        /** \brief returns the number of parameters */
        virtual int paramCount() const {
            return m_parameters.size();
        }
        /** \brief return a name for the model */
        virtual std::string name() const { return id(); }
        /** \brief return a short name for the model (max. around 10-20 characters) */
        virtual std::string shortName() const { return name(); };
        /** \brief return a short unique model ID string */
        virtual std::string id() const=0;

        /*! \brief evaluate the fitting function \f$ f(x, \vec{p}) \f$ at the position \f$ x \f$ with the given parameter vector \f$ \vec{p} \f$
            \param x position \f$ x \f$ where to evaluate the fit function
            \param parameters parameter vector \f$ \vec{p}\in\mathbb{R}^N \f$
            \return the fitting function \f$ f(x, \vec{p}) \f$ at the position \f$ x \f$ with the given parameter vector \f$ \vec{p} \f$
         */
        virtual double evaluate(double x, const double* parameters) const =0;

        /*! \brief evaluate the fitting function derivatives  \f$ J_n=\frac{\partial f}{\partial p_n}(x, \vec{p}) \f$ at the position
                   \f$ x \f$ with the given parameter vector \f$ \vec{p} \f$
            \param[out] derivatives as a vector \f$ \left[\frac{\partial f}{\partial p_1}, \frac{\partial f}{\partial p_2}, ..., \frac{\partial f}{\partial p_N}\right] \f$ .
                        Only the derivaties for fit parameters are calculated.
            \param x position \f$ x \f$ where to evaluate the fit function derivatives
            \param parameters parameter vector \f$ \vec{p}\in\mathbb{R}^N \f$
            \note this is only implemented when get_implementsDerivatives() returns \c true
         */
        virtual void evaluateDerivatives(double* derivatives, double x, const double* parameters) const {};

        /** \brief retrun \c if this fit function implements analytic derivatives in evaluateDerivatives() */
        virtual bool get_implementsDerivatives() const { return false; };

        /*! \brief calculate non-fit parameters and their errors (if \a error is supplied), i.e. \c fit=userEditable=false
            \param[in,out] parameterValues the parameter values on which to base the calculation. This is also
                           used to write back the calculated values.
            \param[in,out] error if \c !=NULL this contains the errors of the parameters in parameterValues. the calculated
                           errors are also stored in this array.
            \see sortParameter()
        */
        virtual void calcParameter(double* parameterValues, double* error=NULL) const {};

        /*! \brief Sometimes it is desireable to sort the fit parameters in some given order. This function may be uused to do so.
                   <b>DO NOT SORT IN calcParameter()</b>
            \param[in,out] parameterValues the parameter values which to sort
            \param[in,out] error if \c !=NULL this contains the errors to be sorted
            \see calcParameter()
        */
        virtual void sortParameter(double* parameterValues, double* error=NULL, bool* fix=NULL) const {};

        /*! \brief returns \c true if the given parameter is currently visible (which could e.g. depend on the setting of the other parameters)
            \param parameter which parameter to test
            \param parameterValues the parameter values on which to base the decission
            \return \c true if parameter is visibled (based on the parameter values in \a parameterValues )
        */
        virtual bool isParameterVisible(int parameter, const double* parameterValues) const { return true; };

        /*! \brief return the number of graphs that should be plotted additional to the function grph itself
            \param params The decision may be based on this parameter set.
            \return number of additional plots, or 0 if none
        */
        virtual unsigned int getAdditionalPlotCount(const double* params) { return 0; }

        /*! \brief transform the given parameter vector so tht it my be used to plot the \a plot -th additional graph
            \param plot the number of the plot to plot
            \param[in,out] params parameter vector. This is assumed to be filled with the full parameters when the function is called
            \return label/name for the graph
        */
        virtual std::string transformParametersForAdditionalPlot(int plot, double* params) { return std::string(""); }

        /*! \brief if implemented (and returns \c true) this function tries to estimate the initial parameters of a fit function from provided data. If called with NULL parameters, this should still return, whether guessing is possible at all!

            The parameter fix is optional and may be ignored by the function. It indicates which values should be fixed to the value given in params for the estimation.
         */
        virtual bool estimateInitial(double* params, const double* dataX, const double* dataY, long N, const bool *fix=NULL) { return false; }

        /** \brief returns the description of the i-th parameter */
        const std::vector<ParameterDescription>& getParameters() const { return m_parameters; }

    protected:
        /*! \brief add a parameter description

            used in the constructor to define the model parameters
            \param description parameter description to be aded
            \return the id of the parameter
         */
        int addParameter(const ParameterDescription& description) {
            m_parameters.push_back(description);
            return m_parameters.size()-1;
        }
        /*! \brief add a parameter description

            used in the constructor to define the model parameters
            \return the id of the parameter
         */
        int addParameter(ParameterType type, std::string id, std::string name, std::string label, std::string unit, std::string unitLabel, bool fit, bool userEditable, bool userRangeEditable, ErrorDisplayMode displayError, bool initialFix, double initialValue, double minValue=-DBL_MAX, double maxValue=DBL_MAX, double inc=1, double absMinValue=-DBL_MAX, double absMaxValue=DBL_MAX, std::vector<std::string> comboItems=std::vector<std::string>()) {
            ParameterDescription d;
            d.type=type;
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

    private:
        std::vector<ParameterDescription> m_parameters;
};


#define EXPORT_FITFUNCTION(classname, author) \
    QFCFitFunction* LIB_EXPORT ffCreate() { \
        return new classname();\
    }\
    std::string LIB_EXPORT ffGetAuthor() { \
        return std::string(author);\
    }

#endif // QFCFITFUNCTION_H
