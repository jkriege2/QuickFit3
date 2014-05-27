#ifndef QFFitFunctionGeneralModHill5P_H
#define QFFitFunctionGeneralModHill5P_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for fit function
    \ingroup qf3fitfunp_fitfunction_general

*/
class QFFitFunctionGeneralModHill5P: public QFFitFunction {
    public:
        QFFitFunctionGeneralModHill5P();
        virtual ~QFFitFunctionGeneralModHill5P() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QObject::tr("Sigmoidals: modified Hill function (5 parameters)"); }
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return name(); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("gen_modhill5p"); }

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

#endif // QFFitFunctionGeneralModHill5P_H
