#include "qfrdrimagestack3dviewer.h"
#include "ui_qfrdrimagestack3dviewer.h"
#include "qfpluginservices.h"
#include "programoptions.h"
#include "qftools.h"
#include "qfrdrimagestack_data.h"
#include <QClipboard>

QFRDRImageStack3DViewer::QFRDRImageStack3DViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFRDRImageStack3DViewer)
{
    ui->setupUi(this);
    rdr=NULL;
    stack=0;
}

QFRDRImageStack3DViewer::~QFRDRImageStack3DViewer()
{
    rdr=NULL;
    saveSettings();
    delete ui;
}

void QFRDRImageStack3DViewer::init(QFRDRImageStackData *rdr, int stack)
{
    if (this->rdr) saveSettings();
    loadWidgetGeometry(*(ProgramOptions::getInstance()->getQSettings()), this, "QFRDRImageStack3DViewer/widget/");
    ui->widget->readSettings(*(ProgramOptions::getInstance()->getQSettings()), "QFRDRImageStack3DViewer/viewer/");
    ui->widget->clearVolumeData();
    this->rdr=rdr;
    this->stack=stack;
    if (rdr) {
        ui->widget->set_doUpdate(false);
        for (int channel=0; channel<rdr->getImageStackChannels(stack); channel++) {
            //qDebug()<<"add stack="<<stack<<"    channel="<<channel<<"/"<<rdr->getImageStackChannels(stack);
            int sX=rdr->getImageStackWidth(stack);
            int sY=rdr->getImageStackHeight(stack);
            int sZ=rdr->getImageStackFrames(stack);
            //qDebug()<<"    size="<<sX<<sY<<sZ;
            if (sX*sY*sZ>0) {
                double wx=2.0/double(sX);
                double wy=2.0/double(sX);
                double wz=2.0/double(sX);
                //qDebug()<<"    stack="<<rdr->getImageStack(stack, 0, channel);
                //qDebug()<<"    w="<<wx<<wy<<wz;
                ui->widget->addCopiedVolumeData(rdr->getImageStack(stack, 0, channel), sX, sY, sZ,wx, wy, wz, rdr->getImageStackChannelName(stack, channel));                
            }
        }
        //qDebug()<<"read from "<<rdr->getID()<<": "<<rdr->getProperty(QString("3DVIEWERCONFIG_STACK%1").arg(stack)).toString();
        ui->widget->setConfig(rdr->getProperty(QString("3DVIEWERCONFIG_STACK%1").arg(stack)).toString());
        ui->widget->set_doUpdate(true);
        ui->labInfo->setText(tr("<b>file:</b> %1").arg(rdr->getName()));
        ui->labInfo->setTextFormat(Qt::RichText);
    } else {
        ui->labInfo->setText(tr("<b>no RDR selected ...</b>"));
    }

}

void QFRDRImageStack3DViewer::showHelp()
{
    QFPluginServices::getInstance()->displayPluginHelpWindow("image_stack", "3dviewer.html");
}

void QFRDRImageStack3DViewer::copyImage()
{
    QImage img=ui->widget->getImage();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setImage(img);
}

void QFRDRImageStack3DViewer::printImage(QPrinter* p_in)
{

    QPrinter* printer=p_in;
    bool delP=false;


    // select a printer
    if (printer==NULL) {
        QString currentPrinter=ProgramOptions::getConfigValue("QFRDRImageStack3DViewer/printer", "").toString();;
        printer=new QPrinter();
        printer->setPrinterName(currentPrinter);
        delP=true;

        QPrintDialog *dialog = new QPrintDialog(printer, NULL);
        dialog->setWindowTitle(tr("Print 3D View"));
        if (dialog->exec() != QDialog::Accepted) {
            delete printer;
            delete dialog;
            return;
        }
        currentPrinter=printer->printerName();
        ProgramOptions::setConfigValue("QFRDRImageStack3DViewer/printer", currentPrinter);
        delete dialog;
    }

    bool ok=false;
    QStringList items;
    items<<tr("fill page");
    items<<tr("2/3 page");
    items<<tr("1/2 page");
    items<<tr("1/3 page");
    items<<tr("1/4 page");
    items<<tr("1/8 page");
    QString it=QInputDialog::getItem(this, tr("Print 3D View"), tr("Select outout size"), items, 0, false, &ok);
    double factor=1;
    int itidx=items.indexOf(it);
    if (itidx==1) factor=2.0/3.0;
    else if (itidx==2) factor=0.5;
    else if (itidx==3) factor=1.0/3.0;
    else if (itidx==4) factor=0.25;
    else if (itidx==5) factor=0.125;
    if (ok && itidx>=0) {
        QImage img=ui->widget->getImage();
        QPainter painter(printer);
        QRect rect = painter.viewport();
        QSize size = img.size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(img.rect());
        painter.drawImage(0,0,img);
    }
    if (delP && printer) delete printer;
}

void QFRDRImageStack3DViewer::saveImage(const QString& filename)
{
    QString fn=filename;
    QStringList filt;
    filt<<tr("PNG Image (*.png)");
    filt<<tr("BMP Image (*.bmp)");
    filt<<tr("TIFF Image (*.tif *.tiff)");
    filt<<tr("JPEG Image (*.jpg *.jpeg)");
    filt<<tr("X11 Bitmap (*.xbm)");
    filt<<tr("X11 Pixmap (*.xpm)");
    QString selFormat="";
    if (fn.isEmpty()) {
        QString currentSaveDirectory=ProgramOptions::getConfigValue("QFRDRImageStack3DViewer/lastSaveDirectory", "").toString();
        selFormat=ProgramOptions::getConfigValue("QFRDRImageStack3DViewer/lastSaveFormat", "").toString();
        fn = QFileDialog::getSaveFileName(NULL, tr("Save 3D image"),
                                    currentSaveDirectory,
                                          filt.join(";;"), &selFormat);
        if (!fn.isEmpty()) {
            currentSaveDirectory=QFileInfo(fn).absolutePath();
            ProgramOptions::setConfigValue("QFRDRImageStack3DViewer/lastSaveFormat", selFormat);
            ProgramOptions::setConfigValue("QFRDRImageStack3DViewer/lastSaveDirectory", currentSaveDirectory);
        }
    }

    if (!fn.isEmpty()) {
        QImage img=ui->widget->getImage();
        int filtID=filt.indexOf(selFormat);
        if (filtID==0 || (filtID<0 && QFileInfo(fn).suffix().toLower()=="png")) {
            img.save(fn, "PNG");
        } else if (filtID==1 || (filtID<0 && QFileInfo(fn).suffix().toLower()=="bmp")) {
            img.save(fn, "BMP");
        } else if (filtID==2 || (filtID<0 && QFileInfo(fn).suffix().toLower()=="tif")) {
            img.save(fn, "TIFF");
        } else if (filtID==3 || (filtID<0 && QFileInfo(fn).suffix().toLower()=="jpg")) {
            img.save(fn, "JPEG");
        } else if (filtID==4 || (filtID<0 && QFileInfo(fn).suffix().toLower()=="xbm")) {
            img.save(fn, "XBM");
        } else if (filtID==5 || (filtID<0 && QFileInfo(fn).suffix().toLower()=="xpm")) {
            img.save(fn, "XPM");
        } else {
            img.save(fn);
        }

    }
}

void QFRDRImageStack3DViewer::saveSettings()
{
    ui->widget->writeSettings(*(ProgramOptions::getInstance()->getQSettings()), "QFRDRImageStack3DViewer/viewer/");
    saveWidgetGeometry(*(ProgramOptions::getInstance()->getQSettings()), this, "QFRDRImageStack3DViewer/widget/");
    if (rdr) {
        rdr->setQFProperty(QString("3DVIEWERCONFIG_STACK%1").arg(stack), ui->widget->getConfig(), false, false);
        //qDebug()<<"write to "<<rdr->getID()<<": "<<ui->widget->getConfig();
    }
}

void QFRDRImageStack3DViewer::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
    deleteLater();
}
