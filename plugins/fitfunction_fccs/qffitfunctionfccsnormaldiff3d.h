#ifndef QFFITFUNCTIONFCCSNORMALDIFF3D_H
#define QFFITFUNCTIONFCCSNORMALDIFF3D_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for fit function
    \ingroup qf3fitfunp_fitfunctions_fccs

*/
class QFFitFunctionFCCSNormalDiff3D: public QFFitFunction {
    public:
        QFFitFunctionFCCSNormalDiff3D();
        virtual ~QFFitFunctionFCCSNormalDiff3D() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("FCCS: 3D Normal Diffusion"); }
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("FCCS: 3D Normal Diffusion"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fccs_diff3d"); }

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

#endif // QFFITFUNCTIONFCCSNORMALDIFF3D_H
