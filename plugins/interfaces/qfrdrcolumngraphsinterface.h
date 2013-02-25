#ifndef QFRDRCOLUMNGRAPHINTERFACE_H
#define QFRDRCOLUMNGRAPHINTERFACE_H

#include <QtPlugin>

/*! \brief interface for column oriented graphs
    \ingroup qf3rdrdp_fcs

*/
class QFRDRColumnGraphsInterface {
    public:
        virtual ~QFRDRColumnGraphsInterface() {};

        enum ColumnGraphTypes {
            cgtLines,
            cgtLinesPoints,
            cgtPoints
        };

        /** \brief add a plot of  columnX against columnY to the given graph */
        virtual void colgraphAddPlot(int graph, int columnX, int columnY, ColumnGraphTypes type, const QString&  title)=0;
        /** \brief add a plot of  columnX against columnY to the given graph */
        virtual void colgraphAddErrorPlot(int graph, int columnX, int columnXError, int columnY, int columnYError, ColumnGraphTypes type, const QString&  title)=0;
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
        /** \brief set graph properties */
        virtual void colgraphSetGraphProps(int graph, const QString&  title)=0;
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
