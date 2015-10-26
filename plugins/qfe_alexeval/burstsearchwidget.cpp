/*
Copyright (c) 2012-2015 by Sebastian Isbaner

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

#include "burstsearchwidget.h"
#include "ui_burstsearchwidget.h"
#include "BurstSearch.h"
#include "compareburstsearch.h"
#include <QMessageBox>
#include <QFileDialog>
#include"analysis.h"

BurstSearchWidget::BurstSearchWidget(QWidget *parent, AlexData *data) :
    QWidget(parent),
    ui(new Ui::BurstSearchWidget)
{
    ui->setupUi(this);
    if(data==NULL) qFatal("data not initialized");
    ad=data;

    connect(ui->pushButtonStart,SIGNAL(clicked()),this,SLOT(runBurstSearch()));

    ui->lineEditIni->addButton(edt=new JKStyledButton(JKStyledButton::SelectFile, ui->lineEditIni, ui->lineEditIni));
    edt->setFilter(tr("AlexEval ini files(*.ini);;All Files (*.*)"));
    edt->setInitPath(ad->currentDir);
    ui->groupBoxMoreOptions->setVisible(false);
    ui->frame->setVisible(false);

    connect(ui->lineEditIni,SIGNAL(editingFinished()),SLOT(loadIni()));
//    connect(ui->comboBoxMethod,SIGNAL(currentIndexChanged(int)),this,SLOT(setMethodSingle(int)));
    connect(ui->comboBoxMethod,SIGNAL(currentIndexChanged(int)),this,SLOT(setMethod()));
    connect(ui->comboBoxMethodDual,SIGNAL(currentIndexChanged(int)),this,SLOT(setMethodDual(int)));
    ui->comboBoxMethodDual->addItem("DCBS Extended");
//    JKQtPlotter* plot = NULL;
    setParameters();
}

BurstSearchWidget::~BurstSearchWidget()
{
    delete ui;
}

void BurstSearchWidget::storeSettings(QSettings &settings, const QString &prefix) const
{
    settings.setValue(prefix+"comboBoxMethod",ui->comboBoxMethod->currentIndex());
    settings.setValue(prefix+"comboBoxMethodDual",ui->comboBoxMethodDual->currentIndex());

    settings.setValue(prefix+"spinBoxMaxRateGlobal",ui->spinBoxMaxRateGlobal->value());
    settings.setValue(prefix+"spinBoxMinRateGlobal",ui->spinBoxMinRateGlobal->value());
    settings.setValue(prefix+"spinBoxMinSizeGlobal",ui->spinBoxMinSizeGlobal->value());

    // channel 1 parameters
    settings.setValue(prefix+"comboBoxChannel1",ui->comboBoxChannel1->currentIndex());
    settings.setValue(prefix+"doubleSpinBox_IPT",ui->doubleSpinBox_IPT->value());
    settings.setValue(prefix+"doubleSpinBox_LeeVar",ui->doubleSpinBox_LeeVar->value());
    settings.setValue(prefix+"doubleSpinBox_T",ui->doubleSpinBox_T->value());
    settings.setValue(prefix+"spinBox_L",ui->spinBox_L->value());
    settings.setValue(prefix+"spinBox_LeeWin",ui->spinBox_LeeWin->value());
    settings.setValue(prefix+"spinBox_M",ui->spinBox_M->value());

    // channel 2 parameters
    settings.setValue(prefix+"comboBoxChannel2",ui->comboBoxChannel2->currentIndex());
    settings.setValue(prefix+"doubleSpinBox_IPT_2",ui->doubleSpinBox_IPT_2->value());
    settings.setValue(prefix+"doubleSpinBox_LeeVar_2",ui->doubleSpinBox_LeeVar_2->value());
    settings.setValue(prefix+"doubleSpinBox_T_2",ui->doubleSpinBox_T_2->value());
    settings.setValue(prefix+"spinBox_L_2",ui->spinBox_L_2->value());
    settings.setValue(prefix+"spinBox_LeeWin_2",ui->spinBox_LeeWin_2->value());
    settings.setValue(prefix+"spinBox_M_2",ui->spinBox_M_2->value());

    settings.setValue(prefix+"iniFile",ui->lineEditIni->text());
}

void BurstSearchWidget::loadSettings(QSettings &settings, const QString &prefix)
{
    ui->comboBoxMethod->setCurrentIndex(settings.value(prefix+"comboBoxMethod",ui->comboBoxMethod->currentIndex()).toInt());
    setMethodSingle(ui->comboBoxMethod->currentIndex());
    ui->comboBoxMethodDual->setCurrentIndex(settings.value(prefix+"comboBoxMethodDual",ui->comboBoxMethodDual->currentIndex()).toInt());    
    setMethodDual(ui->comboBoxMethodDual->currentIndex());

    ui->spinBoxMaxRateGlobal->setValue(settings.value(prefix+"spinBoxMaxRateGlobal",ui->spinBoxMaxRateGlobal->value()).toInt());
    ui->spinBoxMinRateGlobal->setValue(settings.value(prefix+"spinBoxMinRateGlobal",ui->spinBoxMinRateGlobal->value()).toInt());
    ui->spinBoxMinSizeGlobal->setValue(settings.value(prefix+"spinBoxMinSizeGlobal",ui->spinBoxMinSizeGlobal->value()).toInt());

    ui->comboBoxChannel1->setCurrentIndex(settings.value(prefix+"comboBoxChannel1",ui->comboBoxChannel1->currentIndex()).toInt());
    ui->doubleSpinBox_IPT->setValue(settings.value(prefix+"doubleSpinBox_IPT",ui->doubleSpinBox_IPT->value()).toDouble());
    ui->doubleSpinBox_LeeVar->setValue(settings.value(prefix+"doubleSpinBox_LeeVar",ui->doubleSpinBox_LeeVar->value()).toDouble());
    ui->doubleSpinBox_T->setValue(settings.value(prefix+"doubleSpinBox_T",ui->doubleSpinBox_T->value()).toDouble());
    ui->spinBox_L->setValue(settings.value(prefix+"spinBox_L",ui->spinBox_L->value()).toInt());
    ui->spinBox_LeeWin->setValue(settings.value(prefix+"spinBox_LeeWin",ui->spinBox_LeeWin->value()).toInt());
    ui->spinBox_M->setValue(settings.value(prefix+"spinBox_M",ui->spinBox_M->value()).toInt());

    ui->comboBoxChannel2->setCurrentIndex(settings.value(prefix+"comboBoxChannel2",ui->comboBoxChannel2->currentIndex()).toInt());
    ui->doubleSpinBox_IPT_2->setValue(settings.value(prefix+"doubleSpinBox_IPT_2",ui->doubleSpinBox_IPT->value()).toDouble());
    ui->doubleSpinBox_LeeVar_2->setValue(settings.value(prefix+"doubleSpinBox_LeeVar_2",ui->doubleSpinBox_LeeVar->value()).toDouble());
    ui->doubleSpinBox_T_2->setValue(settings.value(prefix+"doubleSpinBox_T_2",ui->doubleSpinBox_T->value()).toDouble());
    ui->spinBox_L_2->setValue(settings.value(prefix+"spinBox_L_2",ui->spinBox_L->value()).toInt());
    ui->spinBox_LeeWin_2->setValue(settings.value(prefix+"spinBox_LeeWin_2",ui->spinBox_LeeWin->value()).toInt());
    ui->spinBox_M_2->setValue(settings.value(prefix+"spinBox_M_2",ui->spinBox_M->value()).toInt());

    ui->lineEditIni->setText(settings.value(prefix+"iniFile",ad->iniFilename).toString());
}

void BurstSearchWidget::automatedRun()
{
//    loadIni();
    runBurstSearch();
}

void BurstSearchWidget::setMethodSingle(int index)
{
    switch(index) {
    case 0:
        ad->setBSMethodSingle(BurstSearchParams_t::Sliding);
        ui->frameLee->setDisabled(true);
        ui->frameSliding->setDisabled(false);
//        ui->spinBox_M_2->setDisabled(false);
//        ui->spinBox_M->setDisabled(false);
    break;
    case 1:
        ad->setBSMethodSingle(BurstSearchParams_t::LeeFilter);
        ui->frameLee->setDisabled(false);
        ui->frameSliding->setDisabled(true);
    break;
    case 2:
        ad->setBSMethodSingle(BurstSearchParams_t::Binned);
        ui->frameLee->setDisabled(true);
        ui->frameSliding->setDisabled(false);
    default:
        qWarning("unknown burst search method");
    break;
    }
}

//void BurstSearchWidget::setMethodDual(int index)
//{
//    switch(index) {
//    case 0:
//        ad->setBSMethodDual(BurstSearchParams_t::AllPhotons);
//        ui->doubleSpinBox_T_2->setDisabled(true);
//        ui->spinBox_L_2->setDisabled(true);
//        ui->spinBox_M_2->setDisabled(true);
//        ui->comboBoxChannel2->setDisabled(true);
//    break;
//    case 1:
//        ad->setBSMethodDual(BurstSearchParams_t::DualChannel);
//        ui->doubleSpinBox_T_2->setDisabled(false);
//        ui->spinBox_L_2->setDisabled(false);
//        ui->comboBoxChannel2->setDisabled(false);
//        if(ui->comboBoxMethod->currentIndex()==2) ;
//        else ui->spinBox_M_2->setDisabled(false);
//    break;
//    case 2:
//        ad->setBSMethodDual(BurstSearchParams_t::CombineBurstsOR);
//        ui->doubleSpinBox_T_2->setDisabled(false);
//        ui->spinBox_L_2->setDisabled(false);
//        ui->comboBoxChannel2->setDisabled(false);
//        if(ui->comboBoxMethod->currentIndex()==2) ;
//        else ui->spinBox_M_2->setDisabled(false);
//    break;
//    case 3:
//        ad->setBSMethodDual(BurstSearchParams_t::DualChannelExt);
//        ui->doubleSpinBox_T_2->setDisabled(false);
//        ui->spinBox_L_2->setDisabled(false);
//        ui->comboBoxChannel2->setDisabled(false);
//        if(ui->comboBoxMethod->currentIndex()==2) ;
//        else ui->spinBox_M_2->setDisabled(false);
//    default:
//        qWarning("unknown burst search method");
//    break;
//    }
//    qDebug("BurstSearchWidget:setMethodDual set BS method to %i", index);
//}

void BurstSearchWidget::setMethod()
{
    switch(ui->comboBoxMethodDual->currentIndex()) {
    case 0:
        ad->setBSMethodDual(BurstSearchParams_t::AllPhotons);
        setDisabledMethodLowerFrame(true);

    break;
    case 1:
        ad->setBSMethodDual(BurstSearchParams_t::DualChannel);
        setDisabledMethodLowerFrame(false);
    break;
    case 2:
        ad->setBSMethodDual(BurstSearchParams_t::CombineBurstsOR);
        setDisabledMethodLowerFrame(false);
    break;
    case 3:
        ad->setBSMethodDual(BurstSearchParams_t::DualChannelExt);
        setDisabledMethodLowerFrame(false);
    default:
        qWarning("unknown burst search method");
    break;
    }
    setMethodSingle(ui->comboBoxMethod->currentIndex());
    qDebug("BurstSearchWidget:setMethodDual set BS method to %i", ui->comboBoxMethodDual->currentIndex());
}

void BurstSearchWidget::setDisabledMethodLowerFrame(bool toggle)
{
    ui->doubleSpinBox_IPT_2->setDisabled(toggle);
    ui->doubleSpinBox_LeeVar_2->setDisabled(toggle);
    ui->spinBox_LeeWin_2->setDisabled(toggle);

    ui->spinBox_M_2->setDisabled(toggle);
    ui->doubleSpinBox_T_2->setDisabled(toggle);
    ui->spinBox_L_2->setDisabled(toggle);

    ui->comboBoxChannel2->setDisabled(toggle);
}

void BurstSearchWidget::runBurstSearch()
{
    qDebug("BurstSearchWidget::runBurstSearch");
    if(ad->isEmpty()) {qWarning("no TTTR data"); return;}

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    getParameters();

    int result=BurstSearch(ad);

    if(result) qWarning("runBurstSearch failed");
    else {
        if(ui->checkBoxSaveSingle->isChecked()) {
            SaveBursts(ad->burstVectorCh1,ad->burstSearchParamsCh1,"./BurstsCh1.txt");
            SaveBursts(ad->burstVectorCh2,ad->burstSearchParamsCh2,"./BurstsCh2.txt");
        }
        if(ui->checkBoxSaveDual->isChecked()) {
            SaveBurstsDual(ad->burstVectorDual,"./BurstsDual.txt");
        }
    }
    ui->textBrowserResult->setText("Results:\n"
                           "#ch1 bursts = " + QString::number(ad->burstVectorCh1.size()) + "\n"
                           "#ch2 bursts = " + QString::number(ad->burstVectorCh2.size()) + "\n"
                           "#dual bursts = " + QString::number(ad->burstVectorDual.size()) + "\n"
                           "\naverage values:\n"
                           "burstrate = " + QString::number(ad->meanBurstrate()*1e-3,'f',2) + "kHz\n"
                           "burst duration = " + QString::number(ad->meanBurstDuration()*1e3,'g',3) + "ms\n"
                           "number of photons = " + QString::number(ad->meanNoOfPhotons(),'g',4)  +"\n"
                           "bursts per second = " + QString::number(ad->burstVectorDual.size()/ad->measurementDuration(),'g',2) + "\n"
                           "bursts per minute = " + QString::number(ad->burstVectorDual.size()/ad->measurementDuration()/60,'g',2) + ""
                                   );


    QApplication::restoreOverrideCursor();
}

void BurstSearchWidget::loadIni()
{
    if(ui->lineEditIni->text().isEmpty()) return;

    ad->iniFilename=ui->lineEditIni->text();
    AlexEvalLog::text("loadIni: ini filename=" + ad->iniFilename);
    if(ad->init()) {
        QMessageBox::warning(this,"Error","Error reading ini file "+ad->iniFilename,QMessageBox::Close);
        ad->iniFilename="";
        ad->init();
    }
    setParameters();
}



void BurstSearchWidget::getParameters()
{
    // global parameters
    ad->burstSearchParamsGlobal.MinBurstSize=ui->spinBoxMinSizeGlobal->value();
    ad->burstSearchParamsGlobal.MinBurstrate=ui->spinBoxMinRateGlobal->value()*1e3;
    ad->burstSearchParamsGlobal.MaxBurstrate=ui->spinBoxMaxRateGlobal->value()*1e3;
    // other global parameters from channel 1
    ad->burstSearchParamsGlobal.channel=ui->comboBoxChannel1->channel();
    ad->burstSearchParamsGlobal.M=ui->spinBox_M->value();
    ad->burstSearchParamsGlobal.T=ui->doubleSpinBox_T->value()*1e-6; // convert from micro seconds to seconds
    ad->burstSearchParamsGlobal.LeeVar=ui->doubleSpinBox_LeeVar->value()*1e-6; // convert from ms^2 to s^2
    ad->burstSearchParamsGlobal.LeeWinSize=ui->spinBox_LeeWin->value();
    ad->burstSearchParamsGlobal.MaxIPT=ui->doubleSpinBox_IPT->value()*1e-6; // convert from us to seconds

    // ch1  parameters
    ad->burstSearchParamsCh1.channel=ui->comboBoxChannel1->channel();
    ad->burstSearchParamsCh1.M=ui->spinBox_M->value();
    ad->burstSearchParamsCh1.MinBurstSize=ui->spinBox_L->value();
    ad->burstSearchParamsCh1.T=ui->doubleSpinBox_T->value()*1e-6; // convert from micro seconds to seconds
    ad->burstSearchParamsCh1.LeeVar=ui->doubleSpinBox_LeeVar->value()*1e-6; // convert from ms^2 to s^2
    ad->burstSearchParamsCh1.LeeWinSize=ui->spinBox_LeeWin->value();
    ad->burstSearchParamsCh1.MaxIPT=ui->doubleSpinBox_IPT->value()*1e-6;  // convert from us to seconds

    // ch2 parameters
    ad->burstSearchParamsCh2.channel=ui->comboBoxChannel2->channel();
    ad->burstSearchParamsCh2.M=ui->spinBox_M_2->value();
    ad->burstSearchParamsCh2.MinBurstSize=ui->spinBox_L_2->value();
    ad->burstSearchParamsCh2.T=ui->doubleSpinBox_T_2->value()*1e-6; // convert from micro seconds to seconds
    ad->burstSearchParamsCh2.LeeVar=ui->doubleSpinBox_LeeVar_2->value()*1e-6; // convert from ms^2 to s^2
    ad->burstSearchParamsCh2.LeeWinSize=ui->spinBox_LeeWin_2->value();
    ad->burstSearchParamsCh2.MaxIPT=ui->doubleSpinBox_IPT_2->value()*1e-6; // convert from us to seconds

    ui->textBrowserParams->setText(ad->printParamsBurstSearch());
}

void BurstSearchWidget::setParameters()
{
    // global parameters (for now taken from donor parameters)
//    ui->doubleSpinBox_T->setValue(ad->burstSearchParams.T*1e6); // convert to us
//    ui->spinBox_L->setValue(ad->burstSearchParams.MinBurstSize);
//    ui->spinBox_M->setValue(ad->burstSearchParams.M);
//    ui->doubleSpinBox_IPT->setValue(ad->burstSearchParams.MaxIPT*1e6); // convert to us
//    ui->doubleSpinBox_LeeVar->setValue(ad->burstSearchParams.LeeVar*1e6); // convert to ms^2
//    ui->spinBox_LeeWin->setValue(ad->burstSearchParams.LeeWinSize);

    ui->spinBoxMaxRateGlobal->setValue(ad->burstSearchParamsGlobal.MaxBurstrate*1e-3);
    ui->spinBoxMinRateGlobal->setValue(ad->burstSearchParamsGlobal.MinBurstrate*1e-3);
    ui->spinBoxMinSizeGlobal->setValue(ad->burstSearchParamsGlobal.MinBurstSize);

    // ch1 parameters
    ui->comboBoxChannel1->setCurrentIndex(ad->burstSearchParamsCh1.channel);
    ui->doubleSpinBox_T->setValue(ad->burstSearchParamsCh1.T*1e6); // convert to us
    ui->spinBox_L->setValue(ad->burstSearchParamsCh1.MinBurstSize);
    ui->spinBox_M->setValue(ad->burstSearchParamsCh1.M);
    ui->doubleSpinBox_IPT->setValue(ad->burstSearchParamsCh1.MaxIPT*1e6); // convert to us
    ui->doubleSpinBox_LeeVar->setValue(ad->burstSearchParamsCh1.LeeVar*1e6); // convert to ms^2
    ui->spinBox_LeeWin->setValue(ad->burstSearchParamsCh1.LeeWinSize);

    // ch2 parameters
    ui->comboBoxChannel2->setCurrentIndex(ad->burstSearchParamsCh2.channel);
    ui->doubleSpinBox_T_2->setValue(ad->burstSearchParamsCh2.T*1e6); // convert to us
    ui->spinBox_L_2->setValue(ad->burstSearchParamsCh2.MinBurstSize);
    ui->spinBox_M_2->setValue(ad->burstSearchParamsCh2.M);
    ui->doubleSpinBox_IPT_2->setValue(ad->burstSearchParamsCh2.MaxIPT*1e6); // convert to us
    ui->doubleSpinBox_LeeVar_2->setValue(ad->burstSearchParamsCh2.LeeVar*1e6); // convert to ms^2
    ui->spinBox_LeeWin_2->setValue(ad->burstSearchParamsCh2.LeeWinSize);

    ui->textBrowserParams->setText(ad->printParamsBurstSearch());
}

void BurstSearchWidget::on_pushButton_clicked()
{
    qDebug("pushButton clicked");
    getParameters();
    compareBurstSearch(ad);
}

void BurstSearchWidget::on_pushButtonSaveIni_clicked()
{
    QString filename =  QFileDialog::getSaveFileName(this, tr("Save File"),
                        ad->currentDir, tr("ini files (*.ini);;Text files (*.asc *.txt)"
                        ";;All Files (*.*)"));
    if(!filename.isEmpty()) ad->saveIni(filename);
}

void BurstSearchWidget::on_pushButtonBurstView_clicked()
{
    qDebug("on_pushButtonBurstView_clicked");
    if(ad->isEmpty()) return;
    JKQtPlotter* plot = new JKQtPlotter(true,NULL);
    getParameters();

    double binwidth=5e-6, alternationPeriod=ad->getExcitationPeriodDonor();
    int nbins=(int)(alternationPeriod/binwidth+1);
    gsl_histogram * histCh1 = gsl_histogram_alloc (nbins);
    gsl_histogram * histCh2 = gsl_histogram_alloc (nbins);
    gsl_histogram_set_ranges_uniform (histCh1, 0, alternationPeriod*1.1);
    gsl_histogram_set_ranges_uniform (histCh2, 0, alternationPeriod*1.1);
    int last=0;
    int lastExc=0;
    while(ad->photons.at(last).time<ad->markerTimeDexc.at(lastExc)) ++last;
    uint ch1=ui->comboBoxChannel1->channel();
    uint ch2=ui->comboBoxChannel2->channel();
    while((lastExc<1e4)&&(lastExc<ad->markerTimeDexc.size()+1)) { // average first 10000 periods

        while ((last<ad->photons.size()) && ad->photons[last].time<ad->markerTimeDexc.at(lastExc+1)) {
//            qDebug()<<QString::number(ad->photons[last].flags,2)+"\t"<<ch1<<ch2<<"\t"<<ad->photons[last].isPhotonFlag(ch1)<<ad->photons[last].isPhotonFlag(ch2);
#ifdef PHOTONMACRO
            if(isPhotonFlag(ad->photons[last],ch1)) gsl_histogram_increment (histCh1, ad->photons[last].time-ad->markerTimeDexc[lastExc]);
            if(isPhotonFlag(ad->photons[last],ch2)) gsl_histogram_increment (histCh2, ad->photons[last].time-ad->markerTimeDexc[lastExc]);
#else
            if(ad->photons[last].isPhotonFlag(ch1)) gsl_histogram_increment (histCh1, ad->photons[last].time-ad->markerTimeDexc[lastExc]);
            if(ad->photons[last].isPhotonFlag(ch2)) gsl_histogram_increment (histCh2, ad->photons[last].time-ad->markerTimeDexc[lastExc]);
#endif
            last++;
        }
        // end of excitation period
        lastExc++;
    }
    JKQTPdatastore* ds=plot->get_plotter()->getDatastore();
    ds->clear();
    plot->get_plotter()->clearGraphs(true);
    plot->get_plotter()->setGrid(false);

    qDebug("plotHist DA");
    unsigned long long nrows=(unsigned long long)histCh1->n;
    for(size_t i=0;i<histCh1->n;++i) histCh1->range[i] *= 1e6;
    plot->get_xAxis()->set_axisLabel("time in us");
    plot->get_yAxis()->set_axisLabel("countrate in Hz");
    gsl_histogram_scale(histCh1,(1.0)/(lastExc*binwidth)); // convert bins to countrate in Hz (counts averaged over lastDex excitation periods in bin of width binwidth)
    gsl_histogram_scale(histCh2,(1.0)/(lastExc*binwidth));

    size_t xColumn=ds->addCopiedColumn(histCh1->range, nrows, "time"); // adds column (copies values!) and returns column ID
    QVector<size_t> yColumns;
    yColumns.push_back(ds->addCopiedColumn(histCh1->bin, nrows, "Channel 1"));
    yColumns.push_back(ds->addCopiedColumn(histCh2->bin, nrows, "Channel 2"));
    QList<QColor> colors;
    colors.append(QColor(COUNTRATE_COLOR_CH1));
    colors.append(QColor(COUNTRATE_COLOR_CH2));
    gsl_histogram_free(histCh1);
    gsl_histogram_free(histCh2);

    // plot
    double width=0.5;
    double s=-0.5+width/2.0;
    for (int i=0; i<yColumns.size(); ++i) {
        JKQTPbarHorizontalGraph* g=new JKQTPbarHorizontalGraph(plot->get_plotter());
        g->set_xColumn(xColumn);
        g->set_yColumn(yColumns[i]);
        g->set_shift(s);
        g->set_width(width);
        g->set_style(Qt::NoPen);
        g->set_fillColor(colors.at(i));
        s=s+width;
        plot->addGraph(g);
    }
    plot->get_plotter()->set_keyPosition(JKQTPkeyInsideTopRight); // set legend position
    plot->get_plotter()->set_showKey(true);
    plot->zoomToFit();

    plot->show();

    // old:
//    if(ad->burstVectorDual.isEmpty())
//        runBurstSearch();
//    analysisFRET(ad->burstVectorDual,ad->FRETparams);

//    BurstView *burstView= new BurstView(this, ad);
//    burstView->exec();
}


