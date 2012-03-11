TEMPLATE = lib
CONFIG += plugin

TARGET = target_id
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += importer_inst.h \
           importer_inst_f1.h

SOURCES += importer_inst.cpp\
           importer_inst_f1.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.target_id.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl
