/*
Copyright (c) 2015
	
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


#include "qfrdrcurve_dataeditor.h"
#include "qfrdrcurve_data.h"
#include "ui_qfrdrcurve_dataeditor.h"
#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtCore>

QFRDRCurveDataEditor::QFRDRCurveDataEditor(QFPluginServices* services,  QFRawDataPropertyEditor* propEditor, QWidget* parent):
    QFRawDataEditor(services, propEditor, parent),
    ui(new Ui::QFRDRCurveDataEditor)
{
    ui->setupUi(this);

}

QFRDRCurveDataEditor::~QFRDRCurveDataEditor()
{
    //dtor
}



void QFRDRCurveDataEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* /*old*/) {
    // this functions connects to a new QFRawDataRecord which could also be NULL, so
    // you will have to connect and disconnect the signals! This is called whenever
    // a user navigates to the next record.
    // here is a basic skeleton:
    disConnectWidgets(false);

    QFRDRCurveData* m=qobject_cast<QFRDRCurveData*>(current); // check whether we have the right QFRawDataRecord class

    if (m) {
        // if so (and if current!=NULL anyways), fill the widget with the data
        rawDataChanged();
    } else {
        ui->plotter->clearGraphs();
    }


}

void QFRDRCurveDataEditor::currentCurveChanged(int curve)
{
    //qDebug()<<"currentCurveChanged(curve)";
    // check for correct RDR object in current
    QFRDRCurveData* m=qobject_cast<QFRDRCurveData*>(current);
    if (!m) return;

    disConnectWidgets(false);

    // store current curve (to a non-user-editable/visible QFProperty
    if (curve>=0) m->setQFProperty("QFRDRCurveDataEditor/currentCurve", curve, false, false);

    // set metadata widgets
    ui->edtName->setText(m->curvesGetName(curve));
    ui->edtLabelX->setText(m->curvesGetXLabel(curve));
    ui->edtLabelY->setText(m->curvesGetYLabel(curve));
    ui->chkLogX->setChecked(m->curvesGetLogX(curve));
    ui->chkLogY->setChecked(m->curvesGetLogY(curve));
    ui->cmbType->setCurrentIndex((int)m->curvesGetType(curve));

    // disable plot-updates while we edit the plot data
    ui->plotter->set_doDrawing(false);
    JKQTPdatastore* ds=ui->plotter->getDatastore();

    // clear plots and datastore
    ui->plotter->clearGraphs(true);
    ds->clear();

    // set plot properties
    ui->plotter->getXAxis()->set_axisLabel(m->curvesGetXLabel(curve));
    ui->plotter->getXAxis()->set_logAxis(m->curvesGetLogX(curve));
    ui->plotter->getYAxis()->set_axisLabel(m->curvesGetYLabel(curve));
    ui->plotter->getYAxis()->set_logAxis(m->curvesGetLogY(curve));

    // add new plot:
    switch(m->curvesGetType(curve)) {
        case QFRDRCurvesInterface::ctPoints: {
                JKQTPxyLineErrorGraph* plt=new JKQTPxyLineErrorGraph(ui->plotter);
                plt->set_drawLine(false);
                plt->set_symbol(JKQTPfilledCircle);
                plt->set_title(m->curvesGetName(curve));
                if (m->curvesGetX(curve).size()>0) plt->set_xColumn(ds->addCopiedColumn(m->curvesGetX(curve), "x-data"));
                if (m->curvesGetY(curve).size()>0) plt->set_yColumn(ds->addCopiedColumn(m->curvesGetY(curve), "y-data"));
                if (m->curvesGetXError(curve).size()>0) plt->set_xErrorColumn(ds->addCopiedColumn(m->curvesGetX(curve), "x-error"));
                if (m->curvesGetYError(curve).size()>0) plt->set_yErrorColumn(ds->addCopiedColumn(m->curvesGetX(curve), "y-error"));
                ui->plotter->addGraph(plt);
            } break;
        case QFRDRCurvesInterface::ctLines: {
                JKQTPxyLineErrorGraph* plt=new JKQTPxyLineErrorGraph(ui->plotter);
                plt->set_drawLine(true);
                plt->set_symbol(JKQTPnoSymbol);
                plt->set_title(m->curvesGetName(curve));
                if (m->curvesGetX(curve).size()>0) plt->set_xColumn(ds->addCopiedColumn(m->curvesGetX(curve), "x-data"));
                if (m->curvesGetY(curve).size()>0) plt->set_yColumn(ds->addCopiedColumn(m->curvesGetY(curve), "y-data"));
                if (m->curvesGetXError(curve).size()>0) plt->set_xErrorColumn(ds->addCopiedColumn(m->curvesGetX(curve), "x-error"));
                if (m->curvesGetYError(curve).size()>0) plt->set_yErrorColumn(ds->addCopiedColumn(m->curvesGetX(curve), "y-error"));
                ui->plotter->addGraph(plt);
            } break;
        case QFRDRCurvesInterface::ctBars: {
                JKQTPbarHorizontalGraph* plt=new JKQTPbarHorizontalGraph(ui->plotter);
                plt->set_title(m->curvesGetName(curve));
                if (m->curvesGetX(curve).size()>0) plt->set_xColumn(ds->addCopiedColumn(m->curvesGetX(curve), "x-data"));
                if (m->curvesGetY(curve).size()>0) plt->set_yColumn(ds->addCopiedColumn(m->curvesGetY(curve), "y-data"));
                if (m->curvesGetXError(curve).size()>0) plt->set_xErrorColumn(ds->addCopiedColumn(m->curvesGetX(curve), "x-error"));
                if (m->curvesGetYError(curve).size()>0) plt->set_yErrorColumn(ds->addCopiedColumn(m->curvesGetX(curve), "y-error"));
                ui->plotter->addGraph(plt);
            } break;
     }



    // zoom full range and re-activate plotting and update plot
    ui->plotter->zoomToFit();
    ui->plotter->set_doDrawing(true);
    ui->plotter->update_plot();

    disConnectWidgets(true);
}

void QFRDRCurveDataEditor::curveMetadataEdited()
{
    //qDebug()<<"curveMetadataEdited()";
    // check for correct RDR object in current
    QFRDRCurveData* m=qobject_cast<QFRDRCurveData*>(current);
    if (!m) return;
    // disconnect widgets
    disConnectWidgets(false);
    // write curve data
    int curve=ui->cmbCurve->currentIndex();
    if (curve>=0) {
        m->curvesSetName(curve, ui->edtName->text());
        if (curve>=0 && curve<ui->cmbCurve->count()) ui->cmbCurve->setItemText(curve, ui->edtName->text());
        m->curvesSetXLabel(curve, ui->edtLabelX->text());
        m->curvesSetYLabel(curve, ui->edtLabelY->text());
        m->curvesSetLogX(curve, ui->chkLogX->isChecked());
        m->curvesSetLogY(curve, ui->chkLogY->isChecked());
        m->curvesSetTypeI(curve, ui->cmbType->currentIndex());
    }
    // eat all intermediate events
    QApplication::processEvents();
    // reconnect widgets
    // replot, as the plot may have changed:
    disConnectWidgets(true);
    if (curve>=0) currentCurveChanged(curve);
}

void QFRDRCurveDataEditor::rawDataChanged() {
    // emitted when the raw data changes

    // check for correct RDR object in current
    QFRDRCurveData* m=qobject_cast<QFRDRCurveData*>(current);
    if (!m) return;

    // disconnect widgets, so we can edit their contents
    disConnectWidgets(false);

    // add new curves to cmbCurve
    ui->cmbCurve->clear();
    for (int i=0; i<m->curvesGetCount(); i++) {
        ui->cmbCurve->addItem(tr("%1: %2").arg(i+1).arg(m->curvesGetName(i)));
    }
    ui->cmbCurve->setCurrentIndex(m->getQFProperty("QFRDRCurveDataEditor/currentCurve", 0).toInt()); // read last curve

    // reconnect widgets
    disConnectWidgets(true);

    // update plot/widgets
    currentCurveChanged(ui->cmbCurve->currentIndex());
}

void QFRDRCurveDataEditor::readSettings() {
    // read widget settings
    if (!settings) return;
}


void QFRDRCurveDataEditor::writeSettings() {
    // rwrite widget settings
    if (!settings) return;
}

void QFRDRCurveDataEditor::disConnectWidgets(bool c)
{
    //qDebug()<<"disConnectWidgets("<<c<<")";
    if (!c) {
        disconnect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
        disconnect(ui->cmbCurve, SIGNAL(currentIndexChanged(int)), this, SLOT(currentCurveChanged(int)));
        disconnect(ui->chkLogX, SIGNAL(toggled(bool)), this, SLOT(curveMetadataEdited()));
        disconnect(ui->chkLogY, SIGNAL(toggled(bool)), this, SLOT(curveMetadataEdited()));
        disconnect(ui->edtLabelX, SIGNAL(editingFinished()), this, SLOT(curveMetadataEdited()));
        disconnect(ui->edtLabelY, SIGNAL(editingFinished()), this, SLOT(curveMetadataEdited()));
        disconnect(ui->edtName, SIGNAL(editingFinished()), this, SLOT(curveMetadataEdited()));
        disconnect(ui->cmbType, SIGNAL(currentIndexChanged(int)), this, SLOT(curveMetadataEdited()));
    } else {
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
        connect(ui->cmbCurve, SIGNAL(currentIndexChanged(int)), this, SLOT(currentCurveChanged(int)));
        connect(ui->chkLogX, SIGNAL(toggled(bool)), this, SLOT(curveMetadataEdited()));
        connect(ui->chkLogY, SIGNAL(toggled(bool)), this, SLOT(curveMetadataEdited()));
        connect(ui->edtLabelX, SIGNAL(editingFinished()), this, SLOT(curveMetadataEdited()));
        connect(ui->edtLabelY, SIGNAL(editingFinished()), this, SLOT(curveMetadataEdited()));
        connect(ui->edtName, SIGNAL(editingFinished()), this, SLOT(curveMetadataEdited()));
        connect(ui->cmbType, SIGNAL(currentIndexChanged(int)), this, SLOT(curveMetadataEdited()));
    }
}





