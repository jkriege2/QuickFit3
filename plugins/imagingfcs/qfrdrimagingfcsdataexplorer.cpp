#include "qfrdrimagingfcsdataexplorer.h"
#include "ui_qfrdrimagingfcsdataexplorer.h"
#include "qmodernprogresswidget.h"
#include "statistics_tools.h"
#include <cmath>
#include <QDebug>

#include "qftools.h"
#include "lmcurve.h"
#include <typeinfo>


QFRDRImagingFCSDataExplorer::QFRDRImagingFCSDataExplorer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRImagingFCSDataExplorer)
{
    stddev=NULL;
    initing=false;
    avg=NULL;
    fit=NULL;
    frame=frameRaw=NULL;
    N=0;
    ui->setupUi(this);
    image=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::FloatArray, NULL, 0, 0, JKQTPMathImage::GRAY, ui->pltImage->get_plotter());
    imageRaw=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::FloatArray, NULL, 0, 0, JKQTPMathImage::GRAY, ui->pltImageRaw->get_plotter());
    cropRegion=new JKQTPgeoRectangle(ui->pltImageRaw->get_plotter(), 0,0,1,1,QColor("red"),1);
    binRegion=new JKQTPgeoRectangle(ui->pltImageRaw->get_plotter(), 0,0,1,1,QColor("salmon"),1);
    ui->pltImage->addGraph(image);
    ui->pltImageRaw->addGraph(imageRaw);
    ui->pltImageRaw->addGraph(binRegion);
    ui->pltImageRaw->addGraph(cropRegion);
    fitRange=new JKQTPverticalRange(ui->pltIntensity->get_plotter());
    fitRange->set_plotCenterLine(false);
    QColor col=fitRange->get_fillColor();
    col.setAlphaF(0.5);
    fitRange->set_fillColor(col);
    avgGraph=new JKQTPxyLineErrorGraph(ui->pltIntensity->get_plotter());
    avgGraph->set_title(tr("Avg \\pm StdDev"));
    avgGraph->set_xErrorStyle(JKQTPnoError);
    avgGraph->set_yErrorStyle(JKQTPerrorLines);
    avgGraph->set_drawLine(true);
    avgGraph->set_symbol(JKQTPnoSymbol);
    avgFit=new JKQTPxyLineGraph(ui->pltIntensity->get_plotter());
    avgFit->set_title(tr("fit"));
    avgGraph->set_drawLine(true);
    avgGraph->set_symbol(JKQTPnoSymbol);
    ui->pltIntensity->addGraph(fitRange);
    ui->pltIntensity->addGraph(avgGraph);
    ui->pltIntensity->addGraph(avgFit);
    ui->pltImage->get_plotter()->set_maintainAspectRatio(true);
    ui->pltImageRaw->get_plotter()->set_maintainAspectRatio(true);
    ui->pltImage->get_plotter()->set_maintainAxisAspectRatio(true);
    ui->pltImageRaw->get_plotter()->set_maintainAxisAspectRatio(true);
    ui->pltImage->get_xAxis()->set_minTicks(5);
    ui->pltImage->get_yAxis()->set_minTicks(5);
    ui->pltImageRaw->get_xAxis()->set_minTicks(5);
    ui->pltImageRaw->get_yAxis()->set_minTicks(5);
    ui->spinDecay->setCheckBounds(false, false);
    ui->edtOffset->setCheckBounds(false, false);
    ui->edtAmplitude->setCheckBounds(false, false);

}

QFRDRImagingFCSDataExplorer::~QFRDRImagingFCSDataExplorer()
{
    delete ui;
    if (avg) free(avg);
    if (stddev) free(stddev);
    if (fit) free(fit);
    if (frame) free(frame);
    if (frameRaw) free(frameRaw);
}

void QFRDRImagingFCSDataExplorer::writeSettings(QSettings &settings, QString prefix) {
    saveWidgetGeometry(settings, this, prefix);
    saveSplitter(settings, ui->splitter, prefix+"splitter/");
    settings.setValue(prefix+"play_frames", ui->spinPlay->value());
    settings.setValue(prefix+"play_delay", playDelay);
}

void QFRDRImagingFCSDataExplorer::readSettings(QSettings &settings, QString prefix) {
    loadWidgetGeometry(settings, this, prefix);
    loadSplitter(settings, ui->splitter, prefix+"splitter/");
    ui->spinPlay->setValue(settings.value(prefix+"play_frames", ui->spinPlay->value()).toInt());
    playDelay=settings.value(prefix+"play_delay", playDelay).toInt();
}

void QFRDRImagingFCSDataExplorer::setBleachDecay(double frames) {
    ui->spinDecay->setValue(frames);
}

double QFRDRImagingFCSDataExplorer::getBleachDecay() const {
    return ui->spinDecay->value();
}

void QFRDRImagingFCSDataExplorer::setBleachA(double A) {
    ui->edtAmplitude->setValue(A);
}

double QFRDRImagingFCSDataExplorer::getBleachA() const {
    return ui->edtAmplitude->value();
}

void QFRDRImagingFCSDataExplorer::setBleachB(double B) {
    ui->edtOffset->setValue(B);
}

double QFRDRImagingFCSDataExplorer::getBleachB() const {
    return ui->edtOffset->value();
}


int QFRDRImagingFCSDataExplorer::getBinning() const {
    return ui->spinBinning->value();
}

bool QFRDRImagingFCSDataExplorer::getUseCropping() const {
    return ui->chkCropping->isChecked();
}

int QFRDRImagingFCSDataExplorer::getCropX0() const {
    return ui->spinX0->value();
}

uint32_t QFRDRImagingFCSDataExplorer::getFirst() const {
    return ui->spinFirst->value();
}

uint32_t QFRDRImagingFCSDataExplorer::getLast() const {
    return ui->spinLast->value();
}

bool QFRDRImagingFCSDataExplorer::getUseFirst() const {
    return ui->chkFirst->isChecked();
}

bool QFRDRImagingFCSDataExplorer::getUseLast() const {
    return ui->chkLast->isChecked();
}


int QFRDRImagingFCSDataExplorer::getCropX1() const {
    return ui->spinX1->value();
}

int QFRDRImagingFCSDataExplorer::getCropY0() const {
    return ui->spinY0->value();
}

int QFRDRImagingFCSDataExplorer::getCropY1() const {
    return ui->spinY1->value();
}

bool QFRDRImagingFCSDataExplorer::init(QFRDRImageReader* reader, QFRDRImageReader* readerRaw, const QString& filename, bool useFirst, uint32_t first, bool useLast, uint32_t last, bool use, int x0, int x1, int y0, int y1, int binning) {
    initing=true;
    this->reader=reader;
    this->readerRaw=readerRaw;
    this->filename=filename;
    this->frames=0;

    bool ok=true;

    QString msg=tr("<b>initializing data explorer:</b><br>&nbsp;&nbsp;&nbsp;%1");

    QModernProgressDialog* prg=new QModernProgressDialog(msg.arg(""), tr("Cancel"), this);
    prg->setMode(true, true);
    prg->setRange(0,100);
    prg->setValue(0);
    prg->setLabelText(msg.arg(QString("opening '%1' ...").arg(filename)));
    prg->show();
    QApplication::processEvents();
    QApplication::processEvents();
    ok=ok && readerRaw->open(filename);
    prg->setValue(10);

    if (ok) {
        prg->setLabelText(msg.arg(QString("counting frames ...")));
        QApplication::processEvents();
        QApplication::processEvents();
        this->frames=readerRaw->countFrames();
        prg->setValue(20);
        QApplication::processEvents();

        prg->setLabelText(msg.arg(QString("calculating image statistics ...")));
        QApplication::processEvents();
        QApplication::processEvents();
        imgRawWidth=readerRaw->frameWidth();
        imgRawHeight=readerRaw->frameHeight();
        fileFormat=readerRaw->formatName();

        ui->spinX0->setRange(0, imgRawWidth);
        ui->spinX1->setRange(0, imgRawWidth);
        ui->spinY0->setRange(0, imgRawHeight);
        ui->spinY1->setRange(0, imgRawHeight);
        ui->spinBinning->setRange(1, qMax(imgRawWidth, imgRawHeight));

        ui->chkCropping->setChecked(use);
        ui->spinX0->setValue(x0);
        ui->spinX1->setValue(x1);
        ui->spinY0->setValue(y0);
        ui->spinY1->setValue(y1);
        ui->spinBinning->setValue(binning);

        reader->setBinning(binning);
        reader->setCropping(x0,x1,y0,y1);
        if (!use) reader->unsetCropping();

        ui->chkFirst->setChecked(useFirst);
        ui->spinFirst->setRange(0,this->frames-1);
        ui->spinFirst->setValue(first);
        ui->chkLast->setChecked(useLast);
        ui->spinLast->setRange(0,this->frames-1);
        ui->spinLast->setValue(last);

        ok=ok && reader->open(filename);

        imgWidth=reader->frameWidth();
        imgHeight=reader->frameHeight();
        currentFrame=-1;
        reallocFrames();
        readStatistics(prg);
    }

    ok=ok && prg->wasCanceled();
    prg->close();
    delete prg;
    ui->labInfo->setText(tr("filename: '%1'").arg(filename));
    ui->pltImage->zoomToFit();
    ui->pltImageRaw->zoomToFit();
    ui->pltIntensity->zoomToFit();
    initing=false;
    return ok;
}

void QFRDRImagingFCSDataExplorer::reallocStatistics(uint32_t N) {
    JKQTPdatastore* ds=ui->pltIntensity->get_plotter()->getDatastore();
    avgGraph->set_xColumn(-1);
    avgGraph->set_yColumn(-1);
    avgGraph->set_yErrorColumn(-1);
    avgFit->set_xColumn(-1);
    avgFit->set_yColumn(-1);

    ds->clear();
    double* oldAvg=avg;
    double* oldStd=stddev;
    double* oldFit=fit;
    this->N=0;
    this->Nfit=0;
    uint32_t NF=qMin((uint32_t)500, qMax((uint32_t)10, N/100));
    avg=(double*)malloc(N*sizeof(double));
    stddev=(double*)malloc(N*sizeof(double));
    fit=(double*)malloc(NF*sizeof(double));
    this->N=N;
    this->Nfit=NF;

    size_t colT=ds->addLinearColumn(N,0,N, "t");
    size_t colA=ds->addColumn(avg, N, "avg");
    size_t colS=ds->addColumn(stddev, N, "stddev");
    size_t colTF=ds->addLinearColumn(NF,0,N, "t_fit");
    size_t colF=ds->addColumn(fit, NF, "fit");

    avgGraph->set_xColumn(colT);
    avgGraph->set_yColumn(colA);
    avgGraph->set_yErrorColumn(colS);

    avgFit->set_xColumn(colTF);
    avgFit->set_yColumn(colF);

    if (oldAvg) free(oldAvg);
    if (oldStd) free(oldStd);
    if (oldFit) free(oldFit);

}

void QFRDRImagingFCSDataExplorer::reallocFrames() {
    if (frame) free(frame);
    if (frameRaw) free(frameRaw);

    frame=(float*)malloc(imgWidth*imgHeight*sizeof(float));
    frameRaw=(float*)malloc(imgRawWidth*imgRawHeight*sizeof(float));
}

void QFRDRImagingFCSDataExplorer::skipFrames(int N) {

    bool ok=true;
    int64_t i=0;
    while (i<N && ok) {
        ok=ok && reader->nextFrame();
        ok=ok && readerRaw->nextFrame();
        if (ok) {
            currentFrame++;
        }
        i++;
    }

}

void QFRDRImagingFCSDataExplorer::readFrames(bool next) {
    bool ok=true;
    if (next) {
        ok=ok && reader->nextFrame();
        ok=ok && readerRaw->nextFrame();
        if (ok) {
            currentFrame++;
        }
    }
    image->set_data(NULL, 0, 0);
    imageRaw->set_data(NULL, 0, 0);
    reader->setBinning(ui->spinBinning->value());
    reader->setCropping(ui->spinX0->value(), ui->spinX1->value(), ui->spinY0->value(), ui->spinY1->value());
    if (!ui->chkCropping->isChecked()) reader->unsetCropping();


    imgWidth=reader->frameWidth();
    imgHeight=reader->frameHeight();

    imgRawWidth=readerRaw->frameWidth();
    imgRawHeight=readerRaw->frameHeight();
    reallocFrames();

    reader->readFrameFloat(frame);
    readerRaw->readFrameFloat(frameRaw);

    image->set_data(frame, imgWidth, imgHeight);
    image->set_width(imgWidth);
    image->set_height(imgHeight);
    imageRaw->set_data(frameRaw, imgRawWidth, imgRawHeight);
    imageRaw->set_width(imgRawWidth);
    imageRaw->set_height(imgRawHeight);
    cropRegion->set_bottomleftrectangle(reader->getCropX0(), reader->getCropY0(), reader->getCropX1()-reader->getCropX0(), reader->getCropY1()-reader->getCropY0());
    binRegion->set_bottomleftrectangle(reader->getCropX0(), reader->getCropY0(), reader->getBinning(), reader->getBinning());

    ui->pltImage->setAbsoluteXY(0,imgWidth,0, imgHeight);
    ui->pltImageRaw->setAbsoluteXY(0,imgRawWidth,0, imgRawHeight);
   // qDebug()<<imgWidth<<imgHeight<<imgRawWidth<<imgRawHeight;
    double imgAspect=(double)imgWidth/(double)imgHeight;
    double imgRawAspect=(double)imgRawWidth/(double)imgRawHeight;
    if (imgAspect<0.05 || imgAspect>20) {
        ui->pltImage->get_plotter()->set_axisAspectRatio(1);
        ui->pltImage->get_plotter()->set_aspectRatio(1);
    } else {
        ui->pltImage->get_plotter()->set_axisAspectRatio(imgAspect);
        ui->pltImage->get_plotter()->set_aspectRatio(imgAspect);
    }
    if (imgRawAspect<0.05 || imgRawAspect>20) {
        ui->pltImageRaw->get_plotter()->set_axisAspectRatio(1);
        ui->pltImageRaw->get_plotter()->set_aspectRatio(1);
    } else {
        ui->pltImageRaw->get_plotter()->set_axisAspectRatio(imgRawAspect);
        ui->pltImageRaw->get_plotter()->set_aspectRatio(imgRawAspect);
    }


    uint32_t start, end;
    getDataRange(start, end);
    ui->pltImage->update_plot();
    ui->pltImageRaw->update_plot();
    ui->pltImageRaw->zoomToFit();
    ui->labImageStatus->setText(tr("frame %1/%2&nbsp;&nbsp;&nbsp;&nbsp;%7<br>raw size: %3&times;%4&nbsp;&nbsp;&nbsp;&nbsp;size: %5&times;%6")
                                .arg(currentFrame+1).arg(frames).arg(imgRawWidth).arg(imgRawHeight).arg(imgWidth).arg(imgHeight)
                                .arg((currentFrame>=start && currentFrame<=end)?tr("<font color=\"green\">OK</font>"):tr("<font color=\"red\">Out Of Range</font>")));
}

bool QFRDRImagingFCSDataExplorer::readStatistics(QModernProgressDialog *prg) {
    uint32_t cnt=0;
    reader->reset();
    readerRaw->reset();
    currentFrame=0;
    reallocStatistics(this->frames);
    if (prg) prg->setRange(0,this->frames);
    do {
        reader->readFrameFloat(frame);
        double var=0;
        avg[cnt]=statisticsAverageVariance(var, frame, imgWidth*imgHeight);
        stddev[cnt]=sqrt(var);
        cnt++;
        if (prg && (this->frames>120)) {
            if (cnt%(this->frames/120)==0) {
                prg->setValue(cnt);
                QApplication::processEvents();
            }
        }
    } while (reader->nextFrame());


    reader->reset();
    updateRange(false);
    readFrames(false);
    calcFit();
    ui->pltIntensity->zoomToFit();

    ui->labIntensityInfo->setText(tr(""));

    if (prg) return !prg->wasCanceled();
    else return true;
}

void QFRDRImagingFCSDataExplorer::on_btnNext_clicked() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    readFrames(true);
    QApplication::restoreOverrideCursor();
}

void QFRDRImagingFCSDataExplorer::on_btnNext100_clicked() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    skipFrames(99);
    readFrames(true);
    QApplication::restoreOverrideCursor();
}

void QFRDRImagingFCSDataExplorer::on_btnPlay_clicked() {
    nextPlayFrame();
}

void QFRDRImagingFCSDataExplorer::nextPlayFrame() {
    if (ui->spinPlay->value()>1) skipFrames(ui->spinPlay->value()-1);
    readFrames(true);
    if (ui->btnPlay->isChecked()) QTimer::singleShot(qMax(10,playDelay), this, SLOT(nextPlayFrame()));
}


void QFRDRImagingFCSDataExplorer::on_btnFirst_clicked() {
    reader->reset();
    readerRaw->reset();
    currentFrame=0;
    readFrames(false);
}

void QFRDRImagingFCSDataExplorer::rereadFrame() {
    if (initing) return;
    readFrames(false);
    ui->pltImage->zoomToFit();
    ui->pltImageRaw->zoomToFit();

}

void QFRDRImagingFCSDataExplorer::calcAvgFrontBack(double& avgFront, double& avgEnd) {
    int avgFrontPoints=qMin((uint32_t)10, N);
    int avgEndPoints=qMin((uint32_t)10, N);
    avgFront=0;
    avgEnd=0;

    for (int i=0; i<avgFrontPoints; i++) {
        avgFront+=avg[i];
    }
    avgFront/=(double)avgFrontPoints;

    for (int i=0; i<avgEndPoints; i++) {
        avgEnd+=avg[N-1-i];
    }
    avgEnd/=(double)avgEndPoints;

}

void QFRDRImagingFCSDataExplorer::getDataRange(uint32_t &start, uint32_t &end) {
    start=0;
    if (!ui->chkFirst->isChecked()) start=ui->spinFirst->value();

    end=N-1;
    if (!ui->chkLast->isChecked()) end=ui->spinLast->value();

    if (start>=N) start=N-1;
    if (end>=N) end=N-1;

    if (start>end) qSwap(start, end);
}

void QFRDRImagingFCSDataExplorer::calcFit() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    double A=ui->edtAmplitude->value();
    double B=ui->edtOffset->value();
    double tau=ui->spinDecay->value();
    for (uint32_t i=0; i<Nfit; i++) {
        double t=(double)i/(double)Nfit*(double)N;
        fit[i]=B+A*exp(-t/tau);
    }
    ui->pltIntensity->update_plot();
    QApplication::restoreOverrideCursor();
}

void QFRDRImagingFCSDataExplorer::updateRange(bool doReplot) {
    if (doReplot) QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    uint32_t start, end;
    getDataRange(start, end);

    fitRange->set_rangeMin(start);
    fitRange->set_rangeMax(end);
    if (doReplot) {
        ui->pltIntensity->update_plot();
        QApplication::restoreOverrideCursor();
    }
}



















void fExpC( const double *par, int m_dat, const void *data,
                       double *fvec, int *info )
{
    register int i;
    const double* I=(double*)data;
    const double tau=par[0];
    const double A=par[1];
    const double B=par[2];
    for ( i = 0; i < m_dat; i++ ) fvec[i] = I[i]-(B+A*exp(-((double)i)/tau));
}

double fExp( double t, const double *par )
{
    const double tau=par[0];
    const double A=par[1];
    const double B=par[2];
    return B+A*exp(-t/tau);

}

void QFRDRImagingFCSDataExplorer::on_btnFit_clicked() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    uint32_t start, end;
    getDataRange(start, end);
    //qDebug()<<start<<end;


    lm_control_struct control=lm_control_double;
    lm_status_struct status;
    double dataI[500];
    double dataT[500];
    srand(123456);
    for (int64_t i=0; i<500; i++) {
        //int64_t idx=start+rand()*(int64_t)(end-start+1)/RAND_MAX;
        int64_t idx=(int64_t)floor((double)start+(double)i/500*(double)(end-start+1));
        if (idx<0) idx=0;
        if (idx>=N) idx=N-1;
        dataT[i]=idx-start;
        dataI[i]=avg[idx];
    }
    srand(time(0));
    double par[3]={ui->spinDecay->value(), ui->edtAmplitude->value(), ui->edtOffset->value()};
    //lmmin( 3, par, end-start+1, (const void*) &avg[start], fExpC, &control, &status, NULL );
    lmcurve_fit(3, par, 500, dataT, dataI, fExp, &control, &status);

    //qDebug()<<par[0]<<par[1]<<par[2];
    ui->spinDecay->setValue(par[0]);
    ui->edtAmplitude->setValue(par[1]);
    ui->edtOffset->setValue(par[2]);

    ui->labFitResult->setText(tr("fit result: %1<br>after %2 iterations, &chi;<sup>2</sup> = %3").arg(QString(lm_infmsg[status.info])).arg(status.nfev).arg(status.fnorm));

    calcFit();
    QApplication::restoreOverrideCursor();
}





