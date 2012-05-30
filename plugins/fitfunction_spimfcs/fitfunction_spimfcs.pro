TEMPLATE = lib
CONFIG += plugin

TARGET = fitfunctions_spimfcs
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfpfitfunctionsspimfcs.h \
           qffitfunctionsspimfcsdiff.h \
    qffitfunctionsspimfcsadiff.h \
    qffitfunctionsspimfcsdiffe2.h

SOURCES += qfpfitfunctionsspimfcs.cpp\
           qffitfunctionsspimfcsdiff.cpp \
    qffitfunctionsspimfcsadiff.cpp \
    qffitfunctionsspimfcsdiffe2.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.fitfunctions_spimfcs.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl

SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS
