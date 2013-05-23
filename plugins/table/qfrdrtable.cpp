#include "qfrdrtable.h"
#include "dlgcsvparameters.h"
#include <QtXml>
#include "qfrdrtableeditor.h"
#include "qfrdrploteditor.h"
#include "qftools.h"
#include "qfrdrtableparserfunctions.h"
#define DEFAULT_EDITVAL double(0.0)

QFRDRTable::GraphInfo::GraphInfo() {
    title="";
    isStrided=false;
    stride=1;
    strideStart=1;
    type=QFRDRTable::gtLines;
    xcolumn=-1;
    ycolumn=-1;
    xerrorcolumn=-1;
    yerrorcolumn=-1;
    style=Qt::SolidLine;
    color=QColor("red");
    errorColor=color.darker();
    linewidth=1;
    symbol=JKQTPfilledCircle;
    symbolSize=15;
    errorStyle=JKQTPnoError;
    drawLine=true;
    errorStyle=JKQTPerrorBars;
    colorTransparent=1;
    errorColorTransparent=1;
    fillColorTransparent=0.3;
    fillColor=color.lighter();
    imageTrueColor=QColor("blue");
    imageTrueTransparent=0.5;
    imageFalseColor=QColor("red");
    imageFalseTransparent=1.0;
    imagePixelWidth=1;
    imageX=0;
    imageY=0;
    imageWidth=1;
    imageHeight=1;
    imagePalette=JKQTPMathImage::MATLAB;
    imageMin=0;
    imageMax=0;
    imageAutoRange=true;
    imageLegend="";
    imageLegendG="";
    imageLegendB="";
    imageLegendMod="";
    imageColorbarRight=true;
    imageColorbarTop=false;
    colorbarWidth=20;
    colorbarRelativeHeight=0.75;
    function="";
    modifierMode=JKQTPMathImage::ModifyValue;
    functionType=gtfString;

}

QFRDRTable::PlotInfo::PlotInfo()
{
    title="";
    xlabel="x";
    ylabel="y";
    xlog=false;
    ylog=false;
    xmin=0;
    xmax=10;
    ymin=0;
    ymax=10;
    showKey=true;
    grid=true;
    QStringList fdb=QFontDatabase().families();
    fontName=QApplication::font().family();
    if (fdb.contains("DejaVu Sans")) fontName="DejaVu Sans";
    else if (fdb.contains("Arial")) fontName="Arial";
    else if (fdb.contains("Helvetica")) fontName="Helvetica";
    keyFontSize=12;
    axisFontSize=14;
    axisLabelFontSize=18;
    labelFontSize=18;
    keyTransparency=0.5;
    keyPosition=JKQTPkeyInsideTopRight;
    keyLayout=JKQTPkeyLayoutOneColumn;
    keepAxisAspectRatio=false;
    keepDataAspectRatio=false;
    dataAspectRatio=1;
    axisAspectRatio=1;
    x0axis=true;
    y0axis=true;



     backgroundColor=QColor("white");
     gridColor=QColor("darkgrey");
     gridStyle=Qt::DashLine;
     gridWidth=1;
     xlabelPos=JKQTPlabelCenter;
     xlabelType=JKQTPCALTexponent;
     xlabelMode1=JKQTPCADMcomplete;
     xlabelMode2=JKQTPCADMticks;
     xdigits=3;
     xminTicks=7;
     xminorTicks=1;
     ylabelPos=JKQTPlabelCenter;
     ylabelType=JKQTPCALTexponent;
     ylabelMode1=JKQTPCADMcomplete;
     ylabelMode2=JKQTPCADMticks;
     ydigits=3;
     yminTicks=5;
     yminorTicks=1;

}







QFRDRTable::QFRDRTable(QFProject* parent/*, QString name, QString inputFile*/):
    QFRawDataRecord(parent)//, name, QStringList(inputFile))
{
    datamodel=NULL;
    autocolors.append(QColor("red"));
    autocolors.append(QColor("green"));
    autocolors.append(QColor("blue"));
    autocolors.append(QColor("magenta"));
    autocolors.append(QColor("darkcyan"));
    autocolors.append(QColor("darkred"));
    autocolors.append(QColor("darkgreen"));
    autocolors.append(QColor("darkblue"));
    autocolors.append(QColor("darkmagenta"));
    autocolors.append(QColor("fuchsia"));
    autocolors.append(QColor("darkorange"));
    autocolors.append(QColor("firebrick"));
    autocolors.append(QColor("darkgreen"));
    autocolors.append(QColor("darkslateblue"));
    autocolors.append(QColor("maroon"));
    autocolors.append(QColor("indianred"));
    autocolors.append(QColor("darkolivegreen"));
    autocolors.append(QColor("mediumpurple"));
    emittedRebuildPlotWidgets=false;
    connect(this, SIGNAL(rebuildPlotWidgets()), this, SLOT(didRebuildPlotWidgets()));
}


QFRDRTable::~QFRDRTable()
{
    //dtor
}

QFTablePluginModel* QFRDRTable::model() {
    if (!datamodel) {
        datamodel=new QFTablePluginModel(this);
        datamodel->setDefaultEditValue(DEFAULT_EDITVAL);
        datamodel->setVerticalHeaderShowRowNumbers(true);
        connect(datamodel, SIGNAL(modelReset()), this, SLOT(trawDataChanged()));
        connect(datamodel, SIGNAL(dataChanged( const QModelIndex & , const QModelIndex &  )), this, SLOT(tdataChanged( const QModelIndex & , const QModelIndex &  )));
    }
    return datamodel;
};

QVariant QFRDRTable::getModelData(quint16 row, quint16 column) {
    if (!datamodel) {
        datamodel=new QFTablePluginModel(this);
        datamodel->setVerticalHeaderShowRowNumbers(true);
        datamodel->setDefaultEditValue(DEFAULT_EDITVAL);
        connect(datamodel, SIGNAL(modelReset()), this, SLOT(trawDataChanged()));
        connect(datamodel, SIGNAL(dataChanged( const QModelIndex & , const QModelIndex &  )), this, SLOT(tdataChanged( const QModelIndex & , const QModelIndex &  )));
    }
    return datamodel->data(datamodel->index(row, column), Qt::DisplayRole);
}



QVariant QFRDRTable::tableGetData(quint16 row, quint16 column) const
{
    if (datamodel) return datamodel->cell(row, column);
    else return QVariant();
}

void QFRDRTable::tableSetData(quint16 row, quint16 column, const QVariant &data) {
    //qDebug()<<"tableSetData("<<row<<", "<<column<<",    "<<data<<")";
    if (datamodel)  {
        datamodel->setCellCreate(row, column, data);
    }
}

void QFRDRTable::tableSetColumnData(quint16 column, QList<QVariant> data)
{
    if (datamodel)  {
        datamodel->setColumnCreate(column, data);
    }
}

void QFRDRTable::tableSetColumnTitle(quint16 column, const QString &data)
{
    //qDebug()<<"tableSetColumnTitle("<<column<<",    "<<data<<")   datamodel<<"<<datamodel;
    if (datamodel)  {
        datamodel->setColumnTitleCreate(column, data);
    }


}

QString QFRDRTable::tableGetColumnTitle(quint16 column) const
{
    if (datamodel) return datamodel->getColumnTitles().value(column, "");
    else return QString("");
}

bool QFRDRTable::tableSupportsExpressions() const
{
    return true;
}

void QFRDRTable::tableSetExpression(quint16 row, quint16 column, const QString &expression)
{
    if (datamodel)  {
        datamodel->setData(datamodel->index(row, column), expression, QFRDRTable::TableExpressionRole);
    }

}

QString QFRDRTable::tableGetExpression(quint16 row, quint16 column) const
{
    if (datamodel) return datamodel->data(datamodel->index(row, column), QFRDRTable::TableExpressionRole).toString();
    else return QString("");
}

void QFRDRTable::tableReevaluateExpressions()
{
    QFRDRTable* m=this;
    if (m) {
        if (m->model()) {
            m->model()->setReadonly(false);
            QModelIndexList idxs;
            for (int c=0; c<m->model()->columnCount(); c++) {
                for (int r=0; r<m->model()->rowCount(); r++) {
                    idxs<<m->model()->index(r, c);
                }

            }

            m->model()->disableSignals();


            bool ok=true;
            QFMathParser mp; // instanciate
            addQFRDRTableFunctions(&mp);
            mp.addVariableDouble("row", 1);
            mp.addVariableDouble("col", 1);
            //mp.addVariableDouble("column", 1);
            mp.addVariableDouble("columns", 1.0);
            mp.addVariableDouble("rows", 1.0);

            QMap<QString, QFMathParser::qfmpNode*> nodes;
            int changes=1;
            int iterations=0;
            int maxIterations=20;
            while (iterations<maxIterations && changes>0) {
                changes=0;
                for (int i=0; i<idxs.size(); i++) {
                    QString lexp=m->model()->cellUserRole(QFRDRTable::TableExpressionRole, idxs[i].row(), idxs[i].column()).toString();
                    if (!lexp.isEmpty()) {
                        QVariant ov=m->model()->cell(idxs[i].row(), idxs[i].column());
                        //qDebug()<<"     reeval "<<lexp;
                        if (!nodes.contains(lexp)) {
                            mp.resetErrors();
                            nodes[lexp]=mp.parse(lexp);
                            if (mp.hasErrorOccured()) {
                                //QMessageBox::critical(this, tr("QuickFit-table"), tr("An error occured while parsing the expression '%1':\n%2").arg(dlgMathExpression->getExpression()).arg(E.what()));
                                ok=false;
                            }
                        }

                        //qDebug()<<"     reeval "<<nodes[lexp]<<ok;
                        ok=nodes[lexp];
                        if (ok) {
                            QVariant nv=evaluateExpression(mp, nodes[lexp], idxs[i], &ok, lexp, NULL);
                            //qDebug()<<"     reeval("<<idxs[i].row()<<idxs[i].column()<<ov<<") <= "<<nv;
                            if (ok && ov!=nv) {
                                changes++;
                                m->model()->setCellCreate(idxs[i].row(), idxs[i].column(), nv);
                                m->model()->setCellUserRoleCreate(QFRDRTable::TableExpressionRole, idxs[i].row(), idxs[i].column(), lexp);
                            }
                        }
                        if (!ok) break;
                    }
                }
                if (!ok) break;
                iterations++;
                //qDebug()<<"** reeval: "<<iterations<<changes;
            }
            if (iterations>=maxIterations) {
                //QMessageBox::critical(this, tr("QuickFit-table"), tr("Stopped reevaluating expressions after %1 iterations!\n  Changes were detected after %1 iterations, this might point to circular references in expression.\n  So results might not be reliable, rerun!").arg(maxIterations));
            }

            m->model()->enableSignals(true);
        }
    }
}

int QFRDRTable::tableGetColumnCount() const
{
    if (datamodel) return datamodel->columnCount();
    return 0;
}

int QFRDRTable::tableGetRowCount() const
{
    if (datamodel) return datamodel->rowCount();
    return 0;
}

bool QFRDRTable::tableIsReadonly() const
{
    if (!datamodel) return true;
    return datamodel->isReadonly();
}

void QFRDRTable::colgraphToolsSetGraphtype(QFRDRTable::GraphInfo &g, QFRDRColumnGraphsInterface::ColumnGraphTypes type)
{
    switch (type) {
        case QFRDRColumnGraphsInterface::cgtLines:
            g.type=gtLines;
            g.drawLine=true;
            g.symbol=JKQTPnoSymbol;
            break;
        case QFRDRColumnGraphsInterface::cgtPoints:
            g.type=gtLines;
            g.drawLine=false;
            break;
        case QFRDRColumnGraphsInterface::cgtLinesPoints:
            g.type=gtLines;
            g.drawLine=true;
            break;
        case QFRDRColumnGraphsInterface::cgtBars:
            g.type=gtBarsVertical;
            break;
        case QFRDRColumnGraphsInterface::cgtSteps:
            g.type=gtStepsVertical;
            break;
        case QFRDRColumnGraphsInterface::cgtImpulses:
            g.type=gtImpulsesVertical;
            break;
    }
}

void QFRDRTable::colgraphAddPlot(int graph, int columnX, int columnY, QFRDRColumnGraphsInterface::ColumnGraphTypes type, const QString &title)
{
    if (graph>=0 && graph<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(graph);
        QFRDRTable::GraphInfo g;
        g.xcolumn=columnX;
        g.ycolumn=columnY;
        g.color=autocolors.value((plt.graphs.size()-1)%autocolors.size(), QColor("red"));
        g.errorColor=g.color.darker();
        g.fillColor=g.color.lighter();
        colgraphToolsSetGraphtype(g, type);
        g.title=title;
        plt.graphs.append(g);

        setPlot(graph, plt);
        emitRebuildPlotWidgets();

    }
}

void QFRDRTable::colgraphAddErrorPlot(int graph, int columnX, int columnXError, int columnY, int columnYError, QFRDRColumnGraphsInterface::ColumnGraphTypes type, const QString &title, ErrorGraphTypes errorStyle)
{
    if (graph>=0 && graph<plots.size()) {

        colgraphAddPlot(graph, columnX, columnY, type, title);

        QFRDRTable::PlotInfo plt=getPlot(graph);
        plt.graphs.last().xerrorcolumn=columnXError;
        plt.graphs.last().yerrorcolumn=columnYError;
        plt.graphs.last().errorStyle=(JKQTPerrorPlotstyle)errorStyle;
        setPlot(graph, plt);
        emitRebuildPlotWidgets();
    }

}

void QFRDRTable::colgraphAddGraph(const QString &title, const QString &xLabel, const QString &yLabel, bool logX, bool logY)
{
    PlotInfo info;
    info.title=title;
    info.xlabel=xLabel;
    info.ylabel=yLabel;
    info.xlog=logX;
    info.ylog=logY;
    addPlot(info);
    emitRebuildPlotWidgets();
}

int QFRDRTable::colgraphGetPlotCount(int graph) const
{
    if (graph>=0 && graph<plots.size()) {
        return plots[graph].graphs.size();
    }
    return 0;
}

int QFRDRTable::colgraphGetGraphCount() const
{
    return getPlotCount();
}

void QFRDRTable::colgraphRemoveGraph(int graph)
{
    deletePlot(graph);
    emitRebuildPlotWidgets();
}

void QFRDRTable::colgraphRemovePlot(int graph, int plot)
{
    if (graph>=0 && graph<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(graph);
        plt.graphs.removeAt(plot);
        setPlot(graph, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetPlotColor(int graph, int plot, QColor color)
{
    if (graph>=0 && graph<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(graph);
        if (plot>=0 && plot<plt.graphs.size()) {
            plt.graphs[plot].color=color;
            plt.graphs[plot].colorTransparent=color.alphaF();
            plt.graphs[plot].errorColor=plt.graphs[plot].color.darker();
            plt.graphs[plot].errorColorTransparent=plt.graphs[plot].errorColor.alphaF();
            plt.graphs[plot].fillColor=plt.graphs[plot].color.lighter();
            plt.graphs[plot].fillColorTransparent=plt.graphs[plot].fillColor.alphaF();
        }
        setPlot(graph, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetPlotColor(int graph, int plot, QColor color, QColor fillColor)
{
    if (graph>=0 && graph<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(graph);
        if (plot>=0 && plot<plt.graphs.size()) {
            plt.graphs[plot].color=color;
            plt.graphs[plot].colorTransparent=color.alphaF();
            plt.graphs[plot].errorColor=plt.graphs[plot].color.darker();
            plt.graphs[plot].errorColorTransparent=plt.graphs[plot].errorColor.alphaF();
            plt.graphs[plot].fillColor=fillColor;
            plt.graphs[plot].fillColorTransparent=plt.graphs[plot].fillColor.alphaF();
        }
        setPlot(graph, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetPlotColor(int graph, int plot, QColor color, QColor fillColor, QColor errorColor)
{
    if (graph>=0 && graph<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(graph);
        if (plot>=0 && plot<plt.graphs.size()) {
            plt.graphs[plot].color=color;
            plt.graphs[plot].colorTransparent=color.alphaF();
            plt.graphs[plot].errorColor=errorColor;
            plt.graphs[plot].errorColorTransparent=plt.graphs[plot].errorColor.alphaF();
            plt.graphs[plot].fillColor=fillColor;
            plt.graphs[plot].fillColorTransparent=plt.graphs[plot].fillColor.alphaF();
        }
        setPlot(graph, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetGraphTitle(int graph, const QString &title)
{
    if (graph>=0 && graph<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(graph);
        plt.title=title;
        setPlot(graph, plt);
        emitRebuildPlotWidgets();
    }
}

QString QFRDRTable::colgraphGetGraphTitle(int graph)
{
    if (graph>=0 && graph<plots.size()) {
        return getPlot(graph).title;
    }
    return "";
}


void QFRDRTable::colgraphSetGraphXAxisProps(int graph, const QString &xLabel, bool logX)
{
    if (graph>=0 && graph<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(graph);
        plt.xlabel=xLabel;
        plt.xlog=logX;
        setPlot(graph, plt);
        emitRebuildPlotWidgets();
    }

}

void QFRDRTable::colgraphSetGraphYAxisProps(int graph, const QString &yLabel, bool logY)
{
    if (graph>=0 && graph<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(graph);
        plt.ylabel=yLabel;
        plt.ylog=logY;
        setPlot(graph, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphsetXRange(int graph, double xmin, double xmax)
{
    if (graph>=0 && graph<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(graph);
        plt.xmin=xmin;
        plt.xmax=xmax;
        setPlot(graph, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphsetYRange(int graph, double xmin, double xmax)
{
    if (graph>=0 && graph<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(graph);
        plt.ymin=xmin;
        plt.ymax=xmax;
        setPlot(graph, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetPlotTitle(int graph, int plot, const QString &title)
{
    if (graph>=0 && graph<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(graph);
        if (plot>=0 && plot<plt.graphs.size()) {
            plt.graphs[plot].title=title;
        }
        setPlot(graph, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetPlotType(int graph, int plot, QFRDRColumnGraphsInterface::ColumnGraphTypes type)
{
    if (graph>=0 && graph<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(graph);
        if (plot>=0 && plot<plt.graphs.size()) {
           colgraphToolsSetGraphtype(plt.graphs[plot], type);
        }
        setPlot(graph, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetPlotErrorStyle(int graph, int plot, QFRDRColumnGraphsInterface::ErrorGraphTypes errorStyle)
{
    if (graph>=0 && graph<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(graph);
        if (plot>=0 && plot<plt.graphs.size()) {
            plt.graphs[plot].errorStyle=(JKQTPerrorPlotstyle)errorStyle;
        }
        setPlot(graph, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetPlotErrorColor(int graph, int plot, QColor errorColor)
{
    if (graph>=0 && graph<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(graph);
        if (plot>=0 && plot<plt.graphs.size()) {
            plt.graphs[plot].errorColor=errorColor;
            plt.graphs[plot].errorColorTransparent=errorColor.alphaF();
        }
        setPlot(graph, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetPlotFillColor(int graph, int plot, QColor fillColor)
{
    if (graph>=0 && graph<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(graph);
        if (plot>=0 && plot<plt.graphs.size()) {
            plt.graphs[plot].fillColor=fillColor;
            plt.graphs[plot].fillColorTransparent=fillColor.alphaF();
        }
        setPlot(graph, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetPlotErrorTransparency(int graph, int plot, double errorT)
{
    if (graph>=0 && graph<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(graph);
        if (plot>=0 && plot<plt.graphs.size()) {
            plt.graphs[plot].errorColorTransparent=errorT;
        }
        plt.graphs[plot];
        setPlot(graph, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetPlotFillTransparency(int graph, int plot, double fillT)
{
    if (graph>=0 && graph<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(graph);
        if (plot>=0 && plot<plt.graphs.size()) {
            plt.graphs[plot].fillColorTransparent=fillT;
        }
        plt.graphs[plot];
        setPlot(graph, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetPlotPlotTransparency(int graph, int plot, double trans)
{
    if (graph>=0 && graph<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(graph);
        if (plot>=0 && plot<plt.graphs.size()) {
            plt.graphs[plot].colorTransparent=trans;
        }
        setPlot(graph, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetPlotSymbol(int graph, int plot, QFRDRColumnGraphsInterface::ColumnGraphSymbols symbol, double symbolSize)
{
    if (graph>=0 && graph<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(graph);
        if (plot>=0 && plot<plt.graphs.size()) {
            plt.graphs[plot].symbol=(JKQTPgraphSymbols)symbol;
            plt.graphs[plot].symbolSize=symbolSize;
        }
        setPlot(graph, plt);
        emitRebuildPlotWidgets();
    }
}


int QFRDRTable::getPlotCount() const
{
    return plots.size();
}

QFRDRTable::PlotInfo QFRDRTable::getPlot(int i) const
{
    return plots[i];
}

void QFRDRTable::addPlot() {
    PlotInfo info;
    info.title="new graph";
    plots.append(info);
    trawDataChanged();
}

void QFRDRTable::addPlot(QFRDRTable::PlotInfo plotInfo)
{
    plots.append(plotInfo);
    trawDataChanged();
}

void QFRDRTable::setPlot(int i, QFRDRTable::PlotInfo plotInfo)
{
    if (i>=0 && i<plots.size()) {
        plots[i]=plotInfo;
        trawDataChanged();
    }
}

void QFRDRTable::deletePlot(int i)
{
    if (i>=0 && i<plots.size()) {
        plots.removeAt(i);
        trawDataChanged();
    }
}

void QFRDRTable::swapPlots(int i, int j)
{
    if (i>=0 && i<plots.size() && j>=0 && j<plots.size()) {
        plots.swap(i, j);
        trawDataChanged();
    }
}





QVariant QFRDRTable::evaluateExpression(QFMathParser& mp, QFMathParser::qfmpNode *n, QModelIndex cell, bool* ok, const QString& expression, QString* error, bool columnMode)
{
    QVariant result;
    QFRDRTable* m=this;
    int row = cell.row();
    int column = cell.column();
    if (m) {
        if (columnMode) {
            mp.addVariableDouble("col", cell.column()+1);
            mp.addVariableDouble("rows", m->model()->rowCount());
            mp.addVariableDouble("columns", m->model()->columnCount());
        } else {
            mp.addVariableDouble("row", cell.row()+1);
            mp.addVariableDouble("col", cell.column()+1);
            //mp.addVariableDouble("column", cell.column()+1);
            mp.addVariableDouble("rows", m->model()->rowCount());
            mp.addVariableDouble("columns", m->model()->columnCount());
        }
        mp.resetErrors();

        QFMathParserData d;
        d.column=column;
        d.row=row;
        d.model=m->model();
        mp.set_data(&d);

        qfmpResult r;
        r=n->evaluate();


        if (r.isUsableResult()) {
            if (r.type==qfmpBool) {
                result=QVariant(r.boolean);
            } else if (r.type==qfmpDoubleVector) {
                if (columnMode) {
                    QVariantList vl;
                    for (int i=0; i<r.numVec.size(); i++) {
                        vl<<r.numVec[i];
                    }
                    return vl;
                } else {
                    double res=r.asNumberAlsoVector();
                    if (QFFloatIsOK(res)) result=QVariant(res);
                    else result=QVariant();
                }
            } else if (r.type==qfmpDouble) {
                if (QFFloatIsOK(r.num))
                    result=QVariant(r.num);
                else
                    result=QVariant();
            } else if (r.type==qfmpString) {
                result=QVariant(r.str);
            } else {
                result=QVariant();
            }
        } else {
            result=QVariant();
        }

        mp.set_data(NULL);
        if (mp.hasErrorOccured()) {
           if (*ok) *ok=false;
           if (error) *error=tr("An error occured while parsing the expression '%1' in cell (row, column)=(%3, %4):\n%2\n\n\"OK\" will still go on evaluating\n\"Cancel\" will cancel evaluation for the rest of the cells.").arg(expression).arg(mp.getLastError()).arg(row).arg(column);
           return QVariant();
        }
    }

    if (*ok) *ok=true;
    return result;
}

void QFRDRTable::emitRebuildPlotWidgets()
{
    if (!emittedRebuildPlotWidgets) emit rebuildPlotWidgets();
    //emittedRebuildPlotWidgets=true;
}



int QFRDRTable::getEditorCount()
{
    return 2;
}

QString QFRDRTable::getEditorName(int i)
{
    if (i==0) return tr("Data Table");
    if (i==1) return tr("Plots");
    return QString("");
}

QFRawDataEditor *QFRDRTable::createEditor(QFPluginServices *services, QFRawDataPropertyEditor *propEditor, int i, QWidget *parent)
{

    if (i==0) {
        QFRDRTableEditor* tab= new QFRDRTableEditor(services, propEditor, parent);
        return tab;
    }
    if (i==1) {
        QFRDRPlotEditor* edt= new QFRDRPlotEditor(services, propEditor, parent);
        if (propEditor) {
            for (int j=0; j<propEditor->getEditorList().size(); j++) {
                QFRDRTableEditor* tabEdt=qobject_cast<QFRDRTableEditor*>(propEditor->getEditorList().value(j, NULL));
                if (tabEdt) {
                    connect(edt, SIGNAL(performFit(int,int,int,int,QString,bool,bool)), tabEdt, SLOT(requestFit(int,int,int,int,QString,bool,bool)));
                    connect(edt, SIGNAL(performRegression(int,int,int,int,bool,bool)), tabEdt, SLOT(requestRegression(int,int,int,int,bool,bool)));
                }
            }
        }
        return edt;
    }
    return NULL;
};


void QFRDRTable::exportData(const QString& format, const QString& filename)const  {
    if (!datamodel) return;
    QString f=format.toUpper();
    if (f=="CSV") {
        datamodel->saveCSV(filename);
    } else if (f=="SSV") {
        datamodel->saveCSV(filename, ";", '.');
    } else if (f=="GERMANEXCEL") {
        datamodel->saveCSV(filename, ";", ',');
    } else if (f=="SLK" || f=="SYLK") {
        datamodel->saveSYLK(filename);
    }
}



void QFRDRTable::intReadData(QDomElement* e) {
    if (!datamodel) {
        datamodel=new QFTablePluginModel(this);
        datamodel->setVerticalHeaderShowRowNumbers(true);
        datamodel->setDefaultEditValue(DEFAULT_EDITVAL);
        connect(datamodel, SIGNAL(modelReset()), this, SLOT(trawDataChanged()));
        connect(datamodel, SIGNAL(dataChanged( const QModelIndex & , const QModelIndex &  )), this, SLOT(tdataChanged( const QModelIndex & , const QModelIndex &  )));
    }
    datamodel->clear();
    datamodel->setReadonly(false);
    if (files.size()>0) {
        //std::cout<<"    reading CSV\n";
        QString s=getProperty("column_separator", ",").toString();
        char column_separator=(s.size()>0)?s[0].toAscii():',';
        s=getProperty("decimal_separator", ".").toString();
        char decimal_separator=(s.size()>0)?s[0].toAscii():'.';
        s=getProperty("header_start", "#!").toString();
        QString header_start=s;
        s=getProperty("coment_start", "#").toString();
        char comment_start=(s.size()>0)?s[0].toAscii():'#';

        //std::cout<<"column_separator="<<column_separator<<"  decimal_separator="<<decimal_separator<<"   header_start="<<header_start.toStdString()<<"   comment_start="<<comment_start<<std::endl;

        datamodel->readCSV(files[0], column_separator, decimal_separator, header_start, comment_start);
        datamodel->setReadonly(true);
        datamodel->resetChanged();
    } else if (e) {
        QDomElement te=e->firstChildElement("column");
        quint16 rows=0, columns=0;
        datamodel->setReadonly(false);
        if (!te.isNull()) {
            while (!te.isNull()) {
                columns++;
                QString n=te.attribute("title");
                QString hexp=te.attribute("expression", te.attribute("column_expression"));
                QString hc=te.attribute("comment", te.attribute("column_comment"));
                QDomElement re=te.firstChildElement("row");
                quint16 r=0;
                //std::cout<<"resize("<<rows<<", "<<columns<<")\n";
                datamodel->resize(rows, columns);
                datamodel->setColumnTitle(columns-1, n);
                if (!hexp.isEmpty()) datamodel->setColumnHeaderData(columns-1, ColumnExpressionRole, hexp);
                if (!hc.isEmpty()) datamodel->setColumnHeaderData(columns-1, ColumnCommentRole, hc);
                QDomNamedNodeMap sl=te.attributes();
                for (int i=0; i<sl.size(); i++) {
                    QString a=sl.item(i).nodeName();
                    //qDebug()<<"found attribute "<<a;
                    QRegExp rx("colpar(\\w+)(\\d+)");
                    if (  rx.indexIn(a)>=0) {
                        bool ok=false;
                        int role=rx.cap(1).toInt(&ok);
                        if (ok&&role>=0 && !te.attribute(a).isEmpty()) {
                            QVariant v= getQVariantFromString( te.attribute(QString("colpart%1").arg(role), "string"), te.attribute(a));
                            datamodel->setColumnHeaderData(columns-1, ColumnExpressionRole, v);
                        }
                    }
                }
                //std::cout<<"resize("<<rows<<", "<<columns<<") DONE\n";
                while (!re.isNull()) {
                    QString t=re.attribute("type").toLower();
                    QString ex=re.attribute("expression");
                    QString comment=re.attribute("comment");
                    if (r+1>rows) {
                        rows=r+1;
                        //std::cout<<"resize("<<rows<<", "<<columns<<")\n";
                        datamodel->resize(rows, columns);
                        //std::cout<<"resize("<<rows<<", "<<columns<<") DONE\n";
                    }
                    QVariant v=getQVariantFromString(t, re.attribute("value"));
                    //std::cout<<"setCell("<<r<<", "<<columns-1<<", '"<<v.toString().toStdString()<<"' ["<<v.typeName()<<"])\n";
                    datamodel->setCell(r, columns-1, v);
                    if (!ex.isEmpty()) datamodel->setCellUserRole(QFRDRTable::TableExpressionRole, r, columns-1, ex);
                    if (!comment.isEmpty()) datamodel->setCellUserRole(QFRDRTable::TableCommentRole, r, columns-1, comment);


                    re = re.nextSiblingElement("row");
                    r++;
                }

                te = te.nextSiblingElement("column");
            }
        } else {
            datamodel->resize(1,1);
        }
        datamodel->resetChanged();
    }

    if (e) {
        QDomElement te=e->firstChildElement("plots");
        plots.clear();
        if (!te.isNull()) {
            te=te.firstChildElement("plot");
            while (!te.isNull()) {
                PlotInfo plot;
                plot.title=te.attribute("title", tr("graph title"));
                plot.xlabel=te.attribute("xlabel", "x");
                plot.ylabel=te.attribute("ylabel", "y");
                plot.xlog=QStringToBool( te.attribute("xlog", "false"));
                plot.ylog=QStringToBool( te.attribute("ylog", "false"));
                plot.x0axis=QStringToBool( te.attribute("x0axis", "true"));
                plot.y0axis=QStringToBool( te.attribute("y0axis", "true"));
                plot.grid=QStringToBool( te.attribute("grid", "true"));
                plot.showKey=QStringToBool( te.attribute("showkey"));
                plot.xmin=CQStringToDouble(te.attribute("xmin", "0"));
                plot.xmax=CQStringToDouble(te.attribute("xmax", "10"));
                plot.ymin=CQStringToDouble(te.attribute("ymin", "0"));
                plot.ymax=CQStringToDouble(te.attribute("ymax", "10"));
                plot.fontName=te.attribute("fontname", "Arial");
                plot.keyFontSize=te.attribute("keyfontsize", "12").toDouble();
                plot.axisFontSize=te.attribute("axisfontsize", "10").toDouble();
                plot.axisLabelFontSize=te.attribute("axislabelfontsize", "12").toDouble();
                plot.labelFontSize=te.attribute("labelFontSize", "16").toDouble();

                plot.keyTransparency=te.attribute("keytransparency", "0.5").toDouble();
                plot.keyPosition=String2JKQTPkeyPosition(te.attribute("keyposition", "inside_right"));
                plot.keyLayout=String2JKQTPkeyLayout(te.attribute("keylayout", "one_column"));

                plot.keepDataAspectRatio=QStringToBool( te.attribute("keep_dataaspect", "false"));
                plot.axisAspectRatio=CQStringToDouble(te.attribute("dataaspect", "1"));
                plot.keepAxisAspectRatio=QStringToBool( te.attribute("keep_axisaspect", "false"));
                plot.dataAspectRatio=CQStringToDouble(te.attribute("axisaspect", "1"));

                plot.backgroundColor=QStringToQColor(te.attribute("background_color", "white"));
                plot.gridColor=QStringToQColor(te.attribute("grid_color", "darkgrey"));
                plot.gridStyle=String2QPenStyle(te.attribute("grid_style", "dash"));
                plot.gridWidth=te.attribute("grid_width", "1").toDouble();
                plot.xdigits=te.attribute("xdigits", "3").toInt();
                plot.ydigits=te.attribute("ydigits", "3").toInt();
                plot.xminTicks=te.attribute("xmin_ticks", "7").toInt();
                plot.xminorTicks=te.attribute("xminor_ticks", "1").toInt();
                plot.yminTicks=te.attribute("ymin_ticks", "7").toInt();
                plot.yminorTicks=te.attribute("yminor_ticks", "1").toInt();
                plot.xlabelPos=String2JKQTPlabelPosition(te.attribute("xlabel_pos", "center"));
                plot.ylabelPos=String2JKQTPlabelPosition(te.attribute("ylabel_pos", "center"));
                plot.xlabelType=String2JKQTPCAlabelType(te.attribute("xlabel_type", "exponent"));
                plot.ylabelType=String2JKQTPCAlabelType(te.attribute("ylabel_type", "exponent"));
                plot.xlabelMode1=String2JKQTPCAdrawMode(te.attribute("xlabel_mode1", "all"));
                plot.xlabelMode2=String2JKQTPCAdrawMode(te.attribute("xlabel_mode2", "ticks"));
                plot.ylabelMode1=String2JKQTPCAdrawMode(te.attribute("ylabel_mode1", "all"));
                plot.ylabelMode2=String2JKQTPCAdrawMode(te.attribute("ylabel_mode2", "ticks"));



                QDomElement ge=te.firstChildElement("graph");
                while (!ge.isNull()) {
                    GraphInfo graph;
                    graph.type=String2GraphType(ge.attribute("type"));
                    graph.title=ge.attribute("title");
                    graph.drawLine= QStringToBool(ge.attribute("drawline", "true"));
                    graph.xcolumn=ge.attribute("xcolumn", "-1").toInt();
                    graph.ycolumn=ge.attribute("ycolumn", "-1").toInt();
                    graph.xerrorcolumn=ge.attribute("xerrorcolumn", "-1").toInt();
                    graph.yerrorcolumn=ge.attribute("yerrorcolumn", "-1").toInt();
                    graph.linewidth=ge.attribute("linewidth", "1").toDouble();
                    graph.symbolSize=ge.attribute("symbolSize", "1").toDouble();
                    graph.color=QStringToQColor(ge.attribute("color", "blue"));
                    graph.errorColor=QStringToQColor(ge.attribute("errorcolor", "darkblue"));
                    graph.fillColor=QStringToQColor(ge.attribute("fillcolor", "blue"));
                    //qDebug()<<ge.attribute("type")<<ge.attribute("fillcolor", "blue");
                    graph.style=String2QPenStyle(ge.attribute("style", "solid"));
                    graph.symbol=String2JKQTPgraphSymbols(ge.attribute("symbol", "symbol_cross"));
                    graph.errorStyle=String2JKQTPerrorPlotstyle(ge.attribute("errorStyle", "error_none"));
                    graph.colorTransparent=ge.attribute("color_trans", "1").toDouble();
                    graph.errorColorTransparent=ge.attribute("errorcolor_trans", "1").toDouble();
                    graph.fillColorTransparent=ge.attribute("fillcolor_trans", "0.3").toDouble();

                    graph.stride=ge.attribute("stride", "1").toInt();
                    graph.strideStart=ge.attribute("stride_start", "1").toInt();
                    graph.isStrided=QStringToBool(ge.attribute("is_strided", "false"));

                    graph.imageTrueColor=QStringToQColor(ge.attribute("image_truecolor", "blue"));
                    graph.imageTrueTransparent=ge.attribute("image_truecolor_trans", "0.5").toDouble();
                    graph.imageFalseColor=QStringToQColor(ge.attribute("image_falsecolor", "red"));
                    graph.imageFalseTransparent=ge.attribute("image_falsecolor_trans", "1.0").toDouble();
                    graph.imagePixelWidth=ge.attribute("image_pixelwidth", "1").toInt();
                    graph.imageX=ge.attribute("image_x", "0").toDouble();
                    graph.imageY=ge.attribute("image_y", "0").toDouble();
                    graph.imageWidth=ge.attribute("image_width", "1").toDouble();
                    graph.imageHeight=ge.attribute("image_height", "1").toDouble();
                    graph.imagePalette=JKQTPMathImage::ColorPalette(ge.attribute("image_palette", "8").toInt());
                    graph.imageMin=ge.attribute("image_min", "0").toDouble();
                    graph.imageMax=ge.attribute("image_max", "0").toDouble();
                    graph.imageAutoRange=QStringToBool(ge.attribute("image_autorange", "true"));

                    graph.imageColorbarRight=QStringToBool(ge.attribute("image_colorbarright", "true"));
                    graph.imageColorbarTop=QStringToBool(ge.attribute("image_colorbartop", "false"));
                    graph.colorbarWidth=ge.attribute("image_colorbarwidth", "20").toDouble();
                    graph.colorbarRelativeHeight=ge.attribute("image_colorbarrelativeheight", "0.75").toDouble();
                    graph.imageLegend=ge.attribute("image_legend", "");
                    graph.imageLegendG=ge.attribute("image_legend_g", "");
                    graph.imageLegendB=ge.attribute("image_legend_b", "");
                    graph.imageLegendMod=ge.attribute("image_legend_mod", "");
                    graph.function=ge.attribute("function", "");
                    graph.functionType=String2GTFunctionType(ge.attribute("functiontype", "string"));
                    graph.modifierMode=JKQTPMathImage::StringToModifierMode(ge.attribute("modifier_mode", "none"));


                    plot.graphs.append(graph);
                    ge = ge.nextSiblingElement("graph");
                }

                plots.append(plot);
                te = te.nextSiblingElement("plot");
            }
        }
    }
}

void QFRDRTable::intWriteData(QXmlStreamWriter& w) {
    if (!datamodel) {
        datamodel=new QFTablePluginModel(this);
        datamodel->setVerticalHeaderShowRowNumbers(true);
        datamodel->setDefaultEditValue(DEFAULT_EDITVAL);
        connect(datamodel, SIGNAL(modelReset()), this, SLOT(trawDataChanged()));
        connect(datamodel, SIGNAL(dataChanged( const QModelIndex & , const QModelIndex &  )), this, SLOT(tdataChanged( const QModelIndex & , const QModelIndex &  )));
    }
    if (files.size()>0) {
        if (datamodel->hasChanged()) datamodel->saveCSV(files[0]);
    } else {
        for (quint16 c=0; c<datamodel->columnCount(); c++) {
            w.writeStartElement("column");
            w.writeAttribute("title", datamodel->columnTitle(c));
            if (datamodel->hasColumnHeaderData(c, ColumnExpressionRole)) {
                w.writeAttribute("expression", datamodel->getColumnHeaderData(c, ColumnExpressionRole).toString());
            }
            if (datamodel->hasColumnHeaderData(c, ColumnCommentRole)) {
                w.writeAttribute("comment", datamodel->getColumnHeaderData(c, ColumnCommentRole).toString());
            }
            QList<quint32> coldroles=datamodel->getColumnHeaderDataRoles();
            for (int i=0; i<coldroles.size(); i++) {
                if (coldroles[i]!=ColumnExpressionRole && coldroles[i]!=ColumnCommentRole) {
                    w.writeAttribute(QString("colpar%1").arg(coldroles[i]), getQVariantData(datamodel->getColumnHeaderData(c, coldroles[i])));
                    w.writeAttribute(QString("colpart%1").arg(coldroles[i]), getQVariantType(datamodel->getColumnHeaderData(c, coldroles[i])));

                }
            }
            for (quint16 r=0; r<datamodel->rowCount(); r++) {
                if (datamodel->cell(r, c).isValid()) {
                    w.writeStartElement("row");
                    w.writeAttribute("type", getQVariantType(datamodel->cell(r, c)));
                    w.writeAttribute("value", getQVariantData(datamodel->cell(r, c)));
                    QString ex=datamodel->cellUserRole(QFRDRTable::TableExpressionRole, r, c).toString();
                    if (!ex.isEmpty()) w.writeAttribute("expression", ex);
                    QString comment=datamodel->cellUserRole(QFRDRTable::TableCommentRole, r, c).toString();
                    if (!comment.isEmpty()) w.writeAttribute("comment", comment);
                    w.writeEndElement();
                }
            }
            w.writeEndElement();
        }
    }

    w.writeStartElement("plots");
    for (int i=0; i<plots.size(); i++) {
        w.writeStartElement("plot");
        w.writeAttribute("title", plots[i].title);
        w.writeAttribute("xlabel", plots[i].xlabel);
        w.writeAttribute("ylabel", plots[i].ylabel);
        w.writeAttribute("showkey", boolToQString(plots[i].showKey));
        w.writeAttribute("grid", boolToQString(plots[i].grid));
        w.writeAttribute("xlog", boolToQString(plots[i].xlog));
        w.writeAttribute("ylog", boolToQString(plots[i].ylog));
        w.writeAttribute("x0axis", boolToQString(plots[i].x0axis));
        w.writeAttribute("y0axis", boolToQString(plots[i].y0axis));
        w.writeAttribute("xmin", CDoubleToQString(plots[i].xmin));
        w.writeAttribute("xmax", CDoubleToQString(plots[i].xmax));
        w.writeAttribute("ymin", CDoubleToQString(plots[i].ymin));
        w.writeAttribute("ymax", CDoubleToQString(plots[i].ymax));
        w.writeAttribute("keyfontsize", CDoubleToQString(plots[i].keyFontSize));
        w.writeAttribute("axisfontsize", CDoubleToQString(plots[i].axisFontSize));
        w.writeAttribute("axislabelfontsize", CDoubleToQString(plots[i].axisLabelFontSize));
        w.writeAttribute("labelfontsize", CDoubleToQString(plots[i].labelFontSize));
        w.writeAttribute("fontname", plots[i].fontName);
        w.writeAttribute("keytransparency", CDoubleToQString(plots[i].keyTransparency));
        w.writeAttribute("keyposition", JKQTPkeyPosition2String(plots[i].keyPosition));
        w.writeAttribute("keylayout", JKQTPkeyLayout2String(plots[i].keyLayout));
        w.writeAttribute("keep_dataaspect", boolToQString(plots[i].keepDataAspectRatio));
        w.writeAttribute("dataaspect", CDoubleToQString(plots[i].axisAspectRatio));
        w.writeAttribute("keep_axisaspect", boolToQString(plots[i].keepAxisAspectRatio));
        w.writeAttribute("axisaspect", CDoubleToQString(plots[i].dataAspectRatio));

        w.writeAttribute("grid_width", CDoubleToQString(plots[i].gridWidth));
        w.writeAttribute("background_color", QColor2String(plots[i].backgroundColor));
        w.writeAttribute("grid_color", QColor2String(plots[i].gridColor));
        w.writeAttribute("grid_style", QPenStyle2String(plots[i].gridStyle));

        w.writeAttribute("xdigits", QString::number(plots[i].xdigits));
        w.writeAttribute("ydigits", QString::number(plots[i].ydigits));
        w.writeAttribute("xmin_ticks", QString::number(plots[i].xminTicks));
        w.writeAttribute("xminor_ticks", QString::number(plots[i].xminorTicks));
        w.writeAttribute("ymin_ticks", QString::number(plots[i].yminTicks));
        w.writeAttribute("yminor_ticks", QString::number(plots[i].yminorTicks));
        w.writeAttribute("xlabel_pos", JKQTPlabelPosition2String(plots[i].xlabelPos));
        w.writeAttribute("ylabel_pos", JKQTPlabelPosition2String(plots[i].ylabelPos));
        w.writeAttribute("xlabel_type", JKQTPCAlabelType2String(plots[i].xlabelType));
        w.writeAttribute("ylabel_type", JKQTPCAlabelType2String(plots[i].ylabelType));

        w.writeAttribute("xlabel_mode1", JKQTPCAdrawMode2String(plots[i].xlabelMode1));
        w.writeAttribute("xlabel_mode2", JKQTPCAdrawMode2String(plots[i].xlabelMode2));
        w.writeAttribute("ylabel_mode1", JKQTPCAdrawMode2String(plots[i].ylabelMode1));
        w.writeAttribute("ylabel_mode2", JKQTPCAdrawMode2String(plots[i].ylabelMode2));


        for (int g=0; g<plots[i].graphs.size(); g++) {
            w.writeStartElement("graph");
            w.writeAttribute("type", GraphType2String(plots[i].graphs[g].type));
            w.writeAttribute("title", plots[i].graphs[g].title);
            w.writeAttribute("drawline", boolToQString( plots[i].graphs[g].drawLine));
            w.writeAttribute("xcolumn", QString::number(plots[i].graphs[g].xcolumn));
            w.writeAttribute("ycolumn", QString::number(plots[i].graphs[g].ycolumn));
            w.writeAttribute("xerrorcolumn", QString::number(plots[i].graphs[g].xerrorcolumn));
            w.writeAttribute("yerrorcolumn", QString::number(plots[i].graphs[g].yerrorcolumn));
            w.writeAttribute("linewidth", QString::number(plots[i].graphs[g].linewidth));
            w.writeAttribute("symbolSize", QString::number(plots[i].graphs[g].symbolSize));
            w.writeAttribute("style", QPenStyle2String(plots[i].graphs[g].style));
            w.writeAttribute("color", QColor2String(plots[i].graphs[g].color));
            w.writeAttribute("errorcolor", QColor2String(plots[i].graphs[g].errorColor));
            w.writeAttribute("fillcolor", QColor2String(plots[i].graphs[g].fillColor));
            w.writeAttribute("symbol", JKQTPgraphSymbols2String(plots[i].graphs[g].symbol));
            w.writeAttribute("errorStyle", JKQTPerrorPlotstyle2String(plots[i].graphs[g].errorStyle));
            w.writeAttribute("color_trans", QString::number(plots[i].graphs[g].colorTransparent));
            w.writeAttribute("errorcolor_trans", QString::number(plots[i].graphs[g].errorColorTransparent));
            w.writeAttribute("fillcolor_trans", QString::number(plots[i].graphs[g].fillColorTransparent));


            w.writeAttribute("image_truecolor", QColor2String(plots[i].graphs[g].imageTrueColor));
            w.writeAttribute("image_truecolor_trans", QString::number(plots[i].graphs[g].imageTrueTransparent));
            w.writeAttribute("image_falsecolor", QColor2String(plots[i].graphs[g].imageFalseColor));
            w.writeAttribute("image_falsecolor_trans", QString::number(plots[i].graphs[g].imageFalseTransparent));
            w.writeAttribute("image_pixelwidth", QString::number(plots[i].graphs[g].imagePixelWidth));
            w.writeAttribute("image_x", QString::number(plots[i].graphs[g].imageX));
            w.writeAttribute("image_y", QString::number(plots[i].graphs[g].imageY));
            w.writeAttribute("image_width", QString::number(plots[i].graphs[g].imageWidth));
            w.writeAttribute("image_height", QString::number(plots[i].graphs[g].imageHeight));
            w.writeAttribute("image_palette", QString::number(plots[i].graphs[g].imagePalette));
            w.writeAttribute("image_min", QString::number(plots[i].graphs[g].imageMin));
            w.writeAttribute("image_max", QString::number(plots[i].graphs[g].imageMax));
            w.writeAttribute("image_autorange", boolToQString(plots[i].graphs[g].imageAutoRange));

            w.writeAttribute("image_colorbarright", boolToQString(plots[i].graphs[g].imageColorbarRight));
            w.writeAttribute("image_colorbartop", boolToQString(plots[i].graphs[g].imageColorbarTop));
            w.writeAttribute("image_colorbarwidth", QString::number(plots[i].graphs[g].colorbarWidth));
            w.writeAttribute("image_colorbarrelativeheight", QString::number(plots[i].graphs[g].colorbarRelativeHeight));
            w.writeAttribute("image_legend", plots[i].graphs[g].imageLegend);
            w.writeAttribute("image_legend_g", plots[i].graphs[g].imageLegendG);
            w.writeAttribute("image_legend_b", plots[i].graphs[g].imageLegendB);
            w.writeAttribute("image_legend_mod", plots[i].graphs[g].imageLegendMod);
            w.writeAttribute("function", plots[i].graphs[g].function);
            w.writeAttribute("functiontype", GTFunctionType2String(plots[i].graphs[g].functionType));

            w.writeAttribute("stride", QString::number(plots[i].graphs[g].stride));
            w.writeAttribute("stride_start", QString::number(plots[i].graphs[g].strideStart));
            w.writeAttribute("is_strided", boolToQString(plots[i].graphs[g].isStrided));
            w.writeAttribute("modifier_mode", JKQTPMathImage::ModifierModeToString(plots[i].graphs[g].modifierMode));

            w.writeEndElement();
        }
        w.writeEndElement();
    }
    w.writeEndElement();
}

void QFRDRTable::didRebuildPlotWidgets()
{
    //emittedRebuildPlotWidgets=false;
}

