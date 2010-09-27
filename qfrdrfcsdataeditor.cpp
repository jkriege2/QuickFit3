#include "qfrdrfcsdataeditor.h"
#include "qfrdrfcsdata.h"

QFRDRFCSDataEditor::QFRDRFCSDataEditor(QWidget* parent):
    QFRawDataEditor(parent)
{
    createWidgets();
}

QFRDRFCSDataEditor::~QFRDRFCSDataEditor()
{
    //dtor
}

void QFRDRFCSDataEditor::createWidgets() {
    QHBoxLayout* l=new QHBoxLayout(this);
    setLayout(l);
    plotter = new JKQtPlotter(true, this);
};

void QFRDRFCSDataEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
};



void QFRDRFCSDataEditor::rawDataChanged() {
    replotData();
};

void QFRDRFCSDataEditor::replotData() {
    JKQTPdatastore* ds=plotter->getDatastore();
    plotter->set_emitSignals(false);
    plotter->clearGraphs();
    ds->clear();
    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
    size_t c_tau=ds->addColumn(m->getCorrelationT(), m->getCorrelationN(), "tau");
    size_t c_mean=ds->addColumn(m->getCorrelationMean(), m->getCorrelationN(), "cmean");
    size_t c_std=ds->addColumn(m->getCorrelationStdDev(), m->getCorrelationN(), "cstddev");
    plotter->addGraphWithYError(c_tau, c_mean, c_std, tr("average run"), JKQTPlines, JKQTPerrorLines);
    plotter->set_logXAxis(true);
    plotter->set_xAxisLabel(tr("lag time [µs]"));
    plotter->set_yAxisLabel(tr("correlation function"));
    plotter->set_emitSignals(true);
    plotter->zoomToFit(true, true, false, true);
};


void QFRDRFCSDataEditor::readSettings() {
    plotter->loadSettings(*settings->getQSettings(), "fcsdataeditor_corrplot");
};


void QFRDRFCSDataEditor::writeSettings() {
    plotter->saveSettings(*settings->getQSettings(), "fcsdataeditor_corrplot");
};



