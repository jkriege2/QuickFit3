#ifndef QFFitFunctionCSFCSDiffD_H
#define QFFitFunctionCSFCSDiffD_H
#include "qfpluginfitfunction.h"
#include "qffitfunction.h"


/*! \brief plugin class for circle scanning FCS Normal Diffusion Fit Functions
    \ingroup qf3fitfunp_fcs


*/
class QFFitFunctionCSFCSDiffD: public QFFitFunction {
    public:
    QFFitFunctionCSFCSDiffD(bool hasGamma=true);
    virtual ~QFFitFunctionCSFCSDiffD() {}

        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const;
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const;

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

    protected:
        bool hasGamma;

};

#endif // QFFitFunctionCSFCSDiffD_H
