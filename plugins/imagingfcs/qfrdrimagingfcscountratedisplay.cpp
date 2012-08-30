#include "qfrdrimagingfcscountratedisplay.h"
#include "ui_qfrdrimagingfcscountratedisplay.h"
#include "qfrdrimagingfcs_data.h"
#include "statistics_tools.h"
#include <cmath>
#include <QDebug>

#include "qftools.h"
#include "lmmin.h"
#include <typeinfo>

typedef struct {
    const double *t;
    const double *y;
    double (*f) (double t, const double *par);
} lmcurve_data_struct;


void lmcurve_evaluate2( const double *par, int m_dat, const void *data,
                       double *fvec, int *info )
{
    int i;
    for ( i = 0; i < m_dat; i++ )
    fvec[i] =
            ((lmcurve_data_struct*)data)->y[i] -
            ((lmcurve_data_struct*)data)->f(
                ((lmcurve_data_struct*)data)->t[i], par );
    // *info = *info; /* to prevent a 'unused variable' warning */
}


void lmcurve_fit2( int n_par, double *par, int m_dat,
                  const double *t, const double *y,
                  double (*f)( double t, const double *par ),
                  const lm_control_struct *control, lm_status_struct *status )
{
    lmcurve_data_struct data = { t, y, f };

    lmmin( n_par, par, m_dat, (const void*) &data,
           lmcurve_evaluate2, control, status, lm_printout_std );
}

QFRDRImagingFCSCountrateDisplay::QFRDRImagingFCSCountrateDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFRDRImagingFCSCountrateDisplay)
{
    current=NULL;
    avgGraph=NULL;
    data=NULL;
    dataN=0;
    dataT=NULL;
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

    rangeGraph=new JKQTPhorizontalRange(ui->pltIntensity->get_plotter());
    QColor rfill=rangeGraph->get_color();
    rfill.setAlphaF(0.3);
    rangeGraph->set_fillColor(rfill);
    rfill.setAlphaF(0.8);
    rangeGraph->set_color(rfill);
    rangeGraph->set_title(tr("avg \\pm stddev."));

    avgFit=new JKQTPxFunctionLineGraph(ui->pltIntensity->get_plotter());
    avgFit->set_title(tr("fit"));
    avgFit->set_drawLine(true);

    avgIndicator=new JKQTPoverlayVerticalLine(0, ui->pltIntensity->get_plotter());


    ui->pltIntensity->addGraph(avgGraph);
    ui->pltIntensity->addGraph(minGraph);
    ui->pltIntensity->addGraph(maxGraph);
    ui->pltIntensity->addGraph(rangeGraph);
    ui->pltIntensity->addGraph(avgFit);
    ui->pltIntensity->get_plotter()->addOverlayElement(avgIndicator);
    ui->pltIntensity->getXAxis()->set_axisLabel(tr("time [seconds]"));
    ui->pltIntensity->getYAxis()->set_axisLabel(tr("intensity [A.U.]"));



    ui->cmbGraph->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("corrected intenisty: average +/- stddev."));
    ui->cmbGraph->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("corrected intenisty: average +/- stddev., min, max"));
    ui->cmbGraph->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("corrected intenisty: average"));
    ui->cmbGraph->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("corrected intenisty: standard deviation"));
    ui->cmbGraph->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("corrected intenisty: minimum"));
    ui->cmbGraph->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("corrected intenisty: maximum"));

    ui->cmbGraph->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("background: average +/- stddev."));
    ui->cmbGraph->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("background: average +/- stddev., min, max"));
    ui->cmbGraph->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("background: average"));
    ui->cmbGraph->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("background: standard deviation"));
    ui->cmbGraph->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("background: minimum"));
    ui->cmbGraph->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("background: maximum"));


    ui->cmbGraph->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("uncorrected: average +/- stddev."));
    ui->cmbGraph->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("uncorrected: average +/- stddev., min, max"));
    ui->cmbGraph->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("uncorrected: average"));
    ui->cmbGraph->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("uncorrected: standard deviation"));
    ui->cmbGraph->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("uncorrected: minimum"));
    ui->cmbGraph->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("uncorrected: maximum"));

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
    ui->pltIntensity->set_doDrawing(false);
    avgIndicator->set_position(pos);
    ui->pltIntensity->set_doDrawing(true);
    ui->pltIntensity->update_overlays();
}

void QFRDRImagingFCSCountrateDisplay::rawDataChanged() {
}

void QFRDRImagingFCSCountrateDisplay::displayData() {
    if (!avgGraph) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    JKQTPdatastore* ds=ui->pltIntensity->get_plotter()->getDatastore();
    data=NULL;
    dataN=0;
    dataT=NULL;
    if (!current || !m) {
        ds->clear();
        ui->pltIntensity->set_doDrawing(false);

        avgGraph->set_xColumn(-1);
        avgGraph->set_yColumn(-1);
        avgGraph->set_xErrorColumn(-1);
        avgGraph->set_yErrorColumn(-1);


        ui->pltIntensity->zoomToFit();
        ui->pltIntensity->set_doDrawing(true);
        ui->pltIntensity->update_plot();
        ui->edtData->clear();
        ui->labInfo->clear();
    } else {

        JKQTPdatastore* ds=ui->pltIntensity->get_plotter()->getDatastore();
        ds->clear();
        ui->pltIntensity->set_doDrawing(false);
        int colT=ds->addColumn(m->getStatisticsT(), m->getStatisticsN(), "time");
        int colAvg=ds->addColumn(m->getStatisticsMean(), m->getStatisticsN(), "avg");
        int colStddev=ds->addColumn(m->getStatisticsStdDev(), m->getStatisticsN(), "stddev");
        int colMin=ds->addColumn(m->getStatisticsMin(), m->getStatisticsN(), "min");
        int colMax=ds->addColumn(m->getStatisticsMax(), m->getStatisticsN(), "max");
        int bcolT=ds->addColumn(m->getBackgroundStatisticsT(), m->getBackgroundStatisticsN(), "back time");
        int bcolAvg=ds->addColumn(m->getBackgroundStatisticsMean(), m->getBackgroundStatisticsN(), "back avg");
        int bcolStddev=ds->addColumn(m->getBackgroundStatisticsStdDev(), m->getBackgroundStatisticsN(), "back stddev");
        int bcolMin=ds->addColumn(m->getBackgroundStatisticsMin(), m->getBackgroundStatisticsN(), "back min");
        int bcolMax=ds->addColumn(m->getBackgroundStatisticsMax(), m->getBackgroundStatisticsN(), "back max");

        int ucolT=ds->addColumn(m->getUncorrectedStatisticsT(), m->getUncorrectedStatisticsN(), "uncorrected time");
        int ucolAvg=ds->addColumn(m->getUncorrectedStatisticsMean(), m->getUncorrectedStatisticsN(), "uncorrected avg");
        int icolStddev=ds->addColumn(m->getUncorrectedStatisticsStdDev(), m->getUncorrectedStatisticsN(), "uncorrected stddev");
        int icolMin=ds->addColumn(m->getUncorrectedStatisticsMin(), m->getUncorrectedStatisticsN(), "uncorrected min");
        int icolMax=ds->addColumn(m->getUncorrectedStatisticsMax(), m->getUncorrectedStatisticsN(), "uncorrected max");

        double varAvg;
        double meanAvg=statisticsAverageVariance(varAvg, m->getStatisticsMean(), m->getStatisticsN());
        double varStddev;
        double meanStddev=statisticsAverageVariance(varStddev, m->getStatisticsStdDev(), m->getStatisticsN());
        double varMin;
        double meanMin=statisticsAverageVariance(varMin, m->getStatisticsMin(), m->getStatisticsN());
        double varMax;
        double meanMax=statisticsAverageVariance(varMax, m->getStatisticsMax(), m->getStatisticsN());

        double bvarAvg;
        double bmeanAvg=statisticsAverageVariance(bvarAvg, m->getBackgroundStatisticsMean(), m->getBackgroundStatisticsN());
        double bvarStddev;
        double bmeanStddev=statisticsAverageVariance(bvarStddev, m->getBackgroundStatisticsStdDev(), m->getBackgroundStatisticsN());
        double bvarMin;
        double bmeanMin=statisticsAverageVariance(bvarMin, m->getBackgroundStatisticsMin(), m->getBackgroundStatisticsN());
        double bvarMax;
        double bmeanMax=statisticsAverageVariance(bvarMax, m->getBackgroundStatisticsMax(), m->getBackgroundStatisticsN());

        double uvarAvg;
        double umeanAvg=statisticsAverageVariance(uvarAvg, m->getUncorrectedStatisticsMean(), m->getUncorrectedStatisticsN());
        double uvarStddev;
        double umeanStddev=statisticsAverageVariance(uvarStddev, m->getUncorrectedStatisticsStdDev(), m->getUncorrectedStatisticsN());
        double uvarMin;
        double umeanMin=statisticsAverageVariance(uvarMin, m->getUncorrectedStatisticsMin(), m->getUncorrectedStatisticsN());
        double uvarMax;
        double umeanMax=statisticsAverageVariance(uvarMax, m->getUncorrectedStatisticsMax(), m->getUncorrectedStatisticsN());

        double dispMean, dispStddev;

        if (ui->cmbGraph->currentIndex()<=5) {
            dataT=m->getStatisticsT();
        } else if (ui->cmbGraph->currentIndex()<=11) {
            dataT=m->getBackgroundStatisticsT();
        } else {
            dataT=m->getUncorrectedStatisticsT();
        }

        if (ui->cmbGraph->currentIndex()==0) {
            avgGraph->set_xColumn(colT);
            avgGraph->set_yColumn(colAvg);
            avgGraph->set_yErrorColumn(colStddev);
            avgGraph->set_title(tr("avg \\pm stddev"));
            minGraph->set_xColumn(-1);
            minGraph->set_yColumn(-1);
            maxGraph->set_xColumn(-1);
            maxGraph->set_yColumn(-1);
            dispMean=meanAvg; dispStddev=sqrt(varAvg);
            data=m->getStatisticsMean();
            dataN=m->getStatisticsN();
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
            dispMean=meanAvg; dispStddev=sqrt(varAvg);
            data=m->getStatisticsMean();
            dataN=m->getStatisticsN();
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
            dispMean=meanAvg; dispStddev=sqrt(varAvg);
            data=m->getStatisticsMean();
            dataN=m->getStatisticsN();
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
            dispMean=meanStddev; dispStddev=sqrt(varStddev);
            data=m->getStatisticsStdDev();
            dataN=m->getStatisticsN();
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
            dispMean=meanMin; dispStddev=sqrt(varMin);
            data=m->getStatisticsMin();
            dataN=m->getStatisticsN();
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
            dispMean=meanMax; dispStddev=sqrt(varMax);
            data=m->getStatisticsMax();
            dataN=m->getStatisticsN();







        } else if (ui->cmbGraph->currentIndex()==6) {
            avgGraph->set_xColumn(bcolT);
            avgGraph->set_yColumn(bcolAvg);
            avgGraph->set_yErrorColumn(bcolStddev);
            avgGraph->set_title(tr("avg \\pm stddev"));
            minGraph->set_xColumn(-1);
            minGraph->set_yColumn(-1);
            maxGraph->set_xColumn(-1);
            maxGraph->set_yColumn(-1);
            dispMean=bmeanAvg; dispStddev=sqrt(bvarAvg);
            data=m->getBackgroundStatisticsMean();
            dataN=m->getBackgroundStatisticsN();
        } else if (ui->cmbGraph->currentIndex()==7) {
            avgGraph->set_xColumn(bcolT);
            avgGraph->set_yColumn(bcolAvg);
            avgGraph->set_yErrorColumn(bcolStddev);
            minGraph->set_xColumn(bcolT);
            minGraph->set_yColumn(bcolMin);
            maxGraph->set_xColumn(bcolT);
            maxGraph->set_yColumn(bcolMax);
            avgGraph->set_title(tr("avg \\pm stddev"));
            minGraph->set_title(tr("minimum"));
            maxGraph->set_title(tr("maximum"));
            dispMean=bmeanAvg; dispStddev=sqrt(bvarAvg);
            data=m->getBackgroundStatisticsMean();
            dataN=m->getBackgroundStatisticsN();
        } else if (ui->cmbGraph->currentIndex()==8) {
            avgGraph->set_xColumn(bcolT);
            avgGraph->set_yColumn(bcolAvg);
            avgGraph->set_yErrorColumn(-1);
            minGraph->set_xColumn(-1);
            minGraph->set_yColumn(-1);
            maxGraph->set_xColumn(-1);
            maxGraph->set_yColumn(-1);
            avgGraph->set_title(tr("average"));
            minGraph->set_title("");
            maxGraph->set_title("");
            dispMean=bmeanAvg; dispStddev=sqrt(bvarAvg);
            data=m->getBackgroundStatisticsMean();
            dataN=m->getBackgroundStatisticsN();
        } else if (ui->cmbGraph->currentIndex()==9) {
            avgGraph->set_xColumn(bcolT);
            avgGraph->set_yColumn(bcolStddev);
            avgGraph->set_yErrorColumn(-1);
            minGraph->set_xColumn(-1);
            minGraph->set_yColumn(-1);
            maxGraph->set_xColumn(-1);
            maxGraph->set_yColumn(-1);
            avgGraph->set_title(tr("stddev"));
            minGraph->set_title("");
            maxGraph->set_title("");
            dispMean=bmeanStddev; dispStddev=sqrt(bvarStddev);
            data=m->getBackgroundStatisticsStdDev();
            dataN=m->getBackgroundStatisticsN();
        } else if (ui->cmbGraph->currentIndex()==10) {
            avgGraph->set_xColumn(bcolT);
            avgGraph->set_yColumn(bcolMin);
            avgGraph->set_yErrorColumn(-1);
            minGraph->set_xColumn(-1);
            minGraph->set_yColumn(-1);
            maxGraph->set_xColumn(-1);
            maxGraph->set_yColumn(-1);
            avgGraph->set_title(tr("minimum"));
            minGraph->set_title("");
            maxGraph->set_title("");
            dispMean=bmeanMin; dispStddev=sqrt(bvarMin);
            data=m->getBackgroundStatisticsMin();
            dataN=m->getBackgroundStatisticsN();
        } else if (ui->cmbGraph->currentIndex()==11) {
            avgGraph->set_xColumn(bcolT);
            avgGraph->set_yColumn(bcolMax);
            avgGraph->set_yErrorColumn(-1);
            minGraph->set_xColumn(-1);
            minGraph->set_yColumn(-1);
            maxGraph->set_xColumn(-1);
            maxGraph->set_yColumn(-1);
            avgGraph->set_title(tr("maximum"));
            minGraph->set_title("");
            maxGraph->set_title("");
            dispMean=bmeanMax; dispStddev=sqrt(bvarMax);
            data=m->getBackgroundStatisticsMax();
            dataN=m->getBackgroundStatisticsN();
        } else if (ui->cmbGraph->currentIndex()==12) {
            avgGraph->set_xColumn(bcolT);
            avgGraph->set_yColumn(bcolAvg);
            avgGraph->set_yErrorColumn(bcolStddev);
            avgGraph->set_title(tr("avg \\pm stddev"));
            minGraph->set_xColumn(-1);
            minGraph->set_yColumn(-1);
            maxGraph->set_xColumn(-1);
            maxGraph->set_yColumn(-1);
            dispMean=bmeanAvg; dispStddev=sqrt(bvarAvg);
            data=m->getUncorrectedStatisticsMean();
            dataN=m->getUncorrectedStatisticsN();
        } else if (ui->cmbGraph->currentIndex()==13) {
            avgGraph->set_xColumn(bcolT);
            avgGraph->set_yColumn(bcolAvg);
            avgGraph->set_yErrorColumn(bcolStddev);
            minGraph->set_xColumn(bcolT);
            minGraph->set_yColumn(bcolMin);
            maxGraph->set_xColumn(bcolT);
            maxGraph->set_yColumn(bcolMax);
            avgGraph->set_title(tr("avg \\pm stddev"));
            minGraph->set_title(tr("minimum"));
            maxGraph->set_title(tr("maximum"));
            dispMean=bmeanAvg; dispStddev=sqrt(bvarAvg);
            data=m->getUncorrectedStatisticsMean();
            dataN=m->getUncorrectedStatisticsN();
        } else if (ui->cmbGraph->currentIndex()==14) {
            avgGraph->set_xColumn(bcolT);
            avgGraph->set_yColumn(bcolAvg);
            avgGraph->set_yErrorColumn(-1);
            minGraph->set_xColumn(-1);
            minGraph->set_yColumn(-1);
            maxGraph->set_xColumn(-1);
            maxGraph->set_yColumn(-1);
            avgGraph->set_title(tr("average"));
            minGraph->set_title("");
            maxGraph->set_title("");
            dispMean=bmeanAvg; dispStddev=sqrt(bvarAvg);
            data=m->getUncorrectedStatisticsMean();
            dataN=m->getUncorrectedStatisticsN();
        } else if (ui->cmbGraph->currentIndex()==15) {
            avgGraph->set_xColumn(bcolT);
            avgGraph->set_yColumn(bcolStddev);
            avgGraph->set_yErrorColumn(-1);
            minGraph->set_xColumn(-1);
            minGraph->set_yColumn(-1);
            maxGraph->set_xColumn(-1);
            maxGraph->set_yColumn(-1);
            avgGraph->set_title(tr("stddev"));
            minGraph->set_title("");
            maxGraph->set_title("");
            dispMean=bmeanStddev; dispStddev=sqrt(bvarStddev);
            data=m->getUncorrectedStatisticsStdDev();
            dataN=m->getUncorrectedStatisticsN();
        } else if (ui->cmbGraph->currentIndex()==16) {
            avgGraph->set_xColumn(bcolT);
            avgGraph->set_yColumn(bcolMin);
            avgGraph->set_yErrorColumn(-1);
            minGraph->set_xColumn(-1);
            minGraph->set_yColumn(-1);
            maxGraph->set_xColumn(-1);
            maxGraph->set_yColumn(-1);
            avgGraph->set_title(tr("minimum"));
            minGraph->set_title("");
            maxGraph->set_title("");
            dispMean=bmeanMin; dispStddev=sqrt(bvarMin);
            data=m->getUncorrectedStatisticsMin();
            dataN=m->getUncorrectedStatisticsN();
        } else if (ui->cmbGraph->currentIndex()==17) {
            avgGraph->set_xColumn(bcolT);
            avgGraph->set_yColumn(bcolMax);
            avgGraph->set_yErrorColumn(-1);
            minGraph->set_xColumn(-1);
            minGraph->set_yColumn(-1);
            maxGraph->set_xColumn(-1);
            maxGraph->set_yColumn(-1);
            avgGraph->set_title(tr("maximum"));
            minGraph->set_title("");
            maxGraph->set_title("");
            dispMean=bmeanMax; dispStddev=sqrt(bvarMax);
            data=m->getUncorrectedStatisticsMax();
            dataN=m->getUncorrectedStatisticsN();
        }


        rangeGraph->set_rangeCenter(dispMean);
        rangeGraph->set_rangeMin(dispMean-dispStddev);
        rangeGraph->set_rangeMax(dispMean+dispStddev);
        ui->pltIntensity->zoomToFit();
        ui->pltIntensity->set_doDrawing(true);
        ui->pltIntensity->update_plot();
        ui->edtData->setText(current->getProperty("imfcs_crddisplay_fitresults", QString("")).toString());
        ui->labInfo->setText(tr("<table width=\"95%\"><tr><td>average of graph: %2</td><td>data points: %1</td></tr>"
                                "<tr><td>stddev of graph: %3</td><td>measurement duration: %4 seconds</td></tr></table>").arg(m->getStatisticsN()).arg(dispMean).arg(dispStddev).arg(current->getProperty("MEASUREMENT_DURATION_MS", 0).toDouble()/1000.0));
    }
    updateFitFuncPlot();
    QApplication::restoreOverrideCursor();
}


double ffExp( double t, const double *par )
{
    const double tau=par[2];
    const double A=par[1];
    const double B=par[0];
    return B+A*exp(-t/tau);

}

double fExpPlot( double t, void *param )
{
    return ffExp(t, (double*)param);
}

double fExp2( double t, const double *par )
{
    const double tau2=par[4];
    const double A2=par[3];
    const double tau=par[2];
    const double A=par[1];
    const double B=par[0];

    return B+A*exp(-t/tau)+A2*exp(-t/tau2);

}

double fExp2Plot( double t, void *param )
{
    return fExp2(t, (double*)param);

}

void QFRDRImagingFCSCountrateDisplay::calcExpFit() {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m || !data || dataN<=0) return;


    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    lm_control_struct control=lm_control_double;
    lm_status_struct status;

    srand(time(0));
    double start=statisticsAverage(data, qMin(5,dataN));
    double end=statisticsAverage(&(data[qMax(0,dataN-6)]), qMin(5,dataN));
    double Tmax=dataT[dataN-1];
    expParam[0]=end;
    expParam[1]=start-end;
    expParam[2]=Tmax/2;
    //lmmin( 3, par, end-start+1, (const void*) &avg[start], fExpC, &control, &status, NULL );
    lmcurve_fit2(3, expParam, dataN, dataT, data, ffExp, &control, &status);



    QString result=tr("<b>fit results:</b><br>&nbsp;&nbsp;&nbsp;&nbsp;decay time &tau; = %4 seconds<br>&nbsp;&nbsp;&nbsp;&nbsp;amplitude A = %5<br>&nbsp;&nbsp;&nbsp;&nbsp;offset B = %6<br>&nbsp;&nbsp;&nbsp;&nbsp;model: <i>f(x) = B + A&middot;</i>exp<i>(-t/&tau)</i><br>"
                      "<b>fit algorithm message:</b> %1<br><b>iterations:</b> %2,&nbsp;&nbsp;&nbsp;&nbsp; <b>&chi;<sup>2</sup></b> = %3").arg(QString(lm_infmsg[status.info])).arg(status.nfev).arg(status.fnorm).arg(expParam[2]).arg(expParam[1]).arg(expParam[0]);
    ui->edtData->setText(result);
    current->setQFProperty("imfcs_crddisplay_fitresults", result);
    current->setQFProperty("imfcs_crddisplay_fitfunc", "exp");
    current->setQFProperty("imfcs_crddisplay_fitparam_exp_0", expParam[0]);
    current->setQFProperty("imfcs_crddisplay_fitparam_exp_1", expParam[1]);
    current->setQFProperty("imfcs_crddisplay_fitparam_exp_2", expParam[2]);

    updateFitFuncPlot();
    QApplication::restoreOverrideCursor();
}

void QFRDRImagingFCSCountrateDisplay::calc2ExpFit() {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m || !data || dataN<=0) return;


    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    lm_control_struct control=lm_control_double;
    lm_status_struct status;

    srand(time(0));
    double start=statisticsAverage(data, qMin(5,dataN));
    double end=statisticsAverage(&(data[qMax(0,dataN-6)]), qMin(5,dataN));
    double Tmax=dataT[dataN-1];
    exp2Param[0]=end;
    exp2Param[1]=start-end;
    exp2Param[2]=Tmax/2;
    exp2Param[3]=start-end;
    exp2Param[4]=Tmax/20;
    //lmmin( 3, par, end-start+1, (const void*) &avg[start], fExpC, &control, &status, NULL );
    lmcurve_fit2(3, exp2Param, dataN, dataT, data, ffExp, &control, &status);
    exp2Param[3]=exp2Param[1]/10.0;
    exp2Param[4]=exp2Param[2]*10.0;
    lmcurve_fit2(5, exp2Param, dataN, dataT, data, fExp2, &control, &status);



    QString result=tr("<b>fit results:</b><br>&nbsp;&nbsp;&nbsp;&nbsp;decay time &tau;<sub>1</sub> = %4 seconds<br>&nbsp;&nbsp;&nbsp;&nbsp;amplitude A<sub>1</sub> = %5<br>&nbsp;&nbsp;&nbsp;&nbsp;decay time &tau;<sub>2</sub> = %8 seconds<br>&nbsp;&nbsp;&nbsp;&nbsp;amplitude A<sub>2</sub> = %7<br>&nbsp;&nbsp;&nbsp;&nbsp;offset B = %6<br>&nbsp;&nbsp;&nbsp;&nbsp;model: <i>f(x) = B + A<sub>1</sub>&middot;</i>exp<i>(-t/&tau;<sub>1</sub>) + A<sub>2</sub>&middot;</i>exp<i>(-t/&tau;<sub>2</sub>)</i><br>"
                      "<b>fit algorithm message:</b> %1<br><b>iterations:</b> %2,&nbsp;&nbsp;&nbsp;&nbsp; <b>&chi;<sup>2</sup></b> = %3").arg(QString(lm_infmsg[status.info])).arg(status.nfev).arg(status.fnorm).arg(exp2Param[2]).arg(exp2Param[1]).arg(exp2Param[0]).arg(exp2Param[3]).arg(exp2Param[4]);
    ui->edtData->setText(result);
    current->setQFProperty("imfcs_crddisplay_fitresults", result);
    current->setQFProperty("imfcs_crddisplay_fitfunc", "exp");
    current->setQFProperty("imfcs_crddisplay_fitparam_exp_0", exp2Param[0]);
    current->setQFProperty("imfcs_crddisplay_fitparam_exp_1", exp2Param[1]);
    current->setQFProperty("imfcs_crddisplay_fitparam_exp_2", exp2Param[2]);
    current->setQFProperty("imfcs_crddisplay_fitparam_exp_3", exp2Param[3]);
    current->setQFProperty("imfcs_crddisplay_fitparam_exp_4", exp2Param[4]);

    updateFitFuncPlot();
    QApplication::restoreOverrideCursor();
}
void QFRDRImagingFCSCountrateDisplay::readSettings(QSettings& settings, const QString& prefix) {
    jkloadSplitter(settings, ui->splitter, prefix+QString("splitter/"));
}

void QFRDRImagingFCSCountrateDisplay::writeSettings(QSettings &settings, const QString &prefix) {
    jksaveSplitter(settings, ui->splitter, prefix+QString("splitter/"));
}

void QFRDRImagingFCSCountrateDisplay::updateFitFuncPlot() {
    if (!avgGraph || !current) return;
    QString func=current->getProperty("imfcs_crddisplay_fitfunc", "none").toString().toLower();
    if (func=="exp") {
        expParam[0]=current->getProperty("imfcs_crddisplay_fitparam_exp_0", 0).toDouble();
        expParam[1]=current->getProperty("imfcs_crddisplay_fitparam_exp_1", 0).toDouble();
        expParam[2]=current->getProperty("imfcs_crddisplay_fitparam_exp_2", 1).toDouble();
        avgFit->set_plotFunction(fExpPlot);
        avgFit->set_params(expParam);
    } else if (func=="2exp") {
        exp2Param[0]=current->getProperty("imfcs_crddisplay_fitparam_2exp_0", 0).toDouble();
        exp2Param[1]=current->getProperty("imfcs_crddisplay_fitparam_2exp_1", 0).toDouble();
        exp2Param[2]=current->getProperty("imfcs_crddisplay_fitparam_2exp_2", 1).toDouble();
        exp2Param[3]=current->getProperty("imfcs_crddisplay_fitparam_2exp_3", 0).toDouble();
        exp2Param[4]=current->getProperty("imfcs_crddisplay_fitparam_2exp_4", 1).toDouble();
        avgFit->set_plotFunction(fExp2Plot);
        avgFit->set_params(exp2Param);
    } else {
        avgFit->set_plotFunction(NULL);
        avgFit->set_params(NULL);

    }
    ui->pltIntensity->update_plot();

}

