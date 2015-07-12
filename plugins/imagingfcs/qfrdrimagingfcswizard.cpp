#include "qfrdrimagingfcswizard.h"
#include "qfpluginservices.h"
#include "programoptions.h"


QFRDRImagingFCSWizard::QFRDRImagingFCSWizard(bool isp, QWidget *parent):
    QFWizard(parent)
{
    isProject=isp;
    imageFilters=QFRDRImagingFCSCorrelationJobThread::getImageFilterList(QFPluginServices::getInstance());
    imageFormatNames=QFRDRImagingFCSCorrelationJobThread::getImageFormatNameList(QFPluginServices::getInstance());

    setWindowTitle(tr("Imaging FCS/FCCS Wizard"));
    addPage(new QFTextWizardPage(tr("Introduction"),
                                      tr("This wizard will help you to correlate an image series in order to perform an imaging FCS or FCCS evaluation<br><br><br><center><img src=\":/imaging_fcs/imfcs_flow.png\"></center>"),
                                      this));
    addPage(wizSelfiles=new QFSelectFilesListWizardPage(tr("Image stack files ..."), this));
    wizSelfiles->setSubTitle(tr("<b>Select one or more image stacks to correlate.</b><br>"
                                "<i><u>Note:</u> If you select several image stacks, they will all be processed with the same settings, so they have to be acquired with the same camera/microscope settings (\"Batch Mode\")</i>"
                                "<br>You can click on '+' to add files, '-' to remove files and use the arrow buttons to change the order of the files in the list."));
    wizSelfiles->setFilters(imageFormatNames, imageFilters);
    wizSelfiles->setSettingsIDs("imaging_fcs/corrwizard/lastdir", "imaging_fcs/corrwizard/lastfilter");
    wizSelfiles->setAddOnStartup(false);
    wizSelfiles->setOnlyOneFormatAllowed(true);

    addPage(wizLSAnalysisImgPreview=new QFImagePlotWizardPage(tr("Image preview ..."), this));
    wizLSAnalysisImgPreview->setSubTitle(tr("Please set the image properties below the overview plot!\nThe plot displays the first frame from the first file"));
    wizSelfiles->setUserOnValidatePage(wizLSAnalysisImgPreview);
    connect(wizSelfiles, SIGNAL(onValidate(QWizardPage*,QWizardPage*)), this, SLOT(wizLSAnalysisImgPreviewOnValidate(QWizardPage*,QWizardPage*)));
    wizLSAnalysisImgPreview->clear();



    wizLSAnalysiscmbStackMode=new QComboBox(wizLSAnalysisImgPreview);
    wizLSAnalysiscmbStackMode->addItem(QIcon(":/image_stack/multifile_stack_large.png"), tr("multi-file image stack"));
    wizLSAnalysiscmbStackMode->addItem(QIcon(":/image_stack/singlefile_stack_large.png"), tr("single-file image stack)"));
    wizLSAnalysiscmbStackMode->addItem(QIcon(":/image_stack/singlefile_dvh_stack_large.png"), tr("horizontal dual-view"));
    wizLSAnalysiscmbStackMode->addItem(QIcon(":/image_stack/singlefile_dvv_stack_large.png"), tr("vertical dual-view"));
    wizLSAnalysiscmbStackMode->setCurrentIndex(1);
    wizLSAnalysiscmbStackMode->setMinimumHeight(36);
    wizLSAnalysiscmbStackMode->setIconSize(QSize(32,32));

    wizLSAnalysisedtPixelSize=new QDoubleSpinBox(wizLSAnalysisImgPreview);
    wizLSAnalysisedtPixelSize->setRange(0,100000);
    wizLSAnalysisedtPixelSize->setSuffix(" nm");
    wizLSAnalysisedtPixelSize->setValue(400);
    wizLSAnalysisedtPixelSize->setDecimals(2);

    wizLSAnalysisedtStepSize=new QDoubleSpinBox(wizLSAnalysisImgPreview);
    wizLSAnalysisedtStepSize->setRange(0,100000);
    wizLSAnalysisedtStepSize->setSuffix(" nm");
    wizLSAnalysisedtStepSize->setValue(1000);
    wizLSAnalysisedtStepSize->setDecimals(2);


    if (wizLSAnalysiscmbStackMode) {
        ProgramOptions::getConfigQComboBox(wizLSAnalysiscmbStackMode, "image_stack/startProjectWizardLightsheetAnalysis/wizLSAnalysiscmbStackMode");
        wizLSAnalysisImgPreview->addRow(tr("stack mode"), wizLSAnalysiscmbStackMode);
    }
    if (wizLSAnalysisedtPixelSize) {
        ProgramOptions::getConfigQDoubleSpinBox(wizLSAnalysisedtPixelSize, "image_stack/startProjectWizardLightsheetAnalysis/wizLSAnalysisedtPixelSize");
        wizLSAnalysisImgPreview->addRow(tr("pixel size"), wizLSAnalysisedtPixelSize);
    }
    if (wizLSAnalysisedtStepSize) {
        ProgramOptions::getConfigQDoubleSpinBox(wizLSAnalysisedtStepSize, "image_stack/startProjectWizardLightsheetAnalysis/wizLSAnalysisedtStepSize");
        wizLSAnalysisImgPreview->addRow(tr("step size"), wizLSAnalysisedtStepSize);
    }

}

QFRDRImagingFCSWizard::~QFRDRImagingFCSWizard()
{

}

