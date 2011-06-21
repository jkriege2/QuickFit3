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

SUBDIRS += plg_fcsfit
plg_fcsfit.subdir = ./plugins/fcsfit
plg_fcsfit.depends = lib libqf3widgets

SUBDIRS += plg_fcs_fitfuctions
plg_fcs_fitfuctions.subdir=./plugins/fcs_fitfuctions

SUBDIRS += plg_fitalgorithm_levmar
plg_fitalgorithm_levmar.subdir=./plugins/fitalgorithm_levmar
plg_fitalgorithm_levmar.depends = lib libqf3widgets

SUBDIRS += plg_fitalgorithm_simanneal
plg_fitalgorithm_simanneal.subdir=./plugins/fitalgorithm_simanneal
plg_fitalgorithm_simanneal.depends = lib libqf3widgets

SUBDIRS += plg_spimb040
plg_spimb040.subdir=./plugins/spimb040
plg_spimb040.depends = lib libqf3widgets

SUBDIRS += plg_cam_testcamera
plg_cam_testcamera.subdir=./plugins/cam_testcamera
plg_cam_testcamera.depends = lib libqf3widgets

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

SUBDIRS += plg_stage_pi
plg_stage_pi.subdir=./plugins/stage_pi
plg_stage_pi.depends = lib libqf3widgets

dir_plugins.commands = ${MKDIR} ./output/plugins
dir_assets.commands = ${MKDIR} ./output/assets
dir_assetshelp.commands = ${MKDIR} ./output/assets/help
dir_assetsplugins.commands = ${MKDIR} ./output/assets/plugins
dir_assetspluginshelp.commands = ${MKDIR} ./output/assets/plugins/help
QMAKE_EXTRA_TARGETS += dir_plugins dir_assets dir_assetshelp dir_assetsplugins dir_assetspluginshelp

PRE_TARGETDEPS += dir_assets dir_assetshelp dir_assetsplugins dir_assetspluginshelp
