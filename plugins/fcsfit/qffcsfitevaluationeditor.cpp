#include "qffcsfitevaluationeditor.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "../fcs/qfrdrfcsdatainterface.h"


QFFCSFitEvaluationEditor::QFFCSFitEvaluationEditor(QWidget* parent):
    QFEvaluationEditor(parent)
{
    cmbModel=NULL;
    createWidgets();
}

QFFCSFitEvaluationEditor::~QFFCSFitEvaluationEditor()
{
    //dtor
}


void QFFCSFitEvaluationEditor::createWidgets() {
    QGridLayout* mainLayout=new QGridLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    setLayout(mainLayout);

    /*labRecord=new QLabel(this);
    mainLayout->addWidget(labRecord, 0, 0);*/

    splitPlot=new QSplitter(this),
    mainLayout->addWidget(splitPlot, 0, 0);
    splitPlot->setOrientation(Qt::Vertical);

    pltData=new JKQtPlotter(true, this);
    splitPlot->addWidget(pltData);

    QWidget* w=new QWidget(this);
    QVBoxLayout* vbl=new QVBoxLayout(w);
    vbl->setContentsMargins(0,0,0,0);
    w->setLayout(vbl);

    pltResiduals=new JKQtPlotter(true, this);
    vbl->addWidget(pltResiduals,1);

    datacut=new DataCutSliders(this);
    vbl->addWidget(datacut,0);

    splitPlot->addWidget(w);

    w=new QWidget(this);
    vbl=new QVBoxLayout(w);
    vbl->setContentsMargins(0,0,0,0);
    w->setLayout(vbl);

    QHBoxLayout* hbl=new QHBoxLayout(w);
    hbl->setContentsMargins(0,0,0,0);
    QLabel* l=new QLabel(tr("Fit &Algorithm:"), this);
    int lwidth=l->width()*1.1;
    l->setMaximumWidth(lwidth);
    l->setMinimumWidth(lwidth);
    cmbAlgorithm=new QComboBox(w);
    cmbAlgorithm->setEditable(false);
    l->setBuddy(cmbAlgorithm);
    hbl->addWidget(l);
    hbl->addWidget(cmbAlgorithm);
    btnConfigAlgorithm=new QPushButton(tr("&Configure Algorithm"), w);
    btnConfigAlgorithm->setMaximumWidth(250);
    hbl->addWidget(btnConfigAlgorithm);
    btnAlgorithmHelp=new QPushButton(tr("Algorithm &Help"), w);
    btnAlgorithmHelp->setMaximumWidth(250);
    hbl->addWidget(btnAlgorithmHelp);
    hbl->addStretch();
    vbl->addLayout(hbl);


    hbl=new QHBoxLayout(w);
    hbl->setContentsMargins(0,0,0,0);
    l=new QLabel(tr("Fit &Model:"), this);
    l->setMaximumWidth(lwidth);
    l->setMinimumWidth(lwidth);
    cmbModel=new QComboBox(w);
    cmbModel->setEditable(false);
    l->setBuddy(cmbModel);
    hbl->addWidget(l);
    hbl->addWidget(cmbModel);
    btnModelHelp=new QPushButton(tr("Model H&elp"), w);
    btnModelHelp->setMaximumWidth(250);
    hbl->addWidget(btnModelHelp);
    hbl->addStretch();
    l=new QLabel(tr("&Weight Model:"), this);
    cmbWeights=new QComboBox(w);
    cmbWeights->setEditable(false);
    cmbWeights->addItem(tr("equal weights"));
    cmbWeights->addItem(tr("from std. dev."));
    l->setBuddy(cmbWeights);
    hbl->addWidget(l);
    hbl->addWidget(cmbWeights);
    vbl->addLayout(hbl);


    splitPlot->addWidget(w);

    connect(datacut, SIGNAL(slidersChanged(int, int, int, int)), this, SLOT(slidersChanged(int, int, int, int)));

}


void QFFCSFitEvaluationEditor::connectWidgets(const QFEvaluationItem* current, const QFEvaluationItem* old) {
    if (old!=NULL) {
        disconnect(old, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
    }
    connect(current, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
}

void QFFCSFitEvaluationEditor::resultsChanged() {
}

void QFFCSFitEvaluationEditor::readSettings() {
    if (cmbModel) {
        splitPlot->restoreState(settings->getQSettings()->value("fcsfitevaleditor/splitter_plot", splitPlot->saveState()).toByteArray());
        cmbAlgorithm->setCurrentIndex(settings->getQSettings()->value("fcsfitevaleditor/algorithm", cmbAlgorithm->currentIndex()).toInt());
        cmbModel->setCurrentIndex(settings->getQSettings()->value("fcsfitevaleditor/model", cmbModel->currentIndex()).toInt());
        cmbWeights->setCurrentIndex(settings->getQSettings()->value("fcsfitevaleditor/weights", cmbWeights->currentIndex()).toInt());
        pltData->loadSettings(*settings->getQSettings(), "fcsfitevaleditor/pltdata/");
        pltResiduals->loadSettings(*settings->getQSettings(), "fcsfitevaleditor/pltresiduals/");

    }
}

void QFFCSFitEvaluationEditor::writeSettings() {
    if (cmbModel) {
        settings->getQSettings()->setValue("fcsfitevaleditor/splitter_plot", splitPlot->saveState());
        settings->getQSettings()->setValue("fcsfitevaleditor/algorithm", cmbAlgorithm->currentIndex());
        settings->getQSettings()->setValue("fcsfitevaleditor/model", cmbModel->currentIndex());
        settings->getQSettings()->setValue("fcsfitevaleditor/weights", cmbWeights->currentIndex());
        pltData->saveSettings(*settings->getQSettings(), "fcsfitevaleditor/pltdata/");
        pltResiduals->saveSettings(*settings->getQSettings(), "fcsfitevaleditor/pltresiduals/");
    }
}

void QFFCSFitEvaluationEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {
    QFRDRFCSDataInterface* data=dynamic_cast<QFRDRFCSDataInterface*>(currentRecord);
    QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    disconnect(formerRecord, SIGNAL(rawDataChanged()), this, SLOT(replotData()));
    if (data) {
        //labRecord->setText(tr("<b>current:</b> <i>%1</i>").arg(currentRecord->getName()) );
        connect(currentRecord, SIGNAL(rawDataChanged()), this, SLOT(replotData()));

        datacut->disableSliderSignals();
        datacut->set_min(0);
        datacut->set_max(data->getCorrelationN());
        datacut->set_userMin(currentRecord->getProperty(resultID+"_datacut_min", 0).toInt());
        datacut->set_userMax(currentRecord->getProperty(resultID+"_datacut_max", data->getCorrelationN()).toInt());
        datacut->enableSliderSignals();

    }
    replotData();
}

void QFFCSFitEvaluationEditor::replotData() {
    if (!current) return;
    if (!cmbModel) return;
    QFRDRFCSDataInterface* data=dynamic_cast<QFRDRFCSDataInterface*>(current->getHighlightedRecord());
    QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();

    JKQTPdatastore* ds=pltData->getDatastore();

    if (!data) {
        pltData->clearGraphs();
        ds->clear();
        return;
    }

    pltData->set_doDrawing(false);
    datacut->set_min(0);
    datacut->set_max(data->getCorrelationN());
    pltData->set_emitSignals(false);
    pltData->clearGraphs();
    ds->clear();

    int errorStyle=1;

    if (data->getCorrelationN()>0) {
        size_t c_tau=ds->addColumn(data->getCorrelationT(), data->getCorrelationN(), "tau");


        //////////////////////////////////////////////////////////////////////////////////
        // Plot average + error markers
        //////////////////////////////////////////////////////////////////////////////////
        size_t c_mean=ds->addColumn(data->getCorrelationMean(), data->getCorrelationN(), "cmean");
        size_t c_std=ds->addColumn(data->getCorrelationStdDev(), data->getCorrelationN(), "cstddev");
        JKQTPerrorPlotstyle styl=JKQTPnoError;
        switch (errorStyle) {
            case 1: styl=JKQTPerrorLines; break;
            case 2: styl=JKQTPerrorBars; break;
            case 3: styl=JKQTPerrorBarsLines; break;
        }

        JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph();
        g->set_color(QColor("blue"));
        g->set_errorColor(QColor("blue").lighter());
        g->set_lineWidth(2);
        g->set_xColumn(c_tau);
        g->set_yColumn(c_mean);
        g->set_yErrorColumn(c_std);
        g->set_title(tr("average run"));
        g->set_yErrorStyle(styl);
        g->set_xErrorStyle(JKQTPnoError);
        g->set_datarange_start(datacut->get_userMin());
        g->set_datarange_end(datacut->get_userMax());
        pltData->addGraph(g);


        pltData->getXAxis()->set_logAxis(true);
        pltData->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [{\\mu}s]"));
        pltData->getYAxis()->set_axisLabel(tr("correlation function $g(\\tau)$"));
        pltData->zoomToFit(true, true);
    }
    pltData->set_doDrawing(true);
    pltData->set_emitSignals(true);
    pltData->update_plot();
}

void QFFCSFitEvaluationEditor::slidersChanged(int userMin, int userMax, int min, int max) {
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    current->getHighlightedRecord()->setQFProperty(resultID+"_datacut_min", userMin, false, false);
    current->getHighlightedRecord()->setQFProperty(resultID+"_datacut_max", userMax, false, false);
    replotData();
}

