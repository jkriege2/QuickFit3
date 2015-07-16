#include "qfrdrimagingfcswizard.h"
#include "qfpluginservices.h"
#include "programoptions.h"


QFRDRImagingFCSWizard::QFRDRImagingFCSWizard(bool is_project, QWidget *parent):
    QFWizard(QSize(600, 440), parent, QString("imaging_fcs/wizard/"))
{
    QLabel* lab;
    frame_data_io=NULL;

    channels=1;
    frame_count_io=0;
    filesize_io=0;
    frametime_io=1;
    baseline_offset_io=1;
    backgroundF_io="";
    pixel_width_io=1;
    pixel_height_io=1;
    dualViewMode_io=0;
    image_width_io=0;
    image_height_io=0;
    hasPixel_io=false;
    inputconfigfile_io="";
    pluginServices=QFPluginServices::getInstance();
    isProject=is_project;
    imageFilters=QFRDRImagingFCSCorrelationJobThread::getImageFilterList();
    imageFormatNames=QFRDRImagingFCSCorrelationJobThread::getImageFormatNameList();
    imageFormatIDs=QFRDRImagingFCSCorrelationJobThread::getImageFormatIDList();
    /*qDebug()<<imageFilters;
    qDebug()<<imageFormatNames;
    qDebug()<<imageFormatIDs;*/

    setWindowTitle(tr("Imaging FCS/FCCS Wizard"));
    setPage(InitPage, wizIntro=new QFRadioButtonListWizardPage(tr("Introduction"), this));
    wizIntro->addRow(tr("This wizard will help you to correlate an image series in order to perform an imaging FCS or FCCS evaluation<br><br><br><center><img src=\":/imaging_fcs/imfcs_flow.png\"></center>"));
    wizIntro->addItem(tr("imFCS / imFCCS evaluation"), true);
    wizIntro->addItem(tr("imFCS focus volume calibration"), false);
    wizIntro->setEnabled(1, QFPluginServices::getInstance()->getEvaluationItemFactory()->contains("imfcs_fit"));
    connect(wizIntro, SIGNAL(onValidate(QWizardPage*)), this, SLOT(finishedIntro()));




    setPage(FileSelectionPage, wizSelfiles=new QFFormWizardPage(tr("Select image stack files ..."), this));
    connect(wizSelfiles, SIGNAL(onInitialize(QWizardPage*)), this, SLOT(initFileSelection()));
    wizSelfiles->setSubTitle(tr("Select the image stack that you want to correlate."));
    wizSelfiles->setExternalValidate(true);
    wizSelfiles->setExternalIsValid(false);
    wizSelfiles->setValidateFunctor(new QFRDRImagingFCSWizard_ImagestackIsValid(this));
    wizSelfiles->setFreeFunctors(true);


    edtFilename=new QFEnhancedLineEdit(wizSelfiles);
    edtFilename->addButton(btnFilename=new QFStyledButton(QFStyledButton::None, edtFilename, edtFilename));
    btnFilename->setDefaultIcon(QFStyledButton::SelectFile);
    connect(btnFilename, SIGNAL(clicked()), this, SLOT(selectFileClicked()));
    cmbFileformat=new QFEnhancedComboBox(wizSelfiles);
    cmbFileformat->addItemsAndStringData(imageFormatNames, imageFormatIDs);
    cmbFileformat->findAndSelectContainedLCText("tinytiff", -1);
    cmbFileformat->setEnabled(false);
    if (cmbFileformat->currentIndex()<0) cmbFileformat->findAndSelectContainedLCText("tiff", -1);
    wizSelfiles->addRow(tr("image stack:"), edtFilename);
    wizSelfiles->addRow(tr("import filter/filetype:"), cmbFileformat);
    wizSelfiles->addRow(lab=new QLabel(tr("<i>Please select the image stack you want to correlate and the image reader that should be used to read it. "
                                                 "If you use the file-open dialog (<img src=\":/lib/qfstyledbutton/selectfile.png\">) to select a file, the correct "
                                                 "image reader will be selected automatically."
                                                 "<br><br>After selecting an image stack and clicking on <b>Next</b>, the file will be analyzed and you can set all necessary"
                                                 "metadata on the next wizard page. Afterwards you can configure the imaging FCS/FCCS correlation process and finish the wizard."
                                                 "<br><br><u>Note:</u>If you want to read an uncompressed TIFF file, we propose to use the <b>tinyTIFF</b> image reader, "
                                                 "as it can read larger files than the libTIFF reader!</i>"), wizSelfiles));
    lab->setWordWrap(true);
    labFileError=new QLabel(wizSelfiles);
    wizSelfiles->addRow(QString(), labFileError);






    setPage(ImagePage, wizImageProps=new QFImagePlotWizardPage(tr("Set image stack properties ..."), this));
    wizImageProps->setSubTitle(tr("Set/check the properties of the image stack.<br><small><i>The plot shows an average over the first 10 frames.</i></small>"));
    connect(wizImageProps, SIGNAL(onInitialize(QWizardPage*)), this, SLOT(initImagePreview()));

    labImageProps=new QLabel(wizImageProps);
    wizImageProps->addRow(tr("Image Stack Properties:"), labImageProps);

    cmbDualView = new QFEnhancedComboBox(wizImageProps);
    cmbDualView->addItem(QIcon(":/imaging_fcs/dvnone.png"), tr("no DualView"));
    cmbDualView->addItem(QIcon(":/imaging_fcs/dvhor.png"), tr("horizontal DualView"));
    cmbDualView->addItem(QIcon(":/imaging_fcs/dvver.png"), tr("vertical DualView"));
    wizImageProps->addRow(tr("&DualView mode:"), cmbDualView);

    widPixSize=new QFPixelSizeEdit(wizImageProps);
    widPixSize->setRange(0,100000);
    widPixSize->setUnits(" nm");
    widPixSize->setPixelSize(400);
    widPixSize->setDecimals(2);
    wizImageProps->addRow(tr("&Pixel Size:"), widPixSize);
    btnPixSize=new QPushButton(tr("calculate ..."), wizImageProps);
    connect(btnPixSize, SIGNAL(clicked()), this, SLOT(calcPixelSize()));
    widPixSize->addWidget(btnPixSize);
    widPixSize->addLayoutStretchAtEnd();

    spinFrametime=new QDoubleSpinBox(wizImageProps);
    spinFrametime->setRange(0,1000000000);
    spinFrametime->setDecimals(3);
    spinFrametime->setValue(1000);
    spinFrametime->setSuffix(QLatin1String(" \xB5s"));
    wizImageProps->addRow(tr("Frame &Time (=1/frame rate):"), spinFrametime);

    widFrameRange=new QFFrameRangeEdit(wizImageProps);
    widFrameRange->setRange(0,1000);
    widFrameRange->addLayoutStretchAtEnd();
    wizImageProps->addRow(tr("Correlated Frame &Range:"), widFrameRange);








    setPage(BackgroundPage, wizBackground=new QFFormWizardPage(tr("Background Correction Settings ..."), this));
    wizBackground->setValidateFunctor(new QFRDRImagingFCSWizard_BackgroundIsValid(this));
    wizBackground->setNextIDFunctor(new QFRDRImagingFCSWizard_BackgroundNextId(this));
    wizBackground->setFreeFunctors(true);
    cmbBackgroundMode=new QComboBox(wizBackground);
    cmbBackgroundMode->addItem(tr("none"));
    cmbBackgroundMode->addItem(tr("remove offset"));
    cmbBackgroundMode->addItem(tr("remove minimum counts & offset"));
    cmbBackgroundMode->addItem(tr("remove background image & offset"));
    connect(cmbBackgroundMode, SIGNAL(currentIndexChanged(int)), this, SLOT(backgroundModeChanged(int)));
    wizBackground->addRow(tr("background correction mode:"), cmbBackgroundMode);
    edtBackgroundFilename=new QFEnhancedLineEdit(wizBackground);
    edtBackgroundFilename->addButton(btnBackgroundFilename=new QFStyledButton(QFStyledButton::SelectFile, edtBackgroundFilename, edtBackgroundFilename));
    btnBackgroundFilename->setFilter(imageFilters.join(";;"));
    edtBackgroundFilename->setEnabled(false);
    wizBackground->addRow(tr("background image stack:"), edtBackgroundFilename);
    spinBackgroundOffset=new QSpinBox(wizBackground);
    spinBackgroundOffset->setSuffix(tr(" ADU"));
    spinBackgroundOffset->setRange(-100000000,100000000);
    spinBackgroundOffset->setValue(0);
    spinBackgroundOffset->setEnabled(false);
    wizBackground->addRow(tr("background offset:"), spinBackgroundOffset);
    labBackgroundError=new QLabel(wizBackground);
    wizBackground->addRow(QString(), labBackgroundError);



    setPage(CalibrationPage, wizCalibration=new QFImagePlotWizardPage(tr("Setup Calibration ..."), this));
    cmbCalibRegion=new QComboBox(wizCalibration);
    cmbCalibRegion->addItem(tr("all pixels"));
    cmbCalibRegion->addItem(tr("left half (x = 0..w/2)"));
    cmbCalibRegion->addItem(tr("right half (x = w/2..w)"));
    cmbCalibRegion->addItem(tr("top half (y = 0..h/2)"));
    cmbCalibRegion->addItem(tr("bottom half (y = h/2..h)"));
    cmbCalibRegion->addItem(tr("center"));
    cmbCalibRegion->addItem(tr("left center (around x = w/4)"));
    cmbCalibRegion->addItem(tr("right center (around x = 3*w/4)"));
    cmbCalibRegion->addItem(tr("top center (around y = h/4)"));
    cmbCalibRegion->addItem(tr("bottom center (around y = 3*h/4)"));
    cmbCalibRegion->addItem(tr("user-defined crop"));
    wizCalibration->addRow(tr("calibration region:"), cmbCalibRegion);
    connect(cmbCalibRegion, SIGNAL(currentIndexChanged(int)), this, SLOT(calibrationCropValuesChanged(int)));
    calibrationCropValuesChanged();
    spinCalibrationCenterSize=new QSpinBox(wizCalibration);
    wizCalibration->addRow(tr("\"center\" region size:"), spinCalibrationCenterSize);
    connect(spinCalibrationCenterSize, SIGNAL(valueChanged(int)), this, SLOT(calibrationCropValuesChanged()));

    widCropCalibration=new QFCropPixelsEdit(wizCalibration);
    wizCalibration->addRow(tr("user-defined crop:"), widCropCalibration);
    connect(widCropCalibration, SIGNAL(valueChanged(int,int,int,int)), this, SLOT(calibrationCropValuesChanged()));


    setPage(CropAndBinPage, wizCropAndBin=new QFFormWizardPage(tr("Setup Crop & Bin ..."), this));
    wizCropAndBin->setNextID(CropAndBinPage);



    setPage(CorrelationPage, wizCorrelation=new QFFormWizardPage(tr("Setup Correlation ..."), this));
    wizCropAndBin->setNextID(CorrelationPage);




    setPage(LastPage, wizFinalizePage=new QFFormWizardPage(tr("Finalize"), this));
    labFinal=new QLabel(this);
    labFinal->setWordWrap(true);
    wizFinalizePage->addRow(labFinal);
    if (!isProject) {
        labFinal->setText(tr("You completed this wizard. The selected files will now be inserted as imaging FCS raw data records (RDR) into the project.<br><br>If not present yet, you can add evaluation items to the project now and start the evaluation."));
    } else {
        labFinal->setText(tr("You completed this wizard. The selected files will now be inserted as imaging FCS raw data records (RDR) into the project.<br><br><b>Please select the evaluation objects that should be added to the project below.</b>"));
        cmbImFCSFitMode=new QFEnhancedComboBox(this);
        wizFinalizePage->addRow(tr("Fit Mode:"), cmbImFCSFitMode);
        cmbImFCSFitMode->addItem(tr("TIR-FCS: normal diffusion 1-component"));
        cmbImFCSFitMode->addItem(tr("TIR-FCS: normal diffusion 2-component"));
        cmbImFCSFitMode->addItem(tr("TIR-FCS: anomalous diffusion"));
        cmbImFCSFitMode->addItem(tr("TIR-FCS: diffusion + flow"));
        cmbImFCSFitMode->addItem(tr("SPIM-FCS: normal diffusion 1-component"));
        cmbImFCSFitMode->addItem(tr("SPIM-FCS: normal diffusion 2-component"));
        cmbImFCSFitMode->addItem(tr("SPIM-FCS: anomalous diffusion"));
        cmbImFCSFitMode->addItem(tr("SPIM-FCS: diffusion + flow"));
        cmbImFCSFitMode->addItem(tr("confocal FCS: normal diffusion 1-component"));
        cmbImFCSFitMode->addItem(tr("confocal FCS: normal diffusion 2-component"));
        cmbImFCSFitMode->addItem(tr("confocal FCS: anomalous diffusion"));
        cmbImFCSFitMode->addItem(tr("confocal FCS: diffusion + flow"));
        chkLastImFCSFit1=new QCheckBox(tr("single-curve FCS fit (e.g. ACF)"), this);
        chkLastImFCSFit1->setChecked(true);
        wizFinalizePage->addRow(tr("Evaluations"), chkLastImFCSFit1);
        chkLastImFCCSFit=new QCheckBox(tr("global FCCS fit (2-color/2-pixel FCCS"), this);
        chkLastImFCCSFit->setChecked(false);
        wizFinalizePage->addRow(QString(), chkLastImFCCSFit);
    }

    setPage(ProcessCorrelationPage, wizProcessJobs=new QFFormWizardPage(tr("Process Correlation ..."), this));
    wizCalibration->setNextID(ProcessCorrelationPage);
    wizFinalizePage->setNextID(ProcessCorrelationPage);
    wizProcessJobs->setFinalPage(true);

}

QFRDRImagingFCSWizard::~QFRDRImagingFCSWizard()
{
    if (frame_data_io) qfFree(frame_data_io);
    frame_data_io=NULL;
}

void QFRDRImagingFCSWizard::selectFileClicked()
{
    QString lastImagefileDir=QDir::homePath();
    QString lastImagefileFilter="TIFF File [tinyTIFF] (*.tiff *.tif)";

    lastImagefileDir=ProgramOptions::getConfigValue("imaging_fcs/wizard/last_imagefile_dir", lastImagefileDir).toString();
    lastImagefileFilter=ProgramOptions::getConfigValue("imaging_fcs/wizard/last_imagefile_filter", lastImagefileFilter).toString();


    QString fileName = qfGetOpenFileName(this, tr("Select Image Stack File ..."), lastImagefileDir, imageFilters.join(";;"), &lastImagefileFilter);
    if (!fileName.isEmpty()) {
        lastImagefileDir=QFileInfo(fileName).dir().absolutePath();
        cmbFileformat->setCurrentIndex(imageFilters.indexOf(lastImagefileFilter));
        btnFilename->setText(fileName);
        btnFilename->setFocus(Qt::MouseFocusReason);
        edtFilename->setText(fileName);
        edtFilenameTextChanged(fileName);
        ProgramOptions::setConfigValue("imaging_fcs/wizard/last_imagefile_dir", lastImagefileDir);
        ProgramOptions::setConfigValue("imaging_fcs/wizard/last_imagefile_filter", lastImagefileFilter);
    }
}

void QFRDRImagingFCSWizard::edtFilenameTextChanged(const QString &filename)
{
    cmbFileformat->setEnabled(!filename.isEmpty());
    wizSelfiles->setExternalIsValid(!filename.isEmpty());
}

void QFRDRImagingFCSWizard::initImagePreview()
{

}

void QFRDRImagingFCSWizard::initFileSelection()
{
    wizSelfiles->setExternalValidate(true);
    wizSelfiles->setExternalIsValid(!edtFilename->text().isEmpty());

}

void QFRDRImagingFCSWizard::finishedIntro()
{
    isCalibration=wizIntro->isChecked(1);
}

void QFRDRImagingFCSWizard::backgroundModeChanged(int mode)
{
    if (mode==0) {
        edtBackgroundFilename->setEnabled(false);
        spinBackgroundOffset->setEnabled(false);
    } else if (mode==1) {
        edtBackgroundFilename->setEnabled(false);
        spinBackgroundOffset->setEnabled(true);
    } else if (mode==2) {
        edtBackgroundFilename->setEnabled(false);
        spinBackgroundOffset->setEnabled(true);
    } else if (mode==3) {
        edtBackgroundFilename->setEnabled(true);
        spinBackgroundOffset->setEnabled(true);
    }
}

void QFRDRImagingFCSWizard::calcPixelSize()
{
    QFRDRImagingFCSPixelFromObjective* dlg=new QFRDRImagingFCSPixelFromObjective(this);
    if (dlg->exec()) {
        widPixSize->setPixelSize(dlg->getPixelSize()*1000.0);
    }
    delete dlg;
}

void QFRDRImagingFCSWizard::calibrationRegionChanged(int region)
{
    spinCalibrationCenterSize->setEnabled(false);
    widCropCalibration->setEnabled(false);
    switch(region) {
        case 0: //all pixels
            wizCalibration->setROI(0,0,image_width_io, image_height_io);
            wizCalibration->resetROI2();
            break;
        case 1: // "left half"
            wizCalibration->setROI(0,0,image_width_io/2, image_height_io);
            wizCalibration->resetROI2();
            break;
        case 2: // "right half"
            wizCalibration->setROI(image_width_io/2,0,image_width_io/2, image_height_io);
            wizCalibration->resetROI2();
            break;
        case 3: // "top half"
            wizCalibration->setROI(0,image_height_io/2,image_width_io, image_height_io/2);
            wizCalibration->resetROI2();
            break;
        case 4: // "bottom half"
            wizCalibration->setROI(0,0,image_width_io, image_height_io/2);
            wizCalibration->resetROI2();
            break;
        case 5: // "center pixels"
            {
                spinCalibrationCenterSize->setEnabled(true);
                int w=spinCalibrationCenterSize->value();
                int x=qBound(0, image_width_io/2-w/2, image_width_io-1);
                int y=qBound(0, image_height_io/2-w/2, image_height_io-1);
                wizCalibration->setROI(x,y,w,w);
                wizCalibration->resetROI2();
            }
            break;
        case 6: // "left center"
            {
                spinCalibrationCenterSize->setEnabled(true);
                int w=spinCalibrationCenterSize->value();
                int x=qBound(0, image_width_io/4-w/2, image_width_io-1);
                int y=qBound(0, image_height_io/2-w/2, image_height_io-1);
                wizCalibration->setROI(x,y,w,w);
                wizCalibration->resetROI2();
            }
            break;
        case 7: // "right center"
            {
                spinCalibrationCenterSize->setEnabled(true);
                int w=spinCalibrationCenterSize->value();
                int x=qBound(0, 3*image_width_io/4-w/2, image_width_io-1);
                int y=qBound(0, image_height_io/2-w/2, image_height_io-1);
                wizCalibration->setROI(x,y,w,w);
                wizCalibration->resetROI2();
            }
            break;
        case 8: // "top center"
            {
                spinCalibrationCenterSize->setEnabled(true);
                int w=spinCalibrationCenterSize->value();
                int x=qBound(0, image_width_io/2-w/2, image_width_io-1);
                int y=qBound(0, 3*image_height_io/4-w/2, image_height_io-1);
                wizCalibration->setROI(x,y,w,w);
                wizCalibration->resetROI2();
            }
            break;
        case 9: // "bottom center"
            {
                spinCalibrationCenterSize->setEnabled(true);
                int w=spinCalibrationCenterSize->value();
                int x=qBound(0, image_width_io/2-w/2, image_width_io-1);
                int y=qBound(0, image_height_io/4-w/2, image_height_io-1);
                wizCalibration->setROI(x,y,w,w);
                wizCalibration->resetROI2();
            }
            break;
        case 10: // "user-defined crop"
            widCropCalibration->setEnabled(true);
            wizCalibration->setROI(widCropCalibration->getX1(), widCropCalibration->getY1(),widCropCalibration->getWIDTH(), widCropCalibration->getHEIGHT());
            wizCalibration->resetROI2();
            break;
    }
}

void QFRDRImagingFCSWizard::calibrationCropValuesChanged()
{
    calibrationRegionChanged(cmbCalibRegion->currentIndex());
}




bool QFRDRImagingFCSWizard_BackgroundIsValid::isValid(QFWizardPage *page)
{
    if (wizard) {
        wizard->labBackgroundError->setText("");
        QFRDRImagingFCSCorrelationDialog::readBackgroundProperties(wizard->edtBackgroundFilename->text(), wizard->cmbFileformat->currentIndex(), page, &(wizard->background_width), &(wizard->background_height), &(wizard->background_count));
        if (wizard->cmbBackgroundMode->currentIndex()==0) {
            return true;
        } else if (wizard->cmbBackgroundMode->currentIndex()==1) {
            return true;
        } else if (wizard->cmbBackgroundMode->currentIndex()==2) {
            return true;
        } else if (wizard->cmbBackgroundMode->currentIndex()==3) {
            if (wizard->background_count<=0) {
                wizard->edtBackgroundFilename->setFocus();
                wizard->labBackgroundError->setText(QObject::tr("<font color=\"red\"><b><u>ERROR:</u> Background file does not contain frames or could not be read!</b></font>"));
                return false;
            }
            if ((wizard->image_width_io==wizard->background_width) && (wizard->image_height_io==wizard->background_height)) {
                return true;
            } else {
                wizard->edtBackgroundFilename->setFocus();
                wizard->labBackgroundError->setText(QObject::tr("<font color=\"red\"><b><u>ERROR:</u> Background file and image stack do not have the same frame size!</b><br>&nbsp;&nbsp;&nbsp;&nbsp;image stack frame size: <tt>%1x%2</tt>,&nbsp;&nbsp;&nbsp;&nbsp;background frame size: <tt>%3x%4</tt></font>").arg(wizard->image_width_io).arg(wizard->image_height_io).arg(wizard->background_width).arg(wizard->background_height));
                return false;
            }
        }
        return true;
    } else {
        return true;
    }
}



bool QFRDRImagingFCSWizard_ImagestackIsValid::isValid(QFWizardPage */*page*/)
{
    if (wizard) {
        QString readerid=wizard->imageFormatIDs.value(wizard->cmbFileformat->currentIndex(), wizard->imageFormatIDs.value(0, ""));

        wizard->wizImageProps->setImageAvg(wizard->edtFilename->text(), readerid, 0, 20);
        wizard->wizCalibration->setImageAvg(wizard->edtFilename->text(), readerid, 0, 20);
        wizard->calibrationRegionChanged(wizard->cmbCalibRegion->currentIndex());


        if (wizard->frame_data_io) qfFree(wizard->frame_data_io);
        wizard->frame_data_io=NULL;
        QFRDRImagingFCSCorrelationDialog::readStackProperties(wizard->edtFilename->text(), wizard->cmbFileformat->currentIndex(), true, true, wizard, &(wizard->channels), &(wizard->frame_count_io), &(wizard->filesize_io), &(wizard->frametime_io), &(wizard->baseline_offset_io), &(wizard->backgroundF_io), &(wizard->pixel_width_io), &(wizard->pixel_height_io), &(wizard->hasPixel_io), &(wizard->dualViewMode_io), &(wizard->image_width_io), &(wizard->image_height_io), &(wizard->inputconfigfile_io), &(wizard->frame_data_io), &(wizard->background_width), &(wizard->background_height), &(wizard->background_count));


        wizard->labImageProps->setText(QObject::tr("frames: %1,   frame-size: %2x%3").arg(wizard->frame_count_io).arg(wizard->image_width_io).arg(wizard->image_height_io));
        wizard->widPixSize->setPixelSize(wizard->pixel_width_io, wizard->pixel_height_io);
        wizard->widFrameRange->setRange(0, wizard->frame_count_io-1);
        wizard->cmbDualView->setCurrentIndex(wizard->dualViewMode_io);
        wizard->spinFrametime->setValue(wizard->frametime_io);
        wizard->edtBackgroundFilename->setText(wizard->backgroundF_io);
        wizard->cmbBackgroundMode->setCurrentIndex(0);
        if (!wizard->backgroundF_io.isEmpty()) wizard->cmbBackgroundMode->setCurrentIndex(3);
        wizard->spinBackgroundOffset->setValue(0);
        wizard->backgroundModeChanged(wizard->cmbBackgroundMode->currentIndex());
        wizard->spinCalibrationCenterSize->setRange(1,qMax(wizard->image_width_io, wizard->image_height_io));
        wizard->spinCalibrationCenterSize->setValue(qMin(wizard->image_width_io, wizard->image_height_io));
        wizard->widCropCalibration->setImageSize(wizard->image_width_io, wizard->image_height_io);
        wizard->widCropCalibration->setFullImageSize();

        if (wizard->frame_count_io<=0) {
            wizard->labFileError->setText(QObject::tr("<font color=\"red\"><b><u>ERROR:</u> Image stack file does not contain frames or could not be read!</b></font>"));
            wizard->edtFilename->setFocus();
            return false;
        }
        if (wizard->frame_count_io<=10000) {
            if (QMessageBox::warning(wizard, QObject::tr("Small image stack"),
                                     QObject::tr("You selected an image stack with only %1 frames, which is unusually small for an imaging FCS evaluation.\n"
                                                 "We suggest correlating at least 10,000-100,000 frames for good statistics!\n\n"
                                                 "Do you still want to \n"
                                                 "   * use the selected image stack [Yes],\n"
                                                 "   * or select another one [No]?").arg(wizard->frame_count_io), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)
                    ==QMessageBox::Yes) {
                return true;
            } else {
                wizard->labFileError->setText(QObject::tr("<font color=\"dark-orange\"><b><u>WARNING:</u> Image stack file contains only %1 frames, which might be too small for good imFCS statistics!</b></font>").arg(wizard->frame_count_io));
                wizard->edtFilename->setFocus();
                return false;
            }
        }
        if (wizard->image_width_io*wizard->image_height_io<=0) {
            wizard->labFileError->setText(QObject::tr("<font color=\"red\"><b><u>ERROR:</u> Image stack does not contain frames with a size larger than 0x0!</b></font>"));
            wizard->edtFilename->setFocus();
            return false;
        }

        return true;
    } else {
        return true;
    }
}



int QFRDRImagingFCSWizard_BackgroundNextId::nextID(const QFWizardPage *page) const
{
    if (wizard->wizIntro->isChecked(1)) {
        return QFRDRImagingFCSWizard::CalibrationPage;
    } else {
        return QFRDRImagingFCSWizard::CropAndBinPage;
    }

}
