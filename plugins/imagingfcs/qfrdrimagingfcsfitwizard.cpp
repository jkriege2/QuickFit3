#include "qfrdrimagingfcsfitwizard.h"
#include "qfpluginservices.h"
#include "programoptions.h"
#include "qfrdrimagingfcs.h"

QFRDRImagingFCFitSWizard::QFRDRImagingFCFitSWizard(QWidget *parent):
    QFWizard(QSize(600, 440), parent, QString("imaging_fcs/wizard/"))
{
    QLabel* lab;

    setOption(QWizard::NoCancelButtonOnLastPage);
    setOption(QWizard::NoBackButtonOnLastPage);
    setOption(QWizard::NoBackButtonOnStartPage);


    setWindowTitle(tr("Imaging FCS/FCCS Fit Wizard"));
    setPage(InitPage, wizIntro=new QFTextWizardPage(tr("Introduction"), this));
    wizIntro->setText(tr("This wizard will help you set up a project for imaging (2-pixel/2-color) FCS or FCCS fitting.<br>"
                        "<u>Note:</u> This wizard assumes that you have already correlated the data. If not, use one of the other imaging FCS wizards, which also allow to fit data.<br><br>"
                        ));


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



    setPage(LastPage, wizFinalizePage=new QFFormWizardPage(tr("Finalize"), this));
    //connect(wizFinalizePage, SIGNAL())
    connect(wizFinalizePage, SIGNAL(onInitialize(QWizardPage*)), this, SLOT(validateCorrelation()));

    //labFinal=new QLabel(this);
    //labFinal->setWordWrap(true);
    //wizFinalizePage->addRow(labFinal);
    wizFinalizePage->setFinalPage(true);
    wizFinalizePage->setNoPreviousButton(true);
    wizFinalizePage->setNoCancelButton(true);
    //labFinal->setText(tr("You completed this wizard. The selected files will now be inserted as imaging FCS raw data records (RDR) into the project.<br><br><b>Please select the evaluation objects that should be added to the project below.</b>"));
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

QFRDRImagingFCFitSWizard::~QFRDRImagingFCFitSWizard()
{
}

void QFRDRImagingFCFitSWizard::finalizeAndModifyProject(bool projectwizard, QFRDRImagingFCSPlugin *plugin)
{

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
        if (chkLastIm2cFCCSFit && chkLastIm2cFCCSFit->isChecked()) {
            QFEvaluationItem* e=project->addEvaluation("imfccs_fit", tr("Global 2-color Imaging FCCS fit"));
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
        if (chkLastIm2fFCCSFit && chkLastIm2fFCCSFit->isChecked()) {
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
                int Nccf=4;
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
                    e->setQFProperty("PRESET_FIT_MODELS_LIST", constructQStringListFromItems(acfmodel, constructQStringListWithMultipleItems(ccfmodel, Nccf)).join(';'), false, false);
                    e->setQFProperty("PRESET_FIT_MODELS_ROLES_LIST", constructQStringListFromItems("acf", constructQStringListWithMultipleItems("dccf", Nccf)).join(';'), false, false);
                } else {
                    e->setQFProperty("PRESET_FIT_MODELS_LIST", constructQStringListWithMultipleItems(ccfmodel, Nccf).join(';'), false, false);
                    e->setQFProperty("PRESET_FIT_MODELS_ROLES_LIST", constructQStringListWithMultipleItems("dccf", Nccf).join(';'), false, false);
                }
                e->setQFProperty("PRESET_FIT_MODELS_GLOBALPARAMS_LIST", globalparams.join(';'), false, false);

            }
        }
    }
}







void QFRDRImagingFCFitSWizard::finishedIntro()
{
    //qDebug()<<"finishedIntro";
    spinWxy->setEnabled(true);
    labWxy->setVisible(true);
    QWidget* lab=NULL;
    if ((lab=wizMicroscopy->getFormLayout()->labelForField(spinWxy))) {
        lab->setEnabled(spinWxy->isVisible());
    }
}





void QFRDRImagingFCFitSWizard::microscopyChoosen()
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

    msg+=QString("<center><img src=\":/imaging_fcs/wizmicroscopy%1.png\"></center>").arg(cmbMicroscopy->currentData().toInt());

    spinWxy->setEnabled(true);
    labWxy->setVisible(true);
    lab=NULL;
    if ((lab=wizMicroscopy->getFormLayout()->labelForField(spinWxy))) {
        lab->setEnabled(spinWxy->isEnabled());
    }

    labMicroscopy->setText(msg);

}



void QFRDRImagingFCFitSWizard::validateCorrelation()
{
    chkLastImFCSFit1->setEnabled(true);
    chkLastIm2cFCCSFit->setEnabled(true);
    chkLastIm2fFCCSFit->setEnabled(true);
}



