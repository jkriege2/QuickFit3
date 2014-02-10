#ifndef QFFITFUNCTIONFCSEXP_H
#define QFFITFUNCTIONFCSEXP_H
#include "qfpluginfitfunction.h"
#include "qffitfunction.h"


/*! \brief plugin class for FCS exponential function
    \ingroup qf3fitfunp_fcs

*/
class QFFitFunctionFCSExp: public QFFitFunction {
    public:
        QFFitFunctionFCSExp();
        virtual ~QFFitFunctionFCSExp() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("FCS: Exponential Function"); };
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fcs_exp"); };

        /*! \copydoc QFFitFunction::evaluate()   */
        virtual double evaluate(double x, const double* parameters) const;

        /*! \copydoc QFFitFunction::calcParameter()   */
        virtual void calcParameter(double* parameterValues, double* error=NULL) const;

        /*! \copydoc QFFitFunction::isParameterVisible()   */
        virtual bool isParameterVisible(int parameter, const double* parameterValues) const;
        /*! \copydoc QFFitFunction::getAdditionalPlotCount()   */
        virtual unsigned int getAdditionalPlotCount(const double* params);

        /*! \copydoc QFFitFunction::transformParametersForAdditionalPlot()   */
        virtual QString transformParametersForAdditionalPlot(int plot, double* params);



};

#endif // QFFITFUNCTIONFCSEXP_H
