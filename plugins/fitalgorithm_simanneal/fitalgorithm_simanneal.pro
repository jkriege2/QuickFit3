TEMPLATE = lib
CONFIG += plugin

TARGET = fitalgorithm_simanneal
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins


include(../../libquickfitwidgets.pri)

# Input
HEADERS += fitalgorithm_simanneal.h \
           simannealconfig.h

SOURCES += fitalgorithm_simanneal.cpp \
           simannealconfig.cpp

FORMS +=

RESOURCES +=

TRANSLATIONS= ./translations/de.fitalgorithm_simanneal.ts

INCLUDEPATH += ../../lib/ \
               ../../extlib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/


QT += core
CONFIG += exceptions rtti stl

