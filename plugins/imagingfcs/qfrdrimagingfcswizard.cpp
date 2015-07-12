#include "qfrdrimagingfcswizard.h"


QFRDRImagingFCSWizard::QFRDRImagingFCSWizard(QDialog *parent):
    QFWizard(parent)
{
    imageFilters=QFRDRImagingFCSCorrelationJobThread::getImageFilterList(pluginServices);
    imageFormatNames=QFRDRImagingFCSCorrelationJobThread::getImageFormatNameList(pluginServices);

    setWindowTitle(tr("Imaging FCS/FCCS Wizard"));
    addPage(new QFTextWizardPage(tr("Introduction"),
                                      tr("This wizard will help you to correlate an image series in order to perform an imaging FCS or FCCS evaluation<center><img src=\":/imaging_fcs/imfcs_flow.png\"></center>"),
                                      wiz));
    addPage(wizSelfiles=new QFSelectFilesListWizardPage(tr("Image stack files ...")));
    wizSelfiles->setSubTitle(tr("<b>Select one or more image stacks to correlate.</b><br>"
                                "<i><u>Note:</u> If you select several image stacks, they will all be processed with the same settings, so they have to be acquired with the same camera/microscope settings (\"Batch Mode\")</i>"
                                "<br>You can click on '+' to add files, '-' to remove files and use the arrow buttons to change the order of the files in the list."));
    wizSelfiles->setFilters(imageFormatNames, imageFilters);
    wizSelfiles->setSettingsIDs("imaging_fcs/corrwizard/lastdir", "imaging_fcs/corrwizard/lastfilter");
    wizSelfiles->setAddOnStartup(false);
    wizSelfiles->setOnlyOneFormatAllowed(true);

    addPage(wizLSAnalysisImgPreview=new QFImagePlotWizardPage(tr("Image preview ...")));
    wizLSAnalysisImgPreview->setSubTitle(tr("Please set the image properties below the overview plot!\nThe plot displays the first frame from the first file"));
    wizSelfiles->setUserOnValidatePage(wizLSAnalysisImgPreview);
    connect(wizSelfiles, SIGNAL(onValidate(QWizardPage*,QWizardPage*)), this, SLOT(wizLSAnalysisImgPreviewOnValidate(QWizardPage*,QWizardPage*)));
    wizLSAnalysisImgPreview->clear();

}

QFRDRImagingFCSWizard::~QFRDRImagingFCSWizard()
{

}

