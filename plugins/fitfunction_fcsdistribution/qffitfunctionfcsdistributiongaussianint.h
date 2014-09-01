#ifndef QFFitFunctionFCSDistributionIntGaussian_H
#define QFFitFunctionFCSDistributionIntGaussian_H
#include "qfpluginfitfunction.h"
#include <QVector>
#include <QPair>
#include <stdint.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_errno.h>


/*! \brief QFFitFunction class for FCS fit with an assumed gaussian distribution of diffusion times
    \ingroup qf3fitfunp_fitfunctions_fcsdistribution

*/
class QFFitFunctionFCSDistributionIntGaussian: public QFFitFunction {
    public:
        QFFitFunctionFCSDistributionIntGaussian();
        virtual ~QFFitFunctionFCSDistributionIntGaussian();
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("FCS Dist: Normal Diffuion 3D with gaussian diffusion time distribution [num_int]"); }
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("FCS Dist: gaussian [num_int]"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fcs_dist_int_norm"); }

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


    protected:

        gsl_integration_workspace * w;
        size_t wN;
};

#endif // QFFitFunctionFCSDistributionIntGaussian_H
