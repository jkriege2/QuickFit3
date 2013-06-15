#include "qfespectraviewerdialog.h"
#include "ui_qfespectraviewerdialog.h"
#include "qfespectraviewer.h"
#include "qftools.h"
#include "spectrummanager.h"
#include "jkautooutputtimer.h"
#include <QToolBar>

double plotFunctionSpectralLine(double x, void* data) {
    double* d=(double*)data;
    return qfCauchy(x, d[0], d[1], d[2]);
}

QFESpectraViewerDialog::QFESpectraViewerDialog(QFESpectraViewer *plugin, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFESpectraViewerDialog)
{
    currentIndex=-1;
    this->plugin=plugin;
    manager=plugin->getSpectrumManager();

    ui->setupUi(this);
    QToolBar* mainToolbar=new QToolBar(plugin->getID()+"maintoolbar", this);
    ui->mainLayout->insertWidget(0,mainToolbar);
    QAction* actClose=new QAction(QIcon(":/lib/exit.png"), tr("&Close"), this);
    connect(actClose, SIGNAL(triggered()), this, SLOT(close()));
    mainToolbar->addAction(actClose);
    mainToolbar->addSeparator();
    ui->plotter->populateToolbar(mainToolbar);

    //qDebug()<<"loading fluorophores";
    QStringList sl=manager->getFluorophores();
    ui->cmbFluorophore->clear();
    for (int i=0; i<sl.size(); i++) {
        if (manager->fluorophoreExists(sl[i])) {
            //qDebug()<<sl[i]<<manager->getFluorophoreData(sl[i]).name;
            ui->cmbFluorophore->addItem(manager->getFluorophoreData(sl[i]).name, sl[i]);
            //qDebug()<<sl[i]<<manager->getFluorophoreData(sl[i]).name;
            //ui->cmbFluorophore->addItem(sl[i], sl[i]);
        }
    }

    //qDebug()<<"setting model";
    ui->tabFluorophoreInfo->setModel(&modItemProperties);


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
    //JKAutoOutputTimer tim("loadSpectraConfig");
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

        it.displayName=settings.value(prefix+QString("spectrum%1/displayname").arg(i), it.name).toString();
        it.filterIsExcitation=settings.value(prefix+QString("spectrum%1/filterisexcitation").arg(i), false).toBool();
        it.centralWavelength=settings.value(prefix+QString("spectrum%1/centralwavelength").arg(i), 525).toDouble();
        it.spectralWidth=settings.value(prefix+QString("spectrum%1/spectralwidth").arg(i), 50).toDouble();

        plotItems<<it;
        QListWidgetItem* item=new QListWidgetItem(it.displayName, ui->lstSpectra);
        switch(it.type) {
            case qfesFluorohpore: item->setIcon(QIcon(":/qfe_spectraviewer/fluorophore.png")); break;
            case qfesLightSourceSpectrum:
            case qfesLightSourceSingleLine: item->setIcon(QIcon(":/qfe_spectraviewer/lightsource.png")); break;
            case qfesFilterSpectrum:
            case qfesFilterBandpass: item->setIcon(QIcon(":/qfe_spectraviewer/filter.png")); break;

            default: break;
        }

        //ui->lstSpectra->addItem(item);
    }
    connect(ui->lstSpectra, SIGNAL(currentRowChanged(int)), this, SLOT(spectrumSelected()));
    ui->lstSpectra->setCurrentRow(0);
    spectrumSelected();
    updatePlots();
}

void QFESpectraViewerDialog::saveSpectraConfig(QSettings &settings, const QString &prefix)
{
    //JKAutoOutputTimer tim("saveSpectraConfig");
    settings.setValue(prefix+QString("count"), plotItems.size());
    for (int i=0; i<plotItems.size(); i++) {
        settings.setValue(prefix+QString("spectrum%1/type").arg(i), (int)plotItems[i].type);
        settings.setValue(prefix+QString("spectrum%1/name").arg(i), plotItems[i].name);
        settings.setValue(prefix+QString("spectrum%1/showexcitation").arg(i), plotItems[i].showExcitation);
        settings.setValue(prefix+QString("spectrum%1/showemission").arg(i), plotItems[i].showEmission);
        settings.setValue(prefix+QString("spectrum%1/displayname").arg(i), plotItems[i].displayName);
        settings.setValue(prefix+QString("spectrum%1/filterisexcitation").arg(i), plotItems[i].filterIsExcitation);
        settings.setValue(prefix+QString("spectrum%1/centralwavelength").arg(i), plotItems[i].centralWavelength);
        settings.setValue(prefix+QString("spectrum%1/spectralwidth").arg(i), plotItems[i].spectralWidth);
    }
}

void QFESpectraViewerDialog::on_btnAddFluorophore_clicked()
{
    //JKAutoOutputTimer tim("on_btnAddFluorophore_clicked");
    QFESpectraViewerPlotItem it;
    it.type=qfesFluorohpore;
    it.showEmission=true;
    it.showExcitation=true;
    plotItems.append(it);
    QListWidgetItem* item=new QListWidgetItem(it.name, ui->lstSpectra);
    //item->setData(Qt::UserRole, plotItems.size()-1);
    //ui->lstSpectra->addItem(item);
    ui->lstSpectra->setCurrentItem(item);
    spectrumSelected();
}

void QFESpectraViewerDialog::on_btnAddFilter_clicked()
{
    //JKAutoOutputTimer tim("on_btnAddFluorophore_clicked");
    QFESpectraViewerPlotItem it;
    it.type=qfesFilterBandpass;
    it.spectralWidth=35;
    it.centralWavelength=525;
    plotItems.append(it);
    QListWidgetItem* item=new QListWidgetItem(it.name, ui->lstSpectra);
    //item->setData(Qt::UserRole, plotItems.size()-1);
    //ui->lstSpectra->addItem(item);
    ui->lstSpectra->setCurrentItem(item);
    spectrumSelected();
}

void QFESpectraViewerDialog::on_btnAddLightsource_clicked()
{
    //JKAutoOutputTimer tim("on_btnAddFluorophore_clicked");
    QFESpectraViewerPlotItem it;
    it.type=qfesLightSourceSingleLine;
    it.spectralWidth=1;
    it.centralWavelength=488;
    plotItems.append(it);
    QListWidgetItem* item=new QListWidgetItem(it.name, ui->lstSpectra);
    //item->setData(Qt::UserRole, plotItems.size()-1);
    //ui->lstSpectra->addItem(item);
    ui->lstSpectra->setCurrentItem(item);
    spectrumSelected();
}

void QFESpectraViewerDialog::on_btnDelete_clicked()
{
    //JKAutoOutputTimer tim("on_btnDelete_clicked");
    if (currentIndex<0 || currentIndex>=plotItems.size()) return;
    int delIdx=currentIndex;
    ui->lstSpectra->setCurrentRow(-1);
    QListWidgetItem* item=ui->lstSpectra->takeItem(delIdx);
    delete item;
    plotItems.removeAt(delIdx);
    if (plotItems.size()<=0) currentIndex=0;
    else ui->lstSpectra->setCurrentRow(0);
    spectrumSelected();
}

void QFESpectraViewerDialog::spectrumSelected()
{
    int idx=ui->lstSpectra->currentRow();
    //JKAutoOutputTimer tim(QString("spectrumSelected idx=%1 currentIdx=%2").arg(idx).arg(currentIndex));
    if (currentIndex==idx) return;

    saveFromWidgets();

    currentIndex=idx;
    if (idx<0) currentIndex=-1;

    loadToWidgets();
}

void QFESpectraViewerDialog::saveFromWidgets()
{
    //JKAutoOutputTimer tim("saveFromWidgets");
    if (currentIndex<0 || currentIndex>=plotItems.size()) return;
    if (ui->stackSpectraEditor->currentWidget()==ui->widFluorophore)  {
        plotItems[currentIndex].type=qfesFluorohpore;
        plotItems[currentIndex].name=ui->cmbFluorophore->itemData(ui->cmbFluorophore->currentIndex()).toString();
        plotItems[currentIndex].showEmission=ui->chkFluorophoreShowEmission->isChecked();
        plotItems[currentIndex].showExcitation=ui->chkFluorophoreShowExcitation->isChecked();
        plotItems[currentIndex].displayName=ui->cmbFluorophore->currentText();
        if (ui->lstSpectra->item(currentIndex)) {
            ui->lstSpectra->item(currentIndex)->setText(plotItems[currentIndex].displayName);
            ui->lstSpectra->item(currentIndex)->setIcon(QIcon(":/qfe_spectraviewer/fluorophore.png"));
        }
    } else if (ui->stackSpectraEditor->currentWidget()==ui->widFilter)  {
        plotItems[currentIndex].displayName="";
        if (ui->cmbFilterType->currentIndex()==0) {
            plotItems[currentIndex].type=qfesFilterBandpass;
            plotItems[currentIndex].displayName=tr("BP %1/%2").arg(ui->spinFilterCentral->value()).arg(ui->spinLaserLinewidth->value());
            plotItems[currentIndex].centralWavelength=ui->spinFilterCentral->value();
            plotItems[currentIndex].spectralWidth=ui->spinFilterLinewidth->value();
        } else {
            plotItems[currentIndex].type=qfesFilterSpectrum;
            plotItems[currentIndex].name=ui->cmbFilter->itemData(ui->cmbFilter->currentIndex()).toString();
            plotItems[currentIndex].displayName=ui->cmbFilter->currentText();
        }
        plotItems[currentIndex].filterIsExcitation=ui->cmbFilterUsage->currentIndex()==0;
        if (ui->lstSpectra->item(currentIndex)) {
            ui->lstSpectra->item(currentIndex)->setText(plotItems[currentIndex].displayName);
            ui->lstSpectra->item(currentIndex)->setIcon(QIcon(":/qfe_spectraviewer/filter.png"));
        }

    } else if (ui->stackSpectraEditor->currentWidget()==ui->widLightSource)  {
        plotItems[currentIndex].displayName="";
        if (ui->cmbLightSourceType->currentIndex()==0) {
            plotItems[currentIndex].type=qfesLightSourceSingleLine;
            plotItems[currentIndex].displayName=tr("LASER %1nm").arg(ui->spinLaserCentral->value());
            plotItems[currentIndex].centralWavelength=ui->spinLaserCentral->value();
            plotItems[currentIndex].spectralWidth=ui->spinLaserLinewidth->value();
        } else {
            plotItems[currentIndex].type=qfesLightSourceSpectrum;
            plotItems[currentIndex].name=ui->cmbLightsource->itemData(ui->cmbLightsource->currentIndex()).toString();
            plotItems[currentIndex].displayName=ui->cmbLightsource->currentText();
        }
        if (ui->lstSpectra->item(currentIndex)) {
            ui->lstSpectra->item(currentIndex)->setText(plotItems[currentIndex].displayName);
            ui->lstSpectra->item(currentIndex)->setIcon(QIcon(":/qfe_spectraviewer/lightsource.png"));
        }

    }
    updatePlots();

}

void QFESpectraViewerDialog::loadToWidgets()
{
    //JKAutoOutputTimer tim("loadToWidgets");
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
        } else  if (plotItems[currentIndex].type==qfesLightSourceSingleLine)  {
             ui->stackSpectraEditor->setCurrentWidget(ui->widLightSource);
             ui->stackLightsource->setCurrentWidget(ui->widLSLaser);
             ui->cmbLightSourceType->setCurrentIndex(0);
             ui->cmbLightsource->setCurrentIndex(ui->cmbLightsource->findData(plotItems[currentIndex].name));
             ui->spinLaserCentral->setValue(plotItems[currentIndex].centralWavelength);
             ui->spinLaserLinewidth->setValue(plotItems[currentIndex].spectralWidth);
             connect(ui->spinLaserCentral, SIGNAL(valueChanged(double)), this, SLOT(saveFromWidgets()));
             connect(ui->spinLaserLinewidth, SIGNAL(valueChanged(double)), this, SLOT(saveFromWidgets()));
             connect(ui->cmbLightSourceType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
             connect(ui->cmbLightsource, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
        } else  if (plotItems[currentIndex].type==qfesLightSourceSpectrum)  {
             ui->stackSpectraEditor->setCurrentWidget(ui->widLightSource);
             ui->stackLightsource->setCurrentWidget(ui->widLSSpectrum);
             ui->cmbLightSourceType->setCurrentIndex(1);
             ui->cmbLightsource->setCurrentIndex(ui->cmbLightsource->findData(plotItems[currentIndex].name));
             ui->spinLaserCentral->setValue(plotItems[currentIndex].centralWavelength);
             ui->spinLaserLinewidth->setValue(plotItems[currentIndex].spectralWidth);
             connect(ui->spinLaserCentral, SIGNAL(valueChanged(double)), this, SLOT(saveFromWidgets()));
             connect(ui->spinLaserLinewidth, SIGNAL(valueChanged(double)), this, SLOT(saveFromWidgets()));
             connect(ui->cmbLightSourceType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
             connect(ui->cmbLightsource, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));

        } else  if (plotItems[currentIndex].type==qfesFilterBandpass)  {
             ui->stackSpectraEditor->setCurrentWidget(ui->widFilter);
             ui->stackFilter->setCurrentWidget(ui->widFilterBandpass);
             ui->cmbFilterType->setCurrentIndex(0);
             ui->cmbFilter->setCurrentIndex(ui->cmbFilter->findData(plotItems[currentIndex].name));
             ui->spinFilterCentral->setValue(plotItems[currentIndex].centralWavelength);
             ui->spinFilterLinewidth->setValue(plotItems[currentIndex].spectralWidth);
             connect(ui->spinFilterCentral, SIGNAL(valueChanged(double)), this, SLOT(saveFromWidgets()));
             connect(ui->spinFilterLinewidth, SIGNAL(valueChanged(double)), this, SLOT(saveFromWidgets()));
             connect(ui->cmbFilterType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
             connect(ui->cmbFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
        } else  if (plotItems[currentIndex].type==qfesFilterSpectrum)  {
             ui->stackSpectraEditor->setCurrentWidget(ui->widFilter);
             ui->stackFilter->setCurrentWidget(ui->widFilterSpectrum);
             ui->cmbFilterType->setCurrentIndex(1);
             ui->cmbFilter->setCurrentIndex(ui->cmbFilter->findData(plotItems[currentIndex].name));
             ui->spinFilterCentral->setValue(plotItems[currentIndex].centralWavelength);
             ui->spinFilterLinewidth->setValue(plotItems[currentIndex].spectralWidth);
             connect(ui->spinFilterCentral, SIGNAL(valueChanged(double)), this, SLOT(saveFromWidgets()));
             connect(ui->spinFilterLinewidth, SIGNAL(valueChanged(double)), this, SLOT(saveFromWidgets()));
             connect(ui->cmbFilterType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
             connect(ui->cmbFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));

        } else {
            ui->stackSpectraEditor->setCurrentWidget(ui->widEmpty);
        }
    }
    updateItemPropertiesModel();
}

void QFESpectraViewerDialog::updateItemPropertiesModel()
{
    //qDebug()<<"updateItemPropertiesModel: "<<currentIndex;
    //JKAutoOutputTimer tim("loadToWidgets");
    modItemProperties.disableSignals();
    modItemProperties.setReadonly(false);
    modItemProperties.clear();
    if (currentIndex>=0 && currentIndex<plotItems.size()) {
        //qDebug()<<"updateItemPropertiesModel: "<<plotItems[currentIndex].name;
        if (plotItems[currentIndex].type==qfesFluorohpore)  {
            //qDebug()<<"updateItemPropertiesModel: is fluorophore";
            if (manager->fluorophoreExists(plotItems[currentIndex].name)) {
                SpectrumManager::FluorophorData fl=manager->getFluorophoreData(plotItems[currentIndex].name);
                //qDebug()<<"updateItemPropertiesModel: fl.name="<<fl.name;
                int row=0;
                modItemProperties.setCellCreate(row,0, tr("name"));
                modItemProperties.setCellCreate(row,1, fl.name);
                row++;
                modItemProperties.setCellCreate(row,0, tr("order No."));
                modItemProperties.setCellCreate(row,1, fl.orderNo);
                row++;
                modItemProperties.setCellCreate(row,0, tr("description"));
                modItemProperties.setCellCreate(row,1, fl.description);
                row++;
                modItemProperties.setCellCreate(row,0, tr("manufacturer"));
                modItemProperties.setCellCreate(row,1, fl.manufacturer);
                row++;
                modItemProperties.setCellCreate(row,0, tr("reference"));
                modItemProperties.setCellCreate(row,1, fl.reference);
                row++;
                modItemProperties.setCellCreate(row,0, tr("max. absorption [nm]"));
                modItemProperties.setCellCreate(row,1, fl.excitation_maxwavelength);
                row++;
                modItemProperties.setCellCreate(row,0, tr("max. emission [nm]"));
                modItemProperties.setCellCreate(row,1, fl.emission_maxwavelength);
                row++;
                modItemProperties.setCellCreate(row,0, tr("conditions"));
                modItemProperties.setCellCreate(row,1, fl.condition);
                row++;
                modItemProperties.setCellCreate(row,0, tr("fl. efficiency [%]"));
                modItemProperties.setCellCreate(row,1, fl.fluorescence_efficiency*100.0);
                row++;
                modItemProperties.setCellCreate(row,0, tr("fl. lifetime [ns]"));
                modItemProperties.setCellCreate(row,1, fl.fluorescence_lifetime*1e9);
                row++;
                modItemProperties.setCellCreate(row,0, tr("extinction coefficient [1/(cm M)]"));
                modItemProperties.setCellCreate(row,1, tr("%1 @ %2nm").arg(fl.extiction_coefficient).arg(fl.extiction_coefficient_wavelength));
                row++;
                modItemProperties.setCellCreate(row,0, tr("abs. crosssection [cm²]"));
                modItemProperties.setCellCreate(row,1, tr("%1 @ %2nm").arg(fl.extiction_coefficient*3.82e-21).arg(fl.extiction_coefficient_wavelength));
                row++;
            }
        } else  if (plotItems[currentIndex].type==qfesLightSourceSpectrum)  {
             if (manager->lightsourceExists(plotItems[currentIndex].name)) {
                 SpectrumManager::LightSourceData ls=manager->getLightSourceData(plotItems[currentIndex].name);
                 int row=0;
                 modItemProperties.setCellCreate(row,0, tr("name"));
                 modItemProperties.setCellCreate(row,1, ls.name);
                 row++;
                 modItemProperties.setCellCreate(row,0, tr("order No."));
                 modItemProperties.setCellCreate(row,1, ls.orderNo);
                 row++;
                 modItemProperties.setCellCreate(row,0, tr("description"));
                 modItemProperties.setCellCreate(row,1, ls.description);
                 row++;
                 modItemProperties.setCellCreate(row,0, tr("manufacturer"));
                 modItemProperties.setCellCreate(row,1, ls.manufacturer);
                 row++;
                 modItemProperties.setCellCreate(row,0, tr("reference"));
                 modItemProperties.setCellCreate(row,1, ls.reference);
                 row++;
                 modItemProperties.setCellCreate(row,0, tr("typical wavelength [nm]"));
                 modItemProperties.setCellCreate(row,1, ls.typical_wavelength);
                 row++;
             }
        } else  if (plotItems[currentIndex].type==qfesFilterSpectrum)  {
             if (manager->filterExists(plotItems[currentIndex].name)) {
                 SpectrumManager::FilterData ls=manager->getFilterData(plotItems[currentIndex].name);
                 int row=0;
                 modItemProperties.setCellCreate(row,0, tr("name"));
                 modItemProperties.setCellCreate(row,1, ls.name);
                 row++;
                 modItemProperties.setCellCreate(row,0, tr("order No."));
                 modItemProperties.setCellCreate(row,1, ls.orderNo);
                 row++;
                 modItemProperties.setCellCreate(row,0, tr("description"));
                 modItemProperties.setCellCreate(row,1, ls.description);
                 row++;
                 modItemProperties.setCellCreate(row,0, tr("manufacturer"));
                 modItemProperties.setCellCreate(row,1, ls.manufacturer);
                 row++;
                 modItemProperties.setCellCreate(row,0, tr("reference"));
                 modItemProperties.setCellCreate(row,1, ls.reference);
                 row++;
                 modItemProperties.setCellCreate(row,0, tr("typical wavelength [nm]"));
                 modItemProperties.setCellCreate(row,1, ls.typical_wavelength);
                 row++;
             }
        }
    }
    modItemProperties.setColumnTitle(0, tr("property"));
    modItemProperties.setColumnTitle(1, tr("name"));
    modItemProperties.setReadonly(true);
    modItemProperties.enableSignals(true);
    ui->tabFluorophoreInfo->setModel(&modItemProperties);
    ui->tabFluorophoreInfo->resizeColumnsToContents();
}

void QFESpectraViewerDialog::writeSettings() {
    //JKAutoOutputTimer tim("writeSettings");
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
    //JKAutoOutputTimer tim("readSettings");
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
    //JKAutoOutputTimer tim("updatePlots");
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
                g->set_lineWidth(2);
                g->set_style(Qt::DashLine);
                g->set_xColumn(cWL);
                g->set_yColumn(cSP);
                g->set_color(wavelengthToColor(spec->getSpectrumMaxWavelength()));
                g->set_title(tr("abs: %1").arg(fl.name));
                ui->plotter->addGraph(g);
            }
            if (manager->spectrumExists(fl.spectrum_fl))  {
                JKQTPfilledCurveXGraph* g=new JKQTPfilledCurveXGraph(ui->plotter->get_plotter());
                SpectrumManager::Spectrum* spec=manager->getSpectrum(fl.spectrum_fl);
                size_t cWL=ds->addCopiedColumn(spec->getWavelength(), spec->getN(), tr("%1_fl_wavelength").arg(item.name));
                size_t cSP=ds->addCopiedColumn(spec->getSpectrum(), spec->getN(), tr("%1_fl_spectrum").arg(item.name));
                //g->set_symbol(JKQTPnoSymbol);
                g->set_drawLine(true);
                g->set_lineWidth(2);
                g->set_style(Qt::DotLine);
                QColor col=wavelengthToColor(spec->getSpectrumMaxWavelength());
                g->set_color(col);
                col.setAlphaF(0.5);
                g->set_fillColor(col);
                g->set_xColumn(cWL);
                g->set_yColumn(cSP);
                g->set_title(tr("fl: %1").arg(fl.name));
                ui->plotter->addGraph(g);
            }
        } else if (item.type==qfesLightSourceSingleLine) {
            /*JKQTPxFunctionLineGraph* g=new  JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
            QVector<double> p;
            p<<1<<item.spectralWidth<<item.centralWavelength;
            g->set_drawLine(true);
            g->set_lineWidth(2);
            g->set_plotFunction(plotFunctionSpectralLine);
            g->set_params(p);
            g->set_color(wavelengthToColor(item.centralWavelength));
            g->set_title(tr("LASER (%1{\\pm}%2)nm").arg(item.centralWavelength).arg(item.spectralWidth));
            ui->plotter->addGraph(g);*/
            JKQTPverticalRange* g=new  JKQTPverticalRange(ui->plotter->get_plotter());
            g->set_lineWidth(2);
            g->set_rangeMin(item.centralWavelength-item.spectralWidth/2.0);
            g->set_rangeMax(item.centralWavelength+item.spectralWidth/2.0);
            g->set_rangeCenter(item.centralWavelength);
            g->set_plotCenterLine(true);
            g->set_sizeMin(0);
            g->set_sizeMax(1);
            g->set_style(Qt::SolidLine);
            g->set_unlimitedSizeMax(false);
            g->set_unlimitedSizeMin(false);
            //g->set_rangeCenter(item.centralWavelength);
            QColor col=wavelengthToColor(item.centralWavelength);
            g->set_centerColor(col);
            g->set_color(col);
            col.setAlphaF(0.5);
            g->set_fillColor(col);
            //col.setAlphaF(0);
            g->set_title(tr("LASER (%1{\\pm}%2)nm").arg(item.centralWavelength).arg(item.spectralWidth));
            ui->plotter->addGraph(g);

        } else if (item.type==qfesFilterBandpass) {
            JKQTPverticalRange* g=new  JKQTPverticalRange(ui->plotter->get_plotter());
            g->set_lineWidth(2);
            g->set_rangeMin(item.centralWavelength-item.spectralWidth/2.0);
            g->set_rangeMax(item.centralWavelength+item.spectralWidth/2.0);
            g->set_rangeCenter(item.centralWavelength);
            g->set_plotCenterLine(false);
            g->set_sizeMin(0);
            g->set_sizeMax(1);
            g->set_unlimitedSizeMax(false);
            g->set_unlimitedSizeMin(false);
            QColor col=wavelengthToColor(item.centralWavelength);
            g->set_style(Qt::SolidLine);
            g->set_color(col);
            col.setAlphaF(0.25);
            g->set_fillColor(col);
            g->set_title(tr("FILTER: BP %1/%2").arg(item.centralWavelength).arg(item.spectralWidth));
            ui->plotter->addGraph(g);
        } else if (item.type==qfesLightSourceSpectrum && manager->lightsourceExists(item.name)) {
            SpectrumManager::LightSourceData ls=manager->getLightSourceData(item.name);
            if (manager->spectrumExists(ls.spectrum))  {
                JKQTPfilledCurveXGraph* g=new JKQTPfilledCurveXGraph(ui->plotter->get_plotter());
                SpectrumManager::Spectrum* spec=manager->getSpectrum(ls.spectrum);
                //spec->ensureSpectrum();
                size_t cWL=ds->addCopiedColumn(spec->getWavelength(), spec->getN(), tr("%1_wavelength").arg(item.name));
                size_t cSP=ds->addCopiedColumn(spec->getSpectrum(), spec->getN(), tr("%1_spectrum").arg(item.name));
                g->set_drawLine(true);
                g->set_lineWidth(2);
                g->set_style(Qt::DashLine);
                g->set_xColumn(cWL);
                g->set_yColumn(cSP);
                QColor col=QColor("darkgrey");
                g->set_color(col);
                col.setAlphaF(0.25);
                g->set_fillColor(col);
                g->set_title(tr("LIGHT: %1").arg(ls.name));
                ui->plotter->addGraph(g);
            }
        }
    }

    ui->plotter->set_doDrawing(true);
    ui->plotter->zoomToFit(true, true, false, true);
    ui->plotter->update_plot();
}

QFESpectraViewerPlotItem::QFESpectraViewerPlotItem(QFESpectraViewerPlotItemType type)
{
    this->type=type;
    name="";
    showExcitation=true;
    showEmission=true;
    filterIsExcitation=false;
    centralWavelength=488;
    spectralWidth=1;
    displayName="";
}


