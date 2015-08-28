/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "qfrdrtable.h"
#include "qfdlgcsvparameters.h"
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
    dataSortOrder=0;
    titleShow=true;
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
    symbolLineWidth=1;
    errorStyle=JKQTPnoError;
    drawLine=true;
    errorStyle=JKQTPerrorBars;

    errorWidth=1;
    errorLineStyle=Qt::SolidLine;
    errorBarSize=7;

    xerrorsymmetric=true;
    yerrorsymmetric=true;


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
    subfunction=-1;
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
    dataSelect1Column=-1;
    dataSelect1Operation=dsoEquals;
    dataSelect1CompareValue=0;
    dataSelect1CompareValue2=1;

    dataSelectLogic12=dsoNone;
    dataSelect2Column=-1;
    dataSelect2Operation=dsoEquals;
    dataSelect2CompareValue=0;
    dataSelect2CompareValue2=1;

    dataSelectLogic23=dsoNone;
    dataSelect3Column=-1;
    dataSelect3Operation=dsoEquals;
    dataSelect3CompareValue=0;
    dataSelect3CompareValue2=1;

    offset=0;
    xerrorcolumnlower=-1;
    yerrorcolumnlower=-1;
}

QFRDRTable::AxisInfo::AxisInfo()
{
    label="x";
    labelAngel=0;
    log=false;
    min=0;
    max=10;
    axisFontSize=8;
    axisLabelFontSize=10;
    axis0=false;
    columnNamedTickNames=-1;
    columnNamedTickValues=-2;
    TickSpacingLog=10;
    minorTickLabelsOnlyDigit=false;
    LinTicksForLogAxis=false;
    minorTickLabels=false;

     labelPos=JKQTPlabelCenter;
     labelType=JKQTPCALTexponent;
     labelMode1=JKQTPCADMcomplete;
     labelMode2=JKQTPCADMline;
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

     axis1LineWidth=1.5;
     zeroAxisLineWidth=1.5;
}

QFRDRTable::PlotInfo::PlotInfo()
{
    showTitle=true;
    graphAutosize=true;
    graphWidth=120;
    graphHeight=70;
    title="";
    xAxis=AxisInfo();
    yAxis=AxisInfo();
    xAxis.label="x";
    yAxis.label="y";
    showKey=true;
    grid=true;
    gridMinor=false;

    gridMinorX=true;
    gridMinorY=true;
    gridMajorX=true;
    gridMajorY=true;

    QStringList fdb=QFontDatabase().families();
    fontName=QApplication::font().family();
    if (fdb.contains("DejaVu Sans")) fontName="DejaVu Sans";
    else if (fdb.contains("Arial")) fontName="Arial";
    else if (fdb.contains("Helvetica")) fontName="Helvetica";
    keyFontSize=10;
    axisFontSize=8;
    axisMinorFontSize=6;
    axisLabelFontSize=10;
    labelFontSize=12;
    keyTransparency=0.5;
    keyPosition=JKQTPkeyInsideTopRight;
    keyLayout=JKQTPkeyLayoutOneColumn;
    keepAxisAspectRatio=false;
    keepDataAspectRatio=false;
    dataAspectRatio=1;
    axisAspectRatio=1;

     keyBoxLineWidth=1;
     keyBackgroundColor=QColor("white");
     keyBox=true;
     keyLineColor=QColor("black");;

     backgroundColor=QColor("white");
     gridColor=QColor("darkgrey");
     gridStyle=Qt::DashLine;
     gridWidth=0.5;
     gridColorMinor=QColor("darkgrey");
     gridStyleMinor=Qt::DotLine;
     gridWidthMinor=0.3;


     keyXMargin=0.5;
     keyYMargin=0.5;
     keyXOffset=0.5;
     keyYOffset=0.5;
     keyXSeparation=0.75;
     keyYSeparation=0.75;
     key_line_length=3;
}







QFRDRTable::QFRDRTable(QFProject* parent/*, QString name, QString inputFile*/):
    QFRawDataRecord(parent), QFFitAlgorithmParameterStorage(parent)//, name, QStringList(inputFile))
{
    qRegisterMetaType<QFRDRTable::GraphDataSelection>("QFRDRTable::GraphDataSelection");

    emitColGraphChangedSignals=true;
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

QVariant QFRDRTable::getModelData(quint32 row, quint32 column) {
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

bool QFRDRTable::showNextPreviousOfSameRoleButton() const
{
    return false;
}



QVariant QFRDRTable::tableGetData(quint32 row, quint32 column) const
{
    if (datamodel) return datamodel->cell(row, column);
    else return QVariant();
}

void QFRDRTable::tableSetData(quint32 row, quint32 column, const QVariant &data) {
    //qDebug()<<"tableSetData("<<row<<", "<<column<<",    "<<data<<")";
    if (datamodel)  {
        datamodel->setCellCreate(row, column, data);        
    }
}

void QFRDRTable::tableSetColumnData(quint32 column, const QList<QVariant> &data)
{
    if (datamodel)  {
        datamodel->setColumnCreate(column, data);
    }
}

QList<QVariant> QFRDRTable::tableGetColumnData(quint32 column) const
{
    if (datamodel)  {
        return datamodel->getColumnData(column);
    }
    return QList<QVariant>();
}

void QFRDRTable::tableSetColumnDataAsDouble(quint32 column, const QVector<double> &data)
{
    if (datamodel)  {
        datamodel->setColumnCreate(column, data);
    }
}

QVector<double> QFRDRTable::tableGetColumnDataAsDouble(int column) const
{
    if (datamodel)  {
        return datamodel->getColumnDataAsNumbers(column);
    }
    return QVector<double>();
}

bool QFRDRTable::tableGetDoEmitSignals() const
{
    if (datamodel)  {
        return datamodel->getDoEmitSignals();
    }
    return true;
}

void QFRDRTable::tableSetDoEmitSignals(bool doEmit)
{
    if (datamodel)  {
        if (doEmit) datamodel->enableSignals(true);
        else datamodel->disableSignals();
    }
}

void QFRDRTable::tableSetPreEvaluationExpression(const QString &expression)
{
    setParserPreScript(expression);
}

QString QFRDRTable::tableGetPreEvaluationExpression() const
{
    return getParserPreScript();
}

bool QFRDRTable::colgraphGetDoEmitSignals() const
{
    return emitColGraphChangedSignals;
}

void QFRDRTable::colgraphSetDoEmitSignals(bool doEmit)
{
    emitColGraphChangedSignals=doEmit;
    if (doEmit) emitRebuildPlotWidgets();
}

int QFRDRTable::colgraphAddBoxPlot(int plotid, QFRDRColumnGraphsInterface::Orientation orientation, int columnX, int columnMin, int columnQ25, int columnMedian, int columnMean, int columnQ75, int columnMax, const QString &title)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        QFRDRTable::GraphInfo g;
        g.xcolumn=columnX;
        g.xerrorcolumn=columnMin;
        g.ycolumn=columnQ25;
        g.yerrorcolumn=columnMedian;
        g.meancolumn=columnMean;
        g.q75column=columnQ75;
        g.maxcolumn=columnMax;
        g.color=autocolors.value((plt.graphs.size()-1)%autocolors.size(), QColor("red"));
        g.errorColor=g.color.darker();
        g.fillColor=g.color.lighter();
        g.type=gtBoxplotY;
        if (orientation==QFRDRColumnGraphsInterface::cgoVertical) g.type=gtBoxplotX;
        g.title=title;
        plt.graphs.append(g);

        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
        return plt.graphs.size()-1;


    }
    return -1;

}



void QFRDRTable::tableSetColumnTitle(quint32 column, const QString &data)
{
    //qDebug()<<"tableSetColumnTitle("<<column<<",    "<<data<<")   datamodel<<"<<datamodel;
    if (datamodel)  {
        datamodel->setColumnTitleCreate(column, data);
    }


}

QString QFRDRTable::tableGetColumnTitle(quint32 column) const
{
    if (datamodel) return datamodel->getColumnTitles().value(column, "");
    else return QString("");
}

bool QFRDRTable::tableSupportsExpressions() const
{
    return true;
}

void QFRDRTable::tableSetExpression(quint32 row, quint32 column, const QString &expression)
{
    if (datamodel)  {
        datamodel->setData(datamodel->index(row, column), expression, QFRDRTable::TableExpressionRole);
    }

}

void QFRDRTable::tableSetColumnExpression(quint32 column, const QString &expression)
{
    if (datamodel)  {
        datamodel->setColumnHeaderData(column, QFRDRTable::ColumnExpressionRole, expression);
    }
}

void QFRDRTable::tableSetColumnComment(quint32 column, const QString &comment)
{
    if (datamodel)  {
        datamodel->setColumnHeaderData(column, QFRDRTable::ColumnCommentRole, comment);
    }
}

void QFRDRTable::tableSetComment(quint32 row, quint32 column, const QString &comment)
{
    if (datamodel)  {
        datamodel->setData(datamodel->index(row, column), comment, QFRDRTable::TableCommentRole);
    }
}

QString QFRDRTable::tableGetExpression(quint32 row, quint32 column) const
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
        case QFRDRColumnGraphsInterface::cgtVerticalRange:
            g.type=gtVerticalRange;
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
        case QFRDRColumnGraphsInterface::cgtBoxPlot:
            g.type=gtBoxplotY;
            break;
        case QFRDRColumnGraphsInterface::cgtParametrizedScatter:
            g.type=gtParametrizedScatter;
            g.drawLine=true;
            g.symbol=JKQTPnoSymbol;
            break;
    }
}

int QFRDRTable::colgraphAddGraph(int plotid, int columnX, int columnY, QFRDRColumnGraphsInterface::ColumnGraphTypes type, const QString &title)
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
        return plt.graphs.size()-1;
    }
    return -1;
}


int QFRDRTable::colgraphAddParametrizedScatterGraph(int plotid, int columnX, int columnY, int columnP, const QString &title, QFRDRColumnGraphsInterface::ParametrizationType type, bool colorbarVisible, const QString& colorbarLabel)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        QFRDRTable::GraphInfo g;
        g.xcolumn=columnX;
        g.ycolumn=columnY;

        colgraphToolsSetGraphtype(g, QFRDRColumnGraphsInterface::cgtParametrizedScatter);
        g.meancolumn=columnP;
        if (type==QFRDRColumnGraphsInterface::cgptColorLines) {
            g.meancolumn=columnP;
            g.drawLine=true;
            g.symbol=JKQTPnoSymbol;
        } else if (type==QFRDRColumnGraphsInterface::cgptColorSymbols) {
            g.meancolumn=columnP;
            g.drawLine=false;
            g.symbol=JKQTPfilledCircle;
        } else if (type==QFRDRColumnGraphsInterface::cgptSize) {
            g.q75column=columnP;
            g.drawLine=false;
            g.symbol=JKQTPfilledCircle;
        }

        g.color=autocolors.value((plt.graphs.size()-1)%autocolors.size(), QColor("red"));
        g.errorColor=g.color.darker();
        g.fillColor=g.color.lighter();
        g.title=title;
        g.imageColorbarRight=colorbarVisible;
        g.imageColorbarTop=false;
        g.imageLegend=colorbarLabel;
        plt.graphs.append(g);

        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
        return plt.graphs.size()-1;
    }
    return -1;
}

int QFRDRTable::colgraphAddFunctionGraph(int plotid, const QString &expression, QFRDRColumnGraphsInterface::ColumnGraphTypes type, const QString &title, int columnParam)
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
        return plt.graphs.size()-1;
    }
    return -1;
}

int QFRDRTable::colgraphAddFunctionGraph(int plotid, const QString &expression, QFRDRColumnGraphsInterface::ColumnGraphTypes type, const QString &title, const QVector<double> &params)
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

        return plt.graphs.size()-1;
    }
    return -1;
}

void QFRDRTable::colgraphSetFunctionGraph(int plotid, int graphid, const QString &expression, QFRDRColumnGraphsInterface::ColumnGraphTypes type, const QString &title, int columnParam)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            QFRDRTable::GraphInfo& g=plt.graphs[graphid];
            g.ycolumn=columnParam;
            //g.color=autocolors.value((plt.graphs.size()-1)%autocolors.size(), QColor("red"));
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
            //g.color=autocolors.value((plt.graphs.size()-1)%autocolors.size(), QColor("red"));
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

int QFRDRTable::colgraphAddErrorGraph(int plotid, int columnX, int columnXError, int columnY, int columnYError, QFRDRColumnGraphsInterface::ColumnGraphTypes type, const QString &title, ErrorGraphTypes errorStyle)
{
    if (plotid>=0 && plotid<plots.size()) {

        colgraphAddGraph(plotid, columnX, columnY, type, title);

        QFRDRTable::PlotInfo plt=getPlot(plotid);
        plt.graphs.last().xerrorcolumn=columnXError;
        plt.graphs.last().yerrorcolumn=columnYError;
        plt.graphs.last().errorStyle=(JKQTPerrorPlotstyle)errorStyle;
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
        return plt.graphs.size()-1;
    }
    return -1;

}

void QFRDRTable::colgraphSetErrorGraphProperties(int plotid, int graphid, int columnXError, int columnYError, QFRDRColumnGraphsInterface::ErrorGraphTypes errorStyle)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].xerrorcolumn=columnXError;
            plt.graphs[graphid].yerrorcolumn=columnYError;
            plt.graphs[graphid].errorStyle=(JKQTPerrorPlotstyle)errorStyle;
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetErrorGraphErrorColumnX(int plotid, int graphid, int columnXError)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].xerrorcolumn=columnXError;
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetErrorGraphErrorColumnY(int plotid, int graphid, int columnYError)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].yerrorcolumn=columnYError;
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }}

void QFRDRTable::colgraphSetErrorGraphStyle(int plotid, int graphid, QFRDRColumnGraphsInterface::ErrorGraphTypes errorStyle)
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

void QFRDRTable::colgraphSetErrorGraphErrorColumnXAsymmetric(int plotid, int graphid, bool xErrorAsyymetric, int columnXErrorAsymmetric)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].xerrorcolumnlower=columnXErrorAsymmetric;
            plt.graphs[graphid].xerrorsymmetric=!xErrorAsyymetric;
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetErrorGraphErrorColumnYAsymmetric(int plotid, int graphid, bool yErrorAsyymetric, int columnYErrorAsymmetric)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].yerrorcolumnlower=columnYErrorAsymmetric;
            plt.graphs[graphid].yerrorsymmetric=!yErrorAsyymetric;
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

int QFRDRTable::colgraphAddImageGraph(int plotid, int imageColumn, QFRDRColumnGraphsInterface::ImageColorPalette palette, double x, double y, double width, double height, int Nx, const QString &title)
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
        return plt.graphs.size()-1;
    }
    return -1;
}

int QFRDRTable::colgraphAddImageMaskGraph(int plotid, int imageColumn, double x, double y, double width, double height, int Nx, const QString &title, QColor trueColor, QColor falseColor)
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
        return plt.graphs.size()-1;
    }
    return -1;
}

int QFRDRTable::colgraphAddRGBImageGrph(int plotid, int imageRColumn, int imageGColumn, int imageBColumn, double x, double y, double width, double height, int Nx, const QString &title)
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
        return plt.graphs.size()-1;
    }
    return -1;
}

int QFRDRTable::colgraphAddRangeGraph(int plotid, QFRDRTable::Orientation orientation, double rangeStart, double rangeEnd, double rangeCenter, const QString &title, bool invertRange, bool fillrange, bool drawRangeLines, bool drawRangeCenter, QColor centerColor, Qt::PenStyle centerStyle, double centerWidth)
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
        return plt.graphs.size()-1;
    }
    return -1;
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

void QFRDRTable::colgraphSetImageGraphRange(int plotid, int graphid, QFRDRColumnGraphsInterface::ImageGraphRangeChannel channel, bool autoRange, double /*min*/, double /*max*/)
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

int QFRDRTable::colgraphAddPlot(const QString &title, const QString &xLabel, const QString &yLabel, bool logX, bool logY)
{
    PlotInfo info;
    info.title=title;
    info.xAxis.label=xLabel;
    info.yAxis.label=yLabel;
    info.xAxis.log=logX;
    info.yAxis.log=logY;
    addPlot(info);
    emitRebuildPlotWidgets();
    return plots.size()-1;
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

void QFRDRTable::colgraphSetGraphWhiskerLineStyle(int plotid, int graphid, Qt::PenStyle style)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].whiskerStyle=style;
        }
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

void QFRDRTable::colgraphSetGraphWidth(int plotid, int graphid, double width)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].width=width;
        }
        setPlot(plotid, plt);
        emitRebuildPlotWidgets();
    }
}

void QFRDRTable::colgraphSetGraphShift(int plotid, int graphid, double shift)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].shift=shift;
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

void QFRDRTable::colgraphSetGraphType(int plotid, int graphid, QFRDRColumnGraphsInterface::ColumnGraphTypes type)
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

void QFRDRTable::colgraphSetColorPalette(int plotid, int graphid, QFRDRColumnGraphsInterface::ImageColorPalette palette)
{
    if (plotid>=0 && plotid<plots.size()) {
        QFRDRTable::PlotInfo plt=getPlot(plotid);
        if (graphid>=0 && graphid<plt.graphs.size()) {
            plt.graphs[graphid].imagePalette=JKQTPMathImageColorPalette((int)palette);
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


QString QFRDRTable::colgraphGetPlotXAxisLabel(int plotid)
{
    if (plotid>=0 && plotid<plots.size()) {
        return getPlot(plotid).xAxis.label;
    }
    return QString();
}

QString QFRDRTable::colgraphGetPlotYAxisLabel(int plotid)
{
    if (plotid>=0 && plotid<plots.size()) {
        return getPlot(plotid).xAxis.label;
    }
    return QString();
}

bool QFRDRTable::colgraphGetPlotXAxisLog(int plotid)
{
    if (plotid>=0 && plotid<plots.size()) {
        return getPlot(plotid).yAxis.log;
    }
    return false;
}

bool QFRDRTable::colgraphGetPlotYAxisLog(int plotid)
{
    if (plotid>=0 && plotid<plots.size()) {
        return getPlot(plotid).xAxis.log;
    }
    return false;
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
            mp.addVariableDouble("thisrdr", getID());
            mp.addVariableDouble("col", cell.column()+1);
            mp.addVariableDouble("rows", m->model()->rowCount());
            mp.addVariableDouble("columns", m->model()->columnCount());
            QVector<double> rv;
            for (int i=0; i<m->model()->rowCount(); i++) rv<<i+1;
            mp.addVariableDoubleVector("rowsvec", rv);
        } else {
            mp.addVariableDouble("row", cell.row()+1);
            mp.addVariableDouble("thisrdr", getID());
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
    if (!emittedRebuildPlotWidgets && emitColGraphChangedSignals) emit rebuildPlotWidgets();
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
                    connect(edt, SIGNAL(performFit(int,int,int,int,int,QString, QFRDRTable::GraphDataSelection ,bool,bool)), tabEdt, SLOT(requestFit(int,int,int,int,int,QString, QFRDRTable::GraphDataSelection ,bool,bool)));
                    connect(edt, SIGNAL(performRegression(int,int,int,int,int, QFRDRTable::GraphDataSelection ,bool,bool)), tabEdt, SLOT(requestRegression(int,int,int,int,int, QFRDRTable::GraphDataSelection ,bool,bool)));
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
    if (f=="QFTABLEXML") {
        datamodel->saveXML(filename);
    } else {
        QRegExp rx("F(\\d+)");
        if (rx.indexIn(f)>=0) {
            int fo=rx.cap(1).toInt();
            QStringList cols, rows;
            QList<QList<QVariant> > dat=datamodel->getDataTable(cols, rows);
            //qDebug()<<dat.size()<<dat.value(0).size()<<"\n"<<cols<<"\n"<<rows;
            QFDataExportHandler::save(dat, fo, filename, cols, rows);
        }
    }
}
void QFRDRTable::readGraphInfo(GraphInfo& graph, QDomElement ge) {
    graph.type=String2GraphType(ge.attribute("type"));
    graph.title=ge.attribute("title");
    graph.titleShow= QStringToBool(ge.attribute("showtitle", "true"));
    graph.drawLine= QStringToBool(ge.attribute("drawline", "true"));
    graph.xcolumn=ge.attribute("xcolumn", "-1").toInt();
    graph.ycolumn=ge.attribute("ycolumn", "-1").toInt();
    graph.xerrorcolumn=ge.attribute("xerrorcolumn", "-1").toInt();
    graph.yerrorcolumn=ge.attribute("yerrorcolumn", "-1").toInt();

    graph.meancolumn=ge.attribute("meancolumn", "-1").toInt();
    graph.q75column=ge.attribute("q75column", "-1").toInt();
    graph.maxcolumn=ge.attribute("maxcolumn", "-1").toInt();

    graph.linewidth=CQStringToDouble(ge.attribute("linewidth", "1"));
    graph.symbolSize=CQStringToDouble(ge.attribute("symbolSize", "6"));
    graph.symbolLineWidth=CQStringToDouble(ge.attribute("symbolLineWidth", "1"));
    graph.color=QStringToQColor(ge.attribute("color", "blue"));
    graph.errorColor=QStringToQColor(ge.attribute("errorcolor", "darkblue"));
    graph.fillColor=QStringToQColor(ge.attribute("fillcolor", "blue"));
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

    graph.dataSortOrder=ge.attribute("data_sort_order", "0").toInt();


    graph.stride=ge.attribute("stride", "1").toInt();
    graph.strideStart=ge.attribute("stride_start", "1").toInt();
    graph.isStrided=QStringToBool(ge.attribute("is_strided", "false"));
    graph.width=CQStringToDouble(ge.attribute("width", "0.9"));
    graph.shift=CQStringToDouble(ge.attribute("shift", "0"));

    graph.isDataSelect=QStringToBool(ge.attribute("is_data_select", "false"));
    graph.dataSelect1Column=ge.attribute("data_select_column", "-1").toInt();;
    graph.dataSelect1Operation=String2DataSelectOperation(ge.attribute("data_select_operation", "=="));
    graph.dataSelect1CompareValue=CQStringToDouble(ge.attribute("data_select_value", "0.0"));
    graph.dataSelect1CompareValue2=CQStringToDouble(ge.attribute("data_select_value2", "1.0"));

    graph.dataSelectLogic12=String2DataSelectLogicOperation(ge.attribute("data_select_logic12", "NONE"));
    graph.dataSelectLogic23=String2DataSelectLogicOperation(ge.attribute("data_select_logic23", "NONE"));


    graph.dataSelect2Column=ge.attribute("data_select2_column", "-1").toInt();;
    graph.dataSelect2Operation=String2DataSelectOperation(ge.attribute("data_select2_operation", "=="));
    graph.dataSelect2CompareValue=CQStringToDouble(ge.attribute("data_select2_value", "0.0"));
    graph.dataSelect2CompareValue2=CQStringToDouble(ge.attribute("data_select2_value2", "1.0"));

    graph.dataSelect3Column=ge.attribute("data_select3_column", "-1").toInt();;
    graph.dataSelect3Operation=String2DataSelectOperation(ge.attribute("data_select3_operation", "=="));
    graph.dataSelect3CompareValue=CQStringToDouble(ge.attribute("data_select3_value", "0.0"));
    graph.dataSelect3CompareValue2=CQStringToDouble(ge.attribute("data_select3_value2", "1.0"));


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
    graph.imageColorbarLabelType=String2JKQTPCAlabelType(ge.attribute("image_labeltype", "exponent"));
    graph.imageColorbarLabelDigits=ge.attribute("image_digits", "3").toInt();

    graph.imageColorbarRight=QStringToBool(ge.attribute("image_colorbarright", "true"));
    graph.imageColorbarTop=QStringToBool(ge.attribute("image_colorbartop", "false"));
    graph.colorbarWidth=CQStringToDouble(ge.attribute("image_colorbarwidth", "20"));
    graph.colorbarRelativeHeight=CQStringToDouble(ge.attribute("image_colorbarrelativeheight", "0.75"));
    graph.imageLegend=ge.attribute("image_legend", "");
    graph.imageLegendG=ge.attribute("image_legend_g", "");
    graph.imageLegendB=ge.attribute("image_legend_b", "");
    graph.imageLegendMod=ge.attribute("image_legend_mod", "");
    graph.function=ge.attribute("function", "");
    graph.subfunction=ge.attribute("subfunction", "-1").toInt();
    graph.showallsubfunctions=QStringToBool(ge.attribute("showallsubfunctions", "false"));
    graph.functionType=String2GTFunctionType(ge.attribute("functiontype", "string"));
    graph.modifierMode=JKQTPMathImage::StringToModifierMode(ge.attribute("modifier_mode", "none"));
    graph.functionParameters=stringToDoubleArray_base64(ge.attribute("fparams", ""));
    //graph.functionParameterErrors=stringToDoubleArray_base64(ge.attribute("ferrors", ""));


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

    graph.offset=CQStringToDouble(ge.attribute("offset", "0"));
    graph.xerrorcolumnlower=ge.attribute("xerrorcolumnlower", "-1").toInt();
    graph.yerrorcolumnlower=ge.attribute("yerrorcolumnlower", "-1").toInt();

    graph.xerrorsymmetric=QStringToBool(ge.attribute("xerrorsymmetric", "true"));
    graph.yerrorsymmetric=QStringToBool(ge.attribute("yerrorsymmetric", "true"));


    graph.moreProperties.clear();
    QDomElement gmp=ge.firstChildElement("property");
    while (!gmp.isNull()) {
        QString name=gmp.attribute("name");
        //QString type=gmp.attribute("type");
        //QString value=gmp.text();
        /*if (!name.isEmpty() && !type.isEmpty()) {
            graph.moreProperties[name]=getQVariantFromString(type, value);
        }*/
        QVariant value=readQVariant(gmp);
        if (!name.isEmpty() && value.isValid()) {
            //qDebug()<<name<<value;
            graph.moreProperties[name]=value;
        }
        gmp=gmp.nextSiblingElement("property");
    }
}

void QFRDRTable::writeAxisInfo(QXmlStreamWriter &w, const QFRDRTable::AxisInfo &plot, const QString &axisName) const
{

    w.writeAttribute(axisName+"label", plot.label);
    w.writeAttribute(axisName+"digits", QString::number(plot.digits));
    w.writeAttribute(axisName+"min_ticks", QString::number(plot.minTicks));
    w.writeAttribute(axisName+"minor_ticks", QString::number(plot.minorTicks));
    w.writeAttribute(axisName+"label_pos", JKQTPlabelPosition2String(plot.labelPos));
    w.writeAttribute(axisName+"label_type", JKQTPCAlabelType2String(plot.labelType));

    w.writeAttribute(axisName+"label_mode1", JKQTPCAdrawMode2String(plot.labelMode1));
    w.writeAttribute(axisName+"label_mode2", JKQTPCAdrawMode2String(plot.labelMode2));

    w.writeAttribute(axisName+"log", boolToQString(plot.log));
    w.writeAttribute(axisName+"0axis", boolToQString(plot.axis0));
    w.writeAttribute(axisName+"min", CDoubleToQString(plot.min));
    w.writeAttribute(axisName+"max", CDoubleToQString(plot.max));

    w.writeAttribute(axisName+"minor_tick_labels_only_digit", boolToQString(plot.minorTickLabelsOnlyDigit));
    w.writeAttribute(axisName+"lin_ticks_for_log_axis", boolToQString(plot.LinTicksForLogAxis));
    w.writeAttribute(axisName+"minor_tick_label", boolToQString(plot.minorTickLabels));
    w.writeAttribute(axisName+"tickspacing_log", CDoubleToQString(plot.TickSpacingLog));

    w.writeAttribute(axisName+"autoticks", boolToQString(plot.AutoTicks));
    w.writeAttribute(axisName+"axisinverted", boolToQString(plot.AxisInverted));
    w.writeAttribute(axisName+"tickspacing", CDoubleToQString(plot.TickSpacing));
    w.writeAttribute(axisName+"tickwidth", CDoubleToQString(plot.AxisTickWidth));
    w.writeAttribute(axisName+"mintickwidth", CDoubleToQString(plot.AxisMinorTickWidth));
    w.writeAttribute(axisName+"tickilength", CDoubleToQString(plot.TickInsideLength));
    w.writeAttribute(axisName+"tickolength", CDoubleToQString(plot.TickOutsideLength));
    w.writeAttribute(axisName+"mintickilength", CDoubleToQString(plot.MinorTickInsideLength));
    w.writeAttribute(axisName+"mintickolength", CDoubleToQString(plot.MinorTickOutsideLength));
    w.writeAttribute(axisName+"labelangel", CDoubleToQString(plot.labelAngel));
    w.writeAttribute(axisName+"namedticks_namecol", QString::number(plot.columnNamedTickNames));
    w.writeAttribute(axisName+"namedticks_valcol", QString::number(plot.columnNamedTickValues));

    w.writeAttribute(axisName+"axis0_linewidth", CDoubleToQString(plot.zeroAxisLineWidth));
    w.writeAttribute(axisName+"axis1_linewidth", CDoubleToQString(plot.axis1LineWidth));


}

void QFRDRTable::writePlotInfo(QXmlStreamWriter &w, const QFRDRTable::PlotInfo &plot, bool writeGraphs) const
{
    w.writeStartElement("plot");
    w.writeAttribute("title", plot.title);
    w.writeAttribute("autosize", boolToQString(plot.graphAutosize));
    w.writeAttribute("gwidth", QString::number(plot.graphWidth));
    w.writeAttribute("gheight", QString::number(plot.graphHeight));

    w.writeAttribute("showkey", boolToQString(plot.showKey));
    w.writeAttribute("showtitle", boolToQString(plot.showTitle));
    w.writeAttribute("grid", boolToQString(plot.grid));
    w.writeAttribute("grid_minor", boolToQString(plot.gridMinor));

    w.writeAttribute("grid_minorx", boolToQString(plot.gridMinorX));
    w.writeAttribute("grid_minory", boolToQString(plot.gridMinorY));
    w.writeAttribute("grid_majorx", boolToQString(plot.gridMajorX));
    w.writeAttribute("grid_majory", boolToQString(plot.gridMajorY));

    w.writeAttribute("keyfontsize", CDoubleToQString(plot.keyFontSize));
    w.writeAttribute("axisfontsize", CDoubleToQString(plot.axisFontSize));
    w.writeAttribute("axisminorfontsize", CDoubleToQString(plot.axisMinorFontSize));
    w.writeAttribute("axislabelfontsize", CDoubleToQString(plot.axisLabelFontSize));
    w.writeAttribute("labelfontsize", CDoubleToQString(plot.labelFontSize));
    w.writeAttribute("fontname", plot.fontName);
    w.writeAttribute("keytransparency", CDoubleToQString(plot.keyTransparency));
    w.writeAttribute("keyposition", JKQTPkeyPosition2String(plot.keyPosition));
    w.writeAttribute("keylayout", JKQTPkeyLayout2String(plot.keyLayout));
    w.writeAttribute("keep_dataaspect", boolToQString(plot.keepDataAspectRatio));
    w.writeAttribute("dataaspect", CDoubleToQString(plot.axisAspectRatio));
    w.writeAttribute("keep_axisaspect", boolToQString(plot.keepAxisAspectRatio));
    w.writeAttribute("axisaspect", CDoubleToQString(plot.dataAspectRatio));

    w.writeAttribute("keybox", boolToQString(plot.keyBox));
    w.writeAttribute("keyboxlinewidth", CDoubleToQString(plot.keyBoxLineWidth));
    w.writeAttribute("keybackgroundcolor", QColor2String(plot.keyBackgroundColor));
    w.writeAttribute("keyboxcolor", QColor2String(plot.keyLineColor));

    w.writeAttribute("meyxmargin", CDoubleToQString(plot.keyXMargin));
    w.writeAttribute("keyymargin", CDoubleToQString(plot.keyYMargin));
    w.writeAttribute("keyxoffset", CDoubleToQString(plot.keyXOffset));
    w.writeAttribute("keyyoffset", CDoubleToQString(plot.keyYOffset));
    w.writeAttribute("keyxseparation", CDoubleToQString(plot.keyXSeparation));
    w.writeAttribute("keyyseparation", CDoubleToQString(plot.keyYSeparation));
    w.writeAttribute("key_line_length", CDoubleToQString(plot.key_line_length));


    w.writeAttribute("background_color", QColor2String(plot.backgroundColor));
    w.writeAttribute("grid_width", CDoubleToQString(plot.gridWidth));
    w.writeAttribute("grid_color", QColor2String(plot.gridColor));
    w.writeAttribute("grid_style", QPenStyle2String(plot.gridStyle));
    w.writeAttribute("grid_width_minor", CDoubleToQString(plot.gridWidthMinor));
    w.writeAttribute("grid_color_minor", QColor2String(plot.gridColorMinor));
    w.writeAttribute("grid_style_minor", QPenStyle2String(plot.gridStyleMinor));



    writeAxisInfo(w, plot.xAxis, "x");
    writeAxisInfo(w, plot.yAxis, "y");


    if (writeGraphs) {
        for (int g=0; g<plot.graphs.size(); g++) {
            writeGraphInfo(w, plot.graphs[g]);
        }
    }
    w.writeEndElement();
}

void QFRDRTable::writeGraphInfo(QXmlStreamWriter &w, const QFRDRTable::GraphInfo &graph) const
{
    w.writeStartElement("graph");
    w.writeAttribute("type", GraphType2String(graph.type));
    w.writeAttribute("title", graph.title);
    w.writeAttribute("showtitle", boolToQString(graph.titleShow));
    w.writeAttribute("drawline", boolToQString( graph.drawLine));
    w.writeAttribute("xcolumn", QString::number(graph.xcolumn));
    w.writeAttribute("ycolumn", QString::number(graph.ycolumn));
    w.writeAttribute("xerrorcolumn", QString::number(graph.xerrorcolumn));
    w.writeAttribute("yerrorcolumn", QString::number(graph.yerrorcolumn));

    w.writeAttribute("meancolumn", QString::number(graph.meancolumn));
    w.writeAttribute("q75column", QString::number(graph.q75column));
    w.writeAttribute("maxcolumn", QString::number(graph.maxcolumn));
    w.writeAttribute("width", CDoubleToQString(graph.width));
    w.writeAttribute("shift", CDoubleToQString(graph.shift));

    w.writeAttribute("linewidth", CDoubleToQString(graph.linewidth));
    w.writeAttribute("symbolSize", CDoubleToQString(graph.symbolSize));
    w.writeAttribute("symbolLineWidth", CDoubleToQString(graph.symbolLineWidth));
    w.writeAttribute("style", QPenStyle2String(graph.style));
    w.writeAttribute("whisker_style", QPenStyle2String(graph.whiskerStyle));
    w.writeAttribute("fill_style", QBrushStyle2String(graph.fillStyle));
    w.writeAttribute("color", QColor2String(graph.color));
    w.writeAttribute("errorcolor", QColor2String(graph.errorColor));
    w.writeAttribute("fillcolor", QColor2String(graph.fillColor));
    w.writeAttribute("symbol", JKQTPgraphSymbols2String(graph.symbol));
    w.writeAttribute("errorStyle", JKQTPerrorPlotstyle2String(graph.errorStyle));
    w.writeAttribute("color_trans", CDoubleToQString(graph.colorTransparent));
    w.writeAttribute("errorcolor_trans", CDoubleToQString(graph.errorColorTransparent));
    w.writeAttribute("fillcolor_trans", CDoubleToQString(graph.fillColorTransparent));


    w.writeAttribute("image_truecolor", QColor2String(graph.imageTrueColor));
    w.writeAttribute("image_truecolor_trans", CDoubleToQString(graph.imageTrueTransparent));
    w.writeAttribute("image_falsecolor", QColor2String(graph.imageFalseColor));
    w.writeAttribute("image_falsecolor_trans", CDoubleToQString(graph.imageFalseTransparent));
    w.writeAttribute("image_pixelwidth", QString::number(graph.imagePixelWidth));
    w.writeAttribute("image_x", CDoubleToQString(graph.imageX));
    w.writeAttribute("image_y", CDoubleToQString(graph.imageY));
    w.writeAttribute("image_width", CDoubleToQString(graph.imageWidth));
    w.writeAttribute("image_height", CDoubleToQString(graph.imageHeight));
    w.writeAttribute("image_palette", QString::number(graph.imagePalette));
    w.writeAttribute("image_min", CDoubleToQString(graph.imageMin));
    w.writeAttribute("image_max", CDoubleToQString(graph.imageMax));

    w.writeAttribute("image_colorbar_ticklen", CDoubleToQString(graph.imageColorbarTicklength));
    w.writeAttribute("image_colorbar_fontsize", CDoubleToQString(graph.imageColorbarFontsize));
    w.writeAttribute("image_ticks", QString::number(graph.imageTicks));
    w.writeAttribute("image_modticks", QString::number(graph.imageModTicks));
    w.writeAttribute("image_labeltype", JKQTPCAlabelType2String(graph.imageColorbarLabelType));
    w.writeAttribute("image_digits", QString::number(graph.imageColorbarLabelDigits));




    w.writeAttribute("image_autorange", boolToQString(graph.imageAutoRange));

    w.writeAttribute("image_colorbarright", boolToQString(graph.imageColorbarRight));
    w.writeAttribute("image_colorbartop", boolToQString(graph.imageColorbarTop));
    w.writeAttribute("image_colorbarwidth", QString::number(graph.colorbarWidth));
    w.writeAttribute("image_colorbarrelativeheight", CDoubleToQString(graph.colorbarRelativeHeight));
    w.writeAttribute("image_legend", graph.imageLegend);
    w.writeAttribute("image_legend_g", graph.imageLegendG);
    w.writeAttribute("image_legend_b", graph.imageLegendB);
    w.writeAttribute("image_legend_mod", graph.imageLegendMod);
    w.writeAttribute("function", graph.function);
    w.writeAttribute("subfunction", QString::number(graph.subfunction));
    w.writeAttribute("showallsubfunctions", boolToQString(graph.showallsubfunctions));
    w.writeAttribute("functiontype", GTFunctionType2String(graph.functionType));
    w.writeAttribute("fparams", doubleArrayToString_base64(graph.functionParameters));
    //w.writeAttribute("ferrors", doubleArrayToString_base64(graph.functionParameterErrors));
    w.writeAttribute("data_sort_order", QString::number(graph.dataSortOrder));
    w.writeAttribute("stride", QString::number(graph.stride));
    w.writeAttribute("stride_start", QString::number(graph.strideStart));
    w.writeAttribute("is_strided", boolToQString(graph.isStrided));
    w.writeAttribute("modifier_mode", JKQTPMathImage::ModifierModeToString(graph.modifierMode));

    w.writeAttribute("is_data_select", boolToQString(graph.isDataSelect));
    w.writeAttribute("data_select_column", QString::number(graph.dataSelect1Column));
    w.writeAttribute("data_select_operation", DataSelectOperation2String(graph.dataSelect1Operation));
    w.writeAttribute("data_select_value", CDoubleToQString(graph.dataSelect1CompareValue));
    w.writeAttribute("data_select_value2", CDoubleToQString(graph.dataSelect1CompareValue2));

    w.writeAttribute("data_select_logic12", DataSelectLogicOperation2String(graph.dataSelectLogic12));
    w.writeAttribute("data_select_logic23", DataSelectLogicOperation2String(graph.dataSelectLogic23));


    w.writeAttribute("data_select2_column", QString::number(graph.dataSelect2Column));
    w.writeAttribute("data_select2_operation", DataSelectOperation2String(graph.dataSelect2Operation));
    w.writeAttribute("data_select2_value", CDoubleToQString(graph.dataSelect2CompareValue));
    w.writeAttribute("data_select2_value2", CDoubleToQString(graph.dataSelect2CompareValue2));

    w.writeAttribute("data_select3_column", QString::number(graph.dataSelect3Column));
    w.writeAttribute("data_select3_operation", DataSelectOperation2String(graph.dataSelect3Operation));
    w.writeAttribute("data_select3_value", CDoubleToQString(graph.dataSelect3CompareValue));
    w.writeAttribute("data_select3_value2", CDoubleToQString(graph.dataSelect3CompareValue2));


    w.writeAttribute("error_width", CDoubleToQString(graph.errorWidth));
    w.writeAttribute("error_line_style", QPenStyle2String(graph.errorLineStyle));
    w.writeAttribute("error_barsize", CDoubleToQString(graph.errorBarSize));



    w.writeAttribute("range_start", CDoubleToQString(graph.rangeStart));
    w.writeAttribute("range_end", CDoubleToQString(graph.rangeEnd));
    w.writeAttribute("range_center", CDoubleToQString(graph.rangeCenter));
    w.writeAttribute("range_cwidth", CDoubleToQString(graph.rangeCenterWidth));
    w.writeAttribute("range_ccolort", CDoubleToQString(graph.rangeCenterColorTransparent));

    w.writeAttribute("range_ccolor", QColor2String(graph.rangeCenterColor));
    w.writeAttribute("range_cstyle", QPenStyle2String(graph.rangeCenterStyle));

    w.writeAttribute("range_inverted", boolToQString( graph.rangeInverted));
    w.writeAttribute("range_fill", boolToQString( graph.rangeFill));
    w.writeAttribute("range_drawcenter", boolToQString( graph.rangeDrawCenter));


    w.writeAttribute("error_color_auto", boolToQString( graph.errorColorAuto));
    w.writeAttribute("fill_color_auto", boolToQString( graph.fillColorAuto));
    w.writeAttribute("center_color_auto", boolToQString( graph.centerColorAuto));

    w.writeAttribute("offset", CDoubleToQString(graph.offset));
    w.writeAttribute("xerrorcolumnlower", QString::number(graph.xerrorcolumnlower));
    w.writeAttribute("yerrorcolumnlower", QString::number(graph.yerrorcolumnlower));

    w.writeAttribute("xerrorsymmetric", boolToQString( graph.xerrorsymmetric));
    w.writeAttribute("yerrorsymmetric", boolToQString( graph.yerrorsymmetric));


    QMapIterator<QString, QVariant> mit(graph.moreProperties);
    while (mit.hasNext()) {
         mit.next();
         w.writeStartElement("property");
         w.writeAttribute("name", mit.key());
         writeQVariant(w, mit.value());
         /*w.writeAttribute("type", getQVariantType(mit.value()));
         if (mit.value().type()==QVariant::List) {
             QVariantList vl=mit.value();
             for (int j=0; j<vl.size(); vl++) {
                 w.writeStartElement("list_item");
                 const QVariant v=vl[j];
                 w.writeAttribute("type", getQVariantType(v.value()));
                 w.writeEndElement();
             }
         } else {
            w.writeCDATA(mit.value().toString());
         }*/
         w.writeEndElement();
    }

    w.writeEndElement();
}

void QFRDRTable::readPlotInfo(PlotInfo& plot, QDomElement te, bool readLabels) {
    if (readLabels) plot.title=te.attribute("title", tr("graph title"));
    plot.graphAutosize=QStringToBool(te.attribute("autosize", "true"));
    plot.graphWidth=te.attribute("gwidth", "150").toInt();
    plot.graphHeight=te.attribute("gheight", "150").toInt();
    plot.grid=QStringToBool( te.attribute("grid", "true"));
    plot.gridMinor=QStringToBool( te.attribute("grid_minor", "false"));

    plot.gridMinorX=QStringToBool( te.attribute("grid_minorx", "true"));
    plot.gridMinorY=QStringToBool( te.attribute("grid_minory", "true"));
    plot.gridMajorX=QStringToBool( te.attribute("grid_majorx", "true"));
    plot.gridMajorY=QStringToBool( te.attribute("grid_majory", "true"));

    plot.showKey=QStringToBool( te.attribute("showkey", "true"));
    plot.showTitle=QStringToBool( te.attribute("showtitle", "true"));
    plot.fontName=te.attribute("fontname", "Arial");
    plot.keyFontSize=CQStringToDouble(te.attribute("keyfontsize", "12"));
    plot.axisFontSize=CQStringToDouble(te.attribute("axisfontsize", "10"));
    plot.axisMinorFontSize=CQStringToDouble(te.attribute("axisminorfontsize", "6"));
    plot.axisLabelFontSize=CQStringToDouble(te.attribute("axislabelfontsize", "12"));
    plot.labelFontSize=CQStringToDouble(te.attribute("labelFontSize", "16"));

    plot.keyTransparency=CQStringToDouble(te.attribute("keytransparency", "0.5"));
    plot.keyPosition=String2JKQTPkeyPosition(te.attribute("keyposition", "inside_right"));
    plot.keyLayout=String2JKQTPkeyLayout(te.attribute("keylayout", "one_column"));
    plot.keyBoxLineWidth=CQStringToDouble(te.attribute("keyboxlinewidth", "1"));
    plot.keyBox=QStringToBool( te.attribute("keybox", "true"));
    plot.keyBackgroundColor=QStringToQColor(te.attribute("keybackgroundcolor", "white"));
    plot.keyLineColor=QStringToQColor(te.attribute("keyboxcolor", "black"));



    plot.keepDataAspectRatio=QStringToBool( te.attribute("keep_dataaspect", "false"));
    plot.axisAspectRatio=CQStringToDouble(te.attribute("dataaspect", "1"));
    plot.keepAxisAspectRatio=QStringToBool( te.attribute("keep_axisaspect", "false"));
    plot.dataAspectRatio=CQStringToDouble(te.attribute("axisaspect", "1"));

    plot.backgroundColor=QStringToQColor(te.attribute("background_color", "white"));
    plot.gridColor=QStringToQColor(te.attribute("grid_color", "darkgrey"));
    plot.gridStyle=String2QPenStyle(te.attribute("grid_style", "dash"));
    plot.gridWidth=CQStringToDouble(te.attribute("grid_width", "1"));
    plot.gridColorMinor=QStringToQColor(te.attribute("grid_color_minor", "darkgrey"));
    plot.gridStyleMinor=String2QPenStyle(te.attribute("grid_style_minor", "dot"));
    plot.gridWidthMinor=CQStringToDouble(te.attribute("grid_width_minor", "0.5"));



    plot.keyXMargin=CQStringToDouble(te.attribute("meyxmargin", "0.5"));
    plot.keyYMargin=CQStringToDouble(te.attribute("keyymargin", "0.25"));
    plot.keyXOffset=CQStringToDouble(te.attribute("keyxoffset", "0.5"));
    plot.keyYOffset=CQStringToDouble(te.attribute("keyyoffset", "0.5"));
    plot.keyXSeparation=CQStringToDouble(te.attribute("keyxseparation", "0.75"));
    plot.keyYSeparation=CQStringToDouble(te.attribute("keyyseparation", "0.2"));
    plot.key_line_length=CQStringToDouble(te.attribute("key_line_length", "3"));

    readAxisInfo(plot.xAxis, "x", te, readLabels);
    readAxisInfo(plot.yAxis, "y", te, readLabels);



    QDomElement ge=te.firstChildElement("graph");
    while (!ge.isNull()) {
        GraphInfo graph;
        readGraphInfo(graph, ge);
        plot.graphs.append(graph);
        ge = ge.nextSiblingElement("graph");
    }
}

void QFRDRTable::readAxisInfo(AxisInfo& plot, const QString& axisName, QDomElement te, bool readLabels) {
    plot.min=CQStringToDouble(te.attribute(axisName+"min", "0"));
    plot.max=CQStringToDouble(te.attribute(axisName+"max", "10"));
    if (readLabels) plot.label=te.attribute(axisName+"label", "x");
    plot.log=QStringToBool( te.attribute(axisName+"log", "false"));
    plot.axis0=QStringToBool( te.attribute(axisName+"0axis", "true"));
    plot.digits=te.attribute(axisName+"digits", "3").toInt();
    plot.minTicks=te.attribute(axisName+"min_ticks", "7").toInt();
    plot.minorTicks=te.attribute(axisName+"minor_ticks", "1").toInt();
    plot.labelPos=String2JKQTPlabelPosition(te.attribute(axisName+"label_pos", "center"));
    plot.labelType=String2JKQTPCAlabelType(te.attribute(axisName+"label_type", "exponent"));
    if (axisName=="x2" || axisName=="y2")  {
        plot.labelMode1=String2JKQTPCAdrawMode(te.attribute(axisName+"label_mode1", "line"));
        plot.labelMode2=String2JKQTPCAdrawMode(te.attribute(axisName+"label_mode2", "all"));
    } else {
        plot.labelMode1=String2JKQTPCAdrawMode(te.attribute(axisName+"label_mode1", "all"));
        plot.labelMode2=String2JKQTPCAdrawMode(te.attribute(axisName+"label_mode2", "line"));
    }

    plot.AutoTicks=QStringToBool( te.attribute(axisName+"autoticks", "true"));
    plot.AxisInverted=QStringToBool( te.attribute(axisName+"axisinverted", "false"));
    plot.TickSpacing=CQStringToDouble(te.attribute(axisName+"tickspacing", "10"));
    plot.AxisTickWidth=CQStringToDouble(te.attribute(axisName+"tickwidth", "1.5"));
    plot.AxisMinorTickWidth=CQStringToDouble(te.attribute(axisName+"mintickwidth", "1"));
    plot.TickInsideLength=CQStringToDouble(te.attribute(axisName+"tickilength", "3"));
    plot.TickOutsideLength=CQStringToDouble(te.attribute(axisName+"tickolength", "3"));
    plot.MinorTickInsideLength=CQStringToDouble(te.attribute(axisName+"mintickilength", "2"));
    plot.MinorTickOutsideLength=CQStringToDouble(te.attribute(axisName+"mintickolength", "2"));
    plot.labelAngel=CQStringToDouble(te.attribute(axisName+"labelangel", "0"));
    plot.zeroAxisLineWidth=CQStringToDouble(te.attribute(axisName+"axis0_linewidth", "1.5"));
    plot.axis1LineWidth=CQStringToDouble(te.attribute(axisName+"axis1_linewidth", "1.5"));




    plot.minorTickLabelsOnlyDigit=QStringToBool( te.attribute(axisName+"minor_tick_labels_only_digit", "false"));
    plot.LinTicksForLogAxis=QStringToBool( te.attribute(axisName+"lin_ticks_for_log_axis", "false"));
    plot.minorTickLabels=QStringToBool( te.attribute(axisName+"minor_tick_label", "false"));
    plot.TickSpacingLog=CQStringToDouble(te.attribute(axisName+"tickspacing_log", "10"));



    plot.columnNamedTickNames=te.attribute(axisName+"namedticks_namecol", "-1").toInt();
    plot.columnNamedTickValues=te.attribute(axisName+"namedticks_valcol", "-2").toInt();
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
    if (files.size()>0 && !getQFProperty("DONT_READWRITE_FILE", false).toBool() ) {
        QString format=getQFProperty("READFILE_FILEFORMAT", "CSV").toString().trimmed().toUpper();
        if (format=="") {
            datamodel->readXMLFile(files[0]);
            QDomDocument docxml;
            if (docxml.setContent(readFile(files[0]))) {
                QDomElement er=docxml.firstChildElement("qfrdrtable");
                if (!er.isNull()) {
                    QDomElement e=er.firstChildElement("properties");
                    if (!e.isNull()) {
                        readProperties(e, false);
                    }
                }
            }
        } else {
            //qDebug()<<"    reading CSV\n";
            QString s=getProperty("column_separator", ",").toString();
            char column_separator=(s.size()>0)?s[0].toLatin1():',';
            s=getProperty("decimal_separator", ".").toString();
            char decimal_separator=(s.size()>0)?s[0].toLatin1():'.';
            s=getProperty("header_start", "#!").toString();
            QString header_start=s;
            s=getProperty("coment_start", "#").toString();
            char comment_start=(s.size()>0)?s[0].toLatin1():'#';

            //qDebug()<<"column_separator="<<column_separator<<"  decimal_separator="<<decimal_separator<<"   header_start="<<header_start.toStdString()<<"   comment_start="<<comment_start<<std::endl;

            QByteArray file=readFile(files[0]);
            QList<QByteArray> lines=file.split('\n');
            for (int l=0; l<lines.size(); l++) {
                QString ll=lines[l].trimmed();
                if (ll.startsWith(comment_start) && (header_start==QString(QChar(comment_start)) || !ll.startsWith(header_start))) {
                    ll=ll.right(ll.size()-1).trimmed();
                    QRegExp rxc("(.+)[\\=|\\:](.+)");
                    if (rxc.exactMatch(ll) && rxc.cap(1).trimmed().size()>0 && rxc.cap(2).trimmed().size()>0) {
                        QString name=rxc.cap(1).trimmed();
                        QString value=rxc.cap(2).trimmed();
                        setQFProperty(name, qfStringToVariantAutoRecognizeType(value), false, true);
                    }
                } else {
                    if (ll.startsWith(header_start) || (!ll.simplified().trimmed().isEmpty())) {
                        break;
                    }
                }
            }

            datamodel->readCSV(files[0], column_separator, decimal_separator, header_start, comment_start);
        }
        datamodel->setReadonly(true);
        if (propertyExists("CONVERT_READWRITE") && getQFProperty("CONVERT_READWRITE", false).toBool()) {
            datamodel->setReadonly(false);
            setQFProperty("DONT_READWRITE_FILE", true, false, false);
            deleteProperty("CONVERT_READWRITE");
        }
        datamodel->resetChanged();
        //qDebug()<<datamodel->getColumnTitles();
    } else if (e) {
        QDomElement te=e->firstChildElement("column");
        quint32 rows=0, columns=0;
        datamodel->setReadonly(false);
        if (!te.isNull()) {
            while (!te.isNull()) {
                columns++;
                QString n=te.attribute("title");
                QString hexp=te.attribute("expression", te.attribute("column_expression"));
                QString hc=te.attribute("comment", te.attribute("column_comment"));
                QDomElement re=te.firstChildElement("row");
                quint32 r=0;
                //qDebug()<<"resize("<<rows<<", "<<columns<<")\n";
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
                //qDebug()<<"resize("<<rows<<", "<<columns<<") DONE\n";
                while (!re.isNull()) {
                    QString t=re.attribute("type").toLower();
                    QString ex=re.attribute("expression");
                    QString comment=re.attribute("comment");
                    if (r+1>rows) {
                        rows=r+1;
                        //qDebug()<<"resize("<<rows<<", "<<columns<<")\n";
                        datamodel->resize(rows, columns);
                        //qDebug()<<"resize("<<rows<<", "<<columns<<") DONE\n";
                    }
                    QVariant v=getQVariantFromString(t, re.attribute("value"));
                    //qDebug()<<"setCell("<<r<<", "<<columns-1<<", '"<<v<<"' ["<<v.typeName()<<"])\n";
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
        //qDebug()<<"read plots ...";
        QDomElement te=e->firstChildElement("plots");
        plots.clear();
        if (!te.isNull()) {
            te=te.firstChildElement("plot");
            //qDebug()<<"  found plot ...";
            while (!te.isNull()) {
                PlotInfo plot;
                readPlotInfo(plot, te);

                plots.append(plot);
                te = te.nextSiblingElement("plot");
            }
        }
    }
    if (e) {
       //qDebug()<<"read fit algorithms ...";

       QDomElement te=e->firstChildElement("fit_algorithms").firstChildElement("algorithm");
       readQFFitAlgorithmParameters(te);
    }
}

void QFRDRTable::intWriteData(QXmlStreamWriter& w) const {
    if (!datamodel) {
        /*datamodel=new QFTablePluginModel(this);
        datamodel->setUndoEnabled(true);
        datamodel->setVerticalHeaderShowRowNumbers(true);
        datamodel->setDefaultEditValue(DEFAULT_EDITVAL);
        connect(datamodel, SIGNAL(modelReset()), this, SLOT(trawDataChanged()));
        connect(datamodel, SIGNAL(dataChanged( const QModelIndex & , const QModelIndex &  )), this, SLOT(tdataChanged( const QModelIndex & , const QModelIndex &  )));*/
        return;
    }
    if (files.size()>0 && !getQFProperty("DONT_READWRITE_FILE", false).toBool()) {
        if (datamodel->hasChanged()) datamodel->saveCSV(files[0]);
    } else {
        for (qint32 c=0; c<datamodel->columnCount(); c++) {
            w.writeStartElement("column");
            w.writeAttribute("title", datamodel->columnTitle(c));
            if (datamodel->hasColumnHeaderData(c, ColumnExpressionRole)) {
                w.writeAttribute("expression", datamodel->getColumnHeaderData(c, ColumnExpressionRole).toString());
            }
            if (datamodel->hasColumnHeaderData(c, ColumnCommentRole)) {
                w.writeAttribute("comment", datamodel->getColumnHeaderData(c, ColumnCommentRole).toString());
            }
            QList<quint64> coldroles=datamodel->getColumnHeaderDataRoles();
            for (int i=0; i<coldroles.size(); i++) {
                if (coldroles[i]!=ColumnExpressionRole && coldroles[i]!=ColumnCommentRole) {
                    w.writeAttribute(QString("colpar%1").arg(coldroles[i]), getQVariantData(datamodel->getColumnHeaderData(c, coldroles[i])));
                    w.writeAttribute(QString("colpart%1").arg(coldroles[i]), getQVariantType(datamodel->getColumnHeaderData(c, coldroles[i])));

                }
            }
            for (qint32 r=0; r<datamodel->rowCount(); r++) {
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
        writePlotInfo(w, plots[i]);
    }
    w.writeEndElement();
    w.writeStartElement("fit_algorithms");
    writeQFFitAlgorithmParameters(w);
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
            g.xerrorcolumnlower=QFRDRTableOffsetIfLarger(g.xerrorcolumnlower, start, count);
            g.yerrorcolumnlower=QFRDRTableOffsetIfLarger(g.yerrorcolumnlower, start, count);
            g.meancolumn=QFRDRTableOffsetIfLarger(g.meancolumn, start, count);
            g.q75column=QFRDRTableOffsetIfLarger(g.q75column, start, count);
            g.maxcolumn=QFRDRTableOffsetIfLarger(g.maxcolumn, start, count);
            g.dataSelect1Column=QFRDRTableOffsetIfLarger(g.dataSelect1Column, start, count);
            g.dataSelect2Column=QFRDRTableOffsetIfLarger(g.dataSelect2Column, start, count);
            g.dataSelect3Column=QFRDRTableOffsetIfLarger(g.dataSelect3Column, start, count);
        }
        p.xAxis.columnNamedTickNames=QFRDRTableOffsetIfLarger(p.xAxis.columnNamedTickNames, start, count);
        p.xAxis.columnNamedTickValues=QFRDRTableOffsetIfLarger(p.xAxis.columnNamedTickValues, start, count);
        p.yAxis.columnNamedTickNames=QFRDRTableOffsetIfLarger(p.yAxis.columnNamedTickNames, start, count);
        p.yAxis.columnNamedTickValues=QFRDRTableOffsetIfLarger(p.yAxis.columnNamedTickValues, start, count);
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
            g.xerrorcolumnlower=QFRDRTableOffsetIfLargerM1IfInRange(g.xerrorcolumnlower, start, count);
            g.yerrorcolumnlower=QFRDRTableOffsetIfLargerM1IfInRange(g.yerrorcolumnlower, start, count);
            g.meancolumn=QFRDRTableOffsetIfLargerM1IfInRange(g.meancolumn, start, count);
            g.q75column=QFRDRTableOffsetIfLargerM1IfInRange(g.q75column, start, count);
            g.maxcolumn=QFRDRTableOffsetIfLargerM1IfInRange(g.maxcolumn, start, count);
            g.dataSelect1Column=QFRDRTableOffsetIfLargerM1IfInRange(g.dataSelect1Column, start, count);
            g.dataSelect2Column=QFRDRTableOffsetIfLargerM1IfInRange(g.dataSelect2Column, start, count);
            g.dataSelect3Column=QFRDRTableOffsetIfLargerM1IfInRange(g.dataSelect3Column, start, count);
        }
        p.xAxis.columnNamedTickNames=QFRDRTableOffsetIfLargerM1IfInRange(p.xAxis.columnNamedTickNames, start, count);
        p.xAxis.columnNamedTickValues=QFRDRTableOffsetIfLargerM1IfInRange(p.xAxis.columnNamedTickValues, start, count);
        p.yAxis.columnNamedTickNames=QFRDRTableOffsetIfLargerM1IfInRange(p.yAxis.columnNamedTickNames, start, count);
        p.yAxis.columnNamedTickValues=QFRDRTableOffsetIfLargerM1IfInRange(p.yAxis.columnNamedTickValues, start, count);
    }

    if (emitRebuild&&datamodel->getDoEmitSignals()) emitRebuildPlotWidgets();
}




void QFRDRTable::loadColumnToComboBox(QComboBox *combo)
{
    int idx=combo->currentIndex();
    //disconnect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));

        int addcolumns=2;
        QFTablePluginModel* tab=model();
        while (combo->count()-addcolumns>tab->columnCount() && combo->count()>addcolumns) {
            combo->removeItem(combo->count()-1);
        }
        while (combo->count()-addcolumns<tab->columnCount() && tab->columnCount()>0) {
            combo->addItem("");
        }
        combo->setItemText(0, tr("--- none ---"));
        combo->setItemData(0, -1);
        combo->setItemText(1, tr("--- special: 1,2,3... ---"));
        combo->setItemData(1, -2);
        for (int i=0; i<tab->columnCount(); i++) {
            combo->setItemText(i+2, tr("#%1: %2").arg(i+1).arg(tab->columnTitle(i)));
            combo->setItemData(i+2, i);//tab->columnTitle(i));
        }
        combo->setCurrentIndex(idx);
}

QString QFRDRTable::getParserPreScript() const
{
    return getQFProperty("table_parser_prescript", "").toString();
}

void QFRDRTable::setParserPreScript(const QString &script)
{
    setQFProperty("table_parser_prescript", script, false, false);
}


QFRDRTable::GraphDataSelection::GraphDataSelection()
{
    isStrided=false;
    stride=1;
    strideStart=1;
    isDataSelect=false;
    dataSelect1Column=-1;
    dataSelect1Operation=dsoEquals;
    dataSelect1CompareValue=0;
    dataSelect1CompareValue2=1;

    dataSelectLogic12=dsoNone;
    dataSelect2Column=-1;
    dataSelect2Operation=dsoEquals;
    dataSelect2CompareValue=0;
    dataSelect2CompareValue2=1;

    dataSelectLogic23=dsoNone;
    dataSelect3Column=-1;
    dataSelect3Operation=dsoEquals;
    dataSelect3CompareValue=0;
    dataSelect3CompareValue2=1;
}

QFRDRTable::GraphDataSelection::GraphDataSelection(const QFRDRTable::GraphDataSelection &g)
{
    operator=(g);
}

QFRDRTable::GraphDataSelection &QFRDRTable::GraphDataSelection::operator=(const QFRDRTable::GraphInfo &g)
{
    isStrided=g.isStrided;
    stride=g.stride;
    strideStart=g.strideStart;
    isDataSelect=g.isDataSelect;
    dataSelect1Column=g.dataSelect1Column;
    dataSelect1Operation=g.dataSelect1Operation;
    dataSelect1CompareValue=g.dataSelect1CompareValue;
    dataSelect1CompareValue2=g.dataSelect1CompareValue2;

    dataSelectLogic12=g.dataSelectLogic12;
    dataSelect2Column=g.dataSelect2Column;
    dataSelect2Operation=g.dataSelect2Operation;
    dataSelect2CompareValue=g.dataSelect2CompareValue;
    dataSelect2CompareValue2=g.dataSelect2CompareValue2;

    dataSelectLogic23=g.dataSelectLogic23;
    dataSelect3Column=g.dataSelect3Column;
    dataSelect3Operation=g.dataSelect3Operation;
    dataSelect3CompareValue=g.dataSelect3CompareValue;
    dataSelect3CompareValue2=g.dataSelect3CompareValue2;
    return *this;
}

QFRDRTable::GraphDataSelection &QFRDRTable::GraphDataSelection::operator=(const QFRDRTable::GraphDataSelection &g)
{
    isStrided=g.isStrided;
    stride=g.stride;
    strideStart=g.strideStart;
    isDataSelect=g.isDataSelect;
    dataSelect1Column=g.dataSelect1Column;
    dataSelect1Operation=g.dataSelect1Operation;
    dataSelect1CompareValue=g.dataSelect1CompareValue;
    dataSelect1CompareValue2=g.dataSelect1CompareValue2;

    dataSelectLogic12=g.dataSelectLogic12;
    dataSelect2Column=g.dataSelect2Column;
    dataSelect2Operation=g.dataSelect2Operation;
    dataSelect2CompareValue=g.dataSelect2CompareValue;
    dataSelect2CompareValue2=g.dataSelect2CompareValue2;

    dataSelectLogic23=g.dataSelectLogic23;
    dataSelect3Column=g.dataSelect3Column;
    dataSelect3Operation=g.dataSelect3Operation;
    dataSelect3CompareValue=g.dataSelect3CompareValue;
    dataSelect3CompareValue2=g.dataSelect3CompareValue2;
    return *this;
}

bool QFRDRTable::GraphDataSelection::getDataWithStride(QVector<double> *dataOut, int* colOut, int column, const QFRDRTable *table, QString *newname, QFPlotter* plotter)
{
    if (colOut) *colOut=-1;
    //qDebug()<<"getColumnWithStride  column="<<column<<"    strided: "<<g.isStrided<<" stride="<<g.stride<<" strideStart="<<g.strideStart;
    if (column==-2) {
        if (plotter && colOut) {
            *colOut=plotter->getDatastore()->ensureColumnNum(QString("rowNumColSpecial"));
            if (newname) *newname=QString("rowNumColSpecial");
            return true;
        } else if (dataOut && table) {
            dataOut->clear();
            for (int i=0; i<table->tableGetColumnCount(); i++) {
                (*dataOut)<<i;
            }
            if (newname) *newname=QString("rowNumColSpecial");
            return true;
        }
    }
    if (plotter && colOut) {
        if (column>=0 && column<(long)plotter->getDatastore()->getColumnCount()) {
            QVector<double> data=plotter->getDatastore()->getColumn(column).copyData();
            if (isDataSelect) {
                QVector<double> dataS, dataS2, dataS3;
                if (dataSelect1Column>=0 && dataSelect1Column<(long)plotter->getDatastore()->getColumnCount()) {
                    dataS=plotter->getDatastore()->getColumn(dataSelect1Column).copyData();
                }
                int cnt=qMin(data.size(), dataS.size());
                if (dataSelectLogic12!=QFRDRTable::dsoNone && dataSelect2Column>=0 && dataSelect2Column<(long)plotter->getDatastore()->getColumnCount()) {
                    dataS2=plotter->getDatastore()->getColumn(dataSelect2Column).copyData();
                    cnt=qMin(cnt, dataS2.size());
                }
                if (dataSelectLogic12!=QFRDRTable::dsoNone && dataSelectLogic23!=QFRDRTable::dsoNone && dataSelect3Column>=0 && dataSelect3Column<(long)plotter->getDatastore()->getColumnCount()) {
                    dataS3=plotter->getDatastore()->getColumn(dataSelect3Column).copyData();
                    cnt=qMin(cnt, dataS3.size());
                }
                QVector<double> dataO;
                int istart=0;
                int iinc=1;
                if (isStrided) {
                    istart=strideStart-1;
                    iinc=stride;
                }

                for (int i=istart; i< cnt; i=i+iinc) {
                    bool ok=true;
                    if (dataS.size()>0) {
                        ok=false;
                        switch(dataSelect1Operation) {
                            case QFRDRTable::dsoEquals:
                                if (dataS[i]==dataSelect1CompareValue) ok=true;
                                break;
                            case QFRDRTable::dsoUnequal:
                                if (dataS[i]!=dataSelect1CompareValue) ok=true;
                                break;
                            case QFRDRTable::dsoGreaterOrEqual:
                                if (dataS[i]>=dataSelect1CompareValue) ok=true;
                                break;
                            case QFRDRTable::dsoSmallerOrEqual:
                                if (dataS[i]<=dataSelect1CompareValue) ok=true;
                                break;
                            case QFRDRTable::dsoGreater:
                                if (dataS[i]>dataSelect1CompareValue) ok=true;
                                break;
                            case QFRDRTable::dsoSmaller:
                                if (dataS[i]<dataSelect1CompareValue) ok=true;
                                break;
                            case QFRDRTable::dsoInRange:
                                if (dataS[i]>=dataSelect1CompareValue && dataS[i]<=dataSelect1CompareValue2) ok=true;
                                break;
                            case QFRDRTable::dsoOutOfRange:
                                if (!(dataS[i]>=dataSelect1CompareValue && dataS[i]<=dataSelect1CompareValue2)) ok=true;
                                break;
                        }
                        if (dataSelectLogic12!=QFRDRTable::dsoNone && dataS2.size()>0) {
                            bool okLocal=false;
                            switch(dataSelect2Operation) {
                                case QFRDRTable::dsoEquals:
                                    if (dataS2[i]==dataSelect2CompareValue) okLocal=true;
                                    break;
                                case QFRDRTable::dsoUnequal:
                                    if (dataS2[i]!=dataSelect2CompareValue) okLocal=true;
                                    break;
                                case QFRDRTable::dsoGreaterOrEqual:
                                    if (dataS2[i]>=dataSelect2CompareValue) okLocal=true;
                                    break;
                                case QFRDRTable::dsoSmallerOrEqual:
                                    if (dataS2[i]<=dataSelect2CompareValue) okLocal=true;
                                    break;
                                case QFRDRTable::dsoGreater:
                                    if (dataS2[i]>dataSelect2CompareValue) okLocal=true;
                                    break;
                                case QFRDRTable::dsoSmaller:
                                    if (dataS2[i]<dataSelect2CompareValue) okLocal=true;
                                    break;
                                case QFRDRTable::dsoInRange:
                                    if (dataS2[i]>=dataSelect2CompareValue && dataS2[i]<=dataSelect2CompareValue2) okLocal=true;
                                    break;
                                case QFRDRTable::dsoOutOfRange:
                                    if (!(dataS2[i]>=dataSelect2CompareValue && dataS2[i]<=dataSelect2CompareValue2)) okLocal=true;
                                    break;
                            }
                            if (dataSelectLogic12==QFRDRTable::dsoAnd) ok=ok&&okLocal;
                            else if (dataSelectLogic12==QFRDRTable::dsoOr) ok=ok||okLocal;
                            else if (dataSelectLogic12==QFRDRTable::dsoXor) ok=(ok&&!okLocal) || (!ok&&okLocal);
                            if (dataSelectLogic23!=QFRDRTable::dsoNone && dataS3.size()>0) {
                                okLocal=false;
                                switch(dataSelect3Operation) {
                                    case QFRDRTable::dsoEquals:
                                        if (dataS3[i]==dataSelect3CompareValue) okLocal=true;
                                        break;
                                    case QFRDRTable::dsoUnequal:
                                        if (dataS3[i]!=dataSelect3CompareValue) okLocal=true;
                                        break;
                                    case QFRDRTable::dsoGreaterOrEqual:
                                        if (dataS3[i]>=dataSelect3CompareValue) okLocal=true;
                                        break;
                                    case QFRDRTable::dsoSmallerOrEqual:
                                        if (dataS3[i]<=dataSelect3CompareValue) okLocal=true;
                                        break;
                                    case QFRDRTable::dsoGreater:
                                        if (dataS3[i]>dataSelect3CompareValue) okLocal=true;
                                        break;
                                    case QFRDRTable::dsoSmaller:
                                        if (dataS3[i]<dataSelect3CompareValue) okLocal=true;
                                        break;
                                    case QFRDRTable::dsoInRange:
                                        if (dataS3[i]>=dataSelect3CompareValue && dataS3[i]<=dataSelect3CompareValue2) okLocal=true;
                                        break;
                                    case QFRDRTable::dsoOutOfRange:
                                        if (!(dataS3[i]>=dataSelect3CompareValue && dataS3[i]<=dataSelect3CompareValue2)) okLocal=true;
                                        break;
                                }
                                if (dataSelectLogic23==QFRDRTable::dsoAnd) ok=ok&&okLocal;
                                else if (dataSelectLogic23==QFRDRTable::dsoOr) ok=ok||okLocal;
                                else if (dataSelectLogic23==QFRDRTable::dsoXor) ok=(ok&&!okLocal) || (!ok&&okLocal);
                            }
                        }
                    }

                    if (ok) dataO.append(data[i]);
                }
                QString s1=tr("%1 %2 %3").arg(plotter->getDatastore()->getColumnNames().value(dataSelect1Column)).arg(QFRDRTable::DataSelectOperation2String(dataSelect1Operation)).arg(dataSelect1CompareValue);
                QString s2="";
                QString s3="";
                if (dataSelectLogic12!=QFRDRTable::dsoNone) {
                    if (dataSelectLogic12==QFRDRTable::dsoAnd) s2=s2+" && ";
                    if (dataSelectLogic12==QFRDRTable::dsoOr) s2=s2+" || ";
                    if (dataSelectLogic12==QFRDRTable::dsoXor) s2=s2+" ~~ ";
                    s2=s2+tr("%1 %2 %3").arg(plotter->getDatastore()->getColumnNames().value(dataSelect2Column)).arg(QFRDRTable::DataSelectOperation2String(dataSelect2Operation)).arg(dataSelect2CompareValue);
                    if (dataSelectLogic23!=QFRDRTable::dsoNone) {
                        if (dataSelectLogic23==QFRDRTable::dsoAnd) s3=s3+" && ";
                        if (dataSelectLogic23==QFRDRTable::dsoOr) s3=s3+" || ";
                        if (dataSelectLogic23==QFRDRTable::dsoXor) s3=s3+" ~~ ";
                        s3=s3+tr("%1 %2 %3").arg(plotter->getDatastore()->getColumnNames().value(dataSelect3Column)).arg(QFRDRTable::DataSelectOperation2String(dataSelect3Operation)).arg(dataSelect3CompareValue);
                    }
                }
                QString nam= tr("(%2,%3)-strided, selected (%4%5%6) \"%1\"").arg(plotter->getDatastore()->getColumnNames().value(column)).arg(strideStart).arg(stride).arg(s1).arg(s2).arg(s3);
                if (dataO.size()>0) {
                    *colOut= plotter->getDatastore()->addCopiedColumn(dataO.data(), dataO.size(),nam);
                    return true;
                }
                else return false;
            } else {
                if (isStrided) {
                    *colOut= plotter->getDatastore()->copyColumn(column, strideStart-1, stride, tr("(%2,%3)-strided \"%1\"").arg(plotter->getDatastore()->getColumnNames().value(column)).arg(strideStart).arg(stride));
                    return true;
                } else {
                    *colOut=column;
                    return true;
                }
            }
        }
    } else if (dataOut && table) {
        if (column>=0 && column<table->tableGetColumnCount()) {
            QVector<double> data=table->tableGetColumnDataAsDouble(column);
            if (isDataSelect) {
                QVector<double> dataS, dataS2, dataS3;
                if (dataSelect1Column>=0 && dataSelect1Column<(long)table->tableGetColumnCount()) {
                    dataS=table->tableGetColumnDataAsDouble(dataSelect1Column);
                }
                int cnt=qMin(data.size(), dataS.size());
                if (dataSelectLogic12!=QFRDRTable::dsoNone && dataSelect2Column>=0 && dataSelect2Column<(long)table->tableGetColumnCount()) {
                    dataS2=table->tableGetColumnDataAsDouble(dataSelect2Column);
                    cnt=qMin(cnt, dataS2.size());
                }
                if (dataSelectLogic12!=QFRDRTable::dsoNone && dataSelectLogic23!=QFRDRTable::dsoNone && dataSelect3Column>=0 && dataSelect3Column<(long)table->tableGetColumnCount()) {
                    dataS3=table->tableGetColumnDataAsDouble(dataSelect3Column);
                    cnt=qMin(cnt, dataS3.size());
                }
                QVector<double> dataO;
                int istart=0;
                int iinc=1;
                if (isStrided) {
                    istart=strideStart-1;
                    iinc=stride;
                }

                for (int i=istart; i< cnt; i=i+iinc) {
                    bool ok=true;
                    if (dataS.size()>0) {
                        ok=false;
                        switch(dataSelect1Operation) {
                            case QFRDRTable::dsoEquals:
                                if (dataS[i]==dataSelect1CompareValue) ok=true;
                                break;
                            case QFRDRTable::dsoUnequal:
                                if (dataS[i]!=dataSelect1CompareValue) ok=true;
                                break;
                            case QFRDRTable::dsoGreaterOrEqual:
                                if (dataS[i]>=dataSelect1CompareValue) ok=true;
                                break;
                            case QFRDRTable::dsoSmallerOrEqual:
                                if (dataS[i]<=dataSelect1CompareValue) ok=true;
                                break;
                            case QFRDRTable::dsoGreater:
                                if (dataS[i]>dataSelect1CompareValue) ok=true;
                                break;
                            case QFRDRTable::dsoSmaller:
                                if (dataS[i]<dataSelect1CompareValue) ok=true;
                                break;
                            case QFRDRTable::dsoInRange:
                                if (dataS[i]>=dataSelect1CompareValue && dataS[i]<=dataSelect1CompareValue2) ok=true;
                                break;
                            case QFRDRTable::dsoOutOfRange:
                                if (!(dataS[i]>=dataSelect1CompareValue && dataS[i]<=dataSelect1CompareValue2)) ok=true;
                                break;
                        }
                        if (dataSelectLogic12!=QFRDRTable::dsoNone && dataS2.size()>0) {
                            bool okLocal=false;
                            switch(dataSelect2Operation) {
                                case QFRDRTable::dsoEquals:
                                    if (dataS2[i]==dataSelect2CompareValue) okLocal=true;
                                    break;
                                case QFRDRTable::dsoUnequal:
                                    if (dataS2[i]!=dataSelect2CompareValue) okLocal=true;
                                    break;
                                case QFRDRTable::dsoGreaterOrEqual:
                                    if (dataS2[i]>=dataSelect2CompareValue) okLocal=true;
                                    break;
                                case QFRDRTable::dsoSmallerOrEqual:
                                    if (dataS2[i]<=dataSelect2CompareValue) okLocal=true;
                                    break;
                                case QFRDRTable::dsoGreater:
                                    if (dataS2[i]>dataSelect2CompareValue) okLocal=true;
                                    break;
                                case QFRDRTable::dsoSmaller:
                                    if (dataS2[i]<dataSelect2CompareValue) okLocal=true;
                                    break;
                                case QFRDRTable::dsoInRange:
                                    if (dataS2[i]>=dataSelect2CompareValue && dataS2[i]<=dataSelect2CompareValue2) okLocal=true;
                                    break;
                                case QFRDRTable::dsoOutOfRange:
                                    if (!(dataS2[i]>=dataSelect2CompareValue && dataS2[i]<=dataSelect2CompareValue2)) okLocal=true;
                                    break;
                            }
                            if (dataSelectLogic12==QFRDRTable::dsoAnd) ok=ok&&okLocal;
                            else if (dataSelectLogic12==QFRDRTable::dsoOr) ok=ok||okLocal;
                            else if (dataSelectLogic12==QFRDRTable::dsoXor) ok=(ok&&!okLocal) || (!ok&&okLocal);
                            if (dataSelectLogic23!=QFRDRTable::dsoNone && dataS3.size()>0) {
                                okLocal=false;
                                switch(dataSelect3Operation) {
                                    case QFRDRTable::dsoEquals:
                                        if (dataS3[i]==dataSelect3CompareValue) okLocal=true;
                                        break;
                                    case QFRDRTable::dsoUnequal:
                                        if (dataS3[i]!=dataSelect3CompareValue) okLocal=true;
                                        break;
                                    case QFRDRTable::dsoGreaterOrEqual:
                                        if (dataS3[i]>=dataSelect3CompareValue) okLocal=true;
                                        break;
                                    case QFRDRTable::dsoSmallerOrEqual:
                                        if (dataS3[i]<=dataSelect3CompareValue) okLocal=true;
                                        break;
                                    case QFRDRTable::dsoGreater:
                                        if (dataS3[i]>dataSelect3CompareValue) okLocal=true;
                                        break;
                                    case QFRDRTable::dsoSmaller:
                                        if (dataS3[i]<dataSelect3CompareValue) okLocal=true;
                                        break;
                                    case QFRDRTable::dsoInRange:
                                        if (dataS3[i]>=dataSelect3CompareValue && dataS3[i]<=dataSelect3CompareValue2) okLocal=true;
                                        break;
                                    case QFRDRTable::dsoOutOfRange:
                                        if (!(dataS3[i]>=dataSelect3CompareValue && dataS3[i]<=dataSelect3CompareValue2)) okLocal=true;
                                        break;
                                }
                                if (dataSelectLogic23==QFRDRTable::dsoAnd) ok=ok&&okLocal;
                                else if (dataSelectLogic23==QFRDRTable::dsoOr) ok=ok||okLocal;
                                else if (dataSelectLogic23==QFRDRTable::dsoXor) ok=(ok&&!okLocal) || (!ok&&okLocal);
                            }
                        }
                    }

                    if (ok) dataO.append(data[i]);
                }
                QString s1=tr("%1 %2 %3").arg(table->tableGetColumnTitle(dataSelect1Column)).arg(QFRDRTable::DataSelectOperation2String(dataSelect1Operation)).arg(dataSelect1CompareValue);
                QString s2="";
                QString s3="";
                if (dataSelectLogic12!=QFRDRTable::dsoNone) {
                    if (dataSelectLogic12==QFRDRTable::dsoAnd) s2=s2+" && ";
                    if (dataSelectLogic12==QFRDRTable::dsoOr) s2=s2+" || ";
                    if (dataSelectLogic12==QFRDRTable::dsoXor) s2=s2+" ~~ ";
                    s2=s2+tr("%1 %2 %3").arg(table->tableGetColumnTitle(dataSelect2Column)).arg(QFRDRTable::DataSelectOperation2String(dataSelect2Operation)).arg(dataSelect2CompareValue);
                    if (dataSelectLogic23!=QFRDRTable::dsoNone) {
                        if (dataSelectLogic23==QFRDRTable::dsoAnd) s3=s3+" && ";
                        if (dataSelectLogic23==QFRDRTable::dsoOr) s3=s3+" || ";
                        if (dataSelectLogic23==QFRDRTable::dsoXor) s3=s3+" ~~ ";
                        s3=s3+tr("%1 %2 %3").arg(table->tableGetColumnTitle(dataSelect3Column)).arg(QFRDRTable::DataSelectOperation2String(dataSelect3Operation)).arg(dataSelect3CompareValue);
                    }
                }
                QString nam= tr("(%2,%3)-strided, selected (%4%5%6) \"%1\"").arg(table->tableGetColumnTitle(column)).arg(strideStart).arg(stride).arg(s1).arg(s2).arg(s3);
                if (dataO.size()>0) {
                    *dataOut=dataO;
                    if (newname) *newname=nam;
                    return true;
                }
                else return false;
            } else {
                if (isStrided) {
                    if (newname) *newname=tr("(%2,%3)-strided \"%1\"").arg(table->tableGetColumnTitle(column)).arg(strideStart).arg(stride);
                    dataOut->clear();
                    for (int i=strideStart-1; i<data.size(); i=i+stride) {
                        if (i>=0 && i<data.size()) (*dataOut)<<data[i];
                    }
                    return true;
                } else {
                    if (newname) *newname=table->tableGetColumnTitle(column);
                    *dataOut=data;
                    return true;
                }
            }
        }
    }
    if (dataOut) dataOut->clear();
    if (newname) newname->clear();
    if (colOut) *colOut=-1;
    return false;
}

