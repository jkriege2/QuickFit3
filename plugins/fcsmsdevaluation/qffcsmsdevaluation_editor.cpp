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

#include "qffcsmsdevaluation_editor.h"
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "qffcsmsdevaluation_item.h"
#include "qffcsmsdevaluationfitmsddialog.h"
#include "qfselectionlistdialog.h"
#include "csvtools.h"
#include "qffcsmsdevaluationaveragechannelsdialog.h"
#include "qffcsmsdevaluationgetnfromfits.h"
#include "qffcsmsdevaluationfitallmsddialog.h"

/////////////
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "qfrdrfcsdatainterface.h"
#include <iostream>
#include <cfloat>
#include "tools.h"
#include "statistics_tools.h"
#include <QThread>
#include "qmoretextobject.h"
#include "qmodernprogresswidget.h"
#include "qffitfunctionmanager.h"
#include "qffitalgorithmmanager.h"
////////////





#include <QtGui>
#include <QtCore>

QFFCSMSDEvaluationEditor::QFFCSMSDEvaluationEditor(QFPluginServices *services, QFEvaluationPropertyEditor *propEditor, QWidget *parent):
    QFFCSByIndexAndModelEvaluationEditor(services, propEditor, parent, "fcsmsdevaleditor/")
{
    dataEventsEnabled=false;
    createWidgets();
    dataEventsEnabled=true;
}

QFFCSMSDEvaluationEditor::~QFFCSMSDEvaluationEditor()
{
}

void QFFCSMSDEvaluationEditor::getPlotData(QFRawDataRecord *record, int index, QList<QFGetPlotdataInterface::GetPlotDataItem> &plotdata, int option)
{
    if (!current) return;
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    int model=eval->getCurrentModel();

    if ((!eval)||(!data)) {
        return;
    }

    int datacut_min=getUserMin(record, index);
    int datacut_max=getUserMax(record, index);
    int data_start=sliderDist->get_userMin();
    int data_end=sliderDist->get_userMax();
    int msd_start=sliderDist->get_userMin();
    int msd_end=sliderDist->get_userMax();

    int wid=spinFitWidth->value();
    int first=0;

    try {

        if (data->getCorrelationN()>0) {
            QVector<double> acftau=arrayToVector(data->getCorrelationT(), data->getCorrelationN());


            long N=data->getCorrelationN();
            double* tauvals=data->getCorrelationT();
            double* corrdata=NULL;

            if (index<0) {
                corrdata=data->getCorrelationMean();
            } else {
                if (index<(int)data->getCorrelationRuns()) {
                    corrdata=data->getCorrelationRun(index);
                } else {
                    corrdata=data->getCorrelationMean();
                }
            }
            QVector<double> acf;
            if (corrdata) acf=arrayToVector(corrdata, N);

            double* weights=eval->allocWeights(NULL, record, index, datacut_min, datacut_max);


            /////////////////////////////////////////////////////////////////////////////////
            // calculate fit statistics
            /////////////////////////////////////////////////////////////////////////////////

            double* modelVals=(double*)qfMalloc(N*sizeof(double));
            QVector<double> dist=eval->getMSD(record, index, model);
            QVector<double> distTau=eval->getMSDTaus(record, index, model);
            QVector<double> fitTau, fitTauStart, fitTauEnd;
            QVector<double> fitD, fitA;
            eval->evaluateModel(record, index, model, tauvals, modelVals, N, distTau.data(), dist.data(), dist.size());

            int last=distTau.size()-1;
            if (chkFitRange->isChecked()) {
                first=sliderDist->get_userMin();
                last=sliderDist->get_userMax();
            }



            double N_particle=eval->getFitValue(record, index, model, "n_particle");
            if (N_particle<=0) {
                N_particle=1;
            }


            if (option==0) {
                QFGetPlotdataInterface::GetPlotDataItem item;
                item.x=distTau;
                item.y=dist;
                item.name=QString("\\verb{%1: MSD}").arg(record->getName());
                plotdata.append(item);
            } else if (option==1) {
                QFGetPlotdataInterface::GetPlotDataItem item;
                item.x=acftau;
                item.y=acf;
                item.yerrors=arrayToVector(weights, N);
                item.name=QString("\\verb{%1: ACF}").arg(record->getName());
                plotdata.append(item);
            } else if (option==2) {
                QFGetPlotdataInterface::GetPlotDataItem item;
                item.x=acftau;
                item.y=acf;
                item.yerrors=arrayToVector(weights, N);
                for (int i=0; i<item.y.size(); i++) {
                    item.y[i]=item.y[i]*N_particle;
                }
                for (int i=0; i<item.yerrors.size(); i++) {
                    item.yerrors[i]=item.yerrors[i]*N_particle;
                }
                item.name=QString("\\verb{%1: norm. ACF}").arg(record->getName());
                plotdata.append(item);
            } else if (option>2 && distTau.size()>1 && dist.size()>1 && wid>=3 && eval->hasResults()) {

                eval->calcMSDFits(fitTau, fitA, fitD, record, index, eval->getCurrentModel(), eval->getFitWidth(record, index) , qMax(1, eval->getFitWidth(record, index)/7), first, eval->getFitType(record, index), &fitTauStart, &fitTauEnd);

                QVector<double> mored;
                if (option==3) {
                    mored=dist;
                    for (int i=0; i<dist.size(); i++) {
                        mored[i]=dist[i]/(6.0*distTau[i]);
                    }
                    QFGetPlotdataInterface::GetPlotDataItem item;
                    item.x=distTau;
                    item.y=mored;
                    item.name=QString("\\verb{%1:} \\langle r^2(\\tau)\\rangle/(6\\tau)").arg(record->getName());
                    plotdata.append(item);
                } else if (option==4) {
                    mored=dist;
                    for (int i=0; i<dist.size(); i++) {
                        mored[i]=dist[i]/(4.0*distTau[i]);
                    }
                    QFGetPlotdataInterface::GetPlotDataItem item;
                    item.x=distTau;
                    item.y=mored;
                    item.name=QString("\\verb{%1:} \\langle r^2(\\tau)\\rangle/(4\\tau)").arg(record->getName());
                    plotdata.append(item);

                } else if (option==5) {
                    mored=dist;
                    double P=eval->getTheoryPre(0, record, index);
                    double D=eval->getTheoryD(0, record, index);
                    for (int i=0; i<dist.size(); i++) {
                        mored[i]=dist[i]/(P*D*distTau[i]);
                    }
                    QFGetPlotdataInterface::GetPlotDataItem item;
                    item.x=distTau;
                    item.y=mored;
                    item.name=QString("\\verb{%1:} \\langle r^2(\\tau)\\rangle/(P\\cdot D_0\\cdot\\tau)").arg(record->getName());
                    plotdata.append(item);
                } else if (option==6) {
                    mored=dist;
                    double P=eval->getTheoryPre(1, record, index);
                    double D=eval->getTheoryD(1, record, index);
                    for (int i=0; i<dist.size(); i++) {
                        mored[i]=dist[i]/(P*D*distTau[i]);
                    }
                    QFGetPlotdataInterface::GetPlotDataItem item;
                    item.x=distTau;
                    item.y=mored;
                    item.name=QString("\\verb{%1:} \\langle r^2(\\tau)\\rangle/(P\\cdot D_1\\cdot\\tau)").arg(record->getName());
                    plotdata.append(item);

                } else if (option==7) {
                    mored=dist;
                    double P=eval->getTheoryPre(2, record, index);
                    double D=eval->getTheoryD(2, record, index);
                    for (int i=0; i<dist.size(); i++) {
                        mored[i]=dist[i]/(P*D*distTau[i]);
                    }
                    QFGetPlotdataInterface::GetPlotDataItem item;
                    item.x=distTau;
                    item.y=mored;
                    item.name=QString("\\verb{%1:} \\langle r^2(\\tau)\\rangle/(P\\cdot D_2\\cdot\\tau)").arg(record->getName());
                    plotdata.append(item);
                } else if (option==8) {
                    mored=dist;
                    double P=eval->getTheoryPre(3, record, index);
                    double D=eval->getTheoryD(3, record, index);
                    for (int i=0; i<dist.size(); i++) {
                        mored[i]=dist[i]/(P*D*distTau[i]);
                    }
                    QFGetPlotdataInterface::GetPlotDataItem item;
                    item.x=distTau;
                    item.y=mored;
                    item.name=QString("\\verb{%1:} \\langle r^2(\\tau)\\rangle/(P\\cdot D_3\\cdot\\tau)").arg(record->getName());
                    plotdata.append(item);
                } else if (option==9) {
                    mored=dist;
                    double P=eval->getTheoryPre(0, record, index);
                    double D=eval->getTheoryD(0, record, index);
                    for (int i=0; i<dist.size(); i++) {
                        mored[i]=dist[i]-(P*D*distTau[i]);
                    }
                    QFGetPlotdataInterface::GetPlotDataItem item;
                    item.x=distTau;
                    item.y=mored;
                    item.name=QString("\\verb{%1:} \\langle r^2(\\tau)\\rangle-P\\cdot D_0\\cdot\\tau").arg(record->getName());
                    plotdata.append(item);
                } else if (option==10) {
                    QFGetPlotdataInterface::GetPlotDataItem item;
                    item.x=fitTau;
                    item.y=fitD;
                    item.name=QString("\\verb{%1: local D}").arg(record->getName());
                    plotdata.append(item);
                } else if (option==11) {
                    QFGetPlotdataInterface::GetPlotDataItem item;
                    item.x=fitTau;
                    item.y=fitA;
                    item.name=QString("\\verb{%1: local \\alpha}").arg(record->getName());
                    plotdata.append(item);
                }

            }


            qfFree(weights);
            //fit_stat.free();

        }
    } catch(std::exception& E) {
        services->log_error(tr("error during plotting, error message: %1\n").arg(E.what()));
    }
}

bool QFFCSMSDEvaluationEditor::getPlotDataSpecs(QStringList *optionNames, QList<QFGetPlotdataInterface::GetPlotPlotOptions> *listPlotOptions)
{
    if (optionNames) {
        *optionNames<<tr("mean squared displacements (MSDs)");
        *optionNames<<tr("autocorrelation data");
        *optionNames<<tr("normalized autocorrelation data");
        *optionNames<<tr("MSD/(6*tau)");
        *optionNames<<tr("MSD/(4*tau)");
        *optionNames<<tr("MSD/(P*D0*tau)");
        *optionNames<<tr("MSD/(P*D1*tau)");
        *optionNames<<tr("MSD/(P*D2*tau)");
        *optionNames<<tr("MSD/(P*D3*tau)");
        *optionNames<<tr("MSD-(P*D0*tau)");
        *optionNames<<tr("local D(tau)");
        *optionNames<<tr("local alpha(tau)");

    }
    if (listPlotOptions) {
        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("lag time \\tau [s]"), tr("mean squared displacements $\\langle r^2\\rangle(\\tau)$"), true, true);
        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("lag time \\tau [s]"), tr("correlation curve $g(\\tau)$"), true, false);
        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("lag time \\tau [s]"), tr("norm. correlation curve $N\\cdot g(\\tau)$"), true, false);
        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("lag time \\tau [s]"), tr("norm. MSD $\\langle r^2\\rangle(\\tau)/(6\\tau)$ [{\\mu}m^2/s]"), true, false);
        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("lag time \\tau [s]"), tr("norm. MSD $\\langle r^2\\rangle(\\tau)/(4\\tau)$ [{\\mu}m^2/s]"), true, false);
        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("lag time \\tau [s]"), tr("norm. MSD $\\langle r^2\\rangle(\\tau)/(P{\\cdot}D_0\\cdot\\tau)$"), true, false);
        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("lag time \\tau [s]"), tr("norm. MSD $\\langle r^2\\rangle(\\tau)/(P{\\cdot}D_1\\cdot\\tau)$"), true, false);
        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("lag time \\tau [s]"), tr("norm. MSD $\\langle r^2\\rangle(\\tau)/(P{\\cdot}D_2\\cdot\\tau)$"), true, false);
        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("lag time \\tau [s]"), tr("norm. MSD $\\langle r^2\\rangle(\\tau)/(P{\\cdot}D_3\\cdot\\tau)$"), true, false);
        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("lag time \\tau [s]"), tr("additive MSD $\\langle r^2\\rangle(\\tau)-(P{\\cdot}D_0\\cdot\\tau)$ [{\\mu}m^2]"), true, false);
        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("lag time \\tau [s]"), tr("local diffusion coefficient $D(\\tau)$ [{\\mu}m^2/s]"), true, false);
        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("lag time \\tau [s]"), tr("local anomality coefficient $\\alpha(\\tau)$"), true, false);
    }
    return true;
}

QWidget* QFFCSMSDEvaluationEditor::createSlopeWidgets(int i) {
    QWidget* w1=new QWidget(this);
    QHBoxLayout* lay1=new QHBoxLayout();
    lay1->setContentsMargins(0,0,0,0);
    w1->setLayout(lay1);
    QWidget* w=new QWidget(this);
    QVBoxLayout* lay=new QVBoxLayout();
    lay->setContentsMargins(0,0,0,0);
    w->setLayout(lay);
    /*if (i>0) {
        QWidget* w2=new QWidget(this);
        QHBoxLayout* lay2=new QHBoxLayout();
        lay2->setContentsMargins(0,0,0,0);
        w2->setLayout(lay1);
    }*/
    numPre[i]=new QFDoubleEdit(this);
    numPre[i]->setRange(0, DBL_MAX);
    numPre[i]->setCheckBounds(true, false);
    numPre[i]->setDecimals(1);
    numPre[i]->setValue(6);
    connect(numPre[i], SIGNAL(valueChanged(double)), this, SLOT(theoryChanged()));

    numD[i]=new QFDoubleEdit(this);
    numD[i]->setRange(0, DBL_MAX);
    numD[i]->setCheckBounds(true, false);
    numD[i]->setDecimals(6);
    numD[i]->setValue(10);
    connect(numD[i], SIGNAL(valueChanged(double)), this, SLOT(theoryChanged()));

    numAlpha[i]=new QFDoubleEdit(this);
    numAlpha[i]->setRange(0, DBL_MAX);
    numAlpha[i]->setCheckBounds(true, false);
    numAlpha[i]->setDecimals(6);
    numAlpha[i]->setValue(1);
    connect(numAlpha[i], SIGNAL(valueChanged(double)), this, SLOT(theoryChanged()));

    QAction* actFit=createActionAndButton(btnFitMSD[i],QIcon(":/fcsmsdevaluation/fit_fitcurrent.png"), tr("fit MSD law %1 ...").arg(i+1), this);
    connect(actFit, SIGNAL(triggered()), this, SLOT(fitMSD()));

    lay1->addWidget(numPre[i]);
    lay1->addWidget(new QLabel("*"));
    lay1->addWidget(numD[i]);
    lay1->addWidget(new QLabel(tr("*t^")));
    lay1->addWidget(numAlpha[i]);
    lay1->addWidget(btnFitMSD[i]);
    chkSlope[i]=new QCheckBox(this);
    chkSlope[i]->setChecked(false);
    w1->setEnabled(false);
    connect(chkSlope[i], SIGNAL(toggled(bool)), w1, SLOT(setEnabled(bool)));
    connect(chkSlope[i], SIGNAL(toggled(bool)), this, SLOT(theoryChanged()));


    chkSlope[i]->setText(tr("MSD %1:").arg(i+1));

    //lay->addWidget(chkSlope[i]);
    lay->addWidget(w1);

    return w;
}


void QFFCSMSDEvaluationEditor::createWidgets() {

    flAlgorithmParams->addRow(tr("Theory:"), new QLabel(tr("<font size=\"+1\"><i>&lang;r<sup>2</sup>&rang; = f&middot;D&middot;t<sup>&alpha;<&sup></i> [&mu;m<sup>2</sup>]</size>")));

    for (int i=0; i<MSDTHEORYCOUNT; i++) {
        if (i>0) {
            numIntersect[i-1]=new QFDoubleEdit(this);
            numIntersect[i-1]->setRange(-DBL_MAX, DBL_MAX);
            numIntersect[i-1]->setCheckBounds(false, false);
            numIntersect[i-1]->setDecimals(6);
            numIntersect[i-1]->setValue(0);
            numIntersect[i-1]->setReadOnly(true);
            flAlgorithmParams->addRow(tr("intersect X<sub>%1, %2</sub>").arg(i, i+1), numIntersect[i-1]);
        }
        QWidget* wsl=createSlopeWidgets(i);
        flAlgorithmParams->addRow(chkSlope[i], wsl);
    }
    cmbFitType=new QComboBox(this);
    cmbFitType->addItems(QFFCSMSDEvaluationItem::getFitTypes());
    flAlgorithmParams->addRow(tr("fit type:"), cmbFitType);

    spinFitWidth=new QSpinBox(this);
    spinFitWidth->setRange(5, INT_MAX);
    spinFitWidth->setValue(10);
    flAlgorithmParams->addRow(tr("fit width:"), spinFitWidth);

    chkFitRange=new QCheckBox(tr("fit alpha ,D only on selected MSD range"), this);
    chkFitRange->setChecked(true);
    flAlgorithmParams->addRow(tr("fit range:"), chkFitRange);





    cmbWeights=new QFFCSWeightingCombobox(this);
    cmbWeights->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    cmbWeights->setEditable(false);
    cmbWeights->setMaximumWidth(150);
    cmbWeights->setMinimumWidth(150);
    QLabel* l=new QLabel(tr("&Weight Model: "), this);
    l->setBuddy(cmbWeights);
    hblModel->addWidget(l);
    hblModel->addWidget(cmbWeights);
    hblModel->addStretch();


    tbPlot=new QToolBar(QString("tbPlot"), this);
    tbPlot->setIconSize(QSize(16,16));
    tbPlot->addAction(pltData->get_plotter()->get_actSavePlot());
    pltData->get_plotter()->get_actSavePlot()->setToolTip(tr("save the correlation and residuals plot as an image (PNG, JPEG, PDF, SVG, ...)"));
    tbPlot->addAction(pltData->get_plotter()->get_actPrint());
    pltData->get_plotter()->get_actPrint()->setToolTip(tr("print the correlation and residuals plot"));
    tbPlot->addAction(pltData->get_plotter()->get_actCopyPixelImage());
    pltData->get_plotter()->get_actCopyPixelImage()->setToolTip(tr("copy the correlation and residuals plot as an image to the system clipboard"));
    tbPlot->addSeparator();
    tbPlot->addAction(pltData->get_plotter()->get_actZoomAll());
    tbPlot->addAction(pltData->get_plotter()->get_actZoomIn());
    tbPlot->addAction(pltData->get_plotter()->get_actZoomOut());
    chkShowKey=new QCheckBox(tr("show key"), this);
    tbPlot->addSeparator();
    tbPlot->addWidget(chkShowKey);
    connect(chkShowKey, SIGNAL(toggled(bool)), this, SLOT(displayData()));

    toolbar->removeAction(pltData->get_plotter()->get_actSavePlot());
    toolbar->removeAction(pltData->get_plotter()->get_actPrint());
    toolbar->removeAction(pltData->get_plotter()->get_actCopyPixelImage());
    toolbar->removeAction(pltData->get_plotter()->get_actZoomAll());
    toolbar->removeAction(pltData->get_plotter()->get_actZoomIn());
    toolbar->removeAction(pltData->get_plotter()->get_actZoomOut());
    plotLayout->insertWidget(0, tbPlot);


    pltDistribution=new QFPlotter(true, this);
    pltDistribution->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltDistribution->useExternalDatastore(pltData->getDatastore());
    pltDistribution->set_displayToolbar(false);
    pltDistribution->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [seconds]"));
    pltDistribution->getXAxis()->set_labelFontSize(11);
    pltDistribution->getXAxis()->set_tickLabelFontSize(10);
    pltDistribution->getXAxis()->set_logAxis(true);
    pltDistribution->getYAxis()->set_axisLabel(tr("MSD $\\langle r^2(\\tau)\\rangle$ [{\\mu}m^2]"));
    pltDistribution->getYAxis()->set_labelFontSize(11);
    pltDistribution->getYAxis()->set_tickLabelFontSize(10);
    pltDistribution->getYAxis()->set_logAxis(true);
    pltDistribution->getXAxis()->set_drawMode1(JKQTPCADMcomplete);
    pltDistribution->get_plotter()->setBorder(1,1,1,1);
    pltDistribution->get_plotter()->set_useAntiAliasingForSystem(true);
    pltDistribution->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltDistribution->set_displayMousePosition(false);
    pltDistribution->get_plotter()->set_keyFontSize(9);
    //pltDistribution->get_plotter()->set_keyXMargin(2);
    //pltDistribution->get_plotter()->set_keyYMargin(2);
    pltDistribution->get_plotter()->set_keyPosition(JKQTPkeyInsideTopLeft);

    ovlDistRange=new JKQTPoverlayVerticalRange(0,0, pltDistribution->get_plotter());
    ovlDistRange->set_inverted(true);
    QColor fillRange=QColor("grey");
    fillRange.setAlphaF(0.2);
    ovlDistRange->set_fillColor(fillRange);
    ovlDistRange->set_visible(false);
    ovlDistRange->set_color(fillRange);
    pltDistribution->get_plotter()->addOverlayElement(ovlDistRange);

    ovlModel=new JKQTPoverlayLine(0,0,0,0,pltDistribution->get_plotter());
    ovlModel->set_visible(false);
    ovlModel->set_color(QColor("darkgrey"));
    ovlModel->set_lineStyle(Qt::DotLine);
    ovlModel->set_lineWidth(2);
    ovlModel->set_infinite(true);
    pltDistribution->get_plotter()->addOverlayElement(ovlModel);



    tbPlotDistribution=new QToolBar(QString("tbPlotDistribution"), this);
    tbPlotDistribution->setIconSize(QSize(16,16));
    tbPlotDistribution->addAction(pltDistribution->get_plotter()->get_actSavePlot());
    pltDistribution->get_plotter()->get_actSavePlot()->setToolTip(tr("save the distribution plot as an image (PNG, JPEG, PDF, SVG, ...)"));
    tbPlotDistribution->addAction(pltDistribution->get_plotter()->get_actPrint());
    pltDistribution->get_plotter()->get_actPrint()->setToolTip(tr("print the distribution plot "));
    tbPlotDistribution->addAction(pltDistribution->get_plotter()->get_actCopyPixelImage());
    pltDistribution->get_plotter()->get_actCopyPixelImage()->setToolTip(tr("copy the distribution plot as an image to the system clipboard"));
    tbPlotDistribution->addSeparator();
    tbPlotDistribution->addAction(pltDistribution->get_plotter()->get_actZoomAll());
    tbPlotDistribution->addAction(pltDistribution->get_plotter()->get_actZoomIn());
    tbPlotDistribution->addAction(pltDistribution->get_plotter()->get_actZoomOut());
    tbPlotDistribution->addSeparator();
    chkShowKeyDist=new QCheckBox(tr("show key"), this);
    chkShowKeyDist->setChecked(true);
    connect(chkShowKeyDist, SIGNAL(toggled(bool)), this, SLOT(updateDistribution()));
    tbPlotDistribution->addWidget(chkShowKeyDist);
    sliderDist=new DataCutSliders(this);


    pltDistResults=new QFPlotter(true, this);
    pltDistResults->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltDistResults->useExternalDatastore(pltData->getDatastore());
    pltDistResults->set_displayToolbar(false);
    pltDistResults->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [seconds]"));
    pltDistResults->getXAxis()->set_labelFontSize(11);
    pltDistResults->getXAxis()->set_tickLabelFontSize(10);
    pltDistResults->getXAxis()->set_logAxis(true);
    pltDistResults->getYAxis()->set_axisLabel("");//tr("MSD $\\langle r^2(\\tau)\\rangle$ [{\\mu}m^2]"));
    pltDistResults->getYAxis()->set_labelFontSize(11);
    pltDistResults->getYAxis()->set_tickLabelFontSize(10);
    pltDistResults->getYAxis()->set_logAxis(false);
    pltDistResults->getXAxis()->set_drawMode1(JKQTPCADMcomplete);
    pltDistResults->get_plotter()->setBorder(1,1,1,1);
    pltDistResults->get_plotter()->set_useAntiAliasingForSystem(true);
    pltDistResults->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltDistResults->get_plotter()->set_keyFontSize(9);
    pltDistResults->get_plotter()->set_keyXMargin(2);
    pltDistResults->useExternalDatastore(pltDistribution->getDatastore());
    pltDistResults->set_displayMousePosition(false);
    pltDistResults->synchronizeToMaster(pltDistribution, true, false);
    connect(pltDistResults, SIGNAL(plotMouseMove(double,double)), this, SLOT(plotDistMouseMoved(double,double)));

    tbPlotDistResults=new QToolBar(QString("tbPlotDistributionResults"), this);
    tbPlotDistResults->setIconSize(QSize(16,16));
    tbPlotDistResults->addAction(pltDistResults->get_plotter()->get_actSavePlot());
    pltDistResults->get_plotter()->get_actSavePlot()->setToolTip(tr("save the distribution plot as an image (PNG, JPEG, PDF, SVG, ...)"));
    tbPlotDistResults->addAction(pltDistResults->get_plotter()->get_actPrint());
    pltDistResults->get_plotter()->get_actPrint()->setToolTip(tr("print the distribution plot "));
    tbPlotDistResults->addAction(pltDistResults->get_plotter()->get_actCopyPixelImage());
    pltDistResults->get_plotter()->get_actCopyPixelImage()->setToolTip(tr("copy the distribution plot as an image to the system clipboard"));
    tbPlotDistResults->addSeparator();
    tbPlotDistResults->addAction(pltDistResults->get_plotter()->get_actZoomAll());
    tbPlotDistResults->addAction(pltDistResults->get_plotter()->get_actZoomIn());
    tbPlotDistResults->addAction(pltDistResults->get_plotter()->get_actZoomOut());
    cmbDistResultsMode=new QComboBox(this);
    cmbDistResultsMode->addItem(tr("local alpha"));
    cmbDistResultsMode->addItem(tr("local D"));
    cmbDistResultsMode->addItem(tr("MSD / (6*tau)"));
    cmbDistResultsMode->addItem(tr("MSD / (4*tau)"));
    cmbDistResultsMode->addItem(tr("MSD / (P0*D0*tau)"));
    cmbDistResultsMode->addItem(tr("MSD / (P1*D1*tau)"));
    cmbDistResultsMode->addItem(tr("MSD / (P2*D2*tau)"));
    cmbDistResultsMode->addItem(tr("MSD / (P3*D3*tau)"));
    cmbDistResultsMode->addItem(tr("MSD - P0*D0*tau"));
    connect(cmbDistResultsMode, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDistributionResults()));
    tbPlotDistResults->addSeparator();
    tbPlotDistResults->addWidget(new QLabel(tr("   parameter: ")));
    tbPlotDistResults->addWidget(cmbDistResultsMode);
    chkShowKeyDistResults=new QCheckBox(tr("show key"), this);
    chkShowKeyDistResults->setChecked(true);
    connect(chkShowKeyDistResults, SIGNAL(toggled(bool)), this, SLOT(updateDistributionResults()));
    tbPlotDistResults->addSeparator();
    tbPlotDistResults->addWidget(chkShowKeyDistResults);

    splitterDist=new QVisibleHandleSplitter(Qt::Vertical, this);
    QWidget* w=new QWidget(this);
    QVBoxLayout* lay=new QVBoxLayout();
    lay->setContentsMargins(0,0,0,0);
    w->setLayout(lay);
    lay->addWidget(tbPlotDistribution);
    lay->addWidget(pltDistribution, 1);
    splitterDist->addWidget(w);

    w=new QWidget(this);
    lay=new QVBoxLayout();
    lay->setContentsMargins(0,0,0,0);
    w->setLayout(lay);
    lay->addWidget(tbPlotDistResults);
    lay->addWidget(pltDistResults, 1);
    lay->addWidget(sliderDist);
    splitterDist->addWidget(w);

    QWidget* wPltDist=new QWidget(this);
    QVBoxLayout* layPltDist=new QVBoxLayout();
    wPltDist->setLayout(layPltDist);
    layPltDist->setContentsMargins(0,0,0,0);
    layPltDist->addWidget(splitterDist);
    splitMorePLot->addWidget(wPltDist);

    //splitMorePLot->setCollapsible(splitMorePLot->indexOf(pltDistribution), false);
    splitterDist->setCollapsible(0, false);
    splitterDist->setCollapsible(1, false);

    QAction* actFirst=menuParameters->actions().value(0, NULL);
    actAverageFirstLags=new QAction(tr("&average first few lags for N"), this);
    connect(actAverageFirstLags, SIGNAL(triggered()), this, SLOT(averageFirstFewLags()));
    actGetNFromFits=new QAction(tr("get N from FCS fits"), this);
    connect(actGetNFromFits, SIGNAL(triggered()), this, SLOT(getNFromFits()));
    menuParameters->insertAction(actFirst, actGetNFromFits);
    menuParameters->insertAction(actFirst, actAverageFirstLags);
    menuParameters->insertSeparator(actFirst);

    actFitAllMSD=new QAction(tr("fit theory to several MSDs"), this);
    connect(actFitAllMSD, SIGNAL(triggered()), this, SLOT(fitAllMSD()));
    actUpdateCalculated=new QAction(tr("update plots derived from fit"), this);
    connect(actUpdateCalculated, SIGNAL(triggered()), this, SLOT(theoryChanged()));
    menuFit->addSeparator();
    menuFit->addAction(actFitAllMSD);
    menuFit->addAction(actUpdateCalculated);

    actCopyAverageData=new QAction(QIcon(":/copy.png"), tr("&copy runs-average of MSD"), this);
    connect(actCopyAverageData, SIGNAL(triggered()), this, SLOT(copyAverageData()));

    menuResults->addSeparator();
    menuResults->addAction(actCopyAverageData);

    pltOverview=new QFRDRImageToRunPreview(this);
    pltOverview->setMaskEditable(true);
    pltOverview->setSelectionEditable(true);
    tabResidulas->addTab(pltOverview, tr("Overview Image"));

    menuTools=propertyEditor->addOrFindMenu("&Tools", 0);
    menuTools->addAction(actOverlayPlot);

    /////
    connect(pltDistribution, SIGNAL(plotMouseMove(double,double)), this, SLOT(plotMouseMove(double,double)));
    connect(pltDistribution, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(distzoomChangedLocally(double,double,double,double,JKQtPlotter*)));
    connect(pltDistResults, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(distzoomChangedLocally(double,double,double,double,JKQtPlotter*)));
    connect(pltDistResults, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));
    /////

}





int QFFCSMSDEvaluationEditor::getMSDMin(QFRawDataRecord* rec, int index, int defaultMin) {
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!data) return defaultMin;
    const QString resultID=data->getEvaluationResultID(index);
    if (!rec) return defaultMin;


    return rec->getProperty(resultID+"_msdcut_min", defaultMin).toInt();
}

int QFFCSMSDEvaluationEditor::getMSDMax(QFRawDataRecord* rec, int index, int defaultMax) {
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!data) return defaultMax;
    const QString resultID=data->getEvaluationResultID(index);

    if (!rec) return defaultMax;

    return rec->getProperty(resultID+"_msdcut_max", defaultMax).toInt();
}


int QFFCSMSDEvaluationEditor::getMSDMin(QFRawDataRecord* rec) {
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!data) return 0;
    return getMSDMin(rec, data->getCurrentIndex());
}

int QFFCSMSDEvaluationEditor::getMSDMax(QFRawDataRecord* rec) {
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!data) return 0;
    return getMSDMax(rec, data->getCurrentIndex());
}


int QFFCSMSDEvaluationEditor::getMSDMin(QFRawDataRecord* rec, int index) {
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!data) return 0;
    return getMSDMin(rec, index, 0);
}

int QFFCSMSDEvaluationEditor::getMSDMax(QFRawDataRecord* rec, int index) {
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!data) return 0;
    return getMSDMax(rec, index, data->getMSD(rec, index, data->getCurrentModel()).size());
}

void QFFCSMSDEvaluationEditor::setMSDMin(int MSDMin) {
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!data) return;
    QFRawDataRecord* rdr=current->getHighlightedRecord();
    const QString resultID=data->getEvaluationResultID(data->getCurrentIndex());
    rdr->setQFProperty(resultID+"_msdcut_min", MSDMin, false, false);
}

void QFFCSMSDEvaluationEditor::setMSDMax(int MSDMax) {
    if (!current) return;
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!data) return;
    QFRawDataRecord* rdr=current->getHighlightedRecord();
    const QString resultID=data->getEvaluationResultID(data->getCurrentIndex());
    rdr->setQFProperty(resultID+"_msdcut_max", MSDMax, false, false);
}


void QFFCSMSDEvaluationEditor::slidersDistChanged(int userMin, int userMax, int min, int max) {
    if (!dataEventsEnabled) return;
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!data) return;
    if (!current->getHighlightedRecord()) return;
    setMSDMax(userMax);
    setMSDMin(userMin);
    displayData();
}

void QFFCSMSDEvaluationEditor::updateSliders() {
    bool en=false;
    sliderDist->disableSliderSignals();
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    sliderDist->set_min(0);
    if (data && current->getHighlightedRecord()) {
        sliderDist->set_userMin(getMSDMin(current->getHighlightedRecord(), data->getCurrentIndex(), 0));
        QFFCSMSDEvaluationItem* item=qobject_cast<QFFCSMSDEvaluationItem*>(current);
        if (item) {
            QVector<double> msd=item->getMSD(current->getHighlightedRecord(), item->getCurrentIndex(), item->getCurrentModel());
            if (msd.size()>0) {
                en=true;
                sliderDist->set_max(msd.size());
                sliderDist->set_userMax(getMSDMax(current->getHighlightedRecord(), data->getCurrentIndex(), msd.size()));
            }
        }
    }

    sliderDist->setEnabled(en);
    sliderDist->enableSliderSignals();

}


void QFFCSMSDEvaluationEditor::connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) {
    QFFCSByIndexAndModelEvaluationEditor::connectWidgets(current, old);

    if (old) {
        //disconnect(edtAlpha, SIGNAL(valueChanged(double)), this, SLOT(alphaChanged(double)));
        disconnect(cmbWeights, SIGNAL(currentIndexChanged(int)), this, SLOT(weightsChanged(int)));
        disconnect(sliderDist, SIGNAL(slidersChanged(int, int, int, int)), this, SLOT(slidersDistChanged(int, int, int, int)));
        disconnect(spinFitWidth, SIGNAL(valueChanged(int)),this,SLOT(fitWidthChanged(int)));
        disconnect(cmbFitType, SIGNAL(currentIndexChanged(int)),this,SLOT(fitTypeChanged(int)));
        disconnect(chkFitRange, SIGNAL(toggled(bool)),this,SLOT(fitRangeChanged(bool)));
        //disconnect(edtNumIter, SIGNAL(valueChanged(int)),this,SLOT(NumIterChanged(int)));
    }

    QFFCSMSDEvaluationItem* item=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    //QFFCSMSDEvaluationItem* item_old=qobject_cast<QFFCSMSDEvaluationItem*>(old);
    if (item) {
        dataEventsEnabled=false;

        for (int i=0; i<MSDTHEORYCOUNT; i++)     {
            chkSlope[i]->setChecked(item->getTheoryEnabled(i, item->getHighlightedRecord(), item->getCurrentIndex()));
            numPre[i]->setValue(item->getTheoryPre(i, item->getHighlightedRecord(), item->getCurrentIndex()));
            numD[i]->setValue(item->getTheoryD(i, item->getHighlightedRecord(), item->getCurrentIndex()));
            numAlpha[i]->setValue(item->getTheoryAlpha(i, item->getHighlightedRecord(), item->getCurrentIndex()));
            //if (i<MSDTHEORYCOUNT-1) numIntersect[i]->setValue(item->getTheoryIntersect(i, item->getHighlightedRecord(), item->getCurrentIndex()));
            if (i>0) {
                numIntersect[i-1]->setValue(item->getTheoryIntersect(i-1, item->getHighlightedRecord(), item->getCurrentIndex()));
                numIntersect[i-1]->setEnabled(chkSlope[i-1]->isChecked()&&chkSlope[i]->isChecked());
            }
        }

        /*edtAlpha->setValue(item->getAlpha());
        connect(edtAlpha, SIGNAL(valueChanged(double)), this, SLOT(alphaChanged(double)));


        edtNumIter->setValue(item->getNumIter());
        connect(edtNumIter, SIGNAL(valueChanged(int)), this, SLOT(NumIterChanged(int)));*/

        cmbFitType->setCurrentIndex(item->getFitType(item->getHighlightedRecord(), item->getCurrentIndex()));
        connect(cmbFitType, SIGNAL(currentIndexChanged(int)), this, SLOT(fitTypeChanged(int)));

        spinFitWidth->setValue(item->getFitWidth(item->getHighlightedRecord(), item->getCurrentIndex()));
        connect(spinFitWidth, SIGNAL(valueChanged(int)), this, SLOT(fitWidthChanged(int)));
        cmbWeights->setCurrentIndex(item->getFitDataWeighting() );//current->getProperty("weights", 0).toInt());
        connect(cmbWeights, SIGNAL(currentIndexChanged(int)), this, SLOT(weightsChanged(int)));
        connect(sliderDist, SIGNAL(slidersChanged(int, int, int, int)), this, SLOT(slidersDistChanged(int, int, int, int)));
        chkFitRange->setChecked(item->getFitRangeLimited(item->getHighlightedRecord(), item->getCurrentIndex()));
        connect(chkFitRange, SIGNAL(toggled(bool)),this,SLOT(fitRangeChanged(bool)));





        dataEventsEnabled=true;

        if (item->propertyExists("PRESET_MODEL")) {
            cmbModel->setCurrentIndex(item->getProperty("PRESET_MODEL", cmbModel->currentIndex()).toInt());
            item->deleteProperty("PRESET_MODEL");
        }

    }

    /*
    /////
    connect(pltDistribution, SIGNAL(plotMouseMove(double,double)), this, SLOT(plotMouseMove(double,double)));
    /////
    */
}


void QFFCSMSDEvaluationEditor::readSettings() {
    QFFCSByIndexAndModelEvaluationEditor::readSettings();
    chkShowKey->setChecked(settings->getQSettings()->value("fcsmsdevaleditor/showkey", true).toBool());
    chkShowKeyDist->setChecked(settings->getQSettings()->value("fcsmsdevaleditor/showkeydist", true).toBool());
    chkShowKeyDistResults->setChecked(settings->getQSettings()->value("fcsmsdevaleditor/showkeydistresults", true).toBool());
    cmbDistResultsMode->setCurrentIndex(settings->getQSettings()->value("fcsmsdevaleditor/distresultmode", true).toBool());

}

void QFFCSMSDEvaluationEditor::writeSettings() {
    QFFCSByIndexAndModelEvaluationEditor::writeSettings();
    settings->getQSettings()->setValue("fcsmsdevaleditor/showkey", chkShowKey->isChecked());
    settings->getQSettings()->setValue("fcsmsdevaleditor/showkeydist", chkShowKeyDist->isChecked());
    settings->getQSettings()->setValue("fcsmsdevaleditor/showkeydistresults", chkShowKeyDistResults->isChecked());
}

void QFFCSMSDEvaluationEditor::plotDistMouseMoved(double x, double y)
{
    ovlDistRange->set_visible(false);
    ovlModel->set_visible(false);
    int colI=pltDistResults->getDatastore()->getColumnNum("msdfit_tau");
    int colSI=pltDistResults->getDatastore()->getColumnNum("msdfit_tau_start");
    int colEI=pltDistResults->getDatastore()->getColumnNum("msdfit_tau_end");
    int colDI=pltDistResults->getDatastore()->getColumnNum("msdfit_D");
    int colAI=pltDistResults->getDatastore()->getColumnNum("msdfit_alpha");
    if (colI>=0 && colEI>=0 && colSI>=0 && cmbDistResultsMode->currentIndex()<2) {
        JKQTPcolumn col=pltDistResults->getDatastore()->getColumn(colI);
        JKQTPcolumn colS=pltDistResults->getDatastore()->getColumn(colSI);
        JKQTPcolumn colE=pltDistResults->getDatastore()->getColumn(colEI);
        if (col.isValid()&&colS.isValid()&&colE.isValid()) {
            double* tau=col.getPointer(0);
            double* tauS=colS.getPointer(0);
            double* tauE=colE.getPointer(0);
            int count=qMin(qMin(col.getRows(), colS.getRows()), colE.getRows());
            int idx=-1;
            if (count>0 && x>=tau[0] && x<=tau[count-1]) {
                for (int i=0; i<count; i++) {
                    if (x<tau[i]) {
                        idx=i-1;
                        break;
                    }
                }
            }
            //qDebug()<<x<<idx;
            /*for (int i=0; i<count; i++) {
                qDebug()<<"   "<<i<<"  "<<tau[i];
            }*/
            if (idx>0) {
                //qDebug()<<x<<idx<<tauS[idx]<<tauE[idx];
                ovlDistRange->set_position(tauS[idx]);
                ovlDistRange->set_position2(tauE[idx]);
                ovlDistRange->set_visible(true);
                if (colDI>=0 && colAI>=0) {
                    double pre=6;
                    JKQTPcolumn colD=pltDistResults->getDatastore()->getColumn(colDI);
                    JKQTPcolumn colA=pltDistResults->getDatastore()->getColumn(colAI);
                    if (colD.isValid()&&colA.isValid()&&idx<=(long long)colD.getRows() && idx<=(long long)colA.getRows()) {
                        double D=colD.getValue(idx)*pre;
                        double alpha=colA.getValue(idx);
                        double r1=D*pow(tauS[idx], alpha);
                        double r2=D*pow(tauE[idx], alpha);
                        //qDebug()<<D/pre<<alpha<<tauS[idx]<<r1<<tauE[idx]<<r2;
                        ovlModel->set_x1(tauS[idx]);
                        ovlModel->set_x2(tauE[idx]);
                        ovlModel->set_y1(r1);
                        ovlModel->set_y2(r2);
                        ovlModel->set_visible(true);
                    }
                }
            }
        }
    }
    pltDistribution->update_overlays();
}



void QFFCSMSDEvaluationEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {

    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    //QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(currentRecord);
    QFRDRImageToRunInterface* dataImg=qobject_cast<QFRDRImageToRunInterface*>(currentRecord);

    disconnect(pltOverview, SIGNAL(currentRunChanged(int)), spinRun, SLOT(setValue(int)));
    disconnect(spinRun, SIGNAL(valueChanged(int)), pltOverview, SLOT(setCurrentRun(int)));

    if (data && eval) {
        bool oldde=dataEventsEnabled;
        dataEventsEnabled=false;

        if (dataImg) {
            pltOverview->setRDR(currentRecord);
            connect(pltOverview, SIGNAL(currentRunChanged(int)), spinRun, SLOT(setValue(int)));
            connect(spinRun, SIGNAL(valueChanged(int)), pltOverview, SLOT(setCurrentRun(int)));
            tabResidulas->setTabEnabled(tabResidulas->indexOf(pltOverview), true);
        } else {
            pltOverview->setRDR(0);
            tabResidulas->setTabEnabled(tabResidulas->indexOf(pltOverview), false);
        }


        //edtAlpha->setValue(eval->getAlpha());
        cmbWeights->setCurrentWeight(eval->getFitDataWeighting());
        cmbFitType->setCurrentIndex(eval->getFitType(currentRecord, eval->getCurrentIndex()));
        spinFitWidth->setValue(eval->getFitWidth(currentRecord, eval->getCurrentIndex()));
        //edtNumIter->setRange(1,10000); //qMax(0,data->getCorrelationN())
        //edtNumIter->setValue(eval->getNumIter());
        for (int i=0; i<MSDTHEORYCOUNT; i++)     {
            chkSlope[i]->setChecked(eval->getTheoryEnabled(i, currentRecord, eval->getCurrentIndex()));
            numPre[i]->setValue(eval->getTheoryPre(i, currentRecord, eval->getCurrentIndex()));
            numD[i]->setValue(eval->getTheoryD(i, currentRecord, eval->getCurrentIndex()));
            numAlpha[i]->setValue(eval->getTheoryAlpha(i, currentRecord, eval->getCurrentIndex()));
            if (i>0) {
                numIntersect[i-1]->setValue(eval->getTheoryIntersect(i-1, currentRecord, eval->getCurrentIndex()));
                numIntersect[i-1]->setEnabled(chkSlope[i-1]->isChecked()&&chkSlope[i]->isChecked());
            }

            /*if (eval->hasResults(currentRecord, eval->getCurrentIndex(), eval->getCurrentModel())) {
                eval->setTheory(i, chkSlope[i]->isChecked(), numPre[i]->value(), numD[i]->value(), numAlpha[i]->value(), currentRecord, eval->getCurrentIndex());
            }*/
        }

        updateSliders();

        dataEventsEnabled=oldde;
    }
    QFFCSByIndexAndModelEvaluationEditor::highlightingChanged(formerRecord, currentRecord);

}


void QFFCSMSDEvaluationEditor::displayData() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    QFFCSWeightingTools* wdata=dynamic_cast<QFFCSWeightingTools*>(current.data());
    JKQTPdatastore* ds=pltData->getDatastore();
    JKQTPdatastore* dsres=pltResiduals->getDatastore();
    JKQTPdatastore* dsresh=pltResidualHistogram->getDatastore();
    JKQTPdatastore* dsresc=pltResidualCorrelation->getDatastore();
    JKQTPdatastore* dsdist=pltDistribution->getDatastore();

    if ((!eval)||(!data)) {
        pltData->clearGraphs();
        ds->clear();
        return;
    } else {

        //qDebug()<<" **** displayData()";
        QTime t, t1;
        t.start();
        t1.start();

        //bool updEn=updatesEnabled();
        //setUpdatesEnabled(false);

        pltResiduals->set_doDrawing(false);
        pltResiduals->set_emitSignals(false);
        pltResiduals->clearGraphs();
        pltData->set_doDrawing(false);
        pltData->set_emitSignals(false);
        pltData->clearGraphs();
        pltResidualHistogram->set_doDrawing(false);
        pltResidualHistogram->set_emitSignals(false);
        pltResidualHistogram->clearGraphs();
        pltResidualCorrelation->set_doDrawing(false);
        pltResidualCorrelation->set_emitSignals(false);
        pltResidualCorrelation->clearGraphs();
        pltResidualCorrelation->set_doDrawing(false);
        pltDistribution->set_emitSignals(false);
        pltDistribution->clearGraphs();
        pltDistResults->set_doDrawing(false);
        pltDistResults->set_emitSignals(false);
        pltDistResults->clearGraphs();
        dsres->clear();
        ds->clear();
        dsresh->clear();
        dsresc->clear();
        dsdist->clear();

        //qDebug()<<"   a "<<t.elapsed()<<" ms";
        t.start();

        pltResiduals->getXAxis()->set_logAxis(chkXLogScale->isChecked());
        pltData->getXAxis()->set_logAxis(chkXLogScale->isChecked());
        if (chkXLogScale->isChecked()) {
            pltData->getXAxis()->set_minorTicks(9);
            pltResiduals->getXAxis()->set_minorTicks(9);
        } else {
            pltData->getXAxis()->set_minorTicks(1);
            pltResiduals->getXAxis()->set_minorTicks(1);
        }
        pltResiduals->getXAxis()->set_drawGrid(chkGrid->isChecked());
        pltResiduals->getYAxis()->set_drawGrid(chkGrid->isChecked());
        pltResiduals->get_plotter()->set_showKey(chkKeyVisible->isChecked());
        pltData->get_plotter()->set_showKey(chkKeyVisible->isChecked());
        pltDistribution->get_plotter()->set_showKey(chkKeyVisible->isChecked());
        pltDistResults->get_plotter()->set_showKey(chkKeyVisible->isChecked());
        pltData->getXAxis()->set_drawGrid(chkGrid->isChecked());
        pltData->getYAxis()->set_drawGrid(chkGrid->isChecked());

        pltDistribution->getXAxis()->set_drawGrid(chkGrid->isChecked());
        pltDistribution->getYAxis()->set_drawGrid(chkGrid->isChecked());
        pltDistResults->getXAxis()->set_drawGrid(chkGrid->isChecked());
        pltDistResults->getYAxis()->set_drawGrid(chkGrid->isChecked());
        pltResidualHistogram->getXAxis()->set_drawGrid(chkGrid->isChecked());
        pltResidualHistogram->getYAxis()->set_drawGrid(chkGrid->isChecked());
        pltResidualCorrelation->getXAxis()->set_drawGrid(chkGrid->isChecked());
        pltResidualCorrelation->getYAxis()->set_drawGrid(chkGrid->isChecked());

        pltData->getYAxis()->set_minTicks(5);
        pltResiduals->getYAxis()->set_minTicks(5);
        pltData->get_plotter()->set_showKey(chkShowKey->isChecked());

        //qDebug()<<"   b "<<t.elapsed()<<" ms";
        t.start();

        //int errorStyle=cmbErrorStyle->currentIndex();
        int plotStyle=cmbPlotStyle->currentIndex();
        //int residualStyle=cmbResidualStyle->currentIndex();

        if (data->getCorrelationN()>0) {
            size_t c_tau=ds->addColumn(data->getCorrelationT(), data->getCorrelationN(), "tau");


            //////////////////////////////////////////////////////////////////////////////////
            // Plot average + error markers
            //////////////////////////////////////////////////////////////////////////////////
            size_t c_mean=0;
            QString graphName="";
            int c_std=-1;
            QString errorName="";
            if (eval->getCurrentIndex()<0) {
                c_mean=ds->addColumn(data->getCorrelationMean(), data->getCorrelationN(), "cmean");
                graphName=tr("\\verb{%1} average").arg(record->getName());
                //c_std=ds->addColumn(data->getCorrelationStdDev(), data->getCorrelationN(), "cstddev");
                //errorName=tr("stddev");
            } else {
                if (eval->getCurrentIndex()<(int)data->getCorrelationRuns()) {
                    c_mean=ds->addColumn(data->getCorrelationRun(eval->getCurrentIndex()), data->getCorrelationN(), QString("run"+QString::number(eval->getCurrentIndex())));
                    graphName=tr("\\verb{%1} %2").arg(record->getName()).arg(data->getCorrelationRunName(eval->getCurrentIndex()));
                    /*if (eval->getFitDataWeighting()==QFFCSWeightingTools::RunErrorWeighting) {
                        c_std=ds->addColumn(data->getCorrelationRunError(eval->getCurrentIndex()), data->getCorrelationN(), "cperrunerror");
                        errorName=tr("per run");
                    } else {
                        c_std=ds->addColumn(data->getCorrelationStdDev(), data->getCorrelationN(), "cstddev");
                        errorName=tr("stddev");
                    }*/
                } else {
                    c_mean=ds->addColumn(data->getCorrelationMean(), data->getCorrelationN(), "cmean");
                    graphName=tr("\\verb{%1} average").arg(record->getName());
                    //c_std=ds->addColumn(data->getCorrelationStdDev(), data->getCorrelationN(), "cstddev");
                    //errorName=tr("stddev");
                }
            }
            if (wdata) {
                bool wok=false;
                double* weigm=wdata->allocWeights(&wok, record, eval->getCurrentIndex());
                if (wok && weigm && eval->getFitDataWeighting()!=QFFCSWeightingTools::EqualWeighting) {
                    errorName=wdata->dataWeightToName(eval->getFitDataWeighting(), tr("run"));
                    c_std=ds->addCopiedColumn(weigm, data->getCorrelationN(), QString("cerr_")+wdata->dataWeightToString(eval->getFitDataWeighting()));
                    qfFree(weigm);
                }
            }
            JKQTPerrorPlotstyle styl=cmbErrorStyle->getErrorStyle();

            //qDebug()<<"   c "<<t.elapsed()<<" ms";
            t.start();

            JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(pltData->get_plotter());
            QColor gcolor=pltData->get_plotter()->get_graphColor();
            g->set_color(gcolor);
            g->set_errorColor(gcolor.lighter());
            g->set_lineWidth(2);
            g->set_symbolSize(8);
            g->set_symbolWidth(1);
            g->set_xColumn(c_tau);
            g->set_yColumn(c_mean);
            g->set_yErrorColumn(c_std);
            g->set_title(graphName);
            g->set_yErrorStyle(styl);
            g->set_xErrorStyle(JKQTPnoError);
            g->set_datarange_start(datacut->get_userMin());
            g->set_datarange_end(datacut->get_userMax());
            // draw lines is default
            if (plotStyle==0) { // draw points
                g->set_drawLine(false);
                g->set_symbol(JKQTPcross);
            } else if (plotStyle==2) {
                g->set_symbol(JKQTPcross);
            }
            pltData->addGraph(g);
            //qDebug()<<"   d "<<t.elapsed()<<" ms";
            t.start();

            updateFitFunctions();
            //qDebug()<<"   e "<<t.elapsed()<<" ms";
            t.start();

            pltData->zoomToFit(true, true);
            pltResiduals->zoomToFit(false, true);
            pltDistribution->zoomToFit(true, true);
            pltDistResults->zoomToFit(false, true);

            pltResiduals->setX(pltData->getXMin(), pltData->getXMax());


            pltResidualHistogram->zoomToFit(true, true);
            pltResidualCorrelation->zoomToFit(true, true);
            //qDebug()<<"   f "<<t.elapsed()<<" ms";
            t.start();
        }


        pltResiduals->set_doDrawing(true);
        pltResiduals->set_emitSignals(true);
        pltData->set_doDrawing(true);
        pltData->set_emitSignals(true);
        pltResidualHistogram->set_doDrawing(true);
        pltResidualHistogram->set_emitSignals(true);
        pltResidualCorrelation->set_doDrawing(true);
        pltResidualCorrelation->set_emitSignals(true);
        pltDistribution->set_doDrawing(true);
        pltDistribution->set_emitSignals(true);
        pltDistResults->set_doDrawing(true);
        pltDistResults->set_emitSignals(true);
        //qDebug()<<"   g "<<t.elapsed()<<" ms";
        t.start();

        pltResiduals->update_plot();
        //qDebug()<<"   h "<<t.elapsed()<<" ms";
        t.start();
        pltData->update_plot();
        //qDebug()<<"   i "<<t.elapsed()<<" ms";
        t.start();
        pltResidualHistogram->update_plot();
        //qDebug()<<"   j "<<t.elapsed()<<" ms";
        t.start();
        pltResidualCorrelation->update_plot();
        //qDebug()<<"   k "<<t.elapsed()<<" ms";
        t.start();
        pltDistribution->update_plot();
        //qDebug()<<"   l "<<t.elapsed()<<" ms";
        t.start();
        pltDistResults->update_plot();
        //qDebug()<<"   m "<<t.elapsed()<<" ms";
        t.start();
        //qDebug()<<"  displayData end  runtime = "<<t1.elapsed()<<" ms";

        //setUpdatesEnabled(updEn);

    }
}


void QFFCSMSDEvaluationEditor::updateFitFunctions() {
    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    int index=eval->getCurrentIndex();
    int model=eval->getCurrentModel();
    //qDebug()<<"model="<<model;
    JKQTPdatastore* ds=pltData->getDatastore();
    JKQTPdatastore* dsres=pltResiduals->getDatastore();
    JKQTPdatastore* dsresh=pltResidualHistogram->getDatastore();
    JKQTPdatastore* dsresc=pltResidualCorrelation->getDatastore();
    JKQTPdatastore* dsdist=pltDistribution->getDatastore();


    if ((!eval)||(!data)) {
        return;
    }


    //qDebug()<<" **** updateFitFunctions";
    QTime t, t1;
    t.start();
    t1.start();

    int residualStyle=cmbResidualStyle->currentIndex();
    int residualHistogramBins=spinResidualHistogramBins->value();
    int datacut_min=datacut->get_userMin();
    int datacut_max=datacut->get_userMax();


    //bool updEn=updatesEnabled();
    //setUpdatesEnabled(false);
    try {

        if (data->getCorrelationN()>0) {
            int c_tau=ds->getColumnNum("tau");
            if (c_tau>=0) { // we only add a graph, if we have a column with tau values
                eval->set_doEmitPropertiesChanged(false);
                eval->set_doEmitResultsChanged(false);
                record->disableEmitResultsChanged();
                //qDebug()<<"    a "<<t.elapsed()<<" ms";
                t.start();


                /////////////////////////////////////////////////////////////////////////////////
                // retrieve data and tau-values from rawdata record
                /////////////////////////////////////////////////////////////////////////////////
                long N=data->getCorrelationN();
                int runAvgWidth=11;
                double* tauvals=data->getCorrelationT();
                double* corrdata=NULL;

                if (index<0) {
                    corrdata=data->getCorrelationMean();
                } else {
                    if (index<(int)data->getCorrelationRuns()) {
                        corrdata=data->getCorrelationRun(index);
                    } else {
                        corrdata=data->getCorrelationMean();
                    }
                }

                double* weights=eval->allocWeights(NULL, record, index, datacut_min, datacut_max);

                int rangeMaxDatarange=getUserMax(record, index);
                int rangeMinDatarange=getUserMin(record, index);


                /////////////////////////////////////////////////////////////////////////////////
                // calculate fit statistics
                /////////////////////////////////////////////////////////////////////////////////
                record->disableEmitResultsChanged();
                double* modelVals=(double*)qfMalloc(N*sizeof(double));
                QVector<double> dist=eval->getMSD(record, index, model);
                QVector<double> distTau=eval->getMSDTaus(record, index, model);

                eval->evaluateModel(record, index, model, tauvals, modelVals, N, distTau.data(), dist.data(), dist.size());

                // CALCULATE FIT STATISTICS
                QFFitStatistics fit_stat=eval->calcFitStatistics(record, index, model, tauvals, modelVals, N, dist.size(), rangeMinDatarange, rangeMaxDatarange, runAvgWidth, residualHistogramBins);

                record->enableEmitResultsChanged();


                size_t c_fit = ds->addCopiedColumn(fit_stat.fitfunc, "fit_model");
                //qDebug()<<"    f "<<t.elapsed()<<" ms";
                t.start();

                /////////////////////////////////////////////////////////////////////////////////
                // plot fit model and additional function graphs
                /////////////////////////////////////////////////////////////////////////////////
                JKQTPxyLineGraph* g_fit=new JKQTPxyLineGraph(pltData->get_plotter());
                g_fit->set_drawLine(true);
                g_fit->set_title("fit function");
                g_fit->set_xColumn(c_tau);
                g_fit->set_yColumn(c_fit);
                g_fit->set_datarange_start(datacut->get_userMin());
                g_fit->set_datarange_end(datacut->get_userMax());
                pltData->addGraph(g_fit);
                //qDebug()<<"    g "<<t.elapsed()<<" ms";
                t.start();

                double N_particle=eval->getFitValue("n_particle");
                if (N_particle>0) {
                    JKQTPgeoInfiniteLine* g_Nline=new JKQTPgeoInfiniteLine(pltData->get_plotter(), tauvals[datacut->get_userMin()], 1.0/N_particle, 1,0,QColor("orange"));
                    pltData->addGraph(g_Nline);
                }
                //qDebug()<<"    g "<<t.elapsed()<<" ms";
                t.start();

                /////////////////////////////////////////////////////////////////////////////////
                // plot residuals
                /////////////////////////////////////////////////////////////////////////////////
                size_t c_taures=c_tau;//dsres->addCopiedColumn(data->getCorrelationT(), N, "tau");
                size_t c_residuals=0;
                JKQTPxyLineGraph* g_residuals=new JKQTPxyLineGraph(pltResiduals->get_plotter());
                if (chkWeightedResiduals->isChecked()) {
                    c_residuals=dsres->addCopiedColumn(fit_stat.residuals_weighted, "residuals_weighted");
                    g_residuals->set_title("weighted residuals");
                } else {
                    c_residuals=dsres->addCopiedColumn(fit_stat.residuals, "residuals");
                    g_residuals->set_title("residuals");
                }
                g_residuals->set_xColumn(c_taures);
                g_residuals->set_yColumn(c_residuals);
                g_residuals->set_symbolSize(8);
                g_residuals->set_symbolWidth(1);
                g_residuals->set_datarange_start(datacut->get_userMin());
                g_residuals->set_datarange_end(datacut->get_userMax());
                g_residuals->set_drawLine(true);
                if (residualStyle==0) { // draw points
                    g_residuals->set_drawLine(false);
                    g_residuals->set_symbol(JKQTPcross);
                } else if (residualStyle==2) {
                    g_residuals->set_symbol(JKQTPcross);
                }
                pltResiduals->addGraph(g_residuals);
                //qDebug()<<"    h "<<t.elapsed()<<" ms";
                t.start();


                /////////////////////////////////////////////////////////////////////////////////
                // plot residuals running average
                /////////////////////////////////////////////////////////////////////////////////
                size_t c_tauresra=dsres->addCopiedColumn(fit_stat.tau_runavg, "tau_resid_runavg");
                size_t c_residualsra=0;
                JKQTPxyLineGraph* g_residualsra=new JKQTPxyLineGraph(pltResiduals->get_plotter());


                if (chkWeightedResiduals->isChecked()) {
                    c_residualsra=dsres->addCopiedColumn(fit_stat.residuals_runavg_weighted, "residuals_runavg_weighted");
                    g_residualsra->set_title("weighted residuals, movAvg");
                } else {
                    c_residualsra=dsres->addCopiedColumn(fit_stat.residuals_runavg, "residuals_runavg");
                    g_residualsra->set_title("residuals, movAvg");
                }
                g_residualsra->set_xColumn(c_tauresra);
                g_residualsra->set_yColumn(c_residualsra);
                g_residualsra->set_symbol(JKQTPnoSymbol);
                g_residualsra->set_symbolSize(6);
                g_residualsra->set_symbolWidth(1);
                g_residualsra->set_color(g_residuals->get_color().darker());
                //g_residuals->set_datarange_start(datacut->get_userMin());
                //g_residuals->set_datarange_end(datacut->get_userMax());
                g_residualsra->set_drawLine(true);
                /*if (residualStyle==0) { // draw points
                    // always draw as lines
                    g_residualsra->set_symbol(JKQTPplus);
                } else if (residualStyle==2) {
                    g_residualsra->set_symbol(JKQTPplus);
                }*/
                pltResiduals->addGraph(g_residualsra);
                //qDebug()<<"    i "<<t.elapsed()<<" ms";
                t.start();

                /////////////////////////////////////////////////////////////////////////////////
                // plot residuals histogram
                /////////////////////////////////////////////////////////////////////////////////
                size_t c_residualHistogramX=0;
                size_t c_residualHistogramY=0;
                if (chkWeightedResiduals->isChecked()) {
                    c_residualHistogramX=dsresh->addLinearColumn(residualHistogramBins, fit_stat.rminw+fit_stat.residHistWBinWidth/2.0, fit_stat.rmaxw-fit_stat.residHistWBinWidth/2.0, "residualhist_weighted_x");
                    c_residualHistogramY=dsresh->addCopiedColumn(fit_stat.resWHistogram,  "residualhist_weighted_y");
                } else {
                    c_residualHistogramX=dsresh->addLinearColumn(residualHistogramBins, fit_stat.rmin+fit_stat.residHistBinWidth/2.0, fit_stat.rmax-fit_stat.residHistBinWidth/2.0, "residualhist_x");
                    c_residualHistogramY=dsresh->addCopiedColumn(fit_stat.resHistogram,  "residualhist_y");
                }
                JKQTPbarHorizontalGraph* g_residualsHistogram=new JKQTPbarHorizontalGraph(pltResidualHistogram->get_plotter());
                g_residualsHistogram->set_xColumn(c_residualHistogramX);
                g_residualsHistogram->set_yColumn(c_residualHistogramY);
                g_residualsHistogram->set_shift(0);
                g_residualsHistogram->set_width(1.0);
                pltResidualHistogram->addGraph(g_residualsHistogram);
                //qDebug()<<"    j "<<t.elapsed()<<" ms";
                t.start();


                /////////////////////////////////////////////////////////////////////////////////
                // plot residuals correlations
                /////////////////////////////////////////////////////////////////////////////////
                size_t c_residualCorrelationX=dsresc->addLinearColumn(fit_stat.resN-1, 1, fit_stat.resN-1, "residualcorr_x");
                size_t c_residualCorrelationY=0;
                if (chkWeightedResiduals->isChecked()) {
                    c_residualCorrelationY=dsresc->addCopiedColumn(&(fit_stat.resWCorrelation[1]), fit_stat.resN-1, "residualcorr_weighted_y");
                } else {
                    c_residualCorrelationY=dsresh->addCopiedColumn(&(fit_stat.resCorrelation[1]), fit_stat.resN-1, "residualcorr_y");
                }
                JKQTPxyLineGraph* g_residualsCorrelation=new JKQTPxyLineGraph(pltResidualCorrelation->get_plotter());
                g_residualsCorrelation->set_xColumn(c_residualCorrelationX);
                g_residualsCorrelation->set_yColumn(c_residualCorrelationY);
                pltResidualCorrelation->addGraph(g_residualsCorrelation);
                //qDebug()<<"    k "<<t.elapsed()<<" ms";
                t.start();



                updateDistribution();

                updateDistributionResults();





                /////////////////////////////////////////////////////////////////////////////////
                // update display of fit results
                /////////////////////////////////////////////////////////////////////////////////
                QString txtFit="<font face=\"Arial\">";
                QString fitResult=record->resultsGetAsString(eval->getEvaluationResultID(), "fitalg_messageHTML");
                if (fitResult.isEmpty()) fitResult=record->resultsGetAsString(eval->getEvaluationResultID(), "fitalg_message");

                if (!fitResult.isEmpty()) {
                    txtFit+=txtFit+tr("<div style=\"border-style:solid\"><b>Fit Result Message:</b><center>%1</center></div><br>").arg(fitResult);
                } else {
                    txtFit+=txtFit+tr("<div style=\"border-style:solid\"><b>Fit Result Message:</b><center>not fit yet</center></div><br>");
                }
                txtFit+=QString("<b>%1</b><cebter>").arg(tr("Fit Statistics:"));
                txtFit+=QString("<table border=\"0\" width=\"95%\">");
                //txtFit+=QString("<tr><td align=\"right\"></td><td align=\"left\"></td><td align=\"right\"></td><td align=\"left\"></td></tr>");
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\"><font size=\"+2\">&chi;<sup>2</sup></font> =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                                "<td align=\"right\" valign=\"bottom\"><font size=\"+2\">&chi;<sup>2</sup></font> (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                                "</tr>").arg(fit_stat.residSqrSum).arg(fit_stat.residWeightSqrSum);
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\">&lang;E&rang;=</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                                "<td align=\"right\" valign=\"bottom\"> &lang;E&rang; (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                                "</tr>").arg(fit_stat.residAverage).arg(fit_stat.residWeightAverage);
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\">&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang;=</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                                "<td align=\"right\" valign=\"bottom\"> &radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang; (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                                "</tr>").arg(fit_stat.residStdDev).arg(fit_stat.residWeightStdDev);
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\">NP =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td></td>"
                                "<td align=\"right\" valign=\"bottom\">NR =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                                "</tr>").arg(fit_stat.fitparamN).arg(fit_stat.dataSize);
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\">DF =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td></td>"
                                "<td align=\"right\" valign=\"bottom\"></td><td align=\"left\" valign=\"bottom\"></td>"
                                "</tr>").arg(fit_stat.degFreedom);
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\">TSS  =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td></td>"
                                "<td align=\"right\" valign=\"bottom\">R<sup>2</sup> =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                                "</tr>").arg(fit_stat.TSS).arg(fit_stat.Rsquared);
                //qDebug()<<"    l "<<t.elapsed()<<" ms";
                t.start();

                txtFit+=QString("</table><br><font size=\"-1\"><i>Legend:</i>: &chi;<sup>2</sup>: sum error square, &lang;E&rang;: residual average, &radic;&lang;E2&rang;: residual stddev., <br>NP: number of fit parameters, NR: number of residuals, <br>DF: degrees of freedom, R<sup>2</sup>: coefficient of determination, <br>TSS: total sum of squares</font>");
                txtFit+=QString("</center></font>");
                fitStatisticsReport=txtFit;
                txtFitStatistics->setHtml(txtFit);

                //qDebug()<<"    m_presignals "<<t.elapsed()<<" ms";
                eval->set_doEmitPropertiesChanged(true);
                eval->set_doEmitResultsChanged(true);
                //record->enableEmitResultsChanged();

                //qDebug()<<"    m "<<t.elapsed()<<" ms";
                t.start();


                /////////////////////////////////////////////////////////////////////////////////
                // clean memory
                /////////////////////////////////////////////////////////////////////////////////
                qfFree(weights);
                fit_stat.free();

                //qDebug()<<"    n "<<t.elapsed()<<" ms";
                t.start();
            }
        }
    } catch(std::exception& E) {
        services->log_error(tr("error during plotting, error message: %1\n").arg(E.what()));
    }
    //setUpdatesEnabled(updEn);
    //qDebug()<<"    updateFitFunctions end   runtime = "<<t1.elapsed()<<"ms";

}

void QFFCSMSDEvaluationEditor::displayParameters() {
    QFFCSByIndexAndModelEvaluationEditor::displayParameters();
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    QFSimpleFitParameterEnumeratorInterface* peval=qobject_cast<QFSimpleFitParameterEnumeratorInterface*>(current);
    if ((!record)||(!eval)/*||(!data)*/) return;

    bool oldde=dataEventsEnabled;
    dataEventsEnabled=false;
    spinFitWidth->setValue(eval->getFitWidth(eval->getHighlightedRecord(), eval->getCurrentIndex()));
    cmbFitType->setCurrentIndex(eval->getFitType(eval->getHighlightedRecord(), eval->getCurrentIndex()));
    chkFitRange->setChecked(eval->getFitRangeLimited(eval->getHighlightedRecord(), eval->getCurrentIndex()));
    //edtNumIter->setValue(eval->getNumIter());
    for (int i=0; i<MSDTHEORYCOUNT; i++)     {
        chkSlope[i]->setChecked(eval->getTheoryEnabled(i, eval->getHighlightedRecord(), eval->getCurrentIndex()));
        numPre[i]->setValue(eval->getTheoryPre(i, eval->getHighlightedRecord(), eval->getCurrentIndex()));
        numD[i]->setValue(eval->getTheoryD(i, eval->getHighlightedRecord(), eval->getCurrentIndex()));
        numAlpha[i]->setValue(eval->getTheoryAlpha(i, eval->getHighlightedRecord(), eval->getCurrentIndex()));
        if (i>0) {
            numIntersect[i-1]->setValue(eval->getTheoryIntersect(i-1, eval->getHighlightedRecord(), eval->getCurrentIndex()));
            numIntersect[i-1]->setEnabled(chkSlope[i-1]->isChecked()&&chkSlope[i]->isChecked());
        }

    }
    dataEventsEnabled=oldde;


    if (eval->hasResults(record)) {
        datacut->setEnabled(false);
        //edtNdist->setEnabled(false);
    } else {
        datacut->setEnabled(true);
        //edtNdist->setEnabled(true);
    }

}

void QFFCSMSDEvaluationEditor::distzoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter *sender) {
    if (!dataEventsEnabled) return;
    if (sender==pltDistribution) {
        disconnect(pltDistResults, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(distzoomChangedLocally(double,double,double,double,JKQtPlotter*)));

        pltDistResults->setX(newxmin, newxmax);
        pltDistResults->zoomToFit(false, true);
        connect(pltDistResults, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(distzoomChangedLocally(double,double,double,double,JKQtPlotter*)));

    }
    if (sender==pltDistResults) {
        disconnect(pltDistribution, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(distzoomChangedLocally(double,double,double,double,JKQtPlotter*)));
        pltDistribution->setX(newxmin, newxmax);
        pltDistribution->zoomToFit(false, true);
        connect(pltDistribution, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(distzoomChangedLocally(double,double,double,double,JKQtPlotter*)));
    }
}

struct averageFirstFewFramesData {
        QFRawDataRecord* record;
        QFRDRFCSDataInterface* data;
        int run;
};

void QFFCSMSDEvaluationEditor::averageFirstFewLags() {

    /* EXECUTE AN EVALUATION FOR THE CURRENT RECORD ONLY */
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    if ((!eval)||(!record)||(!data)) return;

    int data_start=datacut->get_userMin();
    int data_end=datacut->get_userMax();

    QFFCSMSDEvaluationAverageChannelsDialog* dlg=new QFFCSMSDEvaluationAverageChannelsDialog(this);
    dlg->init(ProgramOptions::getConfigValue("QFFCSMSDEvaluationEditor/avgruns_last", 5).toInt(), 1, data_end-data_start);
    //bool ok=false;
    //int points=QInputDialog::getInt(this, windowTitle(), tr("number of points to average"), ProgramOptions::getConfigValue("QFFCSMSDEvaluationEditor/avgruns_last", 5).toInt(), 1, data_end-data_start, 1, &ok);
    if (dlg->exec()) {
        int points=dlg->getPoints();
        ProgramOptions::setConfigValue("QFFCSMSDEvaluationEditor/avgruns_last", points);
        double *d=NULL;

        QList<averageFirstFewFramesData > applyTo;
        if (dlg->getApplyTo()==0) { // current
            averageFirstFewFramesData dr;
            dr.record=record;
            dr.data=data;
            dr.run=eval->getCurrentIndex();
            applyTo.append(dr);
        } else if (dlg->getApplyTo()==1) { // all runs
            for (int i=eval->getIndexMin(record); i<=eval->getIndexMax(record); i++) {
                averageFirstFewFramesData dr;
                dr.record=record;
                dr.data=data;
                dr.run=i;
                applyTo.append(dr);
            }
        } else if (dlg->getApplyTo()==2) { // all files, this runs
            QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
            for (int i=0; i<recs.size(); i++) {
                averageFirstFewFramesData dr;
                dr.record=recs[i];
                dr.data=qobject_cast<QFRDRFCSDataInterface*>(dr.record);
                dr.run=eval->getCurrentIndex();
                if (dr.record&&dr.data && eval->getIndexMin(dr.record)<=dr.run && dr.run<=eval->getIndexMax(dr.record)) applyTo.append(dr);
            }
        } else if (dlg->getApplyTo()==3) { // everything
            QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
            for (int i=0; i<recs.size(); i++) {
                for (int r=eval->getIndexMin(recs[i]); r<=eval->getIndexMax(recs[i]); r++) {
                    averageFirstFewFramesData dr;
                    dr.record=recs[i];
                    dr.data=qobject_cast<QFRDRFCSDataInterface*>(dr.record);
                    dr.run=r;
                    if (dr.record&&dr.data && eval->getIndexMin(dr.record)<=dr.run && dr.run<=eval->getIndexMax(dr.record)) applyTo.append(dr);
                }
            }
        }

        QModernProgressDialog progress(tr("estimating N ..."), tr("Cancel"), this);
        progress.show();
        progress.setMode(true, true);
        progress.setRange(0, applyTo.size());

        for (int i=0; i<applyTo.size(); i++) {
            progress.setValue(i);
            data=applyTo[i].data;
            record=applyTo[i].record;
            int run=applyTo[i].run;

            data_start=getUserMin(record, run);
            data_end=getUserMax(record, run);

            if (run<0) {
                d=data->getCorrelationMean();
            } else {
                if (eval->getCurrentIndex()<(int)data->getCorrelationRuns()) {
                    d=data->getCorrelationRun(run);
                } else {
                    d=data->getCorrelationMean();
                }
            }
            if (d) {
                double avg=0;
                double cnt=0;
                for (int j=data_start; j<qMin(data_end, data_start+points); j++) {
                    avg=avg+d[j];
                    cnt++;
                }
                avg=avg/cnt;
                for (int j=0; j<eval->getParameterCount(eval->getCurrentModel()); j++) {
                    QString pid=eval->getParameterID(eval->getCurrentModel(), j);
                    //qDebug()<<pid<<1.0/avg;
                    if (pid=="n_particle") {
                        if (j==0) eval->setFitValue(record, run, eval->getCurrentModel(), pid, 1.0/avg);
                        eval->setFitResultValue(record, run, eval->getCurrentModel(), pid, 1.0/avg);
                    }
                }
            }
            QApplication::processEvents();
            if (progress.wasCanceled()) break;
        }

    }
    delete dlg;
    widFitParams->updateWidgetValues();
    fitParamChanged();

}

void QFFCSMSDEvaluationEditor::getNFromFits()
{
    /* EXECUTE AN EVALUATION FOR THE CURRENT RECORD ONLY */
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    if ((!eval)||(!record)||(!data)) return;

    int data_start=datacut->get_userMin();
    int data_end=datacut->get_userMax();

    QFFCSMSDEvaluationGetNFromFits* dlg=new QFFCSMSDEvaluationGetNFromFits(record, this);
    //bool ok=false;
    //int points=QInputDialog::getInt(this, windowTitle(), tr("number of points to average"), ProgramOptions::getConfigValue("QFFCSMSDEvaluationEditor/avgruns_last", 5).toInt(), 1, data_end-data_start, 1, &ok);
    if (dlg->exec()) {

        QList<averageFirstFewFramesData > applyTo;
        if (dlg->getApplyTo()==0) { // current
            averageFirstFewFramesData dr;
            dr.record=record;
            dr.data=data;
            dr.run=eval->getCurrentIndex();
            applyTo.append(dr);
        } else if (dlg->getApplyTo()==1) { // all runs
            for (int i=eval->getIndexMin(record); i<=eval->getIndexMax(record); i++) {
                averageFirstFewFramesData dr;
                dr.record=record;
                dr.data=data;
                dr.run=i;
                applyTo.append(dr);
            }
        } else if (dlg->getApplyTo()==2) { // all files, this runs
            QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
            for (int i=0; i<recs.size(); i++) {
                averageFirstFewFramesData dr;
                dr.record=recs[i];
                dr.data=qobject_cast<QFRDRFCSDataInterface*>(dr.record);
                dr.run=eval->getCurrentIndex();
                if (dr.record&&dr.data && eval->getIndexMin(dr.record)<=dr.run && dr.run<=eval->getIndexMax(dr.record)) applyTo.append(dr);
            }
        } else if (dlg->getApplyTo()==3) { // everything
            QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
            for (int i=0; i<recs.size(); i++) {
                for (int r=eval->getIndexMin(recs[i]); r<=eval->getIndexMax(recs[i]); r++) {
                    averageFirstFewFramesData dr;
                    dr.record=recs[i];
                    dr.data=qobject_cast<QFRDRFCSDataInterface*>(dr.record);
                    dr.run=r;
                    if (dr.record&&dr.data && eval->getIndexMin(dr.record)<=dr.run && dr.run<=eval->getIndexMax(dr.record)) applyTo.append(dr);
                }
            }
        }


        QString evalGroup=dlg->getEvalGroup();
        QString readParamN=dlg->getParameterN();
        QString readParamTF=dlg->getParameterTripletTheta();
        QString readParamTTau=dlg->getParameterTripletTau();
        //qDebug()<<evalGroup<<readParam;
        QModernProgressDialog progress(tr("reading parameters from fit results ..."), tr("Cancel"), this);
        progress.show();
        progress.setMode(true, true);
        progress.setRange(0, applyTo.size());

        for (int i=0; i<applyTo.size(); i++) {
            progress.setValue(i);
            data=applyTo[i].data;
            record=applyTo[i].record;
            int run=applyTo[i].run;

            double N=-1;
            double tauT=-1;
            double thetaT=-1;


            if (record) {
                QStringList sl=record->resultsCalcNames("", "", evalGroup);
                QStringList eval=record->resultsCalcEvaluationsInGroup(evalGroup);
                QString runid="_runavg";
                if (run>=0) runid=QString("_run%1").arg(run);
                bool hasTrip=false;
                bool hasN=false;
                bool hasTripT=false;
                for (int ei=0; ei<eval.size(); ei++) {
                    if (eval[ei].endsWith(runid)) {
                        if (! hasN && dlg->getN() && record->resultsExists(eval[ei], readParamN)) {
                            bool ok=false;
                            double NN=record->resultsGetAsDouble(eval[ei], readParamN, &ok);
                            if (ok) {
                                N=NN;
                                hasN=true;
                            }
                        }
                        if (!hasTripT && dlg->getTriplet() && record->resultsExists(eval[ei], readParamTTau)) {
                            bool ok=false;
                            double NN=record->resultsGetAsDouble(eval[ei], readParamTTau, &ok);
                            if (ok) {
                                tauT=NN;
                                hasTripT=true;
                            }
                        }
                        if (!hasTrip && dlg->getTriplet() && record->resultsExists(eval[ei], readParamTF)) {
                            bool ok=false;
                            double NN=record->resultsGetAsDouble(eval[ei], readParamTF, &ok);
                            if (ok) {
                                thetaT=NN;
                                hasTrip=true;
                            }
                        }
                    }
                }
                //qDebug()<<record->getName()<<" run="<<run<<":  N="<<N;
            }



            /*for (int j=0; j<eval->getParameterCount(eval->getCurrentModel()); j++) {
                QString pid=eval->getParameterID(eval->getCurrentModel(), j);
                //qDebug()<<pid<<1.0/avg;
                if (dlg->getN() && N>0 && pid=="n_particle") {
                    if (j==0) eval->setFitValue(record, run, eval->getCurrentModel(), pid, N);
                    eval->setFitResultValue(record, run, eval->getCurrentModel(), pid, N);
                } else if (dlg->getTriplet() && thetaT>0 && pid=="nonfl_theta1") {
                    if (j==0) eval->setFitValue(record, run, eval->getCurrentModel(), pid, thetaT);
                    eval->setFitResultValue(record, run, eval->getCurrentModel(), pid, thetaT);
                } else if (dlg->getTriplet() && tauT>0 && pid=="nonfl_tau1") {
                    if (j==0) eval->setFitValue(record, run, eval->getCurrentModel(), pid, tauT);
                    eval->setFitResultValue(record, run, eval->getCurrentModel(), pid, tauT);
                }
            }*/
            QString pid="n_particle";
            if (dlg->getN() && N>0) {
                //if (j==0) eval->setFitValue(record, run, eval->getCurrentModel(), pid, N);
                eval->setFitResultValue(record, run, eval->getCurrentModel(), pid, N);
            }
            pid="nonfl_theta1";
            if (dlg->getTriplet() && thetaT>0) {
                //if (j==0) eval->setFitValue(record, run, eval->getCurrentModel(), pid, thetaT);
                eval->setFitResultValue(record, run, eval->getCurrentModel(), pid, thetaT);
            }
            pid="nonfl_tau1";
            if (dlg->getTriplet() && tauT>0) {
                //if (j==0) eval->setFitValue(record, run, eval->getCurrentModel(), pid, tauT);
                eval->setFitResultValue(record, run, eval->getCurrentModel(), pid, tauT);
            }
            QApplication::processEvents();
            if (progress.wasCanceled()) break;
        }
    }

    delete dlg;
    widFitParams->updateWidgetValues();
    fitParamChanged();
    //QFFCSMSDEvaluationGetNFromFits;
}

void QFFCSMSDEvaluationEditor::copyAverageData() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFRDRRunSelectionsInterface* rundata=qobject_cast<QFRDRRunSelectionsInterface*>(record);
    if ((!eval)||(!record)||(!data)) return;

    QStringList resultLabels;
    QList<QVariant> resultNames;
    QList<QColor> resultColors;
    QList<bool> selected;

    int runstart=0;
    int runend=data->getCorrelationRuns();
    if (data->getCorrelationRuns()==1) { runstart=-1; runend=0; }
    for (int run=runstart; run<runend; run++) {
        QString evalID=eval->getEvaluationResultID(run, eval->getCurrentModel());
        if (record->resultsExists(evalID, "msd") && record->resultsExists(evalID, "msd_tau")) {
            resultLabels.append(tr("run %1").arg(run));
            resultNames.append(run);
            if (!rundata || (rundata && !rundata->leaveoutRun(run))) {
                selected<<true;
                resultColors<<palette().color(QPalette::WindowText);
            } else {
                selected<<false;
                resultColors<<QColor("grey");
            }
        }
    }

    QFSelectionListDialog* dlg=new QFSelectionListDialog(this);
    dlg->init(resultLabels, resultNames, resultColors, *(ProgramOptions::getInstance()->getQSettings()), "fcsmsdevaleditor/copysel/");
    dlg->setLabel(tr("Select the runs you want to average over (gray runs are excluded!)\nand then below the averaging options!"));
    dlg->selectItems(selected);

    QCheckBox* chkCopyDofTau=new QCheckBox("", dlg);
    chkCopyDofTau->setChecked(ProgramOptions::getInstance()->getQSettings()->value("fcsmsdevaleditor/copysel/copyDofTau", false).toBool());
    dlg->addWidget(tr("copy <i>D(&tau;)</i> curve:"), chkCopyDofTau);
    QCheckBox* chkCopyAlphaTau=new QCheckBox("", dlg);
    chkCopyAlphaTau->setChecked(ProgramOptions::getInstance()->getQSettings()->value("fcsmsdevaleditor/copysel/copyAlphaTau", false).toBool());
    dlg->addWidget(tr("copy <i>&alpha;(&tau;)</i> curve:"), chkCopyAlphaTau);
    QCheckBox* chkCopyRaw=new QCheckBox("", dlg);
    chkCopyRaw->setChecked(ProgramOptions::getInstance()->getQSettings()->value("fcsmsdevaleditor/copysel/copyRaw", false).toBool());
    chkCopyRaw->setToolTip(tr("if this is activated, all the single run's data, used for the average and standard deviation will also be copied."));
    dlg->addWidget(tr("copy also single runs' data:"), chkCopyRaw);

    dlg->setWindowTitle(tr("Copy averaged MSD data"));

    if (dlg->exec()) {
        ProgramOptions::getInstance()->getQSettings()->setValue("fcsmsdevaleditor/copysel/copyDofTau", chkCopyDofTau->isChecked());
        ProgramOptions::getInstance()->getQSettings()->setValue("fcsmsdevaleditor/copysel/copyAlphaTau", chkCopyAlphaTau->isChecked());
        ProgramOptions::getInstance()->getQSettings()->setValue("fcsmsdevaleditor/copysel/copyRaw", chkCopyRaw->isChecked());
        dlg->writeList(*(ProgramOptions::getInstance()->getQSettings()), "fcsmsdevaleditor/copysel/");

        QProgressDialog progress;
        progress.show();
        QList<QVariant> sel=dlg->getSelected();
        QList<double> tau, sum, sum2, fitTau, Dsum, Dsum2, Asum, Asum2;
        QList<QVector<double> > AA, DD;
        double Ncnt=0;
        QString errorMessage="";
        progress.setRange(0, sel.size());
        for (int i=0; i<sel.size(); i++) {
            progress.setLabelText(tr("processing MSD data for copying ... run %1/%2").arg(i+1).arg(sel.size()));
            progress.setValue(i);
            QApplication::processEvents();
            Ncnt++;
            int run=sel[i].toInt();
            QString evalID=eval->getEvaluationResultID(run, eval->getCurrentModel());
            QVector<double> t=eval->getMSDTaus(record, run, eval->getCurrentModel());
            QVector<double> d=eval->getMSD(record, run, eval->getCurrentModel());
            QVector<double> taus_out, alpha_out, D_out;
            if (chkCopyDofTau->isChecked()||chkCopyAlphaTau->isChecked()) {
                eval->calcMSDFits(taus_out, alpha_out, D_out, record, run, eval->getCurrentModel(), spinFitWidth->value(), qMax(1, spinFitWidth->value()/7), sliderDist->get_userMin(), cmbFitType->currentIndex());
                AA.append(alpha_out);
                DD.append(D_out);
            }

            if (i==0) {
                sum=d.toList();
                tau=t.toList();
                fitTau=taus_out.toList();
                Dsum=D_out.toList();
                Asum=alpha_out.toList();
                for (int c=0; c<d.size(); c++) {
                    sum2.append(d[c]*d[c]);
                }
                for (int c=0; c<D_out.size(); c++) {
                    Dsum2.append(D_out[c]*D_out[c]);
                    Asum2.append(alpha_out[c]*alpha_out[c]);
                }
            } else {
                if (tau.size()!=d.size() || sum.size()!=d.size() || sum2.size()!=d.size()) {
                    errorMessage=tr("Can not average over runs: MSD was calculated for run %1 with a different tau-scale!").arg(run);
                } else {
                    for (int c=0; c<d.size(); c++) {
                        if (tau[c] != t[c]) {
                            errorMessage=tr("Can not average over runs: MSD was calculated for run %1 with a different tau-scale!").arg(run);
                        } else {
                            sum[c]=sum[c]+d[c];
                            sum2[c]=sum2[c]+d[c]*d[c];
                        }
                    }
                    if (errorMessage.isEmpty() && (chkCopyDofTau->isChecked()||chkCopyAlphaTau->isChecked())) {
                        for (int c=0; c<D_out.size(); c++) {
                            Dsum[c]=Dsum[c]+(D_out[c]);
                            Asum[c]=Asum[c]+(alpha_out[c]);
                            Dsum2[c]=Dsum2[c]+(D_out[c]*D_out[c]);
                            Asum2[c]=Asum2[c]+(alpha_out[c]*alpha_out[c]);
                        }
                    }
                }
            }
            if (!errorMessage.isEmpty()) break;
            if (progress.wasCanceled()) break;
        }
        if (errorMessage.isEmpty() && !progress.wasCanceled()) {
            QList<QList<double> > data;
            QStringList colNames;
            colNames<<tr("tau [s]")<<tr("avg(msd) [�m�]")<<tr("SD(msd) [�m�]");
            data.append(tau);
            for (int i=0; i<sum.size(); i++) {
                double s=sum[i];
                double s2=sum2[i];
                sum[i]=s/Ncnt;
                if (Ncnt>=2) sum2[i]=sqrt((s2-s*s/Ncnt)/(Ncnt-1.0));
                else sum2[i]=0;
            }
            data.append(sum);
            data.append(sum2);

            if (chkCopyRaw->isChecked()) {
                for (int i=0; i<sel.size(); i++) {
                    int run=sel[i].toInt();
                    QVector<double> d=eval->getMSD(record, run, eval->getCurrentModel());
                    colNames<<tr("msd_run%1 [�m�]").arg(run);
                    data.append(d.toList());
                }
            }

            if (chkCopyDofTau->isChecked()||chkCopyAlphaTau->isChecked()) {
                colNames<<tr("tauDA [s]");
                data.append(fitTau);
            }
            if (chkCopyDofTau->isChecked()) {
                colNames<<tr("avg(D(tauDA)) [�m�/s]")<<tr("SD(D(tauDA)) [�m�/s]");
                for (int i=0; i<Dsum.size(); i++) {
                    double s=Dsum[i];
                    double s2=Dsum2[i];
                    Dsum[i]=s/Ncnt;
                    if (Ncnt>=2) Dsum2[i]=sqrt((s2-s*s/Ncnt)/(Ncnt-1.0));
                    else Dsum2[i]=0;
                }
                data.append(Dsum);
                data.append(Dsum2);
                if (chkCopyRaw->isChecked()) {
                    for (int i=0; i<sel.size(); i++) {
                        int run=sel[i].toInt();
                        colNames<<tr("D_run%1 [�m�/s]").arg(run);
                        data.append(DD[i].toList());
                    }
                }
            }
            if (chkCopyAlphaTau->isChecked()) {
                colNames<<tr("avg(alpha(tauDA))")<<tr("SD(alpha(tauDA))");
                for (int i=0; i<Asum.size(); i++) {
                    double s=Asum[i];
                    double s2=Asum2[i];
                    Asum[i]=s/Ncnt;
                    if (Ncnt>=2) Asum2[i]=sqrt((s2-s*s/Ncnt)/(Ncnt-1.0));
                    else Asum2[i]=0;
                }
                data.append(Asum);
                data.append(Asum2);
                if (chkCopyRaw->isChecked()) {
                    for (int i=0; i<sel.size(); i++) {
                        int run=sel[i].toInt();
                        colNames<<tr("alpha_run%1").arg(run);
                        data.append(AA[i].toList());
                    }
                }
            }

            csvCopy(data, colNames);
        } else {
            QMessageBox::critical(this, tr("copy averaged MSD data"), tr("ERROR:\n%1").arg(errorMessage), QMessageBox::Ok, QMessageBox::Ok);
        }

    }
    delete dlg;
}

void QFFCSMSDEvaluationEditor::fitAllMSD()
{

    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!dataEventsEnabled) return;
    if (!data) return;


    QFFCSMSDEvaluationFitAllMSDDialog* dlg=new QFFCSMSDEvaluationFitAllMSDDialog(data, this);

    dlg->exec();

    for (int i=0; i<MSDTHEORYCOUNT; i++)     {
        chkSlope[i]->setChecked(data->getTheoryEnabled(i, data->getHighlightedRecord(), data->getCurrentIndex()));
        numPre[i]->setValue(data->getTheoryPre(i, data->getHighlightedRecord(), data->getCurrentIndex()));
        numD[i]->setValue(data->getTheoryD(i, data->getHighlightedRecord(), data->getCurrentIndex()));
        numAlpha[i]->setValue(data->getTheoryAlpha(i, data->getHighlightedRecord(), data->getCurrentIndex()));
        //if (i<MSDTHEORYCOUNT-1) numIntersect[i]->setValue(data->getTheoryIntersect(i, data->getHighlightedRecord(), data->getCurrentIndex()));
        if (i>0) {
            numIntersect[i-1]->setValue(data->getTheoryIntersect(i-1, data->getHighlightedRecord(), data->getCurrentIndex()));
            numIntersect[i-1]->setEnabled(chkSlope[i-1]->isChecked()&&chkSlope[i]->isChecked());
        }
    }

    delete dlg;

    updateDistribution();
}





void QFFCSMSDEvaluationEditor::fitCurrent() {
    /* EXECUTE AN EVALUATION FOR THE CURRENT RECORD ONLY */
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if ((!eval)||(!record)/*||(!data)*/) return;



    dlgEvaluationProgress->setLabelText(tr("evaluate '%1' ...").arg(record->getName()));

    dlgEvaluationProgress->setRange(0,100);
    dlgEvaluationProgress->setValue(50);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    bool oldde=dataEventsEnabled;
    dataEventsEnabled=false;

    // here we call doEvaluation to execute our evaluation for the current record only
    eval->doFit(record, eval->getCurrentIndex(), eval->getCurrentModel(), getUserMin(record, eval->getCurrentIndex()), getUserMax(record, eval->getCurrentIndex()), 11, spinResidualHistogramBins->value());    
    dataEventsEnabled=oldde;
    eval->emitResultsChanged(record);
    record->emitResultsChanged();


    updateSliders();
    displayParameters();
    displayData();
    dlgEvaluationProgress->setValue(100);
    dlgEvaluationProgress->close();

    QApplication::restoreOverrideCursor();
}


void QFFCSMSDEvaluationEditor::fitRunsCurrent() {
    /* EXECUTE AN EVALUATION FOR ALL RECORDS */
    if (!current) return;

    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!eval) return;

    // get a list of all raw data records this evaluation is applicable to
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    dlgEvaluationProgress->setRange(0,recs.size());
    dlgEvaluationProgress->setValue(0);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    bool oldde=dataEventsEnabled;
    dataEventsEnabled=false;

    // iterate through all records and all runs therein and do the fits
    //for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=eval->getHighlightedRecord();
        QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
        QApplication::processEvents();
        if ((record)&&(data)) {
            // here we call doEvaluation to execute our evaluation for the current record only

            dlgEvaluationProgress->setRange(0,data->getCorrelationRuns());
            for (int idx=-1; idx<data->getCorrelationRuns(); idx++) {
                dlgEvaluationProgress->setLabelText(tr("evaluate '%1', run %2 ...").arg(record->getName()).arg(idx));
                QApplication::processEvents();
                eval->doFit(record, idx, eval->getCurrentModel(), getUserMin(record, idx), getUserMax(record, idx), 11, spinResidualHistogramBins->value());
                QApplication::processEvents();
                // check whether the user canceled this evaluation
                if (dlgEvaluationProgress->wasCanceled()) break;
                dlgEvaluationProgress->setValue(idx);
            }
        }
        record->emitResultsChanged();
        QApplication::processEvents();
    //}
    dataEventsEnabled=oldde;
    eval->emitResultsChanged(record);
    dlgEvaluationProgress->setValue(recs.size());
    dlgEvaluationProgress->close();
    updateSliders();
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}


void QFFCSMSDEvaluationEditor::fitAll() {
    /* EXECUTE AN EVALUATION FOR ALL RECORDS */
    if (!current) return;

    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!eval) return;

    // get a list of all raw data records this evaluation is applicable to
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    dlgEvaluationProgress->setRange(0,recs.size());
    dlgEvaluationProgress->setValue(0);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool oldde=dataEventsEnabled;
    dataEventsEnabled=false;


    // iterate through all records and all runs therein and do the fits
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i]; 
        QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
        QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
        QApplication::processEvents();
        if ((record)&&(data)) {
            // here we call doEvaluation to execute our evaluation for the current record only

            for (int idx=-1; idx<data->getCorrelationRuns(); idx++) {
                dlgEvaluationProgress->setLabelText(tr("evaluate '%1', run %2 ...").arg(record->getName()).arg(idx));
                QApplication::processEvents();
                eval->doFit(record, idx, eval->getCurrentModel(), getUserMin(record, idx), getUserMax(record, idx), 11, spinResidualHistogramBins->value());
                QApplication::processEvents();
                // check whether the user canceled this evaluation
                if (dlgEvaluationProgress->wasCanceled()) break;
            }
            record->emitResultsChanged();
        }
        dlgEvaluationProgress->setValue(i);
        QApplication::processEvents();
        // check whether the user canceled this evaluation
        if (dlgEvaluationProgress->wasCanceled()) break;
    }
    dataEventsEnabled=oldde;
    eval->emitResultsChanged();
    dlgEvaluationProgress->setValue(recs.size());
    dlgEvaluationProgress->close();
    updateSliders();
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}



void QFFCSMSDEvaluationEditor::fitRunsAll() {
    /* EXECUTE AN EVALUATION FOR ALL RECORDS */
    if (!current) return;

    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!eval) return;

    // get a list of all raw data records this evaluation is applicable to
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    dlgEvaluationProgress->setRange(0,recs.size());
    dlgEvaluationProgress->setValue(0);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool oldde=dataEventsEnabled;
    dataEventsEnabled=false;


    // iterate through all records and all runs therein and do the fits
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
        QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
        QApplication::processEvents();
        if ((record)&&(data)) {
            // here we call doEvaluation to execute our evaluation for the current record only

            //for (int idx=-1; idx<data->getCorrelationRuns(); idx++) {
                int idx=eval->getCurrentIndex();
                dlgEvaluationProgress->setLabelText(tr("evaluate '%1', run %2 ...").arg(record->getName()).arg(idx));
                QApplication::processEvents();
                eval->doFit(record, idx, eval->getCurrentModel(), getUserMin(record, idx), getUserMax(record, idx), 11, spinResidualHistogramBins->value());
                QApplication::processEvents();
                // check whether the user canceled this evaluation
                if (dlgEvaluationProgress->wasCanceled()) break;
            //}
                record->emitResultsChanged();
        }
        dlgEvaluationProgress->setValue(i);
        QApplication::processEvents();
        // check whether the user canceled this evaluation
        if (dlgEvaluationProgress->wasCanceled()) break;
    }
    dataEventsEnabled=oldde;
    eval->emitResultsChanged();
    dlgEvaluationProgress->setValue(recs.size());
    dlgEvaluationProgress->close();
    updateSliders();
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}


void QFFCSMSDEvaluationEditor::copyMoreData(QFRawDataRecord *record, int index, int model) {
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!eval) return;

    //eval->setFitValue(record, index, model, "maxent_alpha", eval->getAlpha());
    //eval->setFitValue(record, index, model, "maxent_numiter", eval->getNumIter());
    //eval->setFitValue(record, index, model, "maxent_Ndist", eval->getNdist());
}

void QFFCSMSDEvaluationEditor::copyMoreDataToInitial() {
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!eval) return;

    /*eval->setInitFitValue("maxent_alpha", eval->getAlpha());
    eval->setInitFitValue("maxent_numiter", eval->getNumIter());
    eval->setInitFitValue("maxent_Ndist", eval->getNdist());*/
}

double fMSD( double t, const double *p )
{
    const double D=p[0];
    const double a=p[1];
    return 6.0*D*pow(t, a);
}

double fMSD_lin( double t, const double *p )
{
    const double D=p[0];
    const double a=p[1];
    return log(6.0*D)+a*log(t);
}

void QFFCSMSDEvaluationEditor::updateDistributionResults() {
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!eval) return;
    QVector<double> distTau=eval->getMSDTaus(eval->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel());
    QVector<double> dist=eval->getMSD(eval->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel());
    //if (distTau.size()>0) qDebug()<<distTau.first()<<distTau.last();
    int data_start=sliderDist->get_userMin();
    int data_end=sliderDist->get_userMax();
    int msd_start=sliderDist->get_userMin();
    int msd_end=sliderDist->get_userMax();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    JKQTPdatastore* dsdist=pltDistResults->get_plotter()->getDatastore();

    bool updt=pltDistResults->get_doDrawing();
    pltDistResults->set_doDrawing(false);
    pltDistResults->clearGraphs();
    pltDistResults->get_plotter()->set_showKey(chkShowKeyDistResults->isChecked());

    //qDebug()<<dsdist->getColumnCount();
    dsdist->deleteAllColumns("msdfit_tau_start");
    dsdist->deleteAllColumns("msdfit_tau_end");
    dsdist->deleteAllColumns("msdfit_tau");
    dsdist->deleteAllColumns("msdfit_D");
    dsdist->deleteAllColumns("msdfit_alpha");
    dsdist->deleteAllColumns("msdtransform_tau");
    dsdist->deleteAllColumns("msdtransform_div6tau");
    dsdist->deleteAllColumns("msdtransform_div4tau");
    dsdist->deleteAllColumns("msdtransform_divPD0tau");
    dsdist->deleteAllColumns("msdtransform_minusPD0tau");
    dsdist->deleteAllColumns("msdtransform_divPD1tau");
    dsdist->deleteAllColumns("msdtransform_divPD2tau");
    dsdist->deleteAllColumns("msdtransform_divPD3tau");
    //qDebug()<<dsdist->getColumnCount();
    int c_tau=dsdist->addCopiedColumn(distTau.data(), distTau.size(), "msdtransform_tau");

    int wid=spinFitWidth->value();
    int first=0;
    int last=distTau.size()-1;
    if (chkFitRange->isChecked()) {
        first=sliderDist->get_userMin();
        last=sliderDist->get_userMax();
    }

    QVector<double> fitTau, fitTauStart, fitTauEnd;
    QVector<double> fitD, fitA;
    if (distTau.size()>1 && dist.size()>1 && wid>=3 && eval->hasResults()) {

        eval->calcMSDFits(fitTau, fitA, fitD, eval->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel(), spinFitWidth->value(), qMax(1, spinFitWidth->value()/7), sliderDist->get_userMin(), cmbFitType->currentIndex(), &fitTauStart, &fitTauEnd);

        QVector<double> mored=dist;
        for (int i=0; i<dist.size(); i++) {
            mored[i]=dist[i]/(6.0*distTau[i]);
        }
        int c_msdtransform6tau=dsdist->addCopiedColumn(mored.data(), mored.size(), "msdtransform_div6tau");
        mored=dist;
        for (int i=0; i<dist.size(); i++) {
            mored[i]=dist[i]/(4.0*distTau[i]);
        }
        int c_msdtransform4tau=dsdist->addCopiedColumn(mored.data(), mored.size(), "msdtransform_div4tau");
        mored=dist;
        double P=eval->getTheoryPre(0, eval->getHighlightedRecord(), eval->getCurrentIndex());
        double D=eval->getTheoryD(0, eval->getHighlightedRecord(), eval->getCurrentIndex());
        for (int i=0; i<dist.size(); i++) {
            mored[i]=dist[i]/(P*D*distTau[i]);
        }
        int c_msdtransformPD0tau=dsdist->addCopiedColumn(mored.data(), mored.size(), "msdtransform_divPD0tau");
        mored=dist;
        P=eval->getTheoryPre(1, eval->getHighlightedRecord(), eval->getCurrentIndex());
        D=eval->getTheoryD(1, eval->getHighlightedRecord(), eval->getCurrentIndex());
        for (int i=0; i<dist.size(); i++) {
            mored[i]=dist[i]/(P*D*distTau[i]);
        }
        int c_msdtransformPD1tau=dsdist->addCopiedColumn(mored.data(), mored.size(), "msdtransform_divPD1tau");
        mored=dist;
        P=eval->getTheoryPre(2, eval->getHighlightedRecord(), eval->getCurrentIndex());
        D=eval->getTheoryD(2, eval->getHighlightedRecord(), eval->getCurrentIndex());
        for (int i=0; i<dist.size(); i++) {
            mored[i]=dist[i]/(P*D*distTau[i]);
        }
        int c_msdtransformPD2tau=dsdist->addCopiedColumn(mored.data(), mored.size(), "msdtransform_divPD2tau");
        mored=dist;
        P=eval->getTheoryPre(3, eval->getHighlightedRecord(), eval->getCurrentIndex());
        D=eval->getTheoryD(3, eval->getHighlightedRecord(), eval->getCurrentIndex());
        for (int i=0; i<dist.size(); i++) {
            mored[i]=dist[i]/(P*D*distTau[i]);
        }
        int c_msdtransformPD3tau=dsdist->addCopiedColumn(mored.data(), mored.size(), "msdtransform_divPD3tau");
        mored=dist;
        P=eval->getTheoryPre(0, eval->getHighlightedRecord(), eval->getCurrentIndex());
        D=eval->getTheoryD(0, eval->getHighlightedRecord(), eval->getCurrentIndex());
        for (int i=0; i<dist.size(); i++) {
            mored[i]=dist[i]-(P*D*distTau[i]);
        }
        int c_msdtransformMinusPD0tau=dsdist->addCopiedColumn(mored.data(), mored.size(), "msdtransform_minusPD0tau");


        int c_msdtau=dsdist->addCopiedColumn(fitTau.data(), fitTau.size(), "msdfit_tau");
        int c_msdD=dsdist->addCopiedColumn(fitD.data(), fitD.size(), "msdfit_D");
        int c_msdA=dsdist->addCopiedColumn(fitA.data(), fitA.size(), "msdfit_alpha");
        dsdist->addCopiedColumn(fitTauStart.data(), fitTauStart.size(), "msdfit_tau_start");
        dsdist->addCopiedColumn(fitTauEnd.data(), fitTauEnd.size(), "msdfit_tau_end");

        //qDebug()<<dsdist->getColumnCount();
        //qDebug()<<c_msdtransformPD0tau<<c_msdtransformPD1tau<<c_msdtransformPD2tau<<c_msdtransformPD3tau<<c_msdtau<<c_msdD<<c_msdA;
        //qDebug()<<fitTau.size()<<fitTau;
        //qDebug()<<fitD.size()<<fitD;
        //qDebug()<<fitA.size()<<fitA;

        double t_start=distTau.value(data_start, distTau.first());
        double t_end=distTau.value(data_end, distTau.last());
        //qDebug()<<"t_start="<<t_start<<"   t_end="<<t_end;
        //qDebug()<<"init: data_start="<<data_start<<"   data_end="<<data_end;

        data_start=0;
        data_end=fitTau.size();
        for (int i=0; i<fitTau.size(); i++) {
            //qDebug()<<"   "<<i<<": "<<fitTau[i];
            if (fitTau[i]<=t_start) data_start=i;
            if (fitTau[i]<=t_end) data_end=i;
        }

        //qDebug()<<"found: data_start="<<data_start<<"   data_end="<<data_end;

        pltDistResults->getYAxis()->set_logAxis(false);
        if (cmbDistResultsMode->currentIndex()==0) {
            JKQTPxyLineGraph* g_msdfit=new JKQTPxyLineGraph(pltDistribution->get_plotter());
            g_msdfit->set_drawLine(true);
            g_msdfit->set_title("local \\alpha");
            g_msdfit->set_xColumn(c_msdtau);
            g_msdfit->set_yColumn(c_msdA);
            g_msdfit->set_datarange_start(data_start);
            g_msdfit->set_datarange_end(data_end);
            g_msdfit->set_symbol(JKQTPcross);
            g_msdfit->set_symbolSize(7);
            g_msdfit->set_color(QColor("red"));
            pltDistResults->addGraph(g_msdfit);
            pltDistResults->getYAxis()->set_axisLabel("local \\alpha");
        } else if (cmbDistResultsMode->currentIndex()==1)  {
            JKQTPxyLineGraph* g_msdfit=new JKQTPxyLineGraph(pltDistribution->get_plotter());
            g_msdfit->set_drawLine(true);
            g_msdfit->set_title("local $D$");
            g_msdfit->set_xColumn(c_msdtau);
            g_msdfit->set_yColumn(c_msdD);
            g_msdfit->set_datarange_start(data_start);
            g_msdfit->set_datarange_end(data_end);
            g_msdfit->set_symbol(JKQTPcross);
            g_msdfit->set_symbolSize(7);
            g_msdfit->set_color(QColor("red"));
            pltDistResults->addGraph(g_msdfit);
            pltDistResults->getYAxis()->set_axisLabel("local $D$ [{\\mu}m^2/s]");
        } else if (cmbDistResultsMode->currentIndex()>=2 && cmbDistResultsMode->currentIndex()<=8 )  {
            JKQTPxyLineGraph* g_msdtransform=new JKQTPxyLineGraph(pltDistribution->get_plotter());
            if (cmbDistResultsMode->currentIndex()==2) {
                g_msdtransform->set_title("\\langle r^2(\\tau)\\rangle/(6\\tau)");
                g_msdtransform->set_xColumn(c_tau);
                g_msdtransform->set_yColumn(c_msdtransform6tau);
                g_msdtransform->set_color(QColor("red"));
                pltDistResults->getYAxis()->set_axisLabel("\\langle r^2(\\tau)\\rangle/(6\\tau) [{\\mu}m^2]");
                pltDistResults->getYAxis()->set_logAxis(true);
            } else if (cmbDistResultsMode->currentIndex()==3) {
                g_msdtransform->set_title("\\langle r^2(\\tau)\\rangle/(4\\tau)");
                g_msdtransform->set_xColumn(c_tau);
                g_msdtransform->set_yColumn(c_msdtransform4tau);
                g_msdtransform->set_color(QColor("red"));
                pltDistResults->getYAxis()->set_axisLabel("\\langle r^2(\\tau)\\rangle/(4\\tau) [{\\mu}m^2]");
                pltDistResults->getYAxis()->set_logAxis(true);
            } else if (cmbDistResultsMode->currentIndex()==4) {
                g_msdtransform->set_title("\\langle r^2(\\tau)\\rangle/(P_0D_0\\tau)");
                g_msdtransform->set_xColumn(c_tau);
                g_msdtransform->set_yColumn(c_msdtransformPD0tau);
                g_msdtransform->set_color(QColor("red"));
                pltDistResults->getYAxis()->set_axisLabel("\\langle r^2(\\tau)\\rangle/(P_0D_0\\tau) [s^{\\alpha-1}]");
                pltDistResults->getYAxis()->set_logAxis(true);
            } else if (cmbDistResultsMode->currentIndex()==5) {
                g_msdtransform->set_title("\\langle r^2(\\tau)\\rangle/(P_1D_1\\tau)");
                g_msdtransform->set_xColumn(c_tau);
                g_msdtransform->set_yColumn(c_msdtransformPD1tau);
                g_msdtransform->set_color(QColor("red"));
                pltDistResults->getYAxis()->set_axisLabel("\\langle r^2(\\tau)\\rangle/(P_1D_1\\tau) [s^{\\alpha-1}]");
                pltDistResults->getYAxis()->set_logAxis(true);
            } else if (cmbDistResultsMode->currentIndex()==6) {
                g_msdtransform->set_title("\\langle r^2(\\tau)\\rangle/(P_2D_2\\tau)");
                g_msdtransform->set_xColumn(c_tau);
                g_msdtransform->set_yColumn(c_msdtransformPD2tau);
                g_msdtransform->set_color(QColor("red"));
                pltDistResults->getYAxis()->set_axisLabel("\\langle r^2(\\tau)\\rangle/(P_2D_2\\tau) [s^{\\alpha-1}]");
                pltDistResults->getYAxis()->set_logAxis(true);
            } else if (cmbDistResultsMode->currentIndex()==7) {
                g_msdtransform->set_title("\\langle r^2(\\tau)\\rangle/(P_3D_3\\tau)");
                g_msdtransform->set_xColumn(c_tau);
                g_msdtransform->set_yColumn(c_msdtransformPD3tau);
                g_msdtransform->set_color(QColor("red"));
                pltDistResults->getYAxis()->set_axisLabel("\\langle r^2(\\tau)\\rangle/(P_3D_3\\tau) [s^{\\alpha-1}]");
                pltDistResults->getYAxis()->set_logAxis(true);
            } else if (cmbDistResultsMode->currentIndex()==8) {
                g_msdtransform->set_title("\\langle r^2(\\tau)\\rangle-P_0D_0\\tau");
                g_msdtransform->set_xColumn(c_tau);
                g_msdtransform->set_yColumn(c_msdtransformMinusPD0tau);
                g_msdtransform->set_color(QColor("red"));
                pltDistResults->getYAxis()->set_axisLabel("\\langle r^2(\\tau)\\rangle - P_0D_0\\tau [{\\mu}m^2]");
                pltDistResults->getYAxis()->set_logAxis(false);
            }

            g_msdtransform->set_drawLine(true);
            g_msdtransform->set_datarange_start(msd_start);
            g_msdtransform->set_datarange_end(msd_end);
            g_msdtransform->set_symbol(JKQTPnoSymbol);
            g_msdtransform->set_symbolSize(7);
            pltDistResults->addGraph(g_msdtransform);
        }

    }

    // ensure that distribution and this plot are synched on x-axis
    pltDistribution->zoomToFit();
    pltDistResults->setX(pltDistribution->getXMin(), pltDistribution->getXMax());
    // zoom y-axis
    pltDistResults->zoomToFit(false, true);
    if (updt) {
        // ensure replot, if we have to
        pltDistResults->set_doDrawing(true);
        pltDistResults->update_plot();
        //qDebug()<<"pltDistResults->update_plot(): "<<pltDistResults->get_xAxis()->getMin()<<" ... "<<pltDistResults->get_xAxis()->getMax()<<",     "<<pltDistResults->get_yAxis()->getMin()<<" ... "<<pltDistResults->get_yAxis()->getMax();
    }
    QApplication::restoreOverrideCursor();
}

void QFFCSMSDEvaluationEditor::updateDistribution() {
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!eval) return;
    QVector<double> distTau=eval->getMSDTaus(eval->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel());
    QVector<double> dist=eval->getMSD(eval->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel());
    int data_start=sliderDist->get_userMin();
    int data_end=sliderDist->get_userMax();

    JKQTPdatastore* dsdist=pltDistribution->get_plotter()->getDatastore();


    bool updt=pltDistribution->get_doDrawing();
    pltDistribution->set_doDrawing(false);
    pltDistribution->clearGraphs();

    /////////////////////////////////////////////////////////////////////////////////
    // plot distribution
    /////////////////////////////////////////////////////////////////////////////////
    int c_disttau=-1;
    int c_dist=-1;
    dsdist->deleteAllColumns("msd_tau");
    dsdist->deleteAllColumns("msd");
    if (distTau.size()>0 && dist.size()>0) {
        c_disttau=dsdist->addCopiedColumn(distTau.data(), distTau.size(), "msd_tau");
        c_dist=dsdist->addCopiedColumn(dist.data(), dist.size(), "msd");
    } else {
        pltDistribution->setXY(pltData->getXMin(), pltData->getXMax(), pltData->getYMin(), pltData->getYMax());
    }
    JKQTPxyLineGraph* g_dist=new JKQTPxyLineGraph(pltDistribution->get_plotter());
    g_dist->set_drawLine(true);
    g_dist->set_title("MSD \\langle r^2(\\tau)\\rangle");
    g_dist->set_xColumn(c_disttau);
    g_dist->set_yColumn(c_dist);
    g_dist->set_datarange_start(data_start);
    g_dist->set_datarange_end(data_end);
    pltDistribution->addGraph(g_dist);
    QColor cb("white");
    cb.setAlphaF(0.5);
    pltDistribution->get_plotter()->set_keyBackgroundColor(cb);
    pltDistribution->get_plotter()->set_showKey(chkShowKeyDist->isChecked());



    /////////////////////////////////////////////////////////////////////////////////
    // plot theory curves
    /////////////////////////////////////////////////////////////////////////////////
    for (int i=0; i<MSDTHEORYCOUNT; i++)     {
        if (chkSlope[i]->isChecked()) {
            const double D=numPre[i]->value()*numD[i]->value();
            const double a=numAlpha[i]->value();

            JKQTPxFunctionLineGraph* g_dist=new JKQTPxFunctionLineGraph(pltDistribution->get_plotter());
            g_dist->set_drawLine(true);
            g_dist->set_title(tr("MSD %1, D=%2{\\mu}m^2/s, \\alpha=%3").arg(i+1).arg(numD[i]->value()).arg(numAlpha[i]->value()));
            g_dist->setSpecialFunction(JKQTPxFunctionLineGraph::PowerLaw);
            QVector<double> vecP;
            vecP<<D<<a;
            g_dist->set_params(vecP);
            g_dist->set_lineWidth(1);
            g_dist->set_style(Qt::DashLine);

            pltDistribution->addGraph(g_dist);
        }
    }

    if (updt) {
        pltDistribution->zoomToFit();
        pltDistribution->set_doDrawing(true);
        pltDistribution->update_plot();
    }

}


void QFFCSMSDEvaluationEditor::fitWidthChanged(int width) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);

    bool rc=data->get_doEmitResultsChanged();
    bool pc=data->get_doEmitPropertiesChanged();
    data->set_doEmitResultsChanged(false);
    data->set_doEmitPropertiesChanged(false);
    if (data) data->setFitWidth(width, data->getHighlightedRecord(), data->getCurrentIndex());
    data->set_doEmitResultsChanged(rc);
    data->set_doEmitPropertiesChanged(pc);
    updateDistributionResults();
}

void QFFCSMSDEvaluationEditor::fitTypeChanged(int type)
{
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);

    bool rc=data->get_doEmitResultsChanged();
    bool pc=data->get_doEmitPropertiesChanged();
    data->set_doEmitResultsChanged(false);
    data->set_doEmitPropertiesChanged(false);
    if (data) data->setFitType(type, data->getHighlightedRecord(), data->getCurrentIndex());
    data->set_doEmitResultsChanged(rc);
    data->set_doEmitPropertiesChanged(pc);
    updateDistributionResults();
}



void QFFCSMSDEvaluationEditor::fitRangeChanged(bool range) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);

    bool rc=data->get_doEmitResultsChanged();
    bool pc=data->get_doEmitPropertiesChanged();
    data->set_doEmitResultsChanged(false);
    data->set_doEmitPropertiesChanged(false);
    if (data) data->setFitRangeLimited(range, data->getHighlightedRecord(), data->getCurrentIndex());
    data->set_doEmitResultsChanged(rc);
    data->set_doEmitPropertiesChanged(pc);
    updateDistributionResults();
}


void QFFCSMSDEvaluationEditor::weightsChanged(int weights) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    data->setFitDataWeighting(weights);
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}


void QFFCSMSDEvaluationEditor::theoryChanged() {
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!dataEventsEnabled) return;
    if (!data) return;

    bool rc=data->get_doEmitResultsChanged();
    bool pc=data->get_doEmitPropertiesChanged();
    data->set_doEmitResultsChanged(false);
    data->set_doEmitPropertiesChanged(false);
    for (int i=0; i<MSDTHEORYCOUNT; i++)     {
        data->setTheory(i, chkSlope[i]->isChecked(), numPre[i]->value(), numD[i]->value(), numAlpha[i]->value(), data->getHighlightedRecord(), data->getCurrentIndex());
        if (i>0) {
            numIntersect[i-1]->setValue(data->getTheoryIntersect(i-1, data->getHighlightedRecord(), data->getCurrentIndex()));
            numIntersect[i-1]->setEnabled(chkSlope[i-1]->isChecked()&&chkSlope[i]->isChecked());
        }
    }
    data->set_doEmitResultsChanged(rc);
    data->set_doEmitPropertiesChanged(pc);

    updateDistribution();
    updateDistributionResults();
}

void QFFCSMSDEvaluationEditor::fitMSD()
{
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!dataEventsEnabled) return;
    if (!data) return;

    // find the ID of the MSD to fit
    QAction* act=qobject_cast<QAction*>(sender());
    int btnID=-1;
    for (int i=0; i<MSDTHEORYCOUNT; i++) {
        if (act==btnFitMSD[i]->defaultAction()) {
            btnID=i;
            break;
        }
    }
    //qDebug()<<"fitMSD: "<<btnID<<act;
    if (btnID<0) return;

    QFFCSMSDEvaluationFitMSDDialog* dlg=new QFFCSMSDEvaluationFitMSDDialog(data, btnID, this);

    dlg->exec();

    for (int i=0; i<MSDTHEORYCOUNT; i++)     {
        chkSlope[i]->setChecked(data->getTheoryEnabled(i, data->getHighlightedRecord(), data->getCurrentIndex()));
        numPre[i]->setValue(data->getTheoryPre(i, data->getHighlightedRecord(), data->getCurrentIndex()));
        numD[i]->setValue(data->getTheoryD(i, data->getHighlightedRecord(), data->getCurrentIndex()));
        numAlpha[i]->setValue(data->getTheoryAlpha(i, data->getHighlightedRecord(), data->getCurrentIndex()));
        //if (i<MSDTHEORYCOUNT-1) numIntersect[i]->setValue(data->getTheoryIntersect(i, data->getHighlightedRecord(), data->getCurrentIndex()));
        if (i>0) {
            numIntersect[i-1]->setValue(data->getTheoryIntersect(i-1, data->getHighlightedRecord(), data->getCurrentIndex()));
            numIntersect[i-1]->setEnabled(chkSlope[i-1]->isChecked()&&chkSlope[i]->isChecked());
        }
    }

    delete dlg;

    updateDistribution();
}





////////////////////////////////////////////////////////////////////////////////////////////////////
// REPORT GENERATION
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
void QFFCSMSDEvaluationEditor::createReportDoc(QTextDocument* document) {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if ((!eval)||(!record))return;    // ||(!data) vielleicht noch hinzufügen


    // we use this QTextCursor to write the document
    QTextCursor cursor(document);

    // here we define some generic formats
    QTextCharFormat fText=cursor.charFormat();
    fText.setFontPointSize(8);
    QTextCharFormat fTextSmall=fText;
    fTextSmall.setFontPointSize(0.85*fText.fontPointSize());
    QTextCharFormat fTextBold=fText;
    fTextBold.setFontWeight(QFont::Bold);
    QTextCharFormat fTextBoldSmall=fTextBold;
    fTextBoldSmall.setFontPointSize(0.85*fText.fontPointSize());
    QTextCharFormat fHeading1=fText;
    QTextBlockFormat bfLeft;
    bfLeft.setAlignment(Qt::AlignLeft);
    QTextBlockFormat bfRight;
    bfRight.setAlignment(Qt::AlignRight);
    QTextBlockFormat bfCenter;
    bfCenter.setAlignment(Qt::AlignHCenter);

    fHeading1.setFontPointSize(2*fText.fontPointSize());
    fHeading1.setFontWeight(QFont::Bold);


    // insert heading
    cursor.insertText(tr("Evaluation Report:\n\n"), fHeading1);
    cursor.movePosition(QTextCursor::End);

    // insert table with some data
    QTextTableFormat tableFormat;
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QTextTable* table = cursor.insertTable(2, 2, tableFormat);
    table->cellAt(0, 0).firstCursorPosition().insertText(tr("raw data:"), fTextBold);
    table->cellAt(0, 1).firstCursorPosition().insertText(record->getName(), fText);
    table->cellAt(1, 0).firstCursorPosition().insertText(tr("ID:"), fTextBold);
    table->cellAt(1, 1).firstCursorPosition().insertText(QString::number(record->getID()));
    cursor.movePosition(QTextCursor::End);

}


*/







void QFFCSMSDEvaluationEditor::createReportDoc(QTextDocument* document) {
    // make sure all widgets ahave the right size
    {   int trci=tabResidulas->currentIndex();
        for (int i=0;i<tabResidulas->count(); i++) {
            tabResidulas->setCurrentIndex(i);
        }
        tabResidulas->setCurrentIndex(trci);
    }

    int PicTextFormat=QTextFormat::UserObject + 1;
    QObject *picInterface = new QPictureTextObject;
    document->documentLayout()->registerHandler(PicTextFormat, picInterface);



    if (!current) return;
    if (!cmbModel) return; ////////////
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(record);

    ////////////////////////////////
    //QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    ////////////////////////////////

    ///////////
    if (!record) return;
    //////////
    if (!eval) return;
    if (!fcs) return;

    /*
    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* algorithm=eval->getFitAlgorithm();
    //int run=eval->getCurrentIndex();
    double* params=eval->allocFillParameters();
    */

    QTextCursor cursor(document);
    QTextCharFormat fText=cursor.charFormat();
    fText.setFontPointSize(8);
    QTextCharFormat fTextSmall=fText;
    fTextSmall.setFontPointSize(0.85*fText.fontPointSize());
    QTextCharFormat fTextBold=fText;
    fTextBold.setFontWeight(QFont::Bold);
    QTextCharFormat fTextBoldSmall=fTextBold;
    fTextBoldSmall.setFontPointSize(0.85*fText.fontPointSize());
    QTextCharFormat fHeading1=fText;
    QTextBlockFormat bfLeft;
    bfLeft.setAlignment(Qt::AlignLeft);
    QTextBlockFormat bfLeftNB=bfLeft;
    bfLeftNB.setNonBreakableLines(true);
    QTextBlockFormat bfRight;
    bfRight.setAlignment(Qt::AlignRight);
    QTextBlockFormat bfRightNB=bfRight;
    bfRightNB.setNonBreakableLines(true);
    QTextBlockFormat bfCenter;
    bfCenter.setAlignment(Qt::AlignHCenter);
    QTextBlockFormat bfCenterNB=bfCenter;
    bfCenterNB.setNonBreakableLines(true);

    fHeading1.setFontPointSize(2*fText.fontPointSize());
    fHeading1.setFontWeight(QFont::Bold);
    cursor.insertText(tr("FCS Mean Squared Displacement Fit Report:\n\n"), fHeading1);
    cursor.movePosition(QTextCursor::End);



    QTextTableFormat tableFormat;
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QTextTable* table = cursor.insertTable(3, 4, tableFormat);
    table->cellAt(0, 0).firstCursorPosition().insertText(tr("file:"), fTextBold);
    table->cellAt(0, 1).firstCursorPosition().insertText(record->getName(), fText);
    table->cellAt(0, 2).firstCursorPosition().insertText(tr("run:"), fTextBold);
    table->cellAt(0, 3).firstCursorPosition().insertText(QString("%1 [%2]").arg((eval->getCurrentIndex()<0)?tr("average"):QString::number(eval->getCurrentIndex())).arg(fcs->getCorrelationRunName(eval->getCurrentIndex())), fText);
    //table->cellAt(1, 0).firstCursorPosition().insertText(tr("fit algorithm:"), fTextBold);
    //table->cellAt(1, 1).firstCursorPosition().insertText(algorithm->name(), fText);
    table->cellAt(1, 0).firstCursorPosition().insertText(tr("data range:"), fTextBold);
    table->cellAt(1, 1).firstCursorPosition().insertText(tr("%1 ... %2 / %3 ... %4").arg(datacut->get_userMin()).arg(datacut->get_userMax()).arg(datacut->get_min()).arg(datacut->get_max()), fText);
    table->cellAt(2, 0).firstCursorPosition().insertText(tr("physical model: "), fTextBold);
    //table->cellAt(2, 1).firstCursorPosition().insertText(ffunc->name(), fText);
    table->cellAt(2, 1).firstCursorPosition().insertText(eval->getModelName(eval->getCurrentModel()), fText);
    table->cellAt(2, 2).firstCursorPosition().insertText(tr("data weighting:"), fTextBold);
    table->cellAt(2, 3).firstCursorPosition().insertText(cmbWeights->currentText(), fText);
    cursor.movePosition(QTextCursor::End);



    cursor.insertBlock(); cursor.insertBlock();
    cursor.insertText(tr("Plots:\n"), fTextBold);
    QTextTableFormat tableFormat1;
    tableFormat1.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat1.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QVector<QTextLength> constraints;
    constraints << QTextLength(QTextLength::PercentageLength, 66);
    constraints << QTextLength(QTextLength::PercentageLength, 34);
    tableFormat1.setColumnWidthConstraints(constraints);

    table = cursor.insertTable(4, 2, tableFormat1);
    table->mergeCells(0,0,2,1);
    table->mergeCells(2,1,2,1);
    {
        QTextCursor tabCursor=table->cellAt(0, 0).firstCursorPosition();
        QPicture pic;
        JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
        pltData->get_plotter()->draw(*painter, QRect(0,0,pltData->width(),pltData->width()));
        delete painter;
        double scale=document->textWidth()*0.4/pic.boundingRect().width(); //0.62
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("correlation data, model and residuals:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    QApplication::processEvents();

    {
        QTextCursor tabCursor=table->cellAt(0, 1).firstCursorPosition();
        QPicture pic;
        JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
        pltDistribution->get_plotter()->draw(*painter, QRect(0,0,pltDistribution->width(),pltDistribution->width()));
        delete painter;
        double scale=document->textWidth()*0.4/pic.boundingRect().width(); //0.62
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("Mean Squared Displacement (MSD):\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    QApplication::processEvents();
    int old_mode=cmbDistResultsMode->currentIndex();
    {
        QTextCursor tabCursor=table->cellAt(1, 1).firstCursorPosition();
        QPicture pic;
        JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
        cmbDistResultsMode->setCurrentIndex(0);
        QApplication::processEvents();
        QApplication::processEvents();
        pltDistResults->get_plotter()->draw(*painter, QRect(0,0,pltDistribution->width(),pltDistribution->width()));
        delete painter;
        double scale=document->textWidth()*0.4/pic.boundingRect().width(); //0.62
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("local anomality:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    QApplication::processEvents();

    {
        QTextCursor tabCursor=table->cellAt(2, 1).firstCursorPosition();
        QPicture pic;
        JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
        cmbDistResultsMode->setCurrentIndex(1);
        QApplication::processEvents();
        QApplication::processEvents();
        pltDistResults->get_plotter()->draw(*painter, QRect(0,0,pltDistribution->width(),pltDistribution->width()));
        delete painter;
        double scale=document->textWidth()*0.4/pic.boundingRect().width(); //0.62
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("local diffusion coefficient:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    cmbDistResultsMode->setCurrentIndex(old_mode);
    QApplication::processEvents();


    {
        QTextCursor tabCursor=table->cellAt(2, 0).firstCursorPosition();
        QPicture pic;
        JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
        pltResidualHistogram->get_plotter()->draw(*painter, QRect(0,0,pltResidualHistogram->width(),pltResidualHistogram->width()));
        delete painter;
        double scale=document->textWidth()*0.4/pic.boundingRect().width();//0.3
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("residual histogram:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    QApplication::processEvents();
    {
        QTextCursor tabCursor=table->cellAt(3,0).firstCursorPosition();
        QPicture pic;
        JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
        pltResidualCorrelation->get_plotter()->draw(*painter, QRect(0,0,pltResidualCorrelation->width(),pltResidualCorrelation->width()));
        delete painter;
        double scale=document->textWidth()*0.4/pic.boundingRect().width();//0.3
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("residual correlations:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }


    cursor.movePosition(QTextCursor::End);
    QApplication::processEvents();

    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////


    /*
    qDebug() << "WRITE DOCUMENT DEBUG: ";
    qDebug() << eval->getCurrentModel();
    qDebug() << eval->getParameterCount(eval->getCurrentModel());
    */


    int fitParamCount=0;
    /*
    for (int i=0; i<eval->getParameterCount(eval->getCurrentModel()); i++){ // ffunc->paramCount(); i++) {

        //if (ffunc->isParameterVisible(i, params)) fitParamCount++;
    }
    */
   fitParamCount=eval->getParameterCount(eval->getCurrentModel());


    QApplication::processEvents();

    cursor.insertBlock(); cursor.insertBlock();

    /*
    if (eval->hasFit()) cursor.insertText(tr("Model Parameters (fit results):"), fTextBold);
    else cursor.insertText(tr("Model Parameters (initial values):"), fTextBold);
    */
    cursor.insertText(tr("Model Parameters: "),fTextBold);

    QTextTableFormat tableFormat2;
    tableFormat2.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat2.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    table = cursor.insertTable(qMax(6,2+eval->getParameterCount(eval->getCurrentModel())), 4, tableFormat2);
    //table = cursor.insertTable(ceil((double)fitParamCount/2.0)+1, 12, tableFormat2);
    QTextCursor tableCursor;
    QApplication::processEvents();


    tableCursor=table->cellAt(0, 0).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(tr("Evaluation Parameters"), fTextBoldSmall);

    tableCursor=table->cellAt(0, 2).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(labFitParameters->text(), fTextBoldSmall);

    //if (algorithm->get_supportsBoxConstraints()) table->cellAt(0, 5).firstCursorPosition().insertText(tr("Range"), fTextBoldSmall);
    QApplication::processEvents();
/*
    tableCursor=table->cellAt(0, 6).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(tr("Parameter"), fTextBoldSmall);
    tableCursor=table->cellAt(0, 8).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(tr("Value"), fTextBoldSmall);
    //if (algorithm->get_supportsBoxConstraints()) table->cellAt(0, 11).firstCursorPosition().insertText(tr("Range"), fTextBoldSmall);
    QApplication::processEvents();
*/

    tableCursor=table->cellAt(1,0).firstCursorPosition();
    tableCursor.setBlockFormat(bfRightNB);
    tableCursor.setBlockCharFormat(fTextSmall);
    tableCursor.insertText(tr(" run =  "), fTextSmall);

    tableCursor=table->cellAt(2,0).firstCursorPosition();
    tableCursor.setBlockFormat(bfRightNB);
    tableCursor.setBlockCharFormat(fTextSmall);
    tableCursor.insertText(tr(" fit width =  "), fTextSmall);

    int k=0;
    for (int i=0; i<3; i++) {
        if (chkSlope[i]->isChecked()) {
            tableCursor=table->cellAt(3+k, 0).firstCursorPosition();
            tableCursor.setBlockFormat(bfRightNB);
            tableCursor.setBlockCharFormat(fTextSmall);
            tableCursor.insertText(tr(" theory %1 =  ").arg(i+1), fTextSmall);

            tableCursor=table->cellAt(3+k, 1).firstCursorPosition();
            tableCursor.setBlockFormat(bfCenterNB);
            tableCursor.setBlockCharFormat(fTextSmall);
            tableCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<font size=\"-1\"> %1 &middot; %2 &middot; &tau;<sup>%3</sup> [&mu;m<sup>2</sup>/s]</font>").arg(numPre[i]->value()).arg(numD[i]->value()).arg(numAlpha[i]->value())));

            k++;
        }
    }

    k=0;
    for (int i=0; i<eval->getParameterCount(eval->getCurrentModel()); i++) {
        tableCursor=table->cellAt(1+k, 2).firstCursorPosition();
        tableCursor.setBlockFormat(bfRightNB);
        tableCursor.setBlockCharFormat(fTextSmall);
        tableCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<font size=\"-1\"> %1 =  </font>").arg(eval->getParameterName(eval->getCurrentModel(), i, true))));

        tableCursor=table->cellAt(1+k, 3).firstCursorPosition();
        tableCursor.setBlockFormat(bfCenterNB);
        tableCursor.setBlockCharFormat(fTextSmall);
        tableCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<font size=\"-1\"> %1</font>").arg(eval->getFitValue(eval->getParameterID(eval->getCurrentModel(), i)))));
        k++;
    }


    tableCursor=table->cellAt(1,1).firstCursorPosition();
    tableCursor.setBlockFormat(bfCenterNB);
    tableCursor.setBlockCharFormat(fTextSmall);
    tableCursor.insertText(QString("%1 [%2]").arg((eval->getCurrentIndex()<0)?tr("average"):QString::number(eval->getCurrentIndex())).arg(fcs->getCorrelationRunName(eval->getCurrentIndex())), fTextSmall);


    cursor.setBlockFormat(bfLeft);
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock(); cursor.insertBlock();
    QString htmlBot=fitStatisticsReport;
    htmlBot.replace("width=\"95%\"", "");
    cursor.insertFragment(QTextDocumentFragment::fromHtml(htmlBot));


}








