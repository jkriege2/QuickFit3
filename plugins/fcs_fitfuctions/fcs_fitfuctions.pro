TEMPLATE = lib
CONFIG += plugin

TARGET = qffcsfitfuncs
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins
#DLLDESTDIR = $$DESTDIR
LIBS +=

# Input
HEADERS += fcs_fitfuctions.h

SOURCES += fcs_fitfuctions.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.qffcsfitfuncs.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl
