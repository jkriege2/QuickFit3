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
plg_cam_radhard2.depends = lib

