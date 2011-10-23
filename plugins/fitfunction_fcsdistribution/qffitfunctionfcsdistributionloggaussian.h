#ifndef QFFITFUNCTIONFCSDISTRIBUTIONLOGGAUSSIAN_H
#define QFFITFUNCTIONFCSDISTRIBUTIONLOGGAUSSIAN_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for fit function
    \ingroup qf3fitfunp_fitfunctions_fcsdistribution

*/
class QFFitFunctionFCSDistributionLogGaussian: public QFFitFunction {
    public:
        QFFitFunctionFCSDistributionLogGaussian();
        virtual ~QFFitFunctionFCSDistributionLogGaussian() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("long fit function name"); };
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("short name"); };
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fcs_dist_lognorm"); };

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
        virtual bool get_implementsDerivatives() { return true; };

        /*! \copydoc QFFitFunction::evaluateDerivatives()   */
        virtual void evaluateDerivatives(double* derivatives, double t, const double* parameters) const ;
};

#endif // QFFITFUNCTIONFCSDISTRIBUTIONLOGGAUSSIAN_H
