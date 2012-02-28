#ifndef QFFITFUNCTION_H
#define QFFITFUNCTION_H

#include <QString>
#include <QList>
#include <QStringList>
#include <cfloat>
#include <cmath>
#include "qfmathtools.h"
#include "lib_imexport.h"

/*! \brief describes a virtual base class fitting functions that are used together with QFFitAlgorithm objects.
    \ingroup qf3lib_fitting

    Mathematically this evaluates a 1D function \f$ f(x, \vec{p})\in\mathbb{R} \f$ at a position \f$ x\in\mathbb{R} \f$
    and of a parameter vector \f$ \vec{p}\in\mathbb{R}^N \f$ . This evaluation is implemented in evaluate().

    The parameters of the function may either be fit parameters or not. Non-fit parameters are used to calculate additional
    values from the fit parameters. This is done in calcParameter() which is called after a fit run, or when the user changes
    some parameters. This way you may use some of the parameters to configure the model (e.g. select the number of components).

    For some fitting algorithms it is advantageous to know the derivatives of the fitting function
    \f$ J_n=\frac{\partial f}{\partial p_n}(x, \vec{p}) \f$ . Note that these derivatives are only needed fot fit parameters!
    Do not implement the rest, as this will decrease the performance of the fitting algorithm. The derivatives are implemented
    in evaluateDerivatives(). If you do not implement the derivatives, return \c false in get_implementsDerivatives(). Then the
    fitting algorithm will have to implement the calculation of derivative numerically, if they are needed.

    Each fit parameter is described by several parameters:
      - \c type: the type of the widget used to edit the parameter
      - \c id: a uniquie (in the scope of the function) ID for the parameter. Do not use whitespaces int his ID. It will
               be used to store the parameter values in then project file and to reference the parameter internally
      - \c name: description of the parameter, e.g. \verbatim "particle number N" \endverbatim
      - \c label: a label for the parameter, e.g. \verbatim "<i>N</i>" \endverbatim You may use HTML markup for this
      - \c unit: unit of the parameter, e.g. \verbatim "usec", "g" \endverbatim
      - \c unitLabel: unit of the parameter, e.g. \verbatim "&mu;s", "g" \endverbatim as HTML markupped label
      - \c fit: \c true if this parameter is a fit parameter
      - \c userEditable: \c true if the parameter may be edited by the user
      - \c displayError: \c true if a widget shall be displayed that shows the error associated with this parameter
      - \c userRangeEditable: \c true if the user may change the range of this parameter
      - \c initialValue: an initial value for the parameter
      - \c minValue: initial minimal value for the parameter
      - \c maxValue: initial maximal value for the parameter
      - \c absMinValue: absolute minimal value for the parameter, or \c -DBL_MAX if disabled
      - \c absMaxValue: absolute maximal value for the parameter, or \c DBL_MAX if disabled
      - \c inc: increment if a user clicks on a "+" or "" button in the widget used to edit the parameter (spin boxes)
    .

    You have to declare all used parameters in the constructor by calling addParameter().

    It is also possible to define transformed parameter sets for plotting. By default the function graph with the full parameter
    set should be plottet. If you define additional graphs, you have to implement getAdditionalPlotCount() and transformParametersForAdditionalPlot().
    The first function should return the number of additional plots whereas the second transforms the plot parameters so they may be used to
    plot the additional function by calling evaluate().


*/
class QFLIB_EXPORT QFFitFunction {
    public:
        /** \brief different types of input widgets for parameters */
        enum ParameterType {
            FloatNumber=0, /**< a floating point number edit widget is used for the parameter*/
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
        struct QFLIB_EXPORT ParameterDescription {
            /** \brief type of the parameter */
            ParameterType type;
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

            ParameterDescription() {
                type=Invalid;
                id="";
                name="";
                label="";
                unit="";
                fit=false;
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
            }
        };

        virtual ~QFFitFunction() {}

        /** \brief returns the number of parameters */
        virtual int paramCount() const {
            return m_parameters.size();
        }
        /** \brief return a name for the model */
        virtual QString name() const=0;
        /** \brief return a short name for the model (max. around 10-20 characters) */
        virtual QString shortName() const { return name(); };
        /** \brief return a short unique model ID string */
        virtual QString id() const=0;

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
        virtual bool get_implementsDerivatives() { return false; };

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
        virtual void sortParameter(double* parameterValues, double* error=NULL) const {};

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
        virtual QString transformParametersForAdditionalPlot(int plot, double* params) { return QString(""); }

        /** \brief returns the description of the i-th parameter */
        ParameterDescription getDescription(int i) const  {
            return m_parameters[i];
        }

        /** \brief returns the description of the parameter id */
        ParameterDescription getDescription(QString id) const  {
            int i=getParameterNum(id);
            if (i>-1) return m_parameters[i];
            else return ParameterDescription();
        }

        /** \brief get num of the given parameter or -1 */
        int getParameterNum(QString param) const {
            for (int i=0; i<m_parameters.size(); i++) {
                if (m_parameters[i].id==param) return i;
            }
            return -1;
        }

        /** \brief return \c true if a parameter wit the given id exists */
        bool hasParameter(QString id) {
            return (getParameterNum(id)>=0);
        }

        /** \brief get id of the given parameter or an empty string */
        QString getParameterID(int i) {
            if ((i>=0) && (i<m_parameters.size())) return m_parameters[i].id;
            return QString("");
        }

        /** \brief return a list with the ids of all parameters of this model */
        QStringList getParameterIDs() {
            QStringList sl;
            for (int i=0; i<m_parameters.size(); i++) sl.append(m_parameters[i].id);
            return sl;
        }


        /*! \brief calculate fit statistics for the given measurement dataset, using this fit function with the given parameters

            \return a wide set of fit statistics in a struct of type FitStatistics
            \param N number of datapoints in corrdata and weights
            \param tauvals input data set x (the values f(x) are given in corrdata)
            \param corrdata input dataset f(x)
            \param weights weights for the input dataset
            \param datacut_min first data point to use
            \param datacut_max last datapoint to use
            \param fullParams parameter vector
            \param errors errors of the fit parameters in fullParams
            \param paramsFix which parameters are fixed
            \param runAvgWidth width of the averaging in the running averge
            \param residualHistogramBins bins in the residual histogram

            \note the arrays in the resulting struct are allocated using \c malloc(), so you will have to free them using \c free() !!!
          */
        QFFitStatistics calcFitStatistics(long N, double* tauvals, double* corrdata, double* weights, int datacut_min, int datacut_max, double* fullParams, double* errors, bool* paramsFix, int runAvgWidth, int residualHistogramBins);
    protected:
        /*! \brief add a parameter description

            used in the constructor to define the model parameters
            \param description parameter description to be aded
            \return the id of the parameter
         */
        int addParameter(const ParameterDescription& description) {
            m_parameters.append(description);
            return m_parameters.size()-1;
        }
        /*! \brief add a parameter description

            used in the constructor to define the model parameters
            \return the id of the parameter
         */
        int addParameter(ParameterType type, QString id, QString name, QString label, QString unit, QString unitLabel, bool fit, bool userEditable, bool userRangeEditable, ErrorDisplayMode displayError, double initialValue, double minValue, double maxValue, double inc, double absMinValue=-DBL_MAX, double absMaxValue=DBL_MAX, QStringList comboItems=QStringList()) {
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

            return addParameter(d);
        }

    private:
        QList<ParameterDescription> m_parameters;
};

#endif // QFFITFUNCTION_H
