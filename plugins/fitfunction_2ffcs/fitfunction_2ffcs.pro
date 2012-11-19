TEMPLATE = lib
CONFIG += plugin

TARGET = fitfunctions_2ffcs
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfpfitfunctions2ffcs.h \
           qffitfunction2ffccsnormaldiff3d.h

SOURCES += qfpfitfunctions2ffcs.cpp\
           qffitfunction2ffccsnormaldiff3d.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.fitfunctions_2ffcs.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl

SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS

