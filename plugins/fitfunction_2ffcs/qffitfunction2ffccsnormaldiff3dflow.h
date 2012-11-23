#ifndef QFFitFunctionFCCSNormalDiff3DFlow2Focus_H
#define QFFitFunctionFCCSNormalDiff3DFlow2Focus_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for fit function
    \ingroup qf3fitfunp_fitfunctions_2ffcs

*/
class QFFitFunctionFCCSNormalDiff3DFlow2Focus: public QFFitFunction {
    public:
        QFFitFunctionFCCSNormalDiff3DFlow2Focus(bool hasGamma);
        virtual ~QFFitFunctionFCCSNormalDiff3DFlow2Focus() {}

        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const;

        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const;

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
        virtual bool get_implementsDerivatives() { return false; }
    protected:
        bool hasGamma;
};

#endif // QFFitFunctionFCCSNormalDiff3DFlow2Focus_H
