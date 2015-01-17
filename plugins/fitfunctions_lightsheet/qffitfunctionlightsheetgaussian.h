#ifndef QFFITFUNCTIONLIGHTSHEETGAUSSIAN_H
#define QFFITFUNCTIONLIGHTSHEETGAUSSIAN_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for fit function
    \ingroup qf3fitfunp_fitfunctions_lightsheet

*/
class QFFitFunctionLightsheetGaussian: public QFFitFunction {
    public:
        QFFitFunctionLightsheetGaussian();
        virtual ~QFFitFunctionLightsheetGaussian() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("SPIM lightsheet: gaussian, 1/e^2-halfwidth"); };
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("SPIM lightsheet: gaussian (1/e^2)"); };
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("lightsheet_gaussian"); };

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
        virtual bool get_implementsDerivatives() const { return false; };

        /*! \copydoc QFFitFunction::evaluateDerivatives()   */
        virtual void evaluateDerivatives(double* derivatives, double t, const double* parameters) const ;
        /*! \copydoc QFFitFunction::estimateInitial()   */
        virtual bool estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool *fix);
};

#endif // QFFITFUNCTIONLIGHTSHEETGAUSSIAN_H
