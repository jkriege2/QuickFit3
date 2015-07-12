/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    

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

#include "qffcsmsdevaluationfitallmsddialog.h"
#include "ui_qffcsmsdevaluationfitallmsddialog.h"
#include "qffcsmsdevaluation_item.h"
#include "qfpluginservices.h"
#include <QProgressDialog>
#include "qfrdrfcsdatainterface.h"
QFFCSMSDEvaluationFitAllMSDDialog::QFFCSMSDEvaluationFitAllMSDDialog(QFFCSMSDEvaluationItem* evaluation, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFFCSMSDEvaluationFitAllMSDDialog)
{
    ui->setupUi(this);
    this->evaluation=evaluation;
    dist=evaluation->getMSD(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel());
    distTau=evaluation->getMSDTaus(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel());

    groupBoxs<<ui->grpTheory1<<ui->grpTheory1_2<<ui->grpTheory1_3<<ui->grpTheory1_4;
    fixAlphas<<ui->chkFixAlpha<<ui->chkFixAlpha_2<<ui->chkFixAlpha_3<<ui->chkFixAlpha_4;
    fixDs<<ui->chkFixD<<ui->chkFixD_2<<ui->chkFixD_3<<ui->chkFixD_4;
    Ds<<ui->edtD<<ui->edtD_2<<ui->edtD_3<<ui->edtD_4;
    Alphas<<ui->edtAlpha<<ui->edtAlpha_2<<ui->edtAlpha_3<<ui->edtAlpha_4;
    Ps<<ui->edtPre<<ui->edtPre_2<<ui->edtPre_3<<ui->edtPre_4;
    Ranges<<ui->datacut<<ui->datacut_2<<ui->datacut_3<<ui->datacut_4;

    //setUpdatesEnabled(false);

    for (int i=0; i<groupBoxs.size(); i++) {
        Ranges[i]->set_min(distTau.first()/10000.0);;
        Ranges[i]->set_max(distTau.last()*10000.0);
        Ranges[i]->setLogScale(true, 20);
        Ranges[i]->setCentralText(tr("..."));

        QString param=QString("msd_theory%1_fit_trangemin").arg(i);
        if (evaluation->fitValueExists(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), param)) {
            Ranges[i]->set_userMin(evaluation->getFitValue(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), param));
        } else {
            param=QString("msd_theory%1_fit_rangemin").arg(i);
            if (evaluation->fitValueExists(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), param)) {
                Ranges[i]->set_userMin(evaluation->getFitValue(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), param));
            } else {
                Ranges[i]->set_userMin(Ranges[i]->get_min());
            }
        }

        param=QString("msd_theory%1_fit_trangemax").arg(i);
        if (evaluation->fitValueExists(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), param)) {
            Ranges[i]->set_userMax(evaluation->getFitValue(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), param));
        } else {
            param=QString("msd_theory%1_fit_rangemax").arg(i);
            if (evaluation->fitValueExists(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), param)) {
                Ranges[i]->set_userMax(evaluation->getFitValue(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), param));
            } else {
                Ranges[i]->set_userMax(Ranges[i]->get_max());
            }
        }
        Alphas[i]->setValue(evaluation->getTheoryAlpha(i, evaluation->getHighlightedRecord(), evaluation->getCurrentIndex()));
        Ds[i]->setValue(evaluation->getTheoryD(i, evaluation->getHighlightedRecord(), evaluation->getCurrentIndex()));
        Ps[i]->setValue(evaluation->getTheoryPre(i, evaluation->getHighlightedRecord(), evaluation->getCurrentIndex()));
        fixAlphas[i]->setChecked(evaluation->getFitFix(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), evaluation->getTheoryAlphaName(i, evaluation->getHighlightedRecord(), evaluation->getCurrentIndex())));
        fixDs[i]->setChecked(evaluation->getFitFix(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), evaluation->getTheoryDName(i, evaluation->getHighlightedRecord(), evaluation->getCurrentIndex())));

    }

    ui->cmbFitType->addItems(QFFCSMSDEvaluationItem::getFitTypes());
    ui->cmbFitType->setCurrentIndex(evaluation->getFitType(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex()));

    replotGraph();
    connectSignals(true);
    //setUpdatesEnabled(true);
}

QFFCSMSDEvaluationFitAllMSDDialog::~QFFCSMSDEvaluationFitAllMSDDialog()
{
    delete ui;
}

struct QFFCSMSDEvaluationFitAllMSDDialogAverageFirstFewFramesData {
        QFRawDataRecord* record;
        QFRDRFCSDataInterface* data;
        int run;
};

void QFFCSMSDEvaluationFitAllMSDDialog::performFit()
{
    bool rc=evaluation->get_doEmitResultsChanged();
    bool pc=evaluation->get_doEmitPropertiesChanged();
    evaluation->set_doEmitResultsChanged(false);
    evaluation->set_doEmitPropertiesChanged(false);


    QProgressDialog progress(tr("fit all MSDs"), tr("&Cancel"), 0, 100, this);
    progress.show();

    QList<QFFCSMSDEvaluationFitAllMSDDialogAverageFirstFewFramesData > applyTo;
    if (ui->cmbApplyTo->currentIndex()==0) { // current
        QFFCSMSDEvaluationFitAllMSDDialogAverageFirstFewFramesData dr;
        dr.record=evaluation->getHighlightedRecord();
        dr.data=qobject_cast<QFRDRFCSDataInterface*>(dr.record);
        dr.run=evaluation->getCurrentIndex();
        applyTo.append(dr);
    } else if (ui->cmbApplyTo->currentIndex()==1) { // all runs
        for (int i=evaluation->getIndexMin(evaluation->getHighlightedRecord()); i<=evaluation->getIndexMax(evaluation->getHighlightedRecord()); i++) {
            QFFCSMSDEvaluationFitAllMSDDialogAverageFirstFewFramesData dr;
            dr.record=evaluation->getHighlightedRecord();
            dr.data=qobject_cast<QFRDRFCSDataInterface*>(dr.record);
            dr.run=i;
            applyTo.append(dr);
        }
    } else if (ui->cmbApplyTo->currentIndex()==2) { // all files, this runs
        QList<QPointer<QFRawDataRecord> > recs=evaluation->getApplicableRecords();
        for (int i=0; i<recs.size(); i++) {
            QFFCSMSDEvaluationFitAllMSDDialogAverageFirstFewFramesData dr;
            dr.record=recs[i];
            dr.data=qobject_cast<QFRDRFCSDataInterface*>(dr.record);
            dr.run=evaluation->getCurrentIndex();
            if (dr.record&&dr.data && evaluation->getIndexMin(dr.record)<=dr.run && dr.run<=evaluation->getIndexMax(dr.record)) applyTo.append(dr);
        }
    } else if (ui->cmbApplyTo->currentIndex()==3) { // everything
        QList<QPointer<QFRawDataRecord> > recs=evaluation->getApplicableRecords();
        for (int i=0; i<recs.size(); i++) {
            for (int r=evaluation->getIndexMin(recs[i]); r<=evaluation->getIndexMax(recs[i]); r++) {
                QFFCSMSDEvaluationFitAllMSDDialogAverageFirstFewFramesData dr;
                dr.record=recs[i];
                dr.data=qobject_cast<QFRDRFCSDataInterface*>(dr.record);
                dr.run=r;
                if (dr.record&&dr.data && evaluation->getIndexMin(dr.record)<=dr.run && dr.run<=evaluation->getIndexMax(dr.record)) applyTo.append(dr);
            }
        }
    }

    progress.setRange(0, applyTo.size()-1);
    progress.setValue(0);
    for (int i=0; i<applyTo.size(); i++) {

        for (int theoryID=0; theoryID<groupBoxs.size(); theoryID++) {
            if (groupBoxs[theoryID]->isChecked()) {
                double alpha=Alphas[theoryID]->value();
                double D=Ds[theoryID]->value();

                int rmin=getRangeMin(evaluation->getMSDTaus(applyTo[i].record, applyTo[i].run, evaluation->getCurrentModel()), Ranges[theoryID]);
                int rmax=getRangeMax(evaluation->getMSDTaus(applyTo[i].record, applyTo[i].run, evaluation->getCurrentModel()), Ranges[theoryID]);

                evaluation->calcMSDFit(alpha, fixAlphas[theoryID]->isChecked(), D, fixDs[theoryID]->isChecked(), applyTo[i].record, applyTo[i].run, evaluation->getCurrentModel(), Ps[theoryID]->value(), rmin, rmax, ui->cmbFitType->currentIndex());
                evaluation->setTheory(theoryID, true, Ps[theoryID]->value(), D, alpha, applyTo[i].record, applyTo[i].run);
                //qDebug()<<applyTo[i].run;

                QString param=QString("msd_theory%1_fit_trangemin").arg(theoryID);
                evaluation->setFitValue(applyTo[i].record, applyTo[i].run, evaluation->getCurrentModel(), param, ui->datacut->get_userMin());
                param=QString("msd_theory%1_fit_trangemax").arg(theoryID);
                evaluation->setFitValue(applyTo[i].record, applyTo[i].run, evaluation->getCurrentModel(), param, ui->datacut->get_userMax());
                evaluation->setFitFix(applyTo[i].record, applyTo[i].run, evaluation->getCurrentModel(), evaluation->getTheoryAlphaName(theoryID, applyTo[i].record, applyTo[i].run), fixAlphas[theoryID]->isChecked());
                evaluation->setFitFix(applyTo[i].record, applyTo[i].run, evaluation->getCurrentModel(), evaluation->getTheoryDName(theoryID, applyTo[i].record, applyTo[i].run), fixDs[theoryID]->isChecked());
            }
        }


        //if (i%5==0) {
            QApplication::processEvents(QEventLoop::AllEvents, 10);
            if (progress.wasCanceled()) break;
        //}
    }

    /*
    double alpha=ui->edtAlpha->value();
    double D=ui->edtD->value();

    int rmin=getRangeMin();
    int rmax=getRangeMax();

    evaluation->calcMSDFit(alpha, ui->chkFixAlpha->isChecked(), D, ui->chkFixD->isChecked(), evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), ui->edtPre->value(), rmin, rmax, ui->cmbFitType->currentIndex());
    ui->edtAlpha->setValue(alpha);
    ui->edtD->setValue(D);




    QString param=QString("msd_theory%1_fit_trangemin").arg(theoryID);
    evaluation->setFitValue(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), param, ui->datacut->get_userMin());
    param=QString("msd_theory%1_fit_trangemax").arg(theoryID);
    evaluation->setFitValue(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), param, ui->datacut->get_userMax());
    evaluation->setTheory(theoryID, true, ui->edtPre->value(), ui->edtD->value(), ui->edtAlpha->value(), evaluation->getHighlightedRecord(), evaluation->getCurrentIndex());
    evaluation->setFitFix(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), evaluation->getTheoryAlphaName(theoryID, evaluation->getHighlightedRecord(), evaluation->getCurrentIndex()), ui->chkFixAlpha->isChecked());
    evaluation->setFitFix(evaluation->getHighlightedRecord(), evaluation->getCurrentIndex(), evaluation->getCurrentModel(), evaluation->getTheoryDName(theoryID, evaluation->getHighlightedRecord(), evaluation->getCurrentIndex()), ui->chkFixD->isChecked());
*/

    evaluation->set_doEmitResultsChanged(rc);
    evaluation->set_doEmitPropertiesChanged(pc);
    accept();
}


void QFFCSMSDEvaluationFitAllMSDDialog::replotGraph()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->pltDistribution->set_doDrawing(false);
    ui->pltDistribution->set_emitSignals(false);
    ui->pltDistribution->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [seconds]"));
    ui->pltDistribution->getXAxis()->set_labelFontSize(11);
    ui->pltDistribution->getXAxis()->set_tickLabelFontSize(10);
    ui->pltDistribution->getXAxis()->set_logAxis(true);
    ui->pltDistribution->getYAxis()->set_axisLabel(tr("MSD $\\langle r^2(\\tau)\\rangle$ [{\\mu}m^2]"));
    ui->pltDistribution->getYAxis()->set_labelFontSize(11);
    ui->pltDistribution->getYAxis()->set_tickLabelFontSize(10);
    ui->pltDistribution->getYAxis()->set_logAxis(true);
    ui->pltDistribution->getXAxis()->set_drawMode1(JKQTPCADMcomplete);
    ui->pltDistribution->get_plotter()->setBorder(1,1,1,1);
    ui->pltDistribution->get_plotter()->set_useAntiAliasingForSystem(true);
    ui->pltDistribution->get_plotter()->set_useAntiAliasingForGraphs(true);
    ui->pltDistribution->get_plotter()->set_keyFontSize(9);
    //ui->pltDistribution->get_plotter()->set_keyXMargin(2);
    //ui->pltDistribution->get_plotter()->set_keyYMargin(2);
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
    size_t c_dist=ds->addCopiedColumn(dist.data(), dist.size(), QLatin1String("MSD [\xB5m^2]"));


    JKQTPxyLineGraph* g_dist=new JKQTPxyLineGraph(ui->pltDistribution->get_plotter());
    g_dist->set_drawLine(true);
    g_dist->set_title(tr("data"));
    g_dist->set_xColumn(c_disttau);
    g_dist->set_yColumn(c_dist);
    g_dist->set_color(QColor("blue"));
    ui->pltDistribution->addGraph(g_dist);

    JKQTPoverlayVerticalRange* ovlRange;

    for (int i=0; i<groupBoxs.size(); i++) {
        if (groupBoxs[i]->isChecked()) {
            ovlRange=new JKQTPoverlayVerticalRange(Ranges[i]->get_userMin(), Ranges[i]->get_userMax(), ui->pltDistribution->get_plotter());
            ovlRange->set_inverted(false);
            QColor fillRange=QColor("red");
            if (i==1) fillRange=QColor("green");
            if (i==2) fillRange=QColor("blue");
            if (i==3) fillRange=QColor("magenta");
            fillRange.setAlphaF(0.2);
            ovlRange->set_fillColor(fillRange);
            ui->pltDistribution->get_plotter()->addOverlayElement(ovlRange);

            JKQTPxFunctionLineGraph* g_fit=new JKQTPxFunctionLineGraph(ui->pltDistribution->get_plotter());
            g_fit->set_drawLine(true);
            g_fit->set_title(tr("power law fit"));
            g_fit->setSpecialFunction(JKQTPxFunctionLineGraph::PowerLaw);
            fillRange.setAlphaF(1);
            g_fit->set_color(QColor(fillRange));
            QVector<double> vecP;
            vecP<<Ps[i]->value()*Ds[i]->value()<<Alphas[i]->value();
            g_fit->set_params(vecP);
            ui->pltDistribution->addGraph(g_fit);
        }
    }



    ui->pltDistribution->zoomToFit();
    ui->pltDistribution->set_doDrawing(true);
    ui->pltDistribution->set_emitSignals(true);
    ui->pltDistribution->update_plot();
    QApplication::restoreOverrideCursor();
}

void QFFCSMSDEvaluationFitAllMSDDialog::showHelp()
{
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelpDirectory("fcs_msd")+"fit_msdall.html");
}

void QFFCSMSDEvaluationFitAllMSDDialog::connectSignals(bool connectS)
{
    for (int i=0; i<groupBoxs.size(); i++) {
        if (connectS) {
            connect(Ps[i], SIGNAL(valueChanged(double)), this, SLOT(replotGraph()));
            connect(Ds[i], SIGNAL(valueChanged(double)), this, SLOT(replotGraph()));
            connect(Alphas[i], SIGNAL(valueChanged(double)), this, SLOT(replotGraph()));
            connect(Ranges[i], SIGNAL(slidersChanged(double,double,double,double)), this, SLOT(replotGraph()));
        } else {
            disconnect(Ps[i], SIGNAL(valueChanged(double)), this, SLOT(replotGraph()));
            disconnect(Ds[i], SIGNAL(valueChanged(double)), this, SLOT(replotGraph()));
            disconnect(Alphas[i], SIGNAL(valueChanged(double)), this, SLOT(replotGraph()));
            disconnect(Ranges[i], SIGNAL(slidersChanged(double,double,double,double)), this, SLOT(replotGraph()));
        }
    }
}

int QFFCSMSDEvaluationFitAllMSDDialog::getRangeMin(const QVector<double>& distTau, DoubleDataCutSliders* sliders)
{
    int rm=0;
    for (int i=0; i<distTau.size(); i++) {
        if (distTau[i]>sliders->get_userMin()) {
            rm=i;
            break;
        }
    }
    return qBound(rm, 0, distTau.size()-1);
}

int QFFCSMSDEvaluationFitAllMSDDialog::getRangeMax(const QVector<double> &distTau, DoubleDataCutSliders *sliders)
{
    int rm=distTau.size()-1;
    for (int i=distTau.size()-1; i>=0; i--) {
        if (distTau[i]<=sliders->get_userMax()) {
            rm=i;
            break;
        }
    }

    return qBound(0, rm, distTau.size()-1);
}
