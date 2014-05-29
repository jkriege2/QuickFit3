#ifndef QFFitFunctionGeneralPolynomCentered_H
#define QFFitFunctionGeneralPolynomCentered_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for fit function
    \ingroup qf3fitfunp_fitfunction_general

*/
class QFFitFunctionGeneralPolynomCentered: public QFFitFunction {
    public:
        QFFitFunctionGeneralPolynomCentered();
        virtual ~QFFitFunctionGeneralPolynomCentered() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QObject::tr("General: Centered Polynom of degree 1..10"); }
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("General: Centered Polynom"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("gen_polynomcenter"); }

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

        /*! \brief if implemented (and returns \c true) this function tries to estimate the initial parameters of a fit function from provided data. */
        virtual bool estimateInitial(double* params, const double* dataX, const double* dataY, long N, const bool *fix=NULL);

};

#endif // QFFitFunctionGeneralPolynomCentered_H
