#include "statistics_tools.h"
#include "qfetcspcimporterfretchen2.h"
#include "ui_qfetcspcimporterfretchen2.h"
#include "programoptions.h"
#include "qfpluginservices.h"
#include "qmodernprogresswidget.h"
#include <QtConcurrent/QtConcurrent>

QFETCSPCImporterFretchen2::QFETCSPCImporterFretchen2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFETCSPCImporterFretchen2)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
    ui->setupUi(this);
    lastTCSPCFileDir="";

    tcspcFilters.clear();
    ui->cmbFileformat->clear();

    // add more fileformats here
    tcspcFilters=QFTCSPCReader::getImporterFilterList();
    tcspcFormatNames=QFTCSPCReader::getImporterFormatNameList();
    ui->cmbFileformat->clear();
    ui->cmbFileformat->addItems(tcspcFormatNames);

    lastTCSPCFileFilter=tcspcFilters[0];

    plteInterphotonTimes=new JKQTPxyLineGraph(ui->pltTrace);
    plteInterphotonTimes->set_drawLine(false);
    plteInterphotonTimes->set_symbol(JKQTPdot);
    plteInterphotonTimes->set_title(tr("all photons"));
    plteInterphotonTimes->set_xColumn(0);
    plteInterphotonTimes->set_yColumn(1);
    plteInterphotonTimes->set_color(QColor("darkblue"));
    plteInterphotonTimes->set_lineWidth(0.5);
    plteIPTLevelLine=new JKQTPgeoLine(ui->pltTrace,0,0,0,0);
    plteIPTLevelLine->set_color(QColor("red"));
    ui->pltTrace->addGraph(plteInterphotonTimes);
    ui->pltTrace->addGraph(plteIPTLevelLine);
    ui->pltTrace->getYAxis()->set_logAxis(true);
    ui->pltTrace->getYAxis()->set_axisLabel(tr("interphoton time [{\\mu}s]"));
    ui->pltTrace->getXAxis()->set_axisLabel(tr("arrivaltime [s]"));

    plteBurstRate=new JKQTPxyLineGraph(ui->pltBurstRate);
    plteBurstRate->set_drawLine(false);
    plteBurstRate->set_symbol(JKQTPfilledCircle);
    plteBurstRate->set_title("");
    plteBurstRate->set_xColumn(0);
    plteBurstRate->set_yColumn(1);
    plteBurstRate->set_symbolSize(4);
    plteBurstRate->set_color(QColor("darkblue"));
    plteBurstRate->set_fillColor(QColor("darkblue").lighter());
    plteBurstRate->set_lineWidth(0.5);
    plteMinBurstRate=new JKQTPxFunctionLineGraph(ui->pltBurstRate);
    plteMinBurstRate->setSpecialFunction(JKQTPxFunctionLineGraph::Polynomial);
    plteMinBurstRate->set_paramsV(0,0);
    plteMinBurstRate->set_color(QColor("red"));
    plteMaxBurstRate=new JKQTPxFunctionLineGraph(ui->pltBurstRate);
    plteMaxBurstRate->setSpecialFunction(JKQTPxFunctionLineGraph::Polynomial);
    plteMaxBurstRate->set_paramsV(0,0);
    plteMaxBurstRate->set_color(QColor("darkgreen"));
    ui->pltBurstRate->addGraph(plteBurstRate);
    ui->pltBurstRate->addGraph(plteMinBurstRate);
    ui->pltBurstRate->addGraph(plteMaxBurstRate);
    ui->pltBurstRate->getXAxis()->set_axisLabel(tr("burst size [photons]"));
    ui->pltBurstRate->getYAxis()->set_axisLabel(tr("burst duration [ms]"));



    plteProximity=new JKQTPbarHorizontalGraph(ui->pltProximity);
    plteProximity->set_xColumn(0);
    plteProximity->set_xColumn(1);
    plteProximity->set_color(QColor("darkblue"));
    plteProximity->set_fillColor(QColor("darkblue").lighter());
    ui->pltProximity->addGraph(plteProximity);
    ui->pltProximity->getXAxis()->set_axisLabel(tr("proximity ratio"));
    ui->pltProximity->getYAxis()->set_axisLabel(tr("frequency"));

    readSettings();
    setEditControlsEnabled(false);

    ui->tabWidget->setCurrentIndex(0);
}

QFETCSPCImporterFretchen2::~QFETCSPCImporterFretchen2()
{
    delete ui;
}

void QFETCSPCImporterFretchen2::writeSettings()
{
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinCrosstalk,  "QFETCSPCImporterFretchen2/spinCrosstalk");
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinFDir,  "QFETCSPCImporterFretchen2/spinFDir");
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinMinRate,  "QFETCSPCImporterFretchen2/spinMinRate");
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinMaxRate,  "QFETCSPCImporterFretchen2/spinMaxRate");
    ProgramOptions::setConfigWindowGeometry(this, "QFETCSPCImporterFretchen2/geometry");
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinMaxBurstDuration,  "QFETCSPCImporterFretchen2/spinMaxBurstDuration");
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinMaxIPT,  "QFETCSPCImporterFretchen2/spinMaxIPT");
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinLEESigma,  "QFETCSPCImporterFretchen2/spinLEESigma");
    ProgramOptions::setConfigQSpinBox(ui->spinLEERange,  "QFETCSPCImporterFretchen2/spinLEERange");
    ProgramOptions::setConfigQSpinBox(ui->spinHBins,  "QFETCSPCImporterFretchen2/spinHBins");
    ProgramOptions::setConfigQCheckBox( ui->chkLEE, "QFETCSPCImporterFretchen2/chkLEE");
    ProgramOptions::setConfigQCheckBox(ui->chkSHowFilteredIPT, "QFETCSPCImporterFretchen2/chkSHowFilteredIPT");
    ProgramOptions::setConfigQSpinBox(ui->spinMinBurstSize,  "QFETCSPCImporterFretchen2/spinMinBurstSize");
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinDisplayLength, "QFETCSPCImporterFretchen2/spinDisplayLength");
    ProgramOptions::setConfigValue( "QFETCSPCImporterFretchen2/lastTCSPCFileFilter",lastTCSPCFileFilter);
    ProgramOptions::setConfigValue( "QFETCSPCImporterFretchen2/lastTCSPCFileDir",lastTCSPCFileDir);
    if (ui->cmbGreenChannel->count()>0 && ui->cmbGreenChannel->currentIndex()>=0) ProgramOptions::setConfigQComboBox(ui->cmbGreenChannel, "QFETCSPCImporterFretchen2/cmbGreenChannel");
    if (ui->cmbRedChannel->count()>0 && ui->cmbRedChannel->currentIndex()>=0) ProgramOptions::setConfigQComboBox(ui->cmbRedChannel, "QFETCSPCImporterFretchen2/cmbRedChannel");
}

void QFETCSPCImporterFretchen2::readSettings()
{
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinCrosstalk,  "QFETCSPCImporterFretchen2/spinCrosstalk", 4);
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinFDir,  "QFETCSPCImporterFretchen2/spinFDir", 0);
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinMinRate,  "QFETCSPCImporterFretchen2/spinMinRate", 10);
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinMaxRate,  "QFETCSPCImporterFretchen2/spinMaxRate", 100);
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinMaxBurstDuration,  "QFETCSPCImporterFretchen2/spinMaxBurstDuration", 8);
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinMaxIPT,  "QFETCSPCImporterFretchen2/spinMaxIPT", 70);
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinLEESigma,  "QFETCSPCImporterFretchen2/spinLEESigma", 5);
    ProgramOptions::getConfigQSpinBox(ui->spinLEERange,  "QFETCSPCImporterFretchen2/spinLEERange", 8);
    ProgramOptions::getConfigQSpinBox(ui->spinHBins,  "QFETCSPCImporterFretchen2/spinHBins", 35);
    ProgramOptions::getConfigQCheckBox( ui->chkLEE, "QFETCSPCImporterFretchen2/chkLEE",true);
    ProgramOptions::getConfigQCheckBox(ui->chkSHowFilteredIPT, "QFETCSPCImporterFretchen2/chkSHowFilteredIPT", true);
    ProgramOptions::getConfigQSpinBox(ui->spinMinBurstSize,  "QFETCSPCImporterFretchen2/spinMinBurstSize", 60);
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinDisplayLength, "QFETCSPCImporterFretchen2/spinDisplayLength", 500);
    ProgramOptions::getConfigWindowGeometry(this, "QFETCSPCImporterFretchen2/geometry");
    lastTCSPCFileFilter=ProgramOptions::getConfigValue( "QFETCSPCImporterFretchen2/lastTCSPCFileFilter",lastTCSPCFileFilter).toString();
    lastTCSPCFileDir=ProgramOptions::getConfigValue( "QFETCSPCImporterFretchen2/lastTCSPCFileDir",lastTCSPCFileDir).toString();
}


void QFETCSPCImporterFretchen2::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelpDirectory("tcspcimporter")+"fretchen.html");
}

void QFETCSPCImporterFretchen2::on_btnClose_clicked()
{
    writeSettings();
    close();
    //deleteLater();
}

void QFETCSPCImporterFretchen2::on_btnSelectFile_clicked()
{
    QString fileName = qfGetOpenFileName(this, tr("Select TCSPC File ..."), lastTCSPCFileDir, tcspcFilters.join(";;"), &lastTCSPCFileFilter);
    if (!fileName.isEmpty()) {
        lastTCSPCFileDir=QFileInfo(fileName).dir().absolutePath();
        ui->cmbFileformat->setCurrentIndex(tcspcFilters.indexOf(lastTCSPCFileFilter));
        ui->edtTCSPCFile->setText(fileName);
        ui->edtTCSPCFile->setFocus(Qt::MouseFocusReason);
        on_btnLoad_clicked();
        writeSettings();
    }

}

void QFETCSPCImporterFretchen2::on_btnSelectBackground_clicked()
{
    QString fileName = qfGetOpenFileName(this, tr("Select TCSPC File ..."), lastTCSPCFileDir, lastTCSPCFileFilter);
    if (!fileName.isEmpty()) {
        lastTCSPCFileDir=QFileInfo(fileName).dir().absolutePath();
        ui->edtTCSPCBackground->setText(fileName);
        ui->edtTCSPCBackground->setFocus(Qt::MouseFocusReason);
        //updateFromFile();
        writeSettings();
    }
}

void QFETCSPCImporterFretchen2::on_btnLoad_clicked()
{
    QString filename=ui->edtTCSPCFile->text();
    if (QFile::exists(filename)) {
        updateFromFile();
        setEditControlsEnabled(true);
    } else {
        setEditControlsEnabled(false);
        QMessageBox::critical(this, tr("TCSPC Burst Analyzer"), tr("The file '%1' does not exist.\nPlease select an existing file!").arg(filename));
    }
    writeSettings();
}


class QFTCSPCReaderOpener {
    public:
        QFTCSPCReaderOpener(QFTCSPCReader* reader) {
            this->reader=reader;
        }

        bool operator()(const QString &filename, const QString &parameters) {
            return reader->open(filename, parameters);
        }
        typedef bool result_type;

        protected:
            QFTCSPCReader* reader;
};

void QFETCSPCImporterFretchen2::on_btnLoadFiles_clicked()
{
    loadTCSPCFiles();


}

void QFETCSPCImporterFretchen2::on_btnApplyBurstSelection_clicked()
{
    updateBurstSelection();
    ui->tabBurstAnalysis->setEnabled(true);
    ui->tabWidget->setTabEnabled(2, true);
}

void QFETCSPCImporterFretchen2::on_btnApplyBurstAnalysis_clicked()
{
    updateAnalysis();
}



void QFETCSPCImporterFretchen2::setEditControlsEnabled(bool en)
{
    ui->widMoreOptions->setEnabled(en);
    if (!en) {
        ui->tabWidget->setTabEnabled(1, false);
        ui->tabWidget->setTabEnabled(2, false);
        ui->tabBurstAnalysis->setEnabled(false);
        ui->tabBurstSelection->setEnabled(false);
        ui->tabWidget->setCurrentIndex(0);
    }
}

void QFETCSPCImporterFretchen2::updateFromFile()
{
    ui->cmbGreenChannel->clear();
    ui->cmbRedChannel->clear();
    QString filename=ui->edtTCSPCFile->text();
    if (QFile::exists(filename)) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QModernProgressDialog progress(this);
        progress.setHasCancel(false);
        progress.setLabelText(tr("reading TCSPC file ..."));
        progress.setMode(true, false);
        progress.show();
        progress.raise();
        QApplication::processEvents();

        QFTCSPCReader* reader=QFTCSPCReader::getImporter(ui->cmbFileformat->currentIndex());
        if (reader) {
            QFTCSPCReaderOpener opener(reader);
            QFuture<bool> opened=QtConcurrent::run(opener,filename, QString());
            while (!opened.isFinished()) {
                QApplication::processEvents();
            }
            if (opened.result()) {
                // && reader->open(filename)) {
                int ccnt=reader->inputChannels();
                QVector<int> channels;
                for (int i=0; i<ccnt; i++) {
                    double cr=reader->avgCountRate(i);
                    ui->cmbGreenChannel->addItem(tr("channel %1 (%2 kHz)").arg(i).arg(cr, 0, 'f', 3));
                    ui->cmbRedChannel->addItem(tr("channel %1 (%2 kHz)").arg(i).arg(cr, 0, 'f', 3));
                    if (cr>0) channels<<i;
                }
                ProgramOptions::getConfigQComboBox(ui->cmbGreenChannel, "QFETCSPCImporterFretchen2/cmbGreenChannel", channels.value(0,0));
                ProgramOptions::getConfigQComboBox(ui->cmbRedChannel, "QFETCSPCImporterFretchen2/cmbRedChannel", channels.value(1,1));
            }
        }
        QApplication::restoreOverrideCursor();
    }

}

void QFETCSPCImporterFretchen2::updateCTRTrace()
{
    //qDebug()<<ui->tabWidget->isTabEnabled(0)<<ui->tabWidget->isTabEnabled(1)<<ui->tabWidget->isTabEnabled(2);
    if (!ui->tabWidget->isTabEnabled(1)) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    double l=ui->spinDisplayLength->value()*1e-3;
    double s=qMin(ui->spinDisplayStart->value(), ui->spinDisplayStart->maximum()-l);
    ui->pltTrace->set_doDrawing(false);
    JKQTPdatastore* ds=ui->pltTrace->getDatastore();
    ds->clear();
    QVector<double> x, y;
    //qDebug()<<s<<l<<photons.firstPhoton<<photons.duration;
    if (ui->chkSHowFilteredIPT->isChecked() && photons.lastLEEWindowSize>0) {
        for (int i=0; i<photons.photondata.size(); i++) {
            const double at=photons.photondata[i].arrivaltime;
            const double ipt=photons.photondata[i].IPT_filtered*1e6;
            if (at>=s && at<=s+l) {
                x<<at;
                y<<ipt;
            }
            if (i%5000==0) QApplication::processEvents();
        }
        plteInterphotonTimes->set_title(tr("all photons, LEE filtered"));
        plteInterphotonTimes->set_drawLine(true);
    } else {
        for (int i=0; i<photons.photondata.size(); i++) {
            const double at=photons.photondata[i].arrivaltime;
            const double ipt=photons.photondata[i].IPT*1e6;
            if (at>=s && at<=s+l) {
                x<<at;
                y<<ipt;
            }
            if (i%5000==0) QApplication::processEvents();
        }
        plteInterphotonTimes->set_drawLine(false);
        plteInterphotonTimes->set_title(tr("all photons"));
    }
    plteInterphotonTimes->set_xColumn(ds->addCopiedColumn(x, tr("Arrivaltime [s]")));
    plteInterphotonTimes->set_yColumn(ds->addCopiedColumn(y, tr("Inter-Photon Time IPT [microseconds]")));

    plteIPTLevelLine->set_x1(s);
    plteIPTLevelLine->set_x2(s+l);
    plteIPTLevelLine->set_y1(ui->spinMaxIPT->value());
    plteIPTLevelLine->set_y2(ui->spinMaxIPT->value());
    ui->pltTrace->zoomToFit();
    ui->pltTrace->set_doDrawing(true);
    ui->pltTrace->update_plot();
    QApplication::restoreOverrideCursor();

}

void QFETCSPCImporterFretchen2::updateAnalysis()
{
    if (!ui->tabWidget->isTabEnabled(2)) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));


    TTTRCalcBurstPropertiesProps p;
    p.backG=ui->spinBackG->value();
    p.backR=ui->spinBackR->value();
    p.crosstalk=ui->spinCrosstalk->value()/100.0;
    p.fdirect=ui->spinFDir->value();
    TTTRCalcBurstProperties(&bursts, p);

    updateAnalysisPlots();

    QApplication::restoreOverrideCursor();
}


void QFETCSPCImporterFretchen2::updateAnalysisPlots()
{
    if (!ui->tabWidget->isTabEnabled(2)) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->pltBurstRate->set_doDrawing(false);
    ui->pltProximity->set_doDrawing(false);
    JKQTPdatastore* dsB=ui->pltBurstRate->getDatastore();
    JKQTPdatastore* dsP=ui->pltProximity->getDatastore();
    dsB->clear();
    dsP->clear();

    QVector<double> PVec, DVec, RVec, SAllVec, DAllVec;
    const double mir=ui->spinMinRate->value()*1e3;
    const double mar=ui->spinMaxRate->value()*1e3;

    for (int i=0; i<bursts.burstdata.size(); i++) {
        const double r=bursts.burstdata[i].getRate();
        SAllVec<<(bursts.burstdata[i].photonG+bursts.burstdata[i].photonR);
        DAllVec<<(bursts.burstdata[i].duration*1e3);
        if (r>=mir && r<=mar) {
            RVec<<r;
            DVec<<(bursts.burstdata[i].duration);
            PVec<<bursts.burstdata[i].P;
        }
    }

    ui->labNBursts->setText(QString::number(PVec.size()));
    ui->labAvgBDuration->setText(QString("%1 ms").arg(qfstatisticsAverage(DVec)*1e3, 0, 'f', 4));
    ui->labAvgRate->setText(QString("%1 kHz").arg(qfstatisticsAverage(RVec)*1e-3, 0, 'f', 3));
    ui->labAvgP->setText(QString("%1").arg(qfstatisticsAverage(PVec), 0, 'f', 4));

    int nbins=ui->spinHBins->value();
    QVector<double> HX(nbins,0.0);
    QVector<double> HY(nbins,0.0);
    statisticsHistogram(PVec.data(), PVec.size(), HX.data(), HY.data(), nbins);
    plteProximity->set_xColumn(dsP->addCopiedColumn(HX, tr("P histogram: P")));
    plteProximity->set_yColumn(dsP->addCopiedColumn(HY, tr("P histogram: frequency")));

    plteBurstRate->set_xColumn(dsB->addCopiedColumn(SAllVec, tr("burst size [photons]")));
    plteBurstRate->set_yColumn(dsB->addCopiedColumn(DAllVec, tr("burst duration [ms]")));

    plteMinBurstRate->set_paramsV(0, 1e3/mir);
    plteMaxBurstRate->set_paramsV(0, 1e3/mar);

    ui->pltBurstRate->zoomToFit(true, true, true, true);
    ui->pltProximity->zoomToFit();
    ui->pltBurstRate->set_doDrawing(true);
    ui->pltProximity->set_doDrawing(true);
    ui->pltBurstRate->update_plot();
    ui->pltProximity->update_plot();
    QApplication::restoreOverrideCursor();
}


class QFETCSPCImporterFretchen2LEEFilterer {
    public:
        QFETCSPCImporterFretchen2LEEFilterer(TCSPCPhotonsData* reader) {
            this->obj=reader;
        }

        void operator()(int win, double sigma0) {
             obj->applyLEEFilter(win, sigma0);
        }
        typedef void result_type;

        protected:
            TCSPCPhotonsData* obj;
};


class QFETCSPCImporterFretchen2BurstFinder {
    public:
        QFETCSPCImporterFretchen2BurstFinder(TCSPCBurstsData* output, const TCSPCPhotonsData* photons) {
            this->output=output;
            this->photons=photons;
        }

        void operator()(TTTRFindBurstsEvalProps props) {
             TTTRFindBursts(output, (*photons), props);
        }
        typedef void result_type;

        protected:
            TCSPCBurstsData* output;
            const TCSPCPhotonsData* photons;
};

void QFETCSPCImporterFretchen2::updateBurstSelection()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QModernProgressDialog progress(this);
    progress.setHasCancel(false);
    progress.setLabelText(tr("performing burst selection ..."));
    progress.setMode(true, false);
    progress.show();
    progress.raise();
    if (ui->chkLEE->isChecked()) {
        QFETCSPCImporterFretchen2LEEFilterer exe(&photons);
        QFuture<void> opened=QtConcurrent::run(exe, ui->spinLEERange->value(), ui->spinLEESigma->value()*1e-3);
        progress.setLabelText(tr("applying LEE filter ..."));
        //photons.applyLEEFilter(ui->spinLEERange->value(), ui->spinLEESigma->value()*1e-3);
        while (!opened.isFinished()) {
            QApplication::processEvents();
        }
    }
    if (photons.lastLEEWindowSize>0) {
        ui->labLEEWindow->setText(QString::number(photons.lastLEEWindowSize));
        ui->labLEESigma0->setText(QString("%1 ms").arg(photons.lastLEESigma0*1e3, 0, 'f', 3));
    } else {
        ui->labLEEWindow->setText(QString("---"));
        ui->labLEESigma0->setText(QString("---"));
    }

    progress.setLabelText(tr("performing burst selection ..."));
    TCSPCBurstsData b;
    QFETCSPCImporterFretchen2BurstFinder burstFinder(&b, &photons);
    TTTRFindBurstsEvalProps props;
    props.maxBurstDuration=ui->spinMaxBurstDuration->value()*1e-3;
    props.maxIPT=ui->spinMaxIPT->value()*1e-6;
    props.minBurstSize=ui->spinMinBurstSize->value();
    props.chG=ui->cmbGreenChannel->currentIndex();
    props.chR=ui->cmbRedChannel->currentIndex();
    props.filtered=ui->chkLEE->isChecked();
    QFuture<void> opened=QtConcurrent::run(burstFinder, props);
    while (!opened.isFinished()) {
        QApplication::processEvents();
    }
    bursts=b;

    ui->labBursts->setText(tr("count: %1,&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;avg. duration: %2 ms").arg(bursts.burstdata.size()).arg(bursts.avgDuration*1e3,0,'f',3));

    updateCTRTrace();
    updateAnalysis();
    QApplication::restoreOverrideCursor();
}

void QFETCSPCImporterFretchen2::loadTCSPCFiles()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString filename=ui->edtTCSPCFile->text();
    QString filenameB=ui->edtTCSPCBackground->text();
    writeSettings();
    if (QFile::exists(filename)) {
        //updateFromFile();
        setEditControlsEnabled(true);


        QModernProgressDialog progress(this);
        progress.setHasCancel(false);
        progress.setLabelText(tr("opening TCSPC file ..."));
        progress.setMode(true, false);
        progress.show();
        progress.raise();
        QApplication::processEvents();
        QFTCSPCReader* reader=QFTCSPCReader::getImporter(ui->cmbFileformat->currentIndex());
        QFTCSPCReader* readerBack=NULL;
        if (QFile::exists(filenameB)) readerBack=QFTCSPCReader::getImporter(ui->cmbFileformat->currentIndex());
        bool openedFiles=false;
        if (reader && readerBack) {
            QFTCSPCReaderOpener opener(reader);
            QFuture<bool> opened=QtConcurrent::run(opener, filename, QString());
            QFTCSPCReaderOpener openerB(readerBack);
            QFuture<bool> openedB=QtConcurrent::run(openerB, filenameB, QString());
            while (!opened.isFinished() || !openedB.isFinished()) {
                QApplication::processEvents();
            }
            openedFiles=opened.result()&&openedB.result();
        } else {
            QFTCSPCReaderOpener opener(reader);
            QFuture<bool> opened=QtConcurrent::run(opener, filename, QString());
            while (!opened.isFinished()) {
                QApplication::processEvents();
            }
            openedFiles=opened.result();
        }

        TCSPCPhotonsData photonsNew;
        progress.setLabelText(tr("reading TCSPC file ..."));

        TTTRReadPhotonsDataEvalProps props;
        props.start=-1;
        props.end=-1;
        props.chG=ui->cmbGreenChannel->currentIndex();
        props.chR=ui->cmbRedChannel->currentIndex();
        if (!ui->chkFromStart->isChecked()) props.start=ui->spinRangeStart->value();
        if (!ui->chkTillEnd->isChecked()) props.end=ui->spinRangeEnd->value();
        if (props.start>=0 && props.end>=0 && props.start>props.end)  qSwap(props.start, props.end);

        QFuture<bool> readTCSPC=QtConcurrent::run(TTTRReadPhotonsData, &photonsNew, reader, readerBack, props);
        while (!readTCSPC.isFinished()) {
            QApplication::processEvents();
        }
        if (readTCSPC.result()) {
            photons=photonsNew;

            ui->spinDisplayStart->setMaximum(photons.duration+photons.firstPhoton);
            ui->spinDisplayStart->setMinimum(photons.firstPhoton);
            ui->spinDisplayStart->setValue(0);
            ui->labDuration->setText(QString("%1 s").arg(photons.duration, 0, 'f', 2));
            ui->labP->setText(QString("%1").arg(photons.photondata.size()));
            ui->labPGreen->setText(QString("%1").arg(photons.photonsG));
            ui->labPRed->setText(QString("%1").arg(photons.photonsR));
            ui->labAvgIPT->setText(QString("%1 &mu;s").arg(photons.avg_IPT, 0, 'f', 3));

            ui->labCGreen->setText(QString("%1 (B: %2) kHz").arg(photons.avgrate.value(ui->cmbGreenChannel->currentIndex())*1e-3, 0, 'f', 3).arg(photons.backgroundrate.value(ui->cmbGreenChannel->currentIndex())*1e-3, 0, 'f', 3));
            ui->labCRed->setText(QString("%1 (B: %2) kHz").arg(photons.avgrate.value(ui->cmbRedChannel->currentIndex())*1e-3, 0, 'f', 3).arg(photons.backgroundrate.value(ui->cmbRedChannel->currentIndex())*1e-3, 0, 'f', 3));
            ui->labBackG->setText(QString("%1 Hz").arg(photons.backgroundrate.value(ui->cmbGreenChannel->currentIndex()), 0, 'f', 3));
            ui->labBackR->setText(QString("%1 Hz").arg(photons.backgroundrate.value(ui->cmbRedChannel->currentIndex()), 0, 'f', 3));
            ui->spinBackG->setValue(photons.backgroundrate.value(ui->cmbGreenChannel->currentIndex(),0));
            ui->spinBackR->setValue(photons.backgroundrate.value(ui->cmbRedChannel->currentIndex(),0));


            setEditControlsEnabled(true);
            ui->tabBurstSelection->setEnabled(true);
            ui->tabWidget->setCurrentIndex(1);
            ui->tabWidget->setTabEnabled(1, true);
            updateCTRTrace();
        }
    } else {
        setEditControlsEnabled(false);
        QMessageBox::critical(this, tr("TCSPC Burst Analyzer"), tr("The file '%1' does not exist.\nPlease select an existing file!").arg(filename));
    }

    QApplication::restoreOverrideCursor();
}
