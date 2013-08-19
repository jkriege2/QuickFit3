#ifndef QFRDRCOLUMNGRAPHINTERFACE_H
#define QFRDRCOLUMNGRAPHINTERFACE_H

#include <QtPlugin>
#include <QColor>

/*! \brief interface for column oriented graphs
    \ingroup qf3rdrdp_fcs

*/
class QFRDRColumnGraphsInterface {
    public:
        virtual ~QFRDRColumnGraphsInterface() {};

        enum ColumnGraphTypes {
            cgtLines,
            cgtLinesPoints,
            cgtPoints,
            cgtBars,
            cgtSteps,
            cgtImpulses,
            cgtExpression,
            cgtPolynomial,
            cgtExponential,
            cgtPowerLaw,
            cgtQFFitFunction,
            cgtImage,
            cgtRGBImage,
            cgtMaskImage,
            cgtHorizontalRange,
            cgtVerticalRange
        };

        enum Orientation {
            cgoHorizontal,
            cgoVertical
        };

        enum ErrorGraphTypes {
            egtNone=0,
            egtBars=4,
            egtLines=5,
            egtPolygons=3,
            egtLinesBars=2,
            egtPolygonsBars=1

        };
        enum ImageModifierMode {
            cgtiModifyNone=0,
            cgtiModifyValue=1,
            cgtiModifySaturation=2,
            cgtiModifyAlpha=3
        };

        enum ImageColorPalette {
            cgtiRED=0,
            cgtiINVERTEDRED=1,
            cgtiGREEN=2,
            cgtiINVERTEDGREEN=3,
            cgtiBLUE=4,
            cgtiINVERTEDBLUE=5,
            cgtiGRAY=6,
            cgtiINVERTEDGRAY=7,
            cgtiMATLAB=8,
            cgtiRYGB=9,
            cgtiHSV=10,
            cgtiINVERTED_HSV=11,
            cgtiRAINBOW=12,
            cgtiHOT=13,
            cgtiOCEAN=14,
            cgtiTRAFFICLIGHT=15,
            cgtiBLUEMAGENTAYELLOW=16,
            cgtiBLUEYELLOW=17,
            cgtiCYAN=18
        };

        enum ImagePlotRangeChannel {
            cgtiDataChannel=0,
            cgtiRedChannel=cgtiDataChannel,
            cgtiGreenChannel=1,
            cgtiBlueChannel=2,
            cgtiModifierChannel=3
        };

        enum ColumnGraphSymbols {
            cgsNoSymbol=0,             /*!< \brief plots no symbol at all (usefull together with error bars) */
            cgsDot=1,                 /*!< \brief a small dot */
            cgsCross=2,               /*!< \brief a X cross */
            cgsPlus=3,                /*!< \brief a + cross */
            cgsCircle=4,              /*!< \brief an unfilled circle */
            cgsFilledCircle=5,        /*!< \brief a filled circle */
            cgsRect=6,                /*!< \brief an unfilled rectangle */
            cgsFilledRect=7,          /*!< \brief a filled rectangle */
            cgsTriangle=8,            /*!< \brief an unfilled triangle (tip at top) */
            cgsFilledTriangle=9,      /*!< \brief a filled triangle (tip at top) */
            cgsDownTriangle=10,        /*!< \brief an unfilled triangle (tip at bottom) */
            cgsFilledDownTriangle=11,  /*!< \brief a filled triangle (tip at bottom) */
            cgsTarget=12
        };



        /** \brief add a plot of  columnX against columnY to the given graph */
        virtual void colgraphAddPlot(int graph, int columnX, int columnY, ColumnGraphTypes type, const QString&  title)=0;
        /** \brief add a plot of  columnX against columnY to the given graph */
        virtual void colgraphAddErrorPlot(int graph, int columnX, int columnXError, int columnY, int columnYError, ColumnGraphTypes type, const QString&  title, ErrorGraphTypes errorStyle=egtBars)=0;
        /** \brief add a new graph */
        virtual void colgraphAddGraph(const QString&  title, const QString& xLabel=QString("x"), const QString& yLabel=QString("y"), bool logX=false, bool logY=false)=0;
        /** \brief returns number of plots in a graph */
        virtual int colgraphGetPlotCount(int graph) const=0;
        /** \brief returns number of graphs */
        virtual int colgraphGetGraphCount() const=0;
        /** \brief remove a graph */
        virtual void colgraphRemoveGraph(int graph) =0;
        /** \brief remove a graph */
        virtual void colgraphRemovePlot(int graph, int plot) =0;
        /** \brief set graph title */
        virtual void colgraphSetGraphTitle(int graph, const QString&  title)=0;
        /** \brief returns the graph title */
        virtual QString colgraphGetGraphTitle(int graph)=0;

        /** \brief set the line style of the graph */
        virtual void colgraphSetPlotLineStyle(int graph,  int plot,  Qt::PenStyle  style)=0;
        /** \brief set the line width of the graph */
        virtual void colgraphSetPlotLineWidth(int graph,  int plot,  double width)=0;
        /** \brief set the line style of the graph */
        virtual void colgraphSetPlotFillStyle(int graph,  int plot,  Qt::BrushStyle  style)=0;

        /** \brief set the color of the graph, the fill and error color are determined automatically */
        virtual void colgraphSetPlotColor(int graph,  int plot,  QColor  color)=0;
        /** \brief set the color of the graph, the error color is determined automatically */
        virtual void colgraphSetPlotColor(int graph,  int plot,  QColor  color, QColor  fillColor)=0;
        /** \brief set the color of the graph */
        virtual void colgraphSetPlotColor(int graph,  int plot,  QColor  color, QColor  fillColor, QColor errorColor)=0;
        /** \brief set the error color of the graph */
        virtual void colgraphSetPlotErrorColor(int graph,  int plot, QColor errorColor)=0;
        /** \brief set the fill color of the graph */
        virtual void colgraphSetPlotFillColor(int graph,  int plot, QColor fillColor)=0;
        /** \brief set the error color transparency */
        virtual void colgraphSetPlotErrorTransparency(int graph,  int plot, double errorT)=0;
        /** \brief set the fill color transparency */
        virtual void colgraphSetPlotFillTransparency(int graph,  int plot, double fillT)=0;
        /** \brief set the plot color transparency */
        virtual void colgraphSetPlotPlotTransparency(int graph,  int plot, double trans)=0;
        /** \brief set the title of the graph */
        virtual void colgraphSetPlotTitle(int graph,  int plot,  const QString& title)=0;
        /** \brief set the type of the graph */
        virtual void colgraphSetPlotType(int graph,  int plot,  ColumnGraphTypes type)=0;
        /** \brief set the error style of the graph */
        virtual void colgraphSetPlotErrorStyle(int graph,  int plot,  ErrorGraphTypes errorType)=0;
        /** \brief set the plot symbol of the graph */
        virtual void colgraphSetPlotSymbol(int graph,  int plot,  ColumnGraphSymbols symbol, double symbolSize=10.0)=0;

        /** \brief set a graph's x-axis properties */
        virtual void colgraphSetGraphXAxisProps(int graph, const QString& xLabel=QString("x"), bool logX=false)=0;
        /** \brief set a graph's y-axis properties */
        virtual void colgraphSetGraphYAxisProps(int graph, const QString& yLabel=QString("y"), bool logY=false)=0;

        /** \brief set x-axis range */
        virtual void colgraphsetXRange(int graph, double xmin, double xmax)=0;
        /** \brief set y-axis range */
        virtual void colgraphsetYRange(int graph, double xmin, double xmax)=0;
        /** \brief add a function plot which takes it's parameters from a column, or doesn't have parameters
         *
         *  \note This function only works with type==cgtExpression, cgtPolynomial, cgtExponential, cgtPowerLaw, cgtQFFitFunction. Depending on
         *        the value of type, the parameter expression either contains a math expression, like \c "p1+p2*sin(p3*x)" or the name of an available QFFitFunction
         *        For the special cases cgtPolynomial, cgtExponential, cgtPowerLaw, the expression is ignored!
         *        If any non-function type is used, the the type is automatically set to be cgtExpression.
         */
        virtual void colgraphAddFunctionPlot(int graph, const QString& expression, ColumnGraphTypes type, const QString&  title, int columnParam=-1)=0;
        /** \brief add a function plot which takes it's parameters from a vector of numbers
         *
         *  \note This function only works with typ==cgtExpression, cgtPolynomial, cgtExponential, cgtPowerLaw, cgtQFFitFunction. Depending on
         *        the value of type, the parameter expression either contains a math expression, like \c "p1+p2*sin(p3*x)" or the name of an available QFFitFunction
         *        For the special cases cgtPolynomial, cgtExponential, cgtPowerLaw, the expression is ignored!
         *        If any non-function type is used, the the type is automatically set to be cgtExpression.
         */
        virtual void colgraphAddFunctionPlot(int graph, const QString& expression, ColumnGraphTypes type, const QString&  title, const QVector<double>& params)=0;
        /** \brief add an image plot which
         */
        virtual void colgraphAddImagePlot(int graph, int imageColumn, ImageColorPalette palette, double x, double y, double width, double height, int Nx, const QString& title)=0;
        /** \brief add a mask image plot which
         */
        virtual void colgraphAddImageMaskPlot(int graph, int imageColumn, double x, double y, double width, double height, int Nx, const QString& title, QColor trueColor=QColor("black"), QColor falseColor=QColor("transparent"))=0;
        /** \brief add a range plot
         */
        virtual void colgraphAddRangePlot(int graph, Orientation orientation, double rangeStart, double rangeEnd, double rangeCenter, const QString& title, bool invertRange=false, bool fillrange=true, bool drawRangeLines=true, bool drawRangeCenter=true, QColor centerColor=QColor("red"),  Qt::PenStyle centerStyle=Qt::SolidLine, double centerWidth=1.0)=0;
        /** \brief add an RGB image plot which
         */
        virtual void colgraphAddRGBImagePlot(int graph, int imageRColumn, int imageGColumn, int imageBColumn, double x, double y, double width, double height, int Nx, const QString& title)=0;
        /** \brief set the modifier channel of the given image plot
         */
        virtual void colgraphSetImagePlotModifier(int graph, int plot, int imageModifierColumn, ImageModifierMode mode)=0;
        /** \brief set the modifier channel of the given image plot
         */
        virtual void colgraphSetImagePlotRange(int graph, int plot, ImagePlotRangeChannel channel, bool autoRange, double min=0, double max=0)=0;

};

Q_DECLARE_INTERFACE( QFRDRColumnGraphsInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRColumnGraphsInterface/1.0")

#endif // QFRDRCOLUMNGRAPHINTERFACE_H
