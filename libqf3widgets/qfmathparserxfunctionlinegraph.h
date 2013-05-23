#ifndef QFMathParserXFunctionLineGraph_H
#define QFMathParserXFunctionLineGraph_H
#include <QString>
#include <QPainter>
#include <QPair>
#include "jkqtptools.h"
#include "tools.h"
#include "libwid_imexport.h"
#include "jkqtpelements.h"
#include "qfmathparser.h"

// forward declarations
class JKQtBasePlotter;


struct QFMathParserXFunctionLineGraphFunctionData {
    QFMathParser* parser;
    QFMathParser::qfmpNode* node;
    int varcount;
    double* x;
};

/*! \brief This implements line plots where the data is taken from a user supplied function \f$ y=f(x) \f$ The function is defined as a string and parsed by QFMathParser
    \ingroup jkqtplotter_plots

    Additional function parameters may be given in the vector parameters. They are accessible in the function as \c p1 , \c p2 , \c p3 , ...
    Parameters may also be given from a data column. Then first the params from the column and the the parameters from the vector are numbered.
 */
class QFWIDLIB_EXPORT QFMathParserXFunctionLineGraph : public JKQTPxFunctionLineGraph
{
        Q_OBJECT
    public:
        explicit QFMathParserXFunctionLineGraph(JKQtBasePlotter *parent = 0);
        

        /** \brief class destructor */
        virtual ~QFMathParserXFunctionLineGraph();

        GetSetMacro(QString, function);

        GetSetMacro(QString, errorFunction);
    protected:


        /** \brief which plot style to use from the parent plotter (via JKQtPlotterBase::getPlotStyle() and JKQtPlotterBase::getNextStyle() ) */
        QString function;
        QFMathParserXFunctionLineGraphFunctionData fdata;

        QString errorFunction;
        QFMathParserXFunctionLineGraphFunctionData efdata;

        GetSetMacroI(jkqtpPlotFunctionType, plotFunction, clearData());
        GetSetMacroI(void*, params, clearData());
        GetSetMacro(jkqtpPlotFunctionType, errorPlotFunction);
        GetSetMacro(void*, errorParams);

        /** \brief fill the data array with data from the function plotFunction */
        virtual void createPlotData(bool collectParams=true);

        double dummyX, dummyEX;

};

#endif // QFMathParserXFunctionLineGraph_H
