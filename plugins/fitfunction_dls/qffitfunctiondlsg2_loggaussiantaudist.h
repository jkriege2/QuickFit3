#ifndef QFFitFunctionDLSG2LogNormTauDist_H
#define QFFitFunctionDLSG2LogNormTauDist_H
#include "qfpluginfitfunction.h"
#include <gsl/gsl_integration.h>
#include <gsl/gsl_errno.h>



/*! \brief QFFitFunction class for a tri-exponential G2 autocorrelation function in DLS
    \ingroup qf3fitfunp_fitfunctions_dls

*/
class QFFitFunctionDLSG2LogNormTauDist: public QFFitFunction {
    public:
        QFFitFunctionDLSG2LogNormTauDist();
        virtual ~QFFitFunctionDLSG2LogNormTauDist();
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("DLS: G2 autocorrelation, log-normally distributed correlation times [num_int]"); };
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("DLS: G2, log-norm distribution of tau [num_int]"); };
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("dls_g2_int_taulognormdist"); };

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

    protected:
        gsl_integration_workspace * w;
        size_t wN;

};

#endif // QFFitFunctionDLSG2LogNormTauDist_H
