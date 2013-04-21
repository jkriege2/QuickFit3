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
            cgtImpulses
        };

        enum ErrorGraphTypes {
            egtNone=0,
            egtBars=4,
            egtLines=5,
            egtPolygons=3,
            egtLinesBars=2,
            egtPolygonsBars=1

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

};

Q_DECLARE_INTERFACE( QFRDRColumnGraphsInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRColumnGraphsInterface/1.0")

#endif // QFRDRCOLUMNGRAPHINTERFACE_H
