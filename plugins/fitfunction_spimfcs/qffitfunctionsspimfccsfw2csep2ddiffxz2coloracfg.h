#ifndef QFFitFunctionsSPIMFCCSFW2CSep2DDiffXZ2ColorACFG_H
#define QFFitFunctionsSPIMFCCSFW2CSep2DDiffXZ2ColorACFG_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for a SPIM-FCCS fit model with pure diffusion for 2-color crosscorrelation and 1/sqrt(e) lateral width
    \ingroup qf3fitfunp_fitfunctions_spimfcs
*/
class QFFitFunctionsSPIMFCCSFW2CSep2DDiffXZ2ColorACFG: public QFFitFunction {
    public:
        QFFitFunctionsSPIMFCCSFW2CSep2DDiffXZ2ColorACFG();
        virtual ~QFFitFunctionsSPIMFCCSFW2CSep2DDiffXZ2ColorACFG() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("SPIM-FCCS: 2-comp. 2D diffusion (xz), species A+B+AB, c per species, D1/D2 per channel, ACF green (1/e² radii)"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fccs_spim_fw_2csep2ddiffxz2coloracfg"); }

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

#endif // QFFitFunctionsSPIMFCCSFW2CSep2DDiffXZ2ColorACFG_H
