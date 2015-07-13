#include "qfrdrimagingfcswizard.h"
#include "qfpluginservices.h"
#include "programoptions.h"


QFRDRImagingFCSWizard::QFRDRImagingFCSWizard(bool isp, QWidget *parent):
    QFWizard(parent)
{
    pluginServices=QFPluginServices::getInstance();
    isProject=isp;
    imageFilters=QFRDRImagingFCSCorrelationJobThread::getImageFilterList(QFPluginServices::getInstance());
    imageFormatNames=QFRDRImagingFCSCorrelationJobThread::getImageFormatNameList(QFPluginServices::getInstance());

    setWindowTitle(tr("Imaging FCS/FCCS Wizard"));
    addPage(new QFTextWizardPage(tr("Introduction"),
                                      tr("This wizard will help you to correlate an image series in order to perform an imaging FCS or FCCS evaluation<br><br><br><center><img src=\":/imaging_fcs/imfcs_flow.png\"></center>"),
                                      this));
    addPage(wizSelfiles=new QFFormWizardPage(tr("Select image stack files ..."), this));
    wizSelfiles->setSubTitle(tr("Select the image stack that you want to correlate."));
    wizSelfiles->setExternalValidate(true);
    wizSelfiles->setExternalIsValid(false);


    edtFilename=new QFEnhancedLineEdit(wizSelfiles);
    edtFilename->addButton(btnFilename=new QFStyledButton(QFStyledButton::None, edtFilename, edtFilename));
    btnFilename->setDefaultIcon(QFStyledButton::SelectFile);
    connect(btnFilename, SIGNAL(clicked()), this, SLOT(selectFileClicked()));
    cmbFileformat=new QFEnhancedComboBox(wizSelfiles);
    cmbFileformat->addItems(imageFormatNames);
    cmbFileformat->findAndSelectContainedLCText("tinytiff", -1);
    cmbFileformat->setEnabled(false);
    if (cmbFileformat->currentIndex()<0) cmbFileformat->findAndSelectContainedLCText("tiff", -1);
    wizSelfiles->addRow(tr("image stack:"), edtFilename);
    wizSelfiles->addRow(tr("import filter/filetype:"), cmbFileformat);


    addPage(wizImageProps=new QFImagePlotWizardPage(tr("Set image stack properties ..."), this));
    wizImageProps->setSubTitle(tr("Set/check the properties of the image stack."));
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
    wizImageProps->addRow(tr("DualView mode:"), cmbDualView);



//    addPage(wizLSAnalysisImgPreview=new QFImagePlotWizardPage(tr("Image preview ..."), this));
//    wizLSAnalysisImgPreview->setSubTitle(tr("Please set the image properties below the overview plot!\nThe plot displays the first frame from the first file"));
//    wizSelfiles->setUserOnValidatePage(wizLSAnalysisImgPreview);
//    connect(wizSelfiles, SIGNAL(onValidate(QWizardPage*,QWizardPage*)), this, SLOT(wizLSAnalysisImgPreviewOnValidate(QWizardPage*,QWizardPage*)));
//    wizLSAnalysisImgPreview->clear();



//    wizLSAnalysiscmbStackMode=new QComboBox(wizLSAnalysisImgPreview);
//    wizLSAnalysiscmbStackMode->addItem(QIcon(":/image_stack/multifile_stack_large.png"), tr("multi-file image stack"));
//    wizLSAnalysiscmbStackMode->addItem(QIcon(":/image_stack/singlefile_stack_large.png"), tr("single-file image stack)"));
//    wizLSAnalysiscmbStackMode->addItem(QIcon(":/image_stack/singlefile_dvh_stack_large.png"), tr("horizontal dual-view"));
//    wizLSAnalysiscmbStackMode->addItem(QIcon(":/image_stack/singlefile_dvv_stack_large.png"), tr("vertical dual-view"));
//    wizLSAnalysiscmbStackMode->setCurrentIndex(1);
//    wizLSAnalysiscmbStackMode->setMinimumHeight(36);
//    wizLSAnalysiscmbStackMode->setIconSize(QSize(32,32));

//    wizLSAnalysisedtPixelSize=new QDoubleSpinBox(wizLSAnalysisImgPreview);
//    wizLSAnalysisedtPixelSize->setRange(0,100000);
//    wizLSAnalysisedtPixelSize->setSuffix(" nm");
//    wizLSAnalysisedtPixelSize->setValue(400);
//    wizLSAnalysisedtPixelSize->setDecimals(2);

//    wizLSAnalysisedtStepSize=new QDoubleSpinBox(wizLSAnalysisImgPreview);
//    wizLSAnalysisedtStepSize->setRange(0,100000);
//    wizLSAnalysisedtStepSize->setSuffix(" nm");
//    wizLSAnalysisedtStepSize->setValue(1000);
//    wizLSAnalysisedtStepSize->setDecimals(2);


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
        edtFilenameTextChanged(fileName);
        ProgramOptions::setConfigValue("imaging_fcs/dlg_correlate/last_imagefile_dir", lastImagefileDir);
        ProgramOptions::setConfigValue("imaging_fcs/dlg_correlate/last_imagefile_filter", lastImagefileFilter);
    }
}

void QFRDRImagingFCSWizard::edtFilenameTextChanged(const QString &filename)
{
    cmbFileformat->setEnabled(!filename.isEmpty());
    wizSelfiles->setExternalIsValid(!filename.isEmpty());
}

void QFRDRImagingFCSWizard::initImagePreview()
{
    QFImporterImageSeries* reader=NULL;
    QString readerid="";
    if (cmbFileformat->currentIndex()>=0 && cmbFileformat->currentIndex()<QFRDRImagingFCSCorrelationJobThread::getImageReaderCount(pluginServices)) {
        reader=QFRDRImagingFCSCorrelationJobThread::getImageReader(cmbFileformat->currentIndex(), pluginServices);
        if (reader) {
            readerid=reader->formatName();
            delete reader;
        }
    }
    wizImageProps->setImageAvg(edtFilename->text(), readerid, 0, 10);
}


