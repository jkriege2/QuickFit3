#ifndef QFSHAREDFITFUNCTION_H
#define QFSHAREDFITFUNCTION_H

#include "qffitfunction.h"
#include <QString>
#include <QList>
#include <QStringList>
#include <cfloat>
#include <cmath>
#include "qfmathtools.h"
#include "lib_imexport.h"
#include <QLibrary>
#include "./sdk_fitfunctions/quickfit-model-tools.h"

class QFLIB_EXPORT QFSharedFitFunction : public QFFitFunction
{
    public:
        explicit QFSharedFitFunction(QLibrary* library);

        QString helpFile() const ;

        bool isValid() const;

        /*! \copydoc QFFitFunction::()  */
        virtual QString name() const;
        /*! \copydoc QFFitFunction::()  */
        virtual QString shortName() const ;
        /*! \copydoc QFFitFunction::()  */
        virtual QString id() const;

        /*! \copydoc QFFitFunction::()  */
        virtual double evaluate(double x, const double* parameters) const ;

        /*! \copydoc QFFitFunction::()  */
        virtual void multiEvaluate(double* y, const double* x, uint64_t N, const double* parameters) const;

        /*! \copydoc QFFitFunction::()  */
        virtual void evaluateDerivatives(double* derivatives, double x, const double* parameters) const ;

        /*! \copydoc QFFitFunction::()  */
        virtual bool get_implementsDerivatives();

        /*! \copydoc QFFitFunction::()  */
        virtual void calcParameter(double* parameterValues, double* error=NULL) const ;

        /*! \copydoc QFFitFunction::()  */
        virtual void sortParameter(double* parameterValues, double* error=NULL, bool* fix=NULL) const ;

        /*! \copydoc QFFitFunction::()  */
        virtual bool isParameterVisible(int parameter, const double* parameterValues) const ;

        /*! \copydoc QFFitFunction::()  */
        virtual unsigned int getAdditionalPlotCount(const double* params);

        /*! \copydoc QFFitFunction::()  */
        virtual QString transformParametersForAdditionalPlot(int plot, double* params) ;

        /*! \copydoc QFFitFunction::()  */
        virtual bool estimateInitial(double* params, const double* dataX, const double* dataY, long N, const bool *fix=NULL);


    protected:
        QLibrary* library;
        QString m_id;
        QString m_name;
        QString m_shortName;
        QString m_help;
        bool libOK;

        QF3SimpleFFGetNameFunc lib_getName;
        QF3SimpleFFMultiEvaluateFunc lib_evalMulti;
        QF3SimpleFFGetParameterDescriptionFunc lib_getParams;
        QF3SimpleFFGetParameterCountFunc lib_getParamCount;
        QF3SimpleFFEvaluateFunc lib_eval;
};

#endif // QFSHAREDFITFUNCTION_H
