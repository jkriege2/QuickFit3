#include "qfrdrimagingfcscountratedisplay.h"
#include "ui_qfrdrimagingfcscountratedisplay.h"
#include "qfrdrimagingfcs_data.h"

QFRDRImagingFCSCountrateDisplay::QFRDRImagingFCSCountrateDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFRDRImagingFCSCountrateDisplay)
{
    current=NULL;
    avgGraph=NULL;
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


    ui->pltIntensity->addGraph(avgGraph);
    ui->pltIntensity->addGraph(minGraph);
    ui->pltIntensity->addGraph(maxGraph);
    ui->pltIntensity->addGraph(avgFit);
    ui->pltIntensity->addGraph(avgIndicator);
    ui->pltIntensity->getXAxis()->set_axisLabel(tr("time [seconds]"));
    ui->pltIntensity->getYAxis()->set_axisLabel(tr("intensity [A.U.]"));



    ui->cmbGraph->addItem(tr("average +/- stddev."));
    ui->cmbGraph->addItem(tr("average +/- stddev., min, max"));
    ui->cmbGraph->addItem(tr("average"));
    ui->cmbGraph->addItem(tr("standard deviation"));
    ui->cmbGraph->addItem(tr("minimum"));
    ui->cmbGraph->addItem(tr("maximum"));

}

QFRDRImagingFCSCountrateDisplay::~QFRDRImagingFCSCountrateDisplay()
{
    delete ui;
}

void QFRDRImagingFCSCountrateDisplay::connectWidgets(QFRawDataRecord *current, QFRawDataRecord *old) {
    if (old) {
        this->current=NULL;
        disconnect(old, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
    }
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        this->current=current;
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
    }
    displayData();
}

void QFRDRImagingFCSCountrateDisplay::showFrameIndicator1(double pos) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    //if (m && frame>0 && frame<m->getStatisticsN() && m->getStatisticsN()>0 && cmbImage->currentIndex()==1) {
        ui->pltIntensity->set_doDrawing(false);
        //const double* T=m->getStatisticsMean();
        //double Tmin=T[0];
        //double Tmax=T[m->getStatisticsN()-1];
        //avgIndicator->set_x(Tmin+(Tmax-Tmin)*player->getRelativeValue());
        avgIndicator->set_x(pos);
        ui->pltIntensity->set_doDrawing(true);
        ui->pltIntensity->update_plot();
    //}
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




void QFRDRImagingFCSCountrateDisplay::calcExpFit()
{
}

void QFRDRImagingFCSCountrateDisplay::readSettings(QSettings& settings, const QString& prefix) {
    jkloadSplitter(settings, ui->splitter, prefix+QString("splitter/"));
}

void QFRDRImagingFCSCountrateDisplay::writeSettings(QSettings &settings, const QString &prefix) {
    jksaveSplitter(settings, ui->splitter, prefix+QString("splitter/"));
}
