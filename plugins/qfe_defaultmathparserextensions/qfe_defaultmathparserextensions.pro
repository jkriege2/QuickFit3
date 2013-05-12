TEMPLATE = lib
CONFIG += plugin

TARGET = qfe_defaultmathparserextensions
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfedefaultmathparserextensions.h

SOURCES += qfedefaultmathparserextensions.cpp

FORMS =

RESOURCES += qfedefaultmathparserextensions.qrc

TRANSLATIONS= ./translations/de.qfe_defaultmathparserextensions.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl



