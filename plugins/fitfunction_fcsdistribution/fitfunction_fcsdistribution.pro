TEMPLATE = lib
CONFIG += plugin

TARGET = fitfunctions_fcsdistribution
DEPENDPATH += ./

include(../plugins.pri)
include(../../extlibs/gsl.pri)

DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfpfitfunctionsfcsdistribution.h \
           qffitfunctionfcsdistributionloggaussian.h \
    qffitfunctionfcsdistributiongaussianD.h \
    qffitfunctionfcsdistributiongaussian.h \
    qffitfunctionfcsdistributiongaussianint.h \
    qffitfunctionfcsdistributionloggaussianint.h

SOURCES += qfpfitfunctionsfcsdistribution.cpp\
           qffitfunctionfcsdistributionloggaussian.cpp \
    qffitfunctionfcsdistributiongaussianD.cpp \
    qffitfunctionfcsdistributiongaussian.cpp \
    qffitfunctionfcsdistributiongaussianint.cpp \
    qffitfunctionfcsdistributionloggaussianint.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.fitfunctions_fcsdistribution.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl

SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS \
                        $$GPL3LICENSE


