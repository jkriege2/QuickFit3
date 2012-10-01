#ifndef QFFitFunctionDLSG2inDIND_H
#define QFFitFunctionDLSG2inDIND_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for a tri-exponential G2 autocorrelation function in DLS
    \ingroup qf3fitfunp_fitfunctions_dls

*/
class QFFitFunctionDLSG2inD: public QFFitFunction {
    public:
        QFFitFunctionDLSG2inD();
        virtual ~QFFitFunctionDLSG2inD() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("DLS: G2 autocorrelation (D)"); };
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("DLS: G2 autocorrelation (D)"); };
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("dls_g2_ind"); };

        /*! \copydoc QFFitFunction::evaluate()   */
        virtual double evaluate(double t, const double* parameters) const;

        /*! \copydoc QFFitFunction::calcParameter()   */
        virtual void calcParameter(double* parameterValues, double* error=NULL) const;

        /*! \copydoc QFFitFunction::isParameterVisible()   */
        virtual bool isParameterVisible(int parameter, const double* parameterValues) const;

};

#endif // QFFitFunctionDLSG2inDIND_H
