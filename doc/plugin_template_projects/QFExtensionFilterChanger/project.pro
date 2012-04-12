TEMPLATE = lib
CONFIG += plugin

TARGET = filterc_test
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfextensiontestfilterchanger.h \
    ../interfaces/qfextensionfilterchanger.h

SOURCES += qfextensiontestfilterchanger.cpp

FORMS =

RESOURCES += qfextensiontestfilterchanger.qrc

TRANSLATIONS= ./translations/de.filterc_test.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl



