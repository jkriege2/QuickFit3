TEMPLATE = lib
CONFIG += plugin

TARGET = importers_picoquant
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfpicoquantimporters.h \
           qftcspcreaderpicoquant.h \
    picoquant_tools.h

SOURCES += qfpicoquantimporters.cpp\
           qftcspcreaderpicoquant.cpp \
    picoquant_tools.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.importers_picoquant.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl
