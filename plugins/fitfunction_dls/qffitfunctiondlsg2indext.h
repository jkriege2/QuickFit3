#ifndef QFFitFunctionDLSG2inDINDEXT_H
#define QFFitFunctionDLSG2inDINDEXT_H
#include "qfpluginfitfunction.h"



/*! \brief QFFitFunction class for a multi exponential field autocorrelation function in DLS
    \ingroup qf3fitfunp_fitfunctions_dls
*/
class QFFitFunctionDLSG2inDExt: public QFFitFunction {
    public:
        QFFitFunctionDLSG2inDExt();
        virtual ~QFFitFunctionDLSG2inDExt() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("DLS: G2 autocorrelation (D) Extended"); };
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("DLS: G2 autocorrelation (D) Extended"); };
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("dls_g2_ind_ext"); };

        /*! \copydoc QFFitFunction::evaluate()   */
        virtual double evaluate(double t, const double* parameters) const;

        /*! \copydoc QFFitFunction::calcParameter()   */
        virtual void calcParameter(double* parameterValues, double* error=NULL) const;

        /*! \copydoc QFFitFunction::isParameterVisible()   */
        virtual bool isParameterVisible(int parameter, const double* parameterValues) const;

};

#endif // QFFitFunctionDLSG2inDINDEXT_H
