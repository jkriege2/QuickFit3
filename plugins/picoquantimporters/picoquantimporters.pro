TEMPLATE = lib
CONFIG += plugin

TARGET = importers_picoquant
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfpicoquantimporters.h \
           qftcspcreaderpicoquant.h \
    picoquant_t3r_tools.h \
    picoquant_pt3_tools.h \
    qftcspcreaderpicoquantpt3.h \
    picoquant_ptu_tools.h

SOURCES += qfpicoquantimporters.cpp\
           qftcspcreaderpicoquant.cpp \
    picoquant_t3r_tools.cpp \
    picoquant_pt3_tools.cpp \
    qftcspcreaderpicoquantpt3.cpp \
    picoquant_ptu_tools.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.importers_picoquant.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl
