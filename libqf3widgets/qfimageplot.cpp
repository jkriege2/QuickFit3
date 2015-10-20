#include "qfimageplot.h"
#include "ui_qfimageplot.h"
#include "programoptions.h"
#include "qftools.h"
#include "statistics_tools.h"
#include "qfimporterimageseries.h"
#include "qfimportermanager.h"
#include "qfimporter.h"
#include "image_tools.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#  include <QtConcurrent/QtConcurrent>
#else
#  include <QtConcurrentRun>
#endif
#include <QFuture>


QFImagePlot::QFImagePlot(QWidget *parent, const QString &prefix):
    QWidget(parent),
    ui(new Ui::QFImagePlot)
{
    colROI=QColor("red");
    colROI2=QColor("blue");
    colFillROI=Qt::transparent;
    colFillROI2=Qt::transparent;

    plteImage=NULL;
    binning=1;
    image_data=NULL;
    image_width=0;
    image_height=0;
    mask=NULL;
    this->prefix=prefix;
    ui->setupUi(this);

    ProgramOptions::getConfigWindowGeometry(this, prefix+"window");
    ProgramOptions::getConfigQRadioButton(ui->chkRangeFull, prefix+"chkRangeFull");
    ProgramOptions::getConfigQRadioButton(ui->chkRangeUser, prefix+"chkRangeUser");
    ProgramOptions::getConfigQComboBox(ui->cmbCOlorbar, prefix+"cmbCOlorbar");
    if (ui->chkRangeUser->isChecked()) {
        qfGetConfigQFDoubleEdit(ui->edtRangeMin, prefix+"edtRangeMin");
        qfGetConfigQFDoubleEdit(ui->edtRangeMax, prefix+"edtRangeMax");
    }
    ui->histogram->readSettings(*(ProgramOptions::getInstance()->getQSettings()), prefix+"histogram/");
    connect(ui->image, SIGNAL(plotMouseMove(double,double)), this, SLOT(plotMouseMove(double,double)));
    ui->tabWidget->setCurrentIndex(0);
}

QFImagePlot::~QFImagePlot()
{
    ProgramOptions::setConfigWindowGeometry(this, prefix+"window");
    ui->histogram->writeSettings(*(ProgramOptions::getInstance()->getQSettings()), prefix+"histogram/");
    ProgramOptions::setConfigQRadioButton(ui->chkRangeFull, prefix+"chkRangeFull");
    ProgramOptions::setConfigQRadioButton(ui->chkRangeUser, prefix+"chkRangeUser");
    ProgramOptions::setConfigQComboBox(ui->cmbCOlorbar, prefix+"cmbCOlorbar");
    if (ui->chkRangeUser->isChecked()) {
        qfSetConfigQFDoubleEdit(ui->edtRangeMin, prefix+"edtRangeMin");
        qfSetConfigQFDoubleEdit(ui->edtRangeMax, prefix+"edtRangeMax");
    }
    delete ui;
    clearData();
}

void QFImagePlot::setImage(const double *image, int32_t width, int32_t height)
{
    clear();
    ui->histogram->clear();
    if (image && width>0 && height>0) {
        image_data=qfDuplicateArray(image, width*height);
        image_width=width;
        image_height=height;
        ui->histogram->addHistogram(tr("full"), image_data, image_width*image_height, true);
    }
    ui->histogram->updateHistogram(true);
    //qDebug()<<image_data<<image_width<<image_height;
    new_plots();

}

void QFImagePlot::setMaskAround(int size)
{
    if (mask) qfFree(mask);
    mask=qfMallocT<bool>(image_width*image_height);
    for (int y=0; y<image_height; y++) {
        for (int x=0; x<image_width; x++) {
            int32_t idx=y*image_width+x;
            mask[idx]=(x<size || x>image_width-size || y<size || y>image_height-size);
        }
    }
    new_plots();
}

void QFImagePlot::setROI(double x, double y, double width, double height)
{
    setROI(QRectF(x,y,width,height));
}

void QFImagePlot::setROI(QRectF r)
{
    roi=r;
    new_plots();
}

void QFImagePlot::resetROI()
{
    setROI(QRectF());
}

void QFImagePlot::setROI2(double x, double y, double width, double height)
{
    setROI2(QRectF(x,y,width,height));
}

void QFImagePlot::setROI2(QRectF r)
{
    roi2=r;
    new_plots();
}

void QFImagePlot::resetROI2()
{
    setROI2(QRectF());
}

void QFImagePlot::setROIColor(QColor col)
{
    colROI=col;
    new_plots();
}

void QFImagePlot::setROI2Color(QColor col)
{
    colROI2=col;
    new_plots();
}

void QFImagePlot::setROIFillColor(QColor col)
{
    colFillROI=col;
    new_plots();
}

void QFImagePlot::setROI2FillColor(QColor col)
{
    colFillROI2=col;
    new_plots();
}

void QFImagePlot::setBinning(int bin)
{
    binning=qMax(1,bin);
    new_plots();
}

void QFImagePlot::update_plot()
{
    if (!plteImage || !image_data || image_width<=0 || image_height<=0) return;
    plteImage->set_palette(ui->cmbCOlorbar->currentColorPalette());
    if (ui->chkRangeFull->isChecked()) {
        double imin=0, imax=0;
        if (binning>1) {
            double* imgb=qfBinImageCreate(image_data, image_width, image_height, binning);
            statisticsMinMax(imgb, (image_width/binning)*(image_height/binning), imin, imax);
            qfFree(imgb);
        } else {
            statisticsMinMax(image_data, image_width*image_height, imin, imax);
        }
        ui->edtRangeMin->setValue(imin);
        ui->edtRangeMax->setValue(imax);
        plteImage->set_imageMin(imin);
        plteImage->set_imageMax(imax);
    } else if (ui->chkRangeUser->isChecked()) {
        plteImage->set_imageMin(ui->edtRangeMin->value());
        plteImage->set_imageMax(ui->edtRangeMax->value());
    }
    ui->image->update_plot();
}

void QFImagePlot::clear()
{
    JKQtBasePlotter* plt=ui->image->get_plotter();
    plt->clearGraphs(true);
    JKQTPdatastore* ds=plt->getDatastore();
    ds->clear();
    plteImage=NULL;
    plteMask=NULL;
    ui->histogram->clear();
    roi=QRect();
    roi2=QRect();
    clearData();
}

void QFImagePlot::clearData()
{
    if (image_data) qfFree(image_data);
    if (mask) qfFree(mask);
    roi=QRect();
    mask=NULL;
    image_data=NULL;
    image_width=0;
    image_height=0;
}

void QFImagePlot::new_plots()
{
    ui->image->set_doDrawing(false);
    ui->image->set_emitSignals(false);

    JKQtBasePlotter* plt=ui->image->get_plotter();
    plt->clearGraphs(true);
    JKQTPdatastore* ds=plt->getDatastore();
    ds->clear();
    plteImage=NULL;

    ui->chkRangeFull->setChecked(true);

    if (image_data) {
        plteImage=new JKQTPColumnMathImage(plt);
        plteImage->set_autoImageRange(false);
        if (binning<=1){
            plteImage->set_imageColumn(ds->addCopiedImageAsColumn(image_data, image_width, image_height, tr("image")));
        } else {
            ds->addCopiedImageAsColumn(image_data, image_width, image_height, tr("image"));
            double* tmp=qfBinImageCreate(image_data, image_width, image_height, binning);
            plteImage->set_imageColumn(ds->addCopiedImageAsColumn(tmp, image_width/binning, image_height/binning, tr("image, %1x%1 binned").arg(binning)));
            qfFree(tmp);
        }
        plteImage->set_x(0);
        plteImage->set_y(0);
        plteImage->set_width(image_width/binning);
        plteImage->set_height(image_height/binning);
        plteImage->set_autoImageRange(false);
        plteImage->set_Nx(image_width/binning);
        plteImage->set_Ny(image_height/binning);
        plteImage->set_palette(ui->cmbCOlorbar->currentColorPalette());
        plteImage->set_imageMin(0);
        plteImage->set_imageMax(1);
        plt->addGraph(plteImage);
        plt->set_axisAspectRatio(double(image_width)/double(image_height));
        plt->set_aspectRatio(double(image_width)/double(image_height));
        plt->set_maintainAspectRatio(true);
        plt->setAbsoluteXY(0,image_width/binning,0,image_height/binning);
    }
    if (mask) {
        plteMask=new JKQTPColumnOverlayImageEnhanced(plt);
        if (binning<=1){
            plteImage->set_imageColumn(ds->addCopiedImageAsColumn(mask, image_width, image_height, tr("mask")));
        } else {
            bool* tmp=NULL;
            plteImage->set_imageColumn(ds->addCopiedImageAsColumn(tmp=qfBinMaskImageCreate(mask, image_width, image_height, binning), image_width/binning, image_height/binning, tr("mask, %1x%1 binned").arg(binning)));
            qfFree(tmp);
            ds->addCopiedImageAsColumn(mask, image_width, image_height, tr("mask"));
        }
        plteMask->set_x(0);
        plteMask->set_y(0);
        plteMask->set_width(image_width/binning);
        plteMask->set_height(image_height/binning);
        plteMask->set_Nx(image_width/binning);
        plteMask->set_Ny(image_height/binning);
        plteMask->set_trueColor(Qt::darkGray);
        plteMask->set_falseColor(Qt::transparent);
        plt->addGraph(plteMask);
    }
    double roishift=0.1;
    if (int(roi.width())/binning>0 && int(roi.height())/binning>0) {
        plteROI=new JKQTPgeoRectangle(plt, roi.center().x()/binning, roi.center().y()/binning, roi.width()/binning-roishift, roi.height()/binning-roishift, colROI);
        plt->addGraph(plteROI);
    }
    if (roi2.width()>0 && roi2.height()>0) {
        plteROI2=new JKQTPgeoRectangle(plt, roi2.center().x()/binning, roi2.center().y()/binning, roi2.width()/binning-roishift, roi2.height()/binning-roishift, colROI2);
        plt->addGraph(plteROI2);
    }

    ui->image->set_doDrawing(true);
    ui->image->set_emitSignals(true);
    ui->image->zoomToFit();
    if (binning>1) {
        ui->labInfo->setText(tr("image: %1x%2 pixels,   binned: %3x%4 superpixels").arg(image_width).arg(image_height).arg(image_width/binning).arg(image_height/binning));
    } else {
        ui->labInfo->setText(tr("image: %1x%2 pixels").arg(image_width).arg(image_height));
    }
    update_plot();
}

void QFImagePlot::plotMouseMove(double x, double y)
{
    if (image_data) {
        if (binning>1) {
            double* img=qfBinImageCreate(image_data, image_width, image_height, binning);
            int xx=floor(x);
            int yy=floor(y);
            double v=0;
            if (xx>=0 && xx<image_width/binning && yy>=0 && yy<image_height/binning) {
                v=image_data[yy*(image_width/binning)+xx];
                ui->labMouse->setText(tr("image(%1, %2) = %3").arg(xx).arg(yy).arg(v));
            } else {
                ui->labMouse->setText("");
            }
            qfFree(img);
        } else {

            int xx=floor(x);
            int yy=floor(y);
            double v=0;
            if (xx>=0 && xx<image_width && yy>=0 && yy<image_height) {
                v=image_data[yy*image_width+xx];
                ui->labMouse->setText(tr("image(%1, %2) = %3").arg(xx).arg(yy).arg(v));
            } else {
                ui->labMouse->setText("");
            }
        }
    } else {
        ui->labMouse->setText("");
    }
}



QFImagePlotWizardPage::QFImagePlotWizardPage(const QString &title, QWidget *parent):
    QFWizardPage(title, parent)
{
    plot=new QFImagePlot(this);


    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(plot);
    formLay=new QFormLayout();
    layout->addLayout(formLay);
    setLayout(layout);

}

void QFImagePlotWizardPage::addStretch()
{
    QSpacerItem* spc;
    formLay->addItem(spc=new QSpacerItem(2,2,QSizePolicy::Minimum,QSizePolicy::Expanding));
}

void QFImagePlotWizardPage::addSpacer(int height)
{
    QSpacerItem* spc;
    formLay->addItem(spc=new QSpacerItem(2,height,QSizePolicy::Minimum,QSizePolicy::Fixed));
}

void QFImagePlotWizardPage::addRow(const QString &label, QWidget *widget)
{
    formLay->addRow(label, widget);
}

void QFImagePlotWizardPage::addRow(const QString &label, QLayout *layout)
{
    formLay->addRow(label, layout);
}

void QFImagePlotWizardPage::setImage(double *image, int32_t width, int32_t height)
{
    if (image && width>0 && height>0){
        plot->setImage(image, width, height);
    } else {
        plot->clear();
    }
}


class QFImporterImageSeriesOpener {
    public:
        QFImporterImageSeriesOpener(QFImporterImageSeries* reader) {
            this->reader=reader;
        }

        bool operator()(const QString &filename) {
            return reader->open(filename);
        }
        typedef bool result_type;

        protected:
            QFImporterImageSeries* reader;
};

class QFImporterImageSeriesFrameCounter {
    public:
        QFImporterImageSeriesFrameCounter(QFImporterImageSeries* reader) {
            this->reader=reader;
        }

        uint32_t operator()() {
            return reader->countFrames();
        }
        typedef uint32_t result_type;

        protected:
            QFImporterImageSeries* reader;
};

void QFImagePlotWizardPage::setImage(const QString &filename, const QString &imageReaderID, int frameNum, QFImporter::FileInfo* fileinfo)
{
    QFImporter* imp=QFPluginServices::getInstance()->getImporterManager()->createImporter(imageReaderID);
    QFImporterImageSeries* r=dynamic_cast<QFImporterImageSeries*>(imp);
    if (r) {
        QFImporterImageSeriesOpener opener(r);
        QFuture<bool> opened=QtConcurrent::run(opener, filename);
        while (!opened.isFinished()) {
            QApplication::processEvents();
        }
        if (opened.result()) {
            if (frameNum<0) {
                QFImporterImageSeriesFrameCounter counter(r);
                QFuture<uint32_t> cnt=QtConcurrent::run(counter);
                while (!cnt.isFinished()) {
                    QApplication::processEvents();
                }
                frameNum=cnt.result()/2;
            }
            if (frameNum>0) {
                for (int i=0; i<frameNum; i++) {
                    r->nextFrame();
                }
            }
            double* frame=qfMallocT<double>(r->frameHeight()*r->frameWidth());
            r->readFrameDouble(frame);
            if (fileinfo) *fileinfo=r->getFileInfo();
            r->close();
            plot->setImage(frame, r->frameWidth(), r->frameHeight());
            qfFree(frame);
        } else {
            plot->clear();
        }
        delete r;
    } else {
        plot->clear();
    }
}

void QFImagePlotWizardPage::setImage(const QString &filename, const QString &imageReaderID, int frameNum, double *&image, int &width, int &height, QFImporter::FileInfo *fileinfo, int* frames)
{
    QFImporter* imp=QFPluginServices::getInstance()->getImporterManager()->createImporter(imageReaderID);
    QFImporterImageSeries* r=dynamic_cast<QFImporterImageSeries*>(imp);
    if (r) {
        QFImporterImageSeriesOpener opener(r);
        QFuture<bool> opened=QtConcurrent::run(opener, filename);
        while (!opened.isFinished()) {
            QApplication::processEvents();
        }
        if (opened.result()) {
            int fs=0;
            QFImporterImageSeriesFrameCounter counter(r);
            QFuture<uint32_t> cnt=QtConcurrent::run(counter);
            while (!cnt.isFinished()) {
                QApplication::processEvents();
            }
            if (frameNum<0) {
                fs=cnt.result();
                frameNum=cnt.result()/2;
            } else {
                if (frames) fs=cnt.result();
            }
            if (frames) *frames=fs;
            if (frameNum>0) {
                for (int i=0; i<frameNum; i++) {
                    r->nextFrame();
                }
            }

            double* frame=qfMallocT<double>(r->frameHeight()*r->frameWidth());
            r->readFrameDouble(frame);
            if (fileinfo) *fileinfo=r->getFileInfo();
            image=duplicateArray(frame, r->frameHeight()*r->frameWidth());
            height=r->frameHeight();
            width=r->frameWidth();
            r->close();
            plot->setImage(frame, r->frameWidth(), r->frameHeight());
            qfFree(frame);
        } else {
            plot->clear();
        }
        delete r;
    } else {
        plot->clear();
    }
}

void QFImagePlotWizardPage::setImageAvg(const QString &filename, const QString &imageReaderID, int frameStart, int frameCount, double *&image, int &width, int &height, QFImporter::FileInfo *fileinfo, int *frames)
{
    QFImporter* imp=QFPluginServices::getInstance()->getImporterManager()->createImporter(imageReaderID);
    QFImporterImageSeries* r=dynamic_cast<QFImporterImageSeries*>(imp);
    //qDebug()<<imp<<r;
    if (r) {
        QFImporterImageSeriesOpener opener(r);
        QFuture<bool> opened=QtConcurrent::run(opener, filename);
        while (!opened.isFinished()) {
            QApplication::processEvents();
        }
        if (opened.result()) {
            //qDebug()<<"opened "<<filename;
            int fs=0;
            frameCount=qMax(frameCount, 1);
            QFImporterImageSeriesFrameCounter counter(r);
            QFuture<uint32_t> cnt=QtConcurrent::run(counter);
            while (!cnt.isFinished()) {
                QApplication::processEvents();
            }
            if (frameStart<0) {
                fs=cnt.result();
                frameStart=qMax((uint32_t)0,(cnt.result()-frameCount)/2);
            } else {
                if (frames) fs=cnt.result();
            }
            if (frames) *frames=fs;
            if (frameStart>0) {
                for (int i=0; i<frameStart; i++) {
                    r->nextFrame();
                }
            }

            height=r->frameHeight();
            width=r->frameWidth();
            //qDebug()<<frameStart<<frameCount<<fs<<width<<height;

            double* frame=qfMallocT<double>(width*height);
            r->readFrameDouble(frame);
            if (fileinfo) *fileinfo=r->getFileInfo();
            image=duplicateArray(frame, width*height);
            //qDebug()<<width<<height;
            double fcnt=1;
            for (int i=1; i<frameCount; i++) {
                if (r->nextFrame()){
                    if (r->readFrameDouble(frame)) {
                        fcnt++;
                        //qDebug()<<fcnt;
                        for (int j=0; j<width*height; j++) {
                            image[j]=image[j]+frame[j];
                        }
                    }
                } else {
                    break;
                }
                if (i%5==0) {
                    QApplication::processEvents();
                }
            }
            //qDebug()<<"normalize "<<fcnt;
            for (int j=0; j<width*height; j++) {
                image[j]=image[j]/fcnt;
            }
            r->close();
            plot->setImage(image, width, height);
            qfFree(frame);
            //qfFree(image);
        } else {
            plot->clear();
        }
        delete r;
    } else {
        plot->clear();
    }
}

void QFImagePlotWizardPage::setImageAvg(const QString &filename, const QString &imageReaderID, int frameStart, int frameCount, QFImporter::FileInfo *fileinfo)
{
    double* image=NULL;
    int width;
    int height;
    setImageAvg(filename, imageReaderID, frameStart, frameCount, image, width, height, fileinfo);
    if (image) qfFree(image);

}

void QFImagePlotWizardPage::setImage(const QString &filename, const QString &imageReaderID, QFImporter::FileInfo *fileinfo)
{
    setImage(filename, imageReaderID, 0, fileinfo);
}

void QFImagePlotWizardPage::setImage(const QString &filename, const QString &imageReaderID, double *&image, int &width, int &height, QFImporter::FileInfo *fileinfo)
{
    setImage(filename, imageReaderID, 0, image, width, height, fileinfo);
}

void QFImagePlotWizardPage::clear()
{
    plot->clear();
}

void QFImagePlotWizardPage::setROI(double x, double y, double width, double height)
{
    plot->setROI(x,y,width,height);
}

void QFImagePlotWizardPage::setROI(QRectF r)
{
    plot->setROI(r);
}

void QFImagePlotWizardPage::resetROI()
{
    plot->resetROI();
}

void QFImagePlotWizardPage::setROI2(double x, double y, double width, double height)
{
    plot->setROI2(x,y,width,height);
}

void QFImagePlotWizardPage::setROI2(QRectF r)
{
    plot->setROI2(r);
}

void QFImagePlotWizardPage::resetROI2()
{
    plot->resetROI2();
}

void QFImagePlotWizardPage::setROIColor(QColor col)
{
    plot->setROIColor(col);
}

void QFImagePlotWizardPage::setROI2Color(QColor col)
{
    plot->setROI2Color(col);
}

void QFImagePlotWizardPage::setROIFillColor(QColor col)
{
    plot->setROIFillColor(col);
}

void QFImagePlotWizardPage::setROI2FillColor(QColor col)
{
    plot->setROI2FillColor(col);
}

void QFImagePlotWizardPage::setBinning(int bin)
{
    plot->setBinning(bin);
}

void QFImagePlotWizardPage::initializePage()
{
    QFWizardPage::initializePage();
}

bool QFImagePlotWizardPage::validatePage()
{
     return QFWizardPage::validatePage();
}
