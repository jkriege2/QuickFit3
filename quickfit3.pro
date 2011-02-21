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

SUBDIRS += fcs_fitfuctions
fcs_fitfuctions.subdir=./plugins/fcs_fitfuctions

SUBDIRS += fitalgorithm_levmar
fitalgorithm_levmar.subdir=./plugins/fitalgorithm_levmar
fitalgorithm_levmar.depends = lib libqf3widgets

SUBDIRS += fitalgorithm_simanneal
fitalgorithm_simanneal.subdir=./plugins/fitalgorithm_simanneal
fitalgorithm_simanneal.depends = lib libqf3widgets

SUBDIRS += spimb040
spimb040.subdir=./plugins/spimb040
spimb040.depends = lib libqf3widgets

