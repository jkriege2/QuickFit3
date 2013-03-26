TEMPLATE = lib
CONFIG += plugin

TARGET = qfe_resultstools
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)


include(../../libquickfitwidgets.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qferesultstools.h \
    qfertresultcalculator.h \
    ../base_classes/qfmathparserwidget.h \
    ../base_classes/qffunctionreferencetool.h \
    ../../../../../LIB/trunk/jkmathparser.h

SOURCES += qferesultstools.cpp \
    qfertresultcalculator.cpp \
    ../base_classes/qfmathparserwidget.cpp \
    ../base_classes/qffunctionreferencetool.cpp \
    ../../../../../LIB/trunk/jkmathparser.cpp

FORMS = \
    qfertresultcalculator.ui \
    ../base_classes/qfmathparserwidget.ui

RESOURCES += qferesultstools.qrc \
    ../../lib/lib.qrc

TRANSLATIONS= ./translations/de.qfe_resultstools.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl



