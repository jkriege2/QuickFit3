#include "qfplotterprivate.h"
#include "qfplotter.h"
#include "qfselectrdrdialog.h"
#include "qfrdrtableinterface.h"
#include "qfpluginservices.h"
#include <QDebug>
#include "qfplottercopytotabledialog.h"

class MyQFSelectRDRDialogMatchFunctor: public QFMatchRDRFunctor {
    public:
        explicit MyQFSelectRDRDialogMatchFunctor(): QFMatchRDRFunctor() {
            //qDebug()<<"MyQFSelectRDRDialogMatchFunctor:constructor ";
        }

        virtual bool matches(const QFRawDataRecord* record) const {
            QFRDRTableInterface* tab=qobject_cast<QFRDRTableInterface*>(record);
            if (tab) return !tab->tableIsReadonly();
            return false;
        }
};

QFPlotterPrivate::QFPlotterPrivate(QFPlotter *plotter, QObject *parent) :
    QObject(parent)
{
    this->plotter=plotter;
    actCopyToTable=new QAction(QIcon(":/libqf3widgets/qfplotter_copytotable.png"), tr("copy plot data to table RDR"), this);
    connect(actCopyToTable, SIGNAL(triggered()), this, SLOT(copyToTable()));
}


void QFPlotterPrivate::copyToTable()
{
    if (!QFPluginServices::getInstance()->getCurrentProject()) return;

    QFPlotterCopyToTableDialog* dlg=new QFPlotterCopyToTableDialog(plotter);
    //dlg->setDescription(tr("Select a table record into which to write the plot data."));
    if (dlg->exec()) {
        QString tabname="";
        QFRDRTableInterface* tab=dlg->getTable();
        QFRDRColumnGraphsInterface* cols=dlg->getGraph();
        if (dlg->getNewTable(tabname)) {
            if (tabname.isEmpty()) tabname=tr("NEW_TABLE");
            QFRawDataRecord* rdr=QFPluginServices::getInstance()->getCurrentProject()->addRawData("table", tabname, "");
            tab=dynamic_cast<QFRDRTableInterface*>(rdr);
            cols=dynamic_cast<QFRDRColumnGraphsInterface*>(rdr);
        }
        bool ok=true;
        QList<int> columns;
        if (tab) {
            JKQTPdatastore* ds=plotter->getDatastore();
            int idx=tab->tableGetColumnCount();
            for (int i=0; i<ds->getColumnCount(); i++) {
                QList<QVariant> d;
                JKQTPcolumn col=ds->getColumn(i);
                for (int j=0; j<col.getRows(); j++) {
                    d<<col.getValue(j);
                }
                tab->tableSetColumnData(idx, d);
                tab->tableSetColumnTitle(idx, col.get_name());
                columns<<idx;
                idx++;
            }
        } else {
            QMessageBox::critical(plotter, tr("Add data to table"), tr("No table selected or could not create table!\nCould not add data!"));
            ok=false;
        }
        if (ok && dlg->getAddGraphs()) {
            if (cols) {
                int graph=dlg->getGraphID();
                QString graphtitle;
                JKQtBasePlotter* p=plotter->get_plotter();
                if (dlg->getNewGraph(graphtitle)) {
                    if (graphtitle.isEmpty()) {
                        graphtitle=tr("new graph");
                        if (!p->get_plotLabel().isEmpty()) graphtitle=p->get_plotLabel().isEmpty();
                    }
                    graph=cols->colgraphGetGraphCount();
                    cols->colgraphAddGraph(graphtitle, p->get_xAxis()->get_axisLabel(), p->get_yAxis()->get_axisLabel(), p->get_xAxis()->get_logAxis(), p->get_yAxis()->get_logAxis());
                }
                cols->colgraphsetXRange(graph, p->getXMin(), p->getXMax());
                cols->colgraphsetYRange(graph, p->getYMin(), p->getYMax());
                for (int j=0; j<p->getGraphCount(); j++) {
                    JKQTPgraph* g=p->getGraph(j);
                    if (g && g->get_visible()) {
                        QString name=g->get_title();
                        JKQTPxyGraph* xyg=dynamic_cast<JKQTPxyGraph*>(g);
                        JKQTPxyLineGraph* xylg=dynamic_cast<JKQTPxyLineGraph*>(g);
                        JKQTPxGraphErrors* xeg=dynamic_cast<JKQTPxGraphErrors*>(g);
                        JKQTPyGraphErrors* yeg=dynamic_cast<JKQTPyGraphErrors*>(g);
                        JKQTPxyGraphErrors* xyeg=dynamic_cast<JKQTPxyGraphErrors*>(g);
                        JKQTPColumnMathImage* colig=dynamic_cast<JKQTPColumnMathImage*>(g);
                        JKQTPMathImage* ig=dynamic_cast<JKQTPMathImage*>(g);
                        QFRDRColumnGraphsInterface::ColumnGraphTypes type=QFRDRColumnGraphsInterface::cgtLinesPoints;

                        if (xyg) {
                            QColor color=QColor("red");
                            QColor fillColor=color.lighter();
                            if (xylg) {
                                color=xylg->get_color();
                                fillColor=xylg->get_fillColor();
                                if (xylg->get_drawLine() && xylg->get_symbol()!=JKQTPnoSymbol) type=QFRDRColumnGraphsInterface::cgtLinesPoints;
                                else if (xylg->get_drawLine() && xylg->get_symbol()==JKQTPnoSymbol) type=QFRDRColumnGraphsInterface::cgtLines;
                                else if (!xylg->get_drawLine() && xylg->get_symbol()!=JKQTPnoSymbol) type=QFRDRColumnGraphsInterface::cgtPoints;
                            }
                            if (dynamic_cast<JKQTPimpulsesHorizontalGraph*>(g) || dynamic_cast<JKQTPimpulsesVerticalGraph*>(g)) type=QFRDRColumnGraphsInterface::cgtImpulses;
                            if (dynamic_cast<JKQTPfilledCurveXGraph*>(g) || dynamic_cast<JKQTPfilledCurveYGraph*>(g)) ;
                            if (dynamic_cast<JKQTPstepHorizontalGraph*>(g) || dynamic_cast<JKQTPstepVerticalGraph*>(g)) type=QFRDRColumnGraphsInterface::cgtSteps;
                            if (dynamic_cast<JKQTPbarHorizontalGraph*>(g) || dynamic_cast<JKQTPbarVerticalGraph*>(g)) type=QFRDRColumnGraphsInterface::cgtBars;

                            int xcol=columns.value(xyg->get_xColumn(), -1);
                            int ycol=columns.value(xyg->get_yColumn(), -1);



                            if (yeg) {
                                cols->colgraphAddErrorPlot(graph, xcol, -1,  ycol, columns.value(yeg->get_yErrorColumn(), -1), type, name);
                                qDebug()<<"addyerrorg "<<yeg->get_yErrorColumn()<<columns.value(yeg->get_yErrorColumn(), -1);
                            } else if (xeg) {
                                cols->colgraphAddErrorPlot(graph, xcol, columns.value(xeg->get_xErrorColumn(), -1),  ycol, -1, type, name);
                                qDebug()<<"addxerrorg "<<xeg->get_xErrorColumn()<<columns.value(xeg->get_xErrorColumn(), -1);
                            } else if (xyeg) {
                                cols->colgraphAddErrorPlot(graph, xcol, columns.value(xyeg->get_xErrorColumn(), -1),  ycol, columns.value(xyeg->get_yErrorColumn(), -1), type, name);
                                qDebug()<<"addxyerrorg "<<xyeg->get_xErrorColumn()<<columns.value(xyeg->get_xErrorColumn(), -1)<<xyeg->get_yErrorColumn()<<columns.value(xyeg->get_yErrorColumn(), -1);
                            } else {
                                cols->colgraphAddPlot(graph, xcol, ycol, type, name);
                            }
                            if (color!=QColor("red")) {
                                if (fillColor!=QColor("red").lighter()) {
                                    cols->colgraphSetPlotColor(graph, cols->colgraphGetPlotCount(graph)-1, color, fillColor);
                                } else {
                                    cols->colgraphSetPlotColor(graph, cols->colgraphGetPlotCount(graph)-1, color);
                                }
                            } else if (fillColor!=QColor("red").lighter()) {
                                cols->colgraphSetPlotColor(graph, cols->colgraphGetPlotCount(graph)-1, color, fillColor);
                            }

                        } else if (colig) {
                        }
                    }
                }
            } else {
                QMessageBox::critical(plotter, tr("Add graphs to table"), tr("Could not add plots, as this is not supported by the selected table record!"));
                ok=false;
            }
        }
    }
    delete dlg;
}
