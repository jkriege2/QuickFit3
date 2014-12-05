#include "qfimageplot.h"
#include "ui_qfimageplot.h"
#include "programoptions.h"
#include "qftools.h"
#include "statistics_tools.h"
#include "qfimporterimageseries.h"
#include "qfimportermanager.h"
#include "qfimporter.h"

QFImagePlot::QFImagePlot(QWidget *parent, const QString &prefix):
    QWidget(parent),
    ui(new Ui::QFImagePlot)
{
    plteImage=NULL;
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

void QFImagePlot::setImage(double *image, int32_t width, int32_t height)
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

void QFImagePlot::update_plot()
{
    if (!plteImage || !image_data || image_width<=0 || image_height<=0) return;
    plteImage->set_palette(ui->cmbCOlorbar->currentColorPalette());
    if (ui->chkRangeFull->isChecked()) {
        double imin, imax;
        statisticsMinMax(image_data, image_width*image_height, imin, imax);
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
        plteImage->set_imageColumn(ds->addCopiedImageAsColumn(image_data, image_width, image_height, tr("image")));
        plteImage->set_x(0);
        plteImage->set_y(0);
        plteImage->set_width(image_width);
        plteImage->set_height(image_height);
        plteImage->set_autoImageRange(false);
        plteImage->set_Nx(image_width);
        plteImage->set_Ny(image_height);
        plteImage->set_palette(ui->cmbCOlorbar->currentColorPalette());
        plteImage->set_imageMin(0);
        plteImage->set_imageMax(1);
        plt->addGraph(plteImage);
        plt->set_axisAspectRatio(double(image_width)/double(image_height));
        plt->set_aspectRatio(double(image_width)/double(image_height));
        plt->set_maintainAspectRatio(true);
        plt->setAbsoluteXY(0,image_width,0,image_height);
    }
    if (mask) {
        plteMask=new JKQTPColumnOverlayImageEnhanced(plt);
        plteMask->set_imageColumn(ds->addCopiedImageAsColumn(mask, image_width, image_height, tr("mask")));
        plteMask->set_x(0);
        plteMask->set_y(0);
        plteMask->set_width(image_width);
        plteMask->set_height(image_height);
        plteMask->set_Nx(image_width);
        plteMask->set_Ny(image_height);
        plteMask->set_trueColor(Qt::darkGray);
        plteMask->set_falseColor(Qt::transparent);
        plt->addGraph(plteMask);
    }
    if (roi.width()>0 && roi.height()>0) {
        plteROI=new JKQTPgeoRectangle(plt, roi.center().x()+0.5, roi.center().y()+0.5, roi.width()-1.0, roi.height()-1.0, QColor("red"));
        plt->addGraph(plteROI);
    }
    if (roi2.width()>0 && roi2.height()>0) {
        plteROI2=new JKQTPgeoRectangle(plt, roi2.center().x()+0.5, roi2.center().y()+0.5, roi2.width()-1.0, roi2.height()-1.0, QColor("blue"));
        plt->addGraph(plteROI2);
    }

    ui->image->set_doDrawing(true);
    ui->image->set_emitSignals(true);
    ui->image->zoomToFit();
    ui->labInfo->setText(tr("image: %1x%2 pixels").arg(image_width).arg(image_height));
    update_plot();
}

void QFImagePlot::plotMouseMove(double x, double y)
{
    if (image_data) {
        int xx=floor(x);
        int yy=floor(y);
        double v=0;
        if (xx>=0 && xx<image_width && yy>=0 && yy<image_height) {
            v=image_data[yy*image_width+xx];
            ui->labMouse->setText(tr("image(%1, %2) = %3").arg(xx).arg(yy).arg(v));
        } else {
            ui->labMouse->setText("");
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

void QFImagePlotWizardPage::setImage(const QString &filename, const QString &imageReaderID, int frameNum, QFImporter::FileInfo* fileinfo)
{
    QFImporter* imp=QFPluginServices::getInstance()->getImporterManager()->createImporter(imageReaderID);
    QFImporterImageSeries* r=dynamic_cast<QFImporterImageSeries*>(imp);
    if (r) {
        if (r->open(filename)) {
            if (frameNum<0) {
                frameNum=r->countFrames()/2;
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

void QFImagePlotWizardPage::setImage(const QString &filename, const QString &imageReaderID, int frameNum, double *&image, int &width, int &height, QFImporter::FileInfo *fileinfo)
{
    QFImporter* imp=QFPluginServices::getInstance()->getImporterManager()->createImporter(imageReaderID);
    QFImporterImageSeries* r=dynamic_cast<QFImporterImageSeries*>(imp);
    if (r) {
        if (r->open(filename)) {
            if (frameNum<0) {
                frameNum=r->countFrames()/2;
            }
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

void QFImagePlotWizardPage::initializePage()
{
    QFWizardPage::initializePage();
}

bool QFImagePlotWizardPage::validatePage()
{
     return QFWizardPage::validatePage();
}
