TEMPLATE = lib
CONFIG += plugin

TARGET = fitfunctions_fccs
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfpfitfunctionsfccs.h \
           qffitfunctionfccsnormaldiff3d.h

SOURCES += qfpfitfunctionsfccs.cpp\
           qffitfunctionfccsnormaldiff3d.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.fitfunctions_fccs.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl
