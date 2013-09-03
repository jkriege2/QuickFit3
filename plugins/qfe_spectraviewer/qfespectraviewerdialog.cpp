#include "qfespectraviewerdialog.h"
#include "ui_qfespectraviewerdialog.h"
#include "qfespectraviewer.h"
#include "qftools.h"
#include "spectrummanager.h"
#include "jkautooutputtimer.h"
#include <QToolBar>
#include "qfespectraviewerlightsourceeditor.h"
#include "qfespectraviewerfiltereditor.h"
#include "qfespectraviewerfluorophoreditor.h"
#include "qfespectraviewerdetectoreditor.h"
#include <QSettings>
#include "qfespectraviewerspilloverdialog.h"
#include "qfversion.h"

double plotFunctionSpectralLine(double x, void* data) {
    double* d=(double*)data;
    return qfCauchy(x, d[0], d[1], d[2]);
}
double plotFunctionSmoothSpectrum(double x, void* data) {
    SpectrumManager::Spectrum* d=(SpectrumManager::Spectrum*)data;
    return d->getSpectrumAt(x);
}

QFESpectraViewerDialog::QFESpectraViewerDialog(QFESpectraViewer *plugin, QWidget *parent) :
    QDialog(parent,  Qt::Window|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint|Qt::WindowCloseButtonHint|Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
    ui(new Ui::QFESpectraViewerDialog)
{
    currentIndex=-1;
    this->plugin=plugin;
    manager=plugin->getSpectrumManager();

    ui->setupUi(this);


    /*ui->cmbLaserLine->addItem(tr("Ar:Kr laser / 488nm"), QPointF(488,1));
    ui->cmbLaserLine->addItem(tr("Ar:Kr laser / 561nm"), QPointF(561,1));
    ui->cmbLaserLine->addItem(tr("Ar:Kr laser / 647nm"), QPointF(647,1));*/
    ui->cmbLaserLine->setModel(manager->getLaserlinesTree());


    QToolBar* mainToolbar=new QToolBar(plugin->getID()+"maintoolbar", this);
    ui->mainLayout->insertWidget(0,mainToolbar);
    QAction* actClose=new QAction(QIcon(":/lib/exit.png"), tr("&Close"), this);
    connect(actClose, SIGNAL(triggered()), this, SLOT(close()));
    mainToolbar->addAction(actClose);
    mainToolbar->addSeparator();

    //QToolButton* btn=new QToolButton(this);
    //btn->setIcon(QIcon(":/lib/prop_add.png"));
    //btn->setText(tr("create new ..."));

    ui->btnNewFluorophore->setIcon(QIcon(":/lib/prop_add.png"));
    ui->btnNewFluorophore->setText(tr("create new ..."));

    //btn->setPopupMode(QToolButton::InstantPopup);
    QAction* actNewLightsource=new QAction(QIcon(":/qfe_spectraviewer/lightsource.png"), tr("... &lightsource"), this);
    connect(actNewLightsource, SIGNAL(triggered()), this, SLOT(createLightSourceSpectrum()));
    //btn->addAction(actNewLightsource);
    ui->btnNewFluorophore->addAction(actNewLightsource);
    QAction* actNewFilter=new QAction(QIcon(":/qfe_spectraviewer/filter.png"), tr("... &filter"), this);
    connect(actNewFilter, SIGNAL(triggered()), this, SLOT(createFilterSpectrum()));
    //btn->addAction(actNewFilter);
    ui->btnNewFluorophore->addAction(actNewFilter);
    QAction* actFluorophore=new QAction(QIcon(":/qfe_spectraviewer/fluorophore.png"), tr("... f&luorophore"), this);
    connect(actFluorophore, SIGNAL(triggered()), this, SLOT(createFluorophoreSpectrum()));
    //btn->addAction(actFluorophore);
    ui->btnNewFluorophore->addAction(actFluorophore);
    //mainToolbar->addWidget(btn);
    QAction* actDetector=new QAction(QIcon(":/qfe_spectraviewer/detector.png"), tr("... &detector"), this);
    connect(actDetector, SIGNAL(triggered()), this, SLOT(createDetectorSpectrum()));
    //btn->addAction(actDetector);
    ui->btnNewFluorophore->addAction(actDetector);
    //mainToolbar->addWidget(btn);

    mainToolbar->addSeparator();

    QAction* actSpillover=new QAction(tr("&Spillover Table"), this);
    connect(actSpillover, SIGNAL(triggered()), this, SLOT(calcSpilloverTable()));
    mainToolbar->addAction(actSpillover);

    /*mainToolbar->addSeparator();
    ui->plotter->populateToolbar(mainToolbar);*/
    ui->plotter->set_toolbarAlwaysOn(true);

    //qDebug()<<"loading fluorophores";
    reloadComboboxes();
    //qDebug()<<"setting model";
    ui->tabFluorophoreInfo->setModel(&modItemProperties);


    //qDebug()<<"read settings";
    readSettings();
    //qDebug()<<"constructor done";
}

void QFESpectraViewerDialog::reloadComboboxes()
{
    ui->lstSpectra->setCurrentRow(-1);
    currentIndex=-1;
    spectrumSelected();

    /*QStringList sl=manager->getFluorophores();
    ui->cmbFluorophore->clear();
    ui->cmbFluorophore->setUpdatesEnabled(false);
    for (int i=0; i<sl.size(); i++) {
        if (manager->fluorophoreExists(sl[i])) {
            //qDebug()<<sl[i]<<manager->getFluorophoreData(sl[i]).name;
            ui->cmbFluorophore->addItem(manager->getFluorophoreData(sl[i]).name, sl[i]);
            //qDebug()<<sl[i]<<manager->getFluorophoreData(sl[i]).name;
            //ui->cmbFluorophore->addItem(sl[i], sl[i]);
        }
    }
    ui->cmbFluorophore->setUpdatesEnabled(true);*/
    ui->cmbFluorophore->setModel(manager->getFluorophoresTree());
    ui->cmbFilter->setModel(manager->getFiltersTree());
    ui->cmbDetector->setModel(manager->getDetectorsTree());
    ui->cmbLightsource->setModel(manager->getLightSourcesTree());

    /*QStringList sl;
    sl=manager->getLightSources();
    ui->cmbLightsource->clear();
    ui->cmbLightsource->setUpdatesEnabled(false);
    for (int i=0; i<sl.size(); i++) {
        if (manager->lightsourceExists(sl[i])) {
            //qDebug()<<sl[i]<<manager->getFluorophoreData(sl[i]).name;
            ui->cmbLightsource->addItem(manager->getLightSourceData(sl[i]).name, sl[i]);
            //qDebug()<<sl[i]<<manager->getFluorophoreData(sl[i]).name;
            //ui->cmbFluorophore->addItem(sl[i], sl[i]);
        }
    }
    ui->cmbLightsource->setUpdatesEnabled(true);

    sl=manager->getFilters();
    ui->cmbFilter->clear();
    ui->cmbFilter->setUpdatesEnabled(false);
    for (int i=0; i<sl.size(); i++) {
        if (manager->filterExists(sl[i])) {
            //qDebug()<<sl[i]<<manager->getFluorophoreData(sl[i]).name;
            ui->cmbFilter->addItem(manager->getFilterData(sl[i]).name, sl[i]);
            //qDebug()<<sl[i]<<manager->getFluorophoreData(sl[i]).name;
            //ui->cmbFluorophore->addItem(sl[i], sl[i]);
        }
    }
    ui->cmbFilter->setUpdatesEnabled(true);


    sl=manager->getDetectors();
    ui->cmbDetector->clear();
    ui->cmbDetector->setUpdatesEnabled(false);
    for (int i=0; i<sl.size(); i++) {
        if (manager->detectorExists(sl[i])) {
            ui->cmbDetector->addItem(manager->getDetectorData(sl[i]).name, sl[i]);
        }
    }
    ui->cmbDetector->setUpdatesEnabled(true);*/

    ui->lstSpectra->setCurrentRow(0);
    spectrumSelected();
}

void QFESpectraViewerDialog::calcSpilloverTable()
{
    QFESpectraViewerSpilloverDialog* dlg=new QFESpectraViewerSpilloverDialog(this);
    dlg->init(plotItems, manager);
    dlg->exec();
    delete dlg;
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
            case qfesDetector: item->setIcon(QIcon(":/qfe_spectraviewer/detector.png")); break;

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
    QListWidgetItem* item=new QListWidgetItem(QIcon(":/qfe_spectraviewer/fluorophore.png"), tr("--- new fluorophore ---"), ui->lstSpectra);
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
    QListWidgetItem* item=new QListWidgetItem(QIcon(":/qfe_spectraviewer/lightsource.png"), tr("--- new filter ---"), ui->lstSpectra);
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
    QListWidgetItem* item=new QListWidgetItem(QIcon(":/qfe_spectraviewer/lightsource.png"), tr("--- new lightsource ---"), ui->lstSpectra);
    //item->setData(Qt::UserRole, plotItems.size()-1);
    //ui->lstSpectra->addItem(item);
    ui->lstSpectra->setCurrentItem(item);
    spectrumSelected();
}

void QFESpectraViewerDialog::on_btnAddDetector_clicked()
{
    //JKAutoOutputTimer tim("on_btnAddFluorophore_clicked");
    QFESpectraViewerPlotItem it;
    it.type=qfesDetector;
    plotItems.append(it);
    QListWidgetItem* item=new QListWidgetItem(QIcon(":/qfe_spectraviewer/detector.png"), tr("--- new detector ---"), ui->lstSpectra);
    //item->setData(Qt::UserRole, plotItems.size()-1);
    //ui->lstSpectra->addItem(item);
    ui->lstSpectra->setCurrentItem(item);
    spectrumSelected();
}


void QFESpectraViewerDialog::on_btnDelete_clicked()
{
    //JKAutoOutputTimer tim("on_btnDelete_clicked");
    //if (currentIndex<0 || currentIndex>=plotItems.size()) return;

    QList<QListWidgetItem*> sel=ui->lstSpectra->selectedItems();
    QList<int> selIdx;
    for (int i=0; i<sel.size(); i++) {
        for (int j=0; j<ui->lstSpectra->count(); j++) {
            if (ui->lstSpectra->item(j)==sel[i]) {
                selIdx<<j;
                //qDebug()<<j;
                break;
            }
        }
    }
    if (sel.size()>0) {
        int delIdx=currentIndex;

        ui->lstSpectra->setCurrentRow(-1);
        spectrumSelected(true);

        for (int i=0; i<sel.size(); i++) {
            delete sel[i];
        }
        qSort(selIdx);
        for (int i=selIdx.size()-1; i>=0; i--) {
            if (selIdx[i]>=0 && selIdx[i]<plotItems.size()) plotItems.removeAt(selIdx[i]);
        }

        if (plotItems.size()<=0) currentIndex=0;
        if (delIdx>0 && delIdx<plotItems.size()) currentIndex=delIdx;
    }

    /*int delIdx=currentIndex;
    ui->lstSpectra->setCurrentRow(-1);
    QListWidgetItem* item=ui->lstSpectra->takeItem(delIdx);
    delete item;
    plotItems.removeAt(delIdx);
    if (plotItems.size()<=0) currentIndex=0;
    if (delIdx>0 && delIdx<plotItems.size()) currentIndex=delIdx;*/

    ui->lstSpectra->setCurrentRow(currentIndex);
    spectrumSelected(true);

}

void QFESpectraViewerDialog::spectrumSelected(bool force)
{
    int idx=ui->lstSpectra->currentRow();
    //JKAutoOutputTimer tim(QString("spectrumSelected idx=%1 currentIdx=%2").arg(idx).arg(currentIndex));
    if (!force && currentIndex==idx) return;

    saveFromWidgets();

    currentIndex=idx;
    if (idx<0) currentIndex=-1;

    loadToWidgets();
    updatePlots();
}

void QFESpectraViewerDialog::saveFromWidgets()
{
    //JKAutoOutputTimer tim("saveFromWidgets");
    if (currentIndex<0 || currentIndex>=plotItems.size()) return;
    if (ui->stackSpectraEditor->currentWidget()==ui->widFluorophore)  {
        plotItems[currentIndex].type=qfesFluorohpore;
        plotItems[currentIndex].name=ui->cmbFluorophore->getCurrentIndexData(Qt::UserRole).toString();
        plotItems[currentIndex].showEmission=ui->chkFluorophoreShowEmission->isChecked();
        plotItems[currentIndex].showExcitation=ui->chkFluorophoreShowExcitation->isChecked();
        plotItems[currentIndex].displayName=ui->cmbFluorophore->currentText();
        if (ui->lstSpectra->item(currentIndex)) {
            ui->lstSpectra->item(currentIndex)->setText(plotItems[currentIndex].displayName);
            ui->lstSpectra->item(currentIndex)->setIcon(QIcon(":/qfe_spectraviewer/fluorophore.png"));
        }
    } else if (ui->stackSpectraEditor->currentWidget()==ui->widDetector)  {
        plotItems[currentIndex].type=qfesDetector;
        plotItems[currentIndex].name=ui->cmbDetector->getCurrentIndexData(Qt::UserRole).toString();//itemData(ui->cmbDetector->currentIndex()).toString();
        plotItems[currentIndex].displayName=ui->cmbDetector->currentText();
        if (ui->lstSpectra->item(currentIndex)) {
            ui->lstSpectra->item(currentIndex)->setText(plotItems[currentIndex].displayName);
            ui->lstSpectra->item(currentIndex)->setIcon(QIcon(":/qfe_spectraviewer/detector.png"));
        }
    } else if (ui->stackSpectraEditor->currentWidget()==ui->widFilter)  {
        plotItems[currentIndex].displayName="";
        if (ui->cmbFilterType->currentIndex()==0) {
            plotItems[currentIndex].type=qfesFilterBandpass;
            plotItems[currentIndex].displayName=tr("BP %1/%2").arg(ui->spinFilterCentral->value()).arg(ui->spinFilterLinewidth->value());
            plotItems[currentIndex].centralWavelength=ui->spinFilterCentral->value();
            plotItems[currentIndex].spectralWidth=ui->spinFilterLinewidth->value();
        } else if (ui->cmbFilterType->currentIndex()==1) {
            plotItems[currentIndex].type=qfesFilterLongpass;
            plotItems[currentIndex].displayName=tr("LP %1").arg(ui->spinFilterCutWavelength->value());
            plotItems[currentIndex].centralWavelength=ui->spinFilterCutWavelength->value();
        } else if (ui->cmbFilterType->currentIndex()==2) {
            plotItems[currentIndex].type=qfesFilterShortpass;
            plotItems[currentIndex].displayName=tr("SP %1").arg(ui->spinFilterCutWavelength->value());
            plotItems[currentIndex].centralWavelength=ui->spinFilterCutWavelength->value();
        } else if (ui->cmbFilterType->currentIndex()==3) {
            plotItems[currentIndex].type=qfesFilterNotch;
            plotItems[currentIndex].displayName=tr("NOTCH %1/%2").arg(ui->spinFilterCentral->value()).arg(ui->spinFilterLinewidth->value());
            plotItems[currentIndex].centralWavelength=ui->spinFilterCentral->value();
            plotItems[currentIndex].spectralWidth=ui->spinFilterLinewidth->value();
        } else {
            plotItems[currentIndex].type=qfesFilterSpectrum;
            plotItems[currentIndex].name=ui->cmbFilter->getCurrentIndexData(Qt::UserRole).toString();//itemData(ui->cmbFilter->currentIndex()).toString();
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
            plotItems[currentIndex].name=ui->cmbLightsource->getCurrentIndexData(Qt::UserRole).toString();//itemData(ui->cmbLightsource->currentIndex()).toString();
            plotItems[currentIndex].displayName=ui->cmbLightsource->currentText();
        }
        if (ui->lstSpectra->item(currentIndex)) {
            ui->lstSpectra->item(currentIndex)->setText(plotItems[currentIndex].displayName);
            ui->lstSpectra->item(currentIndex)->setIcon(QIcon(":/qfe_spectraviewer/lightsource.png"));
        }

    }
    updatePlots();
    updateItemPropertiesModel();

}

void QFESpectraViewerDialog::loadToWidgets()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //JKAutoOutputTimer tim("loadToWidgets");
    disconnect(ui->chkFluorophoreShowEmission, SIGNAL(toggled(bool)), this, SLOT(saveFromWidgets()));
    disconnect(ui->chkFluorophoreShowExcitation, SIGNAL(toggled(bool)), this, SLOT(saveFromWidgets()));
    disconnect(ui->cmbFluorophore, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
    disconnect(ui->spinLaserCentral, SIGNAL(valueChanged(double)), this, SLOT(saveFromWidgets()));
    disconnect(ui->spinLaserLinewidth, SIGNAL(valueChanged(double)), this, SLOT(saveFromWidgets()));
    disconnect(ui->cmbLightSourceType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
    disconnect(ui->cmbLightsource, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
    disconnect(ui->spinFilterCentral, SIGNAL(valueChanged(double)), this, SLOT(saveFromWidgets()));
    disconnect(ui->spinFilterLinewidth, SIGNAL(valueChanged(double)), this, SLOT(saveFromWidgets()));
    disconnect(ui->spinFilterCutWavelength, SIGNAL(valueChanged(double)), this, SLOT(saveFromWidgets()));
    disconnect(ui->cmbFilterType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
    disconnect(ui->cmbFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
    disconnect(ui->cmbDetector, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
    if (currentIndex<0 || currentIndex>=plotItems.size()) {
        ui->stackSpectraEditor->setCurrentWidget(ui->widEmpty);
    } else {
        if (plotItems[currentIndex].type==qfesFluorohpore)  {
            ui->stackSpectraEditor->setCurrentWidget(ui->widFluorophore);
            //ui->cmbFluorophore->setCurrentIndex(ui->cmbFluorophore->findData(plotItems[currentIndex].name));
            ui->cmbFluorophore->setCurrentFromModelData(plotItems[currentIndex].name);
            ui->chkFluorophoreShowEmission->setChecked(plotItems[currentIndex].showEmission);
            ui->chkFluorophoreShowExcitation->setChecked(plotItems[currentIndex].showExcitation);
            connect(ui->chkFluorophoreShowEmission, SIGNAL(toggled(bool)), this, SLOT(saveFromWidgets()));
            connect(ui->chkFluorophoreShowExcitation, SIGNAL(toggled(bool)), this, SLOT(saveFromWidgets()));
            connect(ui->cmbFluorophore, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
        } else  if (plotItems[currentIndex].type==qfesLightSourceSingleLine)  {
             ui->stackSpectraEditor->setCurrentWidget(ui->widLightSource);
             ui->stackLightsource->setCurrentWidget(ui->widLSLaser);
             ui->cmbLightSourceType->setCurrentIndex(0);
             ui->cmbLightsource->setCurrentFromModelData(plotItems[currentIndex].name);//setCurrentIndex(ui->cmbLightsource->findData(plotItems[currentIndex].name));
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
             ui->cmbLightsource->setCurrentFromModelData(plotItems[currentIndex].name);//setCurrentIndex(ui->cmbLightsource->findData(plotItems[currentIndex].name));
             ui->spinLaserCentral->setValue(plotItems[currentIndex].centralWavelength);
             ui->spinLaserLinewidth->setValue(plotItems[currentIndex].spectralWidth);
             connect(ui->spinLaserCentral, SIGNAL(valueChanged(double)), this, SLOT(saveFromWidgets()));
             connect(ui->spinLaserLinewidth, SIGNAL(valueChanged(double)), this, SLOT(saveFromWidgets()));
             connect(ui->cmbLightSourceType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
             connect(ui->cmbLightsource, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
        } else  if (plotItems[currentIndex].type==qfesDetector)  {
             ui->stackSpectraEditor->setCurrentWidget(ui->widDetector);
             ui->cmbDetector->setCurrentFromModelData(plotItems[currentIndex].name);//setCurrentIndex(ui->cmbDetector->findData(plotItems[currentIndex].name));
             connect(ui->cmbDetector, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
        } else  if (plotItems[currentIndex].type==qfesFilterBandpass || plotItems[currentIndex].type==qfesFilterNotch)  {
             ui->stackSpectraEditor->setCurrentWidget(ui->widFilter);
             ui->stackFilter->setCurrentWidget(ui->widFilterBandpass);
             if (plotItems[currentIndex].type==qfesFilterNotch) ui->cmbFilterType->setCurrentIndex(3);
             else ui->cmbFilterType->setCurrentIndex(0);
             ui->cmbFilter->setCurrentFromModelData(plotItems[currentIndex].name);//setCurrentIndex(ui->cmbFilter->findData(plotItems[currentIndex].name));
             ui->spinFilterCentral->setValue(plotItems[currentIndex].centralWavelength);
             ui->spinFilterLinewidth->setValue(plotItems[currentIndex].spectralWidth);
             connect(ui->spinFilterCentral, SIGNAL(valueChanged(double)), this, SLOT(saveFromWidgets()));
             connect(ui->spinFilterLinewidth, SIGNAL(valueChanged(double)), this, SLOT(saveFromWidgets()));
             connect(ui->cmbFilterType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
             connect(ui->cmbFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
        } else  if (plotItems[currentIndex].type==qfesFilterLongpass || plotItems[currentIndex].type==qfesFilterShortpass)  {
             ui->stackSpectraEditor->setCurrentWidget(ui->widFilter);
             ui->stackFilter->setCurrentWidget(ui->widFilterShortpass);
             if (plotItems[currentIndex].type==qfesFilterLongpass) ui->cmbFilterType->setCurrentIndex(1);
             else ui->cmbFilterType->setCurrentIndex(2);
             ui->cmbFilter->setCurrentFromModelData(plotItems[currentIndex].name);//setCurrentIndex(ui->cmbFilter->findData(plotItems[currentIndex].name));
             ui->spinFilterCutWavelength->setValue(plotItems[currentIndex].centralWavelength);
             ui->spinFilterLinewidth->setValue(plotItems[currentIndex].spectralWidth);
             connect(ui->spinFilterCutWavelength, SIGNAL(valueChanged(double)), this, SLOT(saveFromWidgets()));
             connect(ui->cmbFilterType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
             connect(ui->cmbFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(saveFromWidgets()));
        } else  if (plotItems[currentIndex].type==qfesFilterSpectrum)  {
             ui->stackSpectraEditor->setCurrentWidget(ui->widFilter);
             ui->stackFilter->setCurrentWidget(ui->widFilterSpectrum);
             ui->cmbFilterType->setCurrentIndex(ui->cmbFilterType->count()-1);
             ui->cmbFilter->setCurrentFromModelData(plotItems[currentIndex].name);//setCurrentIndex(ui->cmbFilter->findData(plotItems[currentIndex].name));
             ui->spinFilterCentral->setValue(plotItems[currentIndex].centralWavelength);
             ui->spinFilterCutWavelength->setValue(plotItems[currentIndex].centralWavelength);
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
    QApplication::restoreOverrideCursor();
}

void QFESpectraViewerDialog::updateItemPropertiesModel()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
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
                SpectrumManager::FluorophoreData fl=manager->getFluorophoreData(plotItems[currentIndex].name);
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
                modItemProperties.setCellCreate(row,1, tr("%1 @ %2nm").arg(fl.fluorescence_efficiency*100.0).arg(fl.fluorescence_efficiency_wavelength));
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
        } else  if (plotItems[currentIndex].type==qfesDetector)  {
             if (manager->lightsourceExists(plotItems[currentIndex].name)) {
                 SpectrumManager::DetectorData ls=manager->getDetectorData(plotItems[currentIndex].name);
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
                 modItemProperties.setCellCreate(row,0, tr("peak wavelength [nm]"));
                 modItemProperties.setCellCreate(row,1, ls.peak_wavelength);
                 row++;
                 modItemProperties.setCellCreate(row,0, tr("peak sensitivity [%1/W]").arg(ls.peak_sensitivity_unit));
                 modItemProperties.setCellCreate(row,1, ls.peak_sensitivity);
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
    QApplication::restoreOverrideCursor();
}

void QFESpectraViewerDialog::createLightSourceSpectrum()
{
    QFESpectraViewerLightsourceEditor* dlg=new QFESpectraViewerLightsourceEditor(this);
    if (dlg->exec()) {
        QDir d(ProgramOptions::getConfigValue("qfe_spectraviewer/user_database", QFPluginServices::getInstance()->getPluginConfigDirectory("qfe_spectraviewer")).toString());
        QSettings set(d.absoluteFilePath("ligtsources.ini"), QSettings::IniFormat);
        dlg->addDataAndSpectrum(set, manager);
        ui->lstSpectra->setCurrentRow(-1);
        currentIndex=-1;
        spectrumSelected();

        plugin->reloadDatabases();
        reloadComboboxes();
    }
    delete dlg;
}

void QFESpectraViewerDialog::createDetectorSpectrum()
{
    QFESpectraViewerDetectorEditor* dlg=new QFESpectraViewerDetectorEditor(this);
    if (dlg->exec()) {
        QDir d(ProgramOptions::getConfigValue("qfe_spectraviewer/user_database", QFPluginServices::getInstance()->getPluginConfigDirectory("qfe_spectraviewer")).toString());
        QSettings set(d.absoluteFilePath("detectors.ini"), QSettings::IniFormat);
        dlg->addDataAndSpectrum(set, manager);
        ui->lstSpectra->setCurrentRow(-1);
        currentIndex=-1;
        spectrumSelected();

        plugin->reloadDatabases();
        reloadComboboxes();
    }
    delete dlg;
}


void QFESpectraViewerDialog::on_btnEditLightsource_clicked()
{
    if (currentIndex<0 || currentIndex>=plotItems.size()) return;
    QFESpectraViewerLightsourceEditor* dlg=new QFESpectraViewerLightsourceEditor(this);
    if (manager->lightsourceExists(plotItems[currentIndex].name)) dlg->setFromData(plotItems[currentIndex].name, manager->getLightSourceData(plotItems[currentIndex].name), manager);
    if (dlg->exec()) {
        QDir d(ProgramOptions::getConfigValue("qfe_spectraviewer/user_database", QFPluginServices::getInstance()->getPluginConfigDirectory("qfe_spectraviewer")).toString());
        QSettings set(d.absoluteFilePath("ligtsources.ini"), QSettings::IniFormat);
        dlg->addDataAndSpectrum(set, manager);
        ui->lstSpectra->setCurrentRow(-1);
        currentIndex=-1;
        spectrumSelected();

        plugin->reloadDatabases();
        reloadComboboxes();
        updatePlots();
    }
    delete dlg;
}

void QFESpectraViewerDialog::createFilterSpectrum()
{
    QFESpectraViewerFilterEditor* dlg=new QFESpectraViewerFilterEditor(this);
    if (dlg->exec()) {
        QDir d(ProgramOptions::getConfigValue("qfe_spectraviewer/user_database", QFPluginServices::getInstance()->getPluginConfigDirectory("qfe_spectraviewer")).toString());
        QSettings set(d.absoluteFilePath("filters.ini"), QSettings::IniFormat);
        dlg->addDataAndSpectrum(set, manager);
        ui->lstSpectra->setCurrentRow(-1);
        currentIndex=-1;
        spectrumSelected();

        plugin->reloadDatabases();
        reloadComboboxes();
    }
    delete dlg;
}


void QFESpectraViewerDialog::on_btnEditFilter_clicked()
{
    if (currentIndex<0 || currentIndex>=plotItems.size()) return;
    QFESpectraViewerFilterEditor* dlg=new QFESpectraViewerFilterEditor(this);
    if (manager->filterExists(plotItems[currentIndex].name)) dlg->setFromData(plotItems[currentIndex].name, manager->getFilterData(plotItems[currentIndex].name), manager);
    if (dlg->exec()) {
        QDir d(ProgramOptions::getConfigValue("qfe_spectraviewer/user_database", QFPluginServices::getInstance()->getPluginConfigDirectory("qfe_spectraviewer")).toString());
        QSettings set(d.absoluteFilePath("filters.ini"), QSettings::IniFormat);
        dlg->addDataAndSpectrum(set, manager);
        ui->lstSpectra->setCurrentRow(-1);
        currentIndex=-1;
        spectrumSelected();

        plugin->reloadDatabases();
        reloadComboboxes();
        updatePlots();
    }
    delete dlg;
}


void QFESpectraViewerDialog::on_btnEditDetector_clicked()
{
    if (currentIndex<0 || currentIndex>=plotItems.size()) return;
    QFESpectraViewerDetectorEditor* dlg=new QFESpectraViewerDetectorEditor(this);
    if (manager->detectorExists(plotItems[currentIndex].name)) dlg->setFromData(plotItems[currentIndex].name, manager->getDetectorData(plotItems[currentIndex].name), manager);
    if (dlg->exec()) {
        QDir d(ProgramOptions::getConfigValue("qfe_spectraviewer/user_database", QFPluginServices::getInstance()->getPluginConfigDirectory("qfe_spectraviewer")).toString());
        QSettings set(d.absoluteFilePath("detectors.ini"), QSettings::IniFormat);
        dlg->addDataAndSpectrum(set, manager);
        ui->lstSpectra->setCurrentRow(-1);
        currentIndex=-1;
        spectrumSelected();

        plugin->reloadDatabases();
        reloadComboboxes();
        updatePlots();
    }
    delete dlg;
}


void QFESpectraViewerDialog::createFluorophoreSpectrum()
{
    QFESpectraViewerFluorophoreEditor* dlg=new QFESpectraViewerFluorophoreEditor(this);
    if (dlg->exec()) {
        QDir d(ProgramOptions::getConfigValue("qfe_spectraviewer/user_database", QFPluginServices::getInstance()->getPluginConfigDirectory("qfe_spectraviewer")).toString());
        QSettings set(d.absoluteFilePath("fluorophors.ini"), QSettings::IniFormat);
        dlg->addDataAndSpectrum(set, manager);
        ui->lstSpectra->setCurrentRow(-1);
        currentIndex=-1;
        spectrumSelected();

        plugin->reloadDatabases();
        reloadComboboxes();
    }
    delete dlg;
}


void QFESpectraViewerDialog::on_btnEditFluorophore_clicked()
{
    if (currentIndex<0 || currentIndex>=plotItems.size()) return;
    QFESpectraViewerFluorophoreEditor* dlg=new QFESpectraViewerFluorophoreEditor(this);
    if (manager->fluorophoreExists(plotItems[currentIndex].name)) dlg->setFromData(plotItems[currentIndex].name, manager->getFluorophoreData(plotItems[currentIndex].name), manager);
    if (dlg->exec()) {
        QDir d(ProgramOptions::getConfigValue("qfe_spectraviewer/user_database", QFPluginServices::getInstance()->getPluginConfigDirectory("qfe_spectraviewer")).toString());
        QSettings set(d.absoluteFilePath("fluorophors.ini"), QSettings::IniFormat);
        dlg->addDataAndSpectrum(set, manager);
        ui->lstSpectra->setCurrentRow(-1);
        currentIndex=-1;
        spectrumSelected();

        plugin->reloadDatabases();
        reloadComboboxes();
        updatePlots();
    }
    delete dlg;
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
            set->setValue(plugin->getID()+"/spinWavelengthMin", ui->spinWavelengthMin->value());
            set->setValue(plugin->getID()+"/spinWavelengthMax", ui->spinWavelengthMax->value());
            set->setValue(plugin->getID()+"/chkAUtorange", ui->chkAUtorange->isChecked());


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


            ui->spinWavelengthMin->setValue(set->value(plugin->getID()+"/spinWavelengthMin", 400).toDouble());
            ui->spinWavelengthMax->setValue(set->value(plugin->getID()+"/spinWavelengthMax", 800).toDouble());
            ui->chkAUtorange->setChecked(set->value(plugin->getID()+"/chkAUtorange", true).toBool());




        }
    }
}

void QFESpectraViewerDialog::updatePlots()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //JKAutoOutputTimer tim("updatePlots");
    ui->plotter->set_doDrawing(false);

    JKQTPdatastore* ds=ui->plotter->getDatastore();
    ui->plotter->clearGraphs(true);
    ds->clear();

    ui->plotter->getXAxis()->set_axisLabel(tr("wavelength \\lambda [nm]"));
    ui->plotter->getYAxis()->set_axisLabel(tr("relative intensity/absorbtion [0..1]"));
    ui->plotter->get_plotter()->setAbsoluteY(0,1.1);
    if (ui->chkAUtorange->isChecked()) {
        ui->plotter->get_plotter()->setAbsoluteX(0, 1e10);
    } else {
        ui->plotter->get_plotter()->setAbsoluteX(ui->spinWavelengthMin->value(), ui->spinWavelengthMax->value());
        ui->plotter->get_plotter()->setX(ui->spinWavelengthMin->value(), ui->spinWavelengthMax->value());
    }

    for (int i=0; i<plotItems.size(); i++) {
        QFESpectraViewerPlotItem item=plotItems[i];
        if (item.type==qfesFluorohpore && manager->fluorophoreExists(item.name)) {
            SpectrumManager::FluorophoreData fl=manager->getFluorophoreData(item.name);
            //if (!ui->chkSmoothSpectra->isChecked()) {
                if (manager->spectrumExists(fl.spectrum_abs) && item.showExcitation)  {
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
                    g->set_drawSelectionLine(ui->lstSpectra->currentRow()==i);
                    g->set_color(wavelengthToColor(spec->getSpectrumMaxWavelength()));
                    if (manager->spectrumExists(fl.spectrum_fl))  {
                        SpectrumManager::Spectrum* spec1=manager->getSpectrum(fl.spectrum_fl);
                        g->set_color(wavelengthToColor(spec1->getSpectrumMaxWavelength()));
                    }
                    g->set_title(tr("abs: %1").arg(fl.name));
                    ui->plotter->addGraph(g);
                }
                if (manager->spectrumExists(fl.spectrum_fl) && item.showEmission)  {
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
                    g->set_drawSelectionLine(ui->lstSpectra->currentRow()==i);
                    g->set_xColumn(cWL);
                    g->set_yColumn(cSP);
                    g->set_title(tr("fl: %1").arg(fl.name));
                    ui->plotter->addGraph(g);
                }
            /*} else {
                if (manager->spectrumExists(fl.spectrum_abs) && item.showExcitation)  {
                    JKQTPxFunctionLineGraph* g=new  JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
                    SpectrumManager::Spectrum* spec=manager->getSpectrum(fl.spectrum_abs);
                    g->set_plotFunction(plotFunctionSmoothSpectrum);
                    g->set_params((void*)spec);
                    //spec->ensureSpectrum();
                    size_t cWL=ds->addCopiedColumn(spec->getWavelength(), spec->getN(), tr("%1_abs_wavelength").arg(item.name));
                    size_t cSP=ds->addCopiedColumn(spec->getSpectrum(), spec->getN(), tr("%1_abs_spectrum").arg(item.name));
                    g->set_drawLine(true);
                    g->set_lineWidth(2);
                    g->set_style(Qt::DashLine);
                    g->set_color(wavelengthToColor(spec->getSpectrumMaxWavelength()));
                    if (manager->spectrumExists(fl.spectrum_fl))  {
                        SpectrumManager::Spectrum* spec1=manager->getSpectrum(fl.spectrum_fl);
                        g->set_color(wavelengthToColor(spec1->getSpectrumMaxWavelength()));
                    }
                    g->set_title(tr("abs: %1").arg(fl.name));
                    ui->plotter->addGraph(g);
                }
                if (manager->spectrumExists(fl.spectrum_fl) && item.showEmission)  {
                    JKQTPxFunctionLineGraph* g=new  JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
                    SpectrumManager::Spectrum* spec=manager->getSpectrum(fl.spectrum_fl);
                    g->set_plotFunction(plotFunctionSmoothSpectrum);
                    g->set_params((void*)spec);
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
                    g->set_title(tr("fl: %1").arg(fl.name));
                    ui->plotter->addGraph(g);
                }
            }*/

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

        } else if (item.type==qfesFilterBandpass || item.type==qfesFilterNotch) {
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
            if (item.type==qfesFilterNotch) {
                g->set_invertedRange(true);
                g->set_title(tr("FILTER: NOTCH %1/%2").arg(item.centralWavelength).arg(item.spectralWidth));
            }
            ui->plotter->addGraph(g);
        } else if (item.type==qfesFilterLongpass) {
            JKQTPverticalRange* g=new  JKQTPverticalRange(ui->plotter->get_plotter());
            g->set_lineWidth(2);
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
            g->set_title(tr("FILTER: LP %1").arg(item.centralWavelength));
            g->set_rangeMin(NAN);
            g->set_rangeMax(item.centralWavelength);
            g->set_invertedRange(true);
            ui->plotter->addGraph(g);
        } else if (item.type==qfesFilterShortpass) {
            JKQTPverticalRange* g=new  JKQTPverticalRange(ui->plotter->get_plotter());
            g->set_lineWidth(2);
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
            g->set_title(tr("FILTER: LP %1").arg(item.centralWavelength));
            g->set_rangeMin(item.centralWavelength);
            g->set_rangeMax(NAN);
            g->set_invertedRange(true);
            ui->plotter->addGraph(g);
        } else if (item.type==qfesFilterSpectrum && manager->filterExists(item.name)) {
            SpectrumManager::FilterData ls=manager->getFilterData(item.name);
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
                g->set_drawSelectionLine(ui->lstSpectra->currentRow()==i);
                QColor col=wavelengthToColor(ls.typical_wavelength);
                g->set_color(col);
                col.setAlphaF(0.25);
                g->set_fillColor(col);
                g->set_title(tr("FILTER: %1").arg(ls.name));
                ui->plotter->addGraph(g);
            }
        } else if (item.type==qfesLightSourceSpectrum && manager->lightsourceExists(item.name)) {
            SpectrumManager::LightSourceData ls=manager->getLightSourceData(item.name);
            //if (!ui->chkSmoothSpectra->isChecked()) {
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
                    g->set_drawSelectionLine(ui->lstSpectra->currentRow()==i);
                    QColor col=QColor("darkgrey");
                    g->set_color(col);
                    col.setAlphaF(0.25);
                    g->set_fillColor(col);
                    g->set_title(tr("LIGHT: %1").arg(ls.name));
                    ui->plotter->addGraph(g);
                }
            /*} else {
                if (manager->spectrumExists(ls.spectrum))  {
                    JKQTPxFunctionLineGraph* g=new  JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
                    SpectrumManager::Spectrum* spec=manager->getSpectrum(ls.spectrum);
                    //spec->ensureSpectrum();
                    size_t cWL=ds->addCopiedColumn(spec->getWavelength(), spec->getN(), tr("%1_wavelength").arg(item.name));
                    size_t cSP=ds->addCopiedColumn(spec->getSpectrum(), spec->getN(), tr("%1_spectrum").arg(item.name));
                    g->set_drawLine(true);
                    g->set_lineWidth(2);
                    g->set_style(Qt::DashLine);
                    g->set_plotFunction(plotFunctionSmoothSpectrum);
                    g->set_params((void*)spec);
                    QColor col=QColor("darkgrey");
                    g->set_color(col);
                    col.setAlphaF(0.25);
                    g->set_fillColor(col);
                    g->set_title(tr("LIGHT: %1").arg(ls.name));
                    ui->plotter->addGraph(g);
                }
            }*/
        } else if (item.type==qfesDetector && manager->detectorExists(item.name)) {
            SpectrumManager::DetectorData ls=manager->getDetectorData(item.name);
            //if (!ui->chkSmoothSpectra->isChecked()) {
                if (manager->spectrumExists(ls.spectrum))  {
                    JKQTPfilledCurveXGraph* g=new JKQTPfilledCurveXGraph(ui->plotter->get_plotter());
                    SpectrumManager::Spectrum* spec=manager->getSpectrum(ls.spectrum);
                    //spec->ensureSpectrum();
                    size_t cWL=ds->addCopiedColumn(spec->getWavelength(), spec->getN(), tr("%1_wavelength").arg(item.name));
                    size_t cSP=ds->addCopiedColumn(spec->getSpectrum(), spec->getN(), tr("%1_spectrum").arg(item.name));
                    g->set_drawLine(true);
                    g->set_lineWidth(1);
                    g->set_style(Qt::DotLine);
                    g->set_xColumn(cWL);
                    g->set_yColumn(cSP);
                    g->set_drawSelectionLine(ui->lstSpectra->currentRow()==i);
                    QColor col=QColor("darkgrey");
                    g->set_color(col);
                    col.setAlphaF(0.25);
                    g->set_fillColor(col);
                    g->set_title(tr("DETECTOR: %1").arg(ls.name));
                    ui->plotter->addGraph(g);
                }

        }
    }

    ui->plotter->set_doDrawing(true);
    ui->plotter->zoomToFit(true, true, false, true);
    ui->plotter->update_plot();
    QApplication::restoreOverrideCursor();
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


void QFESpectraViewerDialog::on_btnMailFluorophore_clicked() {
    if (currentIndex<0 || currentIndex>=plotItems.size()) return;
    if (manager->fluorophoreExists(plotItems[currentIndex].name)) {
        QString spectrafiles="";
        SpectrumManager::FluorophoreData d=manager->getFluorophoreData(plotItems[currentIndex].name);
        QVector<double> absWL, flWL, absS, flS;
        if (manager->spectrumExists(d.spectrum_abs)) {
            absWL=  arrayToVector(manager->getSpectrum(d.spectrum_abs)->getWavelength(), manager->getSpectrum(d.spectrum_abs)->getN());
            absS=  arrayToVector(manager->getSpectrum(d.spectrum_abs)->getSpectrum(), manager->getSpectrum(d.spectrum_abs)->getN());
        }
        if (manager->spectrumExists(d.spectrum_fl)) {
            flWL=  arrayToVector(manager->getSpectrum(d.spectrum_fl)->getWavelength(), manager->getSpectrum(d.spectrum_fl)->getN());
            flS=  arrayToVector(manager->getSpectrum(d.spectrum_fl)->getSpectrum(), manager->getSpectrum(d.spectrum_fl)->getN());
        }
        QString absFN=cleanStringForFilename(plotItems[currentIndex].name.toLower())+"_abs.spec";
        spectrafiles+=QString("# %1\n").arg(absFN);
        for (int i=0; i<absWL.size(); i++) {
            spectrafiles+=(doubleToQString(absWL[i])+", "+doubleToQString(absS[i])+"\n");
        }
        QString flFN=cleanStringForFilename(plotItems[currentIndex].name.toLower())+"_fl.spec";
        spectrafiles+="\n\n---------------------------------------------------------------------\n";
        spectrafiles+=QString("# %1\n").arg(flFN);
        for (int i=0; i<flWL.size(); i++) {
            spectrafiles+=(doubleToQString(flWL[i])+", "+doubleToQString(flS[i])+"\n");
        }
        spectrafiles+="\n\n\n";

        QString data=QString("[%1]\n").arg(plotItems[currentIndex].name);
        data+=QString("name = \"%1\"\n").arg(d.name);
        data+=QString("manufacturer = \"%1\"\n").arg(d.manufacturer);
        data+=QString("oder_no = \"%1\"\n").arg(d.orderNo);
        data+=QString("reference = \"%1\"\n").arg(d.reference);
        data+=QString("q_fluor = %1\n").arg(d.fluorescence_efficiency);
        data+=QString("q_fluor_wavelength = %1\n").arg(d.fluorescence_efficiency_wavelength);
        data+=QString("molar_extinction = %1\n").arg(d.extiction_coefficient);
        data+=QString("molar_extinction_wavelength = %1\n").arg(d.extiction_coefficient_wavelength);
        data+=QString("tau_fluor = %1\n").arg(d.fluorescence_lifetime);
        data+=QString("excitation_max = %1\n").arg(d.excitation_maxwavelength);
        data+=QString("emission_max = %1\n").arg(d.emission_maxwavelength);
        data+=QString("description = \"%1\"\n").arg(d.description);
        data+=QString("condition = \"%1\"\n").arg(d.condition);
        data+=QString("spectrum_abs = \"%1\"\n").arg(absFN);
        data+=QString("spectrum_abs_id = %1\n").arg(0);
        data+=QString("spectrum_abs_separatewavelengths = %1\n").arg(false);
        if (manager->spectrumExists(d.spectrum_abs)) {
            data+=QString("spectrum_abs_reference = \"%1\"\n").arg(manager->getSpectrum(d.spectrum_abs)->reference);
        }
        data+=QString("spectrum_fl = \"%1\"\n").arg(flFN);
        data+=QString("spectrum_fl_id = %1\n").arg(0);
        data+=QString("spectrum_fl_separatewavelengths = %1\n").arg(false);
        if (manager->spectrumExists(d.spectrum_fl)) {
            data+=QString("spectrum_fl_reference = \"%1\"\n").arg(manager->getSpectrum(d.spectrum_fl)->reference);
        }

        QString mailcontents=tr("Dear authors,\nfind attatched my fluorophores spectrum data for inclusion in the next QuickFit release,\n\nBest,\n\n\nDATA:\n---------------------------------------------------------------------\n%1\n---------------------------------------------------------------------\n%2\n---------------------------------------------------------------------\n").arg(data).arg(spectrafiles);

        QUrl url=QUrl(QByteArray("mailto:")+qfInfoEmail().toLocal8Bit()+"?subject="+QUrl::toPercentEncoding("QuickFit3/qfe_spectraviewer: new fluorophore data")+
                      "&body="+QUrl::toPercentEncoding(mailcontents));
        QDesktopServices::openUrl(url);


    }
}

void QFESpectraViewerDialog::on_btnMailLightsource_clicked()
{
    if (currentIndex<0 || currentIndex>=plotItems.size()) return;
    if (manager->lightsourceExists(plotItems[currentIndex].name)) {
        QString spectrafiles="";
        SpectrumManager::LightSourceData d=manager->getLightSourceData(plotItems[currentIndex].name);
        QVector<double> absWL, absS;
        if (manager->spectrumExists(d.spectrum)) {
            absWL=  arrayToVector(manager->getSpectrum(d.spectrum)->getWavelength(), manager->getSpectrum(d.spectrum)->getN());
            absS=  arrayToVector(manager->getSpectrum(d.spectrum)->getSpectrum(), manager->getSpectrum(d.spectrum)->getN());
        }

        QString absFN=cleanStringForFilename(plotItems[currentIndex].name.toLower())+".spec";
        spectrafiles+=QString("# %1\n").arg(absFN);
        for (int i=0; i<absWL.size(); i++) {
            spectrafiles+=(doubleToQString(absWL[i])+", "+doubleToQString(absS[i])+"\n");
        }
        spectrafiles+="\n\n\n";

        QString data=QString("[%1]\n").arg(plotItems[currentIndex].name);
        data+=QString("name = %1\n").arg(d.name);
        data+=QString("manufacturer = %1\n").arg(d.manufacturer);
        data+=QString("oder_no = %1\n").arg(d.orderNo);
        data+=QString("reference = %1\n").arg(d.reference);
        data+=QString("typical_wavelength = %1\n").arg(d.typical_wavelength);
        data+=QString("description = %1\n").arg(d.description);
        data+=QString("spectrum = \"%1\"\n").arg(absFN);
        data+=QString("spectrum_id = %1\n").arg(0);
        data+=QString("spectrum_separatewavelengths = %1\n").arg(false);
        if (manager->spectrumExists(d.spectrum)) {
            data+=QString("spectrum_reference = \"%1\"\n").arg(manager->getSpectrum(d.spectrum)->reference);
        }

        QString mailcontents=tr("Dear authors,\nfind attatched my lightsource spectrum data for inclusion in the next QuickFit release,\n\nBest,\n\n\nDATA:\n---------------------------------------------------------------------\n%1\n---------------------------------------------------------------------\n%2\n---------------------------------------------------------------------\n").arg(data).arg(spectrafiles);

        QUrl url=QUrl(QByteArray("mailto:")+qfInfoEmail().toLocal8Bit()+"?subject="+QUrl::toPercentEncoding("QuickFit3/qfe_spectraviewer: new lightsource data")+
                      "&body="+QUrl::toPercentEncoding(mailcontents));
        QDesktopServices::openUrl(url);


    }
}

void QFESpectraViewerDialog::on_btnMailDetector_clicked()
{
    if (currentIndex<0 || currentIndex>=plotItems.size()) return;
    if (manager->detectorExists(plotItems[currentIndex].name)) {
        QString spectrafiles="";
        SpectrumManager::DetectorData d=manager->getDetectorData(plotItems[currentIndex].name);
        QVector<double> absWL, absS;
        if (manager->spectrumExists(d.spectrum)) {
            absWL=  arrayToVector(manager->getSpectrum(d.spectrum)->getWavelength(), manager->getSpectrum(d.spectrum)->getN());
            absS=  arrayToVector(manager->getSpectrum(d.spectrum)->getSpectrum(), manager->getSpectrum(d.spectrum)->getN());
        }

        QString absFN=cleanStringForFilename(plotItems[currentIndex].name.toLower())+".spec";
        spectrafiles+=QString("# %1\n").arg(absFN);
        for (int i=0; i<absWL.size(); i++) {
            spectrafiles+=(doubleToQString(absWL[i])+", "+doubleToQString(absS[i])+"\n");
        }
        spectrafiles+="\n\n\n";

        QString data=QString("[%1]\n").arg(plotItems[currentIndex].name);
        data+=QString("name = %1\n").arg(d.name);
        data+=QString("manufacturer = %1\n").arg(d.manufacturer);
        data+=QString("oder_no = %1\n").arg(d.orderNo);
        data+=QString("reference = %1\n").arg(d.reference);
        data+=QString("peak_wavelength = %1\n").arg(d.peak_wavelength);
        data+=QString("peak_sensitivity = %1\n").arg(d.peak_sensitivity);
        data+=QString("peak_sensitivity_unit = %1\n").arg(d.peak_sensitivity_unit);
        data+=QString("description = %1\n").arg(d.description);
        data+=QString("spectrum = \"%1\"\n").arg(absFN);
        data+=QString("spectrum_id = %1\n").arg(0);
        data+=QString("spectrum_separatewavelengths = %1\n").arg(false);
        if (manager->spectrumExists(d.spectrum)) {
            data+=QString("spectrum_reference = \"%1\"\n").arg(manager->getSpectrum(d.spectrum)->reference);
        }

        QString mailcontents=tr("Dear authors,\nfind attatched my detector spectrum data for inclusion in the next QuickFit release,\n\nBest,\n\n\nDATA:\n---------------------------------------------------------------------\n%1\n---------------------------------------------------------------------\n%2\n---------------------------------------------------------------------\n").arg(data).arg(spectrafiles);

        QUrl url=QUrl(QByteArray("mailto:")+qfInfoEmail().toLocal8Bit()+"?subject="+QUrl::toPercentEncoding("QuickFit3/qfe_spectraviewer: new detector data")+
                      "&body="+QUrl::toPercentEncoding(mailcontents));
        QDesktopServices::openUrl(url);


    }
}

void QFESpectraViewerDialog::on_btnMailFilter_clicked()
{
    if (currentIndex<0 || currentIndex>=plotItems.size()) return;
    if (manager->filterExists(plotItems[currentIndex].name)) {
        QString spectrafiles="";
        SpectrumManager::FilterData d=manager->getFilterData(plotItems[currentIndex].name);
        QVector<double> absWL, absS;
        if (manager->spectrumExists(d.spectrum)) {
            absWL=  arrayToVector(manager->getSpectrum(d.spectrum)->getWavelength(), manager->getSpectrum(d.spectrum)->getN());
            absS=  arrayToVector(manager->getSpectrum(d.spectrum)->getSpectrum(), manager->getSpectrum(d.spectrum)->getN());
        }

        QString absFN=cleanStringForFilename(plotItems[currentIndex].name.toLower())+".spec";
        spectrafiles+=QString("# %1\n").arg(absFN);
        for (int i=0; i<absWL.size(); i++) {
            spectrafiles+=(doubleToQString(absWL[i])+", "+doubleToQString(absS[i])+"\n");
        }
        spectrafiles+="\n\n\n";

        QString data=QString("[%1]\n").arg(plotItems[currentIndex].name);
        data+=QString("name = \"%1\"\n").arg(d.name);
        data+=QString("manufacturer = \"%1\"\n").arg(d.manufacturer);
        data+=QString("oder_no = \"%1\"\n").arg(d.orderNo);
        data+=QString("reference = \"%1\"\n").arg(d.reference);
        data+=QString("typical_wavelength = %1\n").arg(d.typical_wavelength);
        data+=QString("description = \"%1\"\n").arg(d.description);
        data+=QString("spectrum = \"%1\"\n").arg(absFN);
        data+=QString("spectrum_id = %1\n").arg(0);
        data+=QString("spectrum_separatewavelengths = %1\n").arg(false);
        if (manager->spectrumExists(d.spectrum)) {
            data+=QString("spectrum_reference = \"%1\"\n").arg(manager->getSpectrum(d.spectrum)->reference);
        }

        QString mailcontents=tr("Dear authors,\nfind attatched my filter spectrum data for inclusion in the next QuickFit release,\n\nBest,\n\n\nDATA:\n---------------------------------------------------------------------\n%1\n---------------------------------------------------------------------\n%2\n---------------------------------------------------------------------\n").arg(data).arg(spectrafiles);

        QUrl url=QUrl(QByteArray("mailto:")+qfInfoEmail().toLocal8Bit()+"?subject="+QUrl::toPercentEncoding("QuickFit3/qfe_spectraviewer: new filter data")+
                      "&body="+QUrl::toPercentEncoding(mailcontents));
        QDesktopServices::openUrl(url);


    }
}

void QFESpectraViewerDialog::on_cmbFilterType_currentIndexChanged(int i)
{
    if (i==ui->cmbFilterType->count()-1) ui->stackFilter->setCurrentIndex(2);
    else if (i==0||i==3) ui->stackFilter->setCurrentIndex(0);
    else ui->stackFilter->setCurrentIndex(1);
}

void QFESpectraViewerDialog::on_cmbLaserLine_currentIndexChanged(int i)
{
    if (i<0) return;
    //QVariant var=ui->cmbLaserLine->itemData(i);
    QVariant var=ui->cmbLaserLine->getCurrentIndexData(Qt::UserRole);
    //qDebug()<<var;
    QPointF data=QPointF(var.toDouble(), 1);
    if (var.canConvert(QVariant::PointF)) data=var.toPointF();
    if (data.x()>0) {
        ui->spinLaserCentral->setValue(data.x());
        if (data.y()<=0) ui->spinLaserLinewidth->setValue(1);
        else ui->spinLaserLinewidth->setValue(data.y());
    }
}

void QFESpectraViewerDialog::on_spinLaserCentral_valueChanged(double value)
{
    QPointF pnt=QPointF(ui->spinLaserCentral->value(), ui->spinLaserLinewidth->value());
    //ui->cmbLaserLine->setCurrentFromModelData(pnt);
    //ui->cmbLaserLine->setCurrentIndex(findLaserLineIndex(ui->spinLaserCentral->value(), ui->spinLaserLinewidth->value()));
}

void QFESpectraViewerDialog::on_spinLaserLinewidth_valueChanged(double value)
{
    QPointF pnt=QPointF(ui->spinLaserCentral->value(), ui->spinLaserLinewidth->value());
    //ui->cmbLaserLine->setCurrentFromModelData(pnt);
    //ui->cmbLaserLine->setCurrentIndex(findLaserLineIndex(ui->spinLaserCentral->value(), ui->spinLaserLinewidth->value()));
}

int QFESpectraViewerDialog::findLaserLineIndex(double line, double width) {
    for (int i=0; i<ui->cmbLaserLine->count(); i++)  {
        QVariant var=ui->cmbLaserLine->itemData(i);
        QPointF data=QPointF(var.toDouble(), 1);
        if (var.canConvert(QVariant::PointF)) data=var.toPointF();
        if (data.x()>0) {
            if (data.x()==line && (width<=0 || width==data.y())) {
                return i;
            }
        }
    }
    return -1;
}
