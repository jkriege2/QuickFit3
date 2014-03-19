#ifndef QFFitFunctionFCSPowerLaw_H
#define QFFitFunctionFCSPowerLaw_H
#include "qfpluginfitfunction.h"
#include "qffitfunction.h"


/*! \brief plugin class for FCS exponential function
    \ingroup qf3fitfunp_fcs

*/
class QFFitFunctionFCSPowerLaw: public QFFitFunction {
    public:
        QFFitFunctionFCSPowerLaw();
        virtual ~QFFitFunctionFCSPowerLaw() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("FCS: Power Law Decay Function"); };
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fcs_powerlaw"); };

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

#endif // QFFitFunctionFCSPowerLaw_H
