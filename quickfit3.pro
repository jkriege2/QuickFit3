TEMPLATE = subdirs
SUBDIRS += lib libqf3widgets application
application.depends = lib libqf3widgets

# also build the plugins
SUBDIRS += plg_table
plg_table.subdir = ./plugins/table
plg_table.depends = lib

SUBDIRS += plg_fcs
plg_fcs.subdir = ./plugins/fcs
plg_fcs.depends = lib

SUBDIRS += plg_fcsfit
plg_fcsfit.subdir = ./plugins/fcsfit
plg_fcsfit.depends = lib



