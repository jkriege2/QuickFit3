TEMPLATE = lib
CONFIG += plugin
TARGET = fitalgorithm_levmar
DEPENDPATH += ./

include(../plugins.pri)
include(../../extlibs/levmar.pri)

DESTDIR = $$QFOUTPUT/plugins


# Input
HEADERS += fitalgorithm_levmar.h \
           levmarconfig.h

SOURCES += fitalgorithm_levmar.cpp \
           levmarconfig.cpp

FORMS +=

RESOURCES +=

TRANSLATIONS= ./translations/de.fitalgorithm_levmar.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/ \
               .

QT += core
CONFIG += exceptions rtti stl


