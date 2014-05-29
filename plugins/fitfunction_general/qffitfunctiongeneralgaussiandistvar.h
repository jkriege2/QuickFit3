#ifndef QFFitFunctionGeneralGaussianDistVar_H
#define QFFitFunctionGeneralGaussianDistVar_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for fit function
    \ingroup qf3fitfunp_fitfunctions_general

*/
class QFFitFunctionGeneralGaussianDistVar: public QFFitFunction {
    public:
        QFFitFunctionGeneralGaussianDistVar();
        virtual ~QFFitFunctionGeneralGaussianDistVar() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QObject::tr("Distributions: Gaussian distribution function (1/sqrt(e) width)"); }
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return name(); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("gen_gaussiandist_sqrte"); }

        /*! \copydoc QFFitFunction::evaluate()   */
        virtual double evaluate(double t, const double* parameters) const;

        /*! \copydoc QFFitFunction::calcParameter()   */
        virtual void calcParameter(double* parameterValues, double* error=NULL) const;

        /*! \copydoc QFFitFunction::isParameterVisible()   */
        virtual bool isParameterVisible(int parameter, const double* parameterValues) const;
        /*! \copydoc QFFitFunction::getAdditionalPlotCount()   */
        virtual unsigned int getAdditionalPlotCount(const double* params);

        /*! \copydoc QFFitFunction::transformParametersForAdditionalPlot()   */
        virtual QString transformParametersForAdditionalPlot(int plot, double* params);

        /*! \copydoc QFFitFunction::get_implementsDerivatives()   */
        virtual bool get_implementsDerivatives();

        /*! \copydoc QFFitFunction::evaluateDerivatives()   */
        virtual void evaluateDerivatives(double* derivatives, double t, const double* parameters) const ;
        bool estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool *fix=NULL);
};

#endif // QFFitFunctionGeneralGaussianDistVar_H
