#include "qfrdrimagingfcswizard.h"
#include "qfpluginservices.h"
#include "programoptions.h"
#include "qfrdrimagingfcs.h"
#include "qmodernprogresswidget.h"

QFRDRImagingFCSWizard::QFRDRImagingFCSWizard(bool is_project, QWidget *parent):
    QFWizard(QSize(600, 440), parent, QString("imaging_fcs/wizard/"))
{
    QLabel* lab;
    frame_data_io=NULL;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    setOption(QWizard::NoCancelButtonOnLastPage);
#endif

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
    //qDebug()<<imageFormatNames;
    //qDebug()<<imageFormatIDs;*/

    setWindowTitle(tr("Imaging FCS/FCCS Wizard"));
    setPage(InitPage, wizIntro=new QFRadioButtonListWizardPage(tr("Introduction"), this));
    wizIntro->addRow(tr("This wizard will help you to correlate an image series in order to perform an imaging FCS or FCCS evaluation<br>"
                        "<u>Note:</u> It offers a simplified user-interface for processing imaging FCS correlations and calibrations. If you should need more options, please use the full correlator UI under the menu entry <tt>Data Items | Insert Raw Data | imFCS | correlate &amp; insert</tt>.<br><br>"
                        "<center><img src=\":/imaging_fcs/imfcs_flow.png\"></center>"));
    wizIntro->addItem(tr("imFCS / imFCCS evaluation"), true);
    wizIntro->addItem(tr("imFCS focus volume calibration"), false);
    wizIntro->setEnabled(1, QFPluginServices::getInstance()->getEvaluationItemFactory()->contains("imfcs_fit"));
    wizIntro->setChecked(ProgramOptions::getConfigValue("imaging_fcs/wizard/wizardmethod", 0).toInt());
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
                                                 "<br><br><u>Note:</u> If you want to read an uncompressed TIFF file, we propose to use the <b>tinyTIFF</b> image reader, "
                                                 "as it can read larger files than the libTIFF reader!</i>"), wizSelfiles));
    lab->setWordWrap(true);
    labFileError=new QLabel(wizSelfiles);
    labFileError->setWordWrap(true);
    wizSelfiles->addRow(labFileError);

    chkMoreFiles=new QCheckBox(tr("correlate additional files (\"batch mode\")"), this);
    chkMoreFiles->setChecked(false);

    wizSelfiles->addRow(chkMoreFiles);
    lstMoreFiles=new QListWidget(this);
    QLabel* labAF=NULL;
    wizSelfiles->addRow(labAF=new QLabel(tr("additional files:")), lstMoreFiles);
    labAF->setEnabled(false);
    lstMoreFiles->setEnabled(false);
    connect(chkMoreFiles, SIGNAL(toggled(bool)), lstMoreFiles, SLOT(setEnabled(bool)));
    connect(chkMoreFiles, SIGNAL(toggled(bool)), labAF, SLOT(setEnabled(bool)));
    btnAddMoreFiles=new QPushButton(tr("add file ..."), this);
    btnRemoveMoreFiles=new QPushButton(tr("remove selected"), this);
    btnAddMoreFiles->setEnabled(false);
    btnRemoveMoreFiles->setEnabled(false);
    QLabel* labMore;
    wizSelfiles->addRow(QString(), qfBuildQHBoxLayoutWithFinalStretch(btnAddMoreFiles, btnRemoveMoreFiles));
    wizSelfiles->addRow(QString(), labMore=new QLabel(tr("<u>Note</u>: additional files will be processed with EXACTLY the same settings, as the main file (selected above)! Therefore make sure that they have the same properties (width, height, frametime/-rate, ...). Also the same file will be used for background correction (if given at all). The number of frames does not necessarily have to be the same, but if more frames are available, some properties of the result (e.g. length/statistics of the segments) may differ."), this));
    labMore->setWordWrap(true);
    labMore->setEnabled(false);
    connect(chkMoreFiles, SIGNAL(toggled(bool)), btnAddMoreFiles, SLOT(setEnabled(bool)));
    connect(chkMoreFiles, SIGNAL(toggled(bool)), btnRemoveMoreFiles, SLOT(setEnabled(bool)));
    connect(chkMoreFiles, SIGNAL(toggled(bool)), labMore, SLOT(setEnabled(bool)));
    connect(btnAddMoreFiles, SIGNAL(clicked()), this, SLOT(addFile()));
    connect(btnRemoveMoreFiles, SIGNAL(clicked()), this, SLOT(removeFiles()));



    setPage(ImagePage, wizImageProps=new QFImagePlotWizardPage(tr("Set image stack properties ..."), this));
    wizImageProps->setSubTitle(tr("Set/check the properties of the image stack.<br><small><i>The plot shows an average over the first 10 frames.</i></small>"));
    connect(wizImageProps, SIGNAL(onInitialize(QWizardPage*)), this, SLOT(initImagePreview()));
    connect(wizImageProps, SIGNAL(onValidate(QWizardPage*)), this, SLOT(finishedImageProps()));

    labImageProps=new QLabel(wizImageProps);
    labImageProps->setWordWrap(true);
    wizImageProps->addRow(tr("Image Stack Properties:"), labImageProps);

    cmbDualView = new QFEnhancedComboBox(wizImageProps);
    cmbDualView->addItem(QIcon(":/imaging_fcs/dvnone.png"), tr("no DualView"), (int)DUALVIEW_NONE);
    cmbDualView->addItem(QIcon(":/imaging_fcs/dvhor.png"), tr("horizontal DualView"), (int)DUALVIEW_HORICONTAL);
    cmbDualView->addItem(QIcon(":/imaging_fcs/dvver.png"), tr("vertical DualView"), (int)DUALVIEW_VERTICAL);
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
    cmbMicroscopy=new QFEnhancedComboBox(wizMicroscopy);
    cmbMicroscopy->addItem(tr("camera with rectangular pixels / lightsheet microscopy (SPIM/LSFM)"), 0);
    cmbMicroscopy->addItem(tr("camera with rectangular pixels / TIRF microscopy"), 1);
    //cmbMicroscopy->addItem(tr("camera with rectangular pixels / other microscopy"), 2);
    cmbMicroscopy->addItem(tr("camera with non-rectangular pixels / lightsheet microscopy (SPIM/LSFM/other)"),3);
    //cmbMicroscopy->addItem(tr("camera with non-rectangular pixels / TIRF microscopyl"),4);
    //cmbMicroscopy->addItem(tr("camera with non-rectangular pixels / other microscopy"),5);
    cmbMicroscopy->setCurrentFromModelData(ProgramOptions::getConfigValue("imaging_fcs/wizard/microscopy", 0).toInt());
    wizMicroscopy->addRow(tr("&Microscopy Technique:"), cmbMicroscopy);
    labMicroscopy=new QLabel(this);
    labMicroscopy->setWordWrap(true);
    wizMicroscopy->addRow(QString(), labMicroscopy);
    wizMicroscopy->addStretch();
    spinWz=new QDoubleSpinBox(wizMicroscopy);
    spinWz->setDecimals(2);
    spinWz->setRange(0.1, 10000);
    spinWz->setValue(ProgramOptions::getConfigValue("imaging_fcs/wizard/calib_wz", 1200).toDouble());
    spinWz->setSuffix(" nm");
    wizMicroscopy->addRow(tr("PSF z-extent <i>w</i><sub>z</sub>:"), spinWz);
    wizMicroscopy->addRow(QString(), labWz=new QLabel(tr("give as 1/e<sup>2</sup>-halfwidth<br><i><u>Note:</u> This is required for calibrating SPIM-microscopes and for data-fitting and can be measured e.g. by a bead-scan.</i>"), wizMicroscopy));
    labWz->setWordWrap(true);

    spinWxy=new QDoubleSpinBox(wizMicroscopy);
    spinWxy->setDecimals(2);
    spinWxy->setRange(0.1, 10000);
    spinWxy->setValue(ProgramOptions::getConfigValue("imaging_fcs/wizard/calib_wxy", 600).toDouble());
    spinWxy->setSuffix(" nm");
    wizMicroscopy->addRow(tr("PSF xy-extent <i>w</i><sub>xy</sub>:"), spinWxy);
    wizMicroscopy->addRow(QString(), labWxy=new QLabel(tr("give as 1/e<sup>2</sup>-halfwidth<br><i><u>Note:</u> This is required for data-fitting and can be determined by an imaging FCS calibration.</i>"), wizMicroscopy));
    labWxy->setWordWrap(true);


    connect(wizMicroscopy, SIGNAL(onInitialize(QWizardPage*)), this, SLOT(microscopyChoosen()));
    connect(wizMicroscopy, SIGNAL(onValidate(QWizardPage*)), this, SLOT(microscopyChoosen()));
    connect(cmbMicroscopy, SIGNAL(currentIndexChanged(int)), this, SLOT(microscopyChoosen()));





    setPage(BackgroundPage, wizBackgroundAndBleach=new QFFormWizardPage(tr("Background and Bleach Correction Settings ..."), this));
    wizBackgroundAndBleach->setValidateFunctor(new QFRDRImagingFCSWizard_BackgroundIsValid(this));
    wizBackgroundAndBleach->setNextIDFunctor(new QFRDRImagingFCSWizard_BackgroundNextId(this));
    wizBackgroundAndBleach->setFreeFunctors(true);
    cmbBackgroundMode=new QFEnhancedComboBox(wizBackgroundAndBleach);
    cmbBackgroundMode->addItem(tr("none"), (int)BACKGROUND_NONE);
    cmbBackgroundMode->addItem(tr("remove offset"), (int)BACKGROUND_REMOVEOFFSET);
    cmbBackgroundMode->addItem(tr("remove minimum counts & offset"), (int)BACKGROUND_REMOVEMINANDOFFSET);
    cmbBackgroundMode->addItem(tr("remove background image & offset"), (int)BACKGROUND_FILEANDOFFSET);
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
    cmbBleachCorrection=new QFEnhancedComboBox(wizBackgroundAndBleach);
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
    cmbCalibCropRegion=new QComboBox(wizCalibration);
    cmbCalibCropRegion->addItem(tr("all pixels"));
    cmbCalibCropRegion->addItem(tr("left half (x = 0..w/2)"));
    cmbCalibCropRegion->addItem(tr("right half (x = w/2..w)"));
    cmbCalibCropRegion->addItem(tr("top half (y = 0..h/2)"));
    cmbCalibCropRegion->addItem(tr("bottom half (y = h/2..h)"));
    cmbCalibCropRegion->addItem(tr("center"));
    cmbCalibCropRegion->addItem(tr("left center (around x = w/4)"));
    cmbCalibCropRegion->addItem(tr("right center (around x = 3*w/4)"));
    cmbCalibCropRegion->addItem(tr("top center (around y = h/4)"));
    cmbCalibCropRegion->addItem(tr("bottom center (around y = 3*h/4)"));
    cmbCalibCropRegion->addItem(tr("user-defined crop"));
    cmbCalibCropRegion->setCurrentIndex(0);
    wizCalibration->addRow(tr("calibration region:"), cmbCalibCropRegion);
    cmbCalibCropRegion->setCurrentIndex(ProgramOptions::getConfigValue("imaging_fcs/wizard/calib_region", 0).toInt());
    spinCalibrationCenterSize=new QSpinBox(wizCalibration);
    wizCalibration->addRow(tr("\"center\" region size:"), spinCalibrationCenterSize);

    widCropCalibration=new QFCropPixelsEdit(wizCalibration);
    widCropCalibration->addLayoutStretchAtEnd();
    wizCalibration->addRow(tr("user-defined crop:"), widCropCalibration);
    wizCalibration->addStretch();


    spinCalibTauMax=new QDoubleSpinBox(wizCalibration);
    spinCalibTauMax->setDecimals(3);
    spinCalibTauMax->setRange(0.001,1000000);
    spinCalibTauMax->setValue(10);
    spinCalibTauMax->setSuffix(tr(" s"));
    wizCalibration->addRow(tr("max. lag-time in correlations:"), spinCalibTauMax);
    spinCalibSegments=new QSpinBox(wizCalibration);
    spinCalibSegments->setRange(1,100);
    spinCalibSegments->setValue(5);
    spinCalibSegments->setSpecialValueText(tr("1 (use blocking for error estimate)"));
    wizCalibration->addRow(tr("number of correlated segments:"), spinCalibSegments);
    labCalibSegments=new QLabel(wizCalibration);
    labCalibSegments->setWordWrap(true);
    wizCalibration->addRow(QString(), labCalibSegments);


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
    connect(cmbCalibCropRegion, SIGNAL(currentIndexChanged(int)), this, SLOT(calibrationRegionChanged(int)));
    connect(wizCalibration, SIGNAL(onValidate(QWizardPage*)), this, SLOT(calibrationSetupFinished()));
    connect(wizCalibration, SIGNAL(onInitialize(QWizardPage*)), this, SLOT(calibWxyTestChanged()));
    connect(wizCalibration, SIGNAL(onInitialize(QWizardPage*)), this, SLOT(calibrationCropValuesChanged()));
    connect(spinCalibExectedWxy, SIGNAL(valueChanged(double)), this, SLOT(calibWxyTestChanged()));
    connect(spinCalibExpectedWxyTests, SIGNAL(valueChanged(int)), this, SLOT(calibWxyTestChanged()));
    connect(spinCalibExpectedWxySteps, SIGNAL(valueChanged(double)), this, SLOT(calibWxyTestChanged()));
    connect(spinCalibSegments, SIGNAL(valueChanged(int)), this, SLOT(calibrationValuesChanged()));
    connect(spinCalibTauMax, SIGNAL(valueChanged(double)), this, SLOT(calibrationValuesChanged()));
    calibrationCropValuesChanged();
    calibWxyTestChanged();
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
    wizCropAndBin->addRow(tr("crop region:"), cmbCropRegion);
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
    spinBinning->setValue(ProgramOptions::getConfigValue("imaging_fcs/wizard/binning", 1).toInt());
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
    connect(wizCorrelation, SIGNAL(onValidate(QWizardPage*)), this, SLOT(validateCorrelation()));
    wizCropAndBin->setNextID(CorrelationPage);
    chkACF=new QCheckBox(wizCorrelation);
    chkACF->setChecked(true);
    wizCorrelation->addRow(tr("calculate autocorrelations (ACFs):"), chkACF);
    chk2ColorFCCS=new QCheckBox(wizCorrelation);
    chk2ColorFCCS->setChecked(false);
    wizCorrelation->addRow(tr("calculate 2-color FCCS:"), chk2ColorFCCS);
    cmb2PixelFCCS=new QFEnhancedComboBox(wizCorrelation);
    cmb2PixelFCCS->addItem(tr("none"),0);
    cmb2PixelFCCS->addItem(tr("4 direct neighbors"), 4); // user data is number of CCFs
    cmb2PixelFCCS->addItem(tr("8 direct neighbors"), 8);
    cmb2PixelFCCS->addItem(tr("right neighbor (dx=1)"), 1);
    cmb2PixelFCCS->addItem(tr("left neighbor (dx=-1)"), 1);
    cmb2PixelFCCS->addItem(tr("top neighbor (dy=1)"), 1);
    cmb2PixelFCCS->addItem(tr("bottom neighbor (dy=-1)"), 1);
    cmb2PixelFCCS->addItem(tr("2 right neighbors (dx=1..2)"), 2);
    cmb2PixelFCCS->addItem(tr("2 left neighbors (dx=-1..-2)"),2);
    cmb2PixelFCCS->addItem(tr("2 top neighbors (dy=1..2)"),2);
    cmb2PixelFCCS->addItem(tr("2 bottom neighbors (dy=-1..-2)"), 2);
    cmb2PixelFCCS->addItem(tr("5 right neighbors (dx=1..5)"), 5);
    cmb2PixelFCCS->addItem(tr("5 left neighbors (dx=-1..-5)"),5);
    cmb2PixelFCCS->addItem(tr("5 top neighbors (dy=1..5)"),5);
    cmb2PixelFCCS->addItem(tr("5 bottom neighbors (dy=-1..-5)"),5);
    cmb2PixelFCCS->addItem(tr("10 right neighbors (dx=1..10)"),10);
    cmb2PixelFCCS->addItem(tr("10 left neighbors (dx=-1..-10)"),10);
    cmb2PixelFCCS->addItem(tr("10 top neighbors (dy=1..10)"),10);
    cmb2PixelFCCS->addItem(tr("10 bottom neighbors (dy=-1..-10)"),10);
    cmb2PixelFCCS->setCurrentIndex(0);
    cmb2PixelFCCS->setCurrentIndex(ProgramOptions::getConfigValue("imaging_fcs/wizard/2pixelfccs", 0).toInt());
    chk2ColorFCCS->setChecked(ProgramOptions::getConfigValue("imaging_fcs/wizard/2colorfccs", false).toBool());
    chkACF->setChecked(ProgramOptions::getConfigValue("imaging_fcs/wizard/acf", true).toBool());


    wizCorrelation->addRow(tr("calculate 2-pixel FCCS:"), cmb2PixelFCCS);
    spinTauMax=new QDoubleSpinBox(wizCorrelation);
    spinTauMax->setDecimals(3);
    spinTauMax->setRange(0.001,1000000);
    spinTauMax->setValue(10);
    spinTauMax->setValue(ProgramOptions::getConfigValue("imaging_fcs/wizard/taumax", 10).toInt());
    spinTauMax->setSuffix(tr(" s"));
    wizCorrelation->addRow(tr("max. lag-time in correlations:"), spinTauMax);
    spinSegments=new QSpinBox(wizCorrelation);
    spinSegments->setRange(1,100);
    spinSegments->setValue(5);
    spinSegments->setSpecialValueText(tr("1 (use blocking for error estimate)"));
    spinSegments->setValue(ProgramOptions::getConfigValue("imaging_fcs/wizard/segments", 5).toInt());
    connect(spinSegments, SIGNAL(valueChanged(int)), this, SLOT(correlationValuesChanged()));
    connect(spinTauMax, SIGNAL(valueChanged(double)), this, SLOT(correlationValuesChanged()));
    connect(cmb2PixelFCCS, SIGNAL(currentIndexChanged(int)), this, SLOT(correlationValuesChanged()));
    connect(chk2ColorFCCS, SIGNAL(toggled(bool)), this, SLOT(correlationValuesChanged()));
    connect(chkACF, SIGNAL(toggled(bool)), this, SLOT(correlationValuesChanged()));
    wizCorrelation->addRow(tr("number of correlated segments:"), spinSegments);
    labSegments=new QLabel(wizCorrelation);
    labSegments->setWordWrap(true);
    wizCorrelation->addRow(QString(), labSegments);
    correlationValuesChanged();


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
    wizProcessJobs->setExternalIsComplete(false);
    connect(wizProcessJobs, SIGNAL(onInitialize(QWizardPage*)), this, SLOT(startProcessingJobs()));



    setPage(LastPage, wizFinalizePage=new QFFormWizardPage(tr("Finalize"), this));
    //connect(wizFinalizePage, SIGNAL())
    connect(wizFinalizePage, SIGNAL(onInitialize(QWizardPage*)), this, SLOT(validateCorrelation()));

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
        cmbImFCSFitMode->addItem(tr("normal diffusion 1-component"));
        cmbImFCSFitMode->addItem(tr("normal diffusion 2-component"));
        cmbImFCSFitMode->addItem(tr("anomalous diffusion"));
        cmbImFCSFitMode->addItem(tr("diffusion + flow"));
        cmbIm2cFCCSFitMode=new QFEnhancedComboBox(this);
        cmbIm2cFCCSFitMode->addItem(tr("normal diffusion 1-component"));
        cmbIm2cFCCSFitMode->addItem(tr("normal diffusion 2-component"));
        cmbIm2cFCCSFitMode->addItem(tr("anomalous diffusion"));
        cmbIm2cFCCSFitMode->setEnabled(false);
        cmbIm2fFCCSFitMode=new QFEnhancedComboBox(this);
        cmbIm2fFCCSFitMode->addItem(tr("1-component normal diffusion + flow"));
        cmbIm2fFCCSFitMode->addItem(tr("2-component normal diffusion + flow"));
        cmbIm2fFCCSFitMode->addItem(tr("1-component normal diffusion"));
        cmbIm2fFCCSFitMode->addItem(tr("2-component normal diffusion"));
        cmbIm2fFCCSFitMode->addItem(tr("1-component anomalous diffusion"));
        cmbIm2fFCCSFitMode->addItem(tr("2-component anomalous diffusion"));
        cmbIm2fFCCSFitMode->setEnabled(false);

        chkLastImFCSFit1=new QCheckBox(tr("single-curve FCS fit:"), this);
        chkLastImFCSFit1->setChecked(true);
        //wizFinalizePage->addRow(tr("Evaluations"), chkLastImFCSFit1);
        wizFinalizePage->addRow(chkLastImFCSFit1, cmbImFCSFitMode);
        connect(chkLastImFCSFit1, SIGNAL(toggled(bool)), cmbImFCSFitMode, SLOT(setEnabled(bool)));
        chkLastIm2cFCCSFit=new QCheckBox(tr("global FCCS fit 2-color FCCS:"), this);
        chkLastIm2cFCCSFit->setChecked(false);
        chkLastIm2cFCCSFit->setEnabled(false);
        wizFinalizePage->addRow(chkLastIm2cFCCSFit, cmbIm2cFCCSFitMode);
        connect(chkLastIm2cFCCSFit, SIGNAL(toggled(bool)), cmbIm2cFCCSFitMode, SLOT(setEnabled(bool)));
        chkLastIm2fFCCSFit=new QCheckBox(tr("global FCCS fit 2-pixel FCCS"), this);
        chkLastIm2fFCCSFit->setChecked(false);
        chkLastIm2fFCCSFit->setEnabled(false);
        connect(chkLastIm2fFCCSFit, SIGNAL(toggled(bool)), cmbIm2fFCCSFitMode, SLOT(setEnabled(bool)));
        wizFinalizePage->addRow(chkLastIm2fFCCSFit, cmbIm2fFCCSFitMode);
        wizFinalizePage->addRow(QString(), tr("<u>Note:</u> This wizard will try to pre-configure the fit evaluation items in the project to meet your settings. In seldom cases, this will not be possible, so please check the fit model configuration before performing any fits.<br>Also you may wish to use differently configured models. In that case you can also reconfigure the presets before starting the fits!<br><br>"
                                              "After finishing this wizard, it will load all records into the project and (possibly) add several fit evaluation objects. After this you are set to perform fits and evaluate their results. There are two major dialogs that you can use for this:<ol>"
                                              "<li>The <b>imagingFCS RDR editor</b> can be reached by double-clicking any raw data record (RDR) in the project. On the second tab (\"Parameter Image\") of this dialog, you will find the parameter images (after the fits) and tools to evaluate them statistically and also to e.g. mask the image.</li>"
                                              "<li>the <b>Fit Evaluation Editor</b> can be reached by double-clicking any evaluation object in the project and allows you to perform fits for the correlated data in the project.</li>"
                                              "</ol>"
                                              "<center><b>Please read the online-help and tutorials (Menu <tt>Help</tt> in the main menu) for further  information and detailed step-by-step tutorials on how to proceed with the fitting and further evaluation.</b></center>"));
    }

    setPage(LastPageCalibration, wizFinalizePageCalibration=new QFFormWizardPage(tr("Finalize Calibration"), this));
    wizFinalizePageCalibration->setNoPreviousButton(true);
    wizFinalizePageCalibration->setNoCancelButton(true);
    wizFinalizePageCalibration->addRow(tr("You completed the imFCS correlation wizard for an imFCS calibration.<br><br>"
                                          "The wizard will now insert the correlation result into the current project and start the imFCS calibration wizard. In this wizard, you will have to follow the proposed steps (buttons on left hand side of the next wizard), which will finally give you an estimate of the lateral PSF-size.<br><br>"
                                          "<center><img src=\":/imaging_fcs/imfcscalib.png\"></center>"));
    wizFinalizePageCalibration->setFinalPage(true);

    wizFinalizePage->setNextID(-1);
    wizFinalizePage->setFinalPage(true);

}

QFRDRImagingFCSWizard::~QFRDRImagingFCSWizard()
{
    if (frame_data_io) qfFree(frame_data_io);
    frame_data_io=NULL;
}

void QFRDRImagingFCSWizard::finalizeAndModifyProject(bool projectwizard, QFRDRImagingFCSPlugin *plugin)
{
    if (currentId()==QFRDRImagingFCSWizard::LastPageCalibration) { // calibration final page
        widProcess->collectThreads();
        plugin->addFiles(widProcess->getFilesToAdd());
        QFPluginCommandsInterface* commandIntf=dynamic_cast<QFPluginCommandsInterface*>(QFPluginServices::getInstance()->getEvaluationItemFactory()->getPlugin("imfcs_fit"));
        if (commandIntf) {
            QVariantList wxylist;
            for (int i=0; i<calibWxyTest.size(); i++) {
                wxylist<<calibWxyTest[i];
            }
            QString model="fcs_spim_diffe2_newveff";//fcs_diff_d_wz";
            if (cmbMicroscopy->currentData().isValid()) {
                if (cmbMicroscopy->currentData().toInt()==0) model="fcs_spim_diffe2_newveff";
                if (cmbMicroscopy->currentData().toInt()==1) model="fcs_tir_diffe2";
                if (cmbMicroscopy->currentData().toInt()==2) model="fcs_spim_diffe2_newveff";
                if (cmbMicroscopy->currentData().toInt()==3) model="fcs_diff_d_wz";
                if (cmbMicroscopy->currentData().toInt()==4) model="fcs_tir_diff3de2";
                if (cmbMicroscopy->currentData().toInt()==5) model="fcs_diff_d_wz";
            }
            commandIntf->sendPluginCommand("run_calibration", spinWz->value(), 0, false, model, wxylist);
        } else {
            QMessageBox::critical(NULL, tr("imaging FCS Wizard"), tr("No usable imaging FCS Fit plugin found!"), QMessageBox::Ok, QMessageBox::Ok);
        }
    } else if (currentId()==QFRDRImagingFCSWizard::LastPage) { // fit final page
        widProcess->collectThreads();
        plugin->addFiles(widProcess->getFilesToAdd());
        QFProject* project=QFPluginServices::getInstance()->getCurrentProject();
        if (project && projectwizard) {
            if (chkLastImFCSFit1 && chkLastImFCSFit1->isChecked()) {
                QFEvaluationItem* e=project->addEvaluation("imfcs_fit", tr("Imaging FCS fit"));
                if (e) {
                    e->setQFProperty("FIT_REPEATS", 2, false, false);
                    e->setQFProperty("PRESET_FOCUS_HEIGHT", spinWz->value(), false, false);
                    e->setQFProperty("PRESET_FOCUS_HEIGHT_ERROR", 0, false, false);
                    e->setQFProperty("PRESET_FOCUS_WIDTH", spinWxy->value(), false, false);
                    e->setQFProperty("PRESET_N_COMPONENTS", 1, false, false);
                    e->setQFProperty("PRESET_FOCUS_WIDTH_ERROR", 0, false, false);
                    if (((cmbMicroscopy->currentData().toInt()==0)||(cmbMicroscopy->currentData().toInt()==2)) && cmbImFCSFitMode->currentIndex()==0) { // 1-comp, normal
                        e->setQFProperty("PRESET_FIT_MODEL", "fcs_spim_diffe2_newveff", false, false);
                    } else if (((cmbMicroscopy->currentData().toInt()==0)||(cmbMicroscopy->currentData().toInt()==2)) && cmbImFCSFitMode->currentIndex()==1) { // 2-comp, normal
                        e->setQFProperty("PRESET_FIT_MODEL", "fcs_spim_diffe2_newveff", false, false);
                        e->setQFProperty("PRESET_N_COMPONENTS", 2, false, false);
                    } else if (((cmbMicroscopy->currentData().toInt()==0)||(cmbMicroscopy->currentData().toInt()==2)) && cmbImFCSFitMode->currentIndex()==2) { // anomalous
                        e->setQFProperty("PRESET_FIT_MODEL", "fcs_spim_adiffe2", false, false);
                    } else if (((cmbMicroscopy->currentData().toInt()==0)||(cmbMicroscopy->currentData().toInt()==2)) && cmbImFCSFitMode->currentIndex()==3) { // normal+flow
                        e->setQFProperty("PRESET_FIT_MODEL", "fcs_spim_diffflowce2", false, false);


                    } else if (cmbMicroscopy->currentData().toInt()==1 && cmbImFCSFitMode->currentIndex()==0) { // 1-comp, normal
                        e->setQFProperty("PRESET_FIT_MODEL", "fcs_tir_diffe2", false, false);
                    } else if (cmbMicroscopy->currentData().toInt()==1 && cmbImFCSFitMode->currentIndex()==1) { // 2-comp, normal
                        e->setQFProperty("PRESET_FIT_MODEL", "fcs_tir_diffe2", false, false);
                        e->setQFProperty("PRESET_N_COMPONENTS", 2, false, false);
                    } else if (cmbMicroscopy->currentData().toInt()==1 && cmbImFCSFitMode->currentIndex()==2) { // anomalous
                        e->setQFProperty("PRESET_FIT_MODEL", "fcs_tir_adiffe2", false, false);
                    } else if (cmbMicroscopy->currentData().toInt()==1 && cmbImFCSFitMode->currentIndex()==3) { // normal+flow
                        e->setQFProperty("PRESET_FIT_MODEL", "fcs_tir_diff_flowe2", false, false);


                    } else if (((cmbMicroscopy->currentData().toInt()==3)||(cmbMicroscopy->currentData().toInt()==5)) && cmbImFCSFitMode->currentIndex()==0) { // 1-comp, normal
                        e->setQFProperty("PRESET_FIT_MODEL", "fcs_diff", false, false);
                    } else if (((cmbMicroscopy->currentData().toInt()==3)||(cmbMicroscopy->currentData().toInt()==5)) && cmbImFCSFitMode->currentIndex()==1) { // 2-comp, normal
                        e->setQFProperty("PRESET_FIT_MODEL", "fcs_diff", false, false);
                        e->setQFProperty("PRESET_N_COMPONENTS", 2, false, false);
                    } else if (((cmbMicroscopy->currentData().toInt()==3)||(cmbMicroscopy->currentData().toInt()==5)) && cmbImFCSFitMode->currentIndex()==2) { // anomalous
                        e->setQFProperty("PRESET_FIT_MODEL", "fcs_adiff2d", false, false);
                    } else if (((cmbMicroscopy->currentData().toInt()==3)||(cmbMicroscopy->currentData().toInt()==5)) && cmbImFCSFitMode->currentIndex()==3) { // normal+flow
                        e->setQFProperty("PRESET_FIT_MODEL", "fcs_diffflowv", false, false);


                    } else if (cmbMicroscopy->currentData().toInt()==4 && cmbImFCSFitMode->currentIndex()==0) { // 1-comp, normal
                        e->setQFProperty("PRESET_FIT_MODEL", "fcs_tir_diff3de2", false, false);
                    } else if (cmbMicroscopy->currentData().toInt()==4 && cmbImFCSFitMode->currentIndex()==1) { // 2-comp, normal
                        e->setQFProperty("PRESET_FIT_MODEL", "fcs_tir_diff3de2", false, false);
                        e->setQFProperty("PRESET_N_COMPONENTS", 2, false, false);
                    } else if (cmbMicroscopy->currentData().toInt()==4 && cmbImFCSFitMode->currentIndex()==2) { // anomalous
                        e->setQFProperty("PRESET_FIT_MODEL", "fcs_tir_adiff3de2", false, false);
                    } else if (cmbMicroscopy->currentData().toInt()==4 && cmbImFCSFitMode->currentIndex()==3) { // normal+flow
                        e->setQFProperty("PRESET_FIT_MODEL", "fcs_tir_diff_flowe2", false, false);

                    }
                }
            }
            //qDebug()<<chkLastIm2cFCCSFit << chkLastIm2cFCCSFit->isChecked() << chk2ColorFCCS->isChecked();
            if (chkLastIm2cFCCSFit && chkLastIm2cFCCSFit->isChecked() && chk2ColorFCCS->isChecked()) {
                QFEvaluationItem* e=project->addEvaluation("imfccs_fit", tr("Global 2-color Imaging FCCS fit"));
                //qDebug()<<" adding 2-color FCCS fit "<<e;
                if (e) {
                    e->setQFProperty("FIT_REPEATS", 2, false, false);
                    e->setQFProperty("PRESET_FOCUS_HEIGHT", spinWz->value(), false, false);
                    e->setQFProperty("PRESET_FOCUS_HEIGHT_ERROR", 0, false, false);
                    e->setQFProperty("PRESET_FOCUS_WIDTH", spinWxy->value(), false, false);
                    e->setQFProperty("PRESET_N_COMPONENTS", 1, false, false);
                    e->setQFProperty("PRESET_FOCUS_WIDTH_ERROR", 0, false, false);
                    if (((cmbMicroscopy->currentData().toInt()==0)||(cmbMicroscopy->currentData().toInt()==2)) && cmbIm2cFCCSFitMode->currentIndex()==0) { // 1-comp, normal
                        e->setQFProperty("PRESET_FIT_MODEL", "2-COLOR-SPIM-FCCS/NORMAL_1DPERSPECIES", false, false);
                    } else if (((cmbMicroscopy->currentData().toInt()==0)||(cmbMicroscopy->currentData().toInt()==2)) && cmbIm2cFCCSFitMode->currentIndex()==1) { // 2-comp, normal
                        e->setQFProperty("PRESET_FIT_MODEL", "2-COLOR-SPIM-FCCS/NORMAL_2DPERCHANNEL", false, false);
                    } else if (((cmbMicroscopy->currentData().toInt()==0)||(cmbMicroscopy->currentData().toInt()==2)) && cmbIm2cFCCSFitMode->currentIndex()==2) { // anomalous
                        e->setQFProperty("PRESET_FIT_MODEL", "2-COLOR-SPIM-FCCS/ANOMALOUS_1DPERSPECIES", false, false);


                    } else if (cmbMicroscopy->currentData().toInt()==1 && cmbIm2cFCCSFitMode->currentIndex()==0) { // 1-comp, normal
                        e->setQFProperty("PRESET_FIT_MODEL", "2-COLOR-TIR-FCCS/NORMAL_1DPERSPECIES", false, false);
                    } else if (cmbMicroscopy->currentData().toInt()==1 && cmbIm2cFCCSFitMode->currentIndex()==1) { // 2-comp, normal
                        e->setQFProperty("PRESET_FIT_MODEL", "2-COLOR-TIR-FCCS/NORMAL_2DPERCHANNEL", false, false);
                    } else if (cmbMicroscopy->currentData().toInt()==1 && cmbIm2cFCCSFitMode->currentIndex()==2) { // anomalous
                        e->setQFProperty("PRESET_FIT_MODEL", "2-COLOR-TIR-FCCS/ANOMALOUS_1DPERSPECIES", false, false);


                    } else if (((cmbMicroscopy->currentData().toInt()==3)||(cmbMicroscopy->currentData().toInt()==5)) && cmbIm2cFCCSFitMode->currentIndex()==0) { // 1-comp, normal
                        e->setQFProperty("PRESET_FIT_MODEL", "2-COLOR-CONFOCAL-FCCS/NORMAL_1DPERSPECIES", false, false);
                    } else if (((cmbMicroscopy->currentData().toInt()==3)||(cmbMicroscopy->currentData().toInt()==5)) && cmbIm2cFCCSFitMode->currentIndex()==1) { // 2-comp, normal
                        e->setQFProperty("PRESET_FIT_MODEL", "2-COLOR-CONFOCAL-FCCS/NORMAL_2DPERCHANNEL", false, false);
                    } else if (((cmbMicroscopy->currentData().toInt()==3)||(cmbMicroscopy->currentData().toInt()==5)) && cmbIm2cFCCSFitMode->currentIndex()==2) { // anomalous
                        e->setQFProperty("PRESET_FIT_MODEL", "2-COLOR-CONFOCAL-FCCS/ANOMALOUS_1DPERSPECIES", false, false);


                    } else if (cmbMicroscopy->currentData().toInt()==4 && cmbIm2cFCCSFitMode->currentIndex()==0) { // 1-comp, normal
                        e->setQFProperty("PRESET_FIT_MODEL", "2-COLOR-CONFOCAL-FCCS/NORMAL_1DPERSPECIES", false, false);
                        e->setQFProperty("PRESET_FOCUS_HEIGHT", 1e6, false, false);
                    } else if (cmbMicroscopy->currentData().toInt()==4 && cmbIm2cFCCSFitMode->currentIndex()==1) { // 2-comp, normal
                        e->setQFProperty("PRESET_FIT_MODEL", "2-COLOR-CONFOCAL-FCCS/NORMAL_2DPERCHANNEL", false, false);
                        e->setQFProperty("PRESET_FOCUS_HEIGHT", 1e6, false, false);
                    } else if (cmbMicroscopy->currentData().toInt()==4 && cmbIm2cFCCSFitMode->currentIndex()==2) { // anomalous
                        e->setQFProperty("PRESET_FIT_MODEL", "2-COLOR-CONFOCAL-FCCS/ANOMALOUS_1DPERSPECIES", false, false);
                        e->setQFProperty("PRESET_FOCUS_HEIGHT", 1e6, false, false);
                    }
                }
            }
            if (chkLastIm2fFCCSFit && chkLastIm2fFCCSFit->isChecked() && cmb2PixelFCCS->currentIndex()>0) {
                QFEvaluationItem* e=project->addEvaluation("imfccs_fit", tr("Global 2-pixel Imaging FCCS fit"));

                if (e) {
                    e->setQFProperty("FIT_REPEATS", 2, false, false);
                    e->setQFProperty("PRESET_FOCUS_HEIGHT", spinWz->value(), false, false);
                    e->setQFProperty("PRESET_FOCUS_HEIGHT_ERROR", 0, false, false);
                    e->setQFProperty("PRESET_FOCUS_WIDTH", spinWxy->value(), false, false);
                    e->setQFProperty("PRESET_N_COMPONENTS", 1, false, false);
                    e->setQFProperty("PRESET_FOCUS_WIDTH_ERROR", 0, false, false);
                    QString acfmodel="";
                    QString ccfmodel="";
                    int Nccf=cmb2PixelFCCS->currentData().toInt();
                    QStringList globalparams;
                    if (cmbIm2fFCCSFitMode->currentIndex()==0 || cmbIm2fFCCSFitMode->currentIndex()==1 || cmbIm2fFCCSFitMode->currentIndex()==2 || cmbIm2fFCCSFitMode->currentIndex()==3) {
                        if (((cmbMicroscopy->currentData().toInt()==0)||(cmbMicroscopy->currentData().toInt()==2)) ) { // anomalous
                            acfmodel=ccfmodel="fccs_spim_diffflowce2";
                            globalparams<<"n_particle"<<"n_components"<<"diff_coeff1"<<"diff_coeff2"<<"diff_coeff3"<<"diff_rho2"<<"diff_rho3"<<"vflowx"<<"vflowy"<<"focus_height"<<"focus_width";
                        } else if (cmbMicroscopy->currentData().toInt()==1 ) { // 1-comp, normal
                            acfmodel=ccfmodel="fccs_tir_diff_flowe2";
                            globalparams<<"n_particle"<<"n_components"<<"diff_coeff1"<<"diff_coeff2"<<"diff_coeff3"<<"diff_rho2"<<"diff_rho3"<<"vflowx"<<"vflowy"<<"focus_height"<<"focus_width";
                        } else if (((cmbMicroscopy->currentData().toInt()==3)||(cmbMicroscopy->currentData().toInt()==5))) { // anomalous
                            acfmodel=ccfmodel="fccs_2f_diff3dflow";
                            globalparams<<"n_particle"<<"n_components"<<"diff_coeff1"<<"diff_coeff2"<<"diff_coeff3"<<"diff_rho2"<<"diff_rho3"<<"vflowx"<<"vflowy"<<"focus_height"<<"focus_width";
                        } else if (cmbMicroscopy->currentData().toInt()==4 ) { // 1-comp, normal
                            acfmodel=ccfmodel="fccs_2f_diff2dflow";
                            e->setQFProperty("PRESET_FOCUS_HEIGHT", 1e6, false, false);
                            globalparams<<"n_particle"<<"n_components"<<"diff_coeff1"<<"diff_coeff2"<<"diff_coeff3"<<"diff_rho2"<<"diff_rho3"<<"vflowx"<<"vflowy"<<"focus_height"<<"focus_width";
                        }

                        if (cmbIm2fFCCSFitMode->currentIndex()==1 || cmbIm2fFCCSFitMode->currentIndex()==3) {
                            e->setQFProperty("PRESET_N_COMPONENTS", 2, false, false);
                        } else {
                            e->setQFProperty("PRESET_N_COMPONENTS", 1, false, false);
                        }

                        if (cmbIm2fFCCSFitMode->currentIndex()==2 || cmbIm2fFCCSFitMode->currentIndex()==3) {
                            e->setQFProperty("PRESET_VFLOWX", 0, false, false);
                            e->setQFProperty("PRESET_VFLOWY", 0, false, false);
                            e->setQFProperty("PRESET_VFLOWX_FIX", true, false, false);
                            e->setQFProperty("PRESET_VFLOWY_FIX", true, false, false);
                        }
                    } else if (cmbIm2fFCCSFitMode->currentIndex()==4 || cmbIm2fFCCSFitMode->currentIndex()==5) {
                        if (((cmbMicroscopy->currentData().toInt()==0)||(cmbMicroscopy->currentData().toInt()==2)) ) { // anomalous
                            acfmodel=ccfmodel="fccs_spim_adiffce2";
                            globalparams<<"n_particle"<<"n_components"<<"diff_acoeff1"<<"diff_acoeff2"<<"diff_acoeff3"<<"diff_alpha1"<<"diff_alpha2"<<"diff_alpha3"<<"diff_rho2"<<"diff_rho3"<<"focus_height"<<"focus_width";
                        } else if (cmbMicroscopy->currentData().toInt()==1 ) { // 1-comp, normal
                            acfmodel=ccfmodel="fccs_tir_adiffe2";
                            globalparams<<"n_particle"<<"n_components"<<"diff_acoeff1"<<"diff_acoeff2"<<"diff_acoeff3"<<"diff_alpha1"<<"diff_alpha2"<<"diff_alpha3"<<"diff_rho2"<<"diff_rho3"<<"focus_height"<<"focus_width";
                        } else if (((cmbMicroscopy->currentData().toInt()==3)||(cmbMicroscopy->currentData().toInt()==5))) { // anomalous
                            acfmodel=ccfmodel="fccs_2f_adiff3d_wz";
                            globalparams<<"n_particle"<<"n_components"<<"diff_acoeff1"<<"diff_acoeff2"<<"diff_acoeff3"<<"diff_alpha1"<<"diff_alpha2"<<"diff_alpha3"<<"diff_rho2"<<"diff_rho3"<<"focus_height"<<"focus_width";
                        } else if (cmbMicroscopy->currentData().toInt()==4 ) { // 1-comp, normal
                            acfmodel=ccfmodel="fccs_2f_adiff3d_wz";
                            e->setQFProperty("PRESET_FOCUS_HEIGHT", 1e6, false, false);
                            globalparams<<"n_particle"<<"n_components"<<"diff_acoeff1"<<"diff_acoeff2"<<"diff_acoeff3"<<"diff_alpha1"<<"diff_alpha2"<<"diff_alpha3"<<"diff_rho2"<<"diff_rho3"<<"focus_height"<<"focus_width";
                        }

                        if (cmbIm2fFCCSFitMode->currentIndex()==1) {
                            e->setQFProperty("PRESET_N_COMPONENTS", 2, false, false);
                        } else {
                            e->setQFProperty("PRESET_N_COMPONENTS", 1, false, false);
                        }

                    }




                    if (!acfmodel.isEmpty()) {
                        e->setQFProperty("PRESET_FIT_MODELS_LIST", constructQStringListFromItems(acfmodel, constructQStringListWithMultipleItems(ccfmodel, Nccf)).join(";"), false, false);
                        e->setQFProperty("PRESET_FIT_MODELS_ROLES_LIST", constructQStringListFromItems(QString("acf"), constructQStringListWithMultipleItems(QString("dccf"), Nccf)).join(";"), false, false);
                    } else {
                        e->setQFProperty("PRESET_FIT_MODELS_LIST", constructQStringListWithMultipleItems(ccfmodel, Nccf).join(";"), false, false);
                        e->setQFProperty("PRESET_FIT_MODELS_ROLES_LIST", constructQStringListWithMultipleItems("dccf", Nccf).join(";"), false, false);
                    }
                    e->setQFProperty("PRESET_FIT_MODELS_GLOBALPARAMS_LIST", globalparams.join(";"), false, false);

                }
            }
        }
    }
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
    //qDebug()<<"edtFilenameTextChanged";
    cmbFileformat->setEnabled(!filename.isEmpty());
    wizSelfiles->setExternalIsComplete(!filename.isEmpty());
}

void QFRDRImagingFCSWizard::initImagePreview()
{
    //qDebug()<<"initImagePreview";
    ProgramOptions::setConfigValue("imaging_fcs/wizard/microscopy", cmbMicroscopy->currentData().toInt());
    ProgramOptions::setConfigValue("imaging_fcs/wizard/wizardmethod", wizIntro->getChecked());
}

void QFRDRImagingFCSWizard::finishedImageProps()
{
    //qDebug()<<"finishedImageProps";
    chk2ColorFCCS->setEnabled(cmbDualView->currentIndex()>0);
    if (cmbDualView->currentIndex()>0) {
        chk2ColorFCCS->setChecked(true);
        chk2ColorFCCS->setText("");
        cmb2PixelFCCS->setCurrentIndex(0);
    } else {
        chk2ColorFCCS->setText("unavailable: you did not select a DualView mode on the previous pages!");
    }
    spinTauMax->setValue(double(widFrameRange->getLast()-widFrameRange->getFirst()+1)*spinFrametime->value()/1.0e6/2.0);
    spinTauMax->setMaximum(double(widFrameRange->getLast()-widFrameRange->getFirst()+1)*spinFrametime->value()/1.0e6);
    spinCalibTauMax->setValue(double(widFrameRange->getLast()-widFrameRange->getFirst()+1)*spinFrametime->value()/1.0e6/2.0);
    spinCalibTauMax->setMaximum(double(widFrameRange->getLast()-widFrameRange->getFirst()+1)*spinFrametime->value()/1.0e6);
}

void QFRDRImagingFCSWizard::initFileSelection()
{
    //qDebug()<<"initFileSelection";
    wizSelfiles->setUseExternalIsComplete(true);
    wizSelfiles->setExternalIsComplete(!edtFilename->text().isEmpty());
    if (wizIntro->isChecked(1)) {
        chkMoreFiles->setChecked(false);
        chkMoreFiles->setEnabled(false);
    }
}

void QFRDRImagingFCSWizard::finishedIntro()
{
    //qDebug()<<"finishedIntro";
    ProgramOptions::setConfigValue("imaging_fcs/wizard/wizardmethod", wizIntro->getChecked());
    isCalibration=wizIntro->isChecked(1);
    spinWxy->setEnabled(!isCalibration);
    labWxy->setVisible(!isCalibration);
    QWidget* lab=NULL;
    if ((lab=wizMicroscopy->getFormLayout()->labelForField(spinWxy))) {
        lab->setEnabled(spinWxy->isVisible());
    }
}

void QFRDRImagingFCSWizard::backgroundModeChanged(int mode)
{
    //qDebug()<<"backgroundModeChanged";
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
    //qDebug()<<"calibrationRegionChanged";
    spinCalibrationCenterSize->setEnabled(false);
    widCropCalibration->setEnabled(false);
    QColor col=QColor("red");
    wizCalibration->setROIColor(col);
    col.setAlphaF(0.2);
    wizCalibration->setROIFillColor(col);
    switch(region) {
        case 0: //all pixels
            widCropCalibration->setCropRange(0,image_width_io-1, 0,image_height_io-1);
            wizCalibration->setROI(0,0,image_width_io, image_height_io);
            wizCalibration->resetROI2();
            break;
        case 1: // "left half"
            widCropCalibration->setCropRange(0,image_width_io/2, 0, image_height_io);
            wizCalibration->setROI(0,0,image_width_io/2, image_height_io);
            wizCalibration->resetROI2();
            break;
        case 2: // "right half"
            widCropCalibration->setCropRange(image_width_io/2+1,image_width_io-1,0, image_height_io);
            wizCalibration->setROI(image_width_io/2,0,image_width_io/2, image_height_io);
            wizCalibration->resetROI2();
            break;
        case 3: // "top half"
            widCropCalibration->setCropRange(0,image_width_io-1,image_height_io/2+1, image_height_io-1);
            wizCalibration->setROI(0,image_height_io/2,image_width_io, image_height_io/2);
            wizCalibration->resetROI2();
            break;
        case 4: // "bottom half"
            widCropCalibration->setCropRange(0,image_width_io-1,0, image_height_io/2);
            wizCalibration->setROI(0,0,image_width_io, image_height_io/2);
            wizCalibration->resetROI2();
            break;
        case 5: // "center pixels"
            {
                spinCalibrationCenterSize->setEnabled(true);
                int w=spinCalibrationCenterSize->value();
                int x=qBound(0, image_width_io/2-w/2, image_width_io-1);
                int y=qBound(0, image_height_io/2-w/2, image_height_io-1);
                widCropCalibration->setCropRange(x,x+w-1,y,y+w-1);
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
                widCropCalibration->setCropRange(x,x+w-1,y,y+w-1);
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
                widCropCalibration->setCropRange(x,x+w-1,y,y+w-1);
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
                widCropCalibration->setCropRange(x,x+w-1,y,y+w-1);
                wizCalibration->resetROI2();
            }
            break;
        case 9: // "bottom center"
            {
                spinCalibrationCenterSize->setEnabled(true);
                int w=spinCalibrationCenterSize->value();
                int x=qBound(0, image_width_io/2-w/2, image_width_io-1);
                int y=qBound(0, image_height_io/4-w/2, image_height_io-1);
                widCropCalibration->setCropRange(x,x+w-1,y,y+w-1);
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
    //qDebug()<<"calibrationCropValuesChanged";
    calibrationRegionChanged(cmbCalibCropRegion->currentIndex());
}

void QFRDRImagingFCSWizard::cropRegionChanged(int region)
{
    ProgramOptions::setConfigValue("imaging_fcs/wizard/binning", spinBinning->value());
    //qDebug()<<"cropRegionChanged";
    spinCropCenterSize->setEnabled(false);
    widCrop->setEnabled(false);
    QColor col=QColor("red");
    wizCropAndBin->setROIColor(col);
    col.setAlphaF(0.2);
    wizCropAndBin->setROIFillColor(col);
    switch(region) {
        case 0: //all pixels
            widCrop->setCropRange(0,image_width_io-1, 0,image_height_io-1);
            wizCropAndBin->setROI(0,0,image_width_io, image_height_io);
            wizCropAndBin->resetROI2();
            break;
        case 1: // "left half"
            widCrop->setCropRange(0,image_width_io/2, 0, image_height_io);
            wizCropAndBin->setROI(0,0,image_width_io/2, image_height_io);
            wizCropAndBin->resetROI2();
            break;
        case 2: // "right half"
            widCrop->setCropRange(image_width_io/2+1,image_width_io-1,0, image_height_io);
            wizCropAndBin->setROI(image_width_io/2,0,image_width_io/2, image_height_io);
            wizCropAndBin->resetROI2();
            break;
        case 3: // "top half"
            widCrop->setCropRange(0,image_width_io-1,image_height_io/2+1, image_height_io-1);
            wizCropAndBin->setROI(0,image_height_io/2,image_width_io, image_height_io/2);
            wizCropAndBin->resetROI2();
            break;
        case 4: // "bottom half"
            widCrop->setCropRange(0,image_width_io-1,0, image_height_io/2);
            wizCropAndBin->setROI(0,0,image_width_io, image_height_io/2);
            wizCropAndBin->resetROI2();
            break;
        case 5: // "center pixels"
            {
                spinCropCenterSize->setEnabled(true);
                int w=spinCropCenterSize->value();
                int x=qBound(0, image_width_io/2-w/2, image_width_io-1);
                int y=qBound(0, image_height_io/2-w/2, image_height_io-1);
                widCrop->setCropRange(x,x+w-1,y,y+w-1);
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
                widCrop->setCropRange(x,x+w-1,y,y+w-1);
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
                widCrop->setCropRange(x,x+w-1,y,y+w-1);
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
                widCrop->setCropRange(x,x+w-1,y,y+w-1);
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
                widCrop->setCropRange(x,x+w-1,y,y+w-1);
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
    //qDebug()<<"cropValuesChanged";
    cropRegionChanged(cmbCropRegion->currentIndex());
    wizCropAndBin->setBinning(spinBinning->value());
}

void QFRDRImagingFCSWizard::microscopyChoosen()
{
    //qDebug()<<"microscopyChoosen";
    ProgramOptions::setConfigValue("imaging_fcs/wizard/microscopy", cmbMicroscopy->currentData().toInt());
    ProgramOptions::setConfigValue("imaging_fcs/wizard/calib_wz", spinWz->value());
    ProgramOptions::setConfigValue("imaging_fcs/wizard/calib_wxy", spinWxy->value());

    QString msg;


    bool isNotTIRF=(cmbMicroscopy->currentData().toInt()!=1)&&(cmbMicroscopy->currentData().toInt()!=4);
    labWz->setVisible(isNotTIRF);
    spinWz->setEnabled(isNotTIRF);
    QWidget* lab=NULL;
    if ((lab=wizMicroscopy->getFormLayout()->labelForField(spinWz))) {
        lab->setEnabled(spinWz->isEnabled());
    }

    //spinWxy->setEnabled(reallyIsCalib);
    //labWxy->setEnabled(reallyIsCalib);

    bool reallyIsCalib=wizIntro->isChecked(1) && this->isCalibration;
    if (reallyIsCalib && cmbMicroscopy->currentData().toInt()>=3) {
        msg+=tr("<br><center><b><u>Warning:</u> The microscopy technique you chose is not based on a pixelated detector. Therefore it cannot be used for imaging FCS calibration.</b> QuickFit uses a calibration method that stepwise increases the camera pixel binning to decouple the optical PSF-parameters (which are calibrated) from the focal volume size.</center><br>");
    }

    msg+=QString("<center><img src=\":/imaging_fcs/wizmicroscopy%1.png\"></center>").arg(cmbMicroscopy->currentData().toInt());

    spinWxy->setEnabled(!reallyIsCalib);
    labWxy->setVisible(!reallyIsCalib);
    lab=NULL;
    if ((lab=wizMicroscopy->getFormLayout()->labelForField(spinWxy))) {
        lab->setEnabled(spinWxy->isEnabled());
    }

    labMicroscopy->setText(msg);

}

void QFRDRImagingFCSWizard::calibrationSetupFinished()
{
    //qDebug()<<"calibrationSetupFinished";
    ProgramOptions::setConfigValue("imaging_fcs/wizard/calib_region", cmbCalibCropRegion->currentIndex());
}

void QFRDRImagingFCSWizard::cropSetupFinished()
{
    //qDebug()<<"cropSetupFinished";
    ProgramOptions::setConfigValue("imaging_fcs/wizard/crop_region", cmbCropRegion->currentIndex());
}

void QFRDRImagingFCSWizard::correlationValuesChanged()
{
    ProgramOptions::setConfigValue("imaging_fcs/wizard/segments", spinSegments->value());
    ProgramOptions::setConfigValue("imaging_fcs/wizard/2pixelfccs", cmb2PixelFCCS->currentIndex());
    ProgramOptions::setConfigValue("imaging_fcs/wizard/2colorfccs", chk2ColorFCCS->isChecked());
    ProgramOptions::setConfigValue("imaging_fcs/wizard/taumax", spinTauMax->value());
    ProgramOptions::setConfigValue("imaging_fcs/wizard/acf", chkACF->isChecked());

    //qDebug()<<"correlationValuesChanged";
    labSegments->setText(tr("=> segment length %1 s").arg(double(widFrameRange->getLast()-widFrameRange->getFirst()+1)*spinFrametime->value()/1.0e6/double(spinSegments->value())));
}

void QFRDRImagingFCSWizard::calibrationValuesChanged()
{
    //qDebug()<<"calibrationValuesChanged";
    labCalibSegments->setText(tr("=> segment length %1 s").arg(double(widFrameRange->getLast()-widFrameRange->getFirst()+1)*spinFrametime->value()/1.0e6/double(spinCalibSegments->value())));
}

void QFRDRImagingFCSWizard::validateCorrelation()
{
    if (chkLastImFCSFit1) chkLastImFCSFit1->setEnabled(true);
    if (chkLastIm2cFCCSFit) chkLastIm2cFCCSFit->setEnabled(chk2ColorFCCS->isChecked());
    if (chkLastIm2fFCCSFit) chkLastIm2fFCCSFit->setEnabled(cmb2PixelFCCS->currentIndex()>0);
}

void QFRDRImagingFCSWizard::addFile()
{
    if (edtFilename->text().isEmpty()) {
        QMessageBox::critical(this, tr("add additional file for processing"), tr("you can only add additional files for processing once, you selected the major file at the top of this wizard page!"), QMessageBox::Ok, QMessageBox::Ok);
    } else {
        QString lastImagefileDir=QDir::homePath();
        QString lastImagefileFilter="TIFF File [tinyTIFF] (*.tiff *.tif)";

        lastImagefileDir=ProgramOptions::getConfigValue("imaging_fcs/wizard/last_imagefile_dir", lastImagefileDir).toString();
        lastImagefileFilter=ProgramOptions::getConfigValue("imaging_fcs/wizard/last_imagefile_filter", lastImagefileFilter).toString();


        QString fileName = qfGetOpenFileName(this, tr("Select Image Stack File ..."), lastImagefileDir, imageFilters.value(cmbFileformat->currentIndex()), &lastImagefileFilter);
        if (!fileName.isEmpty()) {
            lastImagefileDir=QFileInfo(fileName).dir().absolutePath();
            lstMoreFiles->addItem(fileName);
            ProgramOptions::setConfigValue("imaging_fcs/wizard/last_imagefile_dir", lastImagefileDir);
            ProgramOptions::setConfigValue("imaging_fcs/wizard/last_imagefile_filter", lastImagefileFilter);
        }
    }
}

void QFRDRImagingFCSWizard::removeFiles()
{
    if (!chkMoreFiles->isChecked()) return;
    QList<QListWidgetItem *> itemList = lstMoreFiles->selectedItems();
    for (int i=0; i<itemList.size(); i++) {
        lstMoreFiles->takeItem(lstMoreFiles->row(itemList[i]));
        delete itemList[i];
    }
}

void QFRDRImagingFCSWizard::startProcessingJobs()
{
    QStringList files;
    files<<edtFilename->text();
    if (chkMoreFiles->isChecked() && lstMoreFiles->count()>0 && wizIntro->isChecked(0)) {
        for (int i=0; i<lstMoreFiles->count(); i++) {
            if (QFile::exists(lstMoreFiles->item(i)->text())) files<<lstMoreFiles->item(i)->text();
        }
    }
    validateCorrelation();

    for (int f=0; f<files.size(); f++) {
        IMFCSJob basicjob;
        basicjob.filename=files[f];
        basicjob.filenameBackground=edtBackgroundFilename->text();
        basicjob.fileFormat=cmbFileformat->currentIndex();
        basicjob.correlator=CORRELATOR_DIRECTAVG;
        basicjob.backgroundCorrection=cmbBackgroundMode->currentData().toInt();
        basicjob.backgroundOffset=spinBackgroundOffset->value();
        basicjob.P=16;
        basicjob.m=2;
        basicjob.segments=spinSegments->value();
        basicjob.frameTime=spinFrametime->value()*1e-6;
        basicjob.S=qBound(3, basicjob.getIdealS(), 200);
        basicjob.range_min=widFrameRange->getFirst();
        basicjob.range_max=widFrameRange->getLast();
        basicjob.addToProject=true;
        basicjob.prefix="./results/";
        basicjob.postfix="_wizard_corr%correlator%_back%backcorrection%_bleach%bleach%_bin%binning%_%COUNTER%";
        basicjob.acf=false;
        basicjob.statistics_frames=qBound(10,frame_count_io/1000,2000);
        basicjob.backstatistics_frames=qBound(10,background_count/100,2000);
        basicjob.statistics=true;
        basicjob.video=true;
        basicjob.video_frames=basicjob.statistics_frames;
        basicjob.binning=1;
        basicjob.binAverage=false;
        basicjob.use_cropping=false;
        basicjob.crop_x0=0;
        basicjob.crop_x1=image_width_io-1;
        basicjob.crop_y0=0;
        basicjob.crop_y1=image_height_io-1;

        basicjob.distanceCCF=false;
        basicjob.DCCFDeltaX.clear();
        basicjob.DCCFDeltaY.clear();
        basicjob.DCCFrole.clear();
        basicjob.bleach=cmbBleachCorrection->currentData().toInt();
        basicjob.bleachAvgFrames=qMax(20u,basicjob.statistics_frames/10);
        //qDebug()<<"BLEACH_CORRECTION: "<<basicjob.bleach<<"  frames="<<basicjob.bleachAvgFrames;
        basicjob.interleaved_binning=false;
        basicjob.cameraSettingsGiven=true;
        basicjob.cameraPixelWidth=widPixSize->getPixelWidth();
        basicjob.cameraPixelHeight=widPixSize->getPixelHeight();
        basicjob.dualViewMode=cmbDualView->currentData().toInt();
        basicjob.addFCCSSeparately=true;
        basicjob.addNandB=false;
        basicjob.useBlockingErrorEstimate=(basicjob.segments<=1);

        if (wizIntro->isChecked(1)) { // calibration
            basicjob.segments=spinCalibSegments->value();
            basicjob.S=qBound(3, basicjob.getIdealS(), 200);
            for (int b=1; b<=spinCalibBinMax->value(); b++) {
                IMFCSJob job=basicjob;
                job.acf=true;
                job.postfix="_wizardcalib_corr%correlator%_back%backcorrection%_bleach%bleach%_bin%binning%_%COUNTER%";
                job.use_cropping=cmbCalibCropRegion->currentIndex()>0;
                if (job.use_cropping) {
                    job.crop_x0=widCropCalibration->getX1();
                    job.crop_x1=widCropCalibration->getX2();
                    job.crop_y0=widCropCalibration->getY1();
                    job.crop_y1=widCropCalibration->getY2();
                }
                job.binning=b;
                widProcess->addJob(job);
            }
        } else { // normal correlation
            basicjob.postfix="_wizard_corr%correlator%_back%backcorrection%_bleach%bleach%_bin%binning%_%COUNTER%";
            basicjob.binning=spinBinning->value();
            basicjob.use_cropping=cmbCropRegion->currentIndex()>0;
            basicjob.segments=spinSegments->value();
            basicjob.S=qBound(3, basicjob.getIdealS(spinTauMax->value()), 200);

            if (basicjob.use_cropping) {
                basicjob.crop_x0=widCrop->getX1();
                basicjob.crop_x1=widCrop->getX2();
                basicjob.crop_y0=widCrop->getY1();
                basicjob.crop_y1=widCrop->getY2();
            }
            if (chkACF->isChecked()) {
                basicjob.acf=true;
            }
            if (chk2ColorFCCS->isChecked() && cmbDualView->currentData().toInt()>0) {
                basicjob.distanceCCF=true;
                if (cmbDualView->currentIndex()==DUALVIEW_HORICONTAL) {
                    basicjob.DCCFDeltaX << image_width_io/2/basicjob.binning;
                    basicjob.DCCFDeltaY << 0;
                    basicjob.DCCFrole<<QString("FCCS");
                    basicjob.distanceCCF=true;
                } else if (cmbDualView->currentIndex()==DUALVIEW_VERTICAL) {
                    basicjob.DCCFDeltaX << 0;
                    basicjob.DCCFDeltaY << image_height_io/2/basicjob.binning;
                    basicjob.DCCFrole<<QString("FCCS");
                    basicjob.distanceCCF=true;
                }
            }
            if (cmb2PixelFCCS->currentIndex()>0) {
                basicjob.distanceCCF=true;
                if (cmb2PixelFCCS->currentIndex()==1) {
                    basicjob.addDCCF(-1,0);
                    basicjob.addDCCF(1,0);
                    basicjob.addDCCF(0,-1);
                    basicjob.addDCCF(0,1);
                } else if (cmb2PixelFCCS->currentIndex()==2) {
                    basicjob.addDCCF(-1,0);
                    basicjob.addDCCF(1,0);
                    basicjob.addDCCF(0,-1);
                    basicjob.addDCCF(0,1);
                    basicjob.addDCCF(-1,-1);
                    basicjob.addDCCF(-1,1);
                    basicjob.addDCCF(1,-1);
                    basicjob.addDCCF(1,1);
                } else if (cmb2PixelFCCS->currentIndex()==3) {
                    basicjob.addDCCF(1,0);
                } else if (cmb2PixelFCCS->currentIndex()==4) {
                    basicjob.addDCCF(-1,0);
                } else if (cmb2PixelFCCS->currentIndex()==5) {
                    basicjob.addDCCF(0,1);
                } else if (cmb2PixelFCCS->currentIndex()==6) {
                    basicjob.addDCCF(0,-1);
                } else if (cmb2PixelFCCS->currentIndex()==7) {
                    basicjob.addDCCF(1,0);
                    basicjob.addDCCF(2,0);
                } else if (cmb2PixelFCCS->currentIndex()==8) {
                    basicjob.addDCCF(-1,0);
                    basicjob.addDCCF(-2,0);
                } else if (cmb2PixelFCCS->currentIndex()==9) {
                    basicjob.addDCCF(0,1);
                    basicjob.addDCCF(0,2);
                } else if (cmb2PixelFCCS->currentIndex()==10) {
                    basicjob.addDCCF(0,-1);
                    basicjob.addDCCF(0,-2);
                } else if (cmb2PixelFCCS->currentIndex()==11) {
                    for (int d=1; d<=5; d++) basicjob.addDCCF(d,0);
                } else if (cmb2PixelFCCS->currentIndex()==12) {
                    for (int d=1; d<=5; d++) basicjob.addDCCF(-d,0);
                } else if (cmb2PixelFCCS->currentIndex()==13) {
                    for (int d=1; d<=5; d++) basicjob.addDCCF(0,d);
                } else if (cmb2PixelFCCS->currentIndex()==14) {
                    for (int d=1; d<=5; d++) basicjob.addDCCF(0,-d);
                } else if (cmb2PixelFCCS->currentIndex()==15) {
                    for (int d=1; d<=10; d++) basicjob.addDCCF(d,0);
                } else if (cmb2PixelFCCS->currentIndex()==16) {
                    for (int d=1; d<=10; d++) basicjob.addDCCF(-d,0);
                } else if (cmb2PixelFCCS->currentIndex()==17) {
                    for (int d=1; d<=10; d++) basicjob.addDCCF(0,d);
                } else if (cmb2PixelFCCS->currentIndex()==18) {
                    for (int d=1; d<=10; d++) basicjob.addDCCF(0,-d);
                }
            }
            widProcess->addJob(basicjob);
        }
    }


}

void QFRDRImagingFCSWizard::calibWxyTestChanged()
{
    //qDebug()<<"calibWxyTestChanged";
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
    //qDebug()<<"QFRDRImagingFCSWizard_BackgroundIsValid::isValid(";
    if (wizard) {
        ProgramOptions::setConfigValue("imaging_fcs/wizard/bleach_correction", wizard->cmbBleachCorrection->currentIndex());
        wizard->labBackgroundError->setText("");
        if ((wizard->cmbFileformat->currentText()!=lastReaderID && lastFilename!=wizard->edtBackgroundFilename->text()) || (wizard->background_count<=0) || (wizard->background_height*wizard->background_width<=0)) {
            QFRDRImagingFCSCorrelationDialog::readBackgroundProperties(wizard->edtBackgroundFilename->text(), wizard->cmbFileformat->currentIndex(), page, &(wizard->background_width), &(wizard->background_height), &(wizard->background_count));
            lastFilename=wizard->edtBackgroundFilename->text();
            lastReaderID=wizard->cmbFileformat->currentText();
        } else {
            //qDebug()<<"skipped reading background";
        }
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
    //qDebug()<<"QFRDRImagingFCSWizard_ImagestackIsValid::isValid";
    if (wizard) {
        QModernProgressDialog progress(wizard);
        progress.setHasCancel(false);
        progress.setLabelText(QObject::tr("reading image file ..."));
        progress.setMode(true, false);
        progress.show();
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QString readerid=wizard->imageFormatIDs.value(wizard->cmbFileformat->currentIndex(), wizard->imageFormatIDs.value(0, ""));

        wizard->wizImageProps->setImageAvg(wizard->edtFilename->text(), readerid, 0, 20);
        wizard->wizCalibration->setImageAvg(wizard->edtFilename->text(), readerid, 0, 20);
        wizard->wizCropAndBin->setImageAvg(wizard->edtFilename->text(), readerid, 0, 20);
        wizard->calibrationRegionChanged(wizard->cmbCalibCropRegion->currentIndex());
        wizard->cropRegionChanged(wizard->cmbCropRegion->currentIndex());


        if ((readerid!=lastReaderID && lastFilename!=wizard->edtFilename->text()) || (!wizard->frame_data_io) || (wizard->frame_count_io<=0) || (wizard->image_height_io*wizard->image_width_io<=0)) {
            if (wizard->frame_data_io) qfFree(wizard->frame_data_io);
            wizard->frame_data_io=NULL;
            QFRDRImagingFCSCorrelationDialog::readStackProperties(wizard->edtFilename->text(), wizard->cmbFileformat->currentIndex(), true, true, wizard, &(wizard->channels), &(wizard->frame_count_io), &(wizard->filesize_io), &(wizard->frametime_io), &(wizard->baseline_offset_io), &(wizard->backgroundF_io), &(wizard->pixel_width_io), &(wizard->pixel_height_io), &(wizard->hasPixel_io), &(wizard->dualViewMode_io), &(wizard->image_width_io), &(wizard->image_height_io), &(wizard->inputconfigfile_io), &(wizard->frame_data_io), &(wizard->background_width), &(wizard->background_height), &(wizard->background_count));
            lastReaderID=readerid;
            lastFilename=wizard->edtFilename->text();
        } else {
            //qDebug()<<"skipped reading image stack";
        }


        wizard->labImageProps->setText(QObject::tr("frames: %1,   frame-size: %2x%3").arg(wizard->frame_count_io).arg(wizard->image_width_io).arg(wizard->image_height_io));
        wizard->widPixSize->setPixelSize(wizard->pixel_width_io, wizard->pixel_height_io);
        wizard->widFrameRange->setRange(0, wizard->frame_count_io-1);
        if (wizard->wizIntro->isChecked(1)) {
            wizard->cmbDualView->setCurrentIndex(0);
            wizard->cmbDualView->setEnabled(false);
        } else {
            wizard->cmbDualView->setCurrentIndex(wizard->dualViewMode_io);
            wizard->cmbDualView->setEnabled(true);
        }
        if (wizard->wizImageProps->getFormLayout()->labelForField(wizard->cmbDualView)) {
            wizard->wizImageProps->getFormLayout()->labelForField(wizard->cmbDualView)->setEnabled(wizard->cmbDualView->isEnabled());
        }


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
            wizard->cmbCalibCropRegion->setCurrentIndex(1);
        }
        if (wizard->cmbDualView->currentIndex()==2) {
            wizard->cmbCalibCropRegion->setCurrentIndex(3);
        }
        QApplication::restoreOverrideCursor();

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
