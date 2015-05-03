/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfoverlayplotdialog.h"
#include "ui_qfoverlayplotdialog.h"
#include "programoptions.h"
#include "qfpluginservices.h"
#include "qftools.h"

QFOverlayPlotDialog::QFOverlayPlotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFOverlayPlotDialog)
{
    ui->setupUi(this);
    ui->plot->set_toolbarAlwaysOn(true);

    QSettings* set=ProgramOptions::getInstance()->getQSettings();
    if (set) {
        loadWidgetGeometry(*set, this, pos(), size(), "QFOverlayPlotDialog/windowsize");
    }

    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}

QFOverlayPlotDialog::~QFOverlayPlotDialog()
{
    QSettings* set=ProgramOptions::getInstance()->getQSettings();
    if (set) {
        saveWidgetGeometry(*set, this, "QFOverlayPlotDialog/windowsize");
    }

    delete ui;
}

QFPlotter *QFOverlayPlotDialog::getPlotter() const
{
    return ui->plot;
}

void QFOverlayPlotDialog::clearPlots()
{
    ui->plot->clearGraphs();
    ui->plot->getDatastore()->clear();
}

void QFOverlayPlotDialog::startAddingPlots()
{
    ui->plot->set_doDrawing(false);
}

void QFOverlayPlotDialog::addPlot(const QVector<double> &x, const QVector<double> &y, const QString &name, const QVector<double> &xerror, const QVector<double> &yerror)
{
    QFGetPlotdataInterface::GetPlotDataItem item;
    item.x=x;
    item.y=y;
    item.name=name;
    item.xerrors=xerror;
    item.yerrors=yerror;
    addPlot(item);
}

void QFOverlayPlotDialog::addPlot(const QFGetPlotdataInterface::GetPlotDataItem &plotData)
{
    JKQtBasePlotter* plt=ui->plot->get_plotter();
    JKQTPdatastore* ds=plt->getDatastore();
    if (plotData.x.size()>0 && plotData.y.size()>0) {
        JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(plt);
        g->set_title(plotData.name);
        g->set_xColumn(ds->addCopiedColumn(plotData.x.data(), plotData.x.size(), plotData.name+": X"));
        g->set_yColumn(ds->addCopiedColumn(plotData.y.data(), plotData.y.size(), plotData.name+": Y"));
        g->set_symbol(JKQTPnoSymbol);
        g->set_drawLine(true);
        if (plotData.majorPlot) g->set_lineWidth(2);
        else g->set_lineWidth(1);
        if (plotData.xerrors.size()>0) {
            g->set_xErrorColumn(ds->addCopiedColumn(plotData.xerrors.data(), plotData.xerrors.size(), plotData.name+": XErr"));
            g->set_xErrorStyle(ui->cmbErrorStyleX->getErrorStyle());
        }
        if (plotData.yerrors.size()>0) {
            g->set_yErrorColumn(ds->addCopiedColumn(plotData.yerrors.data(), plotData.yerrors.size(), plotData.name+": YErr"));
            g->set_yErrorStyle(ui->cmbErrorStyleY->getErrorStyle());
        }
        plt->addGraph(g);
    }
}

void QFOverlayPlotDialog::setPlotOptions(const QFGetPlotdataInterface::GetPlotPlotOptions &options)
{
    setLog(options.logX, options.logY);
    setAxisLabel(options.xLabel, options.yLabel);
}

void QFOverlayPlotDialog::endAddingPlots()
{
    ui->plot->set_doDrawing(true);
    ui->plot->zoomToFit();
}

void QFOverlayPlotDialog::setLog(bool logX, bool logY)
{
    ui->plot->getXAxis()->set_logAxis(logX);
    ui->plot->getYAxis()->set_logAxis(logY);
}

void QFOverlayPlotDialog::setAxisLabel(const QString &labX, const QString &labY)
{
    ui->plot->getXAxis()->set_axisLabel(labX);
    ui->plot->getYAxis()->set_axisLabel(labY);
}

QFOverlayPlotDialog *QFOverlayPlotDialog::collectOverlayPlot(QFMatchRDRFunctor */*matchFunctor*/)
{
    return NULL;
}

void QFOverlayPlotDialog::showHelp()
{
    QFPluginServices::getInstance()->displayMainHelpWindow("qfoverlayplot.html");
}

void QFOverlayPlotDialog::closeClicked()
{
    accept();
    deleteLater();
}

void QFOverlayPlotDialog::updatePlot()
{
    startAddingPlots();
    ui->plot->get_plotter()->set_showKey(ui->chkKey->isChecked());
    for (size_t i=0; i<ui->plot->get_plotter()->getGraphCount(); i++) {
        JKQTPgraph* g=ui->plot->get_plotter()->getGraph(i);
        JKQTPxyGraphErrors* eg=dynamic_cast<JKQTPxyGraphErrors*>(g);
        if (eg) {
            if (ui->chkErrors->isChecked()) {
                eg->set_yErrorStyle(ui->cmbErrorStyleX->getErrorStyle());
                eg->set_xErrorStyle(ui->cmbErrorStyleY->getErrorStyle());
            } else {
                eg->set_yErrorStyle(JKQTPnoError);
                eg->set_xErrorStyle(JKQTPnoError);
            }
        }
    }
    endAddingPlots();
}
