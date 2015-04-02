/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
        /*! \copydoc QFFitFunction::category()   */
        virtual QString category() const;
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
        virtual bool get_implementsDerivatives() const;


    protected:
        bool m_valid;
        QString m_errors;
        QString m_id;
        QString m_name;
        QString m_category;
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



class QFLIB_EXPORT QFFitFunctionQFMathparserNode : public QFFitFunction{
    public:
        explicit QFFitFunctionQFMathparserNode(QFMathParser* parser, QFMathParser::qfmpNode* pnode, const QStringList& parameterNames, const QVector<double> &initVals=QVector<double>());
        virtual ~QFFitFunctionQFMathparserNode();


        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const;
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const;
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const;

        /*! \copydoc QFFitFunction::evaluate()   */
        virtual double evaluate(double t, const double* parameters) const;


    protected:
        QFMathParser* parser;
        QFMathParser::qfmpNode* pnode;
        QStringList parameterNames;



};

#endif // QFFITFUNCTIONPARSED_H
