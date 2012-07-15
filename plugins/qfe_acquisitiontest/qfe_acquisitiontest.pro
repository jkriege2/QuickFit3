TEMPLATE = lib
CONFIG += plugin

TARGET = qfe_acquisitiontest
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)
include(../../libquickfitwidgets.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfeacquisitiontest.h \
    qfeacquisitiontestdialog.h

SOURCES += qfeacquisitiontest.cpp \
    qfeacquisitiontestdialog.cpp

FORMS = \
    qfeacquisitiontestdialog.ui

RESOURCES += qfeacquisitiontest.qrc

TRANSLATIONS= ./translations/de.qfe_acquisitiontest.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl



