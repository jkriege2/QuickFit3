#ifndef QFFitFunctionLightsheetGaussianFWHM_H
#define QFFitFunctionLightsheetGaussianFWHM_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for fit function
    \ingroup qf3fitfunp_fitfunctions_lightsheet

*/
class QFFitFunctionLightsheetGaussianFWHM: public QFFitFunction {
    public:
        QFFitFunctionLightsheetGaussianFWHM();
        virtual ~QFFitFunctionLightsheetGaussianFWHM() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("SPIM lightsheet: gaussian, FWHM"); };
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("SPIM lightsheet: gaussian (FWHM)"); };
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("lightsheet_gaussian_fwhm"); };
        /*! \copydoc QFFitFunction::category()   */
        virtual QString category() const { return QObject::tr("SPIM lightsheet"); }

        /*! \copydoc QFFitFunction::evaluate()   */
        virtual double evaluate(double t, const double* parameters) const;

        /*! \copydoc QFFitFunction::calcParameter()   */
        //virtual void calcParameter(double* parameterValues, double* error=NULL) const;

        /*! \copydoc QFFitFunction::isParameterVisible()   */
        virtual bool isParameterVisible(int parameter, const double* parameterValues) const;
        /*! \copydoc QFFitFunction::getAdditionalPlotCount()   */
        virtual unsigned int getAdditionalPlotCount(const double* params) const;

        /*! \copydoc QFFitFunction::transformParametersForAdditionalPlot()   */
        virtual QString transformParametersForAdditionalPlot(int plot, double* params) const;

        /*! \copydoc QFFitFunction::get_implementsDerivatives()   */
        virtual bool get_implementsDerivatives() const { return false; };
        /*! \copydoc QFFitFunction::estimateInitial()   */
        virtual bool estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool *fix) const;
};

#endif // QFFitFunctionLightsheetGaussianFWHM_H
