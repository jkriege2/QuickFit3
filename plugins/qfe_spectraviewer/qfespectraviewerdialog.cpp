#include "qfespectraviewerdialog.h"
#include "ui_qfespectraviewerdialog.h"
#include "qfespectraviewer.h"
#include "qftools.h"
#include "spectrummanager.h"
#include "jkautooutputtimer.h"

QFESpectraViewerDialog::QFESpectraViewerDialog(QFESpectraViewer *plugin, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFESpectraViewerDialog)
{
    currentIndex=-1;
    this->plugin=plugin;
    manager=plugin->getSpectrumManager();

    ui->setupUi(this);

    //qDebug()<<"loading fluorophores";
    QStringList sl=manager->getFluorophores();
    ui->cmbFluorophore->clear();
    for (int i=0; i<sl.size(); i++) {
        if (manager->fluorophoreExists(sl[i])) {
            qDebug()<<sl[i]<<manager->getFluorophoreData(sl[i]).name;
            ui->cmbFluorophore->addItem(manager->getFluorophoreData(sl[i]).name, sl[i]);
            //qDebug()<<sl[i]<<manager->getFluorophoreData(sl[i]).name;
            //ui->cmbFluorophore->addItem(sl[i], sl[i]);
        }
    }

    //qDebug()<<"setting model";
    //ui->tabFluorophoreInfo->setModel(&modFluorophoreData);


    //qDebug()<<"read settings";
    readSettings();
    //qDebug()<<"constructor done";
}

QFESpectraViewerDialog::~QFESpectraViewerDialog()
{
    writeSettings();
    delete ui;
}

void QFESpectraViewerDialog::loadSpectraConfig(QSettings &settings, const QString &prefix)
{
    JKAutoOutputTimer tim("loadSpectraConfig");
    plotItems.clear();

    disconnect(ui->lstSpectra, SIGNAL(currentRowChanged(int)), this, SLOT(spectrumSelected()));
    ui->lstSpectra->clear();
    int count=settings.value(prefix+QString("count"), 0).toInt();
    for (int i=0; i<count; i++) {
        QFESpectraViewerPlotItem it;
        it.type=(QFESpectraViewerPlotItemType)settings.value(prefix+QString("spectrum%1/type").arg(i), qfesFluorohpore).toInt();
        it.name=settings.value(prefix+QString("spectrum%1/name").arg(i), "").toString();
        it.showExcitation=settings.value(prefix+QString("spectrum%1/showexcitation").arg(i), true).toBool();
        it.showEmission=settings.value(prefix+QString("spectrum%1/showemission").arg(i), true).toBool();
        plotItems<<it;
        QListWidgetItem* item=new QListWidgetItem(it.name, ui->lstSpectra);
        //ui->lstSpectra->addItem(item);
    }
    connect(ui->lstSpectra, SIGNAL(currentRowChanged(int)), this, SLOT(spectrumSelected()));
    ui->lstSpectra->setCurrentRow(0);
    spectrumSelected();
    updatePlots();
}

void QFESpectraViewerDialog::saveSpectraConfig(QSettings &settings, const QString &prefix)
{
    JKAutoOutputTimer tim("saveSpectraConfig");
    settings.setValue(prefix+QString("count"), plotItems.size());
    for (int i=0; i<plotItems.size(); i++) {
        settings.setValue(prefix+QString("spectrum%1/type").arg(i), (int)plotItems[i].type);
        settings.setValue(prefix+QString("spectrum%1/name").arg(i), plotItems[i].name);
        settings.setValue(prefix+QString("spectrum%1/showexcitation").arg(i), plotItems[i].showExcitation);
        settings.setValue(prefix+QString("spectrum%1/showemission").arg(i), plotItems[i].showEmission);
    }
}

void QFESpectraViewerDialog::on_btnAddFluorophore_clicked()
{
    JKAutoOutputTimer tim("on_btnAddFluorophore_clicked");
    QFESpectraViewerPlotItem it;
    it.type=qfesFluorohpore;
    it.showEmission=true;
    it.showExcitation=true;
    it.name="";
    plotItems.append(it);
    QListWidgetItem* item=new QListWidgetItem(it.name, ui->lstSpectra);
    //item->setData(Qt::UserRole, plotItems.size()-1);
    //ui->lstSpectra->addItem(item);
    ui->lstSpectra->setCurrentItem(item);
    spectrumSelected();
}

void QFESpectraViewerDialog::on_btnDelete_clicked()
{
    JKAutoOutputTimer tim("on_btnDelete_clicked");
    if (currentIndex<0 || currentIndex>=plotItems.size()) return;
    int delIdx=currentIndex;
    ui->lstSpectra->setCurrentRow(-1);
    ui->lstSpectra->removeItemWidget(ui->lstSpectra->item(delIdx));
    plotItems.removeAt(delIdx);
    if (plotItems.size()<=0) currentIndex=0;
    else ui->lstSpectra->setCurrentRow(0);
}

void QFESpectraViewerDialog::spectrumSelected()
{
    int idx=ui->lstSpectra->currentRow();
    JKAutoOutputTimer tim(QString("spectrumSelected idx=%1 currentIdx=%2").arg(idx).arg(currentIndex));
    if (currentIndex==idx) return;

    saveFromWidgets();

    currentIndex=idx;
    if (idx<0) currentIndex=-1;

    loadToWidgets();
}

void QFESpectraViewerDialog::saveFromWidgets()
{
    JKAutoOutputTimer tim("saveFromWidgets");
    if (currentIndex<0 || currentIndex>=plotItems.size()) return;
    if (ui->stackSpectraEditor->currentWidget()==ui->widFluorophore)  {
        plotItems[currentIndex].type=qfesFluorohpore;
        plotItems[currentIndex].name=ui->cmbFluorophore->itemData(ui->cmbFluorophore->currentIndex()).toString();
        plotItems[currentIndex].showEmission=ui->chkFluorophoreShowEmission->isChecked();
        plotItems[currentIndex].showExcitation=ui->chkFluorophoreShowExcitation->isChecked();
        if (ui->lstSpectra->item(currentIndex)) ui->lstSpectra->item(currentIndex)->setText(ui->cmbFluorophore->currentText());
    }
    updatePlots();
}

void QFESpectraViewerDialog::loadToWidgets()
{
    JKAutoOutputTimer tim("loadToWidgets");
    disconnect(ui->chkFluorophoreShowEmission, SIGNAL(toggled(bool)), this, SLOT(saveFromWidgets()));
    disconnect(ui->chkFluorophoreShowExcitation, SIGNAL(toggled(bool)), this, SLOT(saveFromWidgets()));
    disconnect(ui->cmbFluorophore, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
    if (currentIndex<0 || currentIndex>=plotItems.size()) {
        ui->stackSpectraEditor->setCurrentWidget(ui->widEmpty);
    } else {
        if (plotItems[currentIndex].type==qfesFluorohpore)  {
            ui->stackSpectraEditor->setCurrentWidget(ui->widFluorophore);
            ui->cmbFluorophore->setCurrentIndex(ui->cmbFluorophore->findData(plotItems[currentIndex].name));
            ui->chkFluorophoreShowEmission->setChecked(plotItems[currentIndex].showEmission);
            ui->chkFluorophoreShowExcitation->setChecked(plotItems[currentIndex].showExcitation);
            connect(ui->chkFluorophoreShowEmission, SIGNAL(toggled(bool)), this, SLOT(saveFromWidgets()));
            connect(ui->chkFluorophoreShowExcitation, SIGNAL(toggled(bool)), this, SLOT(saveFromWidgets()));
            connect(ui->cmbFluorophore, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
            modFluorophoreData.clear();
            modFluorophoreData.disableSignals();
            if (manager->fluorophoreExists(plotItems[currentIndex].name)) {
                SpectrumManager::FluorophorData fl=manager->getFluorophoreData(plotItems[currentIndex].name);
                int row=0;
                modFluorophoreData.setCellCreate(row,0, tr("name"));
                modFluorophoreData.setCellCreate(row,1, fl.name);
                row++;
                modFluorophoreData.setCellCreate(row,0, tr("description"));
                modFluorophoreData.setCellCreate(row,1, fl.description);
                row++;
                modFluorophoreData.setCellCreate(row,0, tr("manufacturer"));
                modFluorophoreData.setCellCreate(row,1, fl.manufacturer);
                row++;
                modFluorophoreData.setCellCreate(row,0, tr("reference"));
                modFluorophoreData.setCellCreate(row,1, fl.reference);
                row++;
                modFluorophoreData.setCellCreate(row,0, tr("conditions"));
                modFluorophoreData.setCellCreate(row,1, fl.condition);
                row++;
                modFluorophoreData.setCellCreate(row,0, tr("fl. efficiency [%]"));
                modFluorophoreData.setCellCreate(row,1, fl.fluorescence_efficiency*100.0);
                row++;
                modFluorophoreData.setCellCreate(row,0, tr("fl. lifetime [ns]"));
                modFluorophoreData.setCellCreate(row,1, fl.fluorescence_lifetime*1e9);
                row++;
                modFluorophoreData.setCellCreate(row,0, tr("extinction coefficient [1/(cm M)]"));
                modFluorophoreData.setCellCreate(row,1, tr("%1 @ %2nm").arg(fl.extiction_coefficient).arg(fl.extiction_coefficient_wavelength));
                row++;
                modFluorophoreData.setCellCreate(row,0, tr("abs. crosssection [cm²]"));
                modFluorophoreData.setCellCreate(row,1, tr("%1 @ %2nm").arg(fl.extiction_coefficient*3.82e-21).arg(fl.extiction_coefficient_wavelength));
                row++;
            }
            modFluorophoreData.enableSignals(true);

        } else {
            ui->stackSpectraEditor->setCurrentWidget(ui->widEmpty);
        }
    }
}

void QFESpectraViewerDialog::writeSettings() {
    JKAutoOutputTimer tim("writeSettings");
    ProgramOptions* opt=ProgramOptions::getInstance();
    if (opt) {
        QSettings* set=opt->getQSettings();
        if (set) {
            saveWidgetGeometry(*set, this, plugin->getID()+"/widget/");
            //saveSplitter(*set, ui->splitter, plugin->getID()+"/splitter/");
            saveSpectraConfig(*set,plugin->getID()+QString("/lastSpectra/") );

        }
    }
}

void QFESpectraViewerDialog::readSettings() {
    JKAutoOutputTimer tim("readSettings");
    ProgramOptions* opt=ProgramOptions::getInstance();
    if (opt) {
        QSettings* set=opt->getQSettings();
        if (set) {
            loadWidgetGeometry(*set, this, plugin->getID()+"/widget/");
            //loadSplitter(*set, ui->splitter, plugin->getID()+"/splitter/");
            loadSpectraConfig(*set,plugin->getID()+QString("/lastSpectra/") );


        }
    }
}

void QFESpectraViewerDialog::updatePlots()
{
    JKAutoOutputTimer tim("updatePlots");
    ui->plotter->set_doDrawing(false);

    JKQTPdatastore* ds=ui->plotter->getDatastore();
    ui->plotter->clearGraphs(true);
    ds->clear();

    ui->plotter->getXAxis()->set_axisLabel(tr("wavelength \\lambda [nm]"));
    ui->plotter->getYAxis()->set_axisLabel(tr("relative intensity/absorbtion [%]"));

    for (int i=0; i<plotItems.size(); i++) {
        QFESpectraViewerPlotItem item=plotItems[i];
        if (item.type==qfesFluorohpore && manager->fluorophoreExists(item.name)) {
            SpectrumManager::FluorophorData fl=manager->getFluorophoreData(item.name);
            if (manager->spectrumExists(fl.spectrum_abs))  {
                JKQTPxyLineGraph* g=new JKQTPxyLineGraph(ui->plotter->get_plotter());
                SpectrumManager::Spectrum* spec=manager->getSpectrum(fl.spectrum_abs);
                //spec->ensureSpectrum();
                size_t cWL=ds->addCopiedColumn(spec->getWavelength(), spec->getN(), tr("%1_abs_wavelength").arg(item.name));
                size_t cSP=ds->addCopiedColumn(spec->getSpectrum(), spec->getN(), tr("%1_abs_spectrum").arg(item.name));
                g->set_symbol(JKQTPnoSymbol);
                g->set_drawLine(true);
                g->set_lineWidth(1);
                g->set_style(Qt::DashLine);
                g->set_xColumn(cWL);
                g->set_yColumn(cSP);
                g->set_color(wavelengthToColor(spec->getSpectrumMaxWavelength()));
                g->set_title(tr("abs: %1").arg(fl.name));
                ui->plotter->addGraph(g);
            }
            if (manager->spectrumExists(fl.spectrum_fl))  {
                JKQTPxyLineGraph* g=new JKQTPxyLineGraph(ui->plotter->get_plotter());
                SpectrumManager::Spectrum* spec=manager->getSpectrum(fl.spectrum_fl);
                size_t cWL=ds->addCopiedColumn(spec->getWavelength(), spec->getN(), tr("%1_fl_wavelength").arg(item.name));
                size_t cSP=ds->addCopiedColumn(spec->getSpectrum(), spec->getN(), tr("%1_fl_spectrum").arg(item.name));
                g->set_symbol(JKQTPnoSymbol);
                g->set_drawLine(true);
                g->set_lineWidth(1);
                g->set_style(Qt::DotLine);
                g->set_xColumn(cWL);
                g->set_yColumn(cSP);
                g->set_color(wavelengthToColor(spec->getSpectrumMaxWavelength()));
                g->set_title(tr("fl: %1").arg(fl.name));
                ui->plotter->addGraph(g);
            }
        }
    }

    ui->plotter->set_doDrawing(true);
    ui->plotter->zoomToFit(true, true, true);
}


