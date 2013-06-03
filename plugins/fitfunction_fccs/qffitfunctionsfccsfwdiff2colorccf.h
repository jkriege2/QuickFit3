#ifndef QFFitFunctionsFCCSFWDiff2ColorCCF_H
#define QFFitFunctionsFCCSFWDiff2ColorCCF_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for a SPIM-FCCS fit model with pure diffusion for 2-color crosscorrelation and 1/sqrt(e) lateral width
    \ingroup qf3fitfunp_fitfunctions_spimfcs
*/
class QFFitFunctionsFCCSFWDiff2ColorCCF: public QFFitFunction {
    public:
        QFFitFunctionsFCCSFWDiff2ColorCCF();
        virtual ~QFFitFunctionsFCCSFWDiff2ColorCCF() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("FCCS: Diffusion, 2-color, CCF (1/e² radii)"); }
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("FCCS: Diffusion, 2-color, CCF (1/e² radii)"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fccs_fw_diff2colorccf"); }

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
};

#endif // QFFitFunctionsFCCSFWDiff2ColorCCF_H
