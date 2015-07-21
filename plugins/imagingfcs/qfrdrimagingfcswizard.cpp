#include "qfrdrimagingfcswizard.h"
#include "qfpluginservices.h"
#include "programoptions.h"


QFRDRImagingFCSWizard::QFRDRImagingFCSWizard(bool is_project, QWidget *parent):
    QFWizard(QSize(600, 440), parent, QString("imaging_fcs/wizard/"))
{
    QLabel* lab;
    frame_data_io=NULL;

    setOption(QWizard::NoCancelButtonOnLastPage);
    setOption(QWizard::NoBackButtonOnLastPage);
    setOption(QWizard::NoBackButtonOnStartPage);

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
    wizIntro->addRow(tr("This wizard will help you to correlate an image series in order to perform an imaging FCS or FCCS evaluation<br>"
                        "<u>Note:</u> It offers a simplified user-interface for processing imaging FCS correlations and calibrations. If you should need more options, please use the full correlator UI under the menu entry <tt>Data Items | Insert Raw Data | imFCS | correlate &amp; insert</tt>.<br><br>"
                        "<center><img src=\":/imaging_fcs/imfcs_flow.png\"></center>"));
    wizIntro->addItem(tr("imFCS / imFCCS evaluation"), true);
    wizIntro->addItem(tr("imFCS focus volume calibration"), false);
    wizIntro->setEnabled(1, QFPluginServices::getInstance()->getEvaluationItemFactory()->contains("imfcs_fit"));
    wizIntro->setChecked(ProgramOptions::getConfigValue("imaging_fcs/wizard/microscopy", 0).toInt());
    connect(wizIntro, SIGNAL(onValidate(QWizardPage*)), this, SLOT(finishedIntro()));




    setPage(FileSelectionPage, wizSelfiles=new QFFormWizardPage(tr("Select image stack files ..."), this));
    connect(wizSelfiles, SIGNAL(onInitialize(QWizardPage*)), this, SLOT(initFileSelection()));
    wizSelfiles->setSubTitle(tr("Select the image stack that you want to correlate."));
    wizSelfiles->setUseExternalIsComplete(true);
    wizSelfiles->setExternalIsComplete(false);
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
    labFileError->setWordWrap(true);
    wizSelfiles->addRow(labFileError);






    setPage(ImagePage, wizImageProps=new QFImagePlotWizardPage(tr("Set image stack properties ..."), this));
    wizImageProps->setSubTitle(tr("Set/check the properties of the image stack.<br><small><i>The plot shows an average over the first 10 frames.</i></small>"));
    connect(wizImageProps, SIGNAL(onInitialize(QWizardPage*)), this, SLOT(initImagePreview()));
    connect(wizImageProps, SIGNAL(onValidate(QWizardPage*)), this, SLOT(finishedImageProps()));

    labImageProps=new QLabel(wizImageProps);
    labImageProps->setWordWrap(true);
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


    setPage(MicroscopyPage, wizMicroscopy=new QFFormWizardPage(tr("Microscopy Type ..."), this));
    wizMicroscopy->addRow(tr("Please select the type of microscopy that was used for the data acquisition and specify the focal propertiesa. The latter will be used as presets for any further evaluation, but can be changed at any point during the evaluation. They are also used during imaging FCS calibration, in which case they have to be accurate!"));
    wizMicroscopy->addStretch();
    wizMicroscopy->setSubTitle(tr("Select the type of microscopy used during the acquisition."));
    cmbMicroscopy=new QComboBox(wizMicroscopy);
    cmbMicroscopy->addItem(tr("lightsheet microscopy (SPIM/LSFM/...), camera-based"));
    cmbMicroscopy->addItem(tr("TIRF microscopy, camera-based"));
    cmbMicroscopy->addItem(tr("other microscopy, camera-based"));
    cmbMicroscopy->addItem(tr("other microscopy, non-camera-based"));
    cmbMicroscopy->setCurrentIndex(ProgramOptions::getConfigValue("imaging_fcs/wizard/microscopy", 0).toInt());
    wizMicroscopy->addRow(tr("&Microscopy Technique:"), cmbMicroscopy);
    wizMicroscopy->addStretch();
    spinWz=new QDoubleSpinBox(wizMicroscopy);
    spinWz->setDecimals(2);
    spinWz->setRange(0.1, 10000);
    spinWz->setValue(ProgramOptions::getConfigValue("imaging_fcs/wizard/calib_wz", 1200).toDouble());
    spinWz->setSuffix(" nm");
    wizMicroscopy->addRow(tr("PSF z-extent <i>w</i><sub>z</sub>:"), spinWz);
    wizMicroscopy->addRow(QString(), labWz=new QLabel(tr("give as 1/e<sup>2</sup>-halfwidth<br><i><u>Note:</u> This is required for calibrating SPIM-microscopes and for data-fitting and can be determined e.g. by a bead-scan.</i>"), wizMicroscopy));
    labWz->setWordWrap(true);

    spinWxy=new QDoubleSpinBox(wizMicroscopy);
    spinWxy->setDecimals(2);
    spinWxy->setRange(0.1, 10000);
    spinWxy->setValue(ProgramOptions::getConfigValue("imaging_fcs/wizard/calib_wxy", 600).toDouble());
    spinWxy->setSuffix(" nm");
    wizMicroscopy->addRow(tr("PSF xy-extent <i>w</i><sub>xy</sub>:"), spinWxy);
    wizMicroscopy->addRow(QString(), labWxy=new QLabel(tr("give as 1/e<sup>2</sup>-halfwidth<br><i><u>Note:</u> This is required for data-fitting and can be determined by an imaging FCS calibration.</i>"), wizMicroscopy));
    labWxy->setWordWrap(true);


    connect(wizMicroscopy, SIGNAL(onValidate(QWizardPage*)), this, SLOT(microscopyChoosen()));
    connect(cmbMicroscopy, SIGNAL(currentIndexChanged(int)), this, SLOT(microscopyChoosen()));





    setPage(BackgroundPage, wizBackgroundAndBleach=new QFFormWizardPage(tr("Background and Bleach Correction Settings ..."), this));
    wizBackgroundAndBleach->setValidateFunctor(new QFRDRImagingFCSWizard_BackgroundIsValid(this));
    wizBackgroundAndBleach->setNextIDFunctor(new QFRDRImagingFCSWizard_BackgroundNextId(this));
    wizBackgroundAndBleach->setFreeFunctors(true);
    cmbBackgroundMode=new QComboBox(wizBackgroundAndBleach);
    cmbBackgroundMode->addItem(tr("none"));
    cmbBackgroundMode->addItem(tr("remove offset"));
    cmbBackgroundMode->addItem(tr("remove minimum counts & offset"));
    cmbBackgroundMode->addItem(tr("remove background image & offset"));
    connect(cmbBackgroundMode, SIGNAL(currentIndexChanged(int)), this, SLOT(backgroundModeChanged(int)));
    wizBackgroundAndBleach->addRow(tr("Background &Correction Mode:"), cmbBackgroundMode);
    edtBackgroundFilename=new QFEnhancedLineEdit(wizBackgroundAndBleach);
    edtBackgroundFilename->addButton(btnBackgroundFilename=new QFStyledButton(QFStyledButton::SelectFile, edtBackgroundFilename, edtBackgroundFilename));
    btnBackgroundFilename->setFilter(imageFilters.join(";;"));
    edtBackgroundFilename->setEnabled(false);
    wizBackgroundAndBleach->addRow(tr("Background Image Stack &File:"), edtBackgroundFilename);
    spinBackgroundOffset=new QSpinBox(wizBackgroundAndBleach);
    spinBackgroundOffset->setSuffix(tr(" ADU"));
    spinBackgroundOffset->setRange(-100000000,100000000);
    spinBackgroundOffset->setValue(0);
    spinBackgroundOffset->setEnabled(false);
    wizBackgroundAndBleach->addRow(tr("Background &Offset:"), spinBackgroundOffset);
    labBackgroundError=new QLabel(wizBackgroundAndBleach);
    labBackgroundError->setWordWrap(true);
    wizBackgroundAndBleach->addRow(labBackgroundError);

    wizBackgroundAndBleach->addStretch();
    cmbBleachCorrection=new QComboBox(wizBackgroundAndBleach);
    cmbBleachCorrection->addItem(tr("none"), (int)BLEACH_NONE);
    cmbBleachCorrection->addItem(tr("1-exponential LM-fit"), (int)BLEACH_EXP);
    cmbBleachCorrection->addItem(tr("1-exponential(poly2) LM-fit"), (int)BLEACH_EXP_POLY2);
    cmbBleachCorrection->addItem(tr("1-exponential(poly3) LM-fit"), (int)BLEACH_EXP_POLY3);
    cmbBleachCorrection->addItem(tr("1-exponential(poly4) LM-fit"), (int)BLEACH_EXP_POLY4);
    cmbBleachCorrection->addItem(tr("1-exponential(poly5) LM-fit"), (int)BLEACH_EXP_POLY5);
    cmbBleachCorrection->setCurrentIndex(ProgramOptions::getConfigValue("imaging_fcs/wizard/bleach_correction", 0).toInt());
    wizBackgroundAndBleach->addRow(tr("&Bleach Correction:"), cmbBleachCorrection);
    wizBackgroundAndBleach->addRow(QString(), tr("We would propose to choose the bleach correction as follows:"
                                                 "<ul>"
                                                 "<li><b>none</b>: non-bleaching samples (e.g. fluorescent microspheres, QDots and other)</li>"
                                                 "<li><b>1-exponential LM-fit</b> or <b>1-exponential(poly2) LM-fit</b>: weakly bleaching samples (e.g. chemical dyes)</li>"
                                                 "<li><b>1-exponential(poly3-poly5) or higher: strongly bleaching samples (e.g. fluorescent proteins in cells)</li>"
                                                 "</ul>"));
    wizBackgroundAndBleach->addStretch();



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
    cmbCalibRegion->setCurrentIndex(0);
    wizCalibration->addRow(tr("calibration region:"), cmbCalibRegion);
    cmbCalibRegion->setCurrentIndex(ProgramOptions::getConfigValue("imaging_fcs/wizard/calib_region", 0).toInt());
    spinCalibrationCenterSize=new QSpinBox(wizCalibration);
    wizCalibration->addRow(tr("\"center\" region size:"), spinCalibrationCenterSize);

    widCropCalibration=new QFCropPixelsEdit(wizCalibration);
    widCropCalibration->addLayoutStretchAtEnd();
    wizCalibration->addRow(tr("user-defined crop:"), widCropCalibration);
    wizCalibration->addStretch();

    spinCalibExectedWxy=new QDoubleSpinBox(wizCalibration);
    spinCalibExectedWxy->setDecimals(2);
    spinCalibExectedWxy->setRange(0.1, 10000);
    spinCalibExectedWxy->setValue(ProgramOptions::getConfigValue("imaging_fcs/wizard/calib_wxy", 600).toDouble());
    spinCalibExectedWxy->setSuffix(" nm");
    wizCalibration->addRow(tr("expected PSF x/y-extent <i>w</i><sub>xy</sub>:"), spinCalibExectedWxy);

    spinCalibExpectedWxyTests=new QSpinBox(wizCalibration);
    spinCalibExpectedWxyTests->setRange(3,50);
    spinCalibExpectedWxyTests->setValue(ProgramOptions::getConfigValue("imaging_fcs/wizard/calib_wxy_tests", 5).toInt());
    spinCalibExpectedWxySteps=new QDoubleSpinBox(wizCalibration);
    spinCalibExpectedWxySteps->setDecimals(2);
    spinCalibExpectedWxySteps->setRange(0.1, 10000);
    spinCalibExpectedWxySteps->setValue(ProgramOptions::getConfigValue("imaging_fcs/wizard/calib_wxy_steps", 100).toDouble());
    spinCalibExpectedWxySteps->setSuffix(" nm");
    labCalibExpectedWxyTests=new QLabel(wizCalibration);
    labCalibExpectedWxyTests->setWordWrap(true);

    wizCalibration->addRow(tr("<i>w</i><sub>xy</sub>-tests around expectation:"), qfBuildQHBoxLayoutWithFinalStretch(spinCalibExpectedWxyTests, new QLabel(tr("   in steps of "), wizCalibration), spinCalibExpectedWxySteps));
    wizCalibration->addRow(QString(), labCalibExpectedWxyTests);

    spinCalibBinMax=new QSpinBox(wizCalibration);
    spinCalibBinMax->setRange(1,100);
    spinCalibBinMax->setValue(ProgramOptions::getConfigValue("imaging_fcs/wizard/calib_maxbin", 5).toInt());
    spinCalibBinMax->setSuffix(tr(" pixels"));
    wizCalibration->addRow(tr("max. binning:"), spinCalibBinMax);
    labCalibBinMax=new QLabel(wizCalibration);
    labCalibBinMax->setWordWrap(true);
    wizCalibration->addRow(QString(), labCalibBinMax);
    wizCalibration->addStretch();

    connect(spinCalibrationCenterSize, SIGNAL(valueChanged(int)), this, SLOT(calibrationCropValuesChanged()));
    connect(spinCalibBinMax, SIGNAL(valueChanged(int)), this, SLOT(calibWxyTestChanged()));
    connect(widCropCalibration, SIGNAL(valueChanged(int,int,int,int)), this, SLOT(calibrationCropValuesChanged()));
    connect(cmbCalibRegion, SIGNAL(currentIndexChanged(int)), this, SLOT(calibrationRegionChanged(int)));
    connect(wizCalibration, SIGNAL(onValidate(QWizardPage*)), this, SLOT(calibrationSetupFinished()));
    connect(wizCalibration, SIGNAL(onInitialize(QWizardPage*)), this, SLOT(calibWxyTestChanged()));
    connect(wizCalibration, SIGNAL(onInitialize(QWizardPage*)), this, SLOT(calibrationCropValuesChanged()));
    connect(spinCalibExectedWxy, SIGNAL(valueChanged(double)), this, SLOT(calibWxyTestChanged()));
    connect(spinCalibExpectedWxyTests, SIGNAL(valueChanged(int)), this, SLOT(calibWxyTestChanged()));
    connect(spinCalibExpectedWxySteps, SIGNAL(valueChanged(double)), this, SLOT(calibWxyTestChanged()));
    calibrationCropValuesChanged();


    setPage(CropAndBinPage, wizCropAndBin=new QFImagePlotWizardPage(tr("Setup Crop & Bin ..."), this));
    wizCropAndBin->setNextID(CropAndBinPage);
    cmbCropRegion=new QComboBox(wizCropAndBin);
    cmbCropRegion->addItem(tr("all pixels"));
    cmbCropRegion->addItem(tr("left half (x = 0..w/2)"));
    cmbCropRegion->addItem(tr("right half (x = w/2..w)"));
    cmbCropRegion->addItem(tr("top half (y = 0..h/2)"));
    cmbCropRegion->addItem(tr("bottom half (y = h/2..h)"));
    cmbCropRegion->addItem(tr("center"));
    cmbCropRegion->addItem(tr("left center (around x = w/4)"));
    cmbCropRegion->addItem(tr("right center (around x = 3*w/4)"));
    cmbCropRegion->addItem(tr("top center (around y = h/4)"));
    cmbCropRegion->addItem(tr("bottom center (around y = 3*h/4)"));
    cmbCropRegion->addItem(tr("user-defined crop"));
    cmbCropRegion->setCurrentIndex(0);
    wizCropAndBin->addRow(tr("calibration region:"), cmbCropRegion);
    cmbCropRegion->setCurrentIndex(ProgramOptions::getConfigValue("imaging_fcs/wizard/crop_region", 0).toInt());
    spinCropCenterSize=new QSpinBox(wizCropAndBin);
    wizCropAndBin->addRow(tr("\"center\" region size:"), spinCropCenterSize);

    widCrop=new QFCropPixelsEdit(wizCropAndBin);
    widCrop->addLayoutStretchAtEnd();
    wizCropAndBin->addRow(tr("user-defined crop:"), widCrop);
    wizCropAndBin->addStretch();
    spinBinning=new QSpinBox(wizCropAndBin);
    spinBinning->setRange(1,100);
    spinBinning->setValue(1);
    spinBinning->setSuffix(tr(" pixels"));
    labBinning=new QLabel(wizCropAndBin);
    labBinning->setWordWrap(true);
    wizCropAndBin->addRow(tr("pixel-binning:"), spinBinning);
    wizCropAndBin->addRow(QString(), labBinning);
    wizCropAndBin->addStretch();
    connect(spinCropCenterSize, SIGNAL(valueChanged(int)), this, SLOT(cropValuesChanged()));
    connect(widCrop, SIGNAL(valueChanged(int,int,int,int)), this, SLOT(cropValuesChanged()));
    connect(spinBinning, SIGNAL(valueChanged(int)), this, SLOT(cropValuesChanged()));
    connect(cmbCropRegion, SIGNAL(currentIndexChanged(int)), this, SLOT(cropRegionChanged(int)));
    connect(wizCropAndBin, SIGNAL(onValidate(QWizardPage*)), this, SLOT(cropSetupFinished()));
    cropValuesChanged();


    setPage(CorrelationPage, wizCorrelation=new QFFormWizardPage(tr("Setup Correlation ..."), this));
    wizCropAndBin->setNextID(CorrelationPage);
    chkACF=new QCheckBox(wizCorrelation);
    chkACF->setChecked(true);
    wizCorrelation->addRow(tr("calculate autocorrelations (ACFs):"), chkACF);
    chk2ColorFCCS=new QCheckBox(wizCorrelation);
    chk2ColorFCCS->setChecked(false);
    wizCorrelation->addRow(tr("calculate 2-color FCCS:"), chk2ColorFCCS);
    cmb2PixelFCCS=new QComboBox(wizCorrelation);
    cmb2PixelFCCS->addItem(tr("none"));
    cmb2PixelFCCS->addItem(tr("4 direct neighbors"));
    cmb2PixelFCCS->addItem(tr("8 direct neighbors"));
    cmb2PixelFCCS->addItem(tr("right neighbor (dx=1)"));
    cmb2PixelFCCS->addItem(tr("left neighbor (dx=-1)"));
    cmb2PixelFCCS->addItem(tr("top neighbor (dy=1)"));
    cmb2PixelFCCS->addItem(tr("bottom neighbor (dy=-1)"));
    cmb2PixelFCCS->addItem(tr("2 right neighbors (dx=1..2)"));
    cmb2PixelFCCS->addItem(tr("2 left neighbors (dx=-1..-2)"));
    cmb2PixelFCCS->addItem(tr("2 top neighbors (dy=1..2)"));
    cmb2PixelFCCS->addItem(tr("2 bottom neighbors (dy=-1..-2)"));
    cmb2PixelFCCS->addItem(tr("5 right neighbors (dx=1..5)"));
    cmb2PixelFCCS->addItem(tr("5 left neighbors (dx=-1..-5)"));
    cmb2PixelFCCS->addItem(tr("5 top neighbors (dy=1..5)"));
    cmb2PixelFCCS->addItem(tr("5 bottom neighbors (dy=-1..-5)"));
    cmb2PixelFCCS->addItem(tr("10 right neighbors (dx=1..10)"));
    cmb2PixelFCCS->addItem(tr("10 left neighbors (dx=-1..-10)"));
    cmb2PixelFCCS->addItem(tr("10 top neighbors (dy=1..10)"));
    cmb2PixelFCCS->addItem(tr("10 bottom neighbors (dy=-1..-10)"));
    cmb2PixelFCCS->setCurrentIndex(0);
    wizCorrelation->addRow(tr("calculate 2-pixel FCCS:"), cmb2PixelFCCS);
    spinTauMax=new QDoubleSpinBox(wizCorrelation);
    spinTauMax->setDecimals(3);
    spinTauMax->setRange(0.001,1000000);
    spinTauMax->setValue(10);
    spinTauMax->setSuffix(tr(" s"));
    wizCorrelation->addRow(tr("max. lag-time in correlations:"), spinTauMax);
    spinSegments=new QSpinBox(wizCorrelation);
    spinSegments->setRange(1,100);
    spinSegments->setValue(5);
    spinSegments->setSpecialValueText(tr("1 (use blocking for error estimate)"));
    wizCorrelation->addRow(tr("number of correlated segments:"), spinSegments);
    labSegments=new QLabel(wizCorrelation);
    labSegments->setWordWrap(true);


    setPage(ProcessCorrelationPage, wizProcessJobs=new QFGridWizardPage(tr("Process Correlation ..."), this));
    wizCalibration->setNextID(ProcessCorrelationPage);
    wizCorrelation->setNextID(ProcessCorrelationPage);
    wizProcessJobs->setNextIDFunctor(new QFRDRImagingFCSWizard_ProcessNextId(this));
    widProcess=new QFRDRImagingFCSWizardCorrelationProgress(wizProcessJobs);
    wizProcessJobs->addWidget(widProcess, 0, 0);
    wizProcessJobs->setFinalPage(false);
    wizProcessJobs->setNoPreviousButton(true);
    wizProcessJobs->setNoCancelButton(true);
    wizProcessJobs->setUseExternalIsComplete(true);
    wizProcessJobs->setExternalIsComplete(false);
    connect(widProcess, SIGNAL(correlationCompleted(bool)), wizProcessJobs, SLOT(setExternalIsComplete(bool)));



    setPage(LastPage, wizFinalizePage=new QFFormWizardPage(tr("Finalize"), this));
    labFinal=new QLabel(this);
    labFinal->setWordWrap(true);
    wizFinalizePage->addRow(labFinal);
    wizFinalizePage->setFinalPage(true);
    wizFinalizePage->setNoPreviousButton(true);
    wizFinalizePage->setNoCancelButton(true);
    if (!isProject) {
        labFinal->setText(tr("You completed this wizard. The selected files will now be inserted as imaging FCS raw data records (RDR) into the project.<br><br>If not present yet, you can add evaluation items to the project now and start the evaluation."));
    } else {
        labFinal->setText(tr("You completed this wizard. The selected files will now be inserted as imaging FCS raw data records (RDR) into the project.<br><br><b>Please select the evaluation objects that should be added to the project below.</b>"));
        cmbImFCSFitMode=new QFEnhancedComboBox(this);
        wizFinalizePage->addRow(tr("Fit Mode:"), cmbImFCSFitMode);
        cmbImFCSFitMode->addItem(tr("TIR-FCS/FCCS: normal diffusion 1-component"));
        cmbImFCSFitMode->addItem(tr("TIR-FCS/FCCS: normal diffusion 2-component"));
        cmbImFCSFitMode->addItem(tr("TIR-FCS/FCCS: anomalous diffusion"));
        cmbImFCSFitMode->addItem(tr("TIR-FCS/FCCS: diffusion + flow"));
        cmbImFCSFitMode->addItem(tr("SPIM-FCS/FCCS: normal diffusion 1-component"));
        cmbImFCSFitMode->addItem(tr("SPIM-FCS/FCCS: normal diffusion 2-component"));
        cmbImFCSFitMode->addItem(tr("SPIM-FCS/FCCS: anomalous diffusion"));
        cmbImFCSFitMode->addItem(tr("SPIM-FCS/FCCS: diffusion + flow"));
        cmbImFCSFitMode->addItem(tr("confocal FCS/FCCS: normal diffusion 1-component"));
        cmbImFCSFitMode->addItem(tr("confocal FCS/FCCS: normal diffusion 2-component"));
        cmbImFCSFitMode->addItem(tr("confocal FCS/FCCS: anomalous diffusion"));
        cmbImFCSFitMode->addItem(tr("confocal FCS/FCCS: diffusion + flow"));
        chkLastImFCSFit1=new QCheckBox(tr("single-curve FCS fit (e.g. ACF)"), this);
        chkLastImFCSFit1->setChecked(true);
        wizFinalizePage->addRow(tr("Evaluations"), chkLastImFCSFit1);
        chkLastImFCCSFit=new QCheckBox(tr("global FCCS fit (2-color/2-pixel FCCS"), this);
        chkLastImFCCSFit->setChecked(false);
        wizFinalizePage->addRow(QString(), chkLastImFCCSFit);
    }

    setPage(LastPageCalibration, wizFinalizePageCalibration=new QFFormWizardPage(tr("Finalize Calibration"), this));
    wizFinalizePageCalibration->setNoPreviousButton(true);
    wizFinalizePageCalibration->setNoCancelButton(true);
    wizFinalizePageCalibration->addRow(tr("You completed the imFCS correlation wizard for an imFCS calibration.<br><br>"
                                          "The wizard will now insert the correlation result into the current project and start the imFCS calibration wizard. In this wizard, you will have to follow the proposed steps, which will finally give you an estimate of the lateral PSF-size."));
    wizFinalizePageCalibration->setFinalPage(true);

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
    wizSelfiles->setExternalIsComplete(!filename.isEmpty());
}

void QFRDRImagingFCSWizard::initImagePreview()
{
    ProgramOptions::setConfigValue("imaging_fcs/wizard/microscopy", cmbMicroscopy->currentIndex());
    ProgramOptions::setConfigValue("imaging_fcs/wizard/microscopy", wizIntro->getChecked());
}

void QFRDRImagingFCSWizard::finishedImageProps()
{
    chk2ColorFCCS->setEnabled(cmbDualView->currentIndex()>0);
    if (cmbDualView->currentIndex()>0) {
        chk2ColorFCCS->setChecked(true);
        chk2ColorFCCS->setText("");
    } else {
        chk2ColorFCCS->setText("unavailable: you did not select a DualView mode on the previous pages!");
    }
    spinTauMax->setValue(double(widFrameRange->getLast()-widFrameRange->getFirst()+1)*spinFrametime->value()/1.0e6/3.0);
}

void QFRDRImagingFCSWizard::initFileSelection()
{
    wizSelfiles->setUseExternalIsComplete(true);
    wizSelfiles->setExternalIsComplete(!edtFilename->text().isEmpty());

}

void QFRDRImagingFCSWizard::finishedIntro()
{
    isCalibration=wizIntro->isChecked(1);
    spinWxy->setEnabled(!isCalibration);
    labWxy->setEnabled(!isCalibration);
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
    QColor col=QColor("red");
    wizCalibration->setROIColor(col);
    col.setAlphaF(0.2);
    wizCalibration->setROIFillColor(col);
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
                int x=qBound(0, (3*image_width_io-2*w)/4, image_width_io-1);
                int y=qBound(0, (image_height_io-w)/2, image_height_io-1);
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

void QFRDRImagingFCSWizard::cropRegionChanged(int region)
{
    spinCropCenterSize->setEnabled(false);
    widCrop->setEnabled(false);
    QColor col=QColor("red");
    wizCropAndBin->setROIColor(col);
    col.setAlphaF(0.2);
    wizCropAndBin->setROIFillColor(col);
    switch(region) {
        case 0: //all pixels
            wizCropAndBin->setROI(0,0,image_width_io, image_height_io);
            wizCropAndBin->resetROI2();
            break;
        case 1: // "left half"
            wizCropAndBin->setROI(0,0,image_width_io/2, image_height_io);
            wizCropAndBin->resetROI2();
            break;
        case 2: // "right half"
            wizCropAndBin->setROI(image_width_io/2,0,image_width_io/2, image_height_io);
            wizCropAndBin->resetROI2();
            break;
        case 3: // "top half"
            wizCropAndBin->setROI(0,image_height_io/2,image_width_io, image_height_io/2);
            wizCropAndBin->resetROI2();
            break;
        case 4: // "bottom half"
            wizCropAndBin->setROI(0,0,image_width_io, image_height_io/2);
            wizCropAndBin->resetROI2();
            break;
        case 5: // "center pixels"
            {
                spinCropCenterSize->setEnabled(true);
                int w=spinCropCenterSize->value();
                int x=qBound(0, image_width_io/2-w/2, image_width_io-1);
                int y=qBound(0, image_height_io/2-w/2, image_height_io-1);
                wizCropAndBin->setROI(x,y,w,w);
                wizCropAndBin->resetROI2();
            }
            break;
        case 6: // "left center"
            {
                spinCropCenterSize->setEnabled(true);
                int w=spinCropCenterSize->value();
                int x=qBound(0, image_width_io/4-w/2, image_width_io-1);
                int y=qBound(0, image_height_io/2-w/2, image_height_io-1);
                wizCropAndBin->setROI(x,y,w,w);
                wizCropAndBin->resetROI2();
            }
            break;
        case 7: // "right center"
            {
                spinCropCenterSize->setEnabled(true);
                int w=spinCropCenterSize->value();
                int x=qBound(0, (3*image_width_io-2*w)/4, image_width_io-1);
                int y=qBound(0, (image_height_io-w)/2, image_height_io-1);
                wizCropAndBin->setROI(x,y,w,w);
                wizCropAndBin->resetROI2();
            }
            break;
        case 8: // "top center"
            {
                spinCropCenterSize->setEnabled(true);
                int w=spinCropCenterSize->value();
                int x=qBound(0, image_width_io/2-w/2, image_width_io-1);
                int y=qBound(0, 3*image_height_io/4-w/2, image_height_io-1);
                wizCropAndBin->setROI(x,y,w,w);
                wizCropAndBin->resetROI2();
            }
            break;
        case 9: // "bottom center"
            {
                spinCropCenterSize->setEnabled(true);
                int w=spinCropCenterSize->value();
                int x=qBound(0, image_width_io/2-w/2, image_width_io-1);
                int y=qBound(0, image_height_io/4-w/2, image_height_io-1);
                wizCropAndBin->setROI(x,y,w,w);
                wizCropAndBin->resetROI2();
            }
            break;
        case 10: // "user-defined crop"
            widCrop->setEnabled(true);
            wizCropAndBin->setROI(widCrop->getX1(), widCrop->getY1(),widCrop->getWIDTH(), widCrop->getHEIGHT());
            wizCropAndBin->resetROI2();
            break;
    }

    labBinning->setText(tr("=> virtual pixel size: %1x%2 nm<sup>2</sup>").arg(widPixSize->getPixelWidth()*double(spinBinning->value())).arg(widPixSize->getPixelHeight()*double(spinBinning->value())));
}

void QFRDRImagingFCSWizard::cropValuesChanged()
{
    cropRegionChanged(cmbCropRegion->currentIndex());
}

void QFRDRImagingFCSWizard::microscopyChoosen()
{
    ProgramOptions::setConfigValue("imaging_fcs/wizard/calib_wz", spinWz->value());
    ProgramOptions::setConfigValue("imaging_fcs/wizard/calib_wxy", spinWxy->value());

    spinWz->setEnabled(cmbMicroscopy->currentIndex()!=1);
    labWz->setVisible(cmbMicroscopy->currentIndex()!=1);
    spinWxy->setEnabled(wizIntro->isChecked(0) && !isCalibration);
    labWxy->setEnabled(wizIntro->isChecked(0) && !isCalibration);

}

void QFRDRImagingFCSWizard::calibrationSetupFinished()
{
    ProgramOptions::setConfigValue("imaging_fcs/wizard/calib_region", cmbCalibRegion->currentIndex());
}

void QFRDRImagingFCSWizard::cropSetupFinished()
{
    ProgramOptions::setConfigValue("imaging_fcs/wizard/crop_region", cmbCropRegion->currentIndex());
}

void QFRDRImagingFCSWizard::correlationValuesChanged()
{
    labSegments->setText(tr("=> segment length %1 s").arg(double(widFrameRange->getLast()-widFrameRange->getFirst()+1)*spinFrametime->value()/1.0e6/double(spinSegments->value())));
}

void QFRDRImagingFCSWizard::calibWxyTestChanged()
{
    ProgramOptions::setConfigValue("imaging_fcs/wizard/calib_wxy", spinCalibExectedWxy->value());
    ProgramOptions::setConfigValue("imaging_fcs/wizard/calib_wxy_tests", spinCalibExpectedWxyTests->value());
    ProgramOptions::setConfigValue("imaging_fcs/wizard/calib_wxy_steps", spinCalibExpectedWxySteps->value());
    ProgramOptions::setConfigValue("imaging_fcs/wizard/calib_maxbin", spinCalibBinMax->value());

    QStringList wxy;
    calibWxyTest.clear();
    for (int i=-spinCalibExpectedWxyTests->value()/2; i<(spinCalibExpectedWxyTests->value()-spinCalibExpectedWxyTests->value()/2); i++) {
        calibWxyTest.append(spinCalibExectedWxy->value()+double(i)*spinCalibExpectedWxySteps->value());
        wxy.append(QString("%1").arg(calibWxyTest.last()));
    }

    labCalibExpectedWxyTests->setText(tr("<i>testing w<sub>xy</sub> = %1 nm</i>").arg(wxy.join(", ")));
    labCalibBinMax->setText(tr("=> virtual pixel size range: %3x%4 - %1x%2 nm<sup>2</sup>").arg(widPixSize->getPixelWidth()*double(spinCalibBinMax->value())).arg(widPixSize->getPixelHeight()*double(spinCalibBinMax->value())).arg(widPixSize->getPixelWidth()).arg(widPixSize->getPixelHeight()));
}




bool QFRDRImagingFCSWizard_BackgroundIsValid::isValid(QFWizardPage *page)
{
    if (wizard) {
        ProgramOptions::setConfigValue("imaging_fcs/wizard/bleach_correction", wizard->cmbBleachCorrection->currentIndex());
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
        wizard->wizCropAndBin->setImageAvg(wizard->edtFilename->text(), readerid, 0, 20);
        wizard->calibrationRegionChanged(wizard->cmbCalibRegion->currentIndex());
        wizard->cropRegionChanged(wizard->cmbCropRegion->currentIndex());


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
        wizard->spinCropCenterSize->setRange(1,qMax(wizard->image_width_io, wizard->image_height_io));
        wizard->spinCropCenterSize->setValue(qMin(wizard->image_width_io, wizard->image_height_io));
        wizard->widCropCalibration->setImageSize(wizard->image_width_io, wizard->image_height_io);
        wizard->widCropCalibration->setFullImageSize();
        wizard->widCrop->setImageSize(wizard->image_width_io, wizard->image_height_io);
        wizard->widCrop->setFullImageSize();

        if (wizard->cmbDualView->currentIndex()==1) {
            wizard->cmbCalibRegion->setCurrentIndex(1);
        }
        if (wizard->cmbDualView->currentIndex()==2) {
            wizard->cmbCalibRegion->setCurrentIndex(3);
        }

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
                wizard->labFileError->setText(QObject::tr("<font color=\"orange\"><b><u>WARNING:</u> Image stack file contains only %1 frames, which might be too small for good imFCS statistics!</b></font>").arg(wizard->frame_count_io));
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



int QFRDRImagingFCSWizard_BackgroundNextId::nextID(const QFWizardPage */*page*/) const
{
    if (wizard->wizIntro->isChecked(1)) {
        return QFRDRImagingFCSWizard::CalibrationPage;
    } else {
        return QFRDRImagingFCSWizard::CropAndBinPage;
    }

}


int QFRDRImagingFCSWizard_ProcessNextId::nextID(const QFWizardPage */*page*/) const
{
    if (wizard->wizIntro->isChecked(1)) {
        return QFRDRImagingFCSWizard::LastPageCalibration;
    } else {
        return QFRDRImagingFCSWizard::LastPage;
    }

}
