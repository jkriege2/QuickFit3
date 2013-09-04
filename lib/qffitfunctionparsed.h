#ifndef QFFITFUNCTIONPARSED_H
#define QFFITFUNCTIONPARSED_H

#include "qffitfunction.h"
#include "qfmathparser.h"
#include "lib_imexport.h"

class QFLIB_EXPORT QFFitFunctionParsed : public QFFitFunction{
    public:
        explicit QFFitFunctionParsed(const QString& configfile);
        virtual ~QFFitFunctionParsed();

        bool isValid() const;
        QString getErrors() const;
        bool usesBytecode() const;



        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const;
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const;
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const;

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


    protected:
        bool m_valid;
        QString m_errors;
        QString m_id;
        QString m_name;
        QString m_shortName;
        QString m_expression;
        mutable QFMathParser parser;
        QFMathParser::ByteCodeProgram bprog;
        QFMathParser::ByteCodeEnvironment bcenv;
        QFMathParser::qfmpNode* pnode;
        bool useBytecode;
        mutable QVector<double> params;
        mutable double x;

    
};

#endif // QFFITFUNCTIONPARSED_H
