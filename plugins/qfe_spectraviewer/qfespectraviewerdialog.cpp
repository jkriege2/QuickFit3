#include "qfespectraviewerdialog.h"
#include "ui_qfespectraviewerdialog.h"
#include "qfespectraviewer.h"
#include "qftools.h"

QFESpectraViewerDialog::QFESpectraViewerDialog(QFESpectraViewer *plugin, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFESpectraViewerDialog)
{
    ui->setupUi(this);
    this->plugin=plugin;
    readSettings();
}

QFESpectraViewerDialog::~QFESpectraViewerDialog()
{
    writeSettings();
    delete ui;
}

void QFESpectraViewerDialog::loadSpectraConfig(QSettings &settings, const QString &prefix)
{
    plotItems.clear();

    int count=settings.value(prefix+QString("count"), 0).toInt();
    for (int i=0; i<count; i++) {
        QFESpectraViewerPlotItem it;
        it.type=(QFESpectraViewerPlotItemType)settings.value(prefix+QString("spectrum%1/type").arg(i), qfesFluorohpore).toInt();
        it.spectrum=settings.value(prefix+QString("spectrum%1/spectrum").arg(i), -1).toInt();
    }
    updatePlots();
}

void QFESpectraViewerDialog::saveSpectraConfig(QSettings &settings, const QString &prefix)
{
    settings.setValue(prefix+QString("count"), plotItems.size());
    for (int i=0; i<plotItems.size(); i++) {
        settings.setValue(prefix+QString("spectrum%1/type").arg(i), (int)plotItems[i].type);
        settings.setValue(prefix+QString("spectrum%1/spectrum").arg(i), plotItems[i].spectrum);
    }
}

void QFESpectraViewerDialog::writeSettings() {
    ProgramOptions* opt=ProgramOptions::getInstance();
    if (opt) {
        QSettings* set=opt->getQSettings();
        if (set) {
            saveWidgetGeometry(*set, this, plugin->getID()+"/widget/");
            saveSplitter(*set, ui->splitter, plugin->getID()+"/splitter/");
            saveSpectraConfig(*set,plugin->getID()+QString("/lastSpectra/") );

        }
    }
}

void QFESpectraViewerDialog::readSettings() {
    ProgramOptions* opt=ProgramOptions::getInstance();
    if (opt) {
        QSettings* set=opt->getQSettings();
        if (set) {
            loadWidgetGeometry(*set, this, plugin->getID()+"/widget/");
            loadSplitter(*set, ui->splitter, plugin->getID()+"/splitter/");
            loadSpectraConfig(*set,plugin->getID()+QString("/lastSpectra/") );


        }
    }
}

void QFESpectraViewerDialog::updatePlots()
{
    ui->plotter->set_doDrawing(false);

    JKQTPdatastore* ds=ui->plotter->getDatastore();
    ui->plotter->clearGraphs(true);
    ds->clear();

    ui->plotter->getXAxis()->set_axisLabel(tr("wavelength \\lambda [nm]"));
    ui->plotter->getYAxis()->set_axisLabel(tr("relative intensity/absorbtion [%]"));

    for (int i=0; i<plotItems.size(); i++) {
        QFESpectraViewerPlotItem item=plotItems[i];
        if (item.type==qfesFluorohpore) {
            JKQTPxyLineGraph* g=new JKQTPxyLineGraph(ui->plotter->get_plotter());


            ui->plotter->addGraph(g);
        }
    }

    ui->plotter->set_doDrawing(true);
    ui->plotter->zoomToFit(true, true, true);
}


