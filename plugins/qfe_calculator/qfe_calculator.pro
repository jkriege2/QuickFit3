TEMPLATE = lib
CONFIG += plugin

TARGET = qfe_calculator
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)

include(../../libquickfitwidgets.pri)


DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfecalculator.h \
    qfecalculatordialog.h \
    ../../../../../LIB/trunk/jkmathparser.h

SOURCES += qfecalculator.cpp \
    qfecalculatordialog.cpp \
    ../../../../../LIB/trunk/jkmathparser.cpp

FORMS = \
    qfecalculatordialog.ui

RESOURCES += qfecalculator.qrc

TRANSLATIONS= ./translations/de.qfe_calculator.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl



