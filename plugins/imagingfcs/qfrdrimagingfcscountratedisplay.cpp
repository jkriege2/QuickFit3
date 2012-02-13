#include "qfrdrimagingfcscountratedisplay.h"
#include "ui_qfrdrimagingfcscountratedisplay.h"

QFRDRImagingFCSCountrateDisplay::QFRDRImagingFCSCountrateDisplay(QFPluginServices *services, QWidget *parent) :
    QFRawDataEditor(services, parent),
    ui(new Ui::QFRDRImagingFCSCountrateDisplay)
{
    avgGraph=NULL;
    image=NULL;
    ui->setupUi(this);

    avgGraph=new JKQTPxyLineErrorGraph(ui->pltIntensity->get_plotter());
    avgGraph->set_title(tr("Avg \\pm StdDev"));
    avgGraph->set_xErrorStyle(JKQTPnoError);
    avgGraph->set_yErrorStyle(JKQTPerrorPolygons);
    avgGraph->set_drawLine(true);
    QColor col=avgGraph->get_color();
    col.setAlphaF(0.6);
    avgGraph->set_errorFillColor(col);
    avgGraph->set_symbol(JKQTPnoSymbol);

    minGraph=new JKQTPxyLineGraph(ui->pltIntensity->get_plotter());
    minGraph->set_title(tr("Minimum"));
    minGraph->set_drawLine(true);
    minGraph->set_symbol(JKQTPnoSymbol);

    maxGraph=new JKQTPxyLineGraph(ui->pltIntensity->get_plotter());
    maxGraph->set_title(tr("Minimum"));
    maxGraph->set_drawLine(true);
    maxGraph->set_symbol(JKQTPnoSymbol);

    avgFit=new JKQTPxyLineGraph(ui->pltIntensity->get_plotter());
    avgFit->set_title(tr("fit"));
    avgFit->set_drawLine(true);
    avgFit->set_symbol(JKQTPnoSymbol);
    avgIndicator=new JKQTPgeoInfiniteLine(ui->pltIntensity->get_plotter(), 0,0,0,1,QColor("red"));


    image=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::FloatArray, NULL, 0, 0, JKQTPMathImage::GRAY, ui->pltImage->get_plotter());
    cropRegion=new JKQTPgeoRectangle(ui->pltImage->get_plotter(), 0,0,0,0,QColor("red"),1);
    imageRegion=new JKQTPgeoRectangle(ui->pltImage->get_plotter(), 0,0,0,0,QColor("magenta"),1);

    ui->pltIntensity->addGraph(avgGraph);
    ui->pltIntensity->addGraph(minGraph);
    ui->pltIntensity->addGraph(maxGraph);
    ui->pltIntensity->addGraph(avgFit);
    ui->pltIntensity->addGraph(avgIndicator);
    ui->pltIntensity->getXAxis()->set_axisLabel(tr("time [seconds]"));
    ui->pltIntensity->getYAxis()->set_axisLabel(tr("intensity [A.U.]"));

    ui->pltImage->addGraph(image);
    ui->pltImage->addGraph(image);
    ui->pltImage->addGraph(imageRegion);


    ui->player->setSingleShot(true);

    ui->cmbGraph->addItem(tr("average +/- stddev."));
    ui->cmbGraph->addItem(tr("average +/- stddev., min, max"));
    ui->cmbGraph->addItem(tr("average"));
    ui->cmbGraph->addItem(tr("standard deviation"));
    ui->cmbGraph->addItem(tr("minimum"));
    ui->cmbGraph->addItem(tr("maximum"));

    connect(ui->player, SIGNAL(showFrame(int)), this, SLOT(showFrame(int)));

}

QFRDRImagingFCSCountrateDisplay::~QFRDRImagingFCSCountrateDisplay()
{
    delete ui;
}

void QFRDRImagingFCSCountrateDisplay::connectWidgets(QFRawDataRecord *current, QFRawDataRecord *old) {
    if (old) {
        disconnect(old, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
    }
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
    }
    displayData();
}

void QFRDRImagingFCSCountrateDisplay::rawDataChanged() {
}

void QFRDRImagingFCSCountrateDisplay::displayData() {
    if (!avgGraph) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    JKQTPdatastore* ds=ui->pltIntensity->get_plotter()->getDatastore();
    if (!current || !m) {
        ds->clear();
        ui->pltIntensity->set_doDrawing(false);

        avgGraph->set_xColumn(-1);
        avgGraph->set_yColumn(-1);
        avgGraph->set_xErrorColumn(-1);
        avgGraph->set_yErrorColumn(-1);

        ui->pltIntensity->set_doDrawing(true);
        ui->pltIntensity->zoomToFit();
    } else {
        ui->cmbImage->clear();
        ui->cmbImage->addItem(tr("average during measurement"));
        displayImage();

        JKQTPdatastore* ds=ui->pltIntensity->get_plotter()->getDatastore();
        ds->clear();
        ui->pltIntensity->set_doDrawing(false);
        int colT=ds->addColumn(m->getStatisticsT(), m->getStatisticsN(), "time");
        int colAvg=ds->addColumn(m->getStatisticsMean(), m->getStatisticsN(), "avg");
        int colStddev=ds->addColumn(m->getStatisticsStdDev(), m->getStatisticsN(), "stddev");
        int colMin=ds->addColumn(m->getStatisticsMin(), m->getStatisticsN(), "min");
        int colMax=ds->addColumn(m->getStatisticsMax(), m->getStatisticsN(), "max");

        if (ui->cmbGraph->currentIndex()==0) {
            avgGraph->set_xColumn(colT);
            avgGraph->set_yColumn(colAvg);
            avgGraph->set_yErrorColumn(colStddev);
            avgGraph->set_title(tr("avg \\pm stddev"));
            minGraph->set_xColumn(-1);
            minGraph->set_yColumn(-1);
            maxGraph->set_xColumn(-1);
            maxGraph->set_yColumn(-1);
        } else if (ui->cmbGraph->currentIndex()==1) {
            avgGraph->set_xColumn(colT);
            avgGraph->set_yColumn(colAvg);
            avgGraph->set_yErrorColumn(colStddev);
            minGraph->set_xColumn(colT);
            minGraph->set_yColumn(colMin);
            maxGraph->set_xColumn(colT);
            maxGraph->set_yColumn(colMax);
            avgGraph->set_title(tr("avg \\pm stddev"));
            minGraph->set_title(tr("minimum"));
            maxGraph->set_title(tr("maximum"));
        } else if (ui->cmbGraph->currentIndex()==2) {
            avgGraph->set_xColumn(colT);
            avgGraph->set_yColumn(colAvg);
            avgGraph->set_yErrorColumn(-1);
            minGraph->set_xColumn(-1);
            minGraph->set_yColumn(-1);
            maxGraph->set_xColumn(-1);
            maxGraph->set_yColumn(-1);
            avgGraph->set_title(tr("average"));
            minGraph->set_title("");
            maxGraph->set_title("");
        } else if (ui->cmbGraph->currentIndex()==3) {
            avgGraph->set_xColumn(colT);
            avgGraph->set_yColumn(colStddev);
            avgGraph->set_yErrorColumn(-1);
            minGraph->set_xColumn(-1);
            minGraph->set_yColumn(-1);
            maxGraph->set_xColumn(-1);
            maxGraph->set_yColumn(-1);
            avgGraph->set_title(tr("stddev"));
            minGraph->set_title("");
            maxGraph->set_title("");
        } else if (ui->cmbGraph->currentIndex()==4) {
            avgGraph->set_xColumn(colT);
            avgGraph->set_yColumn(colMin);
            avgGraph->set_yErrorColumn(-1);
            minGraph->set_xColumn(-1);
            minGraph->set_yColumn(-1);
            maxGraph->set_xColumn(-1);
            maxGraph->set_yColumn(-1);
            avgGraph->set_title(tr("minimum"));
            minGraph->set_title("");
            maxGraph->set_title("");
        } else if (ui->cmbGraph->currentIndex()==5) {
            avgGraph->set_xColumn(colT);
            avgGraph->set_yColumn(colMax);
            avgGraph->set_yErrorColumn(-1);
            minGraph->set_xColumn(-1);
            minGraph->set_yColumn(-1);
            maxGraph->set_xColumn(-1);
            maxGraph->set_yColumn(-1);
            avgGraph->set_title(tr("maximum"));
            minGraph->set_title("");
            maxGraph->set_title("");
        }

        ui->pltIntensity->set_doDrawing(true);
        ui->pltIntensity->zoomToFit();
    }
}

void QFRDRImagingFCSCountrateDisplay::displayImage() {
    if (!image) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    ui->player->setEnabled(false);
    ui->player->pause();
    ui->pltImage->set_doDrawing(false);
    if (m && ui->cmbImage->currentIndex()==0) {
        image->set_data(m->getDataImagePreview(), m->getDataImageWidth(), m->getDataImageHeight(), JKQTPMathImageBase::UInt16Array);
        image->set_width(m->getDataImageWidth());
        image->set_height(m->getDataImageHeight());
        ui->pltImage->get_plotter()->setAbsoluteXY(0,m->getDataImageWidth(),0,m->getDataImageHeight());
        ui->pltImage->get_plotter()->set_aspectRatio(1);
        ui->pltImage->get_plotter()->set_axisAspectRatio(double(m->getDataImageWidth())/double(m->getDataImageHeight()));
        ui->pltImage->get_plotter()->set_maintainAspectRatio(true);
        ui->pltImage->get_plotter()->set_maintainAxisAspectRatio(true);
        cropRegion->set_x(0);
        cropRegion->set_y(0);
        cropRegion->set_width(0);
        cropRegion->set_height(0);
        imageRegion->set_x(0);
        imageRegion->set_y(0);
        imageRegion->set_width(0);
        imageRegion->set_height(0);
    } else if (m && ui->cmbImage->currentIndex()==1) {
        image->set_data(NULL, 0, 0, JKQTPMathImageBase::UInt16Array);
        cropRegion->set_x(0);
        cropRegion->set_y(0);
        cropRegion->set_width(0);
        cropRegion->set_height(0);
        imageRegion->set_x(0);
        imageRegion->set_y(0);
        imageRegion->set_width(0);
        imageRegion->set_height(0);
        ui->player->setEnabled(true);
        ui->player->setRange(0,100);
    } else {

    }
    ui->pltImage->set_doDrawing(true);
    ui->pltImage->zoomToFit();
}

void QFRDRImagingFCSCountrateDisplay::showFrame(int frame) {
    ui->pltImage->set_doDrawing(false);

    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m && frame>0 && frame<m->getStatisticsN() && m->getStatisticsN()>0 && ui->cmbImage->currentIndex()==1) {
        ui->pltIntensity->set_doDrawing(false);
        const double* T=m->getStatisticsMean();
        double Tmin=T[0];
        double Tmax=T[m->getStatisticsN()-1];
        avgIndicator->set_x(Tmin+(Tmax-Tmin)*ui->player->getRelativeValue());

        ui->pltIntensity->set_doDrawing(true);
        ui->pltIntensity->update_plot();
    }

    ui->pltImage->set_doDrawing(true);
    ui->pltImage->zoomToFit();
}

void QFRDRImagingFCSCountrateDisplay::calcFit()
{
}

void QFRDRImagingFCSCountrateDisplay::readSettings() {
    if (!settings) return;
    ui->player->readSettings(*(settings->getQSettings()), QString("imfcscountseditor/"));
    jkloadSplitter(*(settings->getQSettings()), ui->splitter, QString("imfcscountseditor/splitter"));
    jkloadSplitter(*(settings->getQSettings()), ui->splitterH, QString("imfcscountseditor/splitter_h"));
}

void QFRDRImagingFCSCountrateDisplay::writeSettings() {
    if (!settings) return;
    ui->player->storeSettings(*(settings->getQSettings()), QString("imfcscountseditor/"));
    jksaveSplitter(*(settings->getQSettings()), ui->splitter, QString("imfcscountseditor/splitter"));
    jksaveSplitter(*(settings->getQSettings()), ui->splitterH, QString("imfcscountseditor/splitter_h"));
}
