#include "qfplotterprivate.h"
#include "qfplotter.h"
#include "qfselectrdrdialog.h"
#include "qfrdrtableinterface.h"
#include "qfpluginservices.h"
#include <QDebug>
#include "qfplottercopytotabledialog.h"
#include "qfrawdatapropertyeditor.h"
#include "qffitfunctionplottools.h"
#include "qfmathparserxfunctionlinegraph.h"

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
    actHelp=new QAction(QIcon(":/lib/help.png"), tr("Help on plotter widgets"), this);
    connect(actHelp, SIGNAL(triggered()), this, SLOT(showHelp()));
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
        QFRawDataRecord* rdr=dlg->getRDR();
        if (dlg->getNewTable(tabname)) {
            if (tabname.isEmpty()) tabname=tr("NEW_TABLE");
            rdr=QFPluginServices::getInstance()->getCurrentProject()->addRawData("table", tabname, "");
            tab=dynamic_cast<QFRDRTableInterface*>(rdr);
            cols=dynamic_cast<QFRDRColumnGraphsInterface*>(rdr);
        }
        bool ok=true;
        QMap<int, int> columns;
        if (tab) {
            JKQTPdatastore* ds=plotter->getDatastore();
            JKQtBasePlotter* p=plotter->get_plotter();
            int idx=tab->tableGetColumnCount();
            QList<size_t> cids=ds->getColumnIDs();
            for (int i=0; i<cids.size(); i++) {
                bool copyCol=false;
                if (!dlg->copyOnlyPlotData()) copyCol=true;
                else {
                    for (int j=0; j<p->getGraphCount(); j++) {
                        JKQTPgraph* g=p->getGraph(j);
                        if (g->usesColumn(cids[i])) {
                            copyCol=true;
                            break;
                        }
                    }
                }

                if(copyCol) {
                    QList<QVariant> d;
                    JKQTPcolumn col=ds->getColumn(cids[i]);
                    for (int j=0; j<col.getRows(); j++) {
                        d<<col.getValue(j);
                    }
                    tab->tableSetColumnData(idx, d);
                    QString tit=dlg->getPrefix();
                    if (tit.size()>0) tit=tit+QString(" ");
                    tit=tit+col.get_name();
                    tab->tableSetColumnTitle(idx, tit);
                    //qDebug()<<"set col title idx="<<idx<<"   name="<<col.get_name();
                    columns[cids[i]]=idx;
                    idx++;
                }
            }
        } else {
            QMessageBox::critical(plotter, tr("Add data to table"), tr("No table selected or could not create table!\nCould not add data!"));
            ok=false;
        }
        int graph=-1;
        if (ok && dlg->getAddGraphs()) {
            if (cols) {
                graph=dlg->getGraphID();
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
                        QFMathParserXFunctionLineGraph* qfmpg=dynamic_cast<QFMathParserXFunctionLineGraph*>(g);
                        JKQTPxQFFitFunctionLineGraph* qfffg=dynamic_cast<JKQTPxQFFitFunctionLineGraph*>(g);
                        JKQTPColumnMathImage* colig=dynamic_cast<JKQTPColumnMathImage*>(g);
                        JKQTPMathImage* ig=dynamic_cast<JKQTPMathImage*>(g);
                        QFRDRColumnGraphsInterface::ColumnGraphTypes type=QFRDRColumnGraphsInterface::cgtLinesPoints;
                        QFRDRColumnGraphsInterface::ColumnGraphSymbols symbol=QFRDRColumnGraphsInterface::cgsFilledCircle;
                        double symbolSize=15.0;

                        if (xyg) {
                            QColor color=QColor("red");
                            QColor fillColor=color.lighter();
                            QColor errorColor=color.darker();
                            if (xylg) {
                                color=xylg->get_color();
                                fillColor=xylg->get_fillColor();
                                symbol=(QFRDRColumnGraphsInterface::ColumnGraphSymbols)xylg->get_symbol();
                                symbolSize=xylg->get_symbolSize();
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
                                cols->colgraphAddErrorPlot(graph, xcol, -1,  ycol, columns.value(yeg->get_yErrorColumn(), -1), type, name, (QFRDRColumnGraphsInterface::ErrorGraphTypes)yeg->get_errorStyle());
                                errorColor=yeg->get_errorColor();
                                //qDebug()<<"addyerrorg "<<yeg->get_yErrorColumn()<<columns.value(yeg->get_yErrorColumn(), -1);
                            } else if (xeg) {
                                cols->colgraphAddErrorPlot(graph, xcol, columns.value(xeg->get_xErrorColumn(), -1),  ycol, -1, type, name, (QFRDRColumnGraphsInterface::ErrorGraphTypes)xeg->get_errorStyle());
                                errorColor=xeg->get_errorColor();
                                //qDebug()<<"addxerrorg "<<xeg->get_xErrorColumn()<<columns.value(xeg->get_xErrorColumn(), -1);
                            } else if (xyeg) {
                                cols->colgraphAddErrorPlot(graph, xcol, columns.value(xyeg->get_xErrorColumn(), -1),  ycol, columns.value(xyeg->get_yErrorColumn(), -1), type, name, (QFRDRColumnGraphsInterface::ErrorGraphTypes)xyeg->get_yErrorStyle());
                                errorColor=xyeg->get_errorColor();
                                //qDebug()<<"addxyerrorg "<<xyeg->get_xErrorColumn()<<columns.value(xyeg->get_xErrorColumn(), -1)<<xyeg->get_yErrorColumn()<<columns.value(xyeg->get_yErrorColumn(), -1);
                            } else {
                                cols->colgraphAddPlot(graph, xcol, ycol, type, name);
                            }
                            //qDebug()<<"color="<<color<<"   fillColor="<<fillColor;
                            if (color!=QColor("red")) {
                                cols->colgraphSetPlotColor(graph, cols->colgraphGetPlotCount(graph)-1, color);
                            }
                            if (fillColor!=QColor("red").lighter()) {
                                cols->colgraphSetPlotFillColor(graph, cols->colgraphGetPlotCount(graph)-1, fillColor);
                            }
                            if (errorColor!=QColor("red").darker()) {
                                cols->colgraphSetPlotErrorColor(graph, cols->colgraphGetPlotCount(graph)-1, errorColor);
                            }
                            cols->colgraphSetPlotSymbol(graph, cols->colgraphGetPlotCount(graph)-1, symbol, symbolSize);

                        } else if (colig) {
                        } else if(qfffg && qfffg->get_fitFunction()) {
                            QColor color=QColor("red");
                            QColor fillColor=color.lighter();
                            QColor errorColor=color.darker();
                            type=QFRDRColumnGraphsInterface::cgtQFFitFunction;
                            color=qfffg->get_color();
                            fillColor=qfffg->get_fillColor();


                            int pcol=columns.value(qfffg->get_parameterColumn(), -1);

                            if (pcol>=0) cols->colgraphAddFunctionPlot(graph, qfffg->get_fitFunction()->id(), type, qfffg->get_title(), pcol );
                            else cols->colgraphAddFunctionPlot(graph, qfffg->get_fitFunction()->id(), type, qfffg->get_title(), qfffg->get_internalParams() );

                            if (color!=QColor("red")) {
                                cols->colgraphSetPlotColor(graph, cols->colgraphGetPlotCount(graph)-1, color);
                            }
                            if (fillColor!=QColor("red").lighter()) {
                                cols->colgraphSetPlotFillColor(graph, cols->colgraphGetPlotCount(graph)-1, fillColor);
                            }
                            if (errorColor!=QColor("red").darker()) {
                                cols->colgraphSetPlotErrorColor(graph, cols->colgraphGetPlotCount(graph)-1, errorColor);
                            }
                        } else if(qfmpg) {
                            QColor color=QColor("red");
                            QColor fillColor=color.lighter();
                            QColor errorColor=color.darker();
                            type=QFRDRColumnGraphsInterface::cgtExpression;
                            color=qfmpg->get_color();
                            fillColor=qfmpg->get_fillColor();


                            int pcol=columns.value(qfmpg->get_parameterColumn(), -1);

                            if (pcol>=0) cols->colgraphAddFunctionPlot(graph, qfmpg->get_function(), type, qfmpg->get_title(), pcol );
                            else cols->colgraphAddFunctionPlot(graph, qfmpg->get_function(), type, qfmpg->get_title(), qfmpg->get_internalParams() );

                            if (color!=QColor("red")) {
                                cols->colgraphSetPlotColor(graph, cols->colgraphGetPlotCount(graph)-1, color);
                            }
                            if (fillColor!=QColor("red").lighter()) {
                                cols->colgraphSetPlotFillColor(graph, cols->colgraphGetPlotCount(graph)-1, fillColor);
                            }
                            if (errorColor!=QColor("red").darker()) {
                                cols->colgraphSetPlotErrorColor(graph, cols->colgraphGetPlotCount(graph)-1, errorColor);
                            }

                        }
                    }
                }
            } else {
                QMessageBox::critical(plotter, tr("Add graphs to table"), tr("Could not add plots, as this is not supported by the selected table record!"));
                ok=false;
            }
        }
        if (ok && rdr && dlg->getShowEditor()) {
            QFRawDataPropertyEditor* editor=QFPluginServices::getInstance()->openRawDataEditor(rdr, false);
            if (editor) {
                if (graph>=0) editor->sendEditorCommand("showPlot", graph);
                editor->showTab(2);
            }
        }
    }
    delete dlg;
}

void QFPlotterPrivate::showHelp()
{
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getMainHelpDirectory()+QString("/jkqtplotter.html"));

}
