#ifndef QFFitFunctionGeneralLorentzian_H
#define QFFitFunctionGeneralLorentzian_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for fit function
    \ingroup qf3fitfunp_fitfunctions_general

*/
class QFFitFunctionGeneralLorentzian: public QFFitFunction {
    public:
        QFFitFunctionGeneralLorentzian();
        virtual ~QFFitFunctionGeneralLorentzian() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QObject::tr("Peaks: Lorentzian function"); };
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return name(); };
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("gen_lorentzian"); };

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
        virtual bool get_implementsDerivatives();

        /*! \copydoc QFFitFunction::evaluateDerivatives()   */
        virtual void evaluateDerivatives(double* derivatives, double t, const double* parameters) const ;
};

#endif // QFFitFunctionGeneralLorentzian_H
