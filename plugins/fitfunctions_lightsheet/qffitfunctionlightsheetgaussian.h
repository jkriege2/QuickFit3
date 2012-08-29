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
        virtual QString name() const { return QString("gaussian SPIM lightsheet, 1/e²-halfwidth"); };
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("lightsheet: gaussian (1/e²)"); };
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
        virtual bool get_implementsDerivatives() { return false; };

        /*! \copydoc QFFitFunction::evaluateDerivatives()   */
        virtual void evaluateDerivatives(double* derivatives, double t, const double* parameters) const ;
};

#endif // QFFITFUNCTIONLIGHTSHEETGAUSSIAN_H
