TEMPLATE = lib
CONFIG += plugin

TARGET = fitfunctions_fcsdistribution
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfpfitfunctionsfcsdistribution.h \
           qffitfunctionfcsdistributionloggaussian.h

SOURCES += qfpfitfunctionsfcsdistribution.cpp\
           qffitfunctionfcsdistributionloggaussian.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.fitfunctions_fcsdistribution.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl

SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS

