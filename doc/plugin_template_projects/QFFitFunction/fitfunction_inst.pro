TEMPLATE = lib
CONFIG += plugin

TARGET = target_id
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins
LIBS +=

# Input
HEADERS += fitfunction_inst.h \
           fitfunction_inst_f1.h

SOURCES += fitfunction_inst.cpp\
           fitfunction_inst_f1.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.target_id.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl
