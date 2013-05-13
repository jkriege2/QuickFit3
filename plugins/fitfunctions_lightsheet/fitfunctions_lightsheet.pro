TEMPLATE = lib
CONFIG += plugin

TARGET = fitfunctions_lightsheet
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qffitfunctionslightsheet.h \
           qffitfunctionlightsheetgaussian.h \
           qffitfunctionlightsheetslit.h \
           qffitfunctionlightsheetgaussianvar.h

SOURCES += qffitfunctionslightsheet.cpp\
           qffitfunctionlightsheetgaussian.cpp \
           qffitfunctionlightsheetslit.cpp \
           qffitfunctionlightsheetgaussianvar.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.fitfunctions_lightsheet.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl

SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS


