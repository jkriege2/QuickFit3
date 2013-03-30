#include "qffcsmsdevaluationfitmsddialog.h"
#include "ui_qffcsmsdevaluationfitmsddialog.h"
#include "qffcsmsdevaluation_item.h"
#include "qfpluginservices.h"

QFFCSMSDEvaluationFitMSDDialog::QFFCSMSDEvaluationFitMSDDialog(QFFCSMSDEvaluationItem* evaluation, int theoryID, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFFCSMSDEvaluationFitMSDDialog)
{
    ui->setupUi(this);
    this->evaluation=evaluation;
    this->theoryID=theoryID;
    dist=evaluation->getMSD(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel());
    distTau=evaluation->getMSDTaus(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel());
    ui->datacut->set_min(0);
    ui->datacut->set_max(qMin(dist.size(), distTau.size()));
    QString param=QString("msd_theory%1_fit_rangemin").arg(theoryID);
    if (evaluation->fitValueExists(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), param)) {
        ui->datacut->set_userMin(evaluation->getFitValue(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), param));
    } else {
        ui->datacut->set_userMin(0);
    }
    param=QString("msd_theory%1_fit_rangemax").arg(theoryID);
    if (evaluation->fitValueExists(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), param)) {
        ui->datacut->set_userMax(evaluation->getFitValue(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), param));
    } else {
        ui->datacut->set_userMax(dist.size()-1);
    }
    ui->edtAlpha->setValue(evaluation->getTheoryAlpha(theoryID));
    ui->edtD->setValue(evaluation->getTheoryD(theoryID));
    ui->edtPre->setValue(evaluation->getTheoryPre(theoryID));
    ui->chkFixAlpha->setChecked(evaluation->getFitFix(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), evaluation->getTheoryAlphaName(theoryID)));
    ui->chkFixD->setChecked(evaluation->getFitFix(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), evaluation->getTheoryDName(theoryID)));
    ui->cmbFitType->addItems(QFFCSMSDEvaluationItem::getFitTypes());
    ui->cmbFitType->setCurrentIndex(evaluation->getFitType());

    replotGraph();
    connectSignals(true);
}

QFFCSMSDEvaluationFitMSDDialog::~QFFCSMSDEvaluationFitMSDDialog()
{
    delete ui;
}

void QFFCSMSDEvaluationFitMSDDialog::saveResults()
{



    bool rc=evaluation->get_doEmitResultsChanged();
    bool pc=evaluation->get_doEmitPropertiesChanged();
    evaluation->set_doEmitResultsChanged(false);
    evaluation->set_doEmitPropertiesChanged(false);
    QString param=QString("msd_theory%1_fit_rangemin").arg(theoryID);
    evaluation->setFitValue(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), param, ui->datacut->get_userMin());
    param=QString("msd_theory%1_fit_rangemax").arg(theoryID);
    evaluation->setFitValue(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), param, ui->datacut->get_userMax());
    evaluation->setTheory(theoryID, true, ui->edtPre->value(), ui->edtD->value(), ui->edtAlpha->value());
    evaluation->setFitFix(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), evaluation->getTheoryAlphaName(theoryID), ui->chkFixAlpha->isChecked());
    evaluation->setFitFix(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), evaluation->getTheoryDName(theoryID), ui->chkFixD->isChecked());
    evaluation->set_doEmitResultsChanged(rc);
    evaluation->set_doEmitPropertiesChanged(pc);
    accept();
}

void QFFCSMSDEvaluationFitMSDDialog::on_btnFit_clicked()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    connectSignals(false);
    double alpha=ui->edtAlpha->value();
    double D=ui->edtD->value();
    evaluation->calcMSDFit(alpha, ui->chkFixAlpha->isChecked(), D, ui->chkFixD->isChecked(), evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), ui->edtPre->value(), ui->datacut->get_userMin(), ui->datacut->get_userMax(), ui->cmbFitType->currentIndex());
    ui->edtAlpha->setValue(alpha);
    ui->edtD->setValue(D);
    connectSignals(true);
    QApplication::restoreOverrideCursor();
    replotGraph();
}

void QFFCSMSDEvaluationFitMSDDialog::replotGraph()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->pltDistribution->set_doDrawing(false);
    ui->pltDistribution->set_emitSignals(false);
    ui->pltDistribution->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [seconds]"));
    ui->pltDistribution->getXAxis()->set_labelFontSize(11);
    ui->pltDistribution->getXAxis()->set_tickLabelFontSize(10);
    ui->pltDistribution->getXAxis()->set_logAxis(true);
    ui->pltDistribution->getYAxis()->set_axisLabel(tr("MSD $\\langle r^2(\\tau)\\rangle$ [\\mu m^2]"));
    ui->pltDistribution->getYAxis()->set_labelFontSize(11);
    ui->pltDistribution->getYAxis()->set_tickLabelFontSize(10);
    ui->pltDistribution->getYAxis()->set_logAxis(true);
    ui->pltDistribution->getXAxis()->set_drawMode1(JKQTPCADMcomplete);
    ui->pltDistribution->get_plotter()->setBorder(1,1,1,1);
    ui->pltDistribution->get_plotter()->set_useAntiAliasingForSystem(true);
    ui->pltDistribution->get_plotter()->set_useAntiAliasingForGraphs(true);
    ui->pltDistribution->get_plotter()->set_keyFontSize(9);
    ui->pltDistribution->get_plotter()->set_keyXMargin(2);
    ui->pltDistribution->get_plotter()->set_keyYMargin(2);
    ui->pltDistribution->get_plotter()->set_keyPosition(JKQTPkeyInsideTopLeft);
    QColor cb("white");
    cb.setAlphaF(0.5);
    ui->pltDistribution->get_plotter()->set_keyBackgroundColor(cb);
    ui->pltDistribution->get_plotter()->set_keyPosition(JKQTPkeyInsideTopLeft);


    ui->pltDistribution->get_plotter()->clearGraphs(true);
    ui->pltDistribution->get_plotter()->clearOverlayElement(true);

    JKQTPdatastore* ds=ui->pltDistribution->get_plotter()->getDatastore();
    ds->clear();
    size_t c_disttau=ds->addCopiedColumn(distTau.data(), distTau.size(), tr("tau [s]"));
    size_t c_dist=ds->addCopiedColumn(dist.data(), dist.size(), tr("MSD [µm²]"));


    JKQTPxyLineGraph* g_dist=new JKQTPxyLineGraph(ui->pltDistribution->get_plotter());
    g_dist->set_drawLine(true);
    g_dist->set_title(tr("data"));
    g_dist->set_xColumn(c_disttau);
    g_dist->set_yColumn(c_dist);
    g_dist->set_color(QColor("blue"));
    ui->pltDistribution->addGraph(g_dist);

    JKQTPoverlayVerticalRange* ovlRange=new JKQTPoverlayVerticalRange(distTau.value(ui->datacut->get_userMin(), distTau.first()), distTau.value(ui->datacut->get_userMax(), distTau.last()), ui->pltDistribution->get_plotter());
    ovlRange->set_inverted(true);
    QColor fillRange=QColor("grey");
    fillRange.setAlphaF(0.5);
    ovlRange->set_fillColor(fillRange);
    ui->pltDistribution->get_plotter()->addOverlayElement(ovlRange);

    JKQTPxFunctionLineGraph* g_fit=new JKQTPxFunctionLineGraph(ui->pltDistribution->get_plotter());
    g_fit->set_drawLine(true);
    g_fit->set_title(tr("power law fit"));
    g_fit->setSpecialFunction(JKQTPxFunctionLineGraph::PowerLaw);
    g_fit->set_color(QColor("red"));
    QVector<double> vecP;
    vecP<<ui->edtPre->value()*ui->edtD->value()<<ui->edtAlpha->value();
    g_fit->set_params(vecP);
    ui->pltDistribution->addGraph(g_fit);



    ui->pltDistribution->zoomToFit();
    ui->pltDistribution->set_doDrawing(true);
    ui->pltDistribution->set_emitSignals(true);
    ui->pltDistribution->update_plot();
    QApplication::restoreOverrideCursor();
}

void QFFCSMSDEvaluationFitMSDDialog::showHelp()
{
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelpDirectory("fcs_msd")+"fit_msd.html");
}

void QFFCSMSDEvaluationFitMSDDialog::connectSignals(bool connectS)
{
    if (connectS) {
        connect(ui->edtPre, SIGNAL(valueChanged(double)), this, SLOT(replotGraph()));
        connect(ui->edtD, SIGNAL(valueChanged(double)), this, SLOT(replotGraph()));
        connect(ui->edtAlpha, SIGNAL(valueChanged(double)), this, SLOT(replotGraph()));
        connect(ui->datacut, SIGNAL(slidersChanged(int,int,int,int)), this, SLOT(replotGraph()));
    } else {
        disconnect(ui->edtPre, SIGNAL(valueChanged(double)), this, SLOT(replotGraph()));
        disconnect(ui->edtD, SIGNAL(valueChanged(double)), this, SLOT(replotGraph()));
        disconnect(ui->edtAlpha, SIGNAL(valueChanged(double)), this, SLOT(replotGraph()));
        disconnect(ui->datacut, SIGNAL(slidersChanged(int,int,int,int)), this, SLOT(replotGraph()));
    }
}
