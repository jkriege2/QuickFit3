#ifndef QFFitFunctionsTIRFCSDiffE2DiffbrightNewVeffE2_H
#define QFFitFunctionsTIRFCSDiffE2DiffbrightNewVeffE2_H
#include "qfpluginfitfunction.h"



/*! \brief QFFitFunction class for a TIR-FCS fit model with pure diffusion and lateral 1/e^2 width
    \ingroup qf3fitfunp_fitfunctions_tirfcs

*/
class QFFitFunctionsTIRFCSDiffE2Diffbright: public QFFitFunction {
    public:
        QFFitFunctionsTIRFCSDiffE2Diffbright();
        virtual ~QFFitFunctionsTIRFCSDiffE2Diffbright() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("TIR-FCS: 2D Diffusion, different molecular brightnesses (1/e^2 radii)"); }
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("TIR-FCS: 2D Diffusion, different molecular brightnesses (1/e^2 radii)"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fcs_tir_diffe2_diffbright"); }

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

};

#endif // QFFitFunctionsTIRFCSDiffE2DiffbrightNewVeffE2_H
