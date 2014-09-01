#ifndef QFFITFUNCTIONDLSG2_H
#define QFFITFUNCTIONDLSG2_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for a tri-exponential G2 autocorrelation function in DLS
    \ingroup qf3fitfunp_fitfunctions_dls

*/
class QFFitFunctionDLSG2: public QFFitFunction {
    public:
        QFFitFunctionDLSG2();
        virtual ~QFFitFunctionDLSG2() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("DLS: G2 autocorrelation"); }
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("DLS: G2 autocorrelation"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("dls_g2"); }

        /*! \copydoc QFFitFunction::evaluate()   */
        virtual double evaluate(double t, const double* parameters) const;

        /*! \copydoc QFFitFunction::calcParameter()   */
        virtual void calcParameter(double* parameterValues, double* error=NULL) const;

        /*! \copydoc QFFitFunction::isParameterVisible()   */
        virtual bool isParameterVisible(int parameter, const double* parameterValues) const;

};

#endif // QFFITFUNCTIONDLSG2_H
