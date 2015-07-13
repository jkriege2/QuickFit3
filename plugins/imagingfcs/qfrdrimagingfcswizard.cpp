#include "qfrdrimagingfcswizard.h"
#include "qfpluginservices.h"
#include "programoptions.h"


QFRDRImagingFCSWizard::QFRDRImagingFCSWizard(bool isp, QWidget *parent):
    QFWizard(parent, QString("imaging_fcs/wizard/"))
{
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
    isProject=isp;
    imageFilters=QFRDRImagingFCSCorrelationJobThread::getImageFilterList();
    imageFormatNames=QFRDRImagingFCSCorrelationJobThread::getImageFormatNameList();
    imageFormatIDs=QFRDRImagingFCSCorrelationJobThread::getImageFormatIDList();
    qDebug()<<imageFilters;
    qDebug()<<imageFormatNames;
    qDebug()<<imageFormatIDs;

    setWindowTitle(tr("Imaging FCS/FCCS Wizard"));
    addPage(new QFTextWizardPage(tr("Introduction"),
                                      tr("This wizard will help you to correlate an image series in order to perform an imaging FCS or FCCS evaluation<br><br><br><center><img src=\":/imaging_fcs/imfcs_flow.png\"></center>"),
                                      this));
    addPage(wizSelfiles=new QFFormWizardPage(tr("Select image stack files ..."), this));
    connect(wizSelfiles, SIGNAL(onInitialize(QWizardPage*)), this, SLOT(initFileSelection()));
    wizSelfiles->setSubTitle(tr("Select the image stack that you want to correlate."));
    wizSelfiles->setExternalValidate(true);
    wizSelfiles->setExternalIsValid(false);


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


    addPage(wizImageProps=new QFImagePlotWizardPage(tr("Set image stack properties ..."), this));
    wizImageProps->setSubTitle(tr("Set/check the properties of the image stack.<br><small><i>The plot shows an average over the first 10 frames.</i></small>"));
    connect(wizImageProps, SIGNAL(onInitialize(QWizardPage*)), this, SLOT(initImagePreview()));

//    edtBackground=new QFEnhancedLineEdit(wizImageProps);
//    edtBackground->addButton(btnBackground=new QFStyledButton(QFStyledButton::SelectFile, edtFilename, edtFilename));
//    btnBackground->setFilter(imageFilters.join(";;"));
//    chkBackground=new QCheckBox(tr("background correction:"), wizImageProps);
//    edtBackground->setEnabled(false);
//    connect(chkBackground, SIGNAL(toggled(bool)), edtBackground, SLOT(setEnabled(bool)));
//    wizImageProps->addRow(chkBackground, edtBackground);

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
    widPixSize->addLayoutStretchAtEnd();
    wizImageProps->addRow(tr("&Pixel Size:"), widPixSize);

    spinFrametime=new QDoubleSpinBox(wizImageProps);
    spinFrametime->setRange(0,1000000000);
    spinFrametime->setDecimals(3);
    spinFrametime->setValue(1000);
    spinFrametime->setSuffix(tr("\xB5s"));
    wizImageProps->addRow(tr("Frame &Time (=1/frame rate):"), spinFrametime);

    widFrameRange=new QFFrameRangeEdit(wizImageProps);
    widFrameRange->setRange(0,1000);
    widFrameRange->addLayoutStretchAtEnd();
    wizImageProps->addRow(tr("Correlated Frame &Range:"), widFrameRange);


//    addPage(wizLSAnalysisImgPreview=new QFImagePlotWizardPage(tr("Image preview ..."), this));
//    wizLSAnalysisImgPreview->setSubTitle(tr("Please set the image properties below the overview plot!\nThe plot displays the first frame from the first file"));
//    wizSelfiles->setUserOnValidatePage(wizLSAnalysisImgPreview);
//    connect(wizSelfiles, SIGNAL(onValidate(QWizardPage*,QWizardPage*)), this, SLOT(wizLSAnalysisImgPreviewOnValidate(QWizardPage*,QWizardPage*)));
//    wizLSAnalysisImgPreview->clear();







//    if (wizLSAnalysiscmbStackMode) {
//        ProgramOptions::getConfigQComboBox(wizLSAnalysiscmbStackMode, "image_stack/startProjectWizardLightsheetAnalysis/wizLSAnalysiscmbStackMode");
//        wizLSAnalysisImgPreview->addRow(tr("stack mode"), wizLSAnalysiscmbStackMode);
//    }
//    if (wizLSAnalysisedtPixelSize) {
//        ProgramOptions::getConfigQDoubleSpinBox(wizLSAnalysisedtPixelSize, "image_stack/startProjectWizardLightsheetAnalysis/wizLSAnalysisedtPixelSize");
//        wizLSAnalysisImgPreview->addRow(tr("pixel size"), wizLSAnalysisedtPixelSize);
//    }
//    if (wizLSAnalysisedtStepSize) {
//        ProgramOptions::getConfigQDoubleSpinBox(wizLSAnalysisedtStepSize, "image_stack/startProjectWizardLightsheetAnalysis/wizLSAnalysisedtStepSize");
//        wizLSAnalysisImgPreview->addRow(tr("step size"), wizLSAnalysisedtStepSize);
//    }

}

QFRDRImagingFCSWizard::~QFRDRImagingFCSWizard()
{

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
    //QFImporterImageSeries* reader=NULL;
    QString readerid=imageFormatIDs.value(cmbFileformat->currentIndex(), imageFormatIDs.value(0, ""));

    wizImageProps->setImageAvg(edtFilename->text(), readerid, 0, 10);


    double* frame_data_io=NULL;
    QFRDRImagingFCSCorrelationDialog::readStackProperties(edtFilename->text(), cmbFileformat->currentIndex(), true, true, this, &channels, &frame_count_io, &filesize_io, &frametime_io, &baseline_offset_io, &backgroundF_io, &pixel_width_io, &pixel_height_io, &hasPixel_io, &dualViewMode_io, &image_width_io, &image_height_io, &inputconfigfile_io, &frame_data_io);

    widPixSize->setPixelSize(pixel_width_io, pixel_height_io);
    widFrameRange->setRange(0, frame_count_io-1);
    cmbDualView->setCurrentIndex(dualViewMode_io);
    spinFrametime->setValue(frametime_io);
    if (frame_data_io) qfFree(frame_data_io);
}

void QFRDRImagingFCSWizard::initFileSelection()
{
    wizSelfiles->setExternalValidate(true);
    wizSelfiles->setExternalIsValid(!edtFilename->text().isEmpty());

}


