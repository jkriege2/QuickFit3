#ifndef QFFITFUNCTIONFCSSIMPLEEXP_H
#define QFFITFUNCTIONFCSSIMPLEEXP_H
#include "qfpluginfitfunction.h"
#include "qffitfunction.h"


/*! \brief plugin class for FCS exponential function
    \ingroup qf3fitfunp_fcs

*/
class QFFitFunctionFCSSimpleExp: public QFFitFunction {
    public:
        QFFitFunctionFCSSimpleExp();
        virtual ~QFFitFunctionFCSSimpleExp() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("FCS: Sum of Exponential Functions"); };
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fcs_simpleexp"); };

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

#endif // QFFITFUNCTIONFCSSIMPLEEXP_H
