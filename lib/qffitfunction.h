/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

#ifndef QFFITFUNCTION_H
#define QFFITFUNCTION_H
#include <stdint.h>
#include <QString>
#include <QList>
#include <QStringList>
#include <cfloat>
#include <cmath>
#include "qfmathtools.h"
#include "lib_imexport.h"
#include "qffitfunctionbase.h"

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
class QFLIB_EXPORT QFFitFunction: public QFFitFunctionBase {
    public:
        virtual ~QFFitFunction() {}


        /*! \brief evaluate the fitting function \f$ f(x, \vec{p}) \f$ at the position \f$ x \f$ with the given parameter vector \f$ \vec{p} \f$
            \param x position \f$ x \f$ where to evaluate the fit function
            \param parameters parameter vector \f$ \vec{p}\in\mathbb{R}^N \f$
            \return the fitting function \f$ f(x, \vec{p}) \f$ at the position \f$ x \f$ with the given parameter vector \f$ \vec{p} \f$
         */
        virtual double evaluate(double x, const double* parameters) const =0;
        inline virtual double evaluate(double x, const QVector<double>& parameters) const {
            return evaluate(x, parameters.constData());
        }


        /*! \brief evaluate the fitting function \f$ f(x_i, \vec{p}) \f$ at the positions \f$ x_i \f$ with the given parameter vector \f$ \vec{p} \f$
            \param x vector of positions \f$ \vec{x}=(x_i) \f$ where to evaluate the fit function
            \param parameters parameter vector \f$ \vec{p}\in\mathbb{R}^N \f$
            \return the fitting function \f$ y_i=f(x_i, \vec{p}) \f$ at the positions \f$ x_i \f$ with the given parameter vector \f$ \vec{p} \f$
         */
        QVector<double> multiEvaluate(const QVector<double>& x, const double* parameters) const;
        QVector<double> multiEvaluate(const double* x, long long N, const double* parameters) const;
        inline QVector<double> multiEvaluate(const QVector<double>& x, const QVector<double>& parameters) const {
            return multiEvaluate(x, parameters.constData());
        }

        /*! \brief evaluate the fitting function \f$ f(x_i, \vec{p}) \f$ at the positions \f$ x_i \f$ with the given parameter vector \f$ \vec{p} \f$
            \param y the fitting function \f$ y_i=f(x_i, \vec{p}) \f$ at the positions \f$ x_i \f$ with the given parameter vector \f$ \vec{p} \f$
            \param x vector of positions \f$ \vec{x}=(x_i) \f$ where to evaluate the fit function
            \param N items in x and y
            \param parameters parameter vector \f$ \vec{p}\in\mathbb{R}^N \f$
         */
        virtual void multiEvaluate(double* y, const double* x, uint64_t N,  const double* parameters) const;

        /*! \brief evaluate the fitting function \f$ f(x_i, \vec{p}) \f$ at the positions \f$ x_i \f$ with the given parameter vector \f$ \vec{p} \f$
            \param y the fitting function \f$ y_i=f(x_i, \vec{p}) \f$ at the positions \f$ x_i \f$ with the given parameter vector \f$ \vec{p} \f$
            \param x vector of positions \f$ \vec{x}=(x_i) \f$ where to evaluate the fit function
            \param N items in x and y
            \param parameters parameter vector \f$ \vec{p}\in\mathbb{R}^N \f$
         */
        inline virtual void multiEvaluate(double* y, const double* x, uint64_t N,  const QVector<double>& parameters) const {
            multiEvaluate(y, x, N, parameters.constData());
        }

        /*! \brief evaluate the fitting function derivatives  \f$ J_n=\frac{\partial f}{\partial p_n}(x, \vec{p}) \f$ at the position
                   \f$ x \f$ with the given parameter vector \f$ \vec{p} \f$
            \param[out] derivatives as a vector \f$ \left[\frac{\partial f}{\partial p_1}, \frac{\partial f}{\partial p_2}, ..., \frac{\partial f}{\partial p_N}\right] \f$ .
                        Only the derivaties for fit parameters are calculated.
            \param x position \f$ x \f$ where to evaluate the fit function derivatives
            \param parameters parameter vector \f$ \vec{p}\in\mathbb{R}^N \f$
            \note this is only implemented when get_implementsDerivatives() returns \c true
         */
        virtual void evaluateDerivatives(double* derivatives, double x, const double* parameters) const ;
        inline virtual void evaluateDerivatives(double* derivatives, double x, const QVector<double>&  parameters) const {
            evaluateDerivatives(derivatives, x, parameters.constData());
        }
        inline virtual void evaluateDerivatives(QVector<double>& derivatives, double x, const QVector<double>&  parameters) const {
            derivatives.resize(m_parameters.size());
            evaluateDerivatives(derivatives.data(), x, parameters.constData());
        }
        inline virtual void evaluateDerivatives(QVector<double>& derivatives, double x, const double*  parameters) const {
            derivatives.resize(m_parameters.size());
            evaluateDerivatives(derivatives.data(), x, parameters);
        }




        /*! \brief if implemented (and returns \c true) this function tries to estimate the initial parameters of a fit function from provided data. If called with NULL parameters, this should still return, whether guessing is possible at all!

            The parameter fix is optional and may be ignored by the function. It indicates which values should be fixed to the value given in params for the estimation.
         */
        virtual bool estimateInitial(double* params, const double* dataX, const double* dataY, long N, const bool *fix=NULL);
        inline virtual bool estimateInitial(QVector<double>& params, const QVector<double>& dataX, const QVector<double>& dataY, const QVector<bool>& fix) {
            params.resize(m_parameters.size());
            return estimateInitial(params.data(), dataX.constData(), dataY.constData(), qMin(dataX.size(), dataY.size()), fix.constData());
        }
        inline virtual bool estimateInitial(QVector<double>& params, const QVector<double>& dataX, const QVector<double>& dataY) {
            params.resize(m_parameters.size());
            return estimateInitial(params.data(), dataX.constData(), dataY.constData(), qMin(dataX.size(), dataY.size()), NULL);
        }

        /*! \brief numerically estimates the fitting function derivatives  \f$ J_n=\frac{\partial f}{\partial p_n}(x, \vec{p}) \f$ at the position
                   \f$ x \f$ with the given parameter vector \f$ \vec{p} \f$
            \param[out] derivatives as a vector \f$ \left[\frac{\partial f}{\partial p_1}, \frac{\partial f}{\partial p_2}, ..., \frac{\partial f}{\partial p_N}\right] \f$ .
                        Only the derivaties for fit parameters are calculated.
            \param x position \f$ x \f$ where to evaluate the fit function derivatives
            \param parameters parameter vector \f$ \vec{p}\in\mathbb{R}^N \f$
            \param stepsize stepsize used to evaluate the derivatives
            \note this function uses the 5-point rule to estimate the derivative

            \see <a href="http://hep1.phys.ntu.edu.tw/~kfjack/lecture/02/lecture-02.pdf">http://hep1.phys.ntu.edu.tw/~kfjack/lecture/02/lecture-02.pdf</a>
                 <a href="http://oregonstate.edu/instruct/ch490/lessons/lesson11.htm">http://oregonstate.edu/instruct/ch490/lessons/lesson11.htm</a>
                 <a href="http://en.wikipedia.org/wiki/Five-point_stencil">http://en.wikipedia.org/wiki/Five-point_stencil</a>
         */
        void evaluateNumericalDerivatives(double* derivatives, double x, const double* parameters, double stepsize=30.0*sqrt(DBL_EPSILON)) const;
        void evaluateNumericalDerivatives(QVector<double>& derivatives, double x, const QVector<double>& parameters, double stepsize=30.0*sqrt(DBL_EPSILON)) const {
            evaluateNumericalDerivatives(derivatives.data(), x, parameters.constData(), stepsize);
        }
        void evaluateNumericalDerivatives(QVector<double>& derivatives, double x, const double* parameters, double stepsize=30.0*sqrt(DBL_EPSILON)) const {
            evaluateNumericalDerivatives(derivatives.data(), x, parameters, stepsize);
        }
        QVector<double> evaluateNumericalDerivatives(double x, const QVector<double>& parameters, double stepsize=30.0*sqrt(DBL_EPSILON)) const {
            QVector<double> vec;
            vec.resize(m_parameters.size());
            evaluateNumericalDerivatives(vec.data(), x, parameters.constData(), stepsize);
            return vec;
        }

        void evaluateNumericalParameterErrors(double* errors, double x, const double* parameters, double residualSigma2=1.0, double stepsize=30.0*sqrt(DBL_EPSILON)) const;
        QVector<double> evaluateNumericalParameterErrors(double x, const QVector<double>& parameters, double residualSigma2=1.0, double stepsize=30.0*sqrt(DBL_EPSILON)) const {
            QVector<double> vec;
            vec.resize(m_parameters.size());
            evaluateNumericalParameterErrors(vec.data(), x, parameters.constData(), residualSigma2, stepsize);
            return vec;
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

            \note the arrays in the resulting struct are allocated using \c qfMalloc(), so you will have to free them using \c qfFree() !!!
          */
        QFFitStatistics calcFitStatistics(long N, const double *tauvals, const double *corrdata, const double *weights, int datacut_min, int datacut_max, const double *fullParams, const double *errors, const bool *paramsFix, int runAvgWidth, int residualHistogramBins) const;

};

#endif // QFFITFUNCTION_H
