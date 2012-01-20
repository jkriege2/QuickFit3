#include "qfrdrimagingfcsdataexplorer.h"
#include "ui_qfrdrimagingfcsdataexplorer.h"
#include "qmodernprogresswidget.h"
#include "statistics_tools.h"
#include <cmath>
#include <QDebug>

QFRDRImagingFCSDataExplorer::QFRDRImagingFCSDataExplorer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRImagingFCSDataExplorer)
{
    stddev=NULL;
    avg=NULL;
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
}

QFRDRImagingFCSDataExplorer::~QFRDRImagingFCSDataExplorer()
{
    delete ui;
    if (avg) free(avg);
    if (stddev) free(stddev);
    if (frame) free(frame);
    if (frameRaw) free(frameRaw);
}

void QFRDRImagingFCSDataExplorer::setBleachDecay(double frames) {
    ui->spinDecay->setValue(frames);
}

double QFRDRImagingFCSDataExplorer::getBleachDecay() const {
    return ui->spinDecay->value();
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


int QFRDRImagingFCSDataExplorer::getCropX1() const {
    return ui->spinX1->value();
}

int QFRDRImagingFCSDataExplorer::getCropY0() const {
    return ui->spinY0->value();
}

int QFRDRImagingFCSDataExplorer::getCropY1() const {
    return ui->spinY1->value();
}

bool QFRDRImagingFCSDataExplorer::init(QFRDRImageReader* reader, QFRDRImageReader* readerRaw, const QString& filename, bool use, int x0, int x1, int y0, int y1, int binning) {
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
    return ok;
}

void QFRDRImagingFCSDataExplorer::reallocStatistics(uint32_t N) {
    JKQTPdatastore* ds=ui->pltIntensity->get_plotter()->getDatastore();
    ds->clear();
    size_t colT=ds->addLinearColumn(N,0,N);
    double* oldAvg=avg;
    double* oldStd=stddev;
    this->N=0;
    avg=(double*)malloc(N*sizeof(double));
    stddev=(double*)malloc(N*sizeof(double));
    this->N=N;

    size_t colA=ds->addColumn(avg, N, "avg");
    size_t colS=ds->addColumn(stddev, N, "stddev");

    avgGraph->set_xColumn(colT);
    avgGraph->set_yColumn(colA);
    avgGraph->set_yErrorColumn(colS);

    if (oldAvg) free(oldAvg);
    if (oldStd) free(oldStd);
    ui->pltIntensity->zoomToFit();
}

void QFRDRImagingFCSDataExplorer::reallocFrames() {
    if (frame) free(frame);
    if (frameRaw) free(frameRaw);

    frame=(float*)malloc(imgWidth*imgHeight*sizeof(float));
    frameRaw=(float*)malloc(imgRawWidth*imgRawHeight*sizeof(float));
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



    ui->pltImage->update_plot();
    ui->pltImageRaw->update_plot();
    ui->pltImageRaw->zoomToFit();
    ui->labImageStatus->setText(tr("frame %1/%2<br>raw size: %3&times;%4&nbsp;&nbsp;&nbsp;&nbsp;size: %5&times;%6").arg(currentFrame+1).arg(frames).arg(imgRawWidth).arg(imgRawHeight).arg(imgWidth).arg(imgHeight));
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
    readFrames(false);

    ui->labIntensityInfo->setText(tr(""));

    if (prg) return !prg->wasCanceled();
    else return true;
}

void QFRDRImagingFCSDataExplorer::on_btnNext_clicked() {
    readFrames(true);
}

void QFRDRImagingFCSDataExplorer::on_btnFirst_clicked() {
    reader->reset();
    readerRaw->reset();
    currentFrame=0;
    readFrames(false);
}

void QFRDRImagingFCSDataExplorer::rereadFrame() {
    readFrames(false);
    ui->pltImage->zoomToFit();
    ui->pltImageRaw->zoomToFit();

}

void QFRDRImagingFCSDataExplorer::on_btnFit_clicked() {
}



