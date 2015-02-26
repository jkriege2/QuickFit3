/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


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

void QFPlotterPrivate::setErrorProps(int plotid, int graphid, QFRDRColumnGraphsInterface* cols, JKQTPgraph* g, QColor color,  QMap<int, int>& columns) {
    QColor errorColor=color.darker();
    JKQTPxGraphErrors* xeg=dynamic_cast<JKQTPxGraphErrors*>(g);
    JKQTPyGraphErrors* yeg=dynamic_cast<JKQTPyGraphErrors*>(g);
    JKQTPxyGraphErrors* xyeg=dynamic_cast<JKQTPxyGraphErrors*>(g);
    if (yeg) {
        cols->colgraphSetErrorGraphProperties(plotid, graphid, -1, columns.value(yeg->get_yErrorColumn(), -1), (QFRDRColumnGraphsInterface::ErrorGraphTypes)yeg->get_yErrorStyle());
        errorColor=yeg->get_errorColor();
        cols->colgraphSetErrorGraphErrorColumnYAsymmetric(plotid, graphid, !yeg->get_yErrorSymmetric(), yeg->get_yErrorColumnLower());
    } else if (xeg) {
        cols->colgraphSetErrorGraphProperties(plotid, graphid, columns.value(xeg->get_xErrorColumn(), -1),  -1, (QFRDRColumnGraphsInterface::ErrorGraphTypes)xeg->get_xErrorStyle());
        errorColor=xeg->get_errorColor();
        cols->colgraphSetErrorGraphErrorColumnXAsymmetric(plotid, graphid, !xeg->get_xErrorSymmetric(), xeg->get_xErrorColumnLower());
    } else if (xyeg) {
        cols->colgraphSetErrorGraphProperties(plotid, graphid, columns.value(xyeg->get_xErrorColumn(), -1),  columns.value(xyeg->get_yErrorColumn(), -1), (QFRDRColumnGraphsInterface::ErrorGraphTypes)xyeg->get_yErrorStyle());
        errorColor=xyeg->get_errorColor();
        cols->colgraphSetErrorGraphErrorColumnXAsymmetric(plotid, graphid, !xyeg->get_xErrorSymmetric(), xyeg->get_xErrorColumnLower());
        cols->colgraphSetErrorGraphErrorColumnYAsymmetric(plotid, graphid, !xyeg->get_yErrorSymmetric(), xyeg->get_yErrorColumnLower());
    }
    if (errorColor!=QColor("red").darker()) {
        cols->colgraphSetGraphErrorColor(plotid, cols->colgraphGetGraphCount(plotid)-1, errorColor);
    }

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
            bool emitSigT=tab->tableGetDoEmitSignals();
            tab->tableSetDoEmitSignals(false);

            JKQTPdatastore* ds=plotter->getDatastore();
            JKQtBasePlotter* p=plotter->get_plotter();
            int idx=tab->tableGetColumnCount();
            QList<size_t> cids=ds->getColumnIDs();
            for (int i=0; i<cids.size(); i++) {
                bool copyCol=false;
                if (!dlg->copyOnlyPlotData()) copyCol=true;
                else {
                    for (size_t j=0; j<p->getGraphCount(); j++) {
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
                    for (unsigned long long j=0; j<col.getRows(); j++) {
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
            tab->tableSetDoEmitSignals(emitSigT);
        } else {
            QMessageBox::critical(plotter, tr("Add data to table"), tr("No table selected or could not create table!\nCould not add data!"));
            ok=false;
        }
        int plotid=-1;
        if (ok && dlg->getAddGraphs()) {
            if (cols) {
                bool emitSigG=cols->colgraphGetDoEmitSignals();
                cols->colgraphSetDoEmitSignals(false);
                plotid=dlg->getGraphID();
                QString graphtitle;
                JKQtBasePlotter* p=plotter->get_plotter();
                if (dlg->getNewGraph(graphtitle)) {
                    if (graphtitle.isEmpty()) {
                        graphtitle=tr("new graph");
                        if (!p->get_plotLabel().isEmpty()) graphtitle=p->get_plotLabel().isEmpty();
                    }
                    plotid=cols->colgraphGetPlotCount();
                    cols->colgraphAddPlot(graphtitle, p->get_xAxis()->get_axisLabel(), p->get_yAxis()->get_axisLabel(), p->get_xAxis()->get_logAxis(), p->get_yAxis()->get_logAxis());
                }
                cols->colgraphSetPlotXRange(plotid, p->getXMin(), p->getXMax());
                cols->colgraphSetPlotYRange(plotid, p->getYMin(), p->getYMax());
                for (size_t j=0; j<p->getGraphCount(); j++) {
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
                        JKQTPColumnRGBMathImage* colrgbg=dynamic_cast<JKQTPColumnRGBMathImage*>(g);
                        JKQTPColumnOverlayImageEnhanced* colmask=dynamic_cast<JKQTPColumnOverlayImageEnhanced*>(g);
                        JKQTPhorizontalRange* hrange=dynamic_cast<JKQTPhorizontalRange*>(g);
                        JKQTPverticalRange* vrange=dynamic_cast<JKQTPverticalRange*>(g);
                        QFRDRColumnGraphsInterface::ColumnGraphTypes type=QFRDRColumnGraphsInterface::cgtLinesPoints;
                        QFRDRColumnGraphsInterface::ColumnGraphSymbols symbol=QFRDRColumnGraphsInterface::cgsFilledCircle;
                        JKQTPboxplotVerticalGraph* boxV=dynamic_cast<JKQTPboxplotVerticalGraph*>(g);
                        JKQTPboxplotHorizontalGraph* boxH=dynamic_cast<JKQTPboxplotHorizontalGraph*>(g);

                        JKQTPMathImage* ig=dynamic_cast<JKQTPMathImage*>(g);
                        JKQTPRGBMathImage* rgbg=dynamic_cast<JKQTPRGBMathImage*>(g);
                        JKQTPOverlayImage* mask=dynamic_cast<JKQTPOverlayImage*>(g);

                        double symbolSize=15.0;
                        Qt::BrushStyle fillstyle=Qt::SolidPattern;
                        Qt::PenStyle penstyle=Qt::SolidLine;

                        if (xyg) {
                            QColor color=QColor("red");
                            QColor fillColor=color.lighter();
                            QColor errorColor=color.darker();
                            if (xylg) {
                                color=xylg->get_color();
                                fillColor=xylg->get_fillColor();
                                penstyle=xylg->get_style();
                                symbol=(QFRDRColumnGraphsInterface::ColumnGraphSymbols)xylg->get_symbol();
                                symbolSize=xylg->get_symbolSize();
                                if (xylg->get_drawLine() && xylg->get_symbol()!=JKQTPnoSymbol) type=QFRDRColumnGraphsInterface::cgtLinesPoints;
                                else if (xylg->get_drawLine() && xylg->get_symbol()==JKQTPnoSymbol) type=QFRDRColumnGraphsInterface::cgtLines;
                                else if (!xylg->get_drawLine() && xylg->get_symbol()!=JKQTPnoSymbol) type=QFRDRColumnGraphsInterface::cgtPoints;
                            }
                            if (dynamic_cast<JKQTPimpulsesHorizontalGraph*>(g) || dynamic_cast<JKQTPimpulsesVerticalGraph*>(g)) type=QFRDRColumnGraphsInterface::cgtImpulses;
                            if (dynamic_cast<JKQTPfilledCurveXGraph*>(g) || dynamic_cast<JKQTPfilledCurveYGraph*>(g)) {
                                fillstyle=dynamic_cast<JKQTPfilledCurveXGraph*>(g)->get_fillStyle();
                                penstyle=dynamic_cast<JKQTPfilledCurveXGraph*>(g)->get_style();
                            }
                            if (dynamic_cast<JKQTPstepHorizontalGraph*>(g) || dynamic_cast<JKQTPstepVerticalGraph*>(g)) {
                                type=QFRDRColumnGraphsInterface::cgtSteps;
                                fillstyle=dynamic_cast<JKQTPstepHorizontalGraph*>(g)->get_fillStyle();
                                penstyle=dynamic_cast<JKQTPstepHorizontalGraph*>(g)->get_style();
                            }
                            if (dynamic_cast<JKQTPbarHorizontalGraph*>(g) || dynamic_cast<JKQTPbarVerticalGraph*>(g)) {
                                type=QFRDRColumnGraphsInterface::cgtBars;
                                fillstyle=dynamic_cast<JKQTPbarHorizontalGraph*>(g)->get_fillStyle();
                                penstyle=dynamic_cast<JKQTPbarHorizontalGraph*>(g)->get_style();
                            }

                            int xcol=columns.value(xyg->get_xColumn(), -1);
                            int ycol=columns.value(xyg->get_yColumn(), -1);



                            if (yeg) {
                                cols->colgraphAddErrorGraph(plotid, xcol, -1,  ycol, columns.value(yeg->get_yErrorColumn(), -1), type, name, (QFRDRColumnGraphsInterface::ErrorGraphTypes)yeg->get_errorStyle());
                                errorColor=yeg->get_errorColor();
                                //qDebug()<<"addyerrorg "<<yeg->get_yErrorColumn()<<columns.value(yeg->get_yErrorColumn(), -1);
                            } else if (xeg) {
                                cols->colgraphAddErrorGraph(plotid, xcol, columns.value(xeg->get_xErrorColumn(), -1),  ycol, -1, type, name, (QFRDRColumnGraphsInterface::ErrorGraphTypes)xeg->get_errorStyle());
                                errorColor=xeg->get_errorColor();
                                //qDebug()<<"addxerrorg "<<xeg->get_xErrorColumn()<<columns.value(xeg->get_xErrorColumn(), -1);
                            } else if (xyeg) {
                                cols->colgraphAddErrorGraph(plotid, xcol, columns.value(xyeg->get_xErrorColumn(), -1),  ycol, columns.value(xyeg->get_yErrorColumn(), -1), type, name, (QFRDRColumnGraphsInterface::ErrorGraphTypes)xyeg->get_yErrorStyle());
                                errorColor=xyeg->get_errorColor();
                                //qDebug()<<"addxyerrorg "<<xyeg->get_xErrorColumn()<<columns.value(xyeg->get_xErrorColumn(), -1)<<xyeg->get_yErrorColumn()<<columns.value(xyeg->get_yErrorColumn(), -1);
                            } else {
                                cols->colgraphAddGraph(plotid, xcol, ycol, type, name);
                            }
                            //qDebug()<<"color="<<color<<"   fillColor="<<fillColor;
                            if (color!=QColor("red")) {
                                cols->colgraphSetGraphColor(plotid, cols->colgraphGetGraphCount(plotid)-1, color);
                            }
                            if (fillColor!=QColor("red").lighter()) {
                                cols->colgraphSetGraphFillColor(plotid, cols->colgraphGetGraphCount(plotid)-1, fillColor);
                            }
                            if (errorColor!=QColor("red").darker()) {
                                cols->colgraphSetGraphErrorColor(plotid, cols->colgraphGetGraphCount(plotid)-1, errorColor);
                            }
                            cols->colgraphSetGraphSymbol(plotid, cols->colgraphGetGraphCount(plotid)-1, symbol, symbolSize);
                            cols->colgraphSetGraphFillStyle(plotid, cols->colgraphGetGraphCount(plotid)-1, fillstyle);
                            cols->colgraphSetGraphLineStyle(plotid, cols->colgraphGetGraphCount(plotid)-1, penstyle);

                            if (dynamic_cast<JKQTPbarHorizontalGraph*>(g) || dynamic_cast<JKQTPbarVerticalGraph*>(g)) {
                                type=QFRDRColumnGraphsInterface::cgtBars;
                                JKQTPbarHorizontalGraph* bar=dynamic_cast<JKQTPbarHorizontalGraph*>(g);
                                cols->colgraphSetGraphWidth(plotid, cols->colgraphGetGraphCount(plotid)-1, bar->get_width());
                                cols->colgraphSetGraphShift(plotid, cols->colgraphGetGraphCount(plotid)-1, bar->get_shift());

                            }

                            setErrorProps(plotid, cols->colgraphGetGraphCount(plotid)-1, cols, g, color, columns);

                        } else if (colig) {
                            cols->colgraphAddImageGraph(plotid, columns.value(colig->get_imageColumn(),-1), QFRDRColumnGraphsInterface::ImageColorPalette((int)colig->get_palette()), colig->get_x(), colig->get_y(), colig->get_width(), colig->get_height(), colig->get_Nx(), colig->get_title());
                            cols->colgraphSetImageGraphModifier(plotid, cols->colgraphGetGraphCount(plotid)-1, colig->get_modifierColumn(),  QFRDRColumnGraphsInterface::ImageModifierMode((int)colig->get_modifierMode()));
                            cols->colgraphSetImageGraphRange(plotid, cols->colgraphGetGraphCount(plotid)-1, QFRDRColumnGraphsInterface::cgtiDataChannel, colig->get_autoImageRange(), colig->get_imageMin(), colig->get_imageMax());
                            cols->colgraphSetImageGraphRange(plotid, cols->colgraphGetGraphCount(plotid)-1, QFRDRColumnGraphsInterface::cgtiModifierChannel, colig->get_autoModifierRange(), colig->get_modifierMin(), colig->get_modifierMax());
                        } else if (colmask) {
                            cols->colgraphAddImageMaskGraph(plotid, columns.value(colmask->get_imageColumn(),-1), colmask->get_x(), colmask->get_y(), colmask->get_width(), colmask->get_height(), colmask->get_Nx(), colmask->get_title(), colmask->get_trueColor(), colmask->get_falseColor());
                        } else if (colrgbg) {
                            cols->colgraphAddRGBImageGrph(plotid, columns.value(colrgbg->get_imageRColumn(),-1), columns.value(colrgbg->get_imageGColumn(),-1), columns.value(colrgbg->get_imageBColumn(),-1), colrgbg->get_x(), colrgbg->get_y(), colrgbg->get_width(), colrgbg->get_height(), colrgbg->get_Nx(), colrgbg->get_title());
                            cols->colgraphSetImageGraphModifier(plotid, cols->colgraphGetGraphCount(plotid)-1, colrgbg->get_modifierColumn(),  QFRDRColumnGraphsInterface::ImageModifierMode((int)colrgbg->get_modifierMode()));
                            cols->colgraphSetImageGraphRange(plotid, cols->colgraphGetGraphCount(plotid)-1, QFRDRColumnGraphsInterface::cgtiRedChannel, colrgbg->get_autoImageRange(), colrgbg->get_imageMin(), colrgbg->get_imageMax());
                            cols->colgraphSetImageGraphRange(plotid, cols->colgraphGetGraphCount(plotid)-1, QFRDRColumnGraphsInterface::cgtiGreenChannel, colrgbg->get_autoImageRange(), colrgbg->get_imageMinG(), colrgbg->get_imageMaxG());
                            cols->colgraphSetImageGraphRange(plotid, cols->colgraphGetGraphCount(plotid)-1, QFRDRColumnGraphsInterface::cgtiBlueChannel, colrgbg->get_autoImageRange(), colrgbg->get_imageMinB(), colrgbg->get_imageMaxB());
                            //cols->colgraphSetImagePlotRange(graph, cols->colgraphGetPlotCount(graph), QFRDRColumnGraphsInterface::cgtiModifierChannel, colrgbg->get_g, colrgbg->get_modifierMin(), colrgbg->get_modifierMax());
                        } else if (ig && tab) {
                            int cimg=-1;
                            int cmod=-1;
                            if (ig->get_data()) {
                                cimg=tab->tableGetColumnCount();
                                tab->tableSetColumnDataAsDouble(cimg, ig->getDataAsDoubleVector());
                                tab->tableSetColumnTitle(cimg, tr("image data \"%3\", Nx*Ny=%1*%2").arg(ig->get_Nx()).arg(ig->get_Ny()).arg(ig->get_title()));
                            }
                            if (ig->get_dataModifier()) {
                                cmod=tab->tableGetColumnCount();
                                tab->tableSetColumnDataAsDouble(cmod, ig->getDataAsDoubleVector());
                                tab->tableSetColumnTitle(cimg, tr("image modifier data \"%3\" Nx*Ny=%1*%2").arg(ig->get_Nx()).arg(ig->get_Ny()).arg(ig->get_title()));
                            }

                            cols->colgraphAddImageGraph(plotid, cimg, QFRDRColumnGraphsInterface::ImageColorPalette((int)ig->get_palette()), ig->get_x(), ig->get_y(), ig->get_width(), ig->get_height(), ig->get_Nx(), ig->get_title());
                            cols->colgraphSetImageGraphModifier(plotid, cols->colgraphGetGraphCount(plotid)-1, cmod,  QFRDRColumnGraphsInterface::ImageModifierMode((int)ig->get_modifierMode()));
                            cols->colgraphSetImageGraphRange(plotid, cols->colgraphGetGraphCount(plotid)-1, QFRDRColumnGraphsInterface::cgtiDataChannel, ig->get_autoImageRange(), ig->get_imageMin(), ig->get_imageMax());
                            cols->colgraphSetImageGraphRange(plotid, cols->colgraphGetGraphCount(plotid)-1, QFRDRColumnGraphsInterface::cgtiModifierChannel, ig->get_autoModifierRange(), ig->get_modifierMin(), ig->get_modifierMax());
                        } else if (mask && tab) {
                            int cimg=-1;
                            if (mask->get_data()) {
                                cimg=tab->tableGetColumnCount();
                                tab->tableSetColumnDataAsDouble(cimg, mask->getDataAsDoubleVector());
                                tab->tableSetColumnTitle(cimg, tr("image mask data \"%3\", Nx*Ny=%1*%2").arg(mask->get_Nx()).arg(mask->get_Ny()).arg(mask->get_title()));
                            }

                            cols->colgraphAddImageMaskGraph(plotid, cimg, mask->get_x(), mask->get_y(), mask->get_width(), mask->get_height(), mask->get_Nx(), mask->get_title(), mask->get_trueColor(), mask->get_falseColor());
                        } else if (rgbg && tab) {
                            int cimg=-1;
                            int cimgG=-1;
                            int cimgB=-1;
                            int cmod=-1;
                            if (rgbg->get_data()) {
                                cimg=tab->tableGetColumnCount();
                                tab->tableSetColumnDataAsDouble(cimg, rgbg->getDataAsDoubleVector());
                                tab->tableSetColumnTitle(cimg, tr("R image data \"%3\", Nx*Ny=%1*%2").arg(rgbg->get_Nx()).arg(rgbg->get_Ny()).arg(rgbg->get_title()));
                            }
                            if (rgbg->get_dataG()) {
                                cimgG=tab->tableGetColumnCount();
                                tab->tableSetColumnDataAsDouble(cimg, rgbg->getDataGAsDoubleVector());
                                tab->tableSetColumnTitle(cimg, tr("G image data \"%3\", Nx*Ny=%1*%2").arg(rgbg->get_Nx()).arg(rgbg->get_Ny()).arg(rgbg->get_title()));
                            }
                            if (rgbg->get_dataB()) {
                                cimgB=tab->tableGetColumnCount();
                                tab->tableSetColumnDataAsDouble(cimg, rgbg->getDataBAsDoubleVector());
                                tab->tableSetColumnTitle(cimg, tr("B image data \"%3\", Nx*Ny=%1*%2").arg(rgbg->get_Nx()).arg(rgbg->get_Ny()).arg(rgbg->get_title()));
                            }
                            if (rgbg->get_dataModifier()) {
                                cmod=tab->tableGetColumnCount();
                                tab->tableSetColumnDataAsDouble(cmod, rgbg->getDataAsDoubleVector());
                                tab->tableSetColumnTitle(cimg, tr("image modifier data \"%3\" Nx*Ny=%1*%2").arg(rgbg->get_Nx()).arg(rgbg->get_Ny()).arg(rgbg->get_title()));
                            }

                            cols->colgraphAddRGBImageGrph(plotid, cimg, cimgG, cimgB, rgbg->get_x(), rgbg->get_y(), rgbg->get_width(), rgbg->get_height(), rgbg->get_Nx(), rgbg->get_title());
                            cols->colgraphSetImageGraphModifier(plotid, cols->colgraphGetGraphCount(plotid)-1, cmod,  QFRDRColumnGraphsInterface::ImageModifierMode((int)rgbg->get_modifierMode()));
                            cols->colgraphSetImageGraphRange(plotid, cols->colgraphGetGraphCount(plotid)-1, QFRDRColumnGraphsInterface::cgtiRedChannel, rgbg->get_autoImageRange(), rgbg->get_imageMin(), rgbg->get_imageMax());
                            cols->colgraphSetImageGraphRange(plotid, cols->colgraphGetGraphCount(plotid)-1, QFRDRColumnGraphsInterface::cgtiGreenChannel, rgbg->get_autoImageRange(), rgbg->get_imageMinG(), rgbg->get_imageMaxG());
                            cols->colgraphSetImageGraphRange(plotid, cols->colgraphGetGraphCount(plotid)-1, QFRDRColumnGraphsInterface::cgtiBlueChannel, rgbg->get_autoImageRange(), rgbg->get_imageMinB(), rgbg->get_imageMaxB());
                            //cols->colgraphSetImagePlotRange(graph, cols->colgraphGetPlotCount(graph), QFRDRColumnGraphsInterface::cgtiModifierChannel, rgbg->get_g, rgbg->get_modifierMin(), rgbg->get_modifierMax());
                        } else if(qfffg && qfffg->get_fitFunction()) {
                            QColor color=QColor("red");
                            QColor fillColor=color.lighter();
                            QColor errorColor=color.darker();
                            type=QFRDRColumnGraphsInterface::cgtQFFitFunction;
                            color=qfffg->get_color();
                            fillColor=qfffg->get_fillColor();

                            int pcol=columns.value(qfffg->get_parameterColumn(), -1);

                            if (pcol>=0) cols->colgraphAddFunctionGraph(plotid, qfffg->get_fitFunction()->id(), type, qfffg->get_title(), pcol );
                            else cols->colgraphAddFunctionGraph(plotid, qfffg->get_fitFunction()->id(), type, qfffg->get_title(), qfffg->get_internalParams() );

                            if (color!=QColor("red")) {
                                cols->colgraphSetGraphColor(plotid, cols->colgraphGetGraphCount(plotid)-1, color);
                            }
                            if (fillColor!=QColor("red").lighter()) {
                                cols->colgraphSetGraphFillColor(plotid, cols->colgraphGetGraphCount(plotid)-1, fillColor);
                            }
                            if (errorColor!=QColor("red").darker()) {
                                cols->colgraphSetGraphErrorColor(plotid, cols->colgraphGetGraphCount(plotid)-1, errorColor);
                            }
                            fillstyle=qfffg->get_fillStyle();
                            cols->colgraphSetGraphFillStyle(plotid, cols->colgraphGetGraphCount(plotid)-1, fillstyle);
                            penstyle=qfffg->get_style();
                            cols->colgraphSetGraphLineStyle(plotid, cols->colgraphGetGraphCount(plotid)-1, penstyle);

                        } else if(boxH) {
                            QColor color=QColor("red");
                            QColor fillColor=color.lighter();
                            QColor errorColor=color.darker();
                            type=QFRDRColumnGraphsInterface::cgtBoxPlot;
                            color=boxH->get_color();
                            fillColor=boxH->get_fillColor();
                            //symbol=(QFRDRColumnGraphsInterface::ColumnGraphSymbols)boxH->get_symbol();
                            //symbolSize=boxH->get_symbolSize();

                            int pcol=columns.value(boxH->get_posColumn(), -1);
                            int mincol=columns.value(boxH->get_minColumn(), -1);
                            int q25col=columns.value(boxH->get_percentile25Column(), -1);
                            int medcol=columns.value(boxH->get_medianColumn(), -1);
                            int meancol=columns.value(boxH->get_meanColumn(), -1);
                            int q75col=columns.value(boxH->get_percentile75Column(), -1);
                            int maxcol=columns.value(boxH->get_maxColumn(), -1);
                            QFRDRColumnGraphsInterface::Orientation orient=QFRDRColumnGraphsInterface::cgoHorizontal;
                            double width=boxH->get_boxWidth();

                            cols->colgraphAddBoxPlot(plotid, orient, pcol, mincol, q25col, medcol, meancol, q75col, maxcol, name);

                            if (color!=QColor("red")) {
                                cols->colgraphSetGraphColor(plotid, cols->colgraphGetGraphCount(plotid)-1, color);
                            }
                            if (fillColor!=QColor("red").lighter()) {
                                cols->colgraphSetGraphFillColor(plotid, cols->colgraphGetGraphCount(plotid)-1, fillColor);
                            }
                            if (errorColor!=QColor("red").darker()) {
                                cols->colgraphSetGraphErrorColor(plotid, cols->colgraphGetGraphCount(plotid)-1, errorColor);
                            }
                            fillstyle=boxH->get_fillStyle();
                            cols->colgraphSetGraphFillStyle(plotid, cols->colgraphGetGraphCount(plotid)-1, fillstyle);
                            cols->colgraphSetGraphWidth(plotid, cols->colgraphGetGraphCount(plotid)-1, width);
                            setErrorProps(plotid, cols->colgraphGetGraphCount(plotid)-1, cols, g, color, columns);
                            cols->colgraphSetGraphWhiskerLineStyle(plotid, cols->colgraphGetGraphCount(plotid)-1, boxH->get_whiskerStyle());
                            cols->colgraphSetGraphSymbol(plotid, cols->colgraphGetGraphCount(plotid)-1, symbol, symbolSize);

                        } else if(boxV) {
                            QColor color=QColor("red");
                            QColor fillColor=color.lighter();
                            QColor errorColor=color.darker();
                            type=QFRDRColumnGraphsInterface::cgtBoxPlot;
                            color=boxV->get_color();
                            fillColor=boxV->get_fillColor();

                            int pcol=columns.value(boxV->get_posColumn(), -1);
                            int mincol=columns.value(boxV->get_minColumn(), -1);
                            int q25col=columns.value(boxV->get_percentile25Column(), -1);
                            int medcol=columns.value(boxV->get_medianColumn(), -1);
                            int meancol=columns.value(boxV->get_meanColumn(), -1);
                            int q75col=columns.value(boxV->get_percentile75Column(), -1);
                            int maxcol=columns.value(boxV->get_maxColumn(), -1);
                            QFRDRColumnGraphsInterface::Orientation orient=QFRDRColumnGraphsInterface::cgoHorizontal;
                            double width=boxV->get_boxWidth();

                            cols->colgraphAddBoxPlot(plotid, orient, pcol, mincol, q25col, medcol, meancol, q75col, maxcol, name);

                            if (color!=QColor("red")) {
                                cols->colgraphSetGraphColor(plotid, cols->colgraphGetGraphCount(plotid)-1, color);
                            }
                            if (fillColor!=QColor("red").lighter()) {
                                cols->colgraphSetGraphFillColor(plotid, cols->colgraphGetGraphCount(plotid)-1, fillColor);
                            }
                            if (errorColor!=QColor("red").darker()) {
                                cols->colgraphSetGraphErrorColor(plotid, cols->colgraphGetGraphCount(plotid)-1, errorColor);
                            }
                            fillstyle=boxV->get_fillStyle();
                            cols->colgraphSetGraphFillStyle(plotid, cols->colgraphGetGraphCount(plotid)-1, fillstyle);
                            cols->colgraphSetGraphWidth(plotid, cols->colgraphGetGraphCount(plotid)-1, width);
                            cols->colgraphSetGraphWhiskerLineStyle(plotid, cols->colgraphGetGraphCount(plotid)-1, boxH->get_whiskerStyle());
                            cols->colgraphSetGraphLineStyle(plotid, cols->colgraphGetGraphCount(plotid)-1, penstyle);
                            setErrorProps(plotid, cols->colgraphGetGraphCount(plotid)-1, cols, g, color, columns);
                            cols->colgraphSetGraphSymbol(plotid, cols->colgraphGetGraphCount(plotid)-1, symbol, symbolSize);
                        } else if(qfmpg) {
                            QColor color=QColor("red");
                            QColor fillColor=color.lighter();
                            QColor errorColor=color.darker();
                            type=QFRDRColumnGraphsInterface::cgtExpression;
                            color=qfmpg->get_color();
                            fillColor=qfmpg->get_fillColor();


                            int pcol=columns.value(qfmpg->get_parameterColumn(), -1);

                            if (pcol>=0) cols->colgraphAddFunctionGraph(plotid, qfmpg->get_function(), type, qfmpg->get_title(), pcol );
                            else cols->colgraphAddFunctionGraph(plotid, qfmpg->get_function(), type, qfmpg->get_title(), qfmpg->get_internalParams() );

                            if (color!=QColor("red")) {
                                cols->colgraphSetGraphColor(plotid, cols->colgraphGetGraphCount(plotid)-1, color);
                            }
                            if (fillColor!=QColor("red").lighter()) {
                                cols->colgraphSetGraphFillColor(plotid, cols->colgraphGetGraphCount(plotid)-1, fillColor);
                            }
                            if (errorColor!=QColor("red").darker()) {
                                cols->colgraphSetGraphErrorColor(plotid, cols->colgraphGetGraphCount(plotid)-1, errorColor);
                            }
                            fillstyle=qfmpg->get_fillStyle();
                            cols->colgraphSetGraphFillStyle(plotid, cols->colgraphGetGraphCount(plotid)-1, fillstyle);
                            penstyle=qfmpg->get_style();
                            cols->colgraphSetGraphLineStyle(plotid, cols->colgraphGetGraphCount(plotid)-1, penstyle);
                            setErrorProps(plotid, cols->colgraphGetGraphCount(plotid)-1, cols, g, color, columns);
                        } else if (hrange) {
                            QColor color=QColor("red");
                            QColor fillColor=color.lighter();
                            color=hrange->get_color();
                            fillColor=hrange->get_fillColor();
                            QFRDRColumnGraphsInterface::Orientation ori = QFRDRColumnGraphsInterface::cgoHorizontal;
                            if (vrange) ori=QFRDRColumnGraphsInterface::cgoVertical;


                            // ADD PLOT
                            cols->colgraphAddRangeGraph(plotid, ori, hrange->get_rangeMin(), hrange->get_rangeMax(), hrange->get_rangeCenter(), hrange->get_title(),
                                                       hrange->get_invertedRange(), hrange->get_fillRange(), hrange->get_plotRangeLines(), hrange->get_plotCenterLine(),
                                                       hrange->get_centerColor(), hrange->get_centerStyle(), hrange->get_centerLineWidth());

                            if (color!=QColor("red")) {
                                cols->colgraphSetGraphColor(plotid, cols->colgraphGetGraphCount(plotid)-1, color);
                            }
                            if (fillColor!=QColor("red").lighter()) {
                                cols->colgraphSetGraphFillColor(plotid, cols->colgraphGetGraphCount(plotid)-1, fillColor);
                            }
                            fillstyle=hrange->get_fillStyle();
                            cols->colgraphSetGraphFillStyle(plotid, cols->colgraphGetGraphCount(plotid)-1, fillstyle);
                            penstyle=hrange->get_style();
                            cols->colgraphSetGraphLineStyle(plotid, cols->colgraphGetGraphCount(plotid)-1, penstyle);
                        }
                    }
                }
                cols->colgraphSetDoEmitSignals(emitSigG);
            } else {
                QMessageBox::critical(plotter, tr("Add graphs to table"), tr("Could not add plots, as this is not supported by the selected table record!"));
                ok=false;
            }
        }
        if (ok && rdr && dlg->getShowEditor()) {
            QFRawDataPropertyEditor* editor=QFPluginServices::getInstance()->openRawDataEditor(rdr, false);
            if (editor) {
                if (plotid>=0) editor->sendEditorCommand("showPlot", plotid);
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
