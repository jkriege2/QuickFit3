TEMPLATE = lib
CONFIG += plugin

TARGET = fitfunction_general
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfgeneralfitfunctionsplugin.h \
           qffitfunctiongeneralsigmoid.h

SOURCES += qfgeneralfitfunctionsplugin.cpp\
           qffitfunctiongeneralsigmoid.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.fitfunction_general.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl

SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS

