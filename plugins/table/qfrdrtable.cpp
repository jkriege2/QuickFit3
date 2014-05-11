#include "qfrdrtable.h"
#include "dlgcsvparameters.h"
#include <QtXml>
#include "qfrdrtableeditor.h"
#include "qfrdrploteditor.h"
#include "qftools.h"
#include "qfrdrtableparserfunctions.h"
#include "binarydatatools.h"


#define DEFAULT_EDITVAL double(0.0)



QFRDRTable::GraphInfo::GraphInfo() {
    moreProperties.clear();
    title="";
    isStrided=false;
    stride=1;
    strideStart=1;
    type=QFRDRTable::gtLines;
    xcolumn=-1;
    ycolumn=-1;
    xerrorcolumn=-1;
    yerrorcolumn=-1;
    meancolumn=-1;
    q75column=-1;
    maxcolumn=-1;
    width=0.9;
    shift=0;
    style=Qt::SolidLine;
    whiskerStyle=Qt::SolidLine;
    fillStyle=Qt::SolidPattern;
    color=QColor("red");
    errorColor=color.darker();
    linewidth=1;
    symbol=JKQTPfilledCircle;
    symbolSize=6;
    errorStyle=JKQTPnoError;
    drawLine=true;
    errorStyle=JKQTPerrorBars;

    errorWidth=1;
    errorLineStyle=Qt::SolidLine;
    errorBarSize=7;

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
    imagePalette=JKQTPMathImageMATLAB;
    imageMin=0;
    imageMax=0;
    imageAutoRange=true;
    imageLegend="";
    imageLegendG="";
    imageLegendB="";
    imageLegendMod="";
    imageColorbarRight=true;
    imageColorbarTop=false;

    imageTicks=6;
    imageModTicks=2;
    imageColorbarFontsize=7;
    imageColorbarTicklength=3;

    imageColorbarLabelType=JKQTPCALTexponent;
    imageColorbarLabelDigits=3;


    colorbarWidth=20;
    colorbarRelativeHeight=0.75;
    function="";
    modifierMode=JKQTPMathImage::ModifyValue;
    functionType=gtfString;

    rangeStart=0;
    rangeEnd=1;
    rangeCenter=0.5;
    rangeInverted=false;
    rangeFill=true;
    rangeDrawCenter=true;
    rangeCenterColor=color.darker();
    rangeCenterColorTransparent=1;
    rangeCenterStyle=Qt::DashLine;
    rangeCenterWidth=1;

    errorColorAuto=true;
    fillColorAuto=true;
    centerColorAuto=true;

    isDataSelect=false;
    dataSelectColumn=-1;
    dataSelectOperation=dsoEquals;
    dataSelectCompareValue=0;
}

QFRDRTable::AxisInfo::AxisInfo()
{
    label="x";
    log=false;
    min=0;
    max=10;
    axisFontSize=8;
    axisLabelFontSize=10;
    axis0=true;


     labelPos=JKQTPlabelCenter;
     labelType=JKQTPCALTexponent;
     labelMode1=JKQTPCADMcomplete;
     labelMode2=JKQTPCADMticks;
     digits=3;
     minTicks=5;
     minorTicks=1;




     TickSpacing=10;
     AutoTicks=true;
     AxisTickWidth=1.5;
     AxisMinorTickWidth=1;
     AxisInverted=false;
     TickInsideLength=3;
     TickOutsideLength=3;
     MinorTickInsideLength=1;
     MinorTickOutsideLength=1;
}

QFRDRTable::PlotInfo::PlotInfo()
{
    graphAutosize=true;
    graphWidth=150;
    graphHeight=150;
    title="";
    xAxis=AxisInfo();
    yAxis=AxisInfo();
    xAxis.label="x";
    yAxis.label="y";
    showKey=true;
    grid=true;
    QStringList fdb=QFontDatabase().families();
    fontName=QApplication::font().family();
    if (fdb.contains("DejaVu Sans")) fontName="DejaVu Sans";
    else if (fdb.contains("Arial")) fontName="Arial";
    else if (fdb.contains("Helvetica")) fontName="Helvetica";
    keyFontSize=10;
    axisFontSize=8;
    axisLabelFontSize=10;
    labelFontSize=12;
    keyTransparency=0.5;
    keyPosition=JKQTPkeyInsideTopRight;
    keyLayout=JKQTPkeyLayoutOneColumn;
    keepAxisAspectRatio=false;
    keepDataAspectRatio=false;
    dataAspectRatio=1;
    axisAspectRatio=1;



     backgroundColor=QColor("white");
     gridColor=QColor("darkgrey");
     gridStyle=Qt::DashLine;
     gridWidth=1.5;
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
        datamodel->setUndoEnabled(true);
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
        datamodel->setUndoEnabled(true);
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

void QFRDRTable::tableSetColumnData(quint16 column, const QList<QVariant> &data)
{
    if (datamodel)  {
        datamodel->setColumnCreate(column, data);
    }
}

QList<QVariant> QFRDRTable::tableGetColumnData(quint16 column)
{
    if (datamodel)  {
        return datamodel->getColumnData(column);
    }
    return QList<QVariant>();
}

void QFRDRTable::tableSetColumnDataAsDouble(quint16 column, const QVector<double> &data)
{
    if (datamodel)  {
        datamodel->setColumnCreate(column, data);
    }
}

QVector<double> QFRDRTable::tableGetColumnDataAsDouble(quint16 column)
{
    if (datamodel)  {
        return datamodel->getColumnDataAsNumbers(column);
    }
    return QVector<double>();
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

void QFRDRTable::tableSetColumnExpression(quint16 column, const QString &expression)
{
    if (datamodel)  {
        datamodel->setColumnHeaderData(column, QFRDRTable::ColumnExpressionRole, expression);
    }
}

void QFRDRTable::tableSetColumnComment(quint16 column, const QString &comment)
{
    if (datamodel)  {
        datamodel->setColumnHeaderData(column, QFRDRTable::ColumnCommentRole, comment);
    }
}

void QFRDRTable::tableSetComment(quint16 row, quint16 column, const QString &comment)
{
    if (datamodel)  {
        datamodel->setData(datamodel->index(row, column), comment, QFRDRTable::TableCommentRole);
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
        case QFRDRColumnGraphsInterface::cgtExpression:
        case QFRDRColumnGraphsInterface::cgtPolynomial:
        case QFRDRColumnGraphsInterface::cgtPowerLaw:
        case QFRDRColumnGraphsInterface::cgtExponential:
        case QFRDRColumnGraphsInterface::cgtQFFitFunction:
            g.type=gtFunction;
            break;
        case QFRDRColumnGraphsInterface::cgtHorizontalRange:
            g.type=gtHorizontalRange;
            break;
        case QFRDRColumnGraphsInterface::cgtImage:
            g.type=gtImage;
            break;
        case QFRDRColumnGraphsInterface::cgtMaskImage:
            g.type=gtMaskImage;
            break;
        case QFRDRColumnGraphsInterface::cgtRGBImage:
            g.type=gtRGBImage;
            break;
    }
}

void QFRDRTable::colgraphAddGraph(int plotid, int columnX, int columnY, QFRDRColumnGraphsInterface::ColumnGraphTypes type, const QString &title)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        QFRDRTable::GraphInfo g;
        g.xcolumn=columnX;
        g.ycolumn=columnY;
        g.color=autocolors.value((plt.graphs.size()-1)%autocolors.size(), QColor("red"));
        g.errorColor=g.color.darker();
        g.fillColor=g.color.lighter();
        colgraphToolsSetGraphtype(g, type);
        g.title=title;
        plt.graphs.append(g);

        setPlot(plotid, plt);
        emitRebuildPlotWidgets();

    }
}

void QFRDRTable::colgraphAddFunctionGraph(int plotid, const QString &expression, QFRDRColumnGraphsInterface::ColumnGraphTypes type, const QString &title, int columnParam)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        QFRDRTable::GraphInfo g;
        g.ycolumn=columnParam;
        g.color=autocolors.value((plt.graphs.size()-1)%autocolors.size(), QColor("red"));
        g.errorColor=g.color.darker();
        g.fillColor=g.color.lighter();
        colgraphToolsSetGraphtype(g, type);
        g.title=title;
        g.function=expression;
        switch (type) {
            case cgtPowerLaw: g.functionType=gtfPowerLaw; break;
            case cgtPolynomial: g.functionType=gtfPolynomial; break;
            case cgtExponential: g.functionType=gtfExponential; break;
            case cgtQFFitFunction: g.functionType=gtfQFFunction; break;
            default:
            case cgtExpression: g.functionType=gtfString; break;
        }
        g.functionParameters.clear();
        g.drawLine=true;
        g.symbol=JKQTPnoSymbol;

        plt.graphs.append(g);

        setPlot(plotid, plt);
        emitRebuildPlotWidgets();

    }
}

void QFRDRTable::colgraphAddFunctionGraph(int plotid, const QString &expression, QFRDRColumnGraphsInterface::ColumnGraphTypes type, const QString &title, const QVector<double> &params)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        QFRDRTable::GraphInfo g;
        g.ycolumn=-1;
        g.color=autocolors.value((plt.graphs.size()-1)%autocolors.size(), QColor("red"));
        g.errorColor=g.color.darker();
        g.fillColor=g.color.lighter();
        colgraphToolsSetGraphtype(g, type);
        g.title=title;
        g.function=expression;
        switch (type) {
            case cgtPowerLaw: g.functionType=gtfPowerLaw; break;
            case cgtPolynomial: g.functionType=gtfPolynomial; break;
            case cgtExponential: g.functionType=gtfExponential; break;
            case cgtQFFitFunction: g.functionType=gtfQFFunction; break;
            default:
            case cgtExpression: g.functionType=gtfString; break;
        }
        g.functionParameters=params;
        g.drawLine=true;
        g.symbol=JKQTPnoSymbol;

        plt.graphs.append(g);

        setPlot(plotid, plt);
        emitRebuildPlotWidgets();

    }
}

void QFRDRTable::colgraphSetFunctionGraph(int plotid, int graphid, const QString &expression, QFRDRColumnGraphsInterface::ColumnGraphTypes type, const QString &title, int columnParam)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            QFRDRTable::GraphInfo& g=plt.graphs[graphid];
            g.ycolumn=columnParam;
            g.color=autocolors.value((plt.graphs.size()-1)%autocolors.size(), QColor("red"));
            g.errorColor=g.color.darker();
            g.fillColor=g.color.lighter();
            colgraphToolsSetGraphtype(g, type);
            g.title=title;
            g.function=expression;
            switch (type) {
                case cgtPowerLaw: g.functionType=gtfPowerLaw; break;
                case cgtPolynomial: g.functionType=gtfPolynomial; break;
                case cgtExponential: g.functionType=gtfExponential; break;
                case cgtQFFitFunction: g.functionType=gtfQFFunction; break;
                default:
                case cgtExpression: g.functionType=gtfString; break;
            }
            g.functionParameters.clear();
            g.drawLine=true;
            g.symbol=JKQTPnoSymbol;
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }

}

void QFRDRTable::colgraphSetFunctionGraph(int plotid, int graphid, const QString &expression, QFRDRColumnGraphsInterface::ColumnGraphTypes type, const QString &title, const QVector<double> &params)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            QFRDRTable::GraphInfo& g=plt.graphs[graphid];
            g.ycolumn=-1;
            g.color=autocolors.value((plt.graphs.size()-1)%autocolors.size(), QColor("red"));
            g.errorColor=g.color.darker();
            g.fillColor=g.color.lighter();
            colgraphToolsSetGraphtype(g, type);
            g.title=title;
            g.function=expression;
            switch (type) {
                case cgtPowerLaw: g.functionType=gtfPowerLaw; break;
                case cgtPolynomial: g.functionType=gtfPolynomial; break;
                case cgtExponential: g.functionType=gtfExponential; break;
                case cgtQFFitFunction: g.functionType=gtfQFFunction; break;
                default:
                case cgtExpression: g.functionType=gtfString; break;
            }
            g.functionParameters=params;
            g.drawLine=true;
            g.symbol=JKQTPnoSymbol;
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphAddErrorGraph(int plotid, int columnX, int columnXError, int columnY, int columnYError, QFRDRColumnGraphsInterface::ColumnGraphTypes type, const QString &title, ErrorGraphTypes errorStyle)
{
    if (plotid>=0 && plotid<plots.size()) {

        colgraphAddGraph(plotid, columnX, columnY, type, title);

        QFRDRTable::PlotInfo plt=getPlot(plotid);
        plt.graphs.last().xerrorcolumn=columnXError;
        plt.graphs.last().yerrorcolumn=columnYError;
        plt.graphs.last().errorStyle=(JKQTPerrorPlotstyle)errorStyle;
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }

}

void QFRDRTable::colgraphAddImageGraph(int plotid, int imageColumn, QFRDRColumnGraphsInterface::ImageColorPalette palette, double x, double y, double width, double height, int Nx, const QString &title)
{
    if (plotid>=0 && plotid<plots.size()) {

        QFRDRTable::PlotInfo plt=getPlot(plotid);
        QFRDRTable::GraphInfo g;
        g.type=QFRDRTable::gtImage;
        g.xcolumn=imageColumn;
        g.imageX=x;
        g.imageY=y;
        g.imageWidth=width;
        g.imageHeight=height;
        g.imagePixelWidth=Nx;
        g.title=title;
        g.imagePalette=JKQTPMathImageColorPalette((int)palette);
        plt.graphs.append(g);

        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
     }
}

void QFRDRTable::colgraphAddImageMaskGraph(int plotid, int imageColumn, double x, double y, double width, double height, int Nx, const QString &title, QColor trueColor, QColor falseColor)
{
    if (plotid>=0 && plotid<plots.size()) {

        QFRDRTable::PlotInfo plt=getPlot(plotid);
        QFRDRTable::GraphInfo g;
        g.type=QFRDRTable::gtMaskImage;
        g.xcolumn=imageColumn;
        g.imageX=x;
        g.imageY=y;
        g.imageWidth=width;
        g.imageHeight=height;
        g.imagePixelWidth=Nx;
        g.title=title;
        g.imageTrueColor=trueColor;
        g.imageTrueTransparent=trueColor.alphaF();
        g.imageFalseColor=falseColor;
        g.imageFalseTransparent=falseColor.alphaF();
        plt.graphs.append(g);

        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
     }
}

void QFRDRTable::colgraphAddRGBImageGrph(int plotid, int imageRColumn, int imageGColumn, int imageBColumn, double x, double y, double width, double height, int Nx, const QString &title)
{
    if (plotid>=0 && plotid<plots.size()) {

        QFRDRTable::PlotInfo plt=getPlot(plotid);
        QFRDRTable::GraphInfo g;
        g.type=QFRDRTable::gtRGBImage;
        g.xcolumn=imageRColumn;
        g.xerrorcolumn=imageGColumn;
        g.ycolumn=imageBColumn;
        g.imageX=x;
        g.imageY=y;
        g.imageWidth=width;
        g.imageHeight=height;
        g.imagePixelWidth=Nx;
        g.title=title;
        plt.graphs.append(g);

        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphAddRangeGraph(int plotid, QFRDRTable::Orientation orientation, double rangeStart, double rangeEnd, double rangeCenter, const QString &title, bool invertRange, bool fillrange, bool drawRangeLines, bool drawRangeCenter, QColor centerColor, Qt::PenStyle centerStyle, double centerWidth)
{
    if (plotid>=0 && plotid<plots.size()) {

        QFRDRTable::PlotInfo plt=getPlot(plotid);
        QFRDRTable::GraphInfo g;
        g.type=QFRDRTable::gtHorizontalRange;
        if (orientation==QFRDRTable::cgoVertical) g.type=QFRDRTable::gtVerticalRange;
        g.title=title;
        g.rangeCenter=rangeCenter;
        g.rangeCenterColor=centerColor;
        g.rangeCenterColorTransparent=centerColor.alphaF();
        g.rangeCenterStyle=centerStyle;
        g.rangeCenterWidth=centerWidth;
        g.rangeDrawCenter=drawRangeCenter;
        g.rangeEnd=rangeEnd;
        g.rangeFill=fillrange;
        g.rangeInverted=invertRange;
        g.rangeStart=rangeStart;
        g.drawLine=drawRangeLines;
        plt.graphs.append(g);

        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetImageGraphModifier(int plotid, int graphid, int imageModifierColumn, QFRDRColumnGraphsInterface::ImageModifierMode mode)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            if (plt.graphs[graphid].type==QFRDRTable::gtRGBImage) {
                plt.graphs[graphid].yerrorcolumn=imageModifierColumn;
            } else {
                plt.graphs[graphid].ycolumn=imageModifierColumn;
            }
            plt.graphs[graphid].modifierMode=JKQTPMathImage::ModifierMode((int)mode);
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetImageGraphRange(int plotid, int graphid, QFRDRColumnGraphsInterface::ImageGraphRangeChannel channel, bool autoRange, double min, double max)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            if (channel!=cgtiModifierChannel) {
                plt.graphs[graphid].imageAutoRange=autoRange;
                plt.graphs[graphid].imageMin=autoRange;
                plt.graphs[graphid].imageMax=autoRange;
            }
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetGraphProperty(int plotid, int graphid, const QString &name, const QVariant &value)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].moreProperties[name]=value;
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

QVariant QFRDRTable::colgraphGetGraphProperty(int plotid, int graphid, const QString &name, const QVariant &defaultValue)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            return plt.graphs[graphid].moreProperties.value(name, defaultValue);
        }
    }
    return defaultValue;
}

void QFRDRTable::colgraphAddPlot(const QString &title, const QString &xLabel, const QString &yLabel, bool logX, bool logY)
{
    PlotInfo info;
    info.title=title;
    info.xAxis.label=xLabel;
    info.yAxis.label=yLabel;
    info.xAxis.log=logX;
    info.yAxis.log=logY;
    addPlot(info);
    emitRebuildPlotWidgets();
}

int QFRDRTable::colgraphGetGraphCount(int plotid) const
{
    if (plotid>=0 && plotid<plots.size()) {
        return plots[plotid].graphs.size();
    }
    return 0;
}

int QFRDRTable::colgraphGetPlotCount() const
{
    return getPlotCount();
}

void QFRDRTable::colgraphRemovePlot(int plotid)
{
    deletePlot(plotid);
    emitRebuildPlotWidgets();
}

void QFRDRTable::colgraphRemoveGraph(int plotid, int graphid)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        plt.graphs.removeAt(graphid);
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetGraphLineStyle(int plotid, int graphid, Qt::PenStyle style)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].style=style;
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetGraphLineWidth(int plotid, int graphid, double width)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].linewidth=width;
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetGraphFillStyle(int plotid, int graphid, Qt::BrushStyle style)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].fillStyle=style;
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }

}

void QFRDRTable::colgraphSetGraphColor(int plotid, int graphid, QColor color)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].color=color;
            plt.graphs[graphid].colorTransparent=color.alphaF();
            plt.graphs[graphid].errorColor=plt.graphs[graphid].color.darker();
            plt.graphs[graphid].errorColorTransparent=plt.graphs[graphid].errorColor.alphaF();
            plt.graphs[graphid].fillColor=plt.graphs[graphid].color.lighter();
            plt.graphs[graphid].fillColorTransparent=plt.graphs[graphid].fillColor.alphaF();
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetGraphColor(int plotid, int graphid, QColor color, QColor fillColor)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].color=color;
            plt.graphs[graphid].colorTransparent=color.alphaF();
            plt.graphs[graphid].errorColor=plt.graphs[graphid].color.darker();
            plt.graphs[graphid].errorColorTransparent=plt.graphs[graphid].errorColor.alphaF();
            plt.graphs[graphid].fillColor=fillColor;
            plt.graphs[graphid].fillColorTransparent=plt.graphs[graphid].fillColor.alphaF();
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetGraphColor(int plotid, int graphid, QColor color, QColor fillColor, QColor errorColor)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].color=color;
            plt.graphs[graphid].colorTransparent=color.alphaF();
            plt.graphs[graphid].errorColor=errorColor;
            plt.graphs[graphid].errorColorTransparent=plt.graphs[graphid].errorColor.alphaF();
            plt.graphs[graphid].fillColor=fillColor;
            plt.graphs[graphid].fillColorTransparent=plt.graphs[graphid].fillColor.alphaF();
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetPlotTitle(int plotid, const QString &title)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        plt.title=title;
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

QString QFRDRTable::colgraphGetPlotTitle(int plotid)
{
    if (plotid>=0 && plotid<plots.size()) {
        return getPlot(plotid).title;
    }
    return "";
}


void QFRDRTable::colgraphSetPlotXAxisProps(int plotid, const QString &xLabel, bool logX)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        plt.xAxis.label=xLabel;
        plt.xAxis.log=logX;
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }

}

void QFRDRTable::colgraphSetPlotYAxisProps(int plotid, const QString &yLabel, bool logY)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        plt.yAxis.label=yLabel;
        plt.yAxis.log=logY;
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetPlotXRange(int plotid, double xmin, double xmax)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        plt.xAxis.min=xmin;
        plt.xAxis.max=xmax;
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetPlotYRange(int plotid, double xmin, double xmax)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        plt.yAxis.min=xmin;
        plt.yAxis.max=xmax;
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetGraphTitle(int plotid, int graphid, const QString &title)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].title=title;
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetGrphType(int plotid, int graphid, QFRDRColumnGraphsInterface::ColumnGraphTypes type)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
           colgraphToolsSetGraphtype(plt.graphs[graphid], type);
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetGraphErrorStyle(int plotid, int graphid, QFRDRColumnGraphsInterface::ErrorGraphTypes errorStyle)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].errorStyle=(JKQTPerrorPlotstyle)errorStyle;
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetGraphErrorColor(int plotid, int graphid, QColor errorColor)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].errorColor=errorColor;
            plt.graphs[graphid].errorColorTransparent=errorColor.alphaF();
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetGraphFillColor(int plotid, int graphid, QColor fillColor)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].fillColor=fillColor;
            plt.graphs[graphid].fillColorTransparent=fillColor.alphaF();
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetGraphErrorTransparency(int plotid, int graphid, double errorT)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].errorColorTransparent=errorT;
        }
        plt.graphs[graphid];
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetGraphFillTransparency(int plotid, int graphid, double fillT)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].fillColorTransparent=fillT;
        }
        plt.graphs[graphid];
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetGraphTransparency(int plotid, int graphid, double trans)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].colorTransparent=trans;
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetGraphSymbol(int plotid, int graphid, QFRDRColumnGraphsInterface::ColumnGraphSymbols symbol, double symbolSize)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].symbol=(JKQTPgraphSymbols)symbol;
            plt.graphs[graphid].symbolSize=symbolSize;
        }
        setPlot(plotid, plt);
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
            QVector<double> rv;
            for (int i=0; i<m->model()->rowCount(); i++) rv<<i+1;
            mp.addVariableDoubleVector("rowsvec", rv);
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
            } else if (r.type==qfmpStringVector) {
                if (columnMode) {
                    QVariantList vl;
                    for (int i=0; i<r.strVec.size(); i++) {
                        vl<<r.strVec[i];
                    }
                    return vl;
                } else {
                    result=QVariant(r.asStringAlsoVector());
                }
            } else if (r.type==qfmpBoolVector) {
                if (columnMode) {
                    QVariantList vl;
                    for (int i=0; i<r.boolVec.size(); i++) {
                        vl<<r.boolVec[i];
                    }
                    return vl;
                } else {
                    result=QVariant(r.asBooleanAlsoVector());
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
                    connect(edt, SIGNAL(performRefit(int,int)), tabEdt, SLOT(requestRefit(int,int)));
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
    } else if (f=="QFTABLEXML") {
        datamodel->saveXML(filename);
    }
}



void QFRDRTable::intReadData(QDomElement* e) {
    if (!datamodel) {
        datamodel=new QFTablePluginModel(this);
        datamodel->setUndoEnabled(true);
        datamodel->setVerticalHeaderShowRowNumbers(true);
        datamodel->setDefaultEditValue(DEFAULT_EDITVAL);
        connect(datamodel, SIGNAL(modelReset()), this, SLOT(trawDataChanged()));
        connect(datamodel, SIGNAL(dataChanged( const QModelIndex & , const QModelIndex &  )), this, SLOT(tdataChanged( const QModelIndex & , const QModelIndex &  )));
    }

    datamodel->startMultiUndo();

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
        //qDebug()<<datamodel->getColumnTitles();
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
                            datamodel->setColumnHeaderData(columns-1, role, v);
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

    datamodel->endMultiUndo();
    datamodel->clearUndo();
    //datamodel->addUndoStep();

    if (e) {
        QDomElement te=e->firstChildElement("plots");
        plots.clear();
        if (!te.isNull()) {
            te=te.firstChildElement("plot");
            while (!te.isNull()) {
                PlotInfo plot;
                plot.title=te.attribute("title", tr("graph title"));
                plot.graphAutosize=QStringToBool(te.attribute("autosize", "true"));
                plot.graphWidth=te.attribute("gwidth", "150").toInt();
                plot.graphHeight=te.attribute("gheight", "150").toInt();
                plot.grid=QStringToBool( te.attribute("grid", "true"));
                plot.showKey=QStringToBool( te.attribute("showkey"));
                plot.fontName=te.attribute("fontname", "Arial");
                plot.keyFontSize=CQStringToDouble(te.attribute("keyfontsize", "12"));
                plot.axisFontSize=CQStringToDouble(te.attribute("axisfontsize", "10"));
                plot.axisLabelFontSize=CQStringToDouble(te.attribute("axislabelfontsize", "12"));
                plot.labelFontSize=CQStringToDouble(te.attribute("labelFontSize", "16"));

                plot.keyTransparency=CQStringToDouble(te.attribute("keytransparency", "0.5"));
                plot.keyPosition=String2JKQTPkeyPosition(te.attribute("keyposition", "inside_right"));
                plot.keyLayout=String2JKQTPkeyLayout(te.attribute("keylayout", "one_column"));

                plot.keepDataAspectRatio=QStringToBool( te.attribute("keep_dataaspect", "false"));
                plot.axisAspectRatio=CQStringToDouble(te.attribute("dataaspect", "1"));
                plot.keepAxisAspectRatio=QStringToBool( te.attribute("keep_axisaspect", "false"));
                plot.dataAspectRatio=CQStringToDouble(te.attribute("axisaspect", "1"));

                plot.backgroundColor=QStringToQColor(te.attribute("background_color", "white"));
                plot.gridColor=QStringToQColor(te.attribute("grid_color", "darkgrey"));
                plot.gridStyle=String2QPenStyle(te.attribute("grid_style", "dash"));
                plot.gridWidth=CQStringToDouble(te.attribute("grid_width", "1"));


                plot.xAxis.min=CQStringToDouble(te.attribute("xmin", "0"));
                plot.xAxis.max=CQStringToDouble(te.attribute("xmax", "10"));
                plot.xAxis.label=te.attribute("xlabel", "x");
                plot.xAxis.log=QStringToBool( te.attribute("xlog", "false"));
                plot.xAxis.axis0=QStringToBool( te.attribute("x0axis", "true"));
                plot.xAxis.digits=te.attribute("xdigits", "3").toInt();
                plot.xAxis.minTicks=te.attribute("xmin_ticks", "7").toInt();
                plot.xAxis.minorTicks=te.attribute("xminor_ticks", "1").toInt();
                plot.xAxis.labelPos=String2JKQTPlabelPosition(te.attribute("xlabel_pos", "center"));
                plot.xAxis.labelType=String2JKQTPCAlabelType(te.attribute("xlabel_type", "exponent"));
                plot.xAxis.labelMode1=String2JKQTPCAdrawMode(te.attribute("xlabel_mode1", "all"));
                plot.xAxis.labelMode2=String2JKQTPCAdrawMode(te.attribute("xlabel_mode2", "ticks"));

                plot.xAxis.AutoTicks=QStringToBool( te.attribute("xautoticks", "true"));
                plot.xAxis.AxisInverted=QStringToBool( te.attribute("xaxisinverted", "false"));
                plot.xAxis.TickSpacing=CQStringToDouble(te.attribute("xtickspacing", "10"));
                plot.xAxis.AxisTickWidth=CQStringToDouble(te.attribute("xtickwidth", "1.5"));
                plot.xAxis.AxisMinorTickWidth=CQStringToDouble(te.attribute("xmintickwidth", "1"));
                plot.xAxis.TickInsideLength=CQStringToDouble(te.attribute("xtickilength", "3"));
                plot.xAxis.TickOutsideLength=CQStringToDouble(te.attribute("xtickolength", "3"));
                plot.xAxis.MinorTickInsideLength=CQStringToDouble(te.attribute("xmintickilength", "2"));
                plot.xAxis.MinorTickOutsideLength=CQStringToDouble(te.attribute("xmintickolength", "2"));


                plot.yAxis.min=CQStringToDouble(te.attribute("ymin", "0"));
                plot.yAxis.max=CQStringToDouble(te.attribute("ymax", "10"));
                plot.yAxis.label=te.attribute("ylabel", "y");
                plot.yAxis.log=QStringToBool( te.attribute("ylog", "false"));
                plot.yAxis.axis0=QStringToBool( te.attribute("y0axis", "true"));
                plot.yAxis.minTicks=te.attribute("ymin_ticks", "7").toInt();
                plot.yAxis.minorTicks=te.attribute("yminor_ticks", "1").toInt();
                plot.yAxis.labelPos=String2JKQTPlabelPosition(te.attribute("ylabel_pos", "center"));
                plot.yAxis.labelType=String2JKQTPCAlabelType(te.attribute("ylabel_type", "exponent"));
                plot.yAxis.digits=te.attribute("ydigits", "3").toInt();
                plot.yAxis.labelMode1=String2JKQTPCAdrawMode(te.attribute("ylabel_mode1", "all"));
                plot.yAxis.labelMode2=String2JKQTPCAdrawMode(te.attribute("ylabel_mode2", "ticks"));

                plot.yAxis.AutoTicks=QStringToBool( te.attribute("yautoticks", "true"));
                plot.yAxis.AxisInverted=QStringToBool( te.attribute("yaxisinverted", "false"));
                plot.yAxis.TickSpacing=CQStringToDouble(te.attribute("ytickspacing", "10"));
                plot.yAxis.AxisTickWidth=CQStringToDouble(te.attribute("ytickwidth", "1.5"));
                plot.yAxis.AxisMinorTickWidth=CQStringToDouble(te.attribute("ymintickwidth", "1"));
                plot.yAxis.TickInsideLength=CQStringToDouble(te.attribute("ytickilength", "3"));
                plot.yAxis.TickOutsideLength=CQStringToDouble(te.attribute("ytickolength", "3"));
                plot.yAxis.MinorTickInsideLength=CQStringToDouble(te.attribute("ymintickilength", "2"));
                plot.yAxis.MinorTickOutsideLength=CQStringToDouble(te.attribute("ymintickolength", "2"));


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

                    graph.meancolumn=ge.attribute("meancolumn", "-1").toInt();
                    graph.q75column=ge.attribute("q75column", "-1").toInt();
                    graph.maxcolumn=ge.attribute("maxcolumn", "-1").toInt();

                    graph.linewidth=CQStringToDouble(ge.attribute("linewidth", "1"));
                    graph.symbolSize=CQStringToDouble(ge.attribute("symbolSize", "1"));
                    graph.color=QStringToQColor(ge.attribute("color", "blue"));
                    graph.errorColor=QStringToQColor(ge.attribute("errorcolor", "darkblue"));
                    graph.fillColor=QStringToQColor(ge.attribute("fillcolor", "blue"));
                    //qDebug()<<ge.attribute("type")<<ge.attribute("fillcolor", "blue");
                    graph.style=String2QPenStyle(ge.attribute("style", "solid"));
                    graph.whiskerStyle=String2QPenStyle(ge.attribute("whisker_style", "solid"));
                    graph.fillStyle=String2QBrushStyle(ge.attribute("fill_style", "solid"));
                    graph.symbol=String2JKQTPgraphSymbols(ge.attribute("symbol", "symbol_cross"));
                    graph.errorStyle=String2JKQTPerrorPlotstyle(ge.attribute("errorStyle", "error_none"));
                    graph.colorTransparent=CQStringToDouble(ge.attribute("color_trans", "1"));
                    graph.errorColorTransparent=CQStringToDouble(ge.attribute("errorcolor_trans", "1"));
                    graph.fillColorTransparent=CQStringToDouble(ge.attribute("fillcolor_trans", "0.3"));


                    graph.errorWidth=CQStringToDouble(ge.attribute("error_width", "1"));
                    graph.errorLineStyle=String2QPenStyle(ge.attribute("error_line_style", "solid"));
                    graph.errorBarSize=CQStringToDouble(ge.attribute("error_barsize", "7"));



                    graph.stride=ge.attribute("stride", "1").toInt();
                    graph.strideStart=ge.attribute("stride_start", "1").toInt();
                    graph.isStrided=QStringToBool(ge.attribute("is_strided", "false"));
                    graph.width=CQStringToDouble(ge.attribute("width", "0.9"));
                    graph.shift=CQStringToDouble(ge.attribute("shift", "0"));

                    graph.isDataSelect=QStringToBool(ge.attribute("is_data_select", "false"));
                    graph.dataSelectColumn=ge.attribute("data_select_column", "-1").toInt();;
                    graph.dataSelectOperation=String2DataSelectOperation(ge.attribute("data_select_operation", "=="));
                    graph.dataSelectCompareValue=CQStringToDouble(ge.attribute("data_select_value", "0.0"));


                    graph.imageTrueColor=QStringToQColor(ge.attribute("image_truecolor", "blue"));
                    graph.imageTrueTransparent=CQStringToDouble(ge.attribute("image_truecolor_trans", "0.5"));
                    graph.imageFalseColor=QStringToQColor(ge.attribute("image_falsecolor", "red"));
                    graph.imageFalseTransparent=CQStringToDouble(ge.attribute("image_falsecolor_trans", "1.0"));
                    graph.imagePixelWidth=ge.attribute("image_pixelwidth", "1").toInt();
                    graph.imageX=CQStringToDouble(ge.attribute("image_x", "0"));
                    graph.imageY=CQStringToDouble(ge.attribute("image_y", "0"));
                    graph.imageWidth=CQStringToDouble(ge.attribute("image_width", "1"));
                    graph.imageHeight=CQStringToDouble(ge.attribute("image_height", "1"));
                    graph.imagePalette=JKQTPMathImageColorPalette(ge.attribute("image_palette", "8").toInt());
                    graph.imageMin=CQStringToDouble(ge.attribute("image_min", "0"));
                    graph.imageMax=CQStringToDouble(ge.attribute("image_max", "0"));
                    graph.imageAutoRange=QStringToBool(ge.attribute("image_autorange", "true"));

                    graph.imageColorbarTicklength=CQStringToDouble(ge.attribute("image_colorbar_ticklen", "3"));
                    graph.imageColorbarFontsize=CQStringToDouble(ge.attribute("image_colorbar_fontsize", "7"));
                    graph.imageTicks=ge.attribute("image_ticks", "6").toInt();
                    graph.imageModTicks=ge.attribute("image_modticks", "2").toInt();
                    graph.imageColorbarLabelType=String2JKQTPCAlabelType(te.attribute("image_labeltype", "exponent"));
                    graph.imageColorbarLabelDigits=te.attribute("image_digits", "3").toInt();

                    graph.imageColorbarRight=QStringToBool(ge.attribute("image_colorbarright", "true"));
                    graph.imageColorbarTop=QStringToBool(ge.attribute("image_colorbartop", "false"));
                    graph.colorbarWidth=CQStringToDouble(ge.attribute("image_colorbarwidth", "20"));
                    graph.colorbarRelativeHeight=CQStringToDouble(ge.attribute("image_colorbarrelativeheight", "0.75"));
                    graph.imageLegend=ge.attribute("image_legend", "");
                    graph.imageLegendG=ge.attribute("image_legend_g", "");
                    graph.imageLegendB=ge.attribute("image_legend_b", "");
                    graph.imageLegendMod=ge.attribute("image_legend_mod", "");
                    graph.function=ge.attribute("function", "");
                    graph.functionType=String2GTFunctionType(ge.attribute("functiontype", "string"));
                    graph.modifierMode=JKQTPMathImage::StringToModifierMode(ge.attribute("modifier_mode", "none"));
                    graph.functionParameters=stringToDoubleArray_base64(ge.attribute("fparams", ""));


                    graph.rangeStart=CQStringToDouble(ge.attribute("range_start", "0"));
                    graph.rangeEnd=CQStringToDouble(ge.attribute("range_end", "1"));
                    graph.rangeCenter=CQStringToDouble(ge.attribute("range_center", "0.5"));
                    graph.rangeCenterWidth=CQStringToDouble(ge.attribute("range_cwidth", "0.5"));
                    graph.rangeCenterColor=QStringToQColor(ge.attribute("range_ccolor", "darkred"));
                    graph.rangeCenterColorTransparent=CQStringToDouble(ge.attribute("range_ccolort", "1"));
                    graph.rangeCenterStyle=String2QPenStyle(ge.attribute("range_cstyle", "dash"));
                    graph.rangeInverted=QStringToBool(ge.attribute("range_inverted", "false"));
                    graph.rangeFill=QStringToBool(ge.attribute("range_fill", "true"));
                    graph.rangeDrawCenter=QStringToBool(ge.attribute("range_drawcenter", "true"));

                    graph.errorColorAuto=QStringToBool(ge.attribute("error_color_auto", "true"));
                    graph.fillColorAuto=QStringToBool(ge.attribute("fill_color_auto", "true"));
                    graph.centerColorAuto=QStringToBool(ge.attribute("center_color_auto", "true"));

                    graph.moreProperties.clear();
                    QDomElement gmp=ge.firstChildElement("property");
                    while (!gmp.isNull()) {
                        QString name=gmp.attribute("name");
                        QString type=gmp.attribute("type");
                        QString value=gmp.text();
                        if (!name.isEmpty() && !type.isEmpty()) {
                            graph.moreProperties[name]=getQVariantFromString(type, value);
                        }
                        gmp=gmp.nextSiblingElement("property");
                    }



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
        datamodel->setUndoEnabled(true);
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
                w.writeStartElement("row");
                w.writeAttribute("type", getQVariantType(datamodel->cell(r, c)));
                if (datamodel->cell(r, c).isValid()) {
                    w.writeAttribute("value", getQVariantData(datamodel->cell(r, c)));
                }
                QString ex=datamodel->cellUserRole(QFRDRTable::TableExpressionRole, r, c).toString();
                if (!ex.isEmpty()) w.writeAttribute("expression", ex);
                QString comment=datamodel->cellUserRole(QFRDRTable::TableCommentRole, r, c).toString();
                if (!comment.isEmpty()) w.writeAttribute("comment", comment);
                w.writeEndElement();
            }
            w.writeEndElement();
        }
    }

    w.writeStartElement("plots");
    for (int i=0; i<plots.size(); i++) {
        w.writeStartElement("plot");
        w.writeAttribute("title", plots[i].title);
        w.writeAttribute("autosize", boolToQString(plots[i].graphAutosize));
        w.writeAttribute("gwidth", QString::number(plots[i].graphWidth));
        w.writeAttribute("gheight", QString::number(plots[i].graphHeight));

        w.writeAttribute("showkey", boolToQString(plots[i].showKey));
        w.writeAttribute("grid", boolToQString(plots[i].grid));

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

        w.writeAttribute("xlabel", plots[i].xAxis.label);
        w.writeAttribute("ylabel", plots[i].yAxis.label);
        w.writeAttribute("grid_width", CDoubleToQString(plots[i].gridWidth));
        w.writeAttribute("background_color", QColor2String(plots[i].backgroundColor));
        w.writeAttribute("grid_color", QColor2String(plots[i].gridColor));
        w.writeAttribute("grid_style", QPenStyle2String(plots[i].gridStyle));

        w.writeAttribute("xdigits", QString::number(plots[i].xAxis.digits));
        w.writeAttribute("ydigits", QString::number(plots[i].yAxis.digits));
        w.writeAttribute("xmin_ticks", QString::number(plots[i].xAxis.minTicks));
        w.writeAttribute("xminor_ticks", QString::number(plots[i].xAxis.minorTicks));
        w.writeAttribute("ymin_ticks", QString::number(plots[i].yAxis.minTicks));
        w.writeAttribute("yminor_ticks", QString::number(plots[i].yAxis.minorTicks));
        w.writeAttribute("xlabel_pos", JKQTPlabelPosition2String(plots[i].xAxis.labelPos));
        w.writeAttribute("ylabel_pos", JKQTPlabelPosition2String(plots[i].yAxis.labelPos));
        w.writeAttribute("xlabel_type", JKQTPCAlabelType2String(plots[i].xAxis.labelType));
        w.writeAttribute("ylabel_type", JKQTPCAlabelType2String(plots[i].yAxis.labelType));

        w.writeAttribute("xlabel_mode1", JKQTPCAdrawMode2String(plots[i].xAxis.labelMode1));
        w.writeAttribute("xlabel_mode2", JKQTPCAdrawMode2String(plots[i].xAxis.labelMode2));
        w.writeAttribute("ylabel_mode1", JKQTPCAdrawMode2String(plots[i].yAxis.labelMode1));
        w.writeAttribute("ylabel_mode2", JKQTPCAdrawMode2String(plots[i].yAxis.labelMode2));

        w.writeAttribute("xlog", boolToQString(plots[i].xAxis.log));
        w.writeAttribute("ylog", boolToQString(plots[i].yAxis.log));
        w.writeAttribute("x0axis", boolToQString(plots[i].xAxis.axis0));
        w.writeAttribute("y0axis", boolToQString(plots[i].yAxis.axis0));
        w.writeAttribute("xmin", CDoubleToQString(plots[i].xAxis.min));
        w.writeAttribute("xmax", CDoubleToQString(plots[i].xAxis.max));
        w.writeAttribute("ymin", CDoubleToQString(plots[i].yAxis.min));
        w.writeAttribute("ymax", CDoubleToQString(plots[i].yAxis.max));

        w.writeAttribute("xautoticks", boolToQString(plots[i].xAxis.AutoTicks));
        w.writeAttribute("xaxisinverted", boolToQString(plots[i].xAxis.AxisInverted));
        w.writeAttribute("xtickspacing", CDoubleToQString(plots[i].xAxis.TickSpacing));
        w.writeAttribute("xtickwidth", CDoubleToQString(plots[i].xAxis.AxisTickWidth));
        w.writeAttribute("xmintickwidth", CDoubleToQString(plots[i].xAxis.AxisMinorTickWidth));
        w.writeAttribute("xtickilength", CDoubleToQString(plots[i].xAxis.TickInsideLength));
        w.writeAttribute("xtickolength", CDoubleToQString(plots[i].xAxis.TickOutsideLength));
        w.writeAttribute("xmintickilength", CDoubleToQString(plots[i].xAxis.MinorTickInsideLength));
        w.writeAttribute("xmintickolength", CDoubleToQString(plots[i].xAxis.MinorTickOutsideLength));


        w.writeAttribute("yautoticks", boolToQString(plots[i].yAxis.AutoTicks));
        w.writeAttribute("yaxisinverted", boolToQString(plots[i].yAxis.AxisInverted));
        w.writeAttribute("ytickspacing", CDoubleToQString(plots[i].yAxis.TickSpacing));
        w.writeAttribute("ytickwidth", CDoubleToQString(plots[i].yAxis.AxisTickWidth));
        w.writeAttribute("ymintickwidth", CDoubleToQString(plots[i].yAxis.AxisMinorTickWidth));
        w.writeAttribute("ytickilength", CDoubleToQString(plots[i].yAxis.TickInsideLength));
        w.writeAttribute("ytickolength", CDoubleToQString(plots[i].yAxis.TickOutsideLength));
        w.writeAttribute("ymintickilength", CDoubleToQString(plots[i].yAxis.MinorTickInsideLength));
        w.writeAttribute("ymintickolength", CDoubleToQString(plots[i].yAxis.MinorTickOutsideLength));





        for (int g=0; g<plots[i].graphs.size(); g++) {
            w.writeStartElement("graph");
            w.writeAttribute("type", GraphType2String(plots[i].graphs[g].type));
            w.writeAttribute("title", plots[i].graphs[g].title);
            w.writeAttribute("drawline", boolToQString( plots[i].graphs[g].drawLine));
            w.writeAttribute("xcolumn", QString::number(plots[i].graphs[g].xcolumn));
            w.writeAttribute("ycolumn", QString::number(plots[i].graphs[g].ycolumn));
            w.writeAttribute("xerrorcolumn", QString::number(plots[i].graphs[g].xerrorcolumn));
            w.writeAttribute("yerrorcolumn", QString::number(plots[i].graphs[g].yerrorcolumn));

            w.writeAttribute("meancolumn", QString::number(plots[i].graphs[g].meancolumn));
            w.writeAttribute("q75column", QString::number(plots[i].graphs[g].q75column));
            w.writeAttribute("maxcolumn", QString::number(plots[i].graphs[g].maxcolumn));
            w.writeAttribute("width", CDoubleToQString(plots[i].graphs[g].width));
            w.writeAttribute("shift", CDoubleToQString(plots[i].graphs[g].shift));

            w.writeAttribute("linewidth", CDoubleToQString(plots[i].graphs[g].linewidth));
            w.writeAttribute("symbolSize", CDoubleToQString(plots[i].graphs[g].symbolSize));
            w.writeAttribute("style", QPenStyle2String(plots[i].graphs[g].style));
            w.writeAttribute("whisker_style", QPenStyle2String(plots[i].graphs[g].whiskerStyle));
            w.writeAttribute("fill_style", QBrushStyle2String(plots[i].graphs[g].fillStyle));
            w.writeAttribute("color", QColor2String(plots[i].graphs[g].color));
            w.writeAttribute("errorcolor", QColor2String(plots[i].graphs[g].errorColor));
            w.writeAttribute("fillcolor", QColor2String(plots[i].graphs[g].fillColor));
            w.writeAttribute("symbol", JKQTPgraphSymbols2String(plots[i].graphs[g].symbol));
            w.writeAttribute("errorStyle", JKQTPerrorPlotstyle2String(plots[i].graphs[g].errorStyle));
            w.writeAttribute("color_trans", CDoubleToQString(plots[i].graphs[g].colorTransparent));
            w.writeAttribute("errorcolor_trans", CDoubleToQString(plots[i].graphs[g].errorColorTransparent));
            w.writeAttribute("fillcolor_trans", CDoubleToQString(plots[i].graphs[g].fillColorTransparent));


            w.writeAttribute("image_truecolor", QColor2String(plots[i].graphs[g].imageTrueColor));
            w.writeAttribute("image_truecolor_trans", CDoubleToQString(plots[i].graphs[g].imageTrueTransparent));
            w.writeAttribute("image_falsecolor", QColor2String(plots[i].graphs[g].imageFalseColor));
            w.writeAttribute("image_falsecolor_trans", CDoubleToQString(plots[i].graphs[g].imageFalseTransparent));
            w.writeAttribute("image_pixelwidth", QString::number(plots[i].graphs[g].imagePixelWidth));
            w.writeAttribute("image_x", CDoubleToQString(plots[i].graphs[g].imageX));
            w.writeAttribute("image_y", CDoubleToQString(plots[i].graphs[g].imageY));
            w.writeAttribute("image_width", CDoubleToQString(plots[i].graphs[g].imageWidth));
            w.writeAttribute("image_height", CDoubleToQString(plots[i].graphs[g].imageHeight));
            w.writeAttribute("image_palette", QString::number(plots[i].graphs[g].imagePalette));
            w.writeAttribute("image_min", CDoubleToQString(plots[i].graphs[g].imageMin));
            w.writeAttribute("image_max", CDoubleToQString(plots[i].graphs[g].imageMax));

            w.writeAttribute("image_colorbar_ticklen", CDoubleToQString(plots[i].graphs[g].imageColorbarTicklength));
            w.writeAttribute("image_colorbar_fontsize", CDoubleToQString(plots[i].graphs[g].imageColorbarFontsize));
            w.writeAttribute("image_ticks", QString::number(plots[i].graphs[g].imageTicks));
            w.writeAttribute("image_modticks", QString::number(plots[i].graphs[g].imageModTicks));
            w.writeAttribute("image_labeltype", JKQTPCAlabelType2String(plots[i].graphs[g].imageColorbarLabelType));
            w.writeAttribute("image_digits", QString::number(plots[i].graphs[g].imageColorbarLabelDigits));




            w.writeAttribute("image_autorange", boolToQString(plots[i].graphs[g].imageAutoRange));

            w.writeAttribute("image_colorbarright", boolToQString(plots[i].graphs[g].imageColorbarRight));
            w.writeAttribute("image_colorbartop", boolToQString(plots[i].graphs[g].imageColorbarTop));
            w.writeAttribute("image_colorbarwidth", QString::number(plots[i].graphs[g].colorbarWidth));
            w.writeAttribute("image_colorbarrelativeheight", CDoubleToQString(plots[i].graphs[g].colorbarRelativeHeight));
            w.writeAttribute("image_legend", plots[i].graphs[g].imageLegend);
            w.writeAttribute("image_legend_g", plots[i].graphs[g].imageLegendG);
            w.writeAttribute("image_legend_b", plots[i].graphs[g].imageLegendB);
            w.writeAttribute("image_legend_mod", plots[i].graphs[g].imageLegendMod);
            w.writeAttribute("function", plots[i].graphs[g].function);
            w.writeAttribute("functiontype", GTFunctionType2String(plots[i].graphs[g].functionType));
            w.writeAttribute("fparams", doubleArrayToString_base64(plots[i].graphs[g].functionParameters));
            w.writeAttribute("stride", QString::number(plots[i].graphs[g].stride));
            w.writeAttribute("stride_start", QString::number(plots[i].graphs[g].strideStart));
            w.writeAttribute("is_strided", boolToQString(plots[i].graphs[g].isStrided));
            w.writeAttribute("modifier_mode", JKQTPMathImage::ModifierModeToString(plots[i].graphs[g].modifierMode));

            w.writeAttribute("is_data_select", boolToQString(plots[i].graphs[g].isDataSelect));
            w.writeAttribute("data_select_column", QString::number(plots[i].graphs[g].dataSelectColumn));
            w.writeAttribute("data_select_operation", DataSelectOperation2String(plots[i].graphs[g].dataSelectOperation));
            w.writeAttribute("data_select_value", CDoubleToQString(plots[i].graphs[g].dataSelectCompareValue));



            w.writeAttribute("error_width", CDoubleToQString(plots[i].graphs[g].errorWidth));
            w.writeAttribute("error_line_style", QPenStyle2String(plots[i].graphs[g].errorLineStyle));
            w.writeAttribute("error_barsize", CDoubleToQString(plots[i].graphs[g].errorBarSize));



            w.writeAttribute("range_start", CDoubleToQString(plots[i].graphs[g].rangeStart));
            w.writeAttribute("range_end", CDoubleToQString(plots[i].graphs[g].rangeEnd));
            w.writeAttribute("range_center", CDoubleToQString(plots[i].graphs[g].rangeCenter));
            w.writeAttribute("range_cwidth", CDoubleToQString(plots[i].graphs[g].rangeCenterWidth));
            w.writeAttribute("range_ccolort", CDoubleToQString(plots[i].graphs[g].rangeCenterColorTransparent));

            w.writeAttribute("range_ccolor", QColor2String(plots[i].graphs[g].rangeCenterColor));
            w.writeAttribute("range_cstyle", QPenStyle2String(plots[i].graphs[g].rangeCenterStyle));

            w.writeAttribute("range_inverted", boolToQString( plots[i].graphs[g].rangeInverted));
            w.writeAttribute("range_fill", boolToQString( plots[i].graphs[g].rangeFill));
            w.writeAttribute("range_drawcenter", boolToQString( plots[i].graphs[g].rangeDrawCenter));


            w.writeAttribute("error_color_auto", boolToQString( plots[i].graphs[g].errorColorAuto));
            w.writeAttribute("fill_color_auto", boolToQString( plots[i].graphs[g].fillColorAuto));
            w.writeAttribute("center_color_auto", boolToQString( plots[i].graphs[g].centerColorAuto));



            QMapIterator<QString, QVariant> mit(plots[i].graphs[g].moreProperties);
            while (mit.hasNext()) {
                 mit.next();
                 w.writeStartElement("property");
                 w.writeAttribute("name", mit.key());
                 w.writeAttribute("type", getQVariantType(mit.value()));
                 w.writeCDATA(mit.value().toString());
                 w.writeEndElement();
            }

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

inline int QFRDRTableOffsetIfLarger(int value, int condition, int offset) {
    if (value<0) return value;
    if (value>=condition) return value+offset;
    return value;
}

void QFRDRTable::columnsInserted(int start, int count, bool emitRebuild)
{
    if (!datamodel) return;

    for (int ip=0; ip<plots.size(); ip++) {
        PlotInfo& p=plots[ip];
        for (int ig=0; ig<p.graphs.size(); ig++) {
            GraphInfo& g=p.graphs[ig];
            g.xcolumn=QFRDRTableOffsetIfLarger(g.xcolumn, start, count);
            g.ycolumn=QFRDRTableOffsetIfLarger(g.ycolumn, start, count);
            g.xerrorcolumn=QFRDRTableOffsetIfLarger(g.xerrorcolumn, start, count);
            g.yerrorcolumn=QFRDRTableOffsetIfLarger(g.yerrorcolumn, start, count);
            g.meancolumn=QFRDRTableOffsetIfLarger(g.meancolumn, start, count);
            g.q75column=QFRDRTableOffsetIfLarger(g.q75column, start, count);
            g.maxcolumn=QFRDRTableOffsetIfLarger(g.maxcolumn, start, count);
            g.dataSelectColumn=QFRDRTableOffsetIfLarger(g.dataSelectColumn, start, count);
        }
    }

    if (emitRebuild&&datamodel->getDoEmitSignals()) emitRebuildPlotWidgets();
}

inline int QFRDRTableOffsetIfLargerM1IfInRange(int value, int start, int count) {
    if (value<0) return value;
    if (value>=start) {
        if (value<start+count) return -1;
        else return value-count;
    }
    return value;
}

void QFRDRTable::columnsRemoved(int start, int count, bool emitRebuild)
{
    if (!datamodel) return;

    for (int ip=0; ip<plots.size(); ip++) {
        PlotInfo& p=plots[ip];
        for (int ig=0; ig<p.graphs.size(); ig++) {
            GraphInfo& g=p.graphs[ig];
            g.xcolumn=QFRDRTableOffsetIfLargerM1IfInRange(g.xcolumn, start, count);
            g.ycolumn=QFRDRTableOffsetIfLargerM1IfInRange(g.ycolumn, start, count);
            g.xerrorcolumn=QFRDRTableOffsetIfLargerM1IfInRange(g.xerrorcolumn, start, count);
            g.yerrorcolumn=QFRDRTableOffsetIfLargerM1IfInRange(g.yerrorcolumn, start, count);
            g.meancolumn=QFRDRTableOffsetIfLargerM1IfInRange(g.meancolumn, start, count);
            g.q75column=QFRDRTableOffsetIfLargerM1IfInRange(g.q75column, start, count);
            g.maxcolumn=QFRDRTableOffsetIfLargerM1IfInRange(g.maxcolumn, start, count);
        }
    }

    if (emitRebuild&&datamodel->getDoEmitSignals()) emitRebuildPlotWidgets();
}



