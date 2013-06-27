TEMPLATE = lib
CONFIG += plugin

TARGET = importers_simpletcspcimporter
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfsimpletcspcimporterplugin.h \
           qfsimpletcspcimporter.h

SOURCES += qfsimpletcspcimporterplugin.cpp\
           qfsimpletcspcimporter.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.importers_simpletcspcimporter.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl
