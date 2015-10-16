#include "statistics_tools.h"
#include "qfetcspcimporterfretchen2.h"
#include "ui_qfetcspcimporterfretchen2.h"
#include "programoptions.h"
#include "qfpluginservices.h"
#include "qmodernprogresswidget.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#  include <QtConcurrent/QtConcurrent>
#else
#  include <QtConcurrentRun>
#endif
#include "qfrdrcolumngraphsinterface.h"
#include "qfrdrtableinterface.h"
#include "qfproject.h"
#include "qfrawdatarecord.h"

QFETCSPCImporterFretchen2::QFETCSPCImporterFretchen2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFETCSPCImporterFretchen2)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
    ui->setupUi(this);
    //ui->tabMultiFile->setVisible(true);//false);
    ui->lstMultiFile->setModel(&ms_model);
    //resetEditControls();
    updateMultiFile();
    ms_model.setEditable(true);
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
    plteInterphotonTimes->set_lineWidth(1);
    plteInterphotonTimes2=new JKQTPxyLineGraph(ui->pltTrace);
    plteInterphotonTimes2->set_drawLine(false);
    plteInterphotonTimes2->set_symbol(JKQTPnoSymbol);
    plteInterphotonTimes2->set_title(tr("red photons"));
    plteInterphotonTimes2->set_xColumn(-1);
    plteInterphotonTimes2->set_yColumn(-1);
    plteInterphotonTimes2->set_color(QColor("red"));
    plteInterphotonTimes2->set_lineWidth(1);
    plteInterphotonTimes2->set_visible(false);
    plteIPTLevelLine=new JKQTPgeoLine(ui->pltTrace,0,0,0,0);
    plteIPTLevelLine->set_color(QColor("red"));
    ui->pltTrace->addGraph(plteInterphotonTimes);
    ui->pltTrace->addGraph(plteInterphotonTimes2);
    ui->pltTrace->addGraph(plteIPTLevelLine);
    ui->pltTrace->getYAxis()->set_logAxis(true);
    ui->pltTrace->getYAxis()->set_axisLabel(tr("interphoton time [{\\mu}s]"));
    ui->pltTrace->getXAxis()->set_axisLabel(tr("arrivaltime [s]"));

    plteBurstRate=new JKQTPxyParametrizedScatterGraph(ui->pltBurstRate);
    plteBurstRate->set_drawLine(false);
    plteBurstRate->set_symbol(JKQTPfilledCircle);
    plteBurstRate->set_title("");
    plteBurstRate->set_xColumn(0);
    plteBurstRate->set_yColumn(1);
    plteBurstRate->set_symbolSize(3);
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
    writeSettings();
    delete ui;
}

void QFETCSPCImporterFretchen2::writeSettings()
{
    ProgramOptions::setConfigQCheckBox(ui->chkMSRenumberBursts,  "QFETCSPCImporterFretchen2/chkMSRenumberBursts");
    ProgramOptions::setConfigQCheckBox(ui->chkMSAddFileID,  "QFETCSPCImporterFretchen2/chkMSAddFileID");
    ProgramOptions::setConfigQCheckBox(ui->chkNormHistogram,  "QFETCSPCImporterFretchen2/chkNormHistogram");
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinCrosstalk,  "QFETCSPCImporterFretchen2/spinCrosstalk");
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinGamma,  "QFETCSPCImporterFretchen2/spinGamma");
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinFDir,  "QFETCSPCImporterFretchen2/spinFDir");
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinMinRate,  "QFETCSPCImporterFretchen2/spinMinRate");
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinMaxRate,  "QFETCSPCImporterFretchen2/spinMaxRate");
    ProgramOptions::setConfigWindowGeometry(this, "QFETCSPCImporterFretchen2/geometry");
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinMaxBurstDuration,  "QFETCSPCImporterFretchen2/spinMaxBurstDuration");
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinMaxIPT,  "QFETCSPCImporterFretchen2/spinMaxIPT");
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinLEESigma,  "QFETCSPCImporterFretchen2/spinLEESigma");
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinDisplayBin,  "QFETCSPCImporterFretchen2/spinDisplayBin");
    ProgramOptions::setConfigQSpinBox(ui->spinLEERange,  "QFETCSPCImporterFretchen2/spinLEERange");
    ProgramOptions::setConfigQSpinBox(ui->spinHBins,  "QFETCSPCImporterFretchen2/spinHBins");
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinHBinWidth,  "QFETCSPCImporterFretchen2/spinHBinWidth");
    ProgramOptions::setConfigQCheckBox( ui->chkLEE, "QFETCSPCImporterFretchen2/chkLEE");
    ProgramOptions::setConfigQCheckBox(ui->chkSHowFilteredIPT, "QFETCSPCImporterFretchen2/chkSHowFilteredIPT");
    ProgramOptions::setConfigQCheckBox(ui->chkShowBursts, "QFETCSPCImporterFretchen2/chkShowBursts");
    ProgramOptions::setConfigQSpinBox(ui->spinMinBurstSize,  "QFETCSPCImporterFretchen2/spinMinBurstSize");
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinDisplayLength, "QFETCSPCImporterFretchen2/spinDisplayLength");
    ProgramOptions::setConfigQComboBox(ui->cmbAnaHistMode,  "QFETCSPCImporterFretchen2/cmbAnaHistMode");
    ProgramOptions::setConfigValue( "QFETCSPCImporterFretchen2/lastTCSPCFileFilter",lastTCSPCFileFilter);
    ProgramOptions::setConfigValue( "QFETCSPCImporterFretchen2/lastTCSPCFileDir",lastTCSPCFileDir);
    if (ui->cmbGreenChannel->count()>0 && ui->cmbGreenChannel->currentIndex()>=0) ProgramOptions::setConfigQComboBox(ui->cmbGreenChannel, "QFETCSPCImporterFretchen2/cmbGreenChannel");
    if (ui->cmbRedChannel->count()>0 && ui->cmbRedChannel->currentIndex()>=0) ProgramOptions::setConfigQComboBox(ui->cmbRedChannel, "QFETCSPCImporterFretchen2/cmbRedChannel");
}

void QFETCSPCImporterFretchen2::readSettings()
{
    ProgramOptions::getConfigQCheckBox(ui->chkMSRenumberBursts,  "QFETCSPCImporterFretchen2/chkMSRenumberBursts", true);
    ProgramOptions::getConfigQCheckBox(ui->chkMSAddFileID,  "QFETCSPCImporterFretchen2/chkMSAddFileID", true);
    ProgramOptions::getConfigQCheckBox(ui->chkNormHistogram,  "QFETCSPCImporterFretchen2/chkNormHistogram", true);
    ProgramOptions::getConfigQComboBox(ui->cmbAnaHistMode,  "QFETCSPCImporterFretchen2/cmbAnaHistMode", 0);
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinDisplayBin,  "QFETCSPCImporterFretchen2/spinDisplayBin", 1);
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinCrosstalk,  "QFETCSPCImporterFretchen2/spinCrosstalk", 4);
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinGamma,  "QFETCSPCImporterFretchen2/spinGamma", 1);
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinFDir,  "QFETCSPCImporterFretchen2/spinFDir", 0);
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinMinRate,  "QFETCSPCImporterFretchen2/spinMinRate", 10);
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinMaxRate,  "QFETCSPCImporterFretchen2/spinMaxRate", 100);
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinMaxBurstDuration,  "QFETCSPCImporterFretchen2/spinMaxBurstDuration", 8);
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinMaxIPT,  "QFETCSPCImporterFretchen2/spinMaxIPT", 70);
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinLEESigma,  "QFETCSPCImporterFretchen2/spinLEESigma", 5);
    ProgramOptions::getConfigQSpinBox(ui->spinLEERange,  "QFETCSPCImporterFretchen2/spinLEERange", 8);
    ProgramOptions::getConfigQSpinBox(ui->spinHBins,  "QFETCSPCImporterFretchen2/spinHBins", 31);
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinHBinWidth,  "QFETCSPCImporterFretchen2/spinHBinWidth", 0.05);
    ProgramOptions::getConfigQCheckBox( ui->chkLEE, "QFETCSPCImporterFretchen2/chkLEE",true);
    ProgramOptions::getConfigQCheckBox(ui->chkSHowFilteredIPT, "QFETCSPCImporterFretchen2/chkSHowFilteredIPT", true);
    ProgramOptions::getConfigQCheckBox(ui->chkShowBursts, "QFETCSPCImporterFretchen2/chkShowBursts", true);
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
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QApplication::processEvents();
        lastTCSPCFileDir=QFileInfo(fileName).dir().absolutePath();
        ui->cmbFileformat->setCurrentIndex(tcspcFilters.indexOf(lastTCSPCFileFilter));
        ui->edtTCSPCFile->setText(fileName);
        //ui->edtTCSPCFile->setFocus(Qt::MouseFocusReason);
        on_btnLoad_clicked();
        //writeSettings();
        QApplication::restoreOverrideCursor();
    }

}

void QFETCSPCImporterFretchen2::on_btnSelectBackground_clicked()
{
    QString fileName = qfGetOpenFileName(this, tr("Select TCSPC File ..."), lastTCSPCFileDir, lastTCSPCFileFilter);
    if (!fileName.isEmpty()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QApplication::processEvents();
        lastTCSPCFileDir=QFileInfo(fileName).dir().absolutePath();
        ui->edtTCSPCBackground->setText(fileName);
        ui->edtTCSPCBackground->setFocus(Qt::MouseFocusReason);
        //writeSettings();
        QApplication::restoreOverrideCursor();
    }
}

void QFETCSPCImporterFretchen2::on_btnLoad_clicked()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QModernProgressDialog progress(this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setModal(true);
    progress.setHasCancel(false);
    progress.setLabelText(tr("reading TCSPC file ..."));
    progress.setMode(true, false);
    progress.open();
    progress.raise();
    QApplication::processEvents();

    QApplication::processEvents();
    QString filename=ui->edtTCSPCFile->text();
    if (QFile::exists(filename)) {
        updateFromFile();
        resetEditControls();
    } else {
        setEditControlsEnabled(false);
        QMessageBox::critical(this, tr("TCSPC Burst Analyzer"), tr("The file '%1' does not exist.\nPlease select an existing file!").arg(filename));
    }
    writeSettings();
    QApplication::restoreOverrideCursor();
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


void QFETCSPCImporterFretchen2::saveToProject(const QFDataExportTool &outData, const QFDataExportTool &outDataFiltered, const QString &grp, int calcStartIn, int histStartIn, int coloffset)
{
    saveToProject(outData, grp, tr(" - all bursts"), calcStartIn, histStartIn, coloffset);
    saveToProject(outDataFiltered, grp, tr(" - rate-filtered bursts"), calcStartIn, histStartIn, coloffset);
}

void QFETCSPCImporterFretchen2::saveToProject(const QFDataExportTool &outData, const QString &grp, const QString& nameAdd, int calcStartIn, int histStartIn, int coloffset)
{
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    QFRawDataRecord* rdrA=p->addRawData("table", tr("%1 - spFRET Burst Analysis%2").arg(grp).arg(nameAdd));
    if (rdrA) {
        rdrA->setGroup(p->addOrFindRDRGroup(grp));
        rdrA->setFolder(grp);
        rdrA->setDescription(outData.comment);
        rdrA->setQFProperty("AUTOSCALEXY_PLOTS_ON_SHOWUP", true, false, false);

        QFRDRColumnGraphsInterface* rdrAG=dynamic_cast<QFRDRColumnGraphsInterface*>(rdrA);
        QFRDRTableInterface* rdrAT=dynamic_cast<QFRDRTableInterface*>(rdrA);

            int calcStart=coloffset+calcStartIn;
            int histStart=coloffset+histStartIn;
            if ( rdrAT ) {
                outData.saveToTable(rdrAT, true);



                {
                    double pmin=outData.properties.value("burstfiltered_histogram_P_min", 0).toDouble();
                    double pmax=outData.properties.value("burstfiltered_histogram_P_max", 1).toDouble();
                    int pbins=outData.properties.value("burstfiltered_histogram_P_bins", 31).toInt();
                    double emin=outData.properties.value("burstfiltered_histogram_E_min", 0).toDouble();
                    double emax=outData.properties.value("burstfiltered_histogram_E_max", 1).toDouble();
                    int ebins=outData.properties.value("burstfiltered_histogram_E_bins", 31).toInt();
                    QString pre=QString("// normalize histograms:\nhistograms_normalized = %7;\n\n// burst duration histogram:\nbdur_bins=21;\n\n// burst size histogram:\nbsize_bins=21;\n\n// burst countrate histogram:\nbrate_bins=21;\n\n// P-histogram bins:\npmin = %1;\npmax = %2;\npbins = %3;\n\n// E-histogram bins:\nemin = %4;\nemax = %5;\nebins = %6;\n")
                                .arg(CDoubleToQString(pmin)).arg(CDoubleToQString(pmax)).arg(pbins)
                                .arg(CDoubleToQString(emin)).arg(CDoubleToQString(emax)).arg(ebins)
                                .arg(boolToQString(ui->chkNormHistogram->isChecked()));

                    rdrAT->tableSetPreEvaluationExpression(pre);



                    rdrAT->tableSetColumnTitle(calcStart, tr("Burst Duration: Delta T [ms]"));
                    rdrAT->tableSetColumnExpression(calcStart, QString("column(%1)*1e-3").arg(coloffset+6));
                    rdrAT->tableSetColumnTitle(calcStart+1, tr("Burst Size"));
                    rdrAT->tableSetColumnExpression(calcStart+1, QString("column(%1)+column(%2)").arg(coloffset+4).arg(coloffset+5));


                    rdrAT->tableSetColumnTitle(histStart, tr("P-Histogram: P"));
                    rdrAT->tableSetColumnExpression(histStart, QString("rangedhistogrambins(pmin, pmax, pbins)"));
                    rdrAT->tableSetColumnTitle(histStart+1, tr("P-Histogram: frequency"));
                    rdrAT->tableSetColumnExpression(histStart+1, QString("rangedhistogram(column(%1), pmin, pmax, pbins, histograms_normalized)").arg(coloffset+7));
                    rdrAT->tableSetColumnTitle(histStart+2, tr("E-Histogram: E"));
                    rdrAT->tableSetColumnExpression(histStart+2, QString("rangedhistogrambins(emin, emax, ebins)"));
                    rdrAT->tableSetColumnTitle(histStart+3, tr("E-Histogram: frequency"));
                    rdrAT->tableSetColumnExpression(histStart+3, QString("rangedhistogram(column(%1), emin, emax, ebins, histograms_normalized)").arg(coloffset+8));

                    rdrAT->tableSetColumnTitle(histStart+4, tr("Burst Duration-Histogram: Delta T[ms]"));
                    rdrAT->tableSetColumnExpression(histStart+4, QString("histogrambins(column(%1)*1e3, bdur_bins)").arg(coloffset+3));
                    rdrAT->tableSetColumnTitle(histStart+5, tr("Burst Duration-Histogram: frequency"));
                    rdrAT->tableSetColumnExpression(histStart+5, QString("histogram(column(%1)*1e3, bdur_bins, histograms_normalized)").arg(coloffset+3));

                    rdrAT->tableSetColumnTitle(histStart+6, tr("Burst Size-Histogram: photons"));
                    rdrAT->tableSetColumnExpression(histStart+6, QString("histogrambins(column(%1)+column(%2), bsize_bins)").arg(coloffset+4).arg(coloffset+5));
                    rdrAT->tableSetColumnTitle(histStart+7, tr("Burst Size-Histogram: frequency"));
                    rdrAT->tableSetColumnExpression(histStart+7, QString("histogram(column(%1)+column(%2), bsize_bins, histograms_normalized)").arg(coloffset+4).arg(coloffset+5));

                    rdrAT->tableSetColumnTitle(histStart+8, tr("Burst Countrate-Histogram: countrate [kcps]"));
                    rdrAT->tableSetColumnExpression(histStart+8, QString("histogrambins(column(%1)*1e-3, brate_bins)").arg(coloffset+6));
                    rdrAT->tableSetColumnTitle(histStart+9, tr("Burst Countrate-Histogram: frequency"));
                    rdrAT->tableSetColumnExpression(histStart+9, QString("histogram(column(%1)*1e-3, brate_bins, histograms_normalized)").arg(coloffset+6));



                    rdrAT->tableReevaluateExpressions();
                    rdrAT->tableReevaluateExpressions();

                }


            }
            if ( rdrAG) {
                QFRDRColumnGraphsInterface* cg=rdrAG;
                int g;
                g=cg->colgraphAddPlot(tr("proximity ratio histogram"), tr("proximity ratio P"), tr("frequency"));
                cg->colgraphAddGraph(g, histStart, histStart+1, QFRDRColumnGraphsInterface::cgtBars, "");
                g=cg->colgraphAddPlot(tr("FRET ratio histogram"), tr("FRET ratio E"), tr("frequency"));
                cg->colgraphAddGraph(g, histStart+2, histStart+3, QFRDRColumnGraphsInterface::cgtBars, "");
                g=cg->colgraphAddPlot(tr("burst duration histogram"), tr("burst duration \\Delta t_{Burst} [ms]"), tr("frequency"));
                cg->colgraphAddGraph(g, histStart+4, histStart+5, QFRDRColumnGraphsInterface::cgtBars, "");
                g=cg->colgraphAddPlot(tr("burst size histogram"), tr("burst size [photons]"), tr("frequency"));
                cg->colgraphAddGraph(g, histStart+6, histStart+7, QFRDRColumnGraphsInterface::cgtBars, "");
                g=cg->colgraphAddPlot(tr("burst countrate histogram"), tr("burst countrate [kcps]"), tr("frequency"));
                cg->colgraphAddGraph(g, histStart+8, histStart+9, QFRDRColumnGraphsInterface::cgtBars, "");
                g=cg->colgraphAddPlot(tr("burst size vs. duration"), tr("burst size [photons]"), tr("burst duration [ms]"));
                cg->colgraphAddGraph(g, calcStart+1,calcStart, QFRDRColumnGraphsInterface::cgtPoints, "");

            }

    } else {
        QFPluginServices::getInstance()->log_error(tr("error adding record to project"));
    }
}


void QFETCSPCImporterFretchen2::on_btnSaveToProject_clicked()
{
    QString grp=QFileInfo(ui->edtTCSPCFile->text()).fileName();
    saveToProject(outData, outDataFiltered, grp);
}

void QFETCSPCImporterFretchen2::on_btnAppendToMulti_clicked()
{
    ms_outData.append(outData);
    ms_outDataFiltered.append(outDataFiltered);
    ms_model.addItem(QFileInfo(ui->edtTCSPCFile->text()).fileName(), true, ms_outData.size()-1);
    updateMultiFile();
}

void QFETCSPCImporterFretchen2::on_btnSaveToMulti_clicked()
{
    QFDataExportTool out, outF;
    QStringList files;
    int cnt=collectMultiData(out, outF, files);
    if (cnt>0) {
        saveData(out, outF);
    }

}


int QFETCSPCImporterFretchen2::collectMultiData(QFDataExportTool &out, QFDataExportTool &outF, QStringList &files)
{
    if (ms_model.rowCount()>0) {
        int cnt=0;
        QVariantList vlfid, vlfidF;
        int c=0, cF=0;

        for (int i=0; i<ms_model.rowCount(); i++) {
            if (ms_model.isChecked(i) && i<ms_outData.size() && i<ms_outDataFiltered.size()) {
                if (cnt==0) {
                    out=ms_outData[i];
                    outF=ms_outDataFiltered[i];
                } else {
                    int rc=out.getRowCount();
                    for (int j=0; j<out.data.size(); j++) {
                        while (out.data.at(j).size()<rc) out.data[j].append(QVariant());
                        out.data[j]<<ms_outData[i].data.value(j, QVariantList());


                    }

                    rc=outF.getRowCount();
                    for (int j=0; j<outF.data.size(); j++) {
                        while (outF.data.at(j).size()<rc) outF.data[j].append(QVariant());
                        outF.data[j]<<ms_outDataFiltered[i].data.value(j, QVariantList());

                        //vlfidF<<cnt;
                    }

                }

                int rc=ms_outData[i].getRowCount();
                if (rc>0) {
                    for (int n=0; n<rc; n++) {
                        vlfid<<cnt;
                    }
                }
                rc=ms_outDataFiltered[i].getRowCount();
                if (rc>0) {
                    for (int n=0; n<rc; n++) {
                        vlfidF<<cnt;
                    }
                }
                files<<ms_model.data(ms_model.index(i), Qt::DisplayRole).toString();
                cnt++;
            }
        }
        bool renumber=ui->chkMSRenumberBursts->isChecked();
        if (renumber && cnt>0) {
            if (out.data.size()>0) {
                for (int i=0; i<out.data[0].size(); i++) {
                    out.data[0].operator [](i)=i;
                }
            }
            if (outF.data.size()>0) {
                for (int i=0; i<outF.data[0].size(); i++) {
                    outF.data[0].operator [](i)=i;
                }
            }
        }
        if (cnt>0) {
            if (ui->chkMSAddFileID->isChecked()) {
                out.data.append(vlfid);
                outF.data.append(vlfidF);
                out.colHeaders.append(tr("file-ID"));
                outF.colHeaders.append(tr("file-ID"));
            }
            out.properties.remove("input_file");
            out.properties["input_files"]=files.join(";   ");
            outF.properties.remove("input_file");
            outF.properties["input_files"]=files.join(";   ");
            //
        }
        return cnt;
    }
    return 0;
}

void QFETCSPCImporterFretchen2::clearRanges()
{
    for (int i=0; i<plteRanges.size(); i++) {
        if (plteRanges[i]) {
            ui->pltTrace->deleteGraph(plteRanges[i], true);
        }
    }
    plteRanges.clear();
}

void QFETCSPCImporterFretchen2::on_btnSaveMultiToProject_clicked()
{
    QFDataExportTool out, outF;
    QStringList files;
    int cnt=collectMultiData(out, outF, files);
    if (cnt>0) {
        saveToProject(out, outF, files.join(", "), 1);
    }
}

void QFETCSPCImporterFretchen2::on_btnClear_clicked()
{
    ms_outData.clear();
    ms_outDataFiltered.clear();
    ms_model.clear();
    updateMultiFile();
}

void QFETCSPCImporterFretchen2::on_btnDelete_clicked()
{
    int i=ui->lstMultiFile->currentIndex().row();
    if (i>=0 && i<ms_model.rowCount()) {
        //int id=ms_model.data(ms_model.index(i), Qt::UserRole).toInt();
        ms_model.removeItem(i);
        if (i>=0 && i<ms_outData.size()) ms_outData.removeAt(i);
        if (i>=0 && i<ms_outDataFiltered.size()) ms_outDataFiltered.removeAt(i);
    }
    updateMultiFile();
}

void QFETCSPCImporterFretchen2::on_btnDown_clicked()
{
    int i=ui->lstMultiFile->currentIndex().row();
    if (i>=0 && i<ms_model.rowCount()-1) {
        //int id=ms_model.data(ms_model.index(i), Qt::UserRole).toInt();
        ms_model.swapItems(i,i+1);
        if (i>=0 && i<ms_outData.size()-1) ms_outData.swap(i, i+1);
        if (i>=0 && i<ms_outDataFiltered.size()-1) ms_outDataFiltered.swap(i, i+1);
    }
    updateMultiFile();
}

void QFETCSPCImporterFretchen2::on_btnUp_clicked()
{
    int i=ui->lstMultiFile->currentIndex().row();
    if (i>=1 && i<ms_model.rowCount()) {
        //int id=ms_model.data(ms_model.index(i), Qt::UserRole).toInt();
        ms_model.swapItems(i,i-1);
        if (i>=1 && i<ms_outData.size()) ms_outData.swap(i, i-1);
        if (i>=1 && i<ms_outDataFiltered.size()) ms_outDataFiltered.swap(i, i-1);
    }
    updateMultiFile();
}

void QFETCSPCImporterFretchen2::on_btnLeftBG_clicked()
{
    ui->spinBackG->setValue(photons.backgroundrate.value(ui->cmbGreenChannel->currentIndex(), 0));
}

void QFETCSPCImporterFretchen2::on_btnLeftBR_clicked()
{
    ui->spinBackR->setValue(photons.backgroundrate.value(ui->cmbRedChannel->currentIndex(), 0));
}


void QFETCSPCImporterFretchen2::saveData(const QFDataExportTool &outData, const QFDataExportTool &outDataFiltered)
{
    QStringList f=QFDataExportHandler::getFormats();
    QString selFilter;
    QString fn=qfGetSaveFileNameSet("QFETCSPCImporterFretchen2/savetofile/", this, tr("Save burst data to file"), "", f.join(";;"), &selFilter);
    if (fn.size()>0) {
        QFileInfo fi(fn);
        int filterID=f.indexOf(selFilter);

        saveData(outData, fn, filterID);
        saveData(outDataFiltered, fi.absoluteDir().absoluteFilePath(fi.completeBaseName()+".filtered."+fi.suffix()), filterID);
    }
}

void QFETCSPCImporterFretchen2::saveData(const QFDataExportTool &outData, const QString& filename, int filterID)
{
    QFileInfo fi(filename);
    outData.save(filename, /*fi.absoluteDir().absoluteFilePath(fi.completeBaseName()+".full."+fi.suffix()),*/ filterID);

    if (6<outData.data.size()) {
        QFDataExportTool outDataFretchen=outData;
        outDataFretchen.data.swap(5,6);
        while (outDataFretchen.data.size()>7) {
            outDataFretchen.data.removeLast();
        }
        outDataFretchen.colHeaders.clear();
        outDataFretchen.properties.clear();
        outDataFretchen.comment.clear();
        outDataFretchen.rowHeaders.clear();
        QVariantList vl=outDataFretchen.data[6];
        for (int i=0; i<vl.size(); i++) {
            vl[i]=vl[i].toDouble()*1e-3;
        }
        outDataFretchen.data[6]=vl;
        outDataFretchen.saveCSV(fi.absoluteDir().absoluteFilePath(fi.completeBaseName()+".fretchen.txt"), '.', " ", false, '\"', "#!", 7, 15);
    }
}


void QFETCSPCImporterFretchen2::on_btnSaveToFile_clicked()
{
    saveData(outData, outDataFiltered);
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

void QFETCSPCImporterFretchen2::resetEditControls()
{
    setEditControlsEnabled(false);
    setEditControlsEnabled(true);
}

void QFETCSPCImporterFretchen2::updateFromFile()
{
    ui->cmbGreenChannel->clear();
    ui->cmbRedChannel->clear();
    QString filename=ui->edtTCSPCFile->text();
    if (QFile::exists(filename)) {


        QFTCSPCReader* reader=QFTCSPCReader::getImporter(ui->cmbFileformat->currentIndex());
        QApplication::processEvents();
        QApplication::processEvents();
        if (reader) {
            QFTCSPCReaderOpener opener(reader);
            QFuture<bool> opened=QtConcurrent::run(opener,filename, QString());
            QApplication::processEvents();
            while (!opened.isFinished()) {
                QApplication::processEvents();
            }
            QApplication::processEvents();
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
                ui->spinRangeStart->setRange(0, reader->measurementDuration());
                ui->spinRangeEnd->setRange(0, reader->measurementDuration());
                ui->spinRangeEnd->setValue(reader->measurementDuration());
                ui->spinRangeStart->setValue(0);
                ProgramOptions::getConfigQComboBox(ui->cmbGreenChannel, "QFETCSPCImporterFretchen2/cmbGreenChannel", channels.value(0,0));
                ProgramOptions::getConfigQComboBox(ui->cmbRedChannel, "QFETCSPCImporterFretchen2/cmbRedChannel", channels.value(1,1));
                reader->close();
                delete reader;
                reader=NULL;
            }
        }
    }

}

void QFETCSPCImporterFretchen2::updateCTRTrace()
{

    //qDebug()<<ui->tabWidget->isTabEnabled(0)<<ui->tabWidget->isTabEnabled(1)<<ui->tabWidget->isTabEnabled(2);
    if (!ui->tabWidget->isTabEnabled(1)) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    double l=ui->spinDisplayLength->value()*1e-3;
    double s=qMin(ui->spinDisplayStart->value(), ui->spinDisplayStart->maximum()-l);
    ui->spinDisplayStart->setSingleStep(qMax(0.001, l*0.75));
    ui->pltTrace->set_doDrawing(false);
    JKQTPdatastore* ds=ui->pltTrace->getDatastore();
    ds->clear();

    if (ui->cmbBurstSelPlotMode->currentIndex()==0) {

        QVector<double> x, y;
        x.reserve(photons.photondata.size()/1000+10);
        y.reserve(photons.photondata.size()/1000+10);
        //qDebug()<<s<<l<<photons.firstPhoton<<photons.duration<<photons.photondata.size()<<(ui->chkSHowFilteredIPT->isChecked() && photons.lastLEEWindowSize>0);
        if (ui->chkSHowFilteredIPT->isChecked() && photons.lastLEEWindowSize>0) {
            for (int i=0; i<photons.photondata.size(); i++) {
                const double at=photons.photondata.at(i).arrivaltime;
                const double ipt=photons.photondata.at(i).IPT_filtered*1e6;
                //Debug()<<i<<at<<ipt;
                if (at>=s && at<=s+l) {
                    x<<at;
                    y<<ipt;
                    //qDebug()<<i<<"IPT_f ";
                }
                if (i>0 && i%100000==0) QApplication::processEvents();
            }
            plteInterphotonTimes->set_title(tr("all photons, LEE filtered"));
            plteInterphotonTimes->set_drawLine(true);
            plteInterphotonTimes->set_symbol(JKQTPnoSymbol);
            plteInterphotonTimes->set_color(QColor("darkblue"));
        } else {
            for (int i=0; i<photons.photondata.size(); i++) {
                const double at=photons.photondata.at(i).arrivaltime;
                const double ipt=photons.photondata.at(i).IPT*1e6;
                //qDebug()<<i<<at<<ipt;
                if (at>=s && at<=s+l  ) {
                    x<<at;
                    y<<ipt;
                    //qDebug()<<i;
                }
                if (i>0 && i%100000==0) QApplication::processEvents();
            }
            plteInterphotonTimes->set_drawLine(false);
            plteInterphotonTimes->set_title(tr("all photons"));
            plteInterphotonTimes->set_symbol(JKQTPdot);
            plteInterphotonTimes->set_color(QColor("darkblue"));
        }
        //qDebug()<<x.size()<<y.size();
        plteInterphotonTimes->set_xColumn(ds->addCopiedColumn(x, tr("Arrivaltime [s]")));
        plteInterphotonTimes->set_yColumn(ds->addCopiedColumn(y, tr("Inter-Photon Time IPT [microseconds]")));
        plteInterphotonTimes2->set_visible(false);
        plteIPTLevelLine->set_x1(s);
        plteIPTLevelLine->set_x2(s+l);
        plteIPTLevelLine->set_y1(ui->spinMaxIPT->value());
        plteIPTLevelLine->set_y2(ui->spinMaxIPT->value());
        plteIPTLevelLine->set_visible(true);
        ui->pltTrace->getYAxis()->set_axisLabel(tr("interphoton time [{\\mu}s]"));
        ui->pltTrace->getYAxis()->set_logAxis(true);
    } else if (ui->cmbBurstSelPlotMode->currentIndex()==1) {
        plteIPTLevelLine->set_visible(false);
        double bin=ui->spinDisplayBin->value()*1e-3;
        QVector<double> x, yg, yr;
        x.reserve(photons.photondata.size()/1000+10);
        yg.reserve(photons.photondata.size()/1000+10);
        yr.reserve(photons.photondata.size()/1000+10);
        double tn=s;
        double cg=0;
        double cr=0;
        //qDebug()<<s<<l<<photons.firstPhoton<<photons.duration<<photons.photondata.size()<<(ui->chkSHowFilteredIPT->isChecked() && photons.lastLEEWindowSize>0);
        for (int i=0; i<photons.photondata.size(); i++) {
            const double at=photons.photondata.at(i).arrivaltime;
            const int ch=photons.photondata.at(i).channel;
            const int chg=ui->cmbGreenChannel->currentIndex();
            const int chr=ui->cmbRedChannel->currentIndex();

            //Debug()<<i<<at<<ipt;
            if (at>=s && at<=s+l) {
                if (at>=tn+bin) {
                    x<<tn;
                    yg<<(cg/bin*1e-3);
                    yr<<(cr/bin*1e-3);
                    tn=tn+bin;
                    cg=0;
                    cr=0;
                    while (at>=tn+bin) {
                        x<<tn;
                        yg<<0;
                        yr<<0;
                        tn+=bin;
                    }
                } else {
                    if (ch==chg) cg++;
                    else if (ch==chr) cr++;
                }

                //qDebug()<<i<<"IPT_f ";
            }
            if (i>0 && i%100000==0) QApplication::processEvents();
        }
        plteInterphotonTimes->set_title(tr("green photons"));
        plteInterphotonTimes->set_drawLine(true);
        plteInterphotonTimes->set_symbol(JKQTPnoSymbol);
        plteInterphotonTimes->set_color(QColor("darkgreen"));
        plteInterphotonTimes2->set_visible(true);
        plteInterphotonTimes2->set_color(QColor("red"));
        plteInterphotonTimes2->set_title(tr("red photons"));
        plteInterphotonTimes2->set_drawLine(true);
        plteInterphotonTimes2->set_symbol(JKQTPnoSymbol);

        //qDebug()<<x.size()<<y.size();
        plteInterphotonTimes->set_xColumn(ds->addCopiedColumn(x, tr("Arrivaltime [s]")));
        plteInterphotonTimes->set_yColumn(ds->addCopiedColumn(yg, tr("green countrate [kcps]")));
        plteInterphotonTimes2->set_xColumn(plteInterphotonTimes->get_xColumn());
        plteInterphotonTimes2->set_yColumn(ds->addCopiedColumn(yr, tr("red countrate [kcps]")));
        ui->pltTrace->getYAxis()->set_axisLabel(tr("countrate [kcps]"));
        ui->pltTrace->getYAxis()->set_logAxis(false);
    }

    clearRanges();
    ui->pltTrace->zoomToFit();
    if (ui->chkShowBursts->isChecked()) {
        double tmin=ui->pltTrace->getXMin();
        double tmax=ui->pltTrace->getXMax();
        bool first=true;
        for (int i=0; i<bursts.burstdata.size(); i++) {
            double t0=bursts.burstdata[i].start;
            double t1=t0+bursts.burstdata[i].duration;
            QColor colrange("black");
            colrange.setAlphaF(0.5);

            if ((t0>=tmin && t1<=tmax) || (t1>=tmin && t1<=tmax) || (t0>=tmin && t0<=tmax) || (t1>=tmin && t1<=tmax)) {
                plteRanges.append(new JKQTPverticalRange(ui->pltTrace));
                plteRanges.last()->set_fillColor(colrange);
                plteRanges.last()->set_centerColor(colrange);
                plteRanges.last()->set_color(colrange);
                plteRanges.last()->set_plotCenterLine(false);
                plteRanges.last()->set_rangeMin(t0);
                plteRanges.last()->set_rangeMax(t1);
                plteRanges.last()->set_rangeCenter((t0+t1)/2.0);
                plteRanges.last()->set_plotRange(true);
                plteRanges.last()->set_plotRangeLines(false);
                plteRanges.last()->set_invertedRange(false);
                plteRanges.last()->set_unlimitedSizeMin(true);
                plteRanges.last()->set_unlimitedSizeMax(true);
                if (first) {
                    plteRanges.last()->set_title(tr("bursts"));
                } else {
                    plteRanges.last()->set_title(QString());
                }

                ui->pltTrace->addGraph(plteRanges.last());
                first=false;
            }
        }
        for (int i=0; i<plteRanges.size(); i++) {
            ui->pltTrace->moveGraphBottom(plteRanges[i]);
        }
        ui->pltTrace->moveGraphTop(plteInterphotonTimes2);
        ui->pltTrace->moveGraphTop(plteInterphotonTimes);
    }

    ui->pltTrace->set_doDrawing(true);
    ui->pltTrace->update_plot();
    QApplication::restoreOverrideCursor();

}

void QFETCSPCImporterFretchen2::updateAnalysis()
{
    //writeSettings();
    if (!ui->tabWidget->isTabEnabled(2)) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));


    TTTRCalcBurstPropertiesProps p;
    p.backG=ui->spinBackG->value();
    p.backR=ui->spinBackR->value();
    p.crosstalk=ui->spinCrosstalk->value()/100.0;
    p.fdirect=ui->spinFDir->value();
    p.gamma=ui->spinGamma->value();
    TTTRCalcBurstProperties(&bursts, p);

    outData.properties["burstanalysis_background_green_Hz"]=p.backG;
    outData.properties["burstanalysis_background_red_Hz"]=p.backR;
    outData.properties["burstanalysis_crosstalk"]=p.crosstalk;
    outData.properties["burstanalysis_fdirect"]=p.fdirect;
    outData.properties["burstanalysis_gamma"]=p.gamma;

    while (outData.data.size()<8) {
        outData.data.append(QList<QVariant>());
    }
    if (outDataPCol>=0 && outDataPCol<outData.data.size()) {
        outData.data[outDataPCol].clear();
        for (int i=0; i<bursts.burstdata.size(); i++) {
            outData.data[outDataPCol]<<bursts.burstdata[i].P;
        }
    }
    if (outDataECol>=0 && outDataECol<outData.data.size()) {
        outData.data[outDataECol].clear();
        for (int i=0; i<bursts.burstdata.size(); i++) {
            outData.data[outDataECol]<<bursts.burstdata[i].E;
        }
    }

    updateAnalysisPlots();

    QApplication::restoreOverrideCursor();
}


void QFETCSPCImporterFretchen2::updateAnalysisPlots()
{
    //writeSettings();
    if (!ui->tabWidget->isTabEnabled(2)) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->pltBurstRate->set_doDrawing(false);
    ui->pltProximity->set_doDrawing(false);
    JKQTPdatastore* dsB=ui->pltBurstRate->getDatastore();
    JKQTPdatastore* dsP=ui->pltProximity->getDatastore();
    dsB->clear();
    dsP->clear();

    QVector<double> PVec, EVec, PVecAll, RVecAll, EVecAll, DVec, RVec, SAllVec, DAllVec, RRVec, RGVec, SVec;
    const double mir=ui->spinMinRate->value()*1e3;
    const double mar=ui->spinMaxRate->value()*1e3;

    outDataFiltered=outData;
    outDataFiltered.properties["burstfiltered"]=true;
    outDataFiltered.properties["burstfiltered_minimum_rate_kHz"]=mir;
    outDataFiltered.properties["burstfiltered_minimum_rate_kHz"]=mar;


    while (outDataFiltered.data.size()<8) outDataFiltered.data.append(QList<QVariant>());
    for (int i=0; i<outDataFiltered.data.size(); i++) outDataFiltered.data[i].clear();

    for (int i=0; i<bursts.burstdata.size(); i++) {
        const double r=bursts.burstdata[i].getRate();
        SAllVec<<(bursts.burstdata[i].photonG+bursts.burstdata[i].photonR);
        DAllVec<<(bursts.burstdata[i].duration*1e3);
        PVecAll<<bursts.burstdata[i].P;
        EVecAll<<bursts.burstdata[i].E;
        RVecAll<<(r*1e-3);
        if (r>=mir && r<=mar) {
            RVec<<(r*1e-3);
            DVec<<(bursts.burstdata[i].duration*1e3);
            SVec<<(bursts.burstdata[i].photonG+bursts.burstdata[i].photonR);
            PVec<<bursts.burstdata[i].P;
            EVec<<bursts.burstdata[i].E;
            RGVec<<(double(bursts.burstdata[i].photonG)/bursts.burstdata[i].duration);
            RRVec<<(double(bursts.burstdata[i].photonR)/bursts.burstdata[i].duration);
            outDataFiltered.data[0]<<i;
            outDataFiltered.data[1]<<bursts.burstdata.at(i).start;
            outDataFiltered.data[2]<<bursts.burstdata.at(i).duration;
            outDataFiltered.data[3]<<bursts.burstdata.at(i).photonG;
            outDataFiltered.data[4]<<bursts.burstdata.at(i).photonR;
            outDataFiltered.data[5]<<bursts.burstdata.at(i).getRate();
            outDataFiltered.data[6]<<bursts.burstdata.at(i).P;
            outDataFiltered.data[7]<<bursts.burstdata.at(i).E;
        }
    }

    ui->labNBursts->setText(QString::number(PVec.size()));
    ui->labAvgBDuration->setText(QString("%1 ms").arg(qfstatisticsAverage(DVec), 0, 'f', 4));
    ui->labAvgRate->setText(QString("%1 kHz").arg(qfstatisticsAverage(RVec), 0, 'f', 3));
    ui->labAvgRateG->setText(QString("%1 kHz").arg(qfstatisticsAverage(RGVec)*1e-3, 0, 'f', 3));
    ui->labAvgRateR->setText(QString("%1 kHz").arg(qfstatisticsAverage(RRVec)*1e-3, 0, 'f', 3));
    ui->labAvgP->setText(QString("%1").arg(qfstatisticsAverage(PVec), 0, 'f', 4));
    ui->labAvgE->setText(QString("%1").arg(qfstatisticsAverage(EVec), 0, 'f', 4));

    int nbins=1;
    double binw=ui->spinHBinWidth->value();
    double dmin=0, dmax=0;

    {
        nbins=1;
        dmin=dmax=0;
        calcHistParams(PVecAll, dmin,dmax,nbins,binw);
        outData.properties["burstfiltered_histogram_P_min"]=dmin;
        outData.properties["burstfiltered_histogram_P_max"]=dmax;
        outData.properties["burstfiltered_histogram_P_bins"]=nbins;
        outData.properties["burstfiltered_histogram_P_binwidth"]=binw;
    }
    {
        nbins=1;
        dmin=dmax=0;
        calcHistParams(EVecAll, dmin,dmax,nbins,binw);
        outData.properties["burstfiltered_histogram_E_min"]=dmin;
        outData.properties["burstfiltered_histogram_E_max"]=dmax;
        outData.properties["burstfiltered_histogram_E_bins"]=nbins;
        outData.properties["burstfiltered_histogram_E_binwidth"]=binw;
    }


    {
        nbins=1;
        dmin=dmax=0;
        calcHistParams(PVec, dmin,dmax,nbins,binw);
        outDataFiltered.properties["burstfiltered_histogram_P_min"]=dmin;
        outDataFiltered.properties["burstfiltered_histogram_P_max"]=dmax;
        outDataFiltered.properties["burstfiltered_histogram_P_bins"]=nbins;
        outDataFiltered.properties["burstfiltered_histogram_P_binwidth"]=binw;
    }
    {
        nbins=1;
        dmin=dmax=0;
        calcHistParams(EVec, dmin,dmax,nbins,binw);
        outDataFiltered.properties["burstfiltered_histogram_E_min"]=dmin;
        outDataFiltered.properties["burstfiltered_histogram_E_max"]=dmax;
        outDataFiltered.properties["burstfiltered_histogram_E_bins"]=nbins;
        outDataFiltered.properties["burstfiltered_histogram_E_binwidth"]=binw;
    }

    nbins=1;
    dmin=dmax=0;

    ui->spinHBins->setEnabled(ui->cmbAnaHistMode->currentIndex()>1);
    ui->labHBins->setEnabled(ui->cmbAnaHistMode->currentIndex()>1);
    ui->spinHBinWidth->setEnabled(ui->cmbAnaHistMode->currentIndex()<=1);
    ui->labHBinWidth->setEnabled(ui->cmbAnaHistMode->currentIndex()<=1);
    ui->labHistParams->setVisible(ui->cmbAnaHistMode->currentIndex()<=1);

    if (ui->cmbAnaHistMode->currentIndex()==0) {
        calcHistParams(PVec, dmin,dmax,nbins,binw);
        QVector<double> HX(nbins,0.0);
        QVector<double> HY(nbins,0.0);
        statisticsHistogramRanged(PVec.data(), PVec.size(), dmin, dmax, HX.data(), HY.data(), nbins, ui->chkNormHistogram->isChecked());

        plteProximity->set_xColumn(dsP->addCopiedColumn(HX, tr("P histogram: P")));
        plteProximity->set_yColumn(dsP->addCopiedColumn(HY, tr("P histogram: frequency")));
        ui->pltProximity->getXAxis()->set_axisLabel(tr("proximity ratio P"));
    } else if (ui->cmbAnaHistMode->currentIndex()==1) {
        calcHistParams(EVec, dmin,dmax,nbins,binw);
        QVector<double> HX(nbins,0.0);
        QVector<double> HY(nbins,0.0);
        statisticsHistogramRanged(EVec.data(), EVec.size(), dmin, dmax, HX.data(), HY.data(), nbins, ui->chkNormHistogram->isChecked());

        plteProximity->set_xColumn(dsP->addCopiedColumn(HX, tr("E histogram: E")));
        plteProximity->set_yColumn(dsP->addCopiedColumn(HY, tr("E histogram: frequency")));
        ui->pltProximity->getXAxis()->set_axisLabel(tr("FRET ratio E"));
    } else if (ui->cmbAnaHistMode->currentIndex()==2) {
        nbins=ui->spinHBins->value();
        QVector<double> HX(nbins,0.0);
        QVector<double> HY(nbins,0.0);
        statisticsHistogram(DVec.data(), DVec.size(),HX.data(), HY.data(), nbins, ui->chkNormHistogram->isChecked());

        plteProximity->set_xColumn(dsP->addCopiedColumn(HX, tr("Burst duration histogram: duration [ms]")));
        plteProximity->set_yColumn(dsP->addCopiedColumn(HY, tr("Burst duration histogram: frequency")));
        ui->pltProximity->getXAxis()->set_axisLabel(tr("Burst duration \\Delta t_{burst} [ms]"));
    } else if (ui->cmbAnaHistMode->currentIndex()==3) {
        nbins=ui->spinHBins->value();
        QVector<double> HX(nbins,0.0);
        QVector<double> HY(nbins,0.0);
        statisticsHistogram(SVec.data(), SVec.size(),HX.data(), HY.data(), nbins, ui->chkNormHistogram->isChecked());

        plteProximity->set_xColumn(dsP->addCopiedColumn(HX, tr("Burst size histogram: size [photons]")));
        plteProximity->set_yColumn(dsP->addCopiedColumn(HY, tr("Burst size histogram: frequency")));
        ui->pltProximity->getXAxis()->set_axisLabel(tr("Burst size N_g+N_r [photons]"));
    } else if (ui->cmbAnaHistMode->currentIndex()==4) {
        nbins=ui->spinHBins->value();
        QVector<double> HX(nbins,0.0);
        QVector<double> HY(nbins,0.0);
        statisticsHistogram(RVec.data(), RVec.size(),HX.data(), HY.data(), nbins, ui->chkNormHistogram->isChecked());

        plteProximity->set_xColumn(dsP->addCopiedColumn(HX, tr("Burst rate histogram: rate [kcps]")));
        plteProximity->set_yColumn(dsP->addCopiedColumn(HY, tr("Burst rate histogram: frequency")));
        ui->pltProximity->getXAxis()->set_axisLabel(tr("Burst rate (N_g+N_r)/\\Delta t_{burst} [kcps]"));
    }

    ui->labHistParams->setText(tr("%3 bins in [%1..%2]").arg(dmin,0,'f',4).arg(dmax,0,'f',4).arg(nbins));

    if (ui->cmbAnaCorrMode->currentIndex()==0) {
        plteBurstRate->set_xColumn(dsB->addCopiedColumn(SAllVec, tr("burst size [photons]")));
        plteBurstRate->set_yColumn(dsB->addCopiedColumn(DAllVec, tr("burst duration [ms]")));

        plteMinBurstRate->set_paramsV(0, 1e3/mir);
        plteMaxBurstRate->set_paramsV(0, 1e3/mar);
        plteMaxBurstRate->set_visible(true);
        plteMinBurstRate->set_visible(true);
        ui->pltBurstRate->getXAxis()->set_axisLabel(tr("burst size N_g+N_r [photons]"));
        ui->pltBurstRate->getYAxis()->set_axisLabel(tr("burst duration \\Delta t_{burst} [ms]"));

    } else if (ui->cmbAnaCorrMode->currentIndex()==1) {
        plteBurstRate->set_xColumn(dsB->addCopiedColumn(SVec, tr("burst size [photons]")));
        plteBurstRate->set_yColumn(dsB->addCopiedColumn(PVec, tr("proximity ratio")));

        plteMaxBurstRate->set_visible(false);
        plteMinBurstRate->set_visible(false);
        ui->pltBurstRate->getXAxis()->set_axisLabel(tr("burst size N_g+N_r [photons]"));
        ui->pltBurstRate->getYAxis()->set_axisLabel(tr("proximity ratio P"));
    } else if (ui->cmbAnaCorrMode->currentIndex()==2) {
        plteBurstRate->set_xColumn(dsB->addCopiedColumn(DVec, tr("burst duration [ms]")));
        plteBurstRate->set_yColumn(dsB->addCopiedColumn(PVec, tr("proximity ratio")));

        plteMaxBurstRate->set_visible(false);
        plteMinBurstRate->set_visible(false);
        ui->pltBurstRate->getXAxis()->set_axisLabel(tr("burst duration \\Delta t_{burst} [ms]"));
        ui->pltBurstRate->getYAxis()->set_axisLabel(tr("proximity ratio P"));
    } else if (ui->cmbAnaCorrMode->currentIndex()==3) {
        plteBurstRate->set_xColumn(dsB->addCopiedColumn(RVec, tr("burst rate [kcps]")));
        plteBurstRate->set_yColumn(dsB->addCopiedColumn(PVec, tr("proximity ratio")));

        plteMaxBurstRate->set_visible(false);
        plteMinBurstRate->set_visible(false);
        ui->pltBurstRate->getXAxis()->set_axisLabel(tr("burst rate (N_g+N_r)/\\Delta t_{burst} [kcps]"));
        ui->pltBurstRate->getYAxis()->set_axisLabel(tr("proximity ratio P"));

    }

    if (ui->cmbColorCol->currentIndex()==0) {
        plteBurstRate->set_colorBarRightVisible(false);
        plteBurstRate->set_colorBarTopVisible(false);
        plteBurstRate->set_colorColumn(-1);
    } else if (ui->cmbColorCol->currentIndex()==1) {
        plteBurstRate->set_colorBarRightVisible(true);
        plteBurstRate->set_colorBarTopVisible(false);
        plteBurstRate->get_colorBarRightAxis()->set_axisLabel(tr("proximity ratio P"));
        if (ui->cmbAnaCorrMode->currentIndex()==0)  plteBurstRate->set_colorColumn(dsB->addCopiedColumn(PVecAll, tr("proximity ratio")));
        else plteBurstRate->set_colorColumn(dsB->addCopiedColumn(PVec, tr("proximity ratio")));
    } else if (ui->cmbColorCol->currentIndex()==2) {
        plteBurstRate->set_colorBarRightVisible(true);
        plteBurstRate->set_colorBarTopVisible(false);
        plteBurstRate->get_colorBarRightAxis()->set_axisLabel(tr("burst size N_g+N_r [photons]"));
        if (ui->cmbAnaCorrMode->currentIndex()==0)  plteBurstRate->set_colorColumn(dsB->addCopiedColumn(SAllVec, tr("burst size [photons]")));
        else plteBurstRate->set_colorColumn(dsB->addCopiedColumn(SVec, tr("burst size [photons]")));
    } else if (ui->cmbColorCol->currentIndex()==3) {
        plteBurstRate->set_colorBarRightVisible(true);
        plteBurstRate->set_colorBarTopVisible(false);
        plteBurstRate->get_colorBarRightAxis()->set_axisLabel(tr("burst duration \\Delta t_{Burst} [ms]"));
        if (ui->cmbAnaCorrMode->currentIndex()==0)  plteBurstRate->set_colorColumn(dsB->addCopiedColumn(DAllVec, tr("burst duration [ms]")));
        else plteBurstRate->set_colorColumn(dsB->addCopiedColumn(DVec, tr("burst duration [ms]")));
    } else if (ui->cmbColorCol->currentIndex()==4) {
        plteBurstRate->set_colorBarRightVisible(true);
        plteBurstRate->set_colorBarTopVisible(false);
        plteBurstRate->get_colorBarRightAxis()->set_axisLabel(tr("burst rate (N_g+N_r)/\\Delta t_{Burst} [kcps]"));
        if (ui->cmbAnaCorrMode->currentIndex()==0)  plteBurstRate->set_colorColumn(dsB->addCopiedColumn(RVecAll, tr("burst rate [kcps]")));
        else plteBurstRate->set_colorColumn(dsB->addCopiedColumn(RVec, tr("burst rate [kcps]")));
    }

    if (ui->cmbAnaCorrMode->currentIndex()==0) {
        ui->pltBurstRate->zoomToFit(true, true, true, true);
    } else {
        ui->pltBurstRate->zoomToFit();
    }
    ui->pltProximity->zoomToFit();
    ui->pltBurstRate->set_doDrawing(true);
    ui->pltProximity->set_doDrawing(true);
    ui->pltBurstRate->update_plot();
    ui->pltProximity->update_plot();
    QApplication::restoreOverrideCursor();
}

void QFETCSPCImporterFretchen2::updateMultiFile()
{
    bool vis=ms_model.rowCount()>0;
    ui->tabMultiFile->setVisible(vis);
    if (vis && ui->layTabs->indexOf(ui->tabMultiFile)<0) {
        ui->layTabs->insertWidget(1,ui->tabMultiFile,1);
        ui->layTabs->setStretch(0,4);
    }
    if (!vis && ui->layTabs->indexOf(ui->tabMultiFile)>=0) {
        ui->layTabs->removeWidget(ui->tabMultiFile);
    }
}

void QFETCSPCImporterFretchen2::calcHistParams(const QVector<double> &PVec, double &dmin, double &dmax, int &nbins, double binw)
{
    dmin=0;
    dmax=0;
    statisticsMinMax(PVec.data(), PVec.size(), dmin, dmax);
    if (dmax<1) dmax=1;
    if (dmin>0) dmin=0;
    if (dmin<0) dmin=floor(dmin/binw)*binw;
    else dmin=ceil(dmin/binw)*binw;
    dmax=ceil(dmax/binw)*binw;
    nbins=ceil(fabs(dmax-dmin)/binw);
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
    writeSettings();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QModernProgressDialog progress(this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setModal(true);

    progress.setHasCancel(false);
    progress.setLabelText(tr("performing burst selection ..."));
    progress.setMode(true, false);
    progress.show();
    progress.raise();
    outData.properties["lee_filtered"]=false;
    if (ui->chkLEE->isChecked()) {
        QFETCSPCImporterFretchen2LEEFilterer exe(&photons);
        QFuture<void> opened=QtConcurrent::run(exe, ui->spinLEERange->value(), ui->spinLEESigma->value()*1e-3);
        progress.setLabelText(tr("applying LEE filter ..."));
        outData.properties["lee_filteres"]=true;
        //photons.applyLEEFilter(ui->spinLEERange->value(), ui->spinLEESigma->value()*1e-3);
        while (!opened.isFinished()) {
            QApplication::processEvents();
        }
    }
    if (photons.lastLEEWindowSize>0) {
        ui->labLEEWindow->setText(QString::number(photons.lastLEEWindowSize));
        ui->labLEESigma0->setText(QString("%1 ms").arg(photons.lastLEESigma0*1e3, 0, 'f', 3));
        outData.properties["lee_filtered"]=true;
        outData.properties["lee_windowsize"]=photons.lastLEEWindowSize;
        outData.properties["lee_sigma0"]=photons.lastLEESigma0;
    } else {
        outData.properties["lee_filtered"]=false;
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
    outData.properties["burstselect_max_burst_duration_ms"]=props.maxBurstDuration;
    outData.properties["burstselect_max_ipt_microseconds"]=props.maxIPT;
    outData.properties["burstselect_min_burst_size_photons"]=props.minBurstSize;
    outData.properties["lee_filtered"]=props.filtered;
    QFuture<void> opened=QtConcurrent::run(burstFinder, props);
    while (!opened.isFinished()) {
        QApplication::processEvents();
    }
    bursts=b;
    outData.properties["burstselect_burstcount"]=bursts.burstdata.size();
    outData.properties["burstselect_avg_burst_duration_ms"]=bursts.avgDuration*1e3;

    ui->labBursts->setText(tr("count: %1,&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;avg. duration: %2 ms").arg(bursts.burstdata.size()).arg(bursts.avgDuration*1e3,0,'f',3));

    outData.colHeaders.clear();
    for (int i=0; i<8; i++) {
        if (i<outData.data.size()) {
            outData.data[i].clear();
        }
    }
    while (outData.data.size()<8) {
        outData.data.append(QList<QVariant>());
    }
    outData.colHeaders<<tr("burst");
    outData.colHeaders<<tr("start [s]");
    outData.colHeaders<<tr("duration [s]");
    outData.colHeaders<<tr("photons_green");
    outData.colHeaders<<tr("photons_red");
    outData.colHeaders<<tr("avg_countrate [cps]");
    outData.colHeaders<<tr("Proximity_Ratio");
    outData.colHeaders<<tr("FRET_Ratio");
    outDataPCol=6;
    outDataECol=7;
    for (int i=0; i<bursts.burstdata.size(); i++) {
        outData.data[0]<<i;
        outData.data[1]<<bursts.burstdata.at(i).start;
        outData.data[2]<<bursts.burstdata.at(i).duration;
        outData.data[3]<<bursts.burstdata.at(i).photonG;
        outData.data[4]<<bursts.burstdata.at(i).photonR;
        outData.data[5]<<bursts.burstdata.at(i).getRate();
        //outData.data[6]<<P;
        //outData.data[7]<<E;
    }

    updateCTRTrace();
    updateAnalysis();
    QApplication::restoreOverrideCursor();
}

void QFETCSPCImporterFretchen2::loadTCSPCFiles()
{
    writeSettings();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString filename=ui->edtTCSPCFile->text();
    QString filenameB=ui->edtTCSPCBackground->text();
    writeSettings();
    if (QFile::exists(filename)) {
        setEditControlsEnabled(true);
        outData.clear();
        outDataFiltered.clear();
        outDataECol=-1;
        outDataPCol=-1;
        outData.properties["input_file"]=filename;
        outData.properties["background_file"]=filenameB;


        QModernProgressDialog progress(this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setModal(true);

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

            outData.properties["range_start"]=qMax(0.0, props.start);
            outData.properties["range_end"]=(props.end>=0)?props.end:reader->measurementDuration();
            outData.properties["channel_green"]=props.chG;
            outData.properties["channel_red"]=props.chR;
            outData.properties["first_photon"]=photons.firstPhoton;
            outData.properties["duration"]=photons.duration;
            if (readerBack) {
                outData.properties["background_duration"]=readerBack->measurementDuration();
            }

            ui->spinDisplayStart->setMaximum(photons.duration+photons.firstPhoton);
            ui->spinDisplayStart->setMinimum(photons.firstPhoton);
            ui->spinDisplayStart->setValue(0);
            ui->labDuration->setText(QString("%1 s").arg(photons.duration, 0, 'f', 2));
            ui->labP->setText(QString("%1").arg(photons.photondata.size()));
            outData.properties["photons"]=photons.photondata.size();
            ui->labPGreen->setText(QString("%1").arg(photons.photonsG));
            outData.properties["photons_green"]=quint64(photons.photonsG);
            ui->labPRed->setText(QString("%1").arg(photons.photonsR));
            outData.properties["photons_red"]=quint64(photons.photonsR);
            ui->labAvgIPT->setText(QString("%1 &mu;s").arg(photons.avg_IPT, 0, 'f', 3));
            outData.properties["avg_ipt_all"]=photons.avg_IPT;

            ui->labCGreen->setText(QString("%1 (B: %2) kHz").arg(photons.avgrate.value(ui->cmbGreenChannel->currentIndex())*1e-3, 0, 'f', 3).arg(photons.backgroundrate.value(ui->cmbGreenChannel->currentIndex())*1e-3, 0, 'f', 3));
            outData.properties["countrate_green_avg_fromfile"]=photons.avgrate.value(ui->cmbGreenChannel->currentIndex());
            outData.properties["background_green_avg_fromfile"]=photons.backgroundrate.value(ui->cmbGreenChannel->currentIndex());
            ui->labCRed->setText(QString("%1 (B: %2) kHz").arg(photons.avgrate.value(ui->cmbRedChannel->currentIndex())*1e-3, 0, 'f', 3).arg(photons.backgroundrate.value(ui->cmbRedChannel->currentIndex())*1e-3, 0, 'f', 3));
            outData.properties["countrate_red_avg_fromfile"]=photons.avgrate.value(ui->cmbRedChannel->currentIndex());
            outData.properties["background_red_avg_fromfile"]=photons.backgroundrate.value(ui->cmbRedChannel->currentIndex());
            ui->btnLeftBG->setText(QString("%1 Hz").arg(photons.backgroundrate.value(ui->cmbGreenChannel->currentIndex()), 0, 'f', 3));
            ui->btnLeftBR->setText(QString("%1 Hz").arg(photons.backgroundrate.value(ui->cmbRedChannel->currentIndex()), 0, 'f', 3));
            ui->spinBackG->setValue(photons.backgroundrate.value(ui->cmbGreenChannel->currentIndex(),0));
            ui->spinBackR->setValue(photons.backgroundrate.value(ui->cmbRedChannel->currentIndex(),0));


            setEditControlsEnabled(true);
            ui->tabBurstSelection->setEnabled(true);
            ui->tabWidget->setCurrentIndex(1);
            ui->tabWidget->setTabEnabled(1, true);
            updateCTRTrace();
        }
        reader->close();
        delete reader;
        reader=NULL;
        if (readerBack) {
            readerBack->close();
            delete readerBack;
            readerBack=NULL;
        }
    } else {
        setEditControlsEnabled(false);
        QMessageBox::critical(this, tr("TCSPC Burst Analyzer"), tr("The file '%1' does not exist.\nPlease select an existing file!").arg(filename));
    }

    QApplication::restoreOverrideCursor();
}

