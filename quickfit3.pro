


include(autoversioning.pri)
include(quickfit_config.pri)

release {
    message("building QuickFit in RELEASE mode")
} else {
    message("building QuickFit in DEBUG mode")
}
TEMPLATE = subdirs


SUBDIRS += lib libqf3widgets application
application.depends = lib libqf3widgets

# also build the plugins
SUBDIRS += plg_table
plg_table.subdir = ./plugins/table
plg_table.depends = lib libqf3widgets

SUBDIRS += plg_fcs
plg_fcs.subdir = ./plugins/fcs
plg_fcs.depends = lib libqf3widgets

SUBDIRS += plg_imagingfcs
plg_imagingfcs.subdir = ./plugins/imagingfcs
plg_imagingfcs.depends = lib libqf3widgets

SUBDIRS += plg_fcsfit
plg_fcsfit.subdir = ./plugins/fcsfit
plg_fcsfit.depends = lib libqf3widgets

SUBDIRS += plg_imfcsfit
plg_imfcsfit.subdir = ./plugins/imfcsfit
plg_imfcsfit.depends = lib libqf3widgets

SUBDIRS += plg_imfccsfit
plg_imfccsfit.subdir = ./plugins/imfccsfit
plg_imfccsfit.depends = lib libqf3widgets

SUBDIRS += plg_photoncounts
plg_photoncounts.subdir = ./plugins/photoncounts
plg_photoncounts.depends = lib libqf3widgets

SUBDIRS += plg_tcspcimporter
plg_tcspcimporter.subdir = ./plugins/tcspcimporter
plg_tcspcimporter.depends = lib libqf3widgets

SUBDIRS += plg_fcsmaxent
plg_fcsmaxent.subdir = ./plugins/qffcsmaxentevaluation
plg_fcsmaxent.depends = lib libqf3widgets

SUBDIRS += plg_fcsmsdevaluation
plg_fcsmsdevaluation.subdir = ./plugins/fcsmsdevaluation
plg_fcsmsdevaluation.depends = lib libqf3widgets

SUBDIRS += plg_fcs_fitfuctions
plg_fcs_fitfuctions.subdir=./plugins/fcs_fitfuctions

SUBDIRS += plg_fitfunction_dls
plg_fitfunction_dls.subdir=./plugins/fitfunction_dls

SUBDIRS += plg_fitfunction_spimfcs
plg_fitfunction_spimfcs.subdir=./plugins/fitfunction_spimfcs

SUBDIRS += plg_fitfunction_fcsdistribution
plg_fitfunction_fcsdistribution.subdir=./plugins/fitfunction_fcsdistribution

SUBDIRS += plg_fitfunction_fccs
plg_fitfunction_fccs.subdir=./plugins/fitfunction_fccs

SUBDIRS += plg_fitfunction_2ffcs
plg_fitfunction_2ffcs.subdir=./plugins/fitfunction_2ffcs

SUBDIRS += plg_fitalgorithm_levmar
plg_fitalgorithm_levmar.subdir=./plugins/fitalgorithm_levmar
plg_fitalgorithm_levmar.depends = lib libqf3widgets

SUBDIRS += plg_fitalgorithm_simanneal
plg_fitalgorithm_simanneal.subdir=./plugins/fitalgorithm_simanneal
plg_fitalgorithm_simanneal.depends = lib libqf3widgets

SUBDIRS += plg_fitalgorithm_fit_lmfit
plg_fitalgorithm_fit_lmfit.subdir=./plugins/fitalgorithm_fit_lmfit
plg_fitalgorithm_fit_lmfit.depends = lib libqf3widgets

SUBDIRS += plg_basicimageimporters
plg_basicimageimporters.subdir=./plugins/basicimageimporters
plg_basicimageimporters.depends = lib

SUBDIRS += plg_picoquantimporters
plg_picoquantimporters.subdir=./plugins/picoquantimporters
plg_picoquantimporters.depends = lib

SUBDIRS += plg_imagestack
plg_imagestack.subdir=./plugins/imagestack
plg_imagestack.depends = lib libqf3widgets

SUBDIRS += plg_rdrsettings
plg_rdrsettings.subdir=./plugins/rdrsettings
plg_rdrsettings.depends = lib libqf3widgets

SUBDIRS += plg_calc_diffcoeff
plg_calc_diffcoeff.subdir=./plugins/calc_diffcoeff
plg_calc_diffcoeff.depends = lib libqf3widgets

SUBDIRS += plg_qfe_calculator
plg_qfe_calculator.subdir=./plugins/qfe_calculator
plg_qfe_calculator.depends = lib libqf3widgets

SUBDIRS += plg_qfe_acquisitiontest
plg_qfe_acquisitiontest.subdir=./plugins/qfe_acquisitiontest
plg_qfe_acquisitiontest.depends = lib libqf3widgets

SUBDIRS += plg_qfe_resultstools
plg_qfe_resultstools.subdir=./plugins/qfe_resultstools
plg_qfe_resultstools.depends = lib libqf3widgets

SUBDIRS += plg_numberandbrightness
plg_numberandbrightness.subdir=./plugins/numberandbrightness
plg_numberandbrightness.depends = lib libqf3widgets

SUBDIRS += plg_spim_lightsheet_eval
plg_spim_lightsheet_eval.subdir=./plugins/spim_lightsheet_eval
plg_spim_lightsheet_eval.depends = lib libqf3widgets

SUBDIRS += plg_fitfunctions_lightsheet
plg_fitfunctions_lightsheet.subdir=./plugins/fitfunctions_lightsheet
plg_fitfunctions_lightsheet.depends = lib libqf3widgets

SUBDIRS += plg_qfe_helpeditor
plg_qfe_helpeditor.subdir=./plugins/qfe_helpeditor
plg_qfe_helpeditor.depends = lib libqf3widgets

SUBDIRS += plg_qfe_defaultmathparserextensions
plg_qfe_defaultmathparserextensions.subdir=./plugins/qfe_defaultmathparserextensions
plg_qfe_defaultmathparserextensions.depends = lib libqf3widgets

!macx {

    SUBDIRS += plg_spimb040
    plg_spimb040.subdir=./plugins/spimb040
    plg_spimb040.depends = lib libqf3widgets

    SUBDIRS += plg_cam_testcamera
    plg_cam_testcamera.subdir=./plugins/cam_testcamera
    plg_cam_testcamera.depends = lib libqf3widgets

    !contains(QF3CONFIG, bits64) {
        SUBDIRS += plg_cam_systemcamera
        plg_cam_systemcamera.subdir=./plugins/cam_systemcamera
        plg_cam_systemcamera.depends = lib libqf3widgets
    }

    !contains(QF3CONFIG, nospecialdrivers) {
        SUBDIRS += plg_cam_radhard2
        plg_cam_radhard2.subdir=./plugins/cam_radhard2
        plg_cam_radhard2.depends = lib libqf3widgets

        SUBDIRS += plg_cam_andor
        plg_cam_andor.subdir=./plugins/cam_andor
        plg_cam_andor.depends = lib libqf3widgets

        unix {
            SUBDIRS += plg_cam_rh2v2
            plg_cam_rh2v2.subdir=./plugins/cam_rh2v2
            plg_cam_rh2v2.depends = lib libqf3widgets
        }
    }

    SUBDIRS += plg_stage_pi
    plg_stage_pi.subdir=./plugins/stage_pi
    plg_stage_pi.depends = lib libqf3widgets

    SUBDIRS += plg_shutter_servo_arduino
    plg_shutter_servo_arduino.subdir=./plugins/shutter_servo_arduino
    plg_shutter_servo_arduino.depends = lib libqf3widgets

    SUBDIRS += plg_filterc_test
    plg_filterc_test.subdir=./plugins/filterc_test
    plg_filterc_test.depends = lib libqf3widgets

    SUBDIRS += plg_filterc_tmcl
    plg_filterc_tmcl.subdir=./plugins/filterc_tmcl
    plg_filterc_tmcl.depends = lib libqf3widgets

    SUBDIRS += plg_lights_pccsled
    plg_lights_pccsled.subdir=./plugins/lights_pccsled
    plg_lights_pccsled.depends = lib libqf3widgets

    SUBDIRS += plg_lights_b040laserbox
    plg_lights_b040laserbox.subdir=./plugins/lights_b040laserbox
    plg_lights_b040laserbox.depends = lib libqf3widgets

#    SUBDIRS += plg_b040_ffmcontrol
    plg_b040_ffmcontrol.subdir=./plugins/b040_ffmcontrol
    plg_b040_ffmcontrol.depends = lib libqf3widgets

#    SUBDIRS += plg_multicontrol_stage
    plg_multicontrol_stage.subdir=./plugins/multicontrol_stage
    plg_multicontrol_stage.depends = lib libqf3widgets

#    SUBDIRS += plg_alv_autocorrelator5000
    plg_alv_autocorrelator5000.subdir=./plugins/alv_autocorrelator5000
    plg_alv_autocorrelator5000.depends = lib libqf3widgets

#    SUBDIRS += plg_scanner2000_nicounter
    plg_scanner2000_nicounter.subdir=./plugins/scanner2000_nicounter
    plg_scanner2000_nicounter.depends = lib libqf3widgets
}


