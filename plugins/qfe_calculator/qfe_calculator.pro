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
    ../base_classes/qffunctionreferencetool.h

SOURCES += qfecalculator.cpp \
    qfecalculatordialog.cpp \
    ../base_classes/qffunctionreferencetool.cpp

FORMS = \
    qfecalculatordialog.ui

RESOURCES += qfecalculator.qrc

TRANSLATIONS= ./translations/de.qfe_calculator.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/


HELPREF_FILES.files = ./help/parserreference/*.*
HELPREF_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/parserreference/

HELPREFPIC_FILES.files = ./help/parserreference/pic/*.*
HELPREFPIC_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/parserreference/pic/


INSTALLS += HELPREF_FILES HELPREFPIC_FILES


QT += gui xml svg
CONFIG += exceptions rtti stl



