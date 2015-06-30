#ifndef QFFitFunctionPSFGaussian_H
#define QFFitFunctionPSFGaussian_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for fit function: PSF: gaussian, 1/e^2-halfwidth
    \ingroup qf3rdrdp_eval_beadscanpsf

*/
class QFFitFunctionPSFGaussian: public QFFitFunction {
    public:
        QFFitFunctionPSFGaussian();
        virtual ~QFFitFunctionPSFGaussian() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("PSF: gaussian, 1/e^2-halfwidth"); };
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("PSF: gaussian (1/e^2)"); };
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("beadscan_psf_gaussian"); };
        /*! \copydoc QFFitFunction::category()   */
        virtual QString category() const { return QString("Point Spread Function (PSF)"); };

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

        /*! \copydoc QFFitFunction::estimateInitial()   */
        virtual bool estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool *fix);
};

#endif // QFFitFunctionPSFGaussian_H
